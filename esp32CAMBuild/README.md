rhinoDetection_continuous/ contains a wiring diagram, code, and model for detection
of rhinos and LoRa payloads sent with ABP communication. This is the version with
most current development and testing.

deterrent/ contains a wiring diagram, code, and power information for a version of
tinyscrubcam with a deterrent speaker and IR illimunation with similar features to the 
non deterrent version. This version is not as current and has some bugs, but could
contain some helpful information. 

# LoRa Keys
To obtain keys for LoRa transmission, we use Chirpstack as our LoRa server. It's hosted
at 10.24.17.64:8080. The rhinoDetection_continuous version is the device profile 
'TinyScrubCam ABP' and the device EUI that has run this code is '4043094992099020'. The
device profile contains the decoder information for the payload. One would need to create a
new device with the device profile mentioned above, and generate the keys needed, and input those
keys into the feather.ino code in the proper spots. Status of payloads can be seen once the device
is created and is sending packets in Chirpstack under 'LoRaWAN Frames' and 'Events'.
