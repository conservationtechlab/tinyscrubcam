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

#include <stdint.h>

/* clang-format off */
#include "platform_dct.h"
#include "wiced_dct_common.h"
/* clang-format on */

/* Offsets taken from 43xxx_Wi-Fi/WICED/platform/MCU/wiced_dct_local_file.c */
const uint32_t DCT_section_offsets[] = {
    [DCT_APP_SECTION] = sizeof(platform_dct_data_t),
    [DCT_HK_INFO_SECTION] = sizeof(platform_dct_data_t),
    [DCT_SECURITY_SECTION] =
        OFFSETOF(platform_dct_data_t, security_credentials),
    [DCT_MFG_INFO_SECTION] = OFFSETOF(platform_dct_data_t, mfg_info),
    [DCT_WIFI_CONFIG_SECTION] = OFFSETOF(platform_dct_data_t, wifi_config),
    [DCT_ETHERNET_CONFIG_SECTION] =
        OFFSETOF(platform_dct_data_t, ethernet_config),
    [DCT_NETWORK_CONFIG_SECTION] =
        OFFSETOF(platform_dct_data_t, network_config),
#ifdef WICED_DCT_INCLUDE_BT_CONFIG
    [DCT_BT_CONFIG_SECTION] = OFFSETOF(platform_dct_data_t, bt_config),
#endif
#ifdef WICED_DCT_INCLUDE_P2P_CONFIG
    [DCT_P2P_CONFIG_SECTION] = OFFSETOF(platform_dct_data_t, p2p_config),
#endif
#ifdef OTA2_SUPPORT
    [DCT_OTA2_CONFIG_SECTION] = OFFSETOF(platform_dct_data_t, ota2_config),
#endif
    [DCT_MISC_SECTION] = OFFSETOF(platform_dct_data_t, dct_misc),
    [DCT_INTERNAL_SECTION] = 0,
};
