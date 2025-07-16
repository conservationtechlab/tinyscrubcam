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
static const u1_t PROGMEM APPEUI[8]= { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }; //LEAVE AS IS
void os_getArtEui (u1_t* buf) { memcpy_P(buf, APPEUI, 8); }

// This should also be in little endian format, see above.
static const u1_t PROGMEM DEVEUI[8]= {FILL ME IN };
void os_getDevEui (u1_t* buf) { memcpy_P(buf, DEVEUI, 8); }

// This key should be in big endian format (or, since it is not really a
// number but a block of memory, endianness does not really apply). In
// practice, a key taken from the TTN console can be copied as-is.
static const u1_t PROGMEM APPKEY[16] = {FILL ME IN };
void os_getDevKey (u1_t* buf) {  memcpy_P(buf, APPKEY, 16); }

#define MAX_LENGTH 30

static uint8_t mydata[MAX_LENGTH]; // Enough for "Car/pictureXXX.jpg" or "Rhino/pictureXXX.jpg"
static osjob_t sendjob;

//Bool flags for OTAA communication
bool join = false;
bool first_join = true;

const lmic_pinmap lmic_pins = {
    .nss = 8,
    .rxtx = LMIC_UNUSED_PIN,
    .rst = 4,
    .dio = {3, 6, LMIC_UNUSED_PIN},
    .rxtx_rx_active = 0,
    .rssi_cal = 8,
    .spi_freq = 8000000,
};

// Flash storage struct for LoRaWAN session data including frame counters
struct SessionData {
  uint32_t netid;
  uint32_t devaddr;
  uint8_t nwkKey[16];
  uint8_t artKey[16];
  uint32_t seqnoUp; //Uplink frame counter
  uint32_t seqnoDn; // Downlink frame counter
  bool joined;
};

FlashStorage(session_flash, SessionData); //Flash storage for session data including frame incrementation
FlashStorage(devnonce_flash, uint16_t); //Flash storage for dev Nonce

static uint16_t devNonce = 0; // Stored DevNonce to avoid reuse

// Override LMIC to provide persistent DevNonce for OTAA join
extern "C" uint16_t LMIC_getDevNonce() {
  return devNonce;
}

// Print byte as two hex digits
void printHex2(unsigned v) {
    v &= 0xff;
    if (v < 16) Serial.print('0');
    Serial.print(v, HEX);
}

//print buffer in hex
void printHexBuffer(const uint8_t* buf, size_t len) {
    for (size_t i = 0; i < len; i++) {
        if (i != 0) Serial.print(" ");
        printHex2(buf[i]);
    }
    Serial.println();
}

// Attempt to restore saved session and frame counters to skip OTAA join
bool tryRestoreSession() {
  SessionData session = session_flash.read();

  if (session.joined && session.devaddr != 0) {
    Serial.println(F("Restoring session from flash"));

    LMIC_setSession(session.netid, session.devaddr, session.nwkKey, session.artKey);
    LMIC.seqnoUp = session.seqnoUp;
    LMIC.seqnoDn = session.seqnoDn;

    LMIC_setLinkCheckMode(0);
    LMIC_setDrTxpow(DR_SF7, 14);

    join = true;
    return true;
  }

  Serial.println(F("No valid session found or invalid data, starting OTAA"));
  join = false;
  return false;
}

void onEvent (ev_t ev) {
    Serial.print(os_getTime());
    Serial.print(": ");
    switch(ev) {
        case EV_JOINING:
            Serial.println(F("EV_JOINING"));
            break;
        case EV_JOINED:
            Serial.println(F("EV_JOINED"));
            join = true;
            first_join = false;
            {
              SessionData session;
              session.joined = true;
              LMIC_getSessionKeys(&session.netid, &session.devaddr, session.nwkKey, session.artKey);
              session.seqnoUp = LMIC.seqnoUp;
              session.seqnoDn = LMIC.seqnoDn;
              session_flash.write(session); // Save session and counters

              Serial.println(F("Session saved to flash"));
            }
            break;
        case EV_JOIN_FAILED:
            Serial.println(F("EV_JOIN_FAILED"));
            join = false;
            break;
        case EV_JOIN_TXCOMPLETE:
            Serial.println(F("EV_JOIN_TXCOMPLETE: no JoinAccept"));
            LMIC_reset(); //reset LMIC state for clean join

            devNonce++; //increment and store DevNonce value
            if (devNonce == 0) devNonce = 1;
            devnonce_flash.write(devNonce);
            Serial.print(F("DevNonce incremented and saved (retry): "));
            Serial.println(devNonce);
            //Attempt joining with new DevNonce
            LMIC_startJoining();
            break;
        case EV_TXCOMPLETE:
            Serial.println(F("EV_TXCOMPLETE"));

            if (LMIC.txrxFlags & TXRX_NACK) {
              Serial.println(F("No ACK received, session may be invalid"));
              
              LMIC_reset();

              devNonce++;
              if (devNonce == 0) devNonce = 1;
              devnonce_flash.write(devNonce);
              Serial.print(F("DevNonce incremented and saved (retry): "));
              Serial.println(devNonce);

              LMIC_startJoining();
            } else {
              Serial.println(F("TX success"));
              SessionData session = session_flash.read();
              if (session.joined) {
                session.seqnoUp = LMIC.seqnoUp;
                session.seqnoDn = LMIC.seqnoDn;
                session_flash.write(session);
                Serial.println(F("Frame counters updated in flash"));
              }
            }
            break;
        case EV_TXSTART:
            Serial.println(F("EV_TXSTART"));
            break;
        // Other events you want to keep printing...
        default:
            Serial.print(F("Unknown event: "));
            Serial.println((unsigned) ev);
            break;
    }
}

void do_send(osjob_t* j) {
    // Check if there is not a current TX/RX job running
    if (LMIC.opmode & OP_TXRXPEND) {
        Serial.println(F("OP_TXRXPEND, not sending"));
    } else {
        //See what payload is being sent with the Uplink
        size_t payloadLen = strlen((char*)mydata);
        Serial.print(F("Sending payload (hex): "));
        printHexBuffer(mydata, payloadLen);
        //Print Frame Count
        Serial.print(F("FCntUp before send: "));
        Serial.println(LMIC.seqnoUp);
        // Prepare upstream data transmission at the next possible time.
        LMIC_setTxData2(1, mydata, payloadLen, 0);
        Serial.println(F("Packet queued"));
    }
}


void setup() {
  delay(5000);
  Serial.begin(9600);
  delay(100);
  Serial1.begin(115200); //Must be same as ESP32_CAM
  
  join =false;

   // LMIC init
  os_init();
    // Reset the MAC state
  LMIC_reset();
    // Load devNonce from flash or initialize
  devNonce = devnonce_flash.read();
  if (devNonce == 0xFFFF || devNonce == 0) { // Flash erased or invalid
    devNonce = 1;
    devnonce_flash.write(devNonce);
  }

  Serial.print(F("Starting with DevNonce: ")); 
  Serial.println(devNonce);
  
  LMIC_selectSubBand(0);
  LMIC_setLinkCheckMode(0);
  LMIC_setDrTxpow(DR_SF7, 14);
  LMIC_startJoining();

  bool sessionRestored = tryRestoreSession();

  if (!sessionRestored && first_join) {
    Serial.println(F("No session restored, starting join immediately"));
    LMIC_startJoining();
    first_join = false;
  } else {
    Serial.println(F("Session restored, will verify validity after delay"));
  }
  
}

void loop() {
  os_runloop_once();

  if (Serial.available() && join) { //change to serial if running tests from computer or serial1 for integrated circuit
    String input = Serial.readStringUntil('\n'); //change to serial if running tests from computer or serial1 for integrated circuit
    input.trim();

    Serial.print("Serial Monitor Input: ");
    Serial.println(input);

    if (input.length() < MAX_LENGTH && input.startsWith("car")) {
      input.getBytes(mydata, input.length() + 1);
      do_send(&sendjob);
    } else {
      Serial.println("Serial: Invalid or too long");
      memset(mydata, 0, MAX_LENGTH);
    }
  }
}
