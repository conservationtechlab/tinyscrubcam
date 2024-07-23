## esp32CamDeepSleep

Deep sleep seems to work properly, although there is one bug where the esp32 turns on and off with pir detection randomly, it still runs the ML model but I suspect it is an interrupt issue with setting PIR as the interrupt. Code logic looks perfectly fine

## esp32camera

This is the IR light implemented into one of the GPIOs, currently works but the ML model won't make any detections, in progress of debugging.

## esp32CamWORKING

This is the working version of esp32cam, doesn't have deep sleep or ir light powered.
