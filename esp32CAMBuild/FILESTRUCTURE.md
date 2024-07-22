## featherM0LoRaCARDETECT

Different version of featherM0LoRa since Bob's implementation wouldn't work with rx/tx communication for what was detected, this instead just sends "caught" because if it didn't lora would wait the whole 3 minutes instead of attempting to send. I suspect it is either a faulty connection or a broken board.

## esp32_cameraCARDETECT

This file is the car detection for testing

## esp32camera

This is the current file for cougar detection

## esp32camLibs

Libraries to upload to esp32cam, make sure to replace the libraries in the arduino directory

## featherLibs

Libraries to upload to featherboard, make sure to replace the libraries in the arduino directory

## featherM0LoRa

Thie file is for uploading to the featherboard for lora communication
