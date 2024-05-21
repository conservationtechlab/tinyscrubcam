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
 * OCF Read,write,erase implementation
 */
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "wiced.h"
#include "wiced_rtos.h"
#include "wiced_bt_trace.h"

/******************************************************
 *                      Macros
 ******************************************************/
#define OCF_BUFFER_SIZE 4
#define OCF_READ_BOUNDARY 4
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
extern int32_t ef_read_buffer_in_byte( uint32_t offset, uint8_t* buffer, uint32_t length );
extern wiced_result_t wiced_hal_eflash_write( uint32_t offset, uint8_t* p_buffer, uint32_t length );
extern wiced_result_t wiced_hal_eflash_read( uint32_t offset, uint8_t* p_buffer, uint32_t length );
/******************************************************
 *               Variable Definitions
 ******************************************************/
static uint8_t local_rd_buffer[OCF_BUFFER_SIZE];
static uint8_t local_wr_buffer[OCF_BUFFER_SIZE];
/******************************************************
 *               Function Definitions
 ******************************************************/

wiced_result_t ocf_buffer_read( uint32_t start_address, uint8_t* data, uint32_t size )
{
    wiced_result_t ret;
    int32_t result;
    uint32_t read_address = start_address;
    uint32_t part_read;
    uint32_t modified_size = size;
    uint32_t adjusted_size;
    uint32_t shifted_addr;
    uint8_t *write_ptr = data;

    if ( !data || !size )
    {
        WPRINT_PLATFORM_ERROR(("Bad arg\n"));
        return WICED_BADARG;
    }

    // handle un-aligned access
    part_read = start_address % OCF_READ_BOUNDARY;
    if (part_read)
    {

        shifted_addr = start_address - part_read;

        result = ef_read_buffer_in_byte((uint32_t)shifted_addr, local_rd_buffer, OCF_READ_BOUNDARY);
        if (result)
        {
            WPRINT_PLATFORM_ERROR(("%s %d: Error reading %d !!\n", __func__,__LINE__,(int)result));
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
        /* check un-aligned size */
        part_read = modified_size % OCF_READ_BOUNDARY;
        if (part_read)
        {
            if (part_read != modified_size)
                adjusted_size = modified_size - part_read;
            else
                adjusted_size = 0; // handled by part_read
        }

        else
            adjusted_size = modified_size;

        if (adjusted_size)
        {
            result = ef_read_buffer_in_byte((uint32_t)read_address, write_ptr, (uint32_t)adjusted_size);
            if (result)
            {
                WPRINT_PLATFORM_ERROR(("%s %d: Error reading %d !!\n", __func__,__LINE__,(int)result));
                ret = WICED_ERROR;
                goto ret_func;
            }
            read_address =  read_address + adjusted_size;
            write_ptr = write_ptr + adjusted_size;
        }

        if (part_read)
        {
            result = ef_read_buffer_in_byte((uint32_t)read_address, local_rd_buffer, OCF_READ_BOUNDARY);
            if (result)
            {
                WPRINT_PLATFORM_ERROR(("%s %d: Error reading %d !!\n",__func__,__LINE__,(int)result));
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
    return ret;
}


wiced_result_t ocf_buffer_write( uint32_t start_address, uint8_t* data, uint32_t size )
{
    wiced_result_t ret;
    uint32_t write_address = start_address;
    uint32_t part_read;
    uint32_t modified_size = size;
    uint32_t adjusted_size;
    uint32_t shifted_addr;
    uint8_t *write_ptr = data;

    if ( !data || !size )
    {
        WPRINT_PLATFORM_ERROR(("Bad arg\n"));
        return WICED_BADARG;
    }

    /* 1. start address should be 4 byte aligned.*/
    /* 2. size should be 4 byte aligned */
    /* 3. start_address+size should not more than OCF boundary */

    // handle unaligned address
    part_read = start_address % OCF_READ_BOUNDARY;
    if (part_read)
    {
        /* Got a unaligned address */
        shifted_addr = start_address - part_read;
        ret = wiced_hal_eflash_read ((uint32_t)shifted_addr, local_wr_buffer, OCF_READ_BOUNDARY);
        if (ret != WICED_SUCCESS)
        {
            WPRINT_PLATFORM_ERROR(("Error reading !!\n"));
            ret = WICED_ERROR;
            goto ret_func;
        }
        else
        {
            if (size <=  (OCF_READ_BOUNDARY - part_read))
            {
                memcpy(&local_wr_buffer[part_read], write_ptr , (size_t)size);
                ret = wiced_hal_eflash_write(shifted_addr,local_wr_buffer,OCF_READ_BOUNDARY);

                goto ret_func; // Done. Exit function.
            }
            else
            {
                memcpy(&local_wr_buffer[part_read], write_ptr, (size_t)(OCF_READ_BOUNDARY - part_read));

                ret = wiced_hal_eflash_write(shifted_addr,local_wr_buffer,OCF_READ_BOUNDARY);

                if ( ret != WICED_SUCCESS )
                {
                    WPRINT_PLATFORM_ERROR(("Error writing !!\n"));
                    ret = WICED_ERROR;
                    goto ret_func;
                }
            }

        }

        write_ptr = write_ptr + (OCF_READ_BOUNDARY - part_read);
        write_address = shifted_addr + OCF_READ_BOUNDARY;
        modified_size = size - (OCF_READ_BOUNDARY - part_read);
    }

    /* At this point write address must be 4 byte aligned */
    if (modified_size)
    {
        /* check un-aligned size */
        part_read = modified_size % OCF_READ_BOUNDARY;
        if (part_read )
        {
            /* size is unaligned */
            if (part_read != modified_size)
            {
                /* make adjusted_size to 4bytes aligned */
                /* part_read is less than 4 bytes */
                adjusted_size = modified_size - part_read;
            }
            else
            {
                /* Hare adjusted_size is less than 4 bytes */
                /* it will be handled in the last by part_read */
                adjusted_size = 0;
            }
        }
        else
        {
            /* size is aligned */
            adjusted_size = modified_size;
        }

        if (adjusted_size)
        {
            ret = wiced_hal_eflash_write(write_address,write_ptr,adjusted_size);
            if ( ret != WICED_SUCCESS )
            {
                WPRINT_PLATFORM_ERROR(("Error reading !!\n"));
                ret = WICED_ERROR;
                goto ret_func;
            }

        }

        write_address =  write_address + adjusted_size;
        write_ptr = write_ptr + adjusted_size;
        if (part_read)
        {
            ret = wiced_hal_eflash_read ((uint32_t)write_address, local_wr_buffer, OCF_READ_BOUNDARY);
            if (ret)
            {
                WPRINT_PLATFORM_ERROR(("Error reading\n"));
                ret = WICED_ERROR;
                goto ret_func;
            }
            else
            {

                memcpy(local_wr_buffer, write_ptr, (size_t)( part_read));

                ret = wiced_hal_eflash_write(write_address, local_wr_buffer, OCF_READ_BOUNDARY);

                if ( ret != WICED_SUCCESS )
                {
                    WPRINT_PLATFORM_ERROR(("Error reading !!\n"));
                    ret = WICED_ERROR;
                    goto ret_func;
                }
            }
        }
    }
    ret = WICED_SUCCESS;

ret_func:
    return ret;
}

