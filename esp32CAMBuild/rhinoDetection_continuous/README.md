# Rhino Detection

![image](tinyscrubcam/esp32CAMBuild/images/tinyscrubcam.png)

## Set-up
1) Program esp32
   - Ensure microsd card is not inserted
   - Copy/paste the entire rhino model folder into your arduino libraries folder
   - If using FTDI programmer, ensure pins io0 and GND are connected
   - Use board AI - Thinker after installing this board manager > https://adafruit.github.io/arduino-board-index/package_adafruit_index.json and downloading esp32 board library
2) Program feather m0
   - Fill in ABP values and register device with your LoRa server
   - Download MCCI LoRaWAN LMIC library by IBM library
   - Ensure config is set to proper region (868 for Africa, 915 for US)
3) Refer to circuit diagram for wiring.
4) Insert microsd card to esp32cam before turning the solar charge controller to 'on' position
   
## Project Description

This project (folder `rhinoDetection_continuous`) can be downloaded onto the ESP32-CAM and
as is, it will run object detection with a rhino detector
for 10 seconds once triggered by a PIR sensor, which you
will know inference is happening by the red light
on the back of the ESP32-CAM turning on. If a rhino
is detected, it will send a high signal to pin 13, where
you can connect your feather M0 (not implemented to recieve
that signal yet) or attach an LED to see when it detects something. 
As is, it will save detection photos to the SD card, and turn the
red light off/pin 13 light off once it's finished the 10 seconds
of inference, until it is retriggered by the PIR sensor and it 
will run inference again. You can modify the inference time and
make it so that the photos do not save to the sd card.
