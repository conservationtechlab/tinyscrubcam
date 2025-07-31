# Southwest model (bobcats, cougars, and coyotes)
<img width="1208" height="699" alt="Screenshot from 2025-07-28 14-47-06" src="https://github.com/user-attachments/assets/0bb0eccb-447c-41d6-8b39-f8c0a43b4374" />




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

**How to build your own device**

First start by collecting the parts needed:

-Feather M0

-ESP32-CAM

-PIR sensor

-Antenna

-LIPO Batteries

-wires (both female end to end and male end to end)

-DFRobot solar manager

-6V solar panel

**Assembly (with indicator lights)**

-Begin by following the Set up instructions above to ensure you program both the ESP32-CAM and Feather M0 appropriately

-Next you will want to make any adjustments necessary to the compartment holding your device
  
   -If you intend on including indicator lights drill the holes for the LEDs or if you intend on enlarging a space or filing down edges now is the time
   
   -Once you are done with your adjustments **DO NOT PUT THE INSIDE HOLDER INSIDE THE DEVICE JUST YET**

-After you will need to solder 2 rails onto a solder board (4 if you intend to have indicator lights). Ensure this Board has space a quarter inch of space where no pins will obstruct its placement inside the device

   - 1 4x1 rail for possitive connections to power
   
   - 1 4x1 rail for negative connections to ground (5x1 if you want indicator lights)
   
   - ONLY FOR INDICATOR LIGHTS (3x1 rail for ESP32-CAM to Feather M0 UART connection)
   
   - ONLY FOR INDICATOR LIGHTS (3x1 rail for PIR sensor indicator light)

-ONLY FOR INDICATOR LIGHTS (best to keep soldering everything now if you intend to have indicator lights)

   -Insert LEDs into drilled holes on the face plate of the device
   
   -You will need to extend the cathode (- or short pin)  and the anode (+ or long pin) to either side of the face plate so it holds in place. This will also assist you in knowing which is possitve and negative for soldering
  
   -Next you will wrap a bare metal wire around the cathodes (- or short pins) of both LEDs. You can now solder this connection between the two LEDS where the wire and the cathode connect
   
   -After that you will want to wrap a short male end to end wire (colors help to remember which is what ie. green for the green LED, red for the red LED, and black for ground) onto the remaining anodes of each LED and one for ground.

-You may need to solder the wires or the tips of the wires of the battery you are using depending on the solar manager inputs (please check if you do before putting away the soldering kit)

   -If all connections are satisfactory and no more soldering needs to be done then continue

-Now is time for the hot glue! There is a identifiable square block that is directly to the right of the battery cartridge in the device holder and just below the Feather M0 stand

   -This is where the soldering board will need to be glued. Ensure the pins are not obstructing its placement and then glue

-We can now begin the internal connections. First you will want to put the insider holder within the box

   -Next ensure the Feather M0 has the antenna connected before installing it on the board and then you can follow along with the schematic to make the connections for the ESP32-CAM. Then start the connections for the Feather M0 and finally the PIR sensor.
   
   -The soldering board with the possitive and negative connections will also be available for your connections
   
   -IF YOU ARE USING INDICATOR LIGHTS (follow the connections with the schematic: TX on the Feather must go on the rail along with the pin 13 of the ESP32-CAM and one of the LEDs to the faceplate)

   -If YOU ARE USING INDICATOR LIGHTS (PIR sensor middle pin goes to rail, ESP32-CAM pin 12 and the last LED to the faceplate)

-After completing connections between the devices you can use the hot glue again to glue the battery pack within the battery cartridge

   -Ensure that the connection from the battery charger to the solar manager is established and then begin connecting the solar panel to the appropriate pins to the solar manager

-Once the solar manager is connected then hot glue inside the device

-Check over all connections with the schematic and you should be set to test your device
