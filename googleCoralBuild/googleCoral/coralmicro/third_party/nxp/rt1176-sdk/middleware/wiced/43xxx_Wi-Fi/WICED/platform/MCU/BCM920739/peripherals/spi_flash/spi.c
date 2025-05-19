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
* File Name: spi.c
*
* Abstract: Serial Peripheral Interface driver
*
*
********************************************************************************
*
* $History:$
*
********************************************************************************/
#include "auto_flags.h"
#include "platform.h"
#include "platform_constants.h"
#include "platform_mcu_peripheral.h"
#include "platform_peripheral.h"

#include "wiced_platform.h"
#include "spi.h"
#include "spi_priv.h"
#include <stdio.h>
#include "20739mapb0.h"



/*******************************************************************************
* Defines & Macros for SPI master block
*******************************************************************************/


/******************************************************************************
* Type Definitions
******************************************************************************/

/*******************************************************************************
* Variable Declarations
*******************************************************************************/
extern const wiced_spi_device_t wiced_spi_flash;
extern const platform_gpio_t platform_gpio_pins[];
extern const platform_spi_t platform_spi_peripherals[];


/*******************************************************************************
* Function Prototypes
*******************************************************************************/



/*******************************************************************************
* Function: spi_clearDoneBit
*
* Abstract: This function clears the the transmission complete status bit by
*           writing a one to it
*
* Input/Output:
*           spiffyRegPtr if NUM_SPIFFY_BLOCKS > 1
*
* Return:
*           None
*
* Side Effects:
*           None
*
*******************************************************************************/
void spi_clearDoneBit(tSPIFFY_REG *spiffyRegPtr)
{
    spiffyRegPtr->int_status = SPIFFY_INT_GENERIC_SPI_MASTER_DONE;
}

/*******************************************************************************
* Function: spi_waitForDoneBit
*
* Abstract: This function polls until the transmission complete status bit is
*           set
*
* Input/Output:
*           spiffyRegPtr if NUM_SPIFFY_BLOCKS > 1
*
* Return:
*           None
*
* Side Effects:
*           None
*
* Note: This function will never return if there is a clocking problem.
*
*******************************************************************************/
void spi_waitForDoneBit(tSPIFFY_REG *spiffyRegPtr)
{
    while (!(spiffyRegPtr->int_status & SPIFFY_INT_GENERIC_SPI_MASTER_DONE))
    {
    }
}



/*******************************************************************************
* Function: spi_blockingSend
*
* Abstract: This function starts a transmission and then waits for the
*           transmission to complete
*
* Input/Output:
*           spiffyRegPtr if NUM_SPIFFY_BLOCKS > 1
*
* Return:
*           None
*
* Side Effects:
*           None
*
*******************************************************************************/
void spi_blockingSend(tSPIFFY_REG *spiffyRegPtr,UINT32 len)
{
    spi_clearDoneBit(spiffyRegPtr);

    // NOTE: according to Spiffy v.9 document, this should also apply to 2076A0
    spiffyRegPtr->transmission_len = len;

    spi_waitForDoneBit(spiffyRegPtr);
}



/*******************************************************************************
* Function: spi_disableFifos
*
* Abstract: This function disables TX and RX fifos, and clears flow control bits
*
* Input/Output:
*           spiffyRegPtr if NUM_SPIFFY_BLOCKS > 1
*
* Return:
*           None
*
* Side Effects:
*           None
*
*******************************************************************************/
void spi_disableFifos(tSPIFFY_REG *spiffyRegPtr)
{
    spiffyRegPtr->cfg &= (UINT32)(~(SPIFFY_CFG_TX_FIFO_EN | SPIFFY_CFG_RX_FIFO_EN
            | SPIFFY_CFG_FLOW_CONTROL_MASK));
}



/*******************************************************************************
* Function: spi_masterEnableTxOnly
*
* Abstract: This function enables tx-only flow control.  It is used when data
*           only needs to be sent.  It is only used when the protocol is
*           set to Generic Master mode.
*
* Input/Output:
*           spiffyRegPtr if NUM_SPIFFY_BLOCKS > 1
*
* Return:
*           None
*
* Side Effects:
*           None
*
*******************************************************************************/
void spi_masterEnableTxOnly(tSPIFFY_REG *spiffyRegPtr)
{
    spiffyRegPtr->cfg |= SPIFFY_CFG_TX_FIFO_EN | SPIFFY_CFG_FLOW_CONTROL_TX;
}

/*******************************************************************************
* Function: spi_masterEnableFdShortTx
*
* Abstract: This function enables full-duplex short tx flow control.  It is
*           used when need to transmit fewer bytes than receive.  It is only
*           used when the protocol is set to Generic Master mode.
*
* Input/Output:
*           spiffyRegPtr if NUM_SPIFFY_BLOCKS > 1
*
* Return:
*           None
*
* Side Effects:
*           None
*
*******************************************************************************/
void spi_masterEnableFdShortTx(tSPIFFY_REG *spiffyRegPtr)
{
    spiffyRegPtr->cfg |= SPIFFY_CFG_TX_FIFO_EN
        | SPIFFY_CFG_RX_FIFO_EN | SPIFFY_CFG_FLOW_CONTROL_RX;
}



/*******************************************************************************
* Function: spi_drainRxFifo
*
* Abstract: This function reads and discards bytes from RX fifo
*
* Input/Output:
*           count: Number of bytes to read and discard
*           spiffyRegPtr if NUM_SPIFFY_BLOCKS > 1
*
* Return:
*           None
*
* Side Effects:
*           None
*
*******************************************************************************/
void spi_drainRxFifo(UINT32 count, tSPIFFY_REG *spiffyRegPtr)
{
    UINT32 i;
    UINT8 dummy;

    for (i = 0; i < count; i++)
    {
        dummy = (UINT8)spiffyRegPtr->rx_fifo;
    }

    dummy = dummy;
}


/*******************************************************************************
* Function: spi_soft_reset
*
* Abstract: This function writes the new value to the cfg reg, and soft resets
*           the spiffy and pulses the transport clock if necessary.
*
* Input/Output:
*           new_cfg_reg_value: value to write to config register
*           spiffyRegPtr if NUM_SPIFFY_BLOCKS > 1
*
* Return:
*           None
*
* Side Effects:
*           If soft reset is performed, both the spiffy block and the corresponding
*           transport clock domain will be reset.
*
*******************************************************************************/
void spi_soft_reset(UINT32 new_cfg_reg_value, tSPIFFY_REG *spiffyRegPtr)
{
        // Write new config and put spiffy in soft reset.
        spiffyRegPtr->cfg = new_cfg_reg_value | SPIFFY_CFG_SOFT_RESET;

        // Disable all interrupts.
        spiffyRegPtr->int_enable = 0;

        //for old version of spiffy
        if (spiffyRegPtr == (tSPIFFY_REG *)spiffy_cfg_adr)
        {
            // pulse spiffy1 transport clock domain reset
            REG32(cr_pulse_reset_peri_adr) = CR_PULSE_RESET_SPIFFY_TRANSPORT_CLK_DOMAIN;
        }
#ifdef SPIFFY2_HW
        //for new version of spiffy v9
        else if (spiffyRegPtr == (tSPIFFY_REG *)spiffy2_spiffy_cfg_adr)
        {
#ifdef SPIFFY_16BYTE_FIFO
            // In case pulse reset does not clear bit4:3 if reset already asserted
            REG32(cr_level_reset_ptu_aux_adr) &= 0xFFFFFFE7;

            // Before doing anything we need to reset on Spiffy2 block bit4:3
            // spiffy2_hclk should only be reset during boot up, because soft reset could be called
            // after power up, we should not do it here.
            // pulse_reset_ptu_aux[4]    = spiffy2_hclk
            // pulse_reset_ptu_aux[3]    = spiffy2_tportclk
            REG32(cr_pulse_reset_ptu_aux_adr) = 0x00000008;

            // Be sure to have clock gate on
            REG32(cr_ptu_aux_clk_gate_off_adr) = 0x18;
            REG32(cr_ptu_aux_clk_gate_on_adr)  = 0x18;

            // spiffy2 and uart2 etc share the same fifo in ptu_aux, select it for spiffy2
            REG32(dc_ptu_aux_hc_sel_adr) = HW_PTU_HC_SEL_SPIFFY; //0x20
#else
            // pulse spiffy2 transport clock domain reset
            REG32(cr_pulse_reset_peri_adr) = CR_PULSE_RESET_SPIFFY2_TRANSPORT_CLK_DOMAIN;
#endif
        }
#endif
        else
        {
            //ASSERT(0);
        }

        // Write new config and clear soft reset.
        spiffyRegPtr->cfg = new_cfg_reg_value;

        // Write '1' to all interupt status bits.
        spiffyRegPtr->int_status = (UINT32)~0;

}
/*******************************************************************************
* Function: spi_reset
*
* Abstract: This function resets the spi master block
*
* Input/Output:
*           freq_in_hz: The frequency of spi clock in Hz unit - can be
*                       24000000, or between 93750 and 12000000
*           endian:     SPIFFY_ENDIAN enum
*           pol:        SPIFFY_SS_POLARITY enum
*           mode:       SPIFFY_MODE enum
*           spiffyRegPtr if NUM_SPIFFY_BLOCKS > 1
*
* Return:
*           None
*
* Side Effects:
*           Reset the spi master block and config the block the way parameters
*           specify
*
*******************************************************************************/
void spi_reset(UINT32 hdl, UINT32 freq_in_hz, SPIFFY_ENDIAN endian,
                  SPIFFY_SS_POLARITY pol, SPIFFY_MODE mode)
{
    UINT32 clk_cfg_value;
    UINT32 clk_periods;
    UINT32 new_cfg_reg_value;

    tSPIFFY_REG *spiffyRegPtr = NULL;
    SPIFFY_BLOCK spiffy_block= (SPIFFY_BLOCK)hdl;

    if(spiffy_block == SPIFFY_BLOCK_1)
    {
        spiffyRegPtr = (tSPIFFY_REG *)spiffy_cfg_adr;
    }
    else if(spiffy_block == SPIFFY_BLOCK_2)
    {
        spiffyRegPtr = (tSPIFFY_REG *)spiffy2_spiffy_cfg_adr;
    }

    // Enable transport clock.
    REG32(cr_ptu_clk_en_adr) |= 1;

    new_cfg_reg_value = (UINT32)(SPIFFY_CFG_AUTO_CS | SPIFFY_CFG_DELAY_INT
        | SPIFFY_CFG_INT_OEN | SPIFFY_CFG_CS_VAL
        | SPIFFY_CFG_PROTOCOL_GENERIC_MASTER
        | (endian << SPIFFY_CFG_BIT_ENDIANNESS_BITMASK)
        | (pol << SPIFFY_CFG_CS_POLARITY_BITMASK)
        | (mode << SPIFFY_CFG_MODE_BITMASK));

    // Write new cfg reg value and perform soft reset if necessary.
    spi_soft_reset(new_cfg_reg_value, spiffyRegPtr);

    if (freq_in_hz == SPIFFY_MASTER_CLK_FREQ_IN_HZ)
    {
        // Use 24 MHz transport clock directly
        clk_cfg_value = SPIFFY_CLK_USE_TPORT;
    }
    else
    {
        // If clock is out of range, limit to max or min if not in debug mode.
        if (freq_in_hz < SPIFFY_MIN_INTEGRAL_DIVISOR_FREQ_IN_HZ)
        {
            //ASSERT(0);
            freq_in_hz = SPIFFY_MIN_INTEGRAL_DIVISOR_FREQ_IN_HZ;
        }
        else if (freq_in_hz > SPIFFY_MAX_INTEGRAL_DIVISOR_FREQ_IN_HZ)
        {
            //ASSERT(0);
            freq_in_hz = SPIFFY_MAX_INTEGRAL_DIVISOR_FREQ_IN_HZ;
        }

        // Calculate high and low period for 50% duty cycle
        clk_periods = SPIFFY_MAX_INTEGRAL_DIVISOR_FREQ_IN_HZ / freq_in_hz
            - 1;
        clk_periods &= SPIFFY_CLK_TPORT_CLOCK_DIV_MASK;

        clk_cfg_value = (clk_periods << SPIFFY_CLK_TPORT_CLOCK_DIV_H_SHIFT)
           | (clk_periods << SPIFFY_CLK_TPORT_CLOCK_DIV_L_SHIFT);
    }

    spiffyRegPtr->clock_cfg = clk_cfg_value;
}
 /*******************************************************************************
* Function: spi_write
*
* Abstract: This function writes one block of data to tx fifo
*
* Input/Output:
*           totalLen: the total number of bytes of the whole block. When first
*                       is false, this is the number of bytes left of the entire block.
*                        this is the length of the instruction plus data
*           length:        the number of bytes buffer contains
*           buffer:     pointing to the beginning of a sector of the block
*           first:      true if this is the first sector of the block(the command/instruction)
*           last:       true if this is the last sector of the block(the data the user wish to write)
*           spiffyRegPtr if NUM_SPIFFY_BLOCKS > 1
*
* Return:
*           None
*
* Side Effects:
*           None
*
* Note: There is no tx fifo overflow handling.
*
*******************************************************************************/
void spi_write(UINT32 hdl, IN UINT8 *buffer, UINT32 totalLen, UINT32 length, BOOL32 first, BOOL32 last)
{
    tSPIFFY_REG *spiffyRegPtr = NULL;
    SPIFFY_BLOCK spiffy_block= (SPIFFY_BLOCK)hdl;
    UINT32 i;

    if(spiffy_block == SPIFFY_BLOCK_1)
    {
        spiffyRegPtr = (tSPIFFY_REG *)spiffy_cfg_adr;
    }
    else if(spiffy_block == SPIFFY_BLOCK_2)
    {
        spiffyRegPtr = (tSPIFFY_REG *)spiffy2_spiffy_cfg_adr;
    }

    if(length == 0)
    {
        return;
    }


    // Don't disable fifos if continuing multi-part transmit
    // since the transmit fifo will have data in it
    if (first)
    {
        spi_disableFifos(spiffyRegPtr);
    }

    // Write transmit data to fifo
    for (i = 0; i < length; i++)
    {
        spiffyRegPtr->tx_fifo = *buffer++;
    }

    // Don't begin transmitting data until last buffer written to
    // transmit fifo
    if (last)
    {
        spi_masterEnableTxOnly(spiffyRegPtr);

        spi_blockingSend(spiffyRegPtr, totalLen);
    }
}

/*******************************************************************************
* Function: spi_write_then_read
*
* Abstract: This function transmits data in the transmit buffer and stores the
*           received data to the receive buffer.
*
*           NOTE: Any data received during TX will be discarded.
*
* Input/Output:
*           txLen: length of the data to transmit
*           txBuf: pointer to the data buffer to transmit
*           rxLen: length of the data to receive
*           rxBuf: pointer to the buffer where the read data will be stored
*           spiffyRegPtr if NUM_SPIFFY_BLOCKS > 1
*
* Return:
*           None
*
* Side Effects:
*           None
*
* Note: There is no tx fifo overflow or rx fifo underflow handling.
*
*******************************************************************************/
void spi_write_then_read(UINT32 hdl, UINT8 *tx_buf, UINT32 tx_length,
                               UINT8 *rx_buf, UINT32 rx_length)
{
    tSPIFFY_REG *spiffyRegPtr = NULL;
    SPIFFY_BLOCK spiffy_block = (SPIFFY_BLOCK) hdl;

    UINT32 i;

    if(spiffy_block == SPIFFY_BLOCK_1)
    {
        spiffyRegPtr = (tSPIFFY_REG *)spiffy_cfg_adr;
    }
    else if(spiffy_block == SPIFFY_BLOCK_2)
    {
        spiffyRegPtr = (tSPIFFY_REG *)spiffy2_spiffy_cfg_adr;
    }

    if ((tx_length == 0) || (rx_length == 0))
    {
        return;
    }

    spi_disableFifos(spiffyRegPtr);

    // Write transmit data to fifo
    for (i = 0; i < tx_length; i++)
    {
        spiffyRegPtr->tx_fifo = *tx_buf++;
    }

    spi_masterEnableFdShortTx(spiffyRegPtr);

    spi_blockingSend(spiffyRegPtr, tx_length + rx_length);

    // Discard the first txLen bytes from fifo, which
    // are unwanted byproduct of transmitting data
    spi_drainRxFifo(tx_length, spiffyRegPtr);

    // Remaining data in fifo is actual receive data
    for (i = 0; i < rx_length; i++)
    {
        *rx_buf++ = (UINT8)spiffyRegPtr->rx_fifo;  // FIFO only uses LSByte
    }
}


/*******************************************************************************
* Function: spi_sw_reset
*
* Abstract: This function transmits data in the transmit buffer and stores the
*           received data to the receive buffer.
*
*           NOTE: Any data received during TX will be discarded.
*
* Input/Output:
*           txLen: length of the data to transmit
*           txBuf: pointer to the data buffer to transmit
*           rxLen: length of the data to receive
*           rxBuf: pointer to the buffer where the read data will be stored
*           spiffyRegPtr if NUM_SPIFFY_BLOCKS > 1
*
* Return:
*           None
*
* Side Effects:
*           None
*
* Note: There is no tx fifo overflow or rx fifo underflow handling.
*
*******************************************************************************/
tSPIFFY_REG * spi_sw_reset(UINT32 inst, UINT32 cfg, UINT32 tportdiv)
{
    tSPIFFY_REG *spiffyRegPtr = NULL;
    (void) cfg;
    (void) tportdiv;
    if(inst == SPIFFY_BLOCK_1)
    {
        spiffyRegPtr = (tSPIFFY_REG *)spiffy_cfg_adr;
        REG32(dc_ptu_hc_sel_adr) = DC_PTU_HC_SEL_SPIFFY;
    }
    else
    {
        spiffyRegPtr = (tSPIFFY_REG *)spiffy2_spiffy_cfg_adr;
        REG32(dc_ptu_aux_hc_sel_adr) = DC_PTU_AUX_HC_SEL_SPIFFY;
    }
#if 1

    spiffyRegPtr->cfg = cfg | SPIFFY_CFG_SOFTRESET;
    /* clear all int */
    spiffyRegPtr->int_enable = SPIFFY_INT_CLEAR_32_BITS;
    /* Tport divider */
    spiffyRegPtr->clock_cfg = tportdiv;
    /* update cfg */
    spiffyRegPtr->cfg = cfg;
    /* reset spiffy pulse */
    if(inst == SPIFFY_BLOCK_1)
    {
        REG32(cr_pulse_reset_peri_adr) = PULSE_RESET_PERI_SPIFFY;
    }
    else
    {
        REG32(cr_pulse_reset_ptu_aux_adr) = PULSE_RESET_PTU_AUX_SPIFFY;
    }
#endif

    return spiffyRegPtr;
}


//platform_result_t platform_sflash_spi_pin_config( const platform_spi_t* spi, const platform_spi_config_t* config )
platform_result_t platform_sflash_spi_pin_config( void )
{

    platform_spi_config_t config;
    const platform_spi_t* spi = &platform_spi_peripherals[wiced_spi_flash.port];

    config.speed = wiced_spi_flash.speed;
    config.bits = wiced_spi_flash.bits; // not used
    config.chip_select = &platform_gpio_pins[wiced_spi_flash.chip_select],
    config.mode = wiced_spi_flash.mode;


    if ( spi->port ==  SPI1 )
    {
        return PLATFORM_ERROR;
    }
    else if ( spi->port == SPI2 )
    {
        if ( spi->pin_clock->mux_mode == 1 ) // use BT IO (GPIO)
        {
            if ( spi->pin_clock->pin_number == (uint8_t) WICED_GPIO_45 ) {
                    REG32(cr_pad_fcn_ctl_adr0) = (REG32(cr_pad_fcn_ctl_adr0) & 0xFF0FFFFF) | 0x00100000;
                    REG32(cr_pad_fcn_ctl_ext_adr) = (REG32(cr_pad_fcn_ctl_ext_adr)) | 0x00001000;
                    REG32(cr_pad_config_adr1) = (REG32(cr_pad_config_adr1)      & 0xFFFF00FF) | 0x00003800;
                    REG32(cr_pad_config_src_adr) = (REG32(cr_pad_config_src_adr)   & 0xFFFFFFDF);
            }
        }
        if ( spi->pin_mosi->mux_mode == 1 ) // use BT IO (GPIO)
        {
            if ( spi->pin_mosi->pin_number == (uint8_t) WICED_GPIO_43 ) {
                    REG32(cr_pad_fcn_ctl_adr0) = (REG32(cr_pad_fcn_ctl_adr0)     & 0xFFFF0FFF) | 0x00001000;
                    REG32(cr_pad_fcn_ctl_ext_adr) = (REG32(cr_pad_fcn_ctl_ext_adr)) | 0x00004000;
                    REG32(cr_pad_config_adr0) = (REG32(cr_pad_config_adr0)      & 0x00FFFFFF) | 0x38000000;
                    REG32(cr_pad_config_src_adr) = (REG32(cr_pad_config_src_adr)   & 0xFFFFFFF7);
            }
        }
        if ( spi->pin_miso->mux_mode == 1 ) // use BT IO (GPIO)
        {
            if (spi->pin_miso->pin_number  == (uint8_t) WICED_GPIO_42 ) {
                    REG32(cr_pad_fcn_ctl_adr0)     = (REG32(cr_pad_fcn_ctl_adr0)     & 0xFFFFF0FF) | 0x00000100;
                    REG32(cr_pad_fcn_ctl_ext_adr) = (REG32(cr_pad_fcn_ctl_ext_adr)) | 0x00008000;
                    REG32(cr_pad_config_adr0) = (REG32(cr_pad_config_adr0)      & 0xFF00FFFF) | 0x00390000;
                    REG32(cr_pad_config_src_adr) = (REG32(cr_pad_config_src_adr)   & 0xFFFFFFF3);
            }
        }
        if ( config.chip_select->mux_mode == 1 ) // use BT IO (GPIO)
        {
            if ( config.chip_select->pin_number == (uint8_t) WICED_GPIO_44 ) {
                    REG32(cr_pad_fcn_ctl_adr0) = (REG32(cr_pad_fcn_ctl_adr0)     & 0xFFF0FFFF) | 0x00010000;
                    REG32(cr_pad_fcn_ctl_ext_adr) = (REG32(cr_pad_fcn_ctl_ext_adr)) | 0x00002000;
                    REG32(cr_pad_config_adr1) = (REG32(cr_pad_config_adr1)      & 0xFFFFFF00) | 0x00000038;
                    REG32(cr_pad_config_src_adr) = (REG32(cr_pad_config_src_adr)   & 0xFFFFFFEF);
            }
        }
        return PLATFORM_SUCCESS;
    }
    else
    {
        return PLATFORM_ERROR;
    }
}

