/*******************************************************************************
 * Copyright (c) 2015 Thomas Telkamp and Matthijs Kooijman
 * Copyright (c) 2018 Terry Moore, MCCI
 *
 * Permission is hereby granted, free of charge, to anyone
 * obtaining a copy of this document and accompanying files,
 * to do whatever they want with them without any restriction,
 * including, but not limited to, copying, modification and redistribution.
 * NO WARRANTY OF ANY KIND IS PROVIDED.
 *
 * This example sends a valid LoRaWAN packet with payload "Hello,
 * world!", using frequency and encryption settings matching those of
 * the The Things Network. It's pre-configured for the Adafruit
 * Feather M0 LoRa.
 *
 * This uses OTAA (Over-the-air activation), where where a DevEUI and
 * application key is configured, which are used in an over-the-air
 * activation procedure where a DevAddr and session keys are
 * assigned/generated for use with all further communication.
 *
 * Note: LoRaWAN per sub-band duty-cycle limitation is enforced (1% in
 * g1, 0.1% in g2), but not the TTN fair usage policy (which is probably
 * violated by this sketch when left running for longer)!

 * To use this sketch, first register your application and device with
 * the things network, to set or generate an AppEUI, DevEUI and AppKey.
 * Multiple devices can use the same AppEUI, but each device has its own
 * DevEUI and AppKey.
 *
 * Do not forget to define the radio type correctly in
 * arduino-lmic/project_config/lmic_project_config.h or from your BOARDS.txt.
 *
 *******************************************************************************/

#include <lmic.h>
#include <hal/hal.h>
#include <SPI.h>
//#include <FlashStorage.h>
//
// For normal use, we require that you edit the sketch to replace FILLMEIN
// with values assigned by the TTN console. However, for regression tests,
// we want to be able to compile these scripts. The regression tests define
// COMPILE_REGRESSION_TEST, and in that case we define FILLMEIN to a non-
// working but innocuous value.
//



// This EUI must be in little-endian format, so least-significant-byte
// first. When copying an EUI from ttnctl output, this means to reverse
// the bytes. For TTN issued EUIs the last bytes should be 0xD5, 0xB3,
// 0x70.
static const u1_t PROGMEM APPEUI[8]= { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
void os_getArtEui (u1_t* buf) { memcpy_P(buf, APPEUI, 8);}

// This should also be in little endian format, see above.
static const u1_t PROGMEM DEVEUI[8]= { 0xF4, 0x69, 0xA3, 0x84, 0xA6, 0xDB, 0x0E, 0x59  };
void os_getDevEui (u1_t* buf) { memcpy_P(buf, DEVEUI, 8);}

// This key should be in big endian format (or, since it is not really a
// number but a block of memory, endianness does not really apply). In
// practice, a key taken from the TTN console can be copied as-is.
static const u1_t PROGMEM APPKEY[16] = {  0xDD, 0x1E, 0x23, 0x79, 0x45, 0x3E, 0x5A, 0xB9, 0x84, 0xF9, 0xC3, 0x78, 0x6F, 0xE1, 0x58, 0xDB };
void os_getDevKey (u1_t* buf) {  memcpy_P(buf, APPKEY, 16);}

// static uint8_t mydata[] = "Rhino"; //Send message of Rhino

#define MAX_LENGTH 50  // Enough for "pictureXXXX.jpg"

static uint8_t mydata[MAX_LENGTH]; //will now ensure that mydata can handle large number of char
static osjob_t sendjob;

// Schedule TX every this many seconds (might become longer due to duty
// cycle limitations).
const unsigned TX_INTERVAL = 200; //if doesn't work change back to 60

// Pin mapping
//
// Adafruit BSPs are not consistent -- m0 express defs ARDUINO_SAMD_FEATHER_M0,
// m0 defs ADAFRUIT_FEATHER_M0
//

// Pin mapping for Adafruit Feather M0 LoRa, etc.
// /!\ By default Adafruit Feather M0's pin 6 and DIO1 are not connected.
// Please ensure they are connected.
//#if defined(ARDUINO_SAMD_FEATHER_M0) || defined(ADAFRUIT_FEATHER_M0)
const lmic_pinmap lmic_pins = {
    .nss = 8,
    .rxtx = LMIC_UNUSED_PIN,
    .rst = 4,
    .dio = {3, 6, LMIC_UNUSED_PIN},
    .rxtx_rx_active = 0,
    .rssi_cal = 8,              // LBT cal for the Adafruit Feather M0 LoRa, in dB
    .spi_freq = 8000000,
};
//#endif
//const int eeprom_range_size = 10; // Number of slots for wear leveling

// Define a structure to store data
//struct FrameCountData {
   // int frame_count;
   // int write_index;
//};

// Create flash storage for the structure
//FlashStorage(myFlashStorage, FrameCountData);

//FrameCountData data; // Structure to hold frame_count and index

void printHex2(unsigned v) {
    v &= 0xff;
    if (v < 16)
        Serial.print('0');
    Serial.print(v, HEX);
}

void onEvent (ev_t ev) {
    Serial.print(os_getTime());
    Serial.print(": ");
    switch(ev) {
        case EV_SCAN_TIMEOUT:
            Serial.println(F("EV_SCAN_TIMEOUT"));
            break;
        case EV_BEACON_FOUND:
            Serial.println(F("EV_BEACON_FOUND"));
            break;
        case EV_BEACON_MISSED:
            Serial.println(F("EV_BEACON_MISSED"));
            break;
        case EV_BEACON_TRACKED:
            Serial.println(F("EV_BEACON_TRACKED"));
            break;
        case EV_JOINING:
            Serial.println(F("EV_JOINING"));
                  //blink if completed

            break;
        case EV_JOINED:
            Serial.println(F("EV_JOINED"));
            {
              u4_t netid = 0;
              devaddr_t devaddr = 0;
              u1_t nwkKey[16];
              u1_t artKey[16];
              LMIC_getSessionKeys(&netid, &devaddr, nwkKey, artKey);
              Serial.print("netid: ");
              Serial.println(netid, DEC);
              Serial.print("devaddr: ");
              Serial.println(devaddr, HEX);
              Serial.print("AppSKey: ");
              for (size_t i=0; i<sizeof(artKey); ++i) {
                if (i != 0)
                  Serial.print("-");
                printHex2(artKey[i]);
              }
              Serial.println("");
              Serial.print("NwkSKey: ");
              for (size_t i=0; i<sizeof(nwkKey); ++i) {
                      if (i != 0)
                              Serial.print("-");
                      printHex2(nwkKey[i]);
              }
              Serial.println();
            }
            // Disable link check validation (automatically enabled
            // during join, but because slow data rates change max TX
	    // size, we don't use it in this example.
            LMIC_setLinkCheckMode(0);
            break;
        /*
        || This event is defined but not used in the code. No
        || point in wasting codespace on it.
        ||
        || case EV_RFU1:
        ||     Serial.println(F("EV_RFU1"));
        ||     break;
        */
        case EV_JOIN_FAILED:
            Serial.println(F("EV_JOIN_FAILED"));
            break;
        case EV_REJOIN_FAILED:
            Serial.println(F("EV_REJOIN_FAILED"));
            break;
            break;
        case EV_TXCOMPLETE:
            Serial.println(F("EV_TXCOMPLETE (includes waiting for RX windows)"));
            if (LMIC.txrxFlags & TXRX_ACK)
              Serial.println(F("Received ack"));
            if (LMIC.dataLen) {
              Serial.println(F("Received "));
              Serial.println(LMIC.dataLen);
              Serial.println(F(" bytes of payload"));
            }
            // Schedule next transmission
            os_setTimedCallback(&sendjob, os_getTime()+sec2osticks(TX_INTERVAL), do_send);
            break;
        case EV_LOST_TSYNC:
            Serial.println(F("EV_LOST_TSYNC"));
            break;
        case EV_RESET:
            Serial.println(F("EV_RESET"));
            break;
        case EV_RXCOMPLETE:
            // data received in ping slot
            Serial.println(F("EV_RXCOMPLETE"));
            break;
        case EV_LINK_DEAD:
            Serial.println(F("EV_LINK_DEAD"));
            break;
        case EV_LINK_ALIVE:
            Serial.println(F("EV_LINK_ALIVE"));
            break;
        /*
        || This event is defined but not used in the code. No
        || point in wasting codespace on it.
        ||
        || case EV_SCAN_FOUND:
        ||    Serial.println(F("EV_SCAN_FOUND"));
        ||    break;
        */
        case EV_TXSTART:
            Serial.println(F("EV_TXSTART"));
            break;
        case EV_TXCANCELED:
            Serial.println(F("EV_TXCANCELED"));
            break;
        case EV_RXSTART:
            /* do not print anything -- it wrecks timing */
            break;
        case EV_JOIN_TXCOMPLETE:
            Serial.println(F("EV_JOIN_TXCOMPLETE: no JoinAccept"));
             
            break;

        default:
            Serial.print(F("Unknown event: "));
            Serial.println((unsigned) ev);
            break;
    }
}

void do_send(osjob_t* j){
  //  LMIC.seqnoUp = data.frame_count;
    // Check if there is not a current TX/RX job running
    if (LMIC.opmode & OP_TXRXPEND) {
        Serial.println(F("OP_TXRXPEND, not sending"));
    } else {
        // Prepare upstream data transmission at the next possible time.
        //LMIC_setTxData2(1, mydata, sizeof(mydata)-1, 0); //Original
        LMIC_setTxData2(1, mydata, strlen((char*)mydata), 0); //Doesn't cut down length of string length
        Serial.println(F("Packet queued"));
    }
  // data.frame_count++;
   // myFlashStorage.write(data);
    // Next TX is scheduled after TX_COMPLETE event.
}

void setup() {
digitalWrite(13, HIGH);
   delay(15000); //change back to 5000 if need be

 while (!Serial && millis() < 5000);  // wait up to 5 seconds for Serial
    Serial.begin(9600); 
    delay(100);
    Serial1.begin(115200);     // ALLOWS RX AND TX TO BE ABLE TO READ SERIAL1 DATA BY 
                                //  BEING ON THE SAME BAUD RATE AS ESP32 CAM
                                //
    Serial.println(F("Starting"));

   // data = myFlashStorage.read();

       // Check if flash is uninitialized (e.g., write_index out of range)
   // if (data.write_index < 0 || data.write_index >= eeprom_range_size) {
       // Serial.println("Flash storage uninitialized. Initializing...");
        //data.frame_count = 21;
        //myFlashStorage.write(data); // Save initial data
 //   }

    #ifdef VCC_ENABLE
    // For Pinoccio Scout boards
    pinMode(VCC_ENABLE, OUTPUT);
    digitalWrite(VCC_ENABLE, HIGH);
    delay(1000);
    #endif

    // LMIC init
    os_init();
    // Reset the MAC state. Session and pending data transfers will be discarded.
    LMIC_reset();


    LMIC_setLinkCheckMode(0);
    // TTN uses SF9 for its RX2 window.
    //LMIC.dn2Dr = DR_SF9;
    LMIC_setDrTxpow(DR_SF7,14);
    LMIC_selectSubBand(0);


   digitalWrite(13, LOW);
 

/*do  {
String receivedString = Serial1.readStringUntil('\n'); // Read until newline character
       Serial.print("Received: ");
         Serial.println(receivedString); // print the received line

          if (receivedString.length() < 15) {
              receivedString.getBytes(mydata, receivedString.length() + 1); // +1 to include the null terminator
                 do_send(&sendjob);
          } else {
              Serial.println("Out of bounds");
              receivedString = "ERR: Check Cam";
          }
     
} while (Serial1.available() > 0);

    while (1) {
      if (Serial1.available() > 0) { // Check if data is available to read
          String receivedString = Serial1.readStringUntil('\n'); // Read until newline character
          Serial.print("Received: ");
          Serial.println(receivedString); // print the received line
          
         digitalWrite(13, HIGH);
         delay(2000);
        digitalWrite(13, LOW);
        delay(2000);
       digitalWrite(13, HIGH);
        delay(2000);
        digitalWrite(13, LOW);

         if (receivedString.length() < 15) {
              receivedString.getBytes(mydata, receivedString.length() + 1); // +1 to include the null terminator
         } else {
             Serial.println("Out of bounds");
             receivedString = "ERR: Check Cam";
        }
        break; //original
     }
   break; //actually breaks out endless loop
}

    // Start job (sending automatically starts OTAA too)
   
    */

}

void loop() {
    os_runloop_once();

    while (Serial1.available()) { // Check if data is available to read
String receivedString = Serial1.readStringUntil('\n'); // Read until newline character
       Serial.print("Received: ");
         Serial.println(receivedString); // print the received line

          if (receivedString.length() < MAX_LENGTH) {
              receivedString.getBytes(mydata, receivedString.length() + 1); // +1 to include the null terminator
                 do_send(&sendjob);
          } else {
              Serial.println("Out of bounds");
              receivedString = "ERR: Check Cam";
          }
    }

}