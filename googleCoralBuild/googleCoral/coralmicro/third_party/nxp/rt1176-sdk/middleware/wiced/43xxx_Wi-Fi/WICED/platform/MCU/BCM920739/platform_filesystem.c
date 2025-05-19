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
 * Defines BCM20739 filesystem
 */
#include "stdio.h"
#include "platform_mcu_peripheral.h"
#include "wiced_rtos.h"
#include "data_types.h"
#include "serialflash.h"
#include "wiced.h"

/******************************************************
 *                      Macros
 ******************************************************/

/******************************************************
 *                    Constants
 ******************************************************/
#define SFLASH_SEM_TIMEOUT 10000
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
 *               Function Declarations
 ******************************************************/
platform_result_t platform_filesystem_init( void );
platform_result_t platform_sflash_spi_pin_config(void);

/******************************************************
 *               Variables Definitions
 ******************************************************/
wiced_semaphore_t sflash_mutex;
/* Init SPI port. CPHA = 0 and CPOL = 0 */
static  uint8_t       fs_init_done = 0;


/******************************************************
 *               Function Definitions
 ******************************************************/
 platform_result_t platform_filesystem_init( void )
{
    platform_result_t ret = PLATFORM_SUCCESS;
    if ( fs_init_done == 0)
    {
        if (wiced_rtos_init_semaphore( &sflash_mutex ))
        {
            printf(" Failed to initialize semaphore\n");
            return PLATFORM_ERROR;
        }

        if (platform_sflash_spi_pin_config( ) != PLATFORM_SUCCESS )
        {
            ret = PLATFORM_ERROR;
        }
        else
        {
            ret = PLATFORM_SUCCESS;
            fs_init_done = 1;
        }

        if ( wiced_rtos_set_semaphore( &sflash_mutex ) )
        {
            printf(" Failed to set semaphore\n");
            ret = PLATFORM_ERROR;
        }
    }
    return ret;
}

static wiced_result_t sflash_block_device_init( wiced_block_device_t* device, wiced_block_device_write_mode_t write_mode )
{

    sflash_block_device_specific_data_t* sflash_specific_data = (sflash_block_device_specific_data_t*) device->device_specific_data;

    if (wiced_rtos_get_semaphore( &sflash_mutex, SFLASH_SEM_TIMEOUT ))
    {
        printf(" Failed to get semaphore\n");
        return WICED_ERROR;
    }

    if ( device->init_data->base_address_offset == (uint64_t)-1 )
    {
        sflash_specific_data->offset = PLATFORM_SFLASH_FREE_OFFSET;
    }
    else if ( (uint32_t)device->init_data->base_address_offset < PLATFORM_SFLASH_SIZE)
    {
        sflash_specific_data->offset = (uint32_t)device->init_data->base_address_offset;
    }
    else {
        if ( wiced_rtos_set_semaphore( &sflash_mutex ) )
        {
            printf(" Failed to set semaphore\n");
            return WICED_ERROR;
        }
        return WICED_BADARG;
    }

    sflash_specific_data->write_mode = write_mode;

    device->initialized = WICED_TRUE;

    sflash_init( );

    if ( wiced_rtos_set_semaphore( &sflash_mutex ) )
    {
        printf(" Failed to set semaphore\n");
        return WICED_ERROR;
    }

    return WICED_SUCCESS;
}

static wiced_result_t sflash_block_device_deinit( wiced_block_device_t* device )
{
    if ( wiced_rtos_get_semaphore( &sflash_mutex, SFLASH_SEM_TIMEOUT ))
    {
        printf(" Failed to get semaphore\n");
        return WICED_ERROR;
    }

    device->initialized = WICED_FALSE;

    sflash_deinit();
    if ( wiced_rtos_set_semaphore( &sflash_mutex ) )
    {
        printf(" Failed to set semaphore\n");
        return WICED_ERROR;
    }

    return WICED_SUCCESS;
}

static wiced_result_t sflash_block_write( wiced_block_device_t* device, uint64_t start_address, const uint8_t* data, uint64_t size )
{
    wiced_result_t ret;

    if ( wiced_rtos_get_semaphore( &sflash_mutex, SFLASH_SEM_TIMEOUT ))
    {
        printf(" Failed to get semaphore\n");
        return WICED_ERROR;
    }

    if ( start_address + size > device->device_size )
    {
        ret = WICED_BADARG;
    }
    else if ( !sflash_write( (UINT32) start_address, (UINT8*)data, (UINT32) size ))
    {
        ret = WICED_ERROR;
    }
    else
    {
        ret = WICED_SUCCESS;
    }
    if ( wiced_rtos_set_semaphore( &sflash_mutex ) )
    {
        printf(" Failed to set semaphore\n");
        ret = WICED_ERROR;
    }
    return ret;
}

static wiced_result_t sflash_block_flush( wiced_block_device_t* device )
{
    UNUSED_PARAMETER( device );
    return WICED_SUCCESS;
}

static wiced_result_t sflash_block_read( wiced_block_device_t* device, uint64_t start_address, uint8_t* data, uint64_t size )
{
    wiced_result_t ret;
    (void) data;

    if ( wiced_rtos_get_semaphore( &sflash_mutex, SFLASH_SEM_TIMEOUT ))
    {
        printf(" Failed to get semaphore\n");
        return WICED_ERROR;
    }

    if ( start_address + size > device->device_size )
    {
        ret =  WICED_BADARG;
    }
    else if ( !sflash_read( (long unsigned int) start_address, data, (unsigned int) size ))
    {
        ret = WICED_ERROR;
    }
    else
    {
        ret = WICED_SUCCESS;
    }

    if ( wiced_rtos_set_semaphore( &sflash_mutex ) )
    {
        printf(" Failed to set semaphore\n");
        ret = WICED_ERROR;
    }
    return ret;
}

static wiced_result_t sflash_block_register_callback( wiced_block_device_t* device, wiced_block_device_status_change_callback_t callback )
{
    UNUSED_PARAMETER( device );
    UNUSED_PARAMETER( callback );
    return WICED_SUCCESS;
}

static wiced_result_t sflash_block_status( wiced_block_device_t* device, wiced_block_device_status_t* status )
{
    UNUSED_PARAMETER( device );
    *status = BLOCK_DEVICE_UP_READ_WRITE;
    return WICED_SUCCESS;
}

static wiced_result_t sflash_block_erase( wiced_block_device_t* device, uint64_t start_address, uint64_t size )
{
    wiced_result_t ret;

    if ( wiced_rtos_get_semaphore( &sflash_mutex, SFLASH_SEM_TIMEOUT ))
    {
        printf(" Failed to get semaphore\n");        return WICED_ERROR;
    }

    if ( start_address + size > device->device_size )
    {
        ret =  WICED_BADARG;
    }
    else if ( !sflash_erase( (long unsigned int) start_address, (unsigned int) size ))
    {
        ret = WICED_ERROR;
    }
    else
    {
        ret = WICED_SUCCESS;
    }

    if ( wiced_rtos_set_semaphore( &sflash_mutex ) )
    {
        printf(" Failed to set semaphore\n");
        ret = WICED_ERROR;
    }
    return ret;
}


const wiced_block_device_driver_t sflash_block_device_driver =
{
    .init                = sflash_block_device_init,
    .deinit              = sflash_block_device_deinit,
    .erase               = sflash_block_erase,
    .write               = sflash_block_write,
    .flush               = sflash_block_flush,
    .read                = sflash_block_read,
    .register_callback   = sflash_block_register_callback,
    .status              = sflash_block_status,
};


