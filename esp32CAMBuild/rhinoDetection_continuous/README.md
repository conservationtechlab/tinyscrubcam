This script can be downloaded onto the ESP32-CAM and
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
