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
 * Define default CY20739 initialisation functions
 */
#if defined ( IAR_TOOLCHAIN )
#include "platform_cmis.h"
#endif
#include <stdint.h>
#include <string.h>
#include "platform_config.h"
#include "platform_peripheral.h"
#include "platform_toolchain.h"
#include "platform_sleep.h"
#include "platform_assert.h"
#include "wwd_assert.h"
#include "wiced_defaults.h"
#include "platform_mcu_peripheral.h"
#include "platform_init.h"
#include "platform_dct.h"
#include "wiced_dct_common.h"

#include "bt_target.h"
#include "wiced_bt_dev.h"
#include "wiced_bt_ble.h"
#include "wiced_bt_gatt.h"
#include "wiced_bt_cfg.h"
#include "wiced_bt_stack.h"
#include "wiced_rtos.h"
#include "platform_bt_cfg.h"
#ifdef WICED_USE_PLATFORM_ALLOCATED_POOL
#include "platform_memory.h"
#endif

#if defined (USES_RESOURCE_GENERIC_FILESYSTEM)
#include "platform_resource.h"
#endif
/* for wiced_waf_reboot can be removed once implemented in wiced_waf_common.c*/
#include "wiced_waf_common.h"
#include "wiced_low_power.h"
#ifdef ENABLE_JLINK_TRACE
#include "SEGGER_RTT.h"
#endif

/******************************************************
 *                      Macros
 ******************************************************/

#ifdef USES_RESOURCE_GENERIC_FILESYSTEM
#if defined ( WICED_PLATFORM_INCLUDES_OCF_FS )
#define FILE_SYSTEM_NAME  "OCF"
#elif defined (WICED_PLATFORM_INCLUDES_SPI_FLASH)
#define FILE_SYSTEM_NAME  "SFLASH"
#else
#error Define valid file system name
#endif
#endif

#ifdef WICED_USE_PLATFORM_ALLOCATED_POOL

#define PLATFORM_MAX_RX_POOL_SIZE 12*1024
#define PLATFORM_MAX_TX_POOL_SIZE 12*1024

/*
  SCRATCH_MEMORY_START_ADDRESS : Reusing the unused scratch memory.

  We have implement a way to read from map file to find such
  unused slots and create a pool.
*/
#define SCRATCH_MEMORY_START_ADDRESS ((uint8_t *)0x0020b91c)

#endif

/******************************************************
 *                    Constants
 ******************************************************/

/******************************************************
 *                   Enumerations
 ******************************************************/

/******************************************************
 *                 Type Definitions
 ******************************************************/
extern const wiced_bt_cfg_settings_t WEAK wiced_bt_cfg_settings;
extern const wiced_bt_cfg_buf_pool_t WEAK wiced_bt_cfg_buf_pools[];

extern wiced_bt_cfg_settings_t platform_bt_cfg_settings;
extern wiced_bt_cfg_buf_pool_t platform_bt_cfg_buf_pools[];

#if defined ( USES_RESOURCE_GENERIC_FILESYSTEM )
extern const filesystem_list_t  all_filesystem_devices[];
#endif
/******************************************************
 *                    Structures
 ******************************************************/

/******************************************************
 *               Static Function Declarations
 ******************************************************/
extern void platform_mem_init( void );
extern void platform_init_security( void );
extern wiced_result_t wiced_bt_stack_init_rom(wiced_bt_management_cback_t *p_bt_management_cback,
                                   const wiced_bt_cfg_settings_t     *p_bt_cfg_settings,
                                   const wiced_bt_cfg_buf_pool_t     wiced_bt_cfg_buf_pools[]);
extern void application_start(void);
extern wiced_result_t wiced_bt_app_init( void );
void platform_application_start( void );
#ifdef WICED_SECURITY_ROM
typedef void* (*p_mpaf_cfa_memAlloc_replace)( unsigned int length );
typedef void (*p_mpaf_cfa_memFree_replace)(void *memoryBlock);
extern void mpaf_cfa_mmAlloc_replace( p_mpaf_cfa_memAlloc_replace reg_fun );
extern void mpaf_cfa_memFree_replace( p_mpaf_cfa_memFree_replace reg_fun );
void* tls_host_malloc_cb(unsigned int length);
void tls_host_free_cb( void *memoryBlock);
#endif
#if defined ( USES_RESOURCE_GENERIC_FILESYSTEM )
wiced_result_t platform_resource_fs_init( void );
#endif
extern void     bttransport_stop_polling_sense(void);
/******************************************************
 *               Variable Definitions
 ******************************************************/
wiced_thread_t app_thread;
wiced_bt_management_cback_t *app_management_cback = NULL;
wiced_bt_management_evt_data_t bt_enabled_event_data;
#if defined ( USES_RESOURCE_GENERIC_FILESYSTEM )
const filesystem_list_t* curr_item = all_filesystem_devices;
wiced_filesystem_t resource_fs_handle;
#endif

#ifdef WICED_USE_PLATFORM_ALLOCATED_POOL

uint8_t* platform_tx_buffer_pool_memory = SCRATCH_MEMORY_START_ADDRESS;
ALIGNED_PRE(4) uint8_t platform_rx_buffer_pool_memory [PLATFORM_MAX_RX_POOL_SIZE] ALIGNED(4);

#endif

/******************************************************
 *               Function Definitions
 ******************************************************/
void platform_mcu_reset( void )
{
    platform_watchdog_reset_system();
}

/* BCM920739 common clock initialization function
 * - weak attribute is intentional in case a specific platform (board) needs to override this.
 */
void platform_init_system_clocks( void )
{
    /* System clock initialisation is already done in ROM bootloader */
}

void platform_init_memory( void )
{

}

#ifdef WICED_SECURITY_ROM
void* tls_host_malloc_cb(unsigned int length)
{
    return malloc(length);

}

void tls_host_free_cb( void *memoryBlock)
{
    free(memoryBlock);
}
void platform_init_security( void )
{
    mpaf_cfa_mmAlloc_replace(tls_host_malloc_cb);
    mpaf_cfa_memFree_replace(tls_host_free_cb);
}
#endif

void platform_init_mcu_infrastructure( void )
{

#ifdef WICED_SECURITY_ROM
    platform_init_security();
#endif

    platform_pwm_clk_init();
    /* Initialise external serial flash */
//    platform_sflash_init();
    platform_rtc_init();
}

void platform_init_connectivity_module( void )
{

}

#if defined ( USES_RESOURCE_GENERIC_FILESYSTEM )
wiced_result_t platform_resource_fs_init ( void )
{
    wiced_result_t result;
    result = wiced_filesystem_init ( );
    if (result != WICED_SUCCESS) {
        WPRINT_PLATFORM_ERROR (("File system init failed\n"));
        return WICED_ERROR;
    }
    while ( curr_item->device != NULL )
    {
        if ( strcmp ( FILE_SYSTEM_NAME, curr_item->name ) == 0 )
        {
            result = wiced_filesystem_mount( curr_item->device, curr_item->type, &resource_fs_handle, curr_item->name );
            if ( result != WICED_SUCCESS )
            {
                WPRINT_PLATFORM_ERROR(( "Error mounting filesystem" ));
                return WICED_ERROR;
            }
            break;
        }
        curr_item++;
    }
    return result;
}
#endif

/*
 * Default platform bt/ble link management callback handler
 */
static wiced_result_t platform_bt_management_callback( wiced_bt_management_evt_t event, wiced_bt_management_evt_data_t *p_event_data )
{
    wiced_result_t result = WICED_SUCCESS;

    WPRINT_PLATFORM_DEBUG(("platform_bt_management_callback: %x\n", event ));
    /*If application has registered callback*/
    if(app_management_cback)
    {
        result = app_management_cback(event,p_event_data);
        return result;
    }
    switch( event )
    {
    /* Bluetooth  stack enabled */
    case BTM_ENABLED_EVT:
        bt_enabled_event_data.enabled = p_event_data->enabled;
        /*wiced_bt_app_init should be called first before we init MCU and peripherals*/
        wiced_bt_app_init();
#ifdef ENABLE_JLINK_TRACE
        SEGGER_RTT_Init();
#endif
        platform_init_mcu_infrastructure();
        platform_init_external_devices();
#ifndef WICED_DISABLE_WATCHDOG
        platform_watchdog_init( );
#endif

        /* CYW20739B1_587: CYW9MCU7x9N364: DMA does not work when powersave enabled */

        /* This function stops the polling to sense BT Transport. The default handler
         * of polling timers switches off the DMA UART channels if BT transport is not
         * sensed in the predefinded number of trials. CYW9MCU7x9N364 platform is using
         * UART channels for MCU <-> WiFi data transfer and this transfer stops in powersave mode
         * when sense handler stops the DMA. This API is called by the platform to unregister the
         * sense handlers.
         *
        */
#ifndef WICED_DISABLE_MCU_POWERSAVE
        bttransport_stop_polling_sense();
#endif

#ifndef WICED_DISABLE_MCU_POWERSAVE
    /* Initialise MCU powersave */
    platform_mcu_powersave_init();
#endif

    /* Always tick */
    platform_mcu_powersave_set_tick_mode(PLATFORM_TICK_POWERSAVE_MODE_TICKLESS_NEVER);

    /* Notify if returned from deep-sleep */
     WICED_SLEEP_CALL_EVENT_HANDLERS( WICED_DEEP_SLEEP_IS_WARMBOOT( ), WICED_LOW_POWER_DEEP_SLEEP, WICED_SLEEP_EVENT_LEAVE, &result );

#if defined ( USES_RESOURCE_GENERIC_FILESYSTEM )
        platform_resource_fs_init ();
#endif
        result = wiced_rtos_create_thread(&app_thread,(uint8_t)(WICED_APPLICATION_PRIORITY-1), "app_thread", (wiced_thread_function_t) application_start, (uint32_t)APPLICATION_STACK_SIZE, NULL );
        if(result != WICED_SUCCESS)
        {
            WPRINT_PLATFORM_ERROR(("App thread create Failed\n"));
            return WICED_ERROR;
        }
        break;

    case BTM_DISABLED_EVT:
        WPRINT_PLATFORM_ERROR(("%s received BTM_DISABLED_EVT\n",__func__));
        break;
    default:
        WPRINT_PLATFORM_DEBUG(("%s received event - %d\n",__func__,(int32_t)event));
        break;
    }

    return result;
}
/*This is the bt stack init call for the wiced apps
 * Since the actual stack init is already done from the platform_application_start()
 * this will only register the app callback handler and send the BTM_ENABLED_EVT to the application
 * after this all the bt stack management callback will be directly passed to the app registered callback.
 */
wiced_result_t wiced_bt_stack_init(wiced_bt_management_cback_t *p_bt_management_cback,
                                   const wiced_bt_cfg_settings_t     *p_bt_cfg_settings,
                                   const wiced_bt_cfg_buf_pool_t     app_bt_cfg_buf_pools[WICED_BT_CFG_NUM_BUF_POOLS])
{
    UNUSED_PARAMETER(*p_bt_cfg_settings);
    UNUSED_PARAMETER(*app_bt_cfg_buf_pools);
    if(p_bt_management_cback!=NULL)
        app_management_cback = p_bt_management_cback;
    else
        return WICED_BADARG;

    /*we send the enabled event to the app as stack is already enabled at this time*/
    app_management_cback(BTM_ENABLED_EVT,&bt_enabled_event_data);

    return WICED_SUCCESS;
}

/*we register this now platform app start to perform the platform init
 * later the actual application_start() will be called from app_thread
 */
void platform_application_start( void )
{
    wiced_bt_cfg_buf_pool_t *bt_buf_pool = platform_bt_cfg_buf_pools;

    platform_mem_init();

    /* wiced_bt_cfg_settings and wiced_bt_cfg_buf_pools are declared WEAK
     * for applications to override(it will be NULL if not defined),
     * if app defines them as part of its BT config we shall use it or
     * we apply default platform definition*/
    if(&wiced_bt_cfg_settings != NULL)
    {
        /* app may use const type for wiced_bt_cfg_settings, so we make a local copy*/
        memcpy(&platform_bt_cfg_settings,&wiced_bt_cfg_settings,sizeof(wiced_bt_cfg_settings_t));
        /*we may need one extra for libc heap area*/
        if(platform_bt_cfg_settings.max_number_of_buffer_pools < PLATFORM_BT_CFG_MAX_NO_OF_BUFPOOLS)
            platform_bt_cfg_settings.max_number_of_buffer_pools = PLATFORM_BT_CFG_MAX_NO_OF_BUFPOOLS;
        else
            platform_bt_cfg_settings.max_number_of_buffer_pools++;
    }

    if(wiced_bt_cfg_buf_pools != NULL)
        bt_buf_pool = (wiced_bt_cfg_buf_pool_t *)wiced_bt_cfg_buf_pools;

    app_management_cback = NULL;

    /* Register call back and configuration with stack */
    wiced_bt_stack_init_rom( platform_bt_management_callback ,
                (const wiced_bt_cfg_settings_t*)&platform_bt_cfg_settings, (const wiced_bt_cfg_buf_pool_t *)bt_buf_pool );

}

#ifdef WICED_USE_PLATFORM_ALLOCATED_POOL
uint8_t* platform_get_tx_buffer_pool(uint32_t size)
{
    WPRINT_PLATFORM_DEBUG(("%s \n",__func__));

    if( size < PLATFORM_MAX_TX_POOL_SIZE)
        return platform_tx_buffer_pool_memory; // Unused RAM space from B1 firmware
    else
    {
        WPRINT_PLATFORM_ERROR(("%s : Couldn't Allocate TX Buffer Pool \n",__func__));
        return NULL;
    }
}
uint8_t* platform_get_rx_buffer_pool(uint32_t size)
{
    WPRINT_PLATFORM_DEBUG(("%s \n",__func__));

    if( size < PLATFORM_MAX_RX_POOL_SIZE)
        return platform_rx_buffer_pool_memory;
    else
    {
        WPRINT_PLATFORM_ERROR(("%s : Couldn't Allocate RX Buffer Pool \n",__func__));
        return NULL;
    }
}
#endif
