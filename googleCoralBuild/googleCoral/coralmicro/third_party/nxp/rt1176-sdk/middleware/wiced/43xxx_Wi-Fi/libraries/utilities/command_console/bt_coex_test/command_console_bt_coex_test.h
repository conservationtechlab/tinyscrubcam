/*
 * Copyright 2021, Cypress Semiconductor Corporation or a subsidiary of 
 * Cypress Semiconductor Corporation. All Rights Reserved.
 * 
 * This software, associated documentation and materials ("Software"),
 * is owned by Cypress Semiconductor Corporation
 * or one of its subsidiaries ("Cypress") and is protected by and subject to
 * worldwide patent protection (United States and foreign),
 * United States copyright laws and international treaty provisions.
 * Therefore, you may use this Software only as provided in the license
 * agreement accompanying the software package from which you
 * obtained this Software ("EULA").
 * If no EULA applies, Cypress hereby grants you a personal, non-exclusive,
 * non-transferable license to copy, modify, and compile the Software
 * source code solely for use in connection with Cypress's
 * integrated circuit products. Any reproduction, modification, translation,
 * compilation, or representation of this Software except as specified
 * above is prohibited without the express written permission of Cypress.
 *
 * Disclaimer: THIS SOFTWARE IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, NONINFRINGEMENT, IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. Cypress
 * reserves the right to make changes to the Software without notice. Cypress
 * does not assume any liability arising out of the application or use of the
 * Software or any product or circuit described in the Software. Cypress does
 * not authorize its products for use in any products where a malfunction or
 * failure of the Cypress product may reasonably be expected to result in
 * significant property damage, injury or death ("High Risk Product"). By
 * including Cypress's product in a High Risk Product, the manufacturer
 * of such system or application assumes all risk of such use and in doing
 * so agrees to indemnify Cypress against all liability.
 */

/** @file
 *
 */
#pragma once

/* add header files here */


#ifdef __cplusplus
extern "C" {
#endif

//#define ESCAPE_SPACE_PROMPT "\n\t-->When any parameter has spaces, use quotes. E.g. \"my bd_addr\" \"my passcode\""

/******************************************************
 *                     Macros
 ******************************************************/
/* all the definitions here */

#define BT_COEX_TEST_COMMANDS \
        { (char*) "bt_on",                                 bt_on,                               0, NULL, NULL, (char*) "",                                           (char*) "Turn On  Bluetooth"}, \
        { (char*) "bt_off",                                bt_off,                              0, NULL, NULL, (char*) "",                                           (char*) "Turn Off Bluetooth"}, \
        { (char*) "ble_start_adv",                         adv_ble_start,                       0, NULL, NULL, (char*) "",                                           (char*) "start BLE advertisement."}, \
        { (char*) "ble_stop_adv",                          adv_ble_stop,                        0, NULL, NULL, (char*) "",                                           (char*) "stop advertisement "}, \
        { (char*) "ble_start_scan",                        ble_start_scan,                      0, NULL, NULL, (char*) "",                                           (char*) "start LE Scan and connect to hello sensor"}, \
        { (char*) "ble_stop_scan",                         ble_stop_scan,                       0, NULL, NULL, (char*) "",                                           (char*) "stop LE Scan"}, \
        { (char*) "start_rfcomm_server",                   start_rfcomm_server,                 0, NULL, NULL, (char*) "",                                           (char*) "RFCOMM server Init" }, \
        { (char*) "send_rfcomm_data",                      send_rfcomm_data,                    0, NULL, NULL, (char*) "",                                           (char*) "Send RFCOMM data" }, \
        { (char*) "start_rfcomm_client",                   start_rfcomm_client,                 0, NULL, NULL, (char*) "",                                           (char*) "RFCOMM Client Init" }, \
        { (char*) "rfcomm_scan_connect",                   rfcomm_scan_connect,                 0, NULL, NULL, (char*) "",                                           (char*) "Scan and Connect to RFCOMM Server" }, \
        { (char*) "le_coc_init",                           le_coc_init,                         0, NULL, NULL, (char*) "",                                           (char*) "Initalize LE COC" }, \
        { (char*) "le_coc_adv",                            le_coc_adv,                          0, NULL, NULL, (char*) "",                                           (char*) "le_coc_adv" }, \
        { (char*) "le_coc_scan_connect",                   le_coc_scan_connect,                 0, NULL, NULL, (char*) "",                                           (char*) "le_coc_scan and connect to a COC server" }, \
        { (char*) "le_coc_disconnect",                     le_coc_disconnect,                   0, NULL, NULL, (char*) "",                                           (char*) "le coc disconnect" }, \
        { (char*) "le_coc_send_data",                      le_coc_send_data,                    0, NULL, NULL, (char*) "",                                           (char*) "le coc send data" }, \
        { (char*) "stop_data_send",                        stop_data_send,                      0, NULL, NULL, (char*) "",                                           (char*) "stop data send" }, \
        { (char*) "get_throughput",                        get_throughput,                      0, NULL, NULL, (char*) "",                                           (char*) "get_throughput" }, \

/******************************************************
 *                    Constants
 ******************************************************/

/******************************************************
 *                   Enumerations
 ******************************************************/

/******************************************************
 *                 Type Definitions
 ******************************************************/

/******************************************************
 *                    Structures
 ******************************************************/

/******************************************************
 *                 Global Variables
 ******************************************************/

/******************************************************
 *               Function Declarations
 ******************************************************/
/* add function definiton */
/* Console commands */
int bt_on                               ( int argc, char* argv[] );
int bt_off                              ( int argc, char* argv[] );
int adv_ble_start                       ( int argc, char* argv[] );
int adv_ble_stop                        ( int argc, char* argv[] );
int notify_start                        ( int argc, char* argv[] );
int indicate_start                      ( int argc, char* argv[] );
int ble_start_scan                      ( int argc, char* argv[] );
int ble_stop_scan                       ( int argc, char* argv[] );
int start_rfcomm_server                 ( int argc, char* argv[] );
int start_rfcomm_client                 ( int argc, char* argv[] );
int send_rfcomm_data                    ( int argc, char* argv[] );
//int rfcomm_disconnect                   ( int argc, char* argv[] );
int le_coc_init                         ( int argc, char* argv[] );
int le_coc_adv                          ( int argc, char* argv[] );
int le_coc_scan_connect                 ( int argc, char* argv[] );
int le_coc_send_data                    ( int argc, char* argv[] );
int le_coc_disconnect                   ( int argc, char* argv[] );
int stop_data_send                      ( int argc, char* argv[] );
int get_throughput                      ( int argc, char* argv[] );
int rfcomm_scan_connect                 ( int argc, char* argv[] );
int start_rfcomm_client                 ( int argc, char* argv[] );



#ifdef __cplusplus
} /* extern "C" */
#endif
