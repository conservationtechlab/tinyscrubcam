/*
 * Copyright 2015, Broadcom Corporation
 * All Rights Reserved.
 *
 * This is UNPUBLISHED PROPRIETARY SOURCE CODE of Broadcom Corporation;
 * the contents of this file may not be disclosed to third parties, copied
 * or duplicated in any form, in whole or in part, without the prior
 * written permission of Broadcom Corporation.
 */

/** @file
 *
 * HCI Control Protocol Definitions
 *
 * This file provides definitions for HCI Control Interface between an MCU
 * and hci_control application running on 20706. Please refer to the WICED Smart Ready
 * Software User Manual (WICED-Smart-Ready-SWUM100-R) for additional details on the
 * HCI UART control protocol.
 */
#ifndef __HCI_CONTROL_API_H
#define __HCI_CONTROL_API_H
    
/* Packets exchanged over the UART between MCU and hci_control application contain 5 byte header
* -------------------------------------------------------------------------------------------------------
* |  Packet Type      | Command Code          |    Group Code       |        Packet Length              |
* -------------------------------------------------------------------------------------------------------
* |HCI_WICED_PKT(0x19)|HCI_CONTROL_COMMAND_...|HCI_CONTROL_GROUP_...|length(low byte)| length(high byte)|
* -------------------------------------------------------------------------------------------------------
*/

#define HCI_EVENT_PKT                                       4
#define HCI_ACL_DATA_PKT                                    2
#define HCI_WICED_PKT                                       25

/*
 * Group codes
 */
#define HCI_CONTROL_GROUP_DEVICE                              0x00
#define HCI_CONTROL_GROUP_LE                                  0x01
#define HCI_CONTROL_GROUP_GATT                                0x02
#define HCI_CONTROL_GROUP_HF                                  0x03
#define HCI_CONTROL_GROUP_SPP                                 0x04
#define HCI_CONTROL_GROUP_AUDIO                               0x05
#define HCI_CONTROL_GROUP_HIDD                                0x06
#define HCI_CONTROL_GROUP_AVRC                                0x07
#define HCI_CONTROL_GROUP_TEST                                0x08
#define HCI_CONTROL_GROUP_AIO                                 0x09
#define HCI_CONTROL_GROUP_TIME                                0x0a
#define HCI_CONTROL_GROUP_ANCS                                0x0b
#define HCI_CONTROL_GROUP_ALERT                               0x0c
#define HCI_CONTROL_GROUP_LN                                  0x0d
#define HCI_CONTROL_GROUP_IAP2                                0x0e
#define HCI_CONTROL_GROUP_AG                                  0x0f
#define HCI_CONTROL_GROUP_MISC                                0xFF

#define HCI_CONTROL_GROUP(x) ((((x) >> 8)) & 0xff)

/*
 * General purpose commands
 */
#define HCI_CONTROL_COMMAND_RESET                           ( ( HCI_CONTROL_GROUP_DEVICE << 8 ) | 0x01 )    /* Restart controller */
#define HCI_CONTROL_COMMAND_TRACE_ENABLE                    ( ( HCI_CONTROL_GROUP_DEVICE << 8 ) | 0x02 )    /* Enable or disable WICED traces */
#define HCI_CONTROL_COMMAND_SET_LOCAL_BDA                   ( ( HCI_CONTROL_GROUP_DEVICE << 8 ) | 0x03 )    /* Set local device addrsss */
#define HCI_CONTROL_COMMAND_SET_BAUD_RATE                   ( ( HCI_CONTROL_GROUP_DEVICE << 8 ) | 0x04 )    /* Change UART baud rate */
#define HCI_CONTROL_COMMAND_PUSH_NVRAM_DATA                 ( ( HCI_CONTROL_GROUP_DEVICE << 8 ) | 0x05 )    /* Download previously saved NVRAM chunk */
#define HCI_CONTROL_COMMAND_DELETE_NVRAM_DATA               ( ( HCI_CONTROL_GROUP_DEVICE << 8 ) | 0x06 )    /* Delete NVRAM chunk currently stored in RAM */
#define HCI_CONTROL_COMMAND_INQUIRY                         ( ( HCI_CONTROL_GROUP_DEVICE << 8 ) | 0x07 )    /* Start/stop inquiry */
#define HCI_CONTROL_COMMAND_SET_VISIBILITY                  ( ( HCI_CONTROL_GROUP_DEVICE << 8 ) | 0x08 )    /* Set BR/EDR connectability and discoverability of the device */
#define HCI_CONTROL_COMMAND_USER_CONFIRMATION               ( ( HCI_CONTROL_GROUP_DEVICE << 8 ) | 0x09 )    /* User Confirmation during pairing, TRUE/FALSE passed as parameter */

/*
 * LE Commands
 * Define commands sent to the GAP/GATT implementation on 20706
 */
#define HCI_CONTROL_LE_COMMAND_SCAN                         ( ( HCI_CONTROL_GROUP_LE << 8 ) | 0x01 )    /* start scan */
#define HCI_CONTROL_LE_COMMAND_ADVERTISE                    ( ( HCI_CONTROL_GROUP_LE << 8 ) | 0x02 )    /* start advertisements */
#define HCI_CONTROL_LE_COMMAND_CONNECT                      ( ( HCI_CONTROL_GROUP_LE << 8 ) | 0x03 )    /* connect to peer */
#define HCI_CONTROL_LE_COMMAND_CANCEL_CONNECT               ( ( HCI_CONTROL_GROUP_LE << 8 ) | 0x04 )    /* cancel connect */
#define HCI_CONTROL_LE_COMMAND_DISCONNECT                   ( ( HCI_CONTROL_GROUP_LE << 8 ) | 0x05 )    /* disconnect */

/*
 * GATT Commands
 * Define commands to perform various GATT procedures
 */
#define HCI_CONTROL_GATT_COMMAND_DISCOVER_SERVICES          ( ( HCI_CONTROL_GROUP_GATT << 8 ) | 0x01 )    /* discover services */
#define HCI_CONTROL_GATT_COMMAND_DISCOVER_CHARACTERISTICS   ( ( HCI_CONTROL_GROUP_GATT << 8 ) | 0x02 )    /* discover characteristics */
#define HCI_CONTROL_GATT_COMMAND_DISCOVER_DESCRIPTORS       ( ( HCI_CONTROL_GROUP_GATT << 8 ) | 0x03 )    /* discover descriptors */
#define HCI_CONTROL_GATT_COMMAND_READ_REQUEST               ( ( HCI_CONTROL_GROUP_GATT << 8 ) | 0x04 )    /* send read request */
#define HCI_CONTROL_GATT_COMMAND_READ_RESPONSE              ( ( HCI_CONTROL_GROUP_GATT << 8 ) | 0x05 )    /* send read response */
#define HCI_CONTROL_GATT_COMMAND_WRITE_COMMAND              ( ( HCI_CONTROL_GROUP_GATT << 8 ) | 0x06 )    /* send write command */
#define HCI_CONTROL_GATT_COMMAND_WRITE_REQUEST              ( ( HCI_CONTROL_GROUP_GATT << 8 ) | 0x07 )    /* send write request */
#define HCI_CONTROL_GATT_COMMAND_WRITE_RESPONSE             ( ( HCI_CONTROL_GROUP_GATT << 8 ) | 0x08 )    /* send write response */
#define HCI_CONTROL_GATT_COMMAND_NOTIFY                     ( ( HCI_CONTROL_GROUP_GATT << 8 ) | 0x09 )    /* send notification */
#define HCI_CONTROL_GATT_COMMAND_INDICATE                   ( ( HCI_CONTROL_GROUP_GATT << 8 ) | 0x0a )    /* send indication */
#define HCI_CONTROL_GATT_COMMAND_INDICATE_CONFIRM           ( ( HCI_CONTROL_GROUP_GATT << 8 ) | 0x0b )    /* send indication confirmation */

/* 
 * Handsfree Commands  
 * Define commands sent to the HFP profile
 */
#define HCI_CONTROL_HF_COMMAND_CONNECT                      ( ( HCI_CONTROL_GROUP_HF << 8 ) | 0x01 )    /* establish connection to HF Audio Gateway */
#define HCI_CONTROL_HF_COMMAND_DISCONNECT                   ( ( HCI_CONTROL_GROUP_HF << 8 ) | 0x02 )    /* release HF connection */
#define HCI_CONTROL_HF_COMMAND_OPEN_AUDIO                   ( ( HCI_CONTROL_GROUP_HF << 8 ) | 0x03 )    /* create audio connection on existing service level connection */
#define HCI_CONTROL_HF_COMMAND_CLOSE_AUDIO                  ( ( HCI_CONTROL_GROUP_HF << 8 ) | 0x04 )    /* disconnect audio */

/* 
 * Sub commands to send various AT Commands
 */
#define HCI_CONTROL_HF_AT_COMMAND_BASE                      ( ( HCI_CONTROL_GROUP_HF << 8 ) | 0x20 )    /* send AT command and supporting data */
#define HCI_CONTROL_HF_AT_COMMAND_SPK                       0x00    /* Update speaker volume */
#define HCI_CONTROL_HF_AT_COMMAND_MIC                       0x01    /* Update microphone volume */
#define HCI_CONTROL_HF_AT_COMMAND_A                         0x02    /* Answer incoming call */
#define HCI_CONTROL_HF_AT_COMMAND_BINP                      0x03    /* Retrieve number from voice tag */
#define HCI_CONTROL_HF_AT_COMMAND_BVRA                      0x04    /* Enable/Disable voice recognition */
#define HCI_CONTROL_HF_AT_COMMAND_BLDN                      0x05    /* Last Number redial */
#define HCI_CONTROL_HF_AT_COMMAND_CHLD                      0x06    /* Call hold command */
#define HCI_CONTROL_HF_AT_COMMAND_CHUP                      0x07    /* Call hang up command */
#define HCI_CONTROL_HF_AT_COMMAND_CIND                      0x08    /* Read Indicator Status */
#define HCI_CONTROL_HF_AT_COMMAND_CNUM                      0x09    /* Retrieve Subscriber number */
#define HCI_CONTROL_HF_AT_COMMAND_D                         0x0A    /* Place a call using a number or memory dial */
#define HCI_CONTROL_HF_AT_COMMAND_NREC                      0x0B    /* Disable Noise reduction and echo canceling in AG */
#define HCI_CONTROL_HF_AT_COMMAND_VTS                       0x0C    /* Transmit DTMF tone */
#define HCI_CONTROL_HF_AT_COMMAND_BTRH                      0x0D    /* CCAP incoming call hold */
#define HCI_CONTROL_HF_AT_COMMAND_COPS                      0x0E    /* Query operator selection */
#define HCI_CONTROL_HF_AT_COMMAND_CMEE                      0x0F    /* Enable/disable extended AG result codes */
#define HCI_CONTROL_HF_AT_COMMAND_CLCC                      0x10    /* Query list of current calls in AG */
#define HCI_CONTROL_HF_AT_COMMAND_BIA                       0x11    /* Activate/Deactivate indicators */
#define HCI_CONTROL_HF_AT_COMMAND_BIEV                      0x12    /* Send HF indicator value to peer */
#define HCI_CONTROL_HF_AT_COMMAND_UNAT                      0x13    /* Transmit AT command not in the spec  */
#define HCI_CONTROL_HF_AT_COMMAND_MAX                       0x13    /* For command validation */

/*
 * Serial Port Profile Commands
 * Define commands sent to the SPP profile
 */
#define HCI_CONTROL_SPP_COMMAND_CONNECT                     ( ( HCI_CONTROL_GROUP_SPP << 8 ) | 0x01 )    /* establish connection to SPP server */
#define HCI_CONTROL_SPP_COMMAND_DISCONNECT                  ( ( HCI_CONTROL_GROUP_SPP << 8 ) | 0x02 )    /* release SPP connection */
#define HCI_CONTROL_SPP_COMMAND_DATA                        ( ( HCI_CONTROL_GROUP_SPP << 8 ) | 0x03 )    /* send data */

/*
* Audio Profile Commands
* Define commands sent to the Audio profile
*/
#define HCI_CONTROL_AUDIO_COMMAND_CONNECT                   ( ( HCI_CONTROL_GROUP_AUDIO << 8 ) | 0x01 )    /* Audio connect to sink */
#define HCI_CONTROL_AUDIO_COMMAND_DISCONNECT                ( ( HCI_CONTROL_GROUP_AUDIO << 8 ) | 0x02 )    /* Audio disconnect  */
#define HCI_CONTROL_AUDIO_START                             ( ( HCI_CONTROL_GROUP_AUDIO << 8 ) | 0x03 )    /* start audio with speciifc sample rate/mode */
#define HCI_CONTROL_AUDIO_STOP                              ( ( HCI_CONTROL_GROUP_AUDIO << 8 ) | 0x04 )    /* stop audio */
#define HCI_CONTROL_AUDIO_PACKET_COUNT                      ( ( HCI_CONTROL_GROUP_AUDIO << 8 ) | 0x05 )    /* debug packet counter sent from host */

/*
* AVRC Controller Profile Commands
* Define commands sent to the AVRC profile
*/
#define HCI_CONTROL_AVRC_COMMAND_INITIATE_CONNECTION        ( ( HCI_CONTROL_GROUP_AVRC << 8 ) | 0x01 )    /* Initiate a connection to the peer. */
#define HCI_CONTROL_AVRC_COMMAND_DISCONNECT                 ( ( HCI_CONTROL_GROUP_AVRC << 8 ) | 0x02 )    /* Disconnect a connection to the peer. */
#define HCI_CONTROL_AVRC_COMMAND_PLAY                       ( ( HCI_CONTROL_GROUP_AVRC << 8 ) | 0x03 )    /* Passthrough Play Command */
#define HCI_CONTROL_AVRC_COMMAND_PAUSE                      ( ( HCI_CONTROL_GROUP_AVRC << 8 ) | 0x04 )    /* Passthrough Pause Command */
#define HCI_CONTROL_AVRC_COMMAND_FASTFWD                    ( ( HCI_CONTROL_GROUP_AVRC << 8 ) | 0x05 )    /* Passthrough FFWD Command */
#define HCI_CONTROL_AVRC_COMMAND_REWIND                     ( ( HCI_CONTROL_GROUP_AVRC << 8 ) | 0x06 )    /* Passthrough Rewind Command */
#define HCI_CONTROL_AVRC_COMMAND_NEXT_TRACK                 ( ( HCI_CONTROL_GROUP_AVRC << 8 ) | 0x07 )    /* Passthrough Next Command */
#define HCI_CONTROL_AVRC_COMMAND_PREV_TRACK                 ( ( HCI_CONTROL_GROUP_AVRC << 8 ) | 0x08 )    /* Passthrough Prev Command */
#define HCI_CONTROL_AVRC_COMMAND_TRACK_INFO                 ( ( HCI_CONTROL_GROUP_AVRC << 8 ) | 0x09 )    /* Get Track Metadata */
#define HCI_CONTROL_AVRC_COMMAND_EQ_ENABLE                  ( ( HCI_CONTROL_GROUP_AVRC << 8 ) | 0x0A )    /* Enable Equalizer */
#define HCI_CONTROL_AVRC_COMMAND_REPEAT_MODE                ( ( HCI_CONTROL_GROUP_AVRC << 8 ) | 0x0B )    /* Set Repeat Mode */
#define HCI_CONTROL_AVRC_COMMAND_SHUFFLE_MODE               ( ( HCI_CONTROL_GROUP_AVRC << 8 ) | 0x0C )    /* Set Shuffle Mode */
#define HCI_CONTROL_AVRC_COMMAND_SCAN_ENABLE                ( ( HCI_CONTROL_GROUP_AVRC << 8 ) | 0x0D )    /* Enable Scan */

#define HCI_CONTROL_AVRC_COMMAND_VOLUME_LEVEL               ( ( HCI_CONTROL_GROUP_AVRC << 8 ) | 0x96 )    /* Register for notifications (PTS) */
#define HCI_CONTROL_AVRC_COMMAND_VOLUME_UP                  ( ( HCI_CONTROL_GROUP_AVRC << 8 ) | 0x97 )    /* Register for notifications (PTS) */
#define HCI_CONTROL_AVRC_COMMAND_VOLUME_DOWN                ( ( HCI_CONTROL_GROUP_AVRC << 8 ) | 0x98 )    /* Register for notifications (PTS) */
#define HCI_CONTROL_AVRC_COMMAND_REGISTER_NOTIFICATION      ( ( HCI_CONTROL_GROUP_AVRC << 8 ) | 0x99 )    /* Register for notifications (PTS) */

/*
 * HID Device Commands
 */
#define HCI_CONTROL_HID_COMMAND_ACCEPT_PAIRING              ( ( HCI_CONTROL_GROUP_HIDD << 8 ) | 0x01 )     /* Set device discoverable/connectable to accept pairing */
#define HCI_CONTROL_HID_COMMAND_SEND_REPORT                 ( ( HCI_CONTROL_GROUP_HIDD << 8 ) | 0x02 )     /* Send HID report */
#define HCI_CONTROL_HID_COMMAND_PUSH_PAIRING_HOST_INFO      ( ( HCI_CONTROL_GROUP_HIDD << 8 ) | 0x03 )     /* Paired host address and link keys */
#define HCI_CONTROL_HID_COMMAND_CONNECT                     ( ( HCI_CONTROL_GROUP_HIDD << 8 ) | 0x04 )     /* Connect to previously paired host */

/*
* Test Commands
*/
#define HCI_CONTROL_TEST_COMMAND_LE_RECEIVER                ( ( HCI_CONTROL_GROUP_TEST << 8 ) | 0x01 )     /* Start LE Receiver Test */
#define HCI_CONTROL_TEST_COMMAND_LE_TRASMITTER              ( ( HCI_CONTROL_GROUP_TEST << 8 ) | 0x02 )     /* Start LE Tramsmitter Test */
#define HCI_CONTROL_TEST_COMMAND_LE_TEST_END                ( ( HCI_CONTROL_GROUP_TEST << 8 ) | 0x03 )     /* End LE Test */
#define HCI_CONTROL_TEST_COMMAND_CONTINUOUS_TRANSMIT        ( ( HCI_CONTROL_GROUP_TEST << 8 ) | 0x04 )     /* Start Continuous Transmit */
#define HCI_CONTROL_TEST_COMMAND_RECEIVE_ONLY               ( ( HCI_CONTROL_GROUP_TEST << 8 ) | 0x05 )     /* Start Receiver Test */
#define HCI_CONTROL_TEST_COMMAND_DISABLE_POWER_CONTROL      ( ( HCI_CONTROL_GROUP_TEST << 8 ) | 0x06 )     /* Turn off power control for the Link */
#define HCI_CONTROL_TEST_COMMAND_SET_TX_POWER               ( ( HCI_CONTROL_GROUP_TEST << 8 ) | 0x07 )     /* Set Tx power for the Link */
#define HCI_CONTROL_TEST_COMMAND_INCR_DECR_PEER_POWER       ( ( HCI_CONTROL_GROUP_TEST << 8 ) | 0x08 )     /* Request peer to incr/decr power for the Link */

/*
* Automation IO Commands
*/
#define HCI_CONTROL_AIO_COMMAND_DIGITAL_IN                  ( ( HCI_CONTROL_GROUP_AIO << 8 ) | 0x01 )      /* Digital input */
#define HCI_CONTROL_AIO_COMMAND_ANALOG_IN                   ( ( HCI_CONTROL_GROUP_AIO << 8 ) | 0x02 )      /* Analog input */
#define HCI_CONTROL_AIO_COMMAND_CONNECT                     ( ( HCI_CONTROL_GROUP_AIO << 8 ) | 0x03 )      /* Connect to server */
#define HCI_CONTROL_AIO_COMMAND_READ                        ( ( HCI_CONTROL_GROUP_AIO << 8 ) | 0x04 )      /* Read value */
#define HCI_CONTROL_AIO_COMMAND_WRITE                       ( ( HCI_CONTROL_GROUP_AIO << 8 ) | 0x05 )      /* Write value */
#define HCI_CONTROL_AIO_COMMAND_WRITE_NO_RSP                ( ( HCI_CONTROL_GROUP_AIO << 8 ) | 0x06 )      /* Write with no response */
#define HCI_CONTROL_AIO_COMMAND_SET_CLIENT_CONFIG           ( ( HCI_CONTROL_GROUP_AIO << 8 ) | 0x07 )      /* Set client configuration */
#define HCI_CONTROL_AIO_COMMAND_SET_VALUE_TRIGGER           ( ( HCI_CONTROL_GROUP_AIO << 8 ) | 0x08 )      /* Set value trigger */
#define HCI_CONTROL_AIO_COMMAND_SET_TIME_TRIGGER            ( ( HCI_CONTROL_GROUP_AIO << 8 ) | 0x09 )      /* Set time trigger */
#define HCI_CONTROL_AIO_COMMAND_SET_USER_DESCRIPTION        ( ( HCI_CONTROL_GROUP_AIO << 8 ) | 0x0a )      /* Set user description */
#define HCI_CONTROL_AIO_COMMAND_DISCONNECT                  ( ( HCI_CONTROL_GROUP_AIO << 8 ) | 0x0b )      /* Disconnect from server */

/*
 * Define ANCS commands
 */
#define HCI_CONTROL_ANCS_COMMAND_ACTION                     ( ( HCI_CONTROL_GROUP_ANCS << 8 ) | 0x01 )      /* ANCS notification */

/*
 * IAP2 Commands
 * Define commands sent to the IAP2 implementation
 */
#define HCI_CONTROL_IAP2_COMMAND_CONNECT                     ( ( HCI_CONTROL_GROUP_IAP2 << 8 ) | 0x01 )    /* establish connection to SPP server */
#define HCI_CONTROL_IAP2_COMMAND_DISCONNECT                  ( ( HCI_CONTROL_GROUP_IAP2 << 8 ) | 0x02 )    /* release SPP connection */
#define HCI_CONTROL_IAP2_COMMAND_DATA                        ( ( HCI_CONTROL_GROUP_IAP2 << 8 ) | 0x03 )    /* send data */

/*
 * Handsfree AG Commands
 * Define commands sent to the HF-AG profile
 */
#define HCI_CONTROL_AG_COMMAND_CONNECT                      ( ( HCI_CONTROL_GROUP_AG << 8 ) | 0x01 )    /* establish connection to HF Device */
#define HCI_CONTROL_AG_COMMAND_DISCONNECT                   ( ( HCI_CONTROL_GROUP_AG << 8 ) | 0x02 )    /* release HF connection */
#define HCI_CONTROL_AG_COMMAND_OPEN_AUDIO                   ( ( HCI_CONTROL_GROUP_AG << 8 ) | 0x03 )    /* create audio connection on existing service level connection */
#define HCI_CONTROL_AG_COMMAND_CLOSE_AUDIO                  ( ( HCI_CONTROL_GROUP_AG << 8 ) | 0x04 )    /* disconnect audio */

/*
 * Location and Navigation commands
 */
#define HCI_CONTROL_LN_COMMAND_SET_DATA_SOURCE              ( ( HCI_CONTROL_GROUP_LN << 8 ) | 0x01 )    /* Location/navigation data from host or board */
#define HCI_CONTROL_LN_COMMAND_LN_FEATURE                   ( ( HCI_CONTROL_GROUP_LN << 8 ) | 0x02 )    /* Supported features of server */
#define HCI_CONTROL_LN_COMMAND_LOCATION_SPEED               ( ( HCI_CONTROL_GROUP_LN << 8 ) | 0x03 )    /* Location and speed data */
#define HCI_CONTROL_LN_COMMAND_POSITION_QUALITY_CHANGED     ( ( HCI_CONTROL_GROUP_LN << 8 ) | 0x04 )    /* Position quality data update */
#define HCI_CONTROL_LN_COMMAND_LN_CONTROL_RSP               ( ( HCI_CONTROL_GROUP_LN << 8 ) | 0x05 )    /* Response to LN control point request */
#define HCI_CONTROL_LN_COMMAND_NAVIGATION                   ( ( HCI_CONTROL_GROUP_LN << 8 ) | 0x06 )    /* Navigation data */

/*
 * Miscellaneous commands
 */
#define HCI_CONTROL_MISC_COMMAND_PING                       ( ( HCI_CONTROL_GROUP_MISC << 8 ) | 0x01 )    /* Ping controller */

/*
 * Define general events that controller can send
 */
#define HCI_CONTROL_EVENT_COMMAND_STATUS                    ( ( HCI_CONTROL_GROUP_DEVICE << 8 ) | 0x01 )    /* Command status event for the requested operation */
#define HCI_CONTROL_EVENT_WICED_TRACE                       ( ( HCI_CONTROL_GROUP_DEVICE << 8 ) | 0x02 )    /* WICED trace packet */
#define HCI_CONTROL_EVENT_HCI_TRACE                         ( ( HCI_CONTROL_GROUP_DEVICE << 8 ) | 0x03 )    /* Bluetooth protocol trace */
#define HCI_CONTROL_EVENT_NVRAM_DATA                        ( ( HCI_CONTROL_GROUP_DEVICE << 8 ) | 0x04 )    /* Request to MCU to save NVRAM chunk */
#define HCI_CONTROL_EVENT_DEVICE_STARTED                    ( ( HCI_CONTROL_GROUP_DEVICE << 8 ) | 0x05 )    /* Device completed power up initialization */
#define HCI_CONTROL_EVENT_INQUIRY_RESULT                    ( ( HCI_CONTROL_GROUP_DEVICE << 8 ) | 0x06 )    /* Inquiry result */
#define HCI_CONTROL_EVENT_INQUIRY_COMPLETE                  ( ( HCI_CONTROL_GROUP_DEVICE << 8 ) | 0x07 )    /* Inquiry completed event */
#define HCI_CONTROL_EVENT_PAIRING_COMPLETE                  ( ( HCI_CONTROL_GROUP_DEVICE << 8 ) | 0x08 )    /* Pairing Completed */
#define HCI_CONTROL_EVENT_ENCRYPTION_CHANGED                ( ( HCI_CONTROL_GROUP_DEVICE << 8 ) | 0x09 )    /* Encryption changed event */
#define HCI_CONTROL_EVENT_CONNECTED_DEVICE_NAME             ( ( HCI_CONTROL_GROUP_DEVICE << 8 ) | 0x0A )    /* Device name event */
#define HCI_CONTROL_EVENT_USER_CONFIRMATION                 ( ( HCI_CONTROL_GROUP_DEVICE << 8 ) | 0x0B )    /* User Confirmation during pairing */

/* 
 * Define events from the HFP profile
 */
#define HCI_CONTROL_HF_EVENT_OPEN                           ( ( HCI_CONTROL_GROUP_HF << 8 ) | 0x01 )    /* HS connection opened or connection attempt failed  */
#define HCI_CONTROL_HF_EVENT_CLOSE                          ( ( HCI_CONTROL_GROUP_HF << 8 ) | 0x02 )    /* HS connection closed */
#define HCI_CONTROL_HF_EVENT_CONNECTED                      ( ( HCI_CONTROL_GROUP_HF << 8 ) | 0x03 )    /* HS Service Level Connection is UP */
#define HCI_CONTROL_HF_EVENT_AUDIO_OPEN                     ( ( HCI_CONTROL_GROUP_HF << 8 ) | 0x04 )    /* Audio connection open */
#define HCI_CONTROL_HF_EVENT_AUDIO_CLOSE                    ( ( HCI_CONTROL_GROUP_HF << 8 ) | 0x05 )    /* Audio connection closed */

/*
* Subcommands AT resoponses defined with AT Commands
*/
#define HCI_CONTROL_HF_AT_EVENT_BASE                        ( ( HCI_CONTROL_GROUP_HF << 8 ) | 0x20 )
#define HCI_CONTROL_HF_AT_EVENT_OK                          0x00    /* OK response received to previous AT command */
#define HCI_CONTROL_HF_AT_EVENT_ERROR                       0x01    /* ERROR response received */
#define HCI_CONTROL_HF_AT_EVENT_CMEE                        0x02    /* Extended error codes response */
#define HCI_CONTROL_HF_AT_EVENT_RING                        0x03    /* RING indicator */
#define HCI_CONTROL_HF_AT_EVENT_VGS                         0x04
#define HCI_CONTROL_HF_AT_EVENT_VGM                         0x05
#define HCI_CONTROL_HF_AT_EVENT_CCWA                        0x06
#define HCI_CONTROL_HF_AT_EVENT_CHLD                        0x07
#define HCI_CONTROL_HF_AT_EVENT_CIND                        0x08
#define HCI_CONTROL_HF_AT_EVENT_CLIP                        0x09
#define HCI_CONTROL_HF_AT_EVENT_CIEV                        0x0A
#define HCI_CONTROL_HF_AT_EVENT_BINP                        0x0B
#define HCI_CONTROL_HF_AT_EVENT_BVRA                        0x0C
#define HCI_CONTROL_HF_AT_EVENT_BSIR                        0x0D
#define HCI_CONTROL_HF_AT_EVENT_CNUM                        0x0E
#define HCI_CONTROL_HF_AT_EVENT_BTRH                        0x0F
#define HCI_CONTROL_HF_AT_EVENT_COPS                        0x10
#define HCI_CONTROL_HF_AT_EVENT_CLCC                        0x11
#define HCI_CONTROL_HF_AT_EVENT_BIND                        0x12
#define HCI_CONTROL_HF_AT_EVENT_UNAT                        0x13
#define HCI_CONTROL_HF_AT_EVENT_MAX                         0x13    /* Maximum AT event value */

/* 
 * Define LE events from the BLE GATT/GAP
 */
#define HCI_CONTROL_LE_EVENT_COMMAND_STATUS                 ( ( HCI_CONTROL_GROUP_LE << 8 ) | 0x01 )    /* Command status event for the requested operation */
#define HCI_CONTROL_LE_EVENT_SCAN_STATUS                    ( ( HCI_CONTROL_GROUP_LE << 8 ) | 0x02 )    /* LE scanning state change notification */
#define HCI_CONTROL_LE_EVENT_ADVERTISEMENT_REPORT           ( ( HCI_CONTROL_GROUP_LE << 8 ) | 0x03 )    /* Advertisement report */
#define HCI_CONTROL_LE_EVENT_ADVERTISEMENT_STATE            ( ( HCI_CONTROL_GROUP_LE << 8 ) | 0x04 )    /* LE Advertisement state change notification */
#define HCI_CONTROL_LE_EVENT_CONNECTED                      ( ( HCI_CONTROL_GROUP_LE << 8 ) | 0x05 )    /* LE Connection established */
#define HCI_CONTROL_LE_EVENT_DISCONNECTED                   ( ( HCI_CONTROL_GROUP_LE << 8 ) | 0x06 )    /* Le Connection Terminated */

/*
* Define GATT events 
*/
#define HCI_CONTROL_GATT_EVENT_COMMAND_STATUS               ( ( HCI_CONTROL_GROUP_GATT << 8 ) | 0x01 )    /* Command status event for the requested operation */
#define HCI_CONTROL_GATT_EVENT_DISCOVERY_COMPLETE           ( ( HCI_CONTROL_GROUP_GATT << 8 ) | 0x02 )    /* Discovery requested by host completed */
#define HCI_CONTROL_GATT_EVENT_SERVICE_DISCOVERED           ( ( HCI_CONTROL_GROUP_GATT << 8 ) | 0x03 )    /* Service discovered */
#define HCI_CONTROL_GATT_EVENT_CHARACTERISTIC_DISCOVERED    ( ( HCI_CONTROL_GROUP_GATT << 8 ) | 0x04 )    /* Characteristic discovered */
#define HCI_CONTROL_GATT_EVENT_DESCRIPTOR_DISCOVERED        ( ( HCI_CONTROL_GROUP_GATT << 8 ) | 0x05 )    /* Characteristic descriptor discovered */
#define HCI_CONTROL_GATT_EVENT_READ_REQUEST                 ( ( HCI_CONTROL_GROUP_GATT << 8 ) | 0x06 )    /* Peer sent Read Request */
#define HCI_CONTROL_GATT_EVENT_READ_RESPONSE                ( ( HCI_CONTROL_GROUP_GATT << 8 ) | 0x07 )    /* Read response */
#define HCI_CONTROL_GATT_EVENT_WRITE_REQUEST                ( ( HCI_CONTROL_GROUP_GATT << 8 ) | 0x08 )    /* Peer sent Write Request */
#define HCI_CONTROL_GATT_EVENT_WRITE_RESPONSE               ( ( HCI_CONTROL_GROUP_GATT << 8 ) | 0x09 )    /* Write operation completed */
#define HCI_CONTROL_GATT_EVENT_INDICATION                   ( ( HCI_CONTROL_GROUP_GATT << 8 ) | 0x0a )    /* indication from peer */
#define HCI_CONTROL_GATT_EVENT_NOTIFICATION                 ( ( HCI_CONTROL_GROUP_GATT << 8 ) | 0x0b )    /* notification from peer */

/*
 * Define events from the SPP profile
 */
#define HCI_CONTROL_SPP_EVENT_CONNECTED                     ( ( HCI_CONTROL_GROUP_SPP << 8 ) | 0x01 )    /* SPP connection opened */
#define HCI_CONTROL_SPP_EVENT_SERVICE_NOT_FOUND             ( ( HCI_CONTROL_GROUP_SPP << 8 ) | 0x02 )    /* SDP record with SPP service not found */
#define HCI_CONTROL_SPP_EVENT_CONNECTION_FAILED             ( ( HCI_CONTROL_GROUP_SPP << 8 ) | 0x03 )    /* Connection attempt failed  */
#define HCI_CONTROL_SPP_EVENT_DISCONNECTED                  ( ( HCI_CONTROL_GROUP_SPP << 8 ) | 0x04 )    /* SPP connection closed */
#define HCI_CONTROL_SPP_EVENT_TX_COMPLETE                   ( ( HCI_CONTROL_GROUP_SPP << 8 ) | 0x05 )    /* Data packet has been queued for transmission */
#define HCI_CONTROL_SPP_EVENT_RX_DATA                       ( ( HCI_CONTROL_GROUP_SPP << 8 ) | 0x06 )    /* Data packet has been queued for transmission */


/*
* Define events from the Audio profile
*/
#define HCI_CONTROL_AUDIO_EVENT_COMMAND_COMPLETE            ( ( HCI_CONTROL_GROUP_AUDIO << 8 ) | 0x00 )    /* Command complete event for the requested operation */
#define HCI_CONTROL_AUDIO_EVENT_COMMAND_STATUS              ( ( HCI_CONTROL_GROUP_AUDIO << 8 ) | 0x01 )    /* Command status event for the requested operation */
#define HCI_CONTROL_AUDIO_EVENT_CONNECTED                   ( ( HCI_CONTROL_GROUP_AUDIO << 8 ) | 0x02 )    /* Audio connection opened */
#define HCI_CONTROL_AUDIO_EVENT_SERVICE_NOT_FOUND           ( ( HCI_CONTROL_GROUP_AUDIO << 8 ) | 0x03 )    /* SDP record with audio service not found */
#define HCI_CONTROL_AUDIO_EVENT_CONNECTION_FAILED           ( ( HCI_CONTROL_GROUP_AUDIO << 8 ) | 0x04 )    /* Connection attempt failed  */
#define HCI_CONTROL_AUDIO_EVENT_DISCONNECTED                ( ( HCI_CONTROL_GROUP_AUDIO << 8 ) | 0x05 )    /* Audio connection closed */
#define HCI_CONTROL_AUDIO_EVENT_REQUEST_DATA                ( ( HCI_CONTROL_GROUP_AUDIO << 8 ) | 0x06 )    /* Request for audio pcm sample data */
#define HCI_CONTROL_AUDIO_EVENT_STARTED                     ( ( HCI_CONTROL_GROUP_AUDIO << 8 ) | 0x07 )    /* Command for audio start succeeded */
#define HCI_CONTROL_AUDIO_EVENT_STOPPED                     ( ( HCI_CONTROL_GROUP_AUDIO << 8 ) | 0x08 )    /* Command for audio stop completed */

/*
 * Define events from the AVRCP CT profile
 */
#define HCI_CONTROL_AVRC_EVENT_CONNECTED                    ( ( HCI_CONTROL_GROUP_AVRC << 8 ) | 0x01 )    /* AVRCP Controller connected */
#define HCI_CONTROL_AVRC_EVENT_DISCONNECTED                 ( ( HCI_CONTROL_GROUP_AVRC << 8 ) | 0x02 )    /* AVRCP Controller disconnected */
#define HCI_CONTROL_AVRC_EVENT_CURRENT_TRACK_INFO           ( ( HCI_CONTROL_GROUP_AVRC << 8 ) | 0x03 )    /* AVRCP Controller disconnected */
#define HCI_CONTROL_AVRC_EVENT_PLAY_STATUS                  ( ( HCI_CONTROL_GROUP_AVRC << 8 ) | 0x04 )    /* AVRCP Controller Play Status Change */
#define HCI_CONTROL_AVRC_EVENT_PLAY_POSITION                ( ( HCI_CONTROL_GROUP_AVRC << 8 ) | 0x05 )    /* AVRCP Controller Play Position Change */
#define HCI_CONTROL_AVRC_EVENT_TRACK_CHANGE                 ( ( HCI_CONTROL_GROUP_AVRC << 8 ) | 0x06 )    /* AVRCP Controller Track Changed */
#define HCI_CONTROL_AVRC_EVENT_TRACK_END                    ( ( HCI_CONTROL_GROUP_AVRC << 8 ) | 0x07 )    /* AVRCP Controller Track reached End */
#define HCI_CONTROL_AVRC_EVENT_TRACK_START                  ( ( HCI_CONTROL_GROUP_AVRC << 8 ) | 0x08 )    /* AVRCP Controller Track reached Start */
#define HCI_CONTROL_AVRC_EVENT_SETTING_AVAILABLE            ( ( HCI_CONTROL_GROUP_AVRC << 8 ) | 0x09 )    /* AVRCP Controller Player setting available */
#define HCI_CONTROL_AVRC_EVENT_SETTING_CHANGE               ( ( HCI_CONTROL_GROUP_AVRC << 8 ) | 0x0a )    /* AVRCP Controller Player setting changed */

 /*
 * Define events from the AVRCP TG profile
 */
#define HCI_CONTROL_AVRC_EVENT_PLAY                         ( ( HCI_CONTROL_GROUP_AVRC << 8 ) | 0x01 )    /* Play passthrough command received */
#define HCI_CONTROL_AVRC_EVENT_PAUSE                        ( ( HCI_CONTROL_GROUP_AVRC << 8 ) | 0x02 )    /* Pause passthrough command received */
#define HCI_CONTROL_AVRC_EVENT_STOP                         ( ( HCI_CONTROL_GROUP_AVRC << 8 ) | 0x03 )    /* Stop passthrough command received */
#define HCI_CONTROL_AVRC_EVENT_VOLUME_LEVEL                 ( ( HCI_CONTROL_GROUP_AVRC << 8 ) | 0x04 )    /* Volume Level changed received */
#define HCI_CONTROL_AVRC_EVENT_NEXT_TRACK                   ( ( HCI_CONTROL_GROUP_AVRC << 8 ) | 0x06 )    /* Next Track passthrough command received */
#define HCI_CONTROL_AVRC_EVENT_PREVIOUS_TRACK               ( ( HCI_CONTROL_GROUP_AVRC << 8 ) | 0x07 )    /* Previous track passthrough command received */

/*
 * Define events from the HFP profile
 */
#define HCI_CONTROL_HID_EVENT_OPENED                        ( ( HCI_CONTROL_GROUP_HIDD << 8 ) | 0x01 )    /* Both HID channels are opened */
#define HCI_CONTROL_HID_EVENT_VIRTUAL_CABLE_UNPLUGGED       ( ( HCI_CONTROL_GROUP_HIDD << 8 ) | 0x02 )    /* Host requested Virtual Cable Unplug */
#define HCI_CONTROL_HID_EVENT_DATA                          ( ( HCI_CONTROL_GROUP_HIDD << 8 ) | 0x03 )    /* Host sent report */
#define HCI_CONTROL_HID_EVENT_CLOSED                        ( ( HCI_CONTROL_GROUP_HIDD << 8 ) | 0x04 )    /* Host attempt to establish connection failed */

/*
 * Define events from the Automation IO profile
 */
#define HCI_CONTROL_AIO_EVENT_COMMAND_STATUS                ( ( HCI_CONTROL_GROUP_AIO << 8 ) | 0x01 )      /* Command status */
#define HCI_CONTROL_AIO_EVENT_DIGITAL_OUT                   ( ( HCI_CONTROL_GROUP_AIO << 8 ) | 0x02 )      /* Digital output */
#define HCI_CONTROL_AIO_EVENT_ANALOG_OUT                    ( ( HCI_CONTROL_GROUP_AIO << 8 ) | 0x03 )      /* Analog output */
#define HCI_CONTROL_AIO_EVENT_CONNECTED                     ( ( HCI_CONTROL_GROUP_AIO << 8 ) | 0x04 )      /* Connected to server */
#define HCI_CONTROL_AIO_EVENT_READ_RSP                      ( ( HCI_CONTROL_GROUP_AIO << 8 ) | 0x05 )      /* Read response */
#define HCI_CONTROL_AIO_EVENT_WRITE_RSP                     ( ( HCI_CONTROL_GROUP_AIO << 8 ) | 0x06 )      /* Write response */
#define HCI_CONTROL_AIO_EVENT_VALUE_IN                      ( ( HCI_CONTROL_GROUP_AIO << 8 ) | 0x07 )      /* Notification/indication */
#define HCI_CONTROL_AIO_EVENT_DISCONNECTED                  ( ( HCI_CONTROL_GROUP_AIO << 8 ) | 0x08 )      /* Disconnected from server */

/*
 * Define events from the Current Time
 */
#define HCI_CONTROL_TIME_EVENT_UPDATE                       ( ( HCI_CONTROL_GROUP_TIME << 8 ) | 0x01 )      /* Time Change notification */

/*
 * Define events from the ANCS
 */
#define HCI_CONTROL_ANCS_EVENT_NOTIFICATION                 ( ( HCI_CONTROL_GROUP_ANCS << 8 ) | 0x01 )      /* ANCS notification */

/*
 * Define events from the FindMe application
 */
#define HCI_CONTROL_ALERT_EVENT_NOTIFICATION                ( ( HCI_CONTROL_GROUP_ALERT << 8 ) | 0x01 )     /* Alert Level Notification */

/*
 * Define events from the IAP2 implementation
 */
#define HCI_CONTROL_IAP2_EVENT_CONNECTED                    ( ( HCI_CONTROL_GROUP_IAP2 << 8 ) | 0x01 )    /* IAP2 connection opened */
#define HCI_CONTROL_IAP2_EVENT_SERVICE_NOT_FOUND            ( ( HCI_CONTROL_GROUP_IAP2 << 8 ) | 0x02 )    /* SDP record with IAP2 service not found */
#define HCI_CONTROL_IAP2_EVENT_CONNECTION_FAILED            ( ( HCI_CONTROL_GROUP_IAP2 << 8 ) | 0x03 )    /* Connection attempt failed  */
#define HCI_CONTROL_IAP2_EVENT_DISCONNECTED                 ( ( HCI_CONTROL_GROUP_IAP2 << 8 ) | 0x04 )    /* IAP2 connection closed */
#define HCI_CONTROL_IAP2_EVENT_TX_COMPLETE                  ( ( HCI_CONTROL_GROUP_IAP2 << 8 ) | 0x05 )    /* Data packet has been queued for transmission */
#define HCI_CONTROL_IAP2_EVENT_RX_DATA                      ( ( HCI_CONTROL_GROUP_IAP2 << 8 ) | 0x06 )    /* Data packet has been queued for transmission */

/*
 * Define event for Handsfree AG implementation
 */
#define HCI_CONTROL_AG_EVENT_OPEN                           ( ( HCI_CONTROL_GROUP_AG << 8 ) | 0x01 )
#define HCI_CONTROL_AG_EVENT_CLOSE                          ( ( HCI_CONTROL_GROUP_AG << 8 ) | 0x02 )
#define HCI_CONTROL_AG_EVENT_CONNECTED                      ( ( HCI_CONTROL_GROUP_AG << 8 ) | 0x03 )
#define HCI_CONTROL_AG_EVENT_AUDIO_OPEN                     ( ( HCI_CONTROL_GROUP_AG << 8 ) | 0x04 )
#define HCI_CONTROL_AG_EVENT_AUDIO_CLOSE                    ( ( HCI_CONTROL_GROUP_AG << 8 ) | 0x05 )

/*
 * Location and Navigation events
 */
#define HCI_CONTROL_LN_EVENT_GET_LOCATION_SPEED             ( ( HCI_CONTROL_GROUP_LN << 8 ) | 0x01 )    /* Get location and speed data */
#define HCI_CONTROL_LN_EVENT_LN_CONTROL                     ( ( HCI_CONTROL_GROUP_LN << 8 ) | 0x02 )    /* LN control point request from client */
#define HCI_CONTROL_LN_EVENT_GET_NAVIGATION                 ( ( HCI_CONTROL_GROUP_LN << 8 ) | 0x03 )    /* Get navigation data */

/*
 * Define Miscellaneous events
 */
#define HCI_CONTROL_MISC_EVENT_PING_REPLY                   ( ( HCI_CONTROL_GROUP_MISC << 8 ) | 0x01 )    /* Ping reply */

/*
 * Define Scan state that is reported with the HCI_CONTROL_LE_EVENT_SCAN_STATUS
 */
#define HCI_CONTROL_SCAN_EVENT_NO_SCAN                      0
#define HCI_CONTROL_SCAN_EVENT_HIGH_SCAN                    1
#define HCI_CONTROL_SCAN_EVENT_LOW_SCAN                     2
#define HCI_CONTROL_SCAN_EVENT_HIGH_CONN                    3
#define HCI_CONTROL_SCAN_EVENT_LOW_CONN                     4

/*
* Define status code returned in HCI_CONTROL_EVENT_COMMAND_STATUS
*/
#define HCI_CONTROL_STATUS_SUCCESS                          0
#define HCI_CONTROL_STATUS_IN_PROGRESS                      1
#define HCI_CONTROL_STATUS_ALREADY_CONNECTED                2
#define HCI_CONTROL_STATUS_NOT_CONNECTED                    3
#define HCI_CONTROL_STATUS_BAD_HANDLE                       4
#define HCI_CONTROL_STATUS_WRONG_STATE                      5
#define HCI_CONTROL_STATUS_INVALID_ARGS                     6
#define HCI_CONTROL_STATUS_FAILED                           7
#define HCI_CONTROL_STATUS_UNKNOWN_GROUP                    8
#define HCI_CONTROL_STATUS_UNKNOWN_COMMAND                  9
#define HCI_CONTROL_STATUS_CLIENT_NOT_REGISTERED            10

/* HS open status */
#define HCI_CONTROL_HF_STATUS_SUCCESS                       0   /* Connection successfully opened */
#define HCI_CONTROL_HF_STATUS_FAIL_SDP                      1   /* Open failed due to SDP */
#define HCI_CONTROL_HF_STATUS_FAIL_RFCOMM                   2   /* Open failed due to RFCOMM */
#define HCI_CONTROL_HF_STATUS_FAIL_CONN_TOUT                3   /* Link loss occured due to connection timeout */

#ifndef BD_ADDR_LEN
#define BD_ADDR_LEN 6
#endif

#define LE_ADV_STATE_NO_DISCOVERABLE                        0
#define LE_ADV_STATE_HIGH_DISCOVERABLE                      1
#define LE_ADV_STATE_LOW_DISCOVERABLE                       2

/*
 * HID Report Channel
 */
#define HCI_CONTROL_HID_REPORT_CHANNEL_CONTROL              0
#define HCI_CONTROL_HID_REPORT_CHANNEL_INTERRUPT            1

/*
* HID Report Type (matches BT HID Spec definitions)
*/
#define HCI_CONTROL_HID_REPORT_TYPE_OTHER                   0
#define HCI_CONTROL_HID_REPORT_TYPE_INPUT                   1
#define HCI_CONTROL_HID_REPORT_TYPE_OUTPUT                  2
#define HCI_CONTROL_HID_REPORT_TYPE_FEATURE                 3

/* Max TX packet to be sent over SPP */
#define HCI_CONTROL_SPP_MAX_TX_BUFFER                       700

/* Max GATT command packet size to be sent over uart */
#define HCI_CONTROL_GATT_COMMAND_MAX_TX_BUFFER              100

/*
* Define status code returned in HCI_CONTROL_AIO_EVENT_COMMAND_STATUS
*/
#define HCI_CONTROL_AIO_STATUS_SUCCESS                      0   /* Command executed successfully */
#define HCI_CONTROL_AIO_STATUS_IN_PROGRESS                  1   /* Previous command in progress */
#define HCI_CONTROL_AIO_STATUS_ALREADY_CONNECTED            2   /* Already connected to server */
#define HCI_CONTROL_AIO_STATUS_NOT_CONNECTED                3   /* Not connected to server */
#define HCI_CONTROL_AIO_STATUS_CHAR_NOT_FOUND               4   /* Characteristic not found */
#define HCI_CONTROL_AIO_STATUS_DESC_NOT_FOUND               5   /* Characteristic descriptor not found */
#define HCI_CONTROL_AIO_STATUS_INVALID_ARGS                 6   /* Invalid arguments */
#define HCI_CONTROL_AIO_STATUS_FAILED                       7   /* Generic failure */

/*
* Define AIO characteristic type
*/
#define HCI_CONTROL_AIO_CHAR_TYPE_DIGITAL                   1   /* Digital characteristic */
#define HCI_CONTROL_AIO_CHAR_TYPE_ANALOG                    2   /* Analog characteristic */
#define HCI_CONTROL_AIO_CHAR_TYPE_AGGREGATE                 3   /* Aggregate characteristic */

#endif

