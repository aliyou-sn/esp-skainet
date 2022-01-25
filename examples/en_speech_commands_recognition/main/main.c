/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_wn_iface.h"
#include "esp_wn_models.h"
#include "dl_lib_coefgetter_if.h"
#include "esp_afe_sr_iface.h"
#include "esp_afe_sr_models.h"
#include "esp_mn_iface.h"
#include "esp_mn_models.h"
#include "esp_board_init.h"
#include "driver/i2s.h"
#include "speech_commands_action.h"
#include "model_path.h"

int detect_flag = 0;
static esp_afe_sr_iface_t *afe_handle = NULL;
static int play_voice = -2;

void play_music(void *arg)
{
    while (1) {
        switch (play_voice) {
        case -2:
            vTaskDelay(10);
            break;
        case -1:
            wake_up_action();
            play_voice = -2;
            break;
        default:
            speech_commands_action(play_voice);
            play_voice = -2;
            break;
        }
    }
}

void feed_Task(void *arg)
{
    esp_afe_sr_data_t *afe_data = arg;
    int audio_chunksize = afe_handle->get_feed_chunksize(afe_data);
    int nch = afe_handle->get_channel_num(afe_data);
    int feed_channel = esp_get_feed_channel();
    int16_t *i2s_buff = malloc(audio_chunksize * sizeof(int16_t) * feed_channel);
    assert(i2s_buff);
    size_t bytes_read;

    while (1) {
        esp_get_feed_data(i2s_buff, audio_chunksize * sizeof(int16_t) * feed_channel);

        afe_handle->feed(afe_data, i2s_buff);
    }
    afe_handle->destroy(afe_data);
    vTaskDelete(NULL);
}

void detect_Task(void *arg)
{
    esp_afe_sr_data_t *afe_data = arg;
    int afe_chunksize = afe_handle->get_fetch_chunksize(afe_data);
    int nch = afe_handle->get_channel_num(afe_data);
    int16_t *buff = malloc(afe_chunksize * sizeof(int16_t));
    assert(buff);
    static const esp_mn_iface_t *multinet = &MULTINET_MODEL;
    model_iface_data_t *model_data = multinet->create(&MULTINET_COEFF, 5760);
    int mu_chunksize = multinet->get_samp_chunksize(model_data);
    int chunk_num = multinet->get_samp_chunknum(model_data);
    assert(mu_chunksize == afe_chunksize);
    printf("------------detect start------------\n");

    while (1) {
        int res = afe_handle->fetch(afe_data, buff);

        if (res == AFE_FETCH_WWE_DETECTED) {
            printf("wakeword detected\n");
            printf("-----------listening-----------\n");
        }
        if (res == AFE_FETCH_CHANNEL_VERIFIED) {
            play_voice = -1;
            detect_flag = 1;
            afe_handle->disable_wakenet(afe_data);
        } 

        if (detect_flag == 1) {
            esp_mn_state_t mn_state = multinet->detect(model_data, buff);

            if (mn_state == ESP_MN_STATE_DETECTING) {
                continue;
            }

            if (mn_state == ESP_MN_STATE_DETECTED) {
                esp_mn_results_t *mn_result = multinet->get_results(model_data);
                for (int i = 0; i < mn_result->num; i++) {
                    printf("TOP %d, command_id: %d, phrase_id: %d, prob: %f\n", 
                    i+1, mn_result->command_id[i], mn_result->phrase_id[i], mn_result->prob[i]);
                }
                printf("-----------listening-----------\n");
            }

            if (mn_state == ESP_MN_STATE_TIMEOUT) {
                afe_handle->enable_wakenet(afe_data);
                detect_flag = 0;
                printf("\n-----------awaits to be waken up-----------\n");
                continue;
            }
        }
    }
    afe_handle->destroy(afe_data);
    vTaskDelete(NULL);
}

void app_main()
{
#if defined CONFIG_MODEL_IN_SPIFFS
    srmodel_spiffs_init();
#endif
    ESP_ERROR_CHECK(esp_board_init(AUDIO_HAL_08K_SAMPLES, 1, 16));
    // ESP_ERROR_CHECK(esp_sdcard_init("/sdcard", 10));

#if defined CONFIG_ESP_LYRAT_MINI_V1_1_BOARD || defined CONFIG_ESP32_KORVO_V1_1_BOARD
    led_init();
#endif

#if CONFIG_IDF_TARGET_ESP32
    printf("This demo only support ESP32S3\n");
    return;
#else 
    afe_handle = &ESP_AFE_HANDLE;
#endif

    afe_config_t afe_config = AFE_CONFIG_DEFAULT();
#if defined CONFIG_ESP32_S3_BOX_BOARD || defined CONFIG_ESP32_S3_EYE_BOARD
    afe_config.aec_init = false;
#endif
    esp_afe_sr_data_t *afe_data = afe_handle->create_from_config(&afe_config);

    xTaskCreatePinnedToCore(&feed_Task, "feed", 8 * 1024, (void*)afe_data, 5, NULL, 0);
    xTaskCreatePinnedToCore(&detect_Task, "detect", 8 * 1024, (void*)afe_data, 5, NULL, 1);
#if defined  CONFIG_ESP32_S3_KORVO_V4_0_BOARD
    xTaskCreatePinnedToCore(&led_Task, "led", 2 * 1024, NULL, 5, NULL, 0);
#endif
#if defined  CONFIG_ESP32_S3_KORVO_1_V4_0_BOARD || CONFIG_ESP32_S3_KORVO_2_V3_0_BOARD || CONFIG_ESP32_KORVO_V1_1_BOARD
    xTaskCreatePinnedToCore(&play_music, "play", 4 * 1024, NULL, 5, NULL, 1);
#endif
}
