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
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "platform_mcu_peripheral.h"
#include "wiced_rtos.h"
#include "data_types.h"
#include "wwd_debug.h"

/******************************************************
 *                      Macros
 ******************************************************/

/******************************************************
 *                    Constants
 ******************************************************/
#define OCF_SEM_TIMEOUT 10000
#define OCF_BUFFER_SIZE 4
#define OCF_READ_BOUNDARY 4
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
extern int32_t ef_read_buffer_in_byte(uint32_t offset, uint8_t* buffer, uint32_t length);
extern int32_t ef_write_buffer_in_word(uint32_t offset, uint8_t* buffer, uint32_t length);
extern int32_t ef_erase_multiple_pages(uint32_t offset, uint32_t length);


platform_result_t platform_filesystem_init( void );

/******************************************************
 *               Variables Definitions
 ******************************************************/
static wiced_semaphore_t ocf_mutex;
static  uint8_t       fs_init_done = 0;
static uint8_t local_rd_buffer[OCF_BUFFER_SIZE];

/******************************************************
 *               Function Definitions
 ******************************************************/
 platform_result_t platform_filesystem_init( void )
{
    platform_result_t ret = PLATFORM_SUCCESS;
    if ( fs_init_done == 0)
    {
        if (wiced_rtos_init_semaphore( &ocf_mutex ))
        {
            WPRINT_PLATFORM_ERROR((" Failed to initialize semaphore\n"));
            return PLATFORM_ERROR;
        }
        else
        {
            ret = PLATFORM_SUCCESS;
            fs_init_done = 1;
        }

        if ( wiced_rtos_set_semaphore( &ocf_mutex ) )
        {
            WPRINT_PLATFORM_ERROR((" Failed to set semaphore\n"));
            ret = PLATFORM_ERROR;
        }
    }
    return ret;
}

static wiced_result_t ocf_block_device_init( wiced_block_device_t* device, wiced_block_device_write_mode_t write_mode )
{

    ocf_block_device_specific_data_t* osf_specific_data = (ocf_block_device_specific_data_t*) device->device_specific_data;

    if (wiced_rtos_get_semaphore( &ocf_mutex, OCF_SEM_TIMEOUT ))
    {
        WPRINT_PLATFORM_ERROR((" Failed to get semaphore\n"));
        return WICED_ERROR;
    }

    if ( device->init_data->base_address_offset == (uint64_t)-1 )
    {
        osf_specific_data->offset = WICED_FILESYSTEM_OCF_START;
    }
    else if ( (uint32_t)device->init_data->base_address_offset <  WICED_FILESYSTEM_OCF_END)
    {
        osf_specific_data->offset = (uint32_t)device->init_data->base_address_offset;
    }
    else {
        if ( wiced_rtos_set_semaphore( &ocf_mutex ) )
        {
            WPRINT_PLATFORM_ERROR((" Failed to set semaphore\n"));
            return WICED_ERROR;
        }
        return WICED_BADARG;
    }

    osf_specific_data->write_mode = write_mode;

    device->initialized = WICED_TRUE;


    if ( wiced_rtos_set_semaphore( &ocf_mutex ) )
    {
        WPRINT_PLATFORM_ERROR((" Failed to set semaphore\n"));
        return WICED_ERROR;
    }

    return WICED_SUCCESS;
}

static wiced_result_t ocf_block_device_deinit( wiced_block_device_t* device )
{
    if ( wiced_rtos_get_semaphore( &ocf_mutex, OCF_SEM_TIMEOUT ))
    {
        WPRINT_PLATFORM_ERROR((" Failed to get semaphore\n"));
        return WICED_ERROR;
    }

    device->initialized = WICED_FALSE;

    if ( wiced_rtos_set_semaphore( &ocf_mutex ) )
    {
        WPRINT_PLATFORM_ERROR((" Failed to set semaphore\n"));
        return WICED_ERROR;
    }

    return WICED_SUCCESS;
}


static wiced_result_t ocf_block_flush( wiced_block_device_t* device )
{
    UNUSED_PARAMETER( device );
    return WICED_SUCCESS;
}

static wiced_result_t ocf_block_read( wiced_block_device_t* device, uint64_t start_address, uint8_t* data, uint64_t size )
{
    wiced_result_t ret;
    int32_t result;
    uint64_t read_address = start_address;
    uint64_t part_read;
    uint64_t modified_size = size;
    uint64_t adjusted_size;
    uint64_t shifted_addr;
    uint8_t *write_ptr = data;

    if ( !data || !size || !device )
    {
        WPRINT_PLATFORM_ERROR(("Bad arg\n"));
        return WICED_BADARG;
    }

    if ( (start_address + size) > (device->init_data->base_address_offset + device->device_size) )
    {
        WPRINT_PLATFORM_ERROR(("Bad arg\n"));
        return WICED_BADARG;
    }

    if ( wiced_rtos_get_semaphore( &ocf_mutex, OCF_SEM_TIMEOUT ))
    {
        WPRINT_PLATFORM_ERROR((" Failed to get semaphore\n"));
        return WICED_ERROR;
    }

    // handle un-aligned access
    part_read = start_address % OCF_READ_BOUNDARY;
    if (part_read)
    {

        shifted_addr = start_address - part_read;

        result = ef_read_buffer_in_byte((uint32_t)shifted_addr, local_rd_buffer, OCF_READ_BOUNDARY);
        if (result)
        {
            WPRINT_PLATFORM_ERROR(("Error reading %d !!\n", (int)result));
            ret = WICED_ERROR;
            goto ret_func;
        }
        else
        {
            if (size <=  (OCF_READ_BOUNDARY - part_read))
            {
                memcpy(write_ptr, &local_rd_buffer[part_read], (size_t)size);
                ret = WICED_SUCCESS;
                goto ret_func;
            }
            else
                memcpy(write_ptr, &local_rd_buffer[part_read], (size_t)(OCF_READ_BOUNDARY - part_read));
        }
        write_ptr = write_ptr + (OCF_READ_BOUNDARY - part_read);
        read_address = shifted_addr + OCF_READ_BOUNDARY;
        modified_size = size - (OCF_READ_BOUNDARY - part_read);
    }

    if (modified_size)
    {

        part_read = modified_size % OCF_READ_BOUNDARY;
        if (part_read && (part_read != modified_size))
            adjusted_size = modified_size - part_read;
        else
            adjusted_size = modified_size;

        result = ef_read_buffer_in_byte((uint32_t)read_address, write_ptr, (uint32_t)adjusted_size);
        if (result)
        {
            WPRINT_PLATFORM_ERROR(("Error reading %d !!\n", (int)result));
            ret = WICED_ERROR;
            goto ret_func;
        }
        read_address =  read_address + adjusted_size;
        write_ptr = write_ptr + adjusted_size;
        if (part_read)
        {
            result = ef_read_buffer_in_byte((uint32_t)read_address, local_rd_buffer, OCF_READ_BOUNDARY);
            if (result)
            {
                WPRINT_PLATFORM_ERROR(("Error reading %d !!\n", (int)result));
                ret = WICED_ERROR;
                goto ret_func;
            }
            else
            {
                memcpy(write_ptr, local_rd_buffer, (size_t)( part_read));
            }
        }
    }
    ret = WICED_SUCCESS;

ret_func:
    if ( wiced_rtos_set_semaphore( &ocf_mutex ) )
    {
        WPRINT_PLATFORM_ERROR((" Failed to set semaphore\n"));
        ret = WICED_ERROR;
    }
    return ret;
}

static wiced_result_t ocf_block_register_callback( wiced_block_device_t* device, wiced_block_device_status_change_callback_t callback )
{
    UNUSED_PARAMETER( device );
    UNUSED_PARAMETER( callback );
    return WICED_SUCCESS;
}

static wiced_result_t ocf_block_status( wiced_block_device_t* device, wiced_block_device_status_t* status )
{
    UNUSED_PARAMETER( device );
    *status = BLOCK_DEVICE_UP_READ_WRITE;
    return WICED_SUCCESS;
}

const wiced_block_device_driver_t ocf_block_device_driver =
{
    .init                = ocf_block_device_init,
    .deinit              = ocf_block_device_deinit,
    .erase               = NULL,
    .write               = NULL,
    .flush               = ocf_block_flush,
    .read                = ocf_block_read,
    .register_callback   = ocf_block_register_callback,
    .status              = ocf_block_status,
};
