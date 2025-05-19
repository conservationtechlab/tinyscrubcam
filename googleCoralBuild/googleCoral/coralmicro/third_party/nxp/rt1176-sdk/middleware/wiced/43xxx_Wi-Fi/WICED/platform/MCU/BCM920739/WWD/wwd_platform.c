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

#if defined ( IAR_TOOLCHAIN )
#include "platform_cmis.h"
#endif
#include <stdint.h>
#include "wwd_constants.h"
#include "wwd_assert.h"
#include "platform_peripheral.h"
#include "platform/wwd_platform_interface.h"
#include "wwd_platform_common.h"
#include "platform_config.h"
#include "platform.h"
#include "brcm_fw_types.h"
#include "wiced_hal_gpio.h"
#include "wwd_rtos_interface.h"

//#include "platform_cmsis.h"

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
 *               Function Definitions
 ******************************************************/

wwd_result_t host_platform_init( void )
{
    wiced_hal_gpio_configure_pin( WICED_BT_WIFI_REG_ON_PIN, GPIO_OUTPUT_ENABLE , GPIO_PIN_OUTPUT_HIGH  );
    return WWD_SUCCESS;
}
wwd_result_t host_platform_deinit(void)
{
    return WWD_SUCCESS;
}

void host_platform_reset_wifi( wiced_bool_t reset_asserted )
{
    if ( reset_asserted == WICED_TRUE )
    {
        host_platform_power_wifi( WICED_FALSE );
    }
    else
    {
        host_platform_power_wifi( WICED_TRUE );
        host_rtos_delay_milliseconds( (uint32_t) 100 );
    }
}

void host_platform_power_wifi( wiced_bool_t power_enabled )
{
    if(power_enabled)
    {
        wiced_hal_gpio_set_pin_output(WICED_BT_WIFI_REG_ON_PIN, GPIO_PIN_OUTPUT_HIGH);
    } else {
        wiced_hal_gpio_set_pin_output(WICED_BT_WIFI_REG_ON_PIN, GPIO_PIN_OUTPUT_LOW);
    }
}

wwd_result_t host_platform_init_wlan_powersave_clock( void )
{
    /* Nothing to do here */
    return WWD_SUCCESS;
}

wwd_result_t host_platform_deinit_wlan_powersave_clock( void )
{
    /* Nothing to do here */
    return WWD_UNSUPPORTED;
}



