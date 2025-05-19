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
#include "platform.h"
#include "platform_peripheral.h"
#include "wwd_rtos.h"
#include "wwd_assert.h"
#include "RTOS/wwd_rtos_interface.h"
#include "wiced_hal_i2c.h"
#include "20719mapb0.h"
#include "brcm_fw_types.h"
#include "i2cm.h"
/******************************************************
 *                      Macros
 ******************************************************/
#define DATA_LENGTH_MAX                 ( 128 )

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

static platform_result_t i2c_write    ( const platform_i2c_t* i2c, const platform_i2c_config_t* config, platform_i2c_message_t* message);
static platform_result_t i2c_read     ( const platform_i2c_t* i2c, const platform_i2c_config_t* config, platform_i2c_message_t* message);

/******************************************************
 *               Variable Definitions
 ******************************************************/

/******************************************************
 *               Function Definitions
 ******************************************************/

platform_result_t platform_i2c_init( const platform_i2c_t* i2c, const platform_i2c_config_t* config )
{
    uint8_t i2c_speed;

    /*if GPIO info is NULL,this I2C ch is not usable
     * pin-muxed for other use*/
    if (i2c->pin_scl == NULL )
        return PLATFORM_INIT_FAIL;

    if ((wiced_i2c_t)i2c->bus_id >= WICED_I2C_MAX)
        return PLATFORM_BADARG;
    if(config->speed_mode == I2C_LOW_SPEED_MODE)
        i2c_speed = I2CM_SPEED_100KHZ;
    else if (config->speed_mode == I2C_STANDARD_SPEED_MODE)
        i2c_speed = I2CM_SPEED_400KHZ;
    else if (config->speed_mode == I2C_HIGH_SPEED_MODE)
        i2c_speed = I2CM_SPEED_800KHZ;
    else
        return PLATFORM_UNSUPPORTED;
    if (config->address_width != I2C_ADDRESS_WIDTH_7BIT)
       return PLATFORM_UNSUPPORTED;
    /*  Configure I2C SCL,SDA pin mux */
    /* setup output mux select */
    REG32(iocfg_p0_adr+(4*i2c->pin_scl->pin_number)) = 0;
    REG32(iocfg_p0_adr+(4*i2c->pin_sda->pin_number)) = 0;
    REG32(iocfg_fcn_p0_adr+(4*i2c->pin_scl->pin_number)) = i2c->pin_scl->mux_mode; // SCL mux mode
    REG32(iocfg_fcn_p0_adr+(4*i2c->pin_sda->pin_number)) = i2c->pin_sda->mux_mode; // SDA mux mode
    /*set up input mux select*/
    iocfg_premux_5 &= ~0xFF000000;
    iocfg_premux_5 |= (uint32_t)((i2c->pin_scl->pin_number+1)<<24);
    iocfg_premux_6 &= ~0x000000FF;
    iocfg_premux_6 |= (uint8_t)(i2c->pin_sda->pin_number+1);

    cr_pad_fcn_ctl1 |= 0x1;
    /* HAL has void return for the below functions*/
    wiced_hal_i2c_init();
    wiced_hal_i2c_set_speed(i2c_speed);

    return PLATFORM_SUCCESS;
}

platform_result_t platform_i2c_deinit( const platform_i2c_t* i2c, const platform_i2c_config_t* config )
{
    UNUSED_PARAMETER( i2c );
    UNUSED_PARAMETER( config );
    return PLATFORM_SUCCESS;
}

/* Not Supported */
wiced_bool_t platform_i2c_probe_device( const platform_i2c_t* i2c, const platform_i2c_config_t* config, int retries )
{
    UNUSED_PARAMETER( i2c );

    if (config->address_width != I2C_ADDRESS_WIDTH_7BIT)
       return PLATFORM_UNSUPPORTED;
    do
    {
        if(WICED_FALSE == i2cm_pingSlave((uint8_t)config->address))
        {
            if(retries!=0)
                retries--;
        }
        else
        {
            return WICED_TRUE;
        }
    }while(retries);
    return WICED_FALSE;
}

platform_result_t platform_i2c_transfer( const platform_i2c_t* i2c, const platform_i2c_config_t* config, platform_i2c_message_t* messages, uint16_t number_of_messages )
{
    platform_result_t result = PLATFORM_SUCCESS;
    uint32_t          message_count;
    uint32_t          retries;

    /* Check for message validity. Combo message is unsupported */
    for ( message_count = 0; message_count < number_of_messages; message_count++ )
    {
        if ( messages[message_count].tx_buffer == NULL && messages[message_count].rx_buffer == NULL )
        {
            return PLATFORM_BADARG;
        }

        if ( messages[ message_count ].tx_length > DATA_LENGTH_MAX || messages[ message_count ].rx_length > DATA_LENGTH_MAX )
        {
            return PLATFORM_BADARG;
        }
    }

    for ( message_count = 0; message_count < number_of_messages; message_count++ )
    {
        for ( retries = 0; retries < messages[message_count].retries; retries++ )
        {
            /* First check for Read buffer. It might be combined message for reading from a register */
            if ( messages[message_count].rx_length != 0 )
            {
                result = i2c_read( i2c, config, &messages[message_count] );
                if ( result == PLATFORM_SUCCESS )
                {
                    /* Transaction successful. Break from the inner loop and continue with the next message */
                    break;
                }
            }
            else if ( messages[message_count].tx_length != 0 )
            {
                result = i2c_write( i2c, config, &messages[message_count] );
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
            return result;
        }
    }

    return PLATFORM_SUCCESS;
}

platform_result_t platform_i2c_init_tx_message( platform_i2c_message_t* message, const void* tx_buffer, uint16_t tx_buffer_length, uint16_t retries, wiced_bool_t disable_dma )
{
    wiced_assert( "bad argument", ( message != NULL ) && ( tx_buffer != NULL ) && ( tx_buffer_length != 0 ) );

    /* 4390 I2C does not support DMA */
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

platform_result_t platform_i2c_init_rx_message( platform_i2c_message_t* message, void* rx_buffer, uint16_t rx_buffer_length, uint16_t retries, wiced_bool_t disable_dma )
{
    wiced_assert( "bad argument", ( message != NULL ) && ( rx_buffer != NULL ) && ( rx_buffer_length != 0 ) );

    /* 4390 I2C does not support DMA */
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

/* Combined message can be used to read from a register, as it involves both write and read on I2C */
platform_result_t platform_i2c_init_combined_message( platform_i2c_message_t* message, const void* tx_buffer, void* rx_buffer, uint16_t tx_buffer_length, uint16_t rx_buffer_length, uint16_t retries, wiced_bool_t disable_dma )
{
    wiced_assert( "bad argument", ( message != NULL ) && ( tx_buffer != NULL ) && ( tx_buffer_length != 0 ) && ( rx_buffer != NULL ) && ( rx_buffer_length != 0 ) );

    /* 4390 I2C does not support DMA */
    if ( disable_dma == WICED_FALSE )
    {
        return PLATFORM_UNSUPPORTED;
    }

    memset( message, 0x00, sizeof( *message ) );

    message->rx_buffer = rx_buffer;
    message->tx_buffer = tx_buffer;
    message->retries   = retries;
    message->tx_length = tx_buffer_length;
    message->rx_length = rx_buffer_length;
    message->flags     = 0;

    return PLATFORM_SUCCESS;
}

/* Writes on I2C. The caller is expected to put the register id as the first element of the Tx buffer */
platform_result_t platform_i2c_write( const platform_i2c_t* i2c, const platform_i2c_config_t* config, uint16_t flags, const void* buffer, uint16_t buffer_length )
{
    uint8_t status = I2CM_SUCCESS;

     if (config->address_width != I2C_ADDRESS_WIDTH_7BIT)
        return PLATFORM_UNSUPPORTED;

    /* Transmit on I2C */
    status = wiced_hal_i2c_write((uint8_t*)buffer, buffer_length, (uint8_t)config->address );

    wiced_assert("i2c write failed",(status == I2CM_SUCCESS));

   return PLATFORM_SUCCESS;

}

/* Reads from I2C. The caller is expected to write register address on I2C before calling this read */
platform_result_t platform_i2c_read( const platform_i2c_t* i2c, const platform_i2c_config_t* config, uint16_t flags, void* buffer, uint16_t buffer_length )
{
    uint8_t status = I2CM_SUCCESS;

    if (config->address_width != I2C_ADDRESS_WIDTH_7BIT)
       return PLATFORM_UNSUPPORTED;

    /* Transmit on I2C: The register from where value is to be read
     * must have been written on the bus in previous transaction
     *  */
    status = wiced_hal_i2c_read((uint8_t*)buffer, buffer_length,config->address);

    wiced_assert("i2c read failed",status == I2CM_SUCCESS);

   return PLATFORM_SUCCESS;
}

/* This function can handle the combined messages intending to read from a register. The message
 * would have register address in Tx buffer.
*/
static platform_result_t i2c_read( const platform_i2c_t* i2c, const platform_i2c_config_t* config, platform_i2c_message_t* message)
{
    uint8_t status = I2CM_SUCCESS;
    uint8_t raddr = 0;
    uint8_t rsize = 0;

    if (config->address_width != I2C_ADDRESS_WIDTH_7BIT)
        return PLATFORM_UNSUPPORTED;

    /* Get register address from message
     * for a read message we only expect
     * reg address in tx msg   */
    if( message->tx_length == 1 )
    {
        raddr = ((uint8_t*)message->tx_buffer)[0];
        rsize =  message->tx_length;
    }
    else
    {
        return PLATFORM_BADARG;
    }

    /* I2c Protocol: To read from register, first register address is written
     * on the bus so this function would have both write and read
     */
     status = wiced_hal_i2c_write(&raddr, rsize, (uint8_t)config->address );
     wiced_assert("i2c read address write failed",status == I2CM_SUCCESS);
     status = wiced_hal_i2c_read((uint8_t*)message->rx_buffer, message->rx_length,config->address);
     wiced_assert("i2c read failed",status == I2CM_SUCCESS);

    return PLATFORM_SUCCESS;
}

/* This function handles Tx message. The Tx buffer would have first byte as the register address */
static platform_result_t i2c_write( const platform_i2c_t* i2c, const platform_i2c_config_t* config, platform_i2c_message_t* message)
{
    uint8_t status = I2CM_SUCCESS;

     if (config->address_width != I2C_ADDRESS_WIDTH_7BIT)
        return PLATFORM_UNSUPPORTED;

    /* First byte of write buffer will have register address */
    status = wiced_hal_i2c_write((uint8_t*)message->tx_buffer, message->tx_length, (uint8_t)config->address);
    wiced_assert("i2c read address write failed",status == I2CM_SUCCESS);

    return PLATFORM_SUCCESS;
}

