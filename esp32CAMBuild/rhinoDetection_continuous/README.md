# Southwest model (bobcats, cougars, and coyotes)
<img width="1287" height="742" alt="Screenshot from 2025-07-22 11-17-25" src="https://github.com/user-attachments/assets/24bbcbc0-bbce-4639-bf3b-4606830bf39d" />



## Set-up
1) Program ESP32 CAM
   - Ensure microsd card is not inserted
   - Copy/paste the entire SageBrush_Detector_inferencing folder into your arduino libraries folder
   - If using FTDI programmer, ensure pins io0 and GND are connected (when programming ONLY)
   - Use board AI - Thinker after installing this board manager > https://adafruit.github.io/arduino-board-index/package_adafruit_index.json and downloading esp32 board library
2) Program Feather M0
   - Fill in ABP values and register device with your LoRa server
   - Download MCCI LoRaWAN LMIC library by IBM library
   - Ensure pins io0 and GND are connected (ONLY when running)
   - Ensure config is set to proper region (868 for Africa, 915 for US)
3) Refer to circuit diagram for wiring.
4) Insert microsd card to ESP32 CAM before running
   
## Project Description

SageBrush_Detector_inferencing is the Southwest model and works to detect cougars, coyotes, and bobcats.
You can go to past issues #22, #20 and #19 to see past progress that has led to this branch 
if you need assistance or more information on how to avoid past mistakes or get up to date on how things work.
Feather M0 has SubBand function which will allow it to automatically cycle through each SubBand until finding what
will allow it transmit. ESP32 CAM uses UART2 to communicate with Feather M0 by changing what pin 13 does after writing information onto SD card.

