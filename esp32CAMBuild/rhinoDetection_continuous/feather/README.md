# feather.ino

## description
This file is for sending LoRa messages using the Adafruit Feather M0 LoRa module. It is designed to communicate with The Things Network using preconfigured keys and device settings.

## How it works
   - Sets up parameters (`NWKSKEY`, `APPSKEY`, `DEVADDR`) for ABP and configures transmission data rate and power.
   - Sends the string `"Rhinos"` or a message received via serial communication to the LoRa network at intervals of 60 seconds (`TX_INTERVAL`).
   - Monitors and logs LoRa events such as joining, transmitting, and receiving.

### Key variables
   - `nss = 8`: Chip select for the LoRa module.
   - `rst = 4`: Reset pin.
   - `dio = {6, 5}`: Digital I/O pins for LoRa module communication.
- **Serial Communication:** Receives data via the serial port (`Serial1`) for dynamic message updates.
- **LoRa Events:** Prints detailed event logs to `Serial`.
- **Transmission Interval:** Configured to 60 seconds, respecting duty cycle limits.

For more details, refer to comments inside of `feather.ino`.
