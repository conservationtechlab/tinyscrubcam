This script can be downloaded onto the esp32 cam and allow
for testing while the serial monitor is still available, as
it does not require triggering from PIR to turn on or activate
the feather. It will run continuous inference and save images
that contain a detection of interest.

Future Implementations:
Ideally, this will be modified to only save 1 image if there
are multiple objects of interest in one frame. Currently, if
there are, say, 3 rhinos in 1 frame, it will save 3 continuous
photos. It should only save one but this is the first 
implementation.
