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
#include <stdint.h>
#include <string.h>
#include "platform_cmsis.h"
#include "platform_constants.h"
#include "platform_peripheral.h"
#include "platform_isr_interface.h"
#include "wwd_assert.h"

/******************************************************
 *                      Macros
 ******************************************************/

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
 *               Static Function Declarations
 ******************************************************/

/******************************************************
 *               Variable Definitions
 ******************************************************/

/******************************************************
 *            WICED Function Definitions
 ******************************************************/

platform_result_t platform_spi_init( const platform_spi_t* spi, const platform_spi_config_t* config )
{
    UNUSED_PARAMETER(spi);
    UNUSED_PARAMETER(config);

    platform_mcu_powersave_disable( );

    Pdc* spi_pdc = spi_get_pdc_base( spi->peripheral );

    /* Setup chip select pin */
    platform_gpio_init( config->chip_select, OUTPUT_PUSH_PULL );
    platform_gpio_output_high( config->chip_select );

    /* Setup other pins */
    platform_gpio_peripheral_pin_init( spi->mosi_pin, ( IOPORT_MODE_MUX_A | IOPORT_MODE_PULLUP ) );
    platform_gpio_peripheral_pin_init( spi->miso_pin, ( IOPORT_MODE_MUX_A | IOPORT_MODE_PULLUP ) );
    platform_gpio_peripheral_pin_init( spi->clk_pin,  ( IOPORT_MODE_MUX_A | IOPORT_MODE_PULLUP )  );

    /* Configure an SPI peripheral. */
    pmc_enable_periph_clk( spi->peripheral_id );
    spi_disable( spi->peripheral );
    spi_reset( spi->peripheral );
    spi_set_lastxfer( spi->peripheral );
    spi_set_master_mode( spi->peripheral );
    spi_disable_mode_fault_detect( spi->peripheral );
    spi_set_peripheral_chip_select_value( spi->peripheral, 0 );

    spi_set_clock_polarity( spi->peripheral, 0, ( ( config->mode & SPI_CLOCK_IDLE_HIGH )   ? (1) : (0) ) );
    spi_set_clock_phase( spi->peripheral, 0,    ( ( config->mode & SPI_CLOCK_RISING_EDGE  ) ? (0) : (1) ) );

    spi_set_bits_per_transfer( spi->peripheral, 0, SPI_CSR_BITS_8_BIT );
    spi_set_baudrate_div( spi->peripheral, 0, (uint8_t)( CPU_CLOCK_HZ / config->speed ) );
    spi_set_transfer_delay( spi->peripheral, 0, 0, 0 );
    spi_enable( spi->peripheral );
    pdc_disable_transfer( spi_pdc, PERIPH_PTCR_RXTDIS | PERIPH_PTCR_TXTDIS );

    platform_mcu_powersave_enable( );

    return PLATFORM_SUCCESS;
}

platform_result_t platform_spi_deinit( const platform_spi_t* spi )
{
    UNUSED_PARAMETER(spi);

    /* Disable the RX and TX PDC transfer requests */
    spi_disable( spi->peripheral );
    spi_reset( spi->peripheral );
    return PLATFORM_SUCCESS;
}

platform_result_t sam4s_spi_transfer_internal( const platform_spi_t* spi, const uint8_t* data_out, uint8_t* data_in, uint32_t data_length )
{
    Pdc*         spi_pdc  = spi_get_pdc_base( spi->peripheral );
    pdc_packet_t pdc_spi_packet;

    pdc_spi_packet.ul_addr = (uint32_t)data_in;
    pdc_spi_packet.ul_size = (uint32_t)data_length;
    pdc_rx_init( spi_pdc, &pdc_spi_packet, NULL );

    pdc_spi_packet.ul_addr = (uint32_t)data_out;
    pdc_spi_packet.ul_size = (uint32_t)data_length;
    pdc_tx_init( spi_pdc, &pdc_spi_packet, NULL );

    /* Enable the RX and TX PDC transfer requests */
    pdc_enable_transfer( spi_pdc, PERIPH_PTCR_RXTEN | PERIPH_PTCR_TXTEN );

    /* Waiting transfer done*/
    while ( ( spi_read_status( spi->peripheral ) & SPI_SR_RXBUFF ) == 0 )
    {
    }

    /* Disable the RX and TX PDC transfer requests */
    pdc_disable_transfer(spi_pdc, PERIPH_PTCR_RXTDIS | PERIPH_PTCR_TXTDIS);

    return PLATFORM_SUCCESS;
}


platform_result_t platform_spi_transfer( const platform_spi_t* spi, const platform_spi_config_t* config, const platform_spi_message_segment_t* segments, uint16_t number_of_segments )
{
    int i = 0;
    platform_result_t result;
    platform_mcu_powersave_disable( );

    platform_gpio_output_low( config->chip_select );

    for ( i = 0; i < number_of_segments; i++ )
    {
        result = sam4s_spi_transfer_internal( spi, segments[i].tx_buffer, segments[i].rx_buffer, segments[i].length );

        if ( result != PLATFORM_SUCCESS )
        {
            return result;
        }
    }

    platform_gpio_output_high( config->chip_select );

    platform_mcu_powersave_enable( );

    return PLATFORM_SUCCESS;
}
