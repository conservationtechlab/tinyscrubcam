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

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/* AES XTS key */
static const unsigned char aes_test_xts_key[] =
{
    0x27,0x18,0x28,0x18, 0x28,0x45,0x90,0x45, 0x23,0x53,0x60,0x28, 0x74,0x71,0x35,0x26,
    0x62,0x49,0x77,0x57, 0x24,0x70,0x93,0x69, 0x99,0x59,0x57,0x49, 0x66,0x96,0x76,0x27,
    0x31,0x41,0x59,0x26, 0x53,0x58,0x97,0x93, 0x23,0x84,0x62,0x64, 0x33,0x83,0x27,0x95,
    0x02,0x88,0x41,0x97, 0x16,0x93,0x99,0x37, 0x51,0x05,0x82,0x09, 0x74,0x94,0x45,0x92
};

/* AES XTS data_unit/IV */
static const unsigned char aes_test_xts_data_unit[] =
{
    0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};


#ifdef __cplusplus
} /*extern "C" */
#endif

