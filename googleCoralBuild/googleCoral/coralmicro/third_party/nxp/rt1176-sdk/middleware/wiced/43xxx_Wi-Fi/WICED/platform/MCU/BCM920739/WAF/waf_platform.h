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

/******************************************************
 *                   Constants
 ******************************************************/

/******************************************************
 *                    Macros
 ******************************************************/
#define SECTOR_SIZE (4096)

#if defined(OTA2_SUPPORT)
#define PLATFORM_DCT_COPY1_START_ADDRESS     ( OTA2_IMAGE_CURR_DCT_1_AREA_BASE )
#else
#define PLATFORM_DCT_COPY1_START_ADDRESS     ( 0x3D000 )
#endif

#ifndef PLATFORM_DCT_COPY1_SIZE
#define PLATFORM_DCT_COPY1_SIZE              ( 4 * SECTOR_SIZE )
#endif

#define PLATFORM_DCT_COPY2_START_ADDRESS     ( PLATFORM_DCT_COPY1_START_ADDRESS + PLATFORM_DCT_COPY1_SIZE )

#define PLATFORM_DCT_COPY1_START_SECTOR      ( (PLATFORM_DCT_COPY1_START_ADDRESS) / SECTOR_SIZE )
#define PLATFORM_DCT_COPY1_END_SECTOR        ( (PLATFORM_DCT_COPY1_START_ADDRESS + PLATFORM_DCT_COPY1_SIZE) / SECTOR_SIZE )
#define PLATFORM_DCT_COPY1_END_ADDRESS       (  PLATFORM_DCT_COPY1_START_ADDRESS + PLATFORM_DCT_COPY1_SIZE )

#define PLATFORM_DCT_COPY2_SIZE              ( PLATFORM_DCT_COPY1_SIZE )
#define PLATFORM_DCT_COPY2_START_SECTOR      ( (PLATFORM_DCT_COPY2_START_ADDRESS) / SECTOR_SIZE )
#define PLATFORM_DCT_COPY2_END_SECTOR        ( (PLATFORM_DCT_COPY2_START_ADDRESS + PLATFORM_DCT_COPY2_SIZE) / SECTOR_SIZE )
#define PLATFORM_DCT_COPY2_END_ADDRESS       (  PLATFORM_DCT_COPY2_START_ADDRESS + PLATFORM_DCT_COPY2_SIZE )

#define PLATFORM_DEFAULT_LOAD                ( WICED_FRAMEWORK_LOAD_ALWAYS )
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
/* Check length of time the "Factory Reset" button is pressed
 *
 * NOTES: This is used for ota2_btmcu_bootloader.
 *
 *        To change the button used for this test on your platform, change
 *           platforms/<platform>/platform.h: PLATFORM_FACTORY_RESET_BUTTON_INDEX
 *
 *        To change the LED used for this purpose, see the usage of PLATFORM_RED_LED_INDEX
 *           platforms/<platform>/platform.c:
 *
 *
 * USAGE for OTA2 support (Over The Air version 2 see <Wiced-SDK>/apps/waf/ota2_btmcu_bootloader/ota2_btmcu_bootloader.c).
 *            ~10 seconds - initiate Factory Reset
 *
 * param    max_time    - maximum time to wait
 *
 * returns  usecs button was held
 *
 */
uint32_t  platform_get_factory_reset_button_time ( uint32_t max_time );

#ifdef __cplusplus
} /* extern "C" */
#endif

