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
 *
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
#include <FlashStorage.h>

// This EUI must be in little-endian format, so least-significant-byte
// first. When copying an EUI from ttnctl output, this means to reverse
// the bytes. For TTN issued EUIs the last bytes should be 0xD5, 0xB3,
// 0x70.
static const u1_t PROGMEM APPEUI[8]= { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
void os_getArtEui (u1_t* buf) { memcpy_P(buf, APPEUI, 8); }

// This should also be in little endian format, see above.
static const u1_t PROGMEM DEVEUI[8]= {0xF2,  0xD7,  0x9D,  0xAA,  0xEC,  0xC5,  0x4B, 0xC5};
void os_getDevEui (u1_t* buf) { memcpy_P(buf, DEVEUI, 8); }

// This key should be in big endian format (or, since it is not really a
// number but a block of memory, endianness does not really apply). In
// practice, a key taken from the TTN console can be copied as-is.
static const u1_t PROGMEM APPKEY[16] = { 0xB5, 0xB6, 0xB8, 0xB3, 0xE3, 0x43, 0x34, 0x54, 0xBC, 0x87, 0xC3, 0x69, 0x61, 0xB1, 0x77, 0xD1 };
void os_getDevKey (u1_t* buf) {  memcpy_P(buf, APPKEY, 16); }

//#define MAX_LENGTH 30  // Enough for "Car/pictureXXX.jpg" or "Rhino/pictureXXX.jpg"

static uint8_t mydata[] = "Hello, World!"; // Buffer for outgoing payload mydata[MAX_LENGTH]
static osjob_t sendjob;

bool joined = false;

// Schedule TX every this many seconds (might become longer due to duty cycle limitations).
const unsigned TX_INTERVAL = 60; //180 for real test

// Pin mapping for Adafruit Feather M0 LoRa
const lmic_pinmap lmic_pins = {
    .nss = 8,
    .rxtx = LMIC_UNUSED_PIN,
    .rst = 4,
    .dio = {3, 6, LMIC_UNUSED_PIN},
    .rxtx_rx_active = 0,
    .rssi_cal = 8,              // LBT cal for the Adafruit Feather M0 LoRa, in dB
    .spi_freq = 8000000,
};

// Flash storage struct for LoRaWAN session data including frame counters
struct SessionData {
  uint32_t netid;
  uint32_t devaddr;
  uint8_t nwkKey[16];
  uint8_t artKey[16];
  uint32_t seqnoUp;  // Uplink frame counter
  uint32_t seqnoDn;  // Downlink frame counter
  bool valid;
};

FlashStorage(session_flash, SessionData);

// Flash storage for DevNonce
FlashStorage(devnonce_flash, uint16_t);
static uint16_t devNonce = 0;  // Stored DevNonce to avoid reuse

// Override LMIC to provide persistent DevNonce for OTAA join
extern "C" uint16_t LMIC_getDevNonce() {
  return devNonce;
}

// Utility: print byte as two hex digits
void printHex2(unsigned v) {
    v &= 0xff;
    if (v < 16)
        Serial.print('0');
    Serial.print(v, HEX);
}

// Utility: print buffer in hex
void printHexBuffer(const uint8_t* buf, size_t len) {
    for (size_t i = 0; i < len; i++) {
        if (i != 0) Serial.print(" ");
        printHex2(buf[i]);
    }
    Serial.println();
}

// Attempt to restore saved session and frame counters to skip OTAA join
void tryRestoreSession() {
  SessionData session = session_flash.read();

  if (session.valid) {
    Serial.println(F("Restoring session from flash"));

    LMIC_setSession(session.netid, session.devaddr, session.nwkKey, session.artKey);
    LMIC.seqnoUp = session.seqnoUp;
    LMIC.seqnoDn = session.seqnoDn;

    Serial.println(F("Session restored, skipping OTAA join"));
  } else {
    Serial.println(F("No valid session found, performing OTAA join"));
  }
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
            // Increment devNonce and save it

            break;
        case EV_JOINED:
            Serial.println(F("EV_JOINED")); 
            joined = true;
            {
              SessionData session;
              session.valid = true;
              LMIC_getSessionKeys(&session.netid, &session.devaddr, session.nwkKey, session.artKey);
              session.seqnoUp = LMIC.seqnoUp;
              session.seqnoDn = LMIC.seqnoDn;
              session_flash.write(session); // Save session and counters
              Serial.println(F("Session saved to flash"));
            }

            // Print session info for debug
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
                if (i != 0) Serial.print("-");
                printHex2(artKey[i]);
              }
              Serial.println("");
              Serial.print("NwkSKey: ");
              for (size_t i=0; i<sizeof(nwkKey); ++i) {
                if (i != 0) Serial.print("-");
                printHex2(nwkKey[i]);
              }
              Serial.println();
            }

            // Disable link check validation (can be enabled if needed)
            LMIC_setLinkCheckMode(0);
              // Schedule first transmission 5 seconds from now
    os_setTimedCallback(&sendjob, os_getTime() + sec2osticks(5), do_send);
            break;
        case EV_JOIN_FAILED:
            Serial.println(F("EV_JOIN_FAILED"));
            break;
            case EV_JOIN_TXCOMPLETE:
    Serial.println(F("EV_JOIN_TXCOMPLETE: no JoinAccept"));

    // Reset LMIC state for clean join
    LMIC_reset();
   
    // Increment and store DevNonce
    devNonce++;
    if (devNonce == 0) devNonce = 1; // avoid invalid 0
    devnonce_flash.write(devNonce);
    Serial.print(F("DevNonce incremented and saved (retry): "));
    Serial.println(devNonce);

    break;

        case EV_REJOIN_FAILED:
            Serial.println(F("EV_REJOIN_FAILED"));
            break;
        case EV_TXCOMPLETE:
            Serial.println(F("EV_TXCOMPLETE (includes waiting for RX windows)"));
            if (LMIC.txrxFlags & TXRX_ACK)
              Serial.println(F("Received ack"));
            if (LMIC.dataLen) {
              Serial.print(F("Received "));
              Serial.print(LMIC.dataLen);
              Serial.println(F(" bytes of payload"));
            }
            {
              SessionData session = session_flash.read();
              if (session.valid) {
                session.seqnoUp = LMIC.seqnoUp;
                session.seqnoDn = LMIC.seqnoDn;
                session_flash.write(session);
                Serial.println(F("Frame counters updated in flash"));
              }
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
            Serial.println(F("EV_RXCOMPLETE"));
            break;
        case EV_LINK_DEAD:
            Serial.println(F("EV_LINK_DEAD"));
            break;
        case EV_LINK_ALIVE:
            Serial.println(F("EV_LINK_ALIVE"));
            break;
        case EV_TXSTART:
            Serial.println(F("EV_TXSTART"));
            break;
        case EV_TXCANCELED:
            Serial.println(F("EV_TXCANCELED"));
            break;
        case EV_RXSTART:
            // do not print anything -- it wrecks timing
            break;
        default:
            Serial.print(F("Unknown event: "));
            Serial.println((unsigned) ev);
            break;
    }
}
/*
void do_send(osjob_t* j) {
    // Check if there is not a current TX/RX job running
    if (LMIC.opmode & OP_TXRXPEND) {
        Serial.println(F("OP_TXRXPEND, not sending"));
    } else {
        size_t payloadLen = strlen((char*)mydata);
        Serial.print(F("Sending payload (hex): "));
        printHexBuffer(mydata, payloadLen);

        // Prepare upstream data transmission at the next possible time.
        LMIC_setTxData2(1, mydata, payloadLen, 0);
        Serial.println(F("Packet queued"));
    }
}
*/
void do_send(osjob_t* j){
    // Check if there is not a current TX/RX job running
    if (LMIC.opmode & OP_TXRXPEND) {
        Serial.println(F("OP_TXRXPEND, not sending"));
    } else {
        // Prepare upstream data transmission at the next possible time.
        LMIC_setTxData2(1, mydata, sizeof(mydata)-1, 0);
        Serial.println(F("Packet queued"));
    }
    // Next TX is scheduled after TX_COMPLETE event.
}

void setup() {
    digitalWrite(13, HIGH);
    delay(1000); // wait for stabilization

    while (!Serial && millis() < 5000);  // wait up to 5 seconds for Serial
    Serial.begin(9600); 
    delay(100);
    Serial1.begin(115200);  // For ESP32-CAM comms

    Serial.println(F("Starting"));

    #ifdef VCC_ENABLE
    // For Pinoccio Scout boards
    pinMode(VCC_ENABLE, OUTPUT);
    digitalWrite(VCC_ENABLE, HIGH);
    delay(1000);
    #endif

    // LMIC init
    os_init();

    // Reset the MAC state
    LMIC_reset();

    // Load devNonce from flash or initialize
    devNonce = devnonce_flash.read();
    if (devNonce == 0xFFFF || devNonce == 0) { // Flash erased or invalid
        devNonce = 10;
        devnonce_flash.write(devNonce);
    }
    Serial.print(F("Starting with DevNonce: "));
    Serial.println(devNonce);
       //  Attempt to restore previous session
    tryRestoreSession();



    LMIC_setLinkCheckMode(0);
    LMIC_setDrTxpow(DR_SF7, 14);

  

  
    do_send(&sendjob);
}

void loop() {
      os_runloop_once();
/*
    while (Serial1.available()) { // Check if data is available to read
        String receivedString = Serial1.readStringUntil('\n'); // Read until newline character
        receivedString.trim();  // Remove anything trailing

        Serial.print("Received: ");
        Serial.println(receivedString); // Print the cleaned received line

        if (receivedString.length() < MAX_LENGTH && receivedString.startsWith("car")) {
            // Copy the string into the mydata buffer
            receivedString.getBytes(mydata, receivedString.length() + 1);

            do_send(&sendjob);  // Send after successful preparation
        } else {
            Serial.println("Out of bounds or invalid prefix");
            
            memset(mydata, 0, MAX_LENGTH); //Clear or reset mydata on error
        }
    }
    */
}
