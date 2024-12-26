# esp32cam.ino
## Description
This script detects objects (eg. rhinos) when triggered by a PIR sensor.

## How it works
1. The PIR sensor (connected to pin `PIRSENSOR = 12`) detects motion, triggering a 10 second inference (`endTime = startTime + 10000`).
2. During inference:
   - The camera captures images and runs object detection using Edge Impulse.
   - Detected images are saved to the SD card if `saveToSDCard = 1`.
3. If a rhino is detected:
   - Pin `LORA = 3` sends a high signal to indicate detection.
   - An optional LED (pin `INFERENCELED = 33`) lights up during inference.
4. After inference, the system resets and waits for the next trigger.

### Key variables
- **Inference duration:** 10 seconds (modifiable in `loop()` function).
- **Image saving:** Enabled by default (`saveToSDCard = 1`).
- **Pin configuration:**
  - `PIRSENSOR = 12`: PIR sensor input.
  - `INFERENCELED = 33`: LED output during inference.
  - `LORA = 3`: High signal output on detection.

For more details, see the comments inside of `esp32cam.ino`.