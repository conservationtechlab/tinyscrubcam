/* * Copyright 2021, Cypress Semiconductor Corporation or a subsidiary of 
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
 * so agrees to indemnify Cypress against all liability. */MEMORY{    BTLDR_VECTORS(rx) : ORIGIN = 0x1A000000, LENGTH = 512    CODE_READ_PROTECTION_A(rx) : ORIGIN = 0x1A0002FC, LENGTH = 4        SHARED_API(rx) : ORIGIN = 0x1A000300, LENGTH = 512    BTLDR_CODE(rx) : ORIGIN = 0x1A000500, LENGTH = 15K    DCT1_FLASH(rx) : ORIGIN = 0x1A004000, LENGTH = 16K     /* align DCT to sector boundary */    DCT2_FLASH(rx) : ORIGIN = 0x1A008000, LENGTH = 16K     /* align DCT align to sector boundary */    APP_CODE_A(rx) : ORIGIN = 0x1A00C000, LENGTH = 475135  /* align start address of the app to sector boundary */    /* End address of the BANK A is 0x1A07FFFF */        APP_CODE_B(rx) : ORIGIN = 0x1B000000, LENGTH = 524287    /* End address of the BANK A is 0x1B07FFFF */        CODE_READ_PROTECTION_B(rx) : ORIGIN = 0x1B0002FC, LENGTH = 4    SRAM_FAST(rwx) : ORIGIN = 0x10000000,  LENGTH = 32K    SRAM_DATA(rwx) : ORIGIN = 0x10080000,  LENGTH = 40K    SRAM_STACK(rwx) : ORIGIN = 0x20000000, LENGTH = 32K}