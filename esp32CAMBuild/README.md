# TinyScrubCAM

## Hardware Circuit Diagram -

![image](./images/ESP32CAMDIAGRAM.png)

## What is TinyScrubCAM?

TinyScrubCAM is a low power consumption device that has been through different iterations. It started as a RaspPi running motion detection sensors and cameras. This version consumed too much power, the whole point of TinyScrubCAM is to be a long lasting animal deterrant. Whenever a animal of interest is caught in our cameras point of view it should play a sound on a speaker in hopes of making the animal flee. To make TinyScrubCAM low powered, we switched over to using microcontroller powered by battery. We used hardware interrupts to essentially keep our microcontroller draining as little power as possible until the moment that the PIR sensor is triggered which will activate sound and capture an image to the SD card, which will be used to train more machine learning models on local species of animals.

## Clone the Repository

Begin by cloning the TinyScrubCam repository to your local machine:

```sh
git clone https://github.com/conservationtechlab/tinyscrubcam.git
```

## Set Up Instruction

1. Install Arduino IDE
2. Install Adafruit Featherboards

- Open Preferences or Settings
- Copy paste this --> https://adafruit.github.io/arduino-board-index/package_adafruit_index.json
- Open Boards Manager
- Search Adafruit Feather M0
- Install the board library

3. Install MCCI LoRaWAN LMIC library by IBM
4. At time of writing it is at version 4.1.1 (May 31, 2024)
5. In the MCCI config file make sure to change it to the correct region, it is US915 by default.
6. Setup complete!

## How to run?

This is the ESP32CAM version, utilzing an Adafruit Feather M0 Board to communicate over LoRaWAN, sending pings whenever an animal is detected.

1. Follow the Circuit Diagram to setup hardware
2. Upload the code in esp32CAMBuild/featherM0LoRa into the featherboard
3. Upload the code in esp32CAMBuild/esp32_camera to the ESP32CAM
4. To do this you must connect io0 and ground and connect 5V, GND, RX, TX to a USB adapter, or if you have the ESP32CAM uploading board it will be plug and play.
5. After uploading, make sure to click reset on the back of the ESP32CAM, to run the program
6. You are ready to use your camera trap!

## How does it work?

1. Your ESP32CAM is always on. It is running a loop, looking for a detection on the PIRSENSOR, by doing this instead of sleep like in the google coral version it makes more sensitive/reliable PIR detection, tradeoff being power consumption.
2. Once a detection is made, your ESP32CAM will capture 10secs worth of pictures, running analysis on each one.
3. If the model finds something, it will save that image, turn on the featherboard. Wait 10 seconds to give it time to power and play the speaker.
4. On enable, Featherboard will activate the speaker, then wait for serial data.
5. The ESP32CAM will then print to serial the detection it made e.g. "cougar" and the featherboard will then send this data over LoRa
6. If it doesn't find anything, it will go back to the loop where it is waiting for a detection

## Optimization/Issues/Future Implementation

Future implementations should look into putting the ESP32CAM into deep sleep, I tried but the interrupt didn't work properly and would often go straight back to sleep everytime it is triggered. GPIO pinout is very limited on the ESP32CAM, I didn't use the complete left side facing the camera, because those are data lines for the SDCARD, I think it might create some issues using those pins. Although I was able to use one without issue for the PIR Sensor. Since ESP32CAM only sends data to featherboard and not back, I had to implement the 3 minute window. Which means if the radio takes 4 minutes to send, it will just be shut off. Another implementation for this could be to make them speak back and forth to one another, I didn't do this because in my debugging I was limited because of the data lines, you can attempt to use the data lines and see if it affects anything. So this implementation would essentially be, ESP32CAM sends the serial data to featherboard, then wait for serial input, feather board will send it over lora, then send over serial that it is finished, then the ESP32CAM will be done and wait for another PIR detection, this way every single detection will be sent. Although, after the current fixes I made after writing this, I was able to get very strong, with better antennas, reliable transmission. It would only take from 30secs to a minute meaning the whole 2 minutes after it is completely doing nothing. Although the 3 minute window is the best implementation since it gives leeway for errors in the field, longer distances, etc.

So the two things I mentioned:

1. Deep Sleep for ESP32CAM instead of constant looping (look into Adafruit Sleepydog)
2. Bidirectional communication between boards

## Debugging

1. You might face an issue where the speaker doesn't play, make sure to buy a proper mp3 player and format it in FAT32. Some cheaply made mp3 players may come in the box broken
2. Adafruit Feather M0, may at times become unseen by a MACOS, unsure as to what this issue is, even when putting it into bootloader mode. What I found is usually when I just leave it alone for a bit it'll be findable again, restarting your computer, and also on a Linux computer it never has this issue.
3. Also the DEV EUI/APP KEY, are specific to my gateway, so make sure to change these values in your program.