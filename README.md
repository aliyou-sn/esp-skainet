# NOTE:
This branch is made for the model to suit a home automation board, with ESP32-S3-WROOM-1(N16R8) SoC, made to make users have voice control over there home appliances.
##Features of the hardware

1. Audio input either from an inbuild mic for an sd card
2. Audio output to a speaker or bluetooth classic
3. Battery charger
4. MQTT connectivity

# ESP-Skainet 

ESP-Skainet is Espressif's intelligent voice assistant, which currently supports the Wake Word Engine and Speech Commands Recognition.


# Overview

ESP-Skainet supports the development of wake word detection and speech commands recognition applications based around Espressif Systems' ESP32 series chip in the most convenient way. With ESP-Skainet, you can easily build up wake word detection and speech command recognition applications.

In general, the ESP-Skainet features will be supported, as shown below:

![overview](img/skainet_overview2.png)

## Input Voice Stream

The input audio stream can come from any way of providing voice, such as MIC, wav/pcm files in flash/SD Card.

## Wake Word Engine

Espressif wake word engine [WakeNet](https://docs.espressif.com/projects/esp-sr/en/latest/esp32/wake_word_engine/README.html) is specially designed to provide a high performance and low memory footprint wake word detection algorithm for users, which enables devices always wait for wake words, such as "Alexa",  “天猫精灵” (Tian Mao Jing Ling), and “小爱同学” (Xiao Ai Tong Xue).  


## Speech Commands Recognition

Espressif's speech command recognition model [MultiNet](https://docs.espressif.com/projects/esp-sr/en/latest/esp32/speech_command_recognition/README.html) is specially designed to provide a flexible offline speech command recognition model. With this model, you can easily add your own speech commands, eliminating the need to train model again.

Currently, Espressif **MultiNet** supports up to 200 Chinese or English speech commands, such as “打开空调” (Turn on the air conditioner) and “打开卧室灯” (Turn on the bedroom light). 

## Audio Front End

Espressif Audio Front-End [AFE](https://docs.espressif.com/projects/esp-sr/en/latest/esp32/audio_front_end/index.html) integrates AEC (Acoustic Echo Cancellation),  VAD (Voice Activity Detection),BSS (Blind Source Separation) and NS (Noise Suppression).    

Our two-mic Audio Front-End (AFE) have been qualified as a “Software Audio Front-End Solution” for [Amazon Alexa Built-in devices](https://developer.amazon.com/en-US/alexa/solution-providers/dev-kits#software-audio-front-end-dev-kits).
![afe](img/esp_afe.png)    
  
  




