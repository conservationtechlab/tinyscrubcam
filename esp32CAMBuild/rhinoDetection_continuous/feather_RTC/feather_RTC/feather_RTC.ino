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
#include <TimeLib.h>


#define GPS_UNIX_OFFSET 315964800UL
#define LEAP_SECONDS 18
#define UTC_OFFSET_SECONDS (-8 * 3600) // Pacific Standard Time

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
static const u1_t PROGMEM DEVEUI[8]= {  0xF4, 0x69, 0xA3, 0x84, 0xA6, 0xDB, 0x0E, 0x59  };
void os_getDevEui (u1_t* buf) { memcpy_P(buf, DEVEUI, 8);}

// This key should be in big endian format (or, since it is not really a
// number but a block of memory, endianness does not really apply). In
// practice, a key taken from the TTN console can be copied as-is.
static const u1_t PROGMEM APPKEY[16] = { 0xDD, 0x1E, 0x23, 0x79, 0x45, 0x3E, 0x5A, 0xB9, 0x84, 0xF9, 0xC3, 0x78, 0x6F, 0xE1, 0x58, 0xDB };
void os_getDevKey (u1_t* buf) {  memcpy_P(buf, APPKEY, 16);}

uint32_t gpsTime = 0;

static osjob_t sendjob;
const unsigned TX_INTERVAL = 60;

// US Pacific DST detection
bool isDST(uint32_t unixTime) {
    tmElements_t tm;
    breakTime(unixTime, tm);
    int month = tm.Month;
    int day = tm.Day;
    int weekday = tm.Wday; // Sunday = 1

    if (month < 3 || month > 11) return false;
    if (month > 3 && month < 11) return true;

    if (month == 3) return (day - weekday) >= 8;
    if (month == 11) return (day - weekday) < 1;

    return false;
}

void do_send(osjob_t* j) {
    if (LMIC.opmode & OP_TXRXPEND) {
        Serial.println(F("TX/RX pending, not sending"));
        return;
    }

    if (gpsTime == 0) {
        Serial.println(F("Requesting time with DeviceTimeReq..."));
        LMIC.frame[LMIC.dataLen++] = 0x0D;   // DeviceTimeReq MAC command
        LMIC_setTxData2(1, NULL, 0, 0);      // Not an application payload just requesting info
        return;
    }

    // Time is available — convert and format it
    uint32_t unixTime = gpsTime + GPS_UNIX_OFFSET - LEAP_SECONDS;
    if (isDST(unixTime)) unixTime += 3600;
    time_t localTime = unixTime + UTC_OFFSET_SECONDS;

    tmElements_t tm;
    breakTime(localTime, tm);

    char payload[96];
    snprintf(payload, sizeof(payload),
             "TIME: %02d/%02d/%04d %02d:%02d:%02d",
             tm.Month, tm.Day, tmYearToCalendar(tm.Year),
             tm.Hour, tm.Minute, tm.Second);

    Serial.print(F("Sending payload: "));
    Serial.println(payload);

    LMIC_setTxData2(1, (uint8_t*)payload, strlen(payload), 0);
}

// Feather M0 LoRa pin mapping
const lmic_pinmap lmic_pins = {
    .nss = 8,
    .rxtx = LMIC_UNUSED_PIN,
    .rst = 4,
    .dio = {3, 6, LMIC_UNUSED_PIN},
    .rxtx_rx_active = 0,
    .rssi_cal = 8,
    .spi_freq = 8000000,
};

void onEvent(ev_t ev) {
    Serial.print(os_getTime());
    Serial.print(F(": "));
    switch (ev) {
        case EV_JOINING:
            Serial.println(F("EV_JOINING"));
            break;
        case EV_JOINED:
            Serial.println(F("EV_JOINED"));
            LMIC_setLinkCheckMode(0);
            break;
        case EV_TXCOMPLETE:
            Serial.println(F("EV_TXCOMPLETE"));
            // Schedule next send
            os_setTimedCallback(&sendjob, os_getTime() + sec2osticks(TX_INTERVAL), do_send);
            break;
        case EV_RXCOMPLETE:
            Serial.println(F("EV_RXCOMPLETE"));
            if (LMIC.dataLen > 0 && LMIC.frame[LMIC.dataBeg] == 0x0E && LMIC.dataLen >= 5) {
                gpsTime =
                    ((uint32_t)LMIC.frame[LMIC.dataBeg + 1] << 24) |
                    ((uint32_t)LMIC.frame[LMIC.dataBeg + 2] << 16) |
                    ((uint32_t)LMIC.frame[LMIC.dataBeg + 3] << 8) |
                    ((uint32_t)LMIC.frame[LMIC.dataBeg + 4]);
                Serial.print(F("Received GPS Time: "));
                Serial.println(gpsTime);
                os_setTimedCallback(&sendjob, os_getTime() + sec2osticks(3), do_send);
            }
            break;
        case EV_JOIN_FAILED:
            Serial.println(F("EV_JOIN_FAILED"));
            break;
        case EV_TXSTART:
            Serial.println(F("EV_TXSTART"));
            break;
        default:
            Serial.print(F("Unknown event: "));
            Serial.println((unsigned)ev);
            break;
    }
}

void setup() {
    delay(5000);
    while (!Serial);
    Serial.begin(115200);
    Serial.println(F("Starting"));

    os_init();
    LMIC_reset();
    LMIC_setLinkCheckMode(0);
    LMIC_setDrTxpow(DR_SF7, 14);
    LMIC_selectSubBand(0); // for US915 sub-band 0

    do_send(&sendjob);
}

void loop() {
    os_runloop_once();
}