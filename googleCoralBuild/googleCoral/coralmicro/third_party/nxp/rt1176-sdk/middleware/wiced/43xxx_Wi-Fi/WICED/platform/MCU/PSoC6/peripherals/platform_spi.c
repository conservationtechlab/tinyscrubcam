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
 * PSoC 6 platform SPI driver
 */

#include <stdint.h>
#include <string.h>
#include "platform_cmsis.h"
#include "platform_constants.h"
#include "platform_peripheral.h"
#include "platform_isr_interface.h"
#include "wwd_assert.h"
#include "wiced_utilities.h"

/******************************************************
 *                      Macros
 ******************************************************/

/* SPI Clock Polarity (CPOL) configuration */
#define SCB_SPI_CPOL_HIGH( config )        ( ( config->mode & SPI_CLOCK_IDLE_HIGH ) ? 1 : 0 )

/* SPI Clock Phase (CPHA) configuration */
#define SCB_SPI_CPHA_HIGH( config )        ( ( config->mode & SPI_CLOCK_RISING_EDGE ) ? SCB_SPI_CPOL_HIGH( config ) : !SCB_SPI_CPOL_HIGH( config ) )

/* SPI Data Transfer Order configuration */
#define SCB_SPI_MSB_FIRST( config )        ( ( config->mode & SPI_MSB_FIRST ) ? 1 : 0 )

/* SPI Chip Select Level configuration */
#define SCB_SPI_CS_ACTIVEHIGH( config )    ( ( config->mode & SPI_CS_ACTIVE_HIGH ) ? 1 : 0 )

/******************************************************
 *                    Constants
 ******************************************************/

#define SCB_SPI_PORT_MAX 8
#define DIV_INT_8_BIT_MAX (0x1 << 8)
#define DIV_INT_16_BIT_MAX (0x1 << 16)
#define DIV_INT_24_BIT_MAX (0x1 << 24)
#define PERI_CLOCK_FREQ_HZ 100000000
#define SCB_SPI_OVERSAMPLE 8
#define SCB_SPI_DATA_WIDTH 8

/******************************************************
 *                   Enumerations
 ******************************************************/

/******************************************************
 *                 Type Definitions
 ******************************************************/

typedef platform_result_t (*spi_init_func_t)               ( const platform_spi_t* spi, const platform_spi_config_t* config );
typedef platform_result_t (*spi_deinit_func_t)             ( const platform_spi_t* spi );
typedef platform_result_t (*spi_transfer_func_t)           ( const platform_spi_t* spi, const platform_spi_config_t* config, const platform_spi_message_segment_t* segments, uint16_t number_of_segments );
typedef platform_result_t (*spi_transfer_nosetup_func_t)   ( const platform_spi_t* spi, const platform_spi_config_t* config, const uint8_t* send_ptr, uint8_t* rcv_ptr, uint32_t length );
typedef platform_result_t (*spi_chip_select_toggle_func_t) ( const platform_spi_t* spi, const platform_spi_config_t* config, wiced_bool_t activate );

/******************************************************
 *                    Structures
 ******************************************************/

typedef struct
{
    spi_init_func_t init;
    spi_deinit_func_t deinit;
    spi_transfer_func_t transfer;
    spi_transfer_nosetup_func_t transfer_nosetup;
    spi_chip_select_toggle_func_t chip_select_toggle;
} spi_driver_t;

/******************************************************
 *               Static Function Declarations
 ******************************************************/

/* SCB SPI bus interface */
static platform_result_t    spi_init                  ( const platform_spi_t* spi, const platform_spi_config_t* config );
static platform_result_t    spi_deinit                ( const platform_spi_t* spi );
static platform_result_t    spi_transfer              ( const platform_spi_t* spi, const platform_spi_config_t* config, const platform_spi_message_segment_t* segments, uint16_t number_of_segments );
static platform_result_t    spi_transfer_nosetup      ( const platform_spi_t* spi, const platform_spi_config_t* config, const uint8_t* send_ptr, uint8_t* rcv_ptr, uint32_t length );
static platform_result_t    spi_chip_select_toggle    ( const platform_spi_t* spi, const platform_spi_config_t* config, wiced_bool_t activate );

/******************************************************
 *               Variable Definitions
 ******************************************************/

/* SCB SPI bus driver functions */
const spi_driver_t scb_spi_driver =
{
    .init = spi_init,
    .deinit = spi_deinit,
    .transfer = spi_transfer,
    .transfer_nosetup = spi_transfer_nosetup,
    .chip_select_toggle = spi_chip_select_toggle
};

/* SCB SPI driver instance */
static const spi_driver_t* spi_driver = &scb_spi_driver;

/******************************************************
 *               Function Definitions
 ******************************************************/

static platform_result_t scb_spi_clock_init( platform_scb_t const* platform_scb, uint32_t spi_speed )
{
    uint32_t spi_clk;
    uint32_t div_int;
    uint32_t div_max;
    cy_en_divider_types_t div_type;

    div_type = platform_scb->pclk->divider_type;

    switch ( div_type )
    {
        case CY_SYSCLK_DIV_8_BIT:
            div_max = DIV_INT_8_BIT_MAX;
            break;

        case CY_SYSCLK_DIV_16_BIT:
            div_max = DIV_INT_16_BIT_MAX;
            break;

        case CY_SYSCLK_DIV_16_5_BIT:
            div_max = DIV_INT_16_BIT_MAX;
            break;

        case CY_SYSCLK_DIV_24_5_BIT:
            div_max = DIV_INT_24_BIT_MAX;
            break;

        default:
            return PLATFORM_ERROR;
    }

    spi_clk = (uint32_t)(PERI_CLOCK_FREQ_HZ / SCB_SPI_OVERSAMPLE);

    /* SPI speed = peri_clock / (divider + 1) / oversample */
    for ( div_int = 0 ; div_int < div_max ; div_int++ )
    {
        if ( (spi_clk / (div_int + 1)) <= spi_speed )
        {
            break;
        }
    }

    if ( platform_peripheral_clock_init(platform_scb->pclk, div_int, 0 ) != PLATFORM_SUCCESS )
    {
        return PLATFORM_ERROR;
    }

    return PLATFORM_SUCCESS;
}

static platform_result_t scb_spi_pin_init( const platform_spi_t* spi )
{
    GPIO_PRT_Type *port_base;

    /* Setup the SPI MOSI pin */
    if ( spi->mosi_pin != NULL )
    {
        port_base = Cy_GPIO_PortToAddr( spi->mosi_pin->port_num );
        Cy_GPIO_Pin_FastInit( port_base, spi->mosi_pin->pin_num, CY_GPIO_DM_STRONG_IN_OFF, 0x1, spi->mosi_pin->hsiom );
    }

    /* Setup the SPI MISO pin */
    if ( spi->miso_pin != NULL )
    {
        port_base = Cy_GPIO_PortToAddr( spi->miso_pin->port_num );
        Cy_GPIO_Pin_FastInit( port_base, spi->miso_pin->pin_num, CY_GPIO_DM_HIGHZ, 0x1, spi->miso_pin->hsiom );
    }

    /* Setup the SPI Clock pin */
    if ( spi->clock_pin != NULL )
    {
        port_base = Cy_GPIO_PortToAddr( spi->clock_pin->port_num );
        Cy_GPIO_Pin_FastInit( port_base, spi->clock_pin->pin_num, CY_GPIO_DM_STRONG_IN_OFF, 0x1, spi->clock_pin->hsiom );
    }

    /* Setup the SPI CS pin */
    if ( spi->cs_pin != NULL )
    {
        port_base = Cy_GPIO_PortToAddr( spi->cs_pin->port_num );
        Cy_GPIO_Pin_FastInit( port_base, spi->cs_pin->pin_num, CY_GPIO_DM_STRONG_IN_OFF, 0x1, spi->cs_pin->hsiom );
    }

    return PLATFORM_SUCCESS;
}

static platform_result_t spi_init( const platform_spi_t* spi, const platform_spi_config_t* config )
{
    CySCB_Type* scb_spi;
    uint8_t scb_spi_mode;
    cy_stc_scb_spi_config_t scb_spi_config;

    platform_mcu_powersave_disable();

    /* Check for valid data width */
    if ( config->bits != SCB_SPI_DATA_WIDTH )
    {
        return PLATFORM_UNSUPPORTED;
    }

    /* Lookup the SCB SPI instance */
    scb_spi = spi->port->scb_base;

    if ( scb_spi == NULL )
    {
        return PLATFORM_UNSUPPORTED;
    }

    /* Disable the SCB SPI */
    Cy_SCB_SPI_Disable( scb_spi, NULL );

    /* Setup the SCB SPI configuration structure */
    scb_spi_config.spiMode = CY_SCB_SPI_MASTER;
    scb_spi_config.subMode = CY_SCB_SPI_MOTOROLA;

    scb_spi_mode = ( SCB_SPI_CPHA_HIGH(config) << 1 ) | ( SCB_SPI_CPOL_HIGH(config) );

    switch (scb_spi_mode)
    {
        case 0: /* CPHA = 0, CPOL = 0 */
            scb_spi_config.sclkMode = CY_SCB_SPI_CPHA0_CPOL0;
            break;

        case 1: /* CPHA = 0, CPOL = 1 */
            scb_spi_config.sclkMode = CY_SCB_SPI_CPHA0_CPOL1;
            break;

        case 2: /* CPHA = 1, CPOL = 0 */
            scb_spi_config.sclkMode = CY_SCB_SPI_CPHA1_CPOL0;
            break;

        case 3: /* CPHA = 1, CPOL = 1 */
            scb_spi_config.sclkMode = CY_SCB_SPI_CPHA1_CPOL1;
            break;
    }

    scb_spi_config.oversample = SCB_SPI_OVERSAMPLE;

    scb_spi_config.rxDataWidth = config->bits;
    scb_spi_config.txDataWidth = config->bits;

    if ( SCB_SPI_MSB_FIRST(config) != 0 )
    {
        scb_spi_config.enableMsbFirst = true;
    }
    else
    {
        scb_spi_config.enableMsbFirst = false;
    }

    scb_spi_config.enableInputFilter = false;

    scb_spi_config.enableFreeRunSclk = false;
    scb_spi_config.enableMisoLateSample = false;
    scb_spi_config.enableTransferSeperation = false;

    if ( SCB_SPI_CS_ACTIVEHIGH(config) != 0 )
    {
        scb_spi_config.ssPolarity = ((CY_SCB_SPI_ACTIVE_HIGH << CY_SCB_SPI_SLAVE_SELECT0) |
                                     (CY_SCB_SPI_ACTIVE_HIGH << CY_SCB_SPI_SLAVE_SELECT1) |
                                     (CY_SCB_SPI_ACTIVE_HIGH << CY_SCB_SPI_SLAVE_SELECT2) |
                                     (CY_SCB_SPI_ACTIVE_HIGH << CY_SCB_SPI_SLAVE_SELECT3));
    }
    else
    {
        scb_spi_config.ssPolarity = ((CY_SCB_SPI_ACTIVE_LOW << CY_SCB_SPI_SLAVE_SELECT0) |
                                     (CY_SCB_SPI_ACTIVE_LOW << CY_SCB_SPI_SLAVE_SELECT1) |
                                     (CY_SCB_SPI_ACTIVE_LOW << CY_SCB_SPI_SLAVE_SELECT2) |
                                     (CY_SCB_SPI_ACTIVE_LOW << CY_SCB_SPI_SLAVE_SELECT3));
    }

    scb_spi_config.enableWakeFromSleep = false;

    scb_spi_config.rxFifoTriggerLevel = 0;
    scb_spi_config.rxFifoIntEnableMask = 0x0;

    scb_spi_config.txFifoTriggerLevel = 0;
    scb_spi_config.txFifoIntEnableMask = 0x0;

    scb_spi_config.masterSlaveIntEnableMask = 0x0;

    /* Initialize the SCB SPI clock */
    if ( scb_spi_clock_init( spi->port, config->speed ) != PLATFORM_SUCCESS )
    {
        return PLATFORM_ERROR;
    }

    /* Initialize the SCB SPI pins */
    if ( scb_spi_pin_init( spi ) != PLATFORM_SUCCESS )
    {
        return PLATFORM_ERROR;
    }

    if ( config->chip_select != NULL )
    {
        /* GPIO used as chip select */
        if ( platform_gpio_init( config->chip_select, OUTPUT_PUSH_PULL ) != PLATFORM_SUCCESS )
        {
            return PLATFORM_ERROR;
        }
    }

    /* Initialize the SCB SPI */
    if ( Cy_SCB_SPI_Init( scb_spi, &scb_spi_config, NULL ) != CY_SCB_SPI_SUCCESS )
    {
        return PLATFORM_ERROR;
    }

    /* Set active slave select */
    Cy_SCB_SPI_SetActiveSlaveSelect( scb_spi, spi->ssel );

    /* Enable the SCB SPI */
    Cy_SCB_SPI_Enable( scb_spi );

    platform_mcu_powersave_enable();

    return PLATFORM_SUCCESS;
}

static platform_result_t spi_deinit( const platform_spi_t* spi )
{
    CySCB_Type* scb_spi;

    platform_mcu_powersave_disable();

    /* Lookup the SCB SPI instance */
    scb_spi = spi->port->scb_base;

    if ( scb_spi == NULL )
    {
        return PLATFORM_UNSUPPORTED;
    }

    /* Disable the SCB SPI */
    Cy_SCB_SPI_Disable( scb_spi, NULL );

    /* De-initialize the SCB SPI */
    Cy_SCB_SPI_DeInit( scb_spi );

    platform_mcu_powersave_enable();

    return PLATFORM_SUCCESS;
}

static platform_result_t scb_spi_read_byte( CySCB_Type* scb_spi, uint8_t* data)
{
    uint32_t rx_data;

    while ( Cy_SCB_SPI_GetNumInRxFifo(scb_spi) == 0 )
    {
        /* Wait for RX FIFO not empty */
    }

    rx_data = Cy_SCB_SPI_Read(scb_spi);

    if ( rx_data == CY_SCB_SPI_RX_NO_DATA )
    {
        return PLATFORM_ERROR;
    }

    *data = (uint8_t)rx_data;

    return PLATFORM_SUCCESS;
}

static platform_result_t scb_spi_write_byte( CySCB_Type* scb_spi, uint8_t data)
{
    uint32_t tx_data;

    while ( Cy_SCB_GetNumInTxFifo(scb_spi) == Cy_SCB_GetFifoSize(scb_spi) )
    {
        /* Wait for TX FIFO not full */
    }

    tx_data = (uint32_t)data;

    if ( Cy_SCB_SPI_Write(scb_spi, tx_data) != 1 )
    {
        return PLATFORM_ERROR;
    }

    return PLATFORM_SUCCESS;
}

static platform_result_t spi_transfer_nosetup( const platform_spi_t* spi, const platform_spi_config_t* config, const uint8_t* send_ptr, uint8_t* rcv_ptr, uint32_t length )
{
    CySCB_Type* scb_spi;

    /* Lookup the SCB SPI instance */
    scb_spi = spi->port->scb_base;

    if ( scb_spi == NULL )
    {
        return PLATFORM_UNSUPPORTED;
    }

    while ( length-- )
    {
        uint8_t data = 0xFF;

        if ( send_ptr != NULL )
        {
            data = *send_ptr++;
        }

        if ( scb_spi_write_byte(scb_spi, data) != PLATFORM_SUCCESS )
        {
            return PLATFORM_ERROR;
        }

        if ( scb_spi_read_byte(scb_spi, &data) != PLATFORM_SUCCESS )
        {
            return PLATFORM_ERROR;
        }

        if ( rcv_ptr != NULL )
        {
            *rcv_ptr++ = data;
        }
    }

    return PLATFORM_SUCCESS;
}

static platform_result_t spi_transfer( const platform_spi_t* spi, const platform_spi_config_t* config, const platform_spi_message_segment_t* segments, uint16_t number_of_segments )
{
    uint16_t i;

    /* Activate chip select */
    spi_chip_select_toggle( spi, config, WICED_TRUE );

    /* SPI data transfer */
    for ( i = 0; i < number_of_segments; i++ )
    {
        const uint8_t* send_ptr = ( const uint8_t* )segments[i].tx_buffer;
        uint8_t*       rcv_ptr  = ( uint8_t* )segments[i].rx_buffer;

        spi_transfer_nosetup( spi, config, send_ptr, rcv_ptr, segments[i].length );
    }

    /* Deactivate chip select */
    spi_chip_select_toggle( spi, config, WICED_FALSE );

    return PLATFORM_SUCCESS;
}

static platform_result_t spi_chip_select_toggle( const platform_spi_t* spi, const platform_spi_config_t* config, wiced_bool_t activate )
{
    if ( config->chip_select != NULL )
    {
        if ( activate == WICED_TRUE )
        {
            if ( config->mode & SPI_CS_ACTIVE_HIGH )
            {
                platform_gpio_output_high( config->chip_select );
            }
            else
            {
                platform_gpio_output_low( config->chip_select );
            }
        }
        else
        {
            if ( config->mode & SPI_CS_ACTIVE_HIGH )
            {
                platform_gpio_output_low( config->chip_select );
            }
            else
            {
                platform_gpio_output_high( config->chip_select );
            }
        }
    }

    return PLATFORM_SUCCESS;
}

platform_result_t platform_spi_init( const platform_spi_t* spi, const platform_spi_config_t* config )
{
    platform_result_t result;

    wiced_assert( "bad argument", ( spi != NULL ) && ( config != NULL ) && ( ( config->mode & SPI_USE_DMA ) == 0 ) );

    result = spi_driver->init(spi, config);

    return result;
}

platform_result_t platform_spi_deinit( const platform_spi_t* spi )
{
    platform_result_t result;

    wiced_assert( "bad argument", ( spi != NULL ) );

    result = spi_driver->deinit(spi);

    return result;
}

platform_result_t platform_spi_transfer( const platform_spi_t* spi, const platform_spi_config_t* config, const platform_spi_message_segment_t* segments, uint16_t number_of_segments )
{
    platform_result_t result;

    wiced_assert( "bad argument", ( spi != NULL ) && ( config != NULL ) && ( segments != NULL ) && ( number_of_segments != 0 ) && ( config->bits == 8 ) );

    result = spi_driver->transfer(spi, config, segments, number_of_segments);

    return result;
}

platform_result_t platform_spi_transfer_nosetup( const platform_spi_t* spi, const platform_spi_config_t* config, const uint8_t* send_ptr, uint8_t* rcv_ptr, uint32_t length )
{
    platform_result_t result;

    result = spi_driver->transfer_nosetup(spi, config, send_ptr, rcv_ptr, length);

    return result;
}

platform_result_t platform_spi_chip_select_toggle( const platform_spi_t* spi, const platform_spi_config_t* config, wiced_bool_t activate )
{
    platform_result_t result;

    result = spi_driver->chip_select_toggle(spi, config, activate);

    return result;
}

static platform_result_t scb_spi_slave_pin_init( const platform_spi_t* spi )
{
    GPIO_PRT_Type *port_base;

    /* Setup the SPI MOSI pin */
    if ( spi->mosi_pin != NULL )
    {
        port_base = Cy_GPIO_PortToAddr( spi->mosi_pin->port_num );
        Cy_GPIO_Pin_FastInit( port_base, spi->mosi_pin->pin_num, CY_GPIO_DM_HIGHZ, 0x1, spi->mosi_pin->hsiom );
    }

    /* Setup the SPI MISO pin */
    if ( spi->miso_pin != NULL )
    {
        port_base = Cy_GPIO_PortToAddr( spi->miso_pin->port_num );
        Cy_GPIO_Pin_FastInit( port_base, spi->miso_pin->pin_num, CY_GPIO_DM_STRONG_IN_OFF, 0x1, spi->miso_pin->hsiom );
    }

    /* Setup the SPI Clock pin */
    if ( spi->clock_pin != NULL )
    {
        port_base = Cy_GPIO_PortToAddr( spi->clock_pin->port_num );
        Cy_GPIO_Pin_FastInit( port_base, spi->clock_pin->pin_num, CY_GPIO_DM_HIGHZ, 0x1, spi->clock_pin->hsiom );
    }

    /* Setup the SPI CS pin */
    if ( spi->cs_pin != NULL )
    {
        port_base = Cy_GPIO_PortToAddr( spi->cs_pin->port_num );
        Cy_GPIO_Pin_FastInit( port_base, spi->cs_pin->pin_num, CY_GPIO_DM_HIGHZ, 0x1, spi->cs_pin->hsiom );
    }

    return PLATFORM_SUCCESS;
}

platform_result_t scb_spi_slave_init( platform_spi_slave_driver_t* driver, const platform_spi_t* peripheral, const platform_spi_slave_config_t* config )
{
    CySCB_Type* scb_spi;
    uint8_t scb_spi_mode;
    cy_stc_scb_spi_config_t scb_spi_config;

    platform_mcu_powersave_disable();

    /* Check for valid data width */
    if ( config->bits != SCB_SPI_DATA_WIDTH )
    {
        return PLATFORM_UNSUPPORTED;
    }

    /* Lookup the SCB SPI instance */
    scb_spi = peripheral->port->scb_base;

    if ( scb_spi == NULL )
    {
        return PLATFORM_UNSUPPORTED;
    }

    /* Disable the SCB SPI */
    Cy_SCB_SPI_Disable( scb_spi, NULL );

    driver->peripheral = peripheral;
    driver->config = config;

    /* Setup the SCB SPI configuration structure */
    scb_spi_config.spiMode = CY_SCB_SPI_SLAVE;
    scb_spi_config.subMode = CY_SCB_SPI_MOTOROLA;

    scb_spi_mode = ( SCB_SPI_CPHA_HIGH(config) << 1 ) | ( SCB_SPI_CPOL_HIGH(config) );

    switch (scb_spi_mode)
    {
        case 0: /* CPHA = 0, CPOL = 0 */
            scb_spi_config.sclkMode = CY_SCB_SPI_CPHA0_CPOL0;
            break;

        case 1: /* CPHA = 0, CPOL = 1 */
            scb_spi_config.sclkMode = CY_SCB_SPI_CPHA0_CPOL1;
            break;

        case 2: /* CPHA = 1, CPOL = 0 */
            scb_spi_config.sclkMode = CY_SCB_SPI_CPHA1_CPOL0;
            break;

        case 3: /* CPHA = 1, CPOL = 1 */
            scb_spi_config.sclkMode = CY_SCB_SPI_CPHA1_CPOL1;
            break;
    }

    scb_spi_config.oversample = 0;

    scb_spi_config.rxDataWidth = config->bits;
    scb_spi_config.txDataWidth = config->bits;

    if ( SCB_SPI_MSB_FIRST(config) != 0 )
    {
        scb_spi_config.enableMsbFirst = true;
    }
    else
    {
        scb_spi_config.enableMsbFirst = false;
    }

    scb_spi_config.enableInputFilter = false;

    scb_spi_config.enableFreeRunSclk = false;
    scb_spi_config.enableMisoLateSample = false;
    scb_spi_config.enableTransferSeperation = false;

    if ( SCB_SPI_CS_ACTIVEHIGH(config) != 0 )
    {
        scb_spi_config.ssPolarity = ((CY_SCB_SPI_ACTIVE_HIGH << CY_SCB_SPI_SLAVE_SELECT0) |
                                     (CY_SCB_SPI_ACTIVE_HIGH << CY_SCB_SPI_SLAVE_SELECT1) |
                                     (CY_SCB_SPI_ACTIVE_HIGH << CY_SCB_SPI_SLAVE_SELECT2) |
                                     (CY_SCB_SPI_ACTIVE_HIGH << CY_SCB_SPI_SLAVE_SELECT3));
    }
    else
    {
        scb_spi_config.ssPolarity = ((CY_SCB_SPI_ACTIVE_LOW << CY_SCB_SPI_SLAVE_SELECT0) |
                                     (CY_SCB_SPI_ACTIVE_LOW << CY_SCB_SPI_SLAVE_SELECT1) |
                                     (CY_SCB_SPI_ACTIVE_LOW << CY_SCB_SPI_SLAVE_SELECT2) |
                                     (CY_SCB_SPI_ACTIVE_LOW << CY_SCB_SPI_SLAVE_SELECT3));
    }

    scb_spi_config.enableWakeFromSleep = false;

    scb_spi_config.rxFifoTriggerLevel = 0;
    scb_spi_config.rxFifoIntEnableMask = 0x0;

    scb_spi_config.txFifoTriggerLevel = 0;
    scb_spi_config.txFifoIntEnableMask = 0x0;

    scb_spi_config.masterSlaveIntEnableMask = 0x0;

    /* Initialize the SCB SPI clock */
    if ( scb_spi_clock_init( peripheral->port, config->speed ) != PLATFORM_SUCCESS )
    {
        return PLATFORM_ERROR;
    }

    /* Initialize the SCB SPI pins */
    if ( scb_spi_slave_pin_init( peripheral ) != PLATFORM_SUCCESS )
    {
        return PLATFORM_ERROR;
    }

    /* Initialize the SCB SPI */
    if ( Cy_SCB_SPI_Init( scb_spi, &scb_spi_config, NULL ) != CY_SCB_SPI_SUCCESS )
    {
        return PLATFORM_ERROR;
    }

    /* Set active slave select */
    Cy_SCB_SPI_SetActiveSlaveSelect( scb_spi, peripheral->ssel );

    /* Enable the SCB SPI */
    Cy_SCB_SPI_Enable( scb_spi );

    platform_mcu_powersave_enable();

    return PLATFORM_SUCCESS;
}

static platform_result_t scb_spi_slave_deinit( platform_spi_slave_driver_t* driver )
{
    CySCB_Type* scb_spi;

    platform_mcu_powersave_disable();

    /* Lookup the SCB SPI instance */
    scb_spi = driver->peripheral->port->scb_base;

    if ( scb_spi == NULL )
    {
        return PLATFORM_UNSUPPORTED;
    }

    /* Disable the SCB SPI */
    Cy_SCB_SPI_Disable( scb_spi, NULL );

    /* De-initialize the SCB SPI */
    Cy_SCB_SPI_DeInit( scb_spi );

    platform_mcu_powersave_enable();

    return PLATFORM_SUCCESS;
}

static platform_result_t scb_spi_slave_transfer_data( platform_spi_slave_driver_t* driver, platform_spi_slave_transfer_direction_t direction, platform_spi_slave_data_buffer_t* buffer, uint32_t timeout_ms )
{
    CySCB_Type* scb_spi;
    uint16_t tx_bytes_remaining;
    uint16_t rx_bytes_remaining;
    uint16_t rx_index;
    uint16_t tx_index;
    uint8_t  rchar = 0;
    uint8_t  tchar = 0xFF;

    wiced_assert( "Invalid argument", driver != NULL );
    wiced_assert( "Invalid argument", buffer != NULL );
    wiced_assert( "Invalid argument", ( direction == SPI_SLAVE_TRANSFER_WRITE ) || ( direction == SPI_SLAVE_TRANSFER_READ ) );

    platform_mcu_powersave_disable();

    /* Lookup the SCB SPI instance */
    scb_spi = driver->peripheral->port->scb_base;

    if ( scb_spi == NULL )
    {
        return PLATFORM_UNSUPPORTED;
    }

    tx_bytes_remaining = buffer->data_length + 1;
    rx_bytes_remaining = buffer->data_length + 1;
    tx_index = 0;
    rx_index = 0;

    while ( tx_bytes_remaining || rx_bytes_remaining )
    {
        if ( tx_bytes_remaining )
        {
            if ( tx_index == 0 )
            {
                tchar = buffer->status;
            }
            else
            {
                if ( direction == SPI_SLAVE_TRANSFER_READ )
                {
                    tchar = buffer->data[tx_index - 1];
                }
                else
                {
                    tchar = 0xFF;
                }
            }

            if ( scb_spi_write_byte( scb_spi, tchar ) != PLATFORM_SUCCESS )
            {
                return PLATFORM_ERROR;
            }

            tx_bytes_remaining--;
            tx_index++;
        }

        if ( rx_bytes_remaining )
        {
            if ( scb_spi_read_byte( scb_spi, &rchar ) != PLATFORM_SUCCESS )
            {
                return PLATFORM_ERROR;
            }

            if ( rx_index == 0 )
            {
                buffer->status = rchar;
            }
            else
            {
                buffer->data[rx_index - 1] = rchar;
            }

            rx_bytes_remaining--;
            rx_index++;
        }
    }

    platform_mcu_powersave_enable();

    return PLATFORM_SUCCESS;
}

platform_result_t platform_spi_slave_init( platform_spi_slave_driver_t* driver, const platform_spi_t* peripheral, const platform_spi_slave_config_t* config )
{
    platform_result_t result;

    wiced_assert( "bad argument", ( driver != NULL ) && ( peripheral != NULL ) && ( config != NULL ) );

    result = scb_spi_slave_init( driver, peripheral, config);

    return result;
}

platform_result_t platform_spi_slave_deinit( platform_spi_slave_driver_t* driver )
{
    platform_result_t result;

    wiced_assert( "bad argument", ( driver != NULL ) );

    result = scb_spi_slave_deinit( driver );

    return result;
}

platform_result_t platform_spi_slave_send_error_status( platform_spi_slave_driver_t* driver, platform_spi_slave_transfer_status_t error_status )
{
    platform_spi_slave_data_buffer_t buffer = { 0 };

    buffer.status      = error_status;
    buffer.data_length = 0;

    return scb_spi_slave_transfer_data( driver, SPI_SLAVE_TRANSFER_WRITE, &buffer, WICED_NEVER_TIMEOUT );
}

platform_result_t platform_spi_slave_receive_command( platform_spi_slave_driver_t* driver, platform_spi_slave_command_t* command, uint32_t timeout_ms )
{
    CySCB_Type* scb_spi;
    uint32_t i = 0;
    uint8_t* command_ptr = (uint8_t*)command;
    uint8_t rx = 0;

    /* Lookup the SCB SPI instance */
    scb_spi = driver->peripheral->port->scb_base;

    if ( scb_spi == NULL )
    {
        return PLATFORM_UNSUPPORTED;
    }

     /* Read SPI slave protocol header from the RX FIFO */
    for ( i = 0 ; i < sizeof(platform_spi_slave_command_t) ; i++ )
    {
        if ( scb_spi_read_byte( scb_spi, &rx ) != PLATFORM_SUCCESS )
        {
            return PLATFORM_ERROR;
        }

        *command_ptr++ = rx;
    }

    return PLATFORM_SUCCESS;
}

platform_result_t platform_spi_slave_transfer_data( platform_spi_slave_driver_t* driver, platform_spi_slave_transfer_direction_t direction, platform_spi_slave_data_buffer_t* buffer, uint32_t timeout_ms )
{
    buffer->status = SPI_SLAVE_TRANSFER_SUCCESS;
    return scb_spi_slave_transfer_data( driver, direction, buffer, timeout_ms );
}

platform_result_t platform_spi_slave_generate_interrupt( platform_spi_slave_driver_t* driver, uint32_t pulse_duration_ms )
{
    return PLATFORM_UNSUPPORTED;
}
