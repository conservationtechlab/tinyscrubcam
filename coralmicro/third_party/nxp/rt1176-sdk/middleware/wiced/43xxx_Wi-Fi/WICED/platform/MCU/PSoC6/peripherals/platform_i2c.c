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
#include "stdint.h"
#include "string.h"
#include "platform_peripheral.h"
#include "platform_isr.h"
#include "platform_isr_interface.h"
#include "wwd_rtos.h"
#include "wwd_assert.h"
#include "wiced_platform.h"

/******************************************************
 *                      Macros
 ******************************************************/

/******************************************************
 *                    Constants
 ******************************************************/

#define I2C_START_FLAG             (1U << 0)
#define I2C_STOP_FLAG              (1U << 1)
#define I2C_RW_BIT                 (0x1)

#define PLATFORM_I2C_CHANNEL_MAX   8
#define PLATFORM_I2C_PIN_INDEX_MAX 8
#define PLATFORM_I2C_PIN_PORT_MAX  15

#define I2C_PDL_TIMEOUT            (20)     /* in milliseconds */
#define I2C_PDL_START_TIMEOUT      I2C_PDL_TIMEOUT
#define I2C_PDL_STOP_TIMEOUT       I2C_PDL_TIMEOUT
#define I2C_PDL_WRITE_TIMEOUT      I2C_PDL_TIMEOUT
#define I2C_PDL_READ_TIMEOUT       I2C_PDL_TIMEOUT

#define PERI_CLOCK_HZ              (100000000)
#define PERI_CLOCK_KHZ             (100000)

/******************************************************
 *                   Enumerations
 ******************************************************/

typedef enum {
   I2C_MASTER_WRITE,
   I2C_MASTER_READ
} i2c_rdwr_type;

/******************************************************
 *                 Type Definitions
 ******************************************************/

typedef platform_result_t (i2c_io_fn_t)( const platform_i2c_t*, const platform_i2c_config_t*,
                                         uint8_t flags, uint8_t* buffer, uint16_t buffer_length);

typedef platform_result_t (*i2c_init_func_t)     ( const platform_i2c_t* i2c,
                                                   const platform_i2c_config_t* config );
typedef platform_result_t (*i2c_deinit_func_t)   ( const platform_i2c_t* i2c,
                                                   const platform_i2c_config_t* config );
typedef platform_result_t (*i2c_read_func_t)     ( const platform_i2c_t* i2c, const platform_i2c_config_t* config,
                                                   uint8_t flags, uint8_t* data_in, uint16_t length );
typedef platform_result_t (*i2c_write_func_t)    ( const platform_i2c_t* i2c, const platform_i2c_config_t* config,
                                                   uint8_t flags, uint8_t* data_out, uint16_t length );
typedef platform_result_t (*i2c_transfer_func_t) ( const platform_i2c_t* i2c, const platform_i2c_config_t* config,
                                                   uint16_t flags, void* buffer, uint16_t buffer_length, i2c_io_fn_t* fn );

/******************************************************
 *                    Structures
 ******************************************************/

typedef struct
{
    i2c_init_func_t init;
    i2c_deinit_func_t deinit;
    i2c_read_func_t read;
    i2c_write_func_t write;
    i2c_transfer_func_t transfer;
} i2c_driver_t;

typedef struct
{
    uint32_t data_rate;
    uint32_t peri_clock;
    uint32_t divider;
} scb_i2c_clockrate_config_t;

/******************************************************
 *               Function Declarations
 ******************************************************/

/* Platform I2C bus interface */
static platform_result_t    platform_psoc_i2c_init     ( const platform_i2c_t* i2c, const platform_i2c_config_t* config );
static platform_result_t    platform_psoc_i2c_deinit   ( const platform_i2c_t* i2c, const platform_i2c_config_t* config );
static platform_result_t    platform_psoc_i2c_read     ( const platform_i2c_t* i2c, const platform_i2c_config_t* config,
                                                         uint8_t flags, uint8_t* data_in, uint16_t length );
static platform_result_t    platform_psoc_i2c_write    ( const platform_i2c_t* i2c, const platform_i2c_config_t* config,
                                                         uint8_t flags, uint8_t* data_out, uint16_t length );
static platform_result_t    platform_psoc_i2c_transfer ( const platform_i2c_t* i2c, const platform_i2c_config_t* config,
                                                         uint16_t flags, void* buffer, uint16_t buffer_length, i2c_io_fn_t* fn );

/******************************************************
 *               Variables Definitions
 ******************************************************/

/* Platform I2C bus driver functions */
static const i2c_driver_t i2c_platform_driver =
{
    .init     = platform_psoc_i2c_init,
    .deinit   = platform_psoc_i2c_deinit,
    .read     = platform_psoc_i2c_read,
    .write    = platform_psoc_i2c_write,
    .transfer = platform_psoc_i2c_transfer,
};

/* scbclock = peri_clock / (divider + 1) */
/* Oversample is handled with I2C block logic */
const scb_i2c_clockrate_config_t clockrate_config[] =
{
    {.data_rate = 100000, .peri_clock = PERI_CLOCK_HZ, .divider = 49},
    {.data_rate = 400000, .peri_clock = PERI_CLOCK_HZ, .divider = 10}
};

/* Platform I2C driver instance */
static const i2c_driver_t* i2c_driver = &i2c_platform_driver;

static cy_stc_scb_i2c_context_t scb_context[PLATFORM_I2C_CHANNEL_MAX];

/******************************************************
 *               Function Definitions
 ******************************************************/

/* Mapping of Platform Port I2C port and pin index to Platform I2C SCL pin function */
/* No validation of hsiom setting and Port/pin with scb block validation */
static platform_result_t platform_i2c_scl_pin_init( uint32_t port, const platform_gpio_t* pin )
{
    GPIO_PRT_Type* port_base;

    if ( ( port >= PLATFORM_I2C_CHANNEL_MAX ) || ( pin == NULL )
         || ( pin->port_num > PLATFORM_I2C_PIN_PORT_MAX )
         || ( pin->pin_num > PLATFORM_I2C_PIN_INDEX_MAX ) )

    {
        return PLATFORM_UNSUPPORTED;
    }

    port_base = Cy_GPIO_PortToAddr( pin->port_num );
    Cy_GPIO_Pin_FastInit( port_base, pin->pin_num, CY_GPIO_DM_OD_DRIVESLOW, 0x1, pin->hsiom );

    return PLATFORM_SUCCESS;
}

/* Mapping of Platform I2C port and pin index to Platform I2C SDA pin function */
/* No validation of hsiom setting and Port/pin with scb block validation */
static platform_result_t platform_i2c_sda_pin_init( uint32_t port, const platform_gpio_t* pin )
{
    GPIO_PRT_Type* port_base;

    if ( (port >= PLATFORM_I2C_CHANNEL_MAX ) || ( pin == NULL )
         || ( pin->port_num > PLATFORM_I2C_PIN_PORT_MAX )
         || ( pin->pin_num > PLATFORM_I2C_PIN_INDEX_MAX ) )
    {
        return PLATFORM_UNSUPPORTED;
    }

    port_base = Cy_GPIO_PortToAddr( pin->port_num );
    Cy_GPIO_Pin_FastInit( port_base, pin->pin_num, CY_GPIO_DM_OD_DRIVESLOW, 0x1, pin->hsiom );

    return PLATFORM_SUCCESS;
}

static const scb_i2c_clockrate_config_t* platform_i2c_clockrate_config( uint32_t data_rate )
{
    uint32_t idx;

    for ( idx = 0 ; idx < ( sizeof(clockrate_config)/sizeof(clockrate_config[0] ) ) ; idx++ )
    {
        if ( clockrate_config[idx].data_rate == data_rate )
        {
            return &clockrate_config[idx];
        }
    }

    return NULL;
}

static platform_result_t platform_psoc_i2c_init( const platform_i2c_t* i2c, const platform_i2c_config_t* config )
{

    CySCB_Type* scb_base;
    cy_stc_scb_i2c_context_t* context;
    cy_en_scb_i2c_status_t status;
    cy_stc_scb_i2c_config_t psoc_config = {'\0'};
    const scb_i2c_clockrate_config_t* clkcfg;
    uint32_t data_rate;
    platform_result_t result;

    if ( ( i2c == NULL ) || ( config == NULL ) )
    {
        wiced_assert( "bad argument", 0 );
        return PLATFORM_ERROR;
    }

    scb_base = i2c->scb->scb_base;
    context  = &scb_context[i2c->port];

    if ( scb_base == NULL )
    {
        return PLATFORM_ERROR;
    }

    if ( ( ( config->flags & I2C_DEVICE_NO_DMA ) == 0 ) || ( config->address_width != I2C_ADDRESS_WIDTH_7BIT ) )
    {
        return PLATFORM_UNSUPPORTED;
    }

    /* Configure I2C SCL and SDA pin mapping */
    if ( platform_i2c_scl_pin_init( i2c->port, i2c->scl_pin ) != PLATFORM_SUCCESS )
    {
        return PLATFORM_UNSUPPORTED;
    }

    if ( platform_i2c_sda_pin_init( i2c->port, i2c->sda_pin ) != PLATFORM_SUCCESS )
    {
        return PLATFORM_UNSUPPORTED;
    }

    switch ( config->speed_mode )
    {
        case I2C_LOW_SPEED_MODE:    /* 10Khz devices */
            return PLATFORM_UNSUPPORTED;

        case I2C_STANDARD_SPEED_MODE:    /* 100Khz devices */
            data_rate = 100000u;
            break;

        case I2C_HIGH_SPEED_MODE:    /* 400Khz devices */
            data_rate = 400000u;
            break;

        default:
            return PLATFORM_BADARG;
    }

    clkcfg = platform_i2c_clockrate_config( data_rate );

    if ( clkcfg == NULL )
    {
        return PLATFORM_ERROR;
    }

    result = platform_peripheral_clock_init( i2c->scb->pclk, clkcfg->divider, 0 );

    if ( result != PLATFORM_SUCCESS )
    {
        return PLATFORM_ERROR;
    }

    Cy_SCB_I2C_Disable( scb_base, context );

    /* Setup Master Mode */
    psoc_config.i2cMode = CY_SCB_I2C_MASTER;

    status = Cy_SCB_I2C_Init( scb_base, &psoc_config, context );

    if ( status != CY_SCB_I2C_SUCCESS )
    {
        return PLATFORM_ERROR;
    }

    Cy_SCB_I2C_SetDataRate( scb_base, data_rate, ( PERI_CLOCK_HZ / ( clkcfg->divider + 1 ) ) );

    Cy_SCB_I2C_Enable( scb_base );

    return PLATFORM_SUCCESS;
}

static platform_result_t platform_psoc_i2c_deinit( const platform_i2c_t* i2c, const platform_i2c_config_t* config )
{
    CySCB_Type* scb_base;
    cy_stc_scb_i2c_context_t* context;

    if ( ( i2c == NULL ) || ( config == NULL ) )
    {
        wiced_assert( "bad argument", 0 );
        return PLATFORM_ERROR;
    }

    scb_base = i2c->scb->scb_base;
    context  = &scb_context[i2c->port];

    if ( scb_base == NULL )
    {
        return PLATFORM_ERROR;
    }

    Cy_SCB_I2C_Disable( scb_base, context );

    Cy_SCB_I2C_DeInit( scb_base );

    return PLATFORM_SUCCESS;
}

static platform_result_t platform_psoc_i2c_start( const platform_i2c_t* i2c, uint8_t slave, i2c_rdwr_type type )
{
    CySCB_Type* scb_base;
    cy_en_scb_i2c_status_t status;
    uint32_t   bitrw;
    cy_stc_scb_i2c_context_t* context;

    if ( i2c == NULL )
    {
        return PLATFORM_ERROR;
    }

    scb_base = i2c->scb->scb_base;
    context  = &scb_context[i2c->port];

    if ( scb_base == NULL )
    {
        return PLATFORM_ERROR;
    }


    if ( type == I2C_MASTER_WRITE )
    {
        bitrw = CY_SCB_I2C_WRITE_XFER;
    } else
    {
        bitrw = CY_SCB_I2C_READ_XFER;
    }

    /* Send Start */
    status =  Cy_SCB_I2C_MasterSendStart( scb_base, slave, bitrw, I2C_PDL_START_TIMEOUT, context );

    if ( status != CY_SCB_I2C_SUCCESS ) {
        status = Cy_SCB_I2C_MasterSendReStart(scb_base, slave,
                                    bitrw, I2C_PDL_START_TIMEOUT, context);
        if ( status != CY_SCB_I2C_SUCCESS )
        {
            return PLATFORM_ERROR;
        }
    }

    return PLATFORM_SUCCESS;
}


static platform_result_t platform_psoc_i2c_stop( const platform_i2c_t* i2c )
{
    CySCB_Type* scb_base;
    cy_en_scb_i2c_status_t status;
    cy_stc_scb_i2c_context_t* context;

    if ( i2c == NULL )
    {
        return PLATFORM_ERROR;
    }

    scb_base = i2c->scb->scb_base;
    context  = &scb_context[i2c->port];

    if ( scb_base == NULL )
    {
        return PLATFORM_ERROR;
    }

    /* Send Stop */

    status =  Cy_SCB_I2C_MasterSendStop( scb_base, I2C_PDL_STOP_TIMEOUT, context) ;

    if ( status != CY_SCB_I2C_SUCCESS ) {
        return PLATFORM_ERROR;
    }

    return PLATFORM_SUCCESS;
}

static platform_result_t platform_i2c_writebytes( const platform_i2c_t* i2c, uint8_t* data_out, uint16_t length )
{
    uint16_t i;
    CySCB_Type* scb_base;
    cy_en_scb_i2c_status_t status;
    cy_stc_scb_i2c_context_t* context;

    if ( i2c == NULL )
    {
        return PLATFORM_ERROR;
    }

    scb_base = i2c->scb->scb_base;
    context  = &scb_context[i2c->port];

    if ( scb_base == NULL )
    {
        return PLATFORM_ERROR;
    }


    for ( i = 0 ; i < length ; i++ )
    {
        /* Transmit the data */
        status = Cy_SCB_I2C_MasterWriteByte( scb_base, data_out[i], I2C_PDL_WRITE_TIMEOUT, context );

        if ( status != CY_SCB_I2C_SUCCESS )
        {
            return PLATFORM_ERROR;
        }
    }

    return PLATFORM_SUCCESS;
}


static platform_result_t platform_i2c_readbytes( const platform_i2c_t* i2c, uint8_t* data_in,
                                                 uint16_t length, int last )
{
    uint16_t i = 0;
    CySCB_Type* scb_base;
    cy_en_scb_i2c_status_t status;
    cy_stc_scb_i2c_context_t* context;

    /* Setting ackNak = TRUE */
    uint32_t acknak = CY_SCB_I2C_ACK;

    if ( i2c == NULL )
    {
        return PLATFORM_ERROR;
    }

    scb_base = i2c->scb->scb_base;
    context  = &scb_context[i2c->port];

    if ( scb_base == NULL )
    {
        return PLATFORM_ERROR;
    }

    while ( i < length )
    {

        if ( last && ( i == ( length - 1 ) )  )
        {
           acknak = CY_SCB_I2C_NAK;
        }

        status = Cy_SCB_I2C_MasterReadByte( scb_base, acknak,
                    &data_in[i++], I2C_PDL_READ_TIMEOUT, context );

        if ( status != CY_SCB_I2C_SUCCESS )
        {
            return PLATFORM_ERROR;
        }
    }

    return PLATFORM_SUCCESS;
}

static platform_result_t platform_psoc_i2c_write( const platform_i2c_t* i2c, const platform_i2c_config_t* config,
                                                  uint8_t flags, uint8_t* data_out, uint16_t length )
{
    platform_result_t ret;
    uint8_t address;

    if ( data_out == NULL || length == 0 )
    {
        return PLATFORM_BADARG;
    }

    address = config->address;

    /* Start Condition */
    if ( ( flags & I2C_START_FLAG ) != 0 )
    {
        ret = platform_psoc_i2c_start( i2c, address, I2C_MASTER_WRITE );
        if ( ret != PLATFORM_SUCCESS )
        {
            platform_psoc_i2c_stop( i2c );
            return PLATFORM_ERROR;
        }
    }

    /* Write the data */
    ret = platform_i2c_writebytes( i2c, data_out, length );

    /* Stop Condition */
    if ( ( ret != PLATFORM_SUCCESS ) || (( flags & I2C_STOP_FLAG ) != 0 ) )
    {
        platform_psoc_i2c_stop( i2c );
    }

    if ( ret != PLATFORM_SUCCESS )
    {
        return PLATFORM_ERROR;
    }

    return PLATFORM_SUCCESS;
}

static platform_result_t platform_psoc_i2c_read( const platform_i2c_t* i2c, const platform_i2c_config_t* config,
                                                 uint8_t flags, uint8_t* data_in, uint16_t length )
{
    platform_result_t ret;
    uint8_t address;

    if ( data_in == NULL || length == 0 )
    {
        return PLATFORM_BADARG;
    }

    address = config->address;

    /* Start Condition */
    if ( ( flags & I2C_START_FLAG ) != 0 )
    {
        ret = platform_psoc_i2c_start( i2c, address, I2C_MASTER_READ );
        if ( ret != PLATFORM_SUCCESS )
        {
            platform_psoc_i2c_stop( i2c );
            return PLATFORM_ERROR;
        }
    }

    /* Read the data */
    ret = platform_i2c_readbytes( i2c, data_in, length, ( ( flags & I2C_STOP_FLAG ) != 0 ) ? 1 : 0 );

    /* Stop Condition */
    if ( ( ret != PLATFORM_SUCCESS ) || ( ( flags & I2C_STOP_FLAG ) != 0 ) )
    {
        platform_psoc_i2c_stop( i2c );
    }

    if ( ret != PLATFORM_SUCCESS )
    {
        return PLATFORM_ERROR;
    }

    return PLATFORM_SUCCESS;
}

static platform_result_t platform_psoc_i2c_transfer( const platform_i2c_t* i2c, const platform_i2c_config_t* config,
                                                     uint16_t flags, void* buffer, uint16_t buffer_length, i2c_io_fn_t* fn )
{
    uint8_t i2c_flags = 0;

    if ( ( flags & ( WICED_I2C_START_FLAG | WICED_I2C_REPEATED_START_FLAG ) ) != 0 )
    {
        i2c_flags |= I2C_START_FLAG;
    }
    if ( ( flags & WICED_I2C_STOP_FLAG ) != 0 )
    {
        i2c_flags |= I2C_STOP_FLAG;
    }

    return ( *fn )( i2c, config, i2c_flags, buffer, buffer_length );
}


platform_result_t platform_i2c_init( const platform_i2c_t* i2c, const platform_i2c_config_t* config )
{
    platform_result_t result;

    wiced_assert( "bad argument", ( i2c != NULL ) && ( i2c_driver != NULL ) && ( config != NULL )
                  && ( config->flags & I2C_DEVICE_USE_DMA ) == 0);

    result = i2c_driver->init( i2c, config );

    return result;
}

platform_result_t platform_i2c_deinit( const platform_i2c_t* i2c, const platform_i2c_config_t* config )
{
    platform_result_t result;

    wiced_assert( "bad argument", ( i2c != NULL ) && ( i2c_driver != NULL ) && ( config != NULL ) );

    result = i2c_driver->deinit( i2c, config );

    return result;
}

wiced_bool_t platform_i2c_probe_device( const platform_i2c_t* i2c, const platform_i2c_config_t* config, int retries )
{
    uint32_t    i;
    uint8_t     dummy[2];
    platform_result_t result;

    /* Read two bytes from the addressed-slave. The slave-address won't be
     * acknowledged if it isn't on the I2C bus. The read won't trigger
     * a NACK from the slave (unless of error), since only the receiver can do that.
     */
    for ( i = 0; i < retries; i++ )
    {
        result = i2c_driver->read( i2c, config, I2C_START_FLAG | I2C_STOP_FLAG, dummy, sizeof dummy );

        if (  result == PLATFORM_SUCCESS )
        {
            return WICED_TRUE;
        }
    }

    return WICED_FALSE;
}

platform_result_t platform_i2c_init_tx_message( platform_i2c_message_t* message, const void* tx_buffer,
                                                uint16_t tx_buffer_length, uint16_t retries, wiced_bool_t disable_dma )
{
    wiced_assert( "bad argument", ( message != NULL ) && ( tx_buffer != NULL ) && ( tx_buffer_length != 0 ) );

    if ( disable_dma == WICED_FALSE )
    {
        return PLATFORM_UNSUPPORTED;
    }

    memset( message, 0x00, sizeof( *message ) );
    message->tx_buffer = tx_buffer;
    message->retries   = retries;
    message->tx_length = tx_buffer_length;
    message->flags     = 0;

    return PLATFORM_SUCCESS;
}

platform_result_t platform_i2c_init_rx_message( platform_i2c_message_t* message, void* rx_buffer,
                                                uint16_t rx_buffer_length, uint16_t retries, wiced_bool_t disable_dma )
{
    wiced_assert( "bad argument", ( message != NULL ) && ( rx_buffer != NULL ) && ( rx_buffer_length != 0 ) );

    if ( disable_dma == WICED_FALSE )
    {
        return PLATFORM_UNSUPPORTED;
    }

    memset( message, 0x00, sizeof( *message ) );

    message->rx_buffer = rx_buffer;
    message->retries   = retries;
    message->rx_length = rx_buffer_length;
    message->flags     = 0;

    return PLATFORM_SUCCESS;
}

platform_result_t platform_i2c_transfer( const platform_i2c_t* i2c, const platform_i2c_config_t* config,
                                         platform_i2c_message_t* messages, uint16_t number_of_messages )
{
    platform_result_t result = PLATFORM_SUCCESS;
    uint32_t          message_count;
    uint32_t          retries;

    /* Check for message validity. Combo message is unsupported */
    for ( message_count = 0; message_count < number_of_messages; message_count++ )
    {
        if ( messages[message_count].rx_buffer != NULL && messages[message_count].tx_buffer != NULL )
        {
            return PLATFORM_UNSUPPORTED;
        }

        if ( ( messages[message_count].tx_buffer == NULL && messages[message_count].tx_length != 0 )  ||
             ( messages[message_count].tx_buffer != NULL && messages[message_count].tx_length == 0 )  ||
             ( messages[message_count].rx_buffer == NULL && messages[message_count].rx_length != 0 )  ||
             ( messages[message_count].rx_buffer != NULL && messages[message_count].rx_length == 0 )  )
        {
            return PLATFORM_BADARG;
        }

        if ( messages[message_count].tx_buffer == NULL && messages[message_count].rx_buffer == NULL )
        {
            return PLATFORM_BADARG;
        }
    }

    for ( message_count = 0; message_count < number_of_messages; message_count++ )
    {
        for ( retries = 0; retries < messages[message_count].retries; retries++ )
        {
            if ( messages[message_count].tx_length != 0 )
            {
                result = i2c_driver->write( i2c, config, I2C_START_FLAG | I2C_STOP_FLAG,
                                            (uint8_t*) messages[message_count].tx_buffer,
                                            messages[message_count].tx_length );

                if ( result == PLATFORM_SUCCESS )
                {
                    /* Transaction successful. Break from the inner loop and continue with the next message */
                    break;
                }
            }
            else if ( messages[message_count].rx_length != 0 )
            {
                result = i2c_driver->read( i2c, config, I2C_START_FLAG | I2C_STOP_FLAG,
                                           (uint8_t*) messages[message_count].rx_buffer,
                                            messages[message_count].rx_length );

                if ( result == PLATFORM_SUCCESS )
                {
                    /* Transaction successful. Break from the inner loop and continue with the next message */
                    break;
                }
            }
        }

        /* Check if retry is maxed out. If yes, return immediately */
        if ( retries == messages[message_count].retries && result != PLATFORM_SUCCESS )
        {
            result = PLATFORM_ERROR;
            break;
        }
    }

    return result;
}

platform_result_t platform_i2c_write( const platform_i2c_t* i2c, const platform_i2c_config_t* config,
                                      uint16_t flags, const void* buffer, uint16_t buffer_length )
{
    platform_result_t result;

    result = i2c_driver->transfer( i2c, config, flags, (void *) buffer, buffer_length, i2c_driver->write );

    return result;
}

platform_result_t platform_i2c_read( const platform_i2c_t* i2c, const platform_i2c_config_t* config,
                                     uint16_t flags, void* buffer, uint16_t buffer_length )
{
    platform_result_t result;

    result = i2c_driver->transfer( i2c, config, flags, (void *) buffer, buffer_length, i2c_driver->read );

    return result;
}
