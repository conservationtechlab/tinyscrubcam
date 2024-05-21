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
 *  MCU powersave implementation
 */
#if defined ( IAR_TOOLCHAIN )
#include "platform_cmis.h"
#endif
#include <stdint.h>
#include <string.h>
#include "platform_config.h"
#include "platform_peripheral.h"
#include "platform_sleep.h"
#include "platform_assert.h"
#include "wwd_assert.h"
#include <wiced_sleep.h>
#include "brcm_fw_types.h"
#include "wiced_hal_gpio.h"
#include "wiced_low_power.h"
#include "wiced_rtos.h"
#include "wiced_wifi.h"

/******************************************************
 *                      Macros
 ******************************************************/

/* Tick modes for FW2 function */
#define TICK_NEVER 0
#define TICK_ALWAYS 1
#define TICK_TICKLESS 2

/******************************************************
 *                    Constants
 ******************************************************/

/******************************************************
 *                   Enumerations
 ******************************************************/

/******************************************************
 *                 Type Definitions
 ******************************************************/

/* Powersave Initialisation State
 *
 */
typedef enum
  {
      POWERSAVE_NOT_READY,
      POWERSAVE_READY
  } powersave_state_t;

/******************************************************
 *                    Structures
 ******************************************************/

/******************************************************
 *               Function Declarations
 ******************************************************/
extern wiced_bool_t wiced_hidd_is_transport_detection_polling_on(void);
extern wiced_bool_t wiced_bt_aon_driver_is_SDS_allowed(void);

/*   INPUT mode = 0 - Never tick
*         mode = 1 - Always tick
*         mode = 2 = Tickless i.e. tick before timer expire
*/
extern void set_tick_mode(uint8_t);
extern uint8_t get_tick_mode(void);

extern uint8_t is_tick_timer_active(void);
extern  void set_min_tick_burst_counter(uint8_t);

#ifndef WICED_DISABLE_MCU_POWERSAVE
static void prepare_device_for_deep_sleep(void);

/* pre deep sleep callback */
void pre_deep_sleep_callback(void);
#endif

extern void ef_register_prepost_sleep_handler(void);

#ifdef WICED_POWER_LOGGER_ENABLE
void platform_wpl_pre_deep_sleep( void );
platform_result_t platform_wpl_pre_sleep( void );
#endif

/******************************************************
 *               Variable Definitions
 ******************************************************/
#ifndef WICED_DISABLE_MCU_POWERSAVE
static wiced_sleep_config_t         mcu_sleep_config;
powersave_state_t                   powersave_init_state = POWERSAVE_NOT_READY;

/* Sleep flag set to disable during initialization */
static wiced_bool_t powersave_sleep_flag          = WICED_FALSE;
static platform_mcu_powersave_mode_t powersave_mcu_mode = PLATFORM_MCU_POWERSAVE_MODE_MAX; /* Initial state is unknown */
extern uint8_t timedWake_SDS;

/* Counter to understand whether user has enabled
 * power save or just an intermediate state where we disable
 * power save -> perform some operation -> enable back
 */
static int powersave_counter;

static uint8_t deep_sleep_ready;

/* FW2 function to register pre-deepsleep callback */
void wiced_sleep_register_pre_shutdown_sleep_callback(void (*appCB)(void));
#endif


extern uint8_t bcs_liteEnable;

/******************************************************
 *               Function Definitions
 ******************************************************/
wiced_bool_t platform_mcu_powersave_is_permitted( void )
{
#ifndef WICED_DISABLE_MCU_POWERSAVE
    if( powersave_mcu_mode == PLATFORM_MCU_POWERSAVE_MODE_DEEP_SLEEP )
    {
        if(powersave_sleep_flag)
        {
            /* Set for Deep Sleep, */
            prepare_device_for_deep_sleep();
        }
    }
    else if( powersave_mcu_mode == PLATFORM_MCU_POWERSAVE_MODE_SLEEP )
    {
        /* Set for PDS. If System Tick timer is active, do not allow powersave */
        if( is_tick_timer_active() )
        {
            return WICED_FALSE;
        }
    }
    else
    {
        return WICED_FALSE;
    }
    return powersave_sleep_flag;
#endif
    return WICED_FALSE;
}

#ifndef WICED_DISABLE_MCU_POWERSAVE

static void prepare_device_for_deep_sleep(void)
{
    /* Before going into deep sleep, callback handler is called 2-3 times so avoid repeating the same preparatory steps */
    if(deep_sleep_ready)
        return;
    WPRINT_LIB_INFO(("Going into DeepSleep. Be prepared....\n"));
    wiced_wlan_connectivity_deinit();
#ifdef WICED_POWER_LOGGER_ENABLE
    /* Let WPL prepare for the deep sleep */
    platform_wpl_pre_deep_sleep();
#endif
    set_tick_mode(TICK_NEVER);
    platform_watchdog_disable();
    deep_sleep_ready = 1;

/* When BLE goes to SDS with neither advertising nor connected, every 3 min coldboot
 * is observed. Setting timedWake_SDS to FALSE avoids that situation but it should be
 * again set to TRUE when advertising/connected. Since this decision cannot be made
 * here so keeping it commented.
 */
//    timedWake_SDS = 0;
}

/* Pre SDS callback. It is called by FW2 just before going into deep sleep */
void pre_deep_sleep_callback(void)
{
    wiced_result_t result;

    UNUSED_PARAMETER( result );
    WICED_SLEEP_CALL_EVENT_HANDLERS( 1, WICED_LOW_POWER_DEEP_SLEEP, WICED_SLEEP_EVENT_ENTER, &result );
}
#endif


/* Callback from firmware to check if sleep is allowed.
 *
 */
static uint32_t mcu_sleep_permission_handler(wiced_sleep_poll_type_t type )
{
#ifndef WICED_DISABLE_MCU_POWERSAVE
    uint32_t ret = WICED_SLEEP_NOT_ALLOWED;

    switch(type)
    {
        case WICED_SLEEP_POLL_SLEEP_PERMISSION:
            if( platform_mcu_powersave_is_permitted() == WICED_TRUE )
            {
                if(powersave_mcu_mode == PLATFORM_MCU_POWERSAVE_MODE_DEEP_SLEEP)
                {
                    // Deep Sleep
                    ret = WICED_SLEEP_ALLOWED_WITH_SHUTDOWN;
                }
                else if(powersave_mcu_mode == PLATFORM_MCU_POWERSAVE_MODE_SLEEP)
                {
                    // PDS
                    ret = WICED_SLEEP_ALLOWED_WITHOUT_SHUTDOWN;

#ifdef WICED_POWER_LOGGER_ENABLE
                    /* Give WPL a chance to send the power log data to host. Not allowing immediate sleep if
                     * WPL has put some data on the UART
                     * */
                    if( platform_wpl_pre_sleep() == PLATFORM_SUCCESS )
                        ret = WICED_SLEEP_NOT_ALLOWED;
#endif
                }
            }
            break;

        case WICED_SLEEP_POLL_TIME_TO_SLEEP:
            if( platform_mcu_powersave_is_permitted() != WICED_TRUE )
            {
                ret = 0;
            }
            else
            {
                // Longer Sleep
                ret = WICED_SLEEP_MAX_TIME_TO_SLEEP;
            }
    }
    return ret;
#else
    UNUSED_PARAMETER( type );
    return WICED_SLEEP_NOT_ALLOWED;
#endif
}

/* Initializes powersave mode.
 * - Only Deep-Sleep (BCS) mode supported for now.
 * - Registers sleep callback for BCS mode
 */
platform_result_t platform_mcu_powersave_init( void )
{
#ifndef WICED_DISABLE_MCU_POWERSAVE
    platform_result_t              result = PLATFORM_SUCCESS;

    WPRINT_LIB_INFO(("platform_mcu_powersave_init: \n"));

    if ( WICED_DEEP_SLEEP_IS_WARMBOOT( ) )
        WPRINT_LIB_INFO(("******** WARMBOOT *********\n"));

    /* Initialize to NO SLEEP */
    powersave_init_state = POWERSAVE_NOT_READY;

    /* configure to sleep  */
    mcu_sleep_config.sleep_mode             = WICED_SLEEP_MODE_NO_TRANSPORT;
    mcu_sleep_config.device_wake_mode       = WICED_BT_MCU_WAKE_MODE;
    mcu_sleep_config.device_wake_source     = WICED_SLEEP_WAKE_SOURCE_GPIO;
    mcu_sleep_config.device_wake_gpio_num   = WICED_BT_MCU_WAKE_PIN;
    mcu_sleep_config.host_wake_mode         = WICED_BT_MCU_WAKE_MODE;
    mcu_sleep_config.sleep_permit_handler   = mcu_sleep_permission_handler;

    ef_register_prepost_sleep_handler();

    if( wiced_sleep_configure( &mcu_sleep_config ) != WICED_SUCCESS )
    {
        WPRINT_PLATFORM_ERROR(("platform_mcu_powersave_init: Error initializing powersave\n"));
        return PLATFORM_ERROR;
    }
    powersave_init_state = POWERSAVE_READY;

    /* Lite BCS feature implemented in FW2 has increased the power numbers for BLE data transfer case.
     * We disable the feature here temporarily until JIRA CYW20739B1-796 is resolved.
     */
    bcs_liteEnable = 0;

//    set_min_tick_burst_counter(1);

    // Set default sleep mode to PDS
    powersave_mcu_mode = PLATFORM_MCU_POWERSAVE_MODE_SLEEP;

    wiced_sleep_register_pre_shutdown_sleep_callback(pre_deep_sleep_callback);
    return result;
#endif
    return PLATFORM_UNSUPPORTED;
}

/* Checks for warmboot from deep sleep
 *
 */
wiced_bool_t platform_mcu_powersave_is_warmboot( void )
{
#if WICED_DEEP_SLEEP_IS_ENABLED()
    if(wiced_sleep_get_boot_mode() == WICED_SLEEP_FAST_BOOT)
        return WICED_TRUE;
#endif
    return WICED_FALSE;
}


/* It maintains the sleep flag.
 *
 */
static platform_result_t platform_mcu_set_sleep( wiced_bool_t sleep_flag )
{
#ifndef WICED_DISABLE_MCU_POWERSAVE
    if (powersave_init_state == POWERSAVE_NOT_READY)
        return PLATFORM_UNINITLIASED;

    powersave_sleep_flag = sleep_flag;

    return PLATFORM_SUCCESS;
#else
    UNUSED_PARAMETER( sleep_flag );
    return PLATFORM_UNSUPPORTED;
#endif /* WICED_DISABLE_MCU_POWERSAVE */
}


/* Disables powersave:
 *
 */
platform_result_t platform_mcu_powersave_disable( void )
{
    platform_result_t result = PLATFORM_SUCCESS;

#ifndef WICED_DISABLE_MCU_POWERSAVE
    powersave_counter++;

    result = platform_mcu_set_sleep( WICED_FALSE );
    if(result == PLATFORM_SUCCESS)
    {
        /* Always tick */
        platform_mcu_powersave_set_tick_mode(PLATFORM_TICK_POWERSAVE_MODE_TICKLESS_NEVER);
    }
#endif
    return result;
}

/* Enables powersave:
 *
 */

platform_result_t platform_mcu_powersave_enable( void )
{
    platform_result_t result = PLATFORM_SUCCESS;
#ifndef WICED_DISABLE_MCU_POWERSAVE
    powersave_counter--;
    if(powersave_counter < 0)
    {
        result = platform_mcu_set_sleep( WICED_TRUE );
        if(result == PLATFORM_SUCCESS)
        {
            /* Set the tick mode to tickless */
            platform_mcu_powersave_set_tick_mode(PLATFORM_TICK_POWERSAVE_MODE_TICKLESS_ALWAYS);
        }
    }
#endif
    return result;
}

void platform_mcu_powersave_set_tick_mode( platform_tick_powersave_mode_t mode )
{
    if(mode == PLATFORM_TICK_POWERSAVE_MODE_TICKLESS_ALWAYS)
    {
        /* Set the mode to tickless */
        set_tick_mode(TICK_TICKLESS);
    }
    else if(mode == PLATFORM_TICK_POWERSAVE_MODE_TICKLESS_NEVER)
    {
        /* Always tick */
        set_tick_mode(TICK_ALWAYS);
    }
    return;
}

platform_tick_powersave_mode_t platform_mcu_powersave_get_tick_mode( void )
{
    uint8_t mode;

    mode = get_tick_mode();

    if(mode == TICK_ALWAYS)
        return PLATFORM_TICK_POWERSAVE_MODE_TICKLESS_NEVER;
    else if(mode == TICK_TICKLESS)
        return PLATFORM_TICK_POWERSAVE_MODE_TICKLESS_ALWAYS;
    return PLATFORM_TICK_POWERSAVE_MODE_MAX;
}

/* Sets powersave mode to Deep Sleep (SDS) or Sleep (PDS) */
void platform_mcu_powersave_set_mode( platform_mcu_powersave_mode_t mode )
{
#ifndef WICED_DISABLE_MCU_POWERSAVE
    if(mode == PLATFORM_MCU_POWERSAVE_MODE_DEEP_SLEEP)
    {
        #if !WICED_DEEP_SLEEP_IS_ENABLED()
            /* Ignore Deep Sleep mode if Deep sleep not enabled. Use PDS */
            powersave_mcu_mode =  PLATFORM_MCU_POWERSAVE_MODE_SLEEP;
            return;
        #endif
    }
    powersave_mcu_mode = mode;
#else
    UNUSED_PARAMETER( mode );
#endif /* WICED_DISABLE_MCU_POWERSAVE */
    return;
}

platform_mcu_powersave_mode_t platform_mcu_powersave_get_mode( void )
{
#ifndef WICED_DISABLE_MCU_POWERSAVE
    return powersave_mcu_mode;
#else
    return PLATFORM_MCU_POWERSAVE_MODE_MAX;
#endif
}


void platform_mcu_powersave_exit_notify( void )
{
    return;
}

void platform_mcu_pm_enable( void )
{
}

void platform_mcu_release_power_lock(void)
{
}

void platform_mcu_acquire_power_lock(void)
{
}

/******************************************************
 *               RTOS Powersave Hooks
 ******************************************************/

void platform_idle_hook( void )
{
    return;
}

uint32_t platform_power_down_hook( uint32_t sleep_ms )
{
    UNUSED_PARAMETER( sleep_ms );
    return 0;
}
