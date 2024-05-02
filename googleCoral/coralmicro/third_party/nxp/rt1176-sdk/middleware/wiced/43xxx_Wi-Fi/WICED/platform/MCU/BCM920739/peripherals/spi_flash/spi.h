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
#ifndef _SPI_H_
#define _SPI_H_
/*******************************************************************************
* THIS INFORMATION IS PROPRIETARY TO BROADCOM CORP
*
* ------------------------------------------------------------------------------
*
* Copyright (c) 2016 Broadcom Corp.
*
*          ALL RIGHTS RESERVED
*
********************************************************************************
*
* File Name: spi.h
*
* Abstract: Serial Peripheral Interface driver
*
*
********************************************************************************
*
* $History:$
*
********************************************************************************/
#include "brcm_fw_types.h"

#ifdef __cplusplus
extern "C"
{
#endif
/******************************************************
 *                     Macros
 ******************************************************/
/******************************************************
 *                    Constants
 ******************************************************/
#define SPIFFY_CLK_FREQ_12_MHZ        12000000
#define SPIFFY_IO_GET_SFLASH        1
#define SPIFFY_IO_GET_TRANSPORT        2
/******************************************************
 *                   Enumerations
 ******************************************************/
/******************************************************
* Type Definitions
******************************************************/
typedef enum
{
    SPIFFY_LSB_FIRST,
    SPIFFY_MSB_FIRST
} SPIFFY_ENDIAN;

typedef enum
{
    SPIFFY_SS_ACTIVE_LOW,
    SPIFFY_SS_ACTIVE_HIGH
} SPIFFY_SS_POLARITY;

typedef enum
{
    SPIFFY_MODE_0,
    SPIFFY_MODE_1,
    SPIFFY_MODE_2,
    SPIFFY_MODE_3
} SPIFFY_MODE;

typedef enum
{
    SPIFFY_SUCCESS,
    SPIFFY_FAIL,
    SPIFFY_SLAVE_NOT_ENOUGH_RX_FIFO_BYTES
} SPIFFY_STATUS;

typedef enum
{
    SPIFFY_BLOCK_1 = 0,
    SPIFFY_BLOCK_2 = 1
} SPIFFY_BLOCK;



/*******************************************************************************
* Function Prototypes
*******************************************************************************/
void spi_reset(UINT32 hdl, UINT32 freq_in_hz, SPIFFY_ENDIAN endian, SPIFFY_SS_POLARITY pol,
                   SPIFFY_MODE mode);
void spi_write(UINT32 hdl,IN UINT8 * buffer,UINT32 totalLen,UINT32 length,BOOL32 first,BOOL32 last);
void spi_write_then_read(UINT32 hdl, UINT8 *tx_buf, UINT32 tx_length,
                              UINT8 *rx_buf, UINT32 rx_length);
platform_result_t platform_sflash_spi_pin_config( void );

#ifdef __cplusplus
} /* extern "C" */
#endif
#endif  //_SPI_H_



