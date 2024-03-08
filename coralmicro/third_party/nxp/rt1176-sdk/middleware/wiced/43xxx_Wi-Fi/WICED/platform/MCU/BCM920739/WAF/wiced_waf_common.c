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
 * Handle WAF (WICED Application Framework) common stuff.
 * Mainly:
 *      * Erasing Apps
 *      * Loading Apps
 *      * Setting to boot from certain App
 */
#include "wiced.h"
#include "wwd_assert.h"
#include "wiced_result.h"
#include "wiced_utilities.h"
#include "platform_dct.h"
#include "wiced_framework.h"
#include "wiced_apps_common.h"
#include "wiced_waf_common.h"
#include "wiced_dct_common.h"
#include "waf_platform.h"
#include "platform_peripheral.h"
#include "platform_resource.h"
#include "platform.h"
#include "platform_ocf.h"

/******************************************************
 *                      Macros
 ******************************************************/

/******************************************************
 *                    Constants
 ******************************************************/

#define PLATFORM_SFLASH_PERIPHERAL_ID  (0)

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
static wiced_result_t wiced_apps_get_physical_address( app_header_t *app_header, uint32_t offset, uint32_t* address, uint32_t* size );
/******************************************************
 *               Variables Definitions
 ******************************************************/

/******************************************************
 *               Function Definitions
 ******************************************************/

extern wiced_result_t platform_load_app (uint32_t source, uint32_t* startaddr);
extern void platform_start_app( uint32_t entry_point );

wiced_result_t wiced_waf_reboot( void )
{
    /* Reset request */
    platform_mcu_reset( );

    return WICED_SUCCESS;
}

#if (DCT_BOOTLOADER_SDK_VERSION < DCT_BOOTLOADER_SDK_3_1_2)
    /* this SDK does not have apps_locations in bootloader_dct_header_t (platform_dct_header_t for the SDK) */
#else

wiced_result_t wiced_waf_app_set_boot(uint8_t app_id, char load_once)
{
    boot_detail_t boot;

    boot.entry_point                 = 0;
    boot.load_details.load_once      = load_once;
    boot.load_details.valid          = 1;
    boot.load_details.destination.id = INTERNAL;

    if ( wiced_dct_get_app_header_location( app_id, &boot.load_details.source ) != WICED_SUCCESS )
    {
        return WICED_ERROR;
    }
//    printf ("%s: App Id [%d] location [0x%x]\n",__func__,app_id,(unsigned int)boot.load_details.source.detail.external_fixed.location);
    if ( wiced_dct_write_boot_details( &boot ) != WICED_SUCCESS)
        return WICED_ERROR;

    return WICED_SUCCESS;
}


wiced_result_t wiced_waf_app_open( uint8_t app_id, wiced_app_t* app )
{
    if ( wiced_dct_get_app_header_location( app_id, &app->app_header_location ) != WICED_SUCCESS )
    {
        return WICED_ERROR;
    }
    app->offset = 0x00000000;
    app->app_id = app_id;
    app->last_erased_sector = 0xFFFFFFFF;

    return WICED_SUCCESS;
}

wiced_result_t wiced_waf_app_close( wiced_app_t* app )
{
    (void) app;
    return WICED_SUCCESS;
}

wiced_result_t wiced_waf_app_erase( wiced_app_t* app )
{
    UNUSED_PARAMETER(app);

    return WICED_ERROR;
}

wiced_result_t wiced_waf_app_get_size( wiced_app_t* app, uint32_t* size )
{
    return wiced_apps_get_size( &app->app_header_location, size );
}

wiced_result_t wiced_waf_app_set_size(wiced_app_t* app, uint32_t size)
{
    if ( wiced_apps_set_size( &app->app_header_location, size ) != WICED_SUCCESS )
    {
        return WICED_ERROR;
    }
    return wiced_dct_set_app_header_location( app->app_id, &app->app_header_location );
}


wiced_result_t wiced_waf_app_write_chunk( wiced_app_t* app, const uint8_t* data, uint32_t size)
{
    UNUSED_PARAMETER (app);
    UNUSED_PARAMETER (data);
    UNUSED_PARAMETER (size);

    /* TODO: */
    return WICED_ERROR;
}

wiced_result_t wiced_waf_app_read_chunk( wiced_app_t* app, uint32_t offset, uint8_t* data, uint32_t size )
{
    UNUSED_PARAMETER (app);
    UNUSED_PARAMETER (offset);
    UNUSED_PARAMETER (data);
    UNUSED_PARAMETER (size);

    /* TODO: */
    return WICED_ERROR;
}


static wiced_result_t wiced_apps_get_physical_address( app_header_t *app_header, uint32_t offset, uint32_t* address, uint32_t* size )
{
    uint8_t  index;
    uint32_t current_offset = 0;

    for ( index = 0; index < app_header->count; index++ )
    {
        uint32_t current_size = (uint32_t)app_header->sectors[ index ].count * SECTOR_SIZE;

#ifndef BOOTLOADER_APP_LUT_NO_SECURE_FLAG
        if ( app_header->secure )
        {
            /* If secure sector, reduce the metadata_size from total size */
//            current_size -= SECURE_SFLASH_METADATA_SIZE( current_size );
        }
#endif
        if ( ( offset >= current_offset ) && ( offset < ( current_offset + current_size ) ) )
        {
            uint32_t diff = offset - current_offset;

#ifndef BOOTLOADER_APP_LUT_NO_SECURE_FLAG
            if ( app_header->secure )
            {
                /* Get the Secure address */
//                *address = ( app_header->sectors[ index ].start * OCF_SECTOR_SIZE ) + SECURE_SECTOR_ADDRESS( diff ) + OFFSET_WITHIN_SECURE_SECTOR( diff );
                *address = 0;
            }
            else
#endif
            {
                *address = (uint32_t)app_header->sectors[ index ].start * SECTOR_SIZE + diff;
            }

            *size = current_size - diff;
            return WICED_SUCCESS;
        }
        current_offset += current_size;
    }
    return WICED_ERROR;
}


/*
 * Reading the image from ocf location and loads it in RAM
 * app_header_location - input. Here the location address is LUT item address. So we have to read the app details from LUT and loads it.
 * destination - output
 * */
wiced_result_t wiced_waf_app_load( const image_location_t* app_header_location, uint32_t* entrypoint )
{
    wiced_result_t       result = WICED_BADARG;

    if ( app_header_location->id == EXTERNAL_FIXED_LOCATION )
    {
        app_header_t    app_header;
        uint32_t        address=0, available_size=0;

        /* Read the image location from LUT */
        WICED_VERIFY( ocf_buffer_read( app_header_location->detail.external_fixed.location, (uint8_t*)&app_header, sizeof(app_header_t) ) );

        /* TODO: process multiple sectors*/
        if (app_header.count > 0)
        {
            wiced_apps_get_physical_address (&app_header, 0, &address, &available_size );

//            printf ("App physical location : 0x%x\r\n",(unsigned int)address);

            platform_load_app(address,entrypoint);

            /* Handling only 1st sector */
        }

        result = WICED_SUCCESS;
    }
    else
    {
        /*  Not supported */
        *entrypoint = 0;
        result = WICED_ERROR;
    }

    return result;
}

void wiced_waf_start_app( uint32_t entry_point )
{
    platform_start_app( entry_point );
}
#endif

wiced_result_t wiced_waf_check_factory_reset( void )
{
    /* TODO: */
    return WICED_ERROR;
}

uint32_t wiced_waf_get_button_press_time( int button_index, int led_index, uint32_t max_time )
{
    UNUSED_PARAMETER(button_index);
    UNUSED_PARAMETER(led_index);
    UNUSED_PARAMETER(max_time);

    /* TODO: */
    return WICED_ERROR;
}


uint32_t wiced_waf_get_factory_reset_button_time( void )
{
    return platform_get_factory_reset_button_time( PLATFORM_FACTORY_RESET_TIMEOUT );
}
