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
 * BCM920739 common GPIO implementation
 */
#include "stdint.h"
#include "string.h"
#include "platform_peripheral.h"
#include "platform_isr.h"
//#include "platform_isr_interface.h"
#include "wwd_rtos.h"
#include "wwd_assert.h"
#include "platform_mcu_peripheral.h"
#include "brcm_fw_types.h"
#include "wiced_hal_gpio.h"
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

static platform_gpio_port_t* gpio_ports[ NUMBER_OF_GPIO_PORTS ];

/******************************************************
 *            Platform Function Definitions
 ******************************************************/

static platform_gpio_port_t* gpio_get_port( uint32_t pin )
{
    uint32_t count;
    for ( count = 0; count < NUMBER_OF_GPIO_PORTS; count++ )
    {
        if ( ( gpio_ports[ count ] != NULL ) && gpio_ports[ count ]->pin_number == pin )
            return gpio_ports[ count ];
    }
    return NULL;
}

static void gpio_interrupt_handler( void *data, uint8_t pin )
{
    platform_gpio_port_t* port;
    platform_gpio_irq_callback_t callback;

    port = gpio_get_port( pin );
    if ( port != NULL && port->callback != NULL )
    {
        callback = (platform_gpio_irq_callback_t) port->callback;
        callback( port->arg );
    }
}

static uint32_t gpio_init_config( const platform_gpio_t* gpio )
{
    uint32_t config = 0;
    platform_gpio_port_t* port;
    wiced_assert( "bad argument", ( gpio != NULL ) );

    port = gpio->port;
    wiced_assert( "bad argument", ( port != NULL ) );

    if ( ( port->config == INPUT_PULL_UP ) || ( port->config == INPUT_PULL_DOWN ) || ( port->config == INPUT_HIGH_IMPEDANCE ) )
    {
        config = GPIO_INPUT_ENABLE;

    }
    else
    {
        config = GPIO_OUTPUT_ENABLE; //WICED_SET_GPIO_DIR_OUTPUT;
    }

    if ( ( port->config == INPUT_PULL_UP ) || ( port->config == OUTPUT_OPEN_DRAIN_PULL_UP ) )
    {
        config |= GPIO_PULL_UP;
    }
    else if ( port->config == INPUT_PULL_DOWN )
    {
        config |= GPIO_PULL_DOWN;
    }
    else
    {
        config |= GPIO_PULL_UP_DOWN_NONE;
    }

    if ( port->callback != NULL )
    {
        if ( port->interrupt == IRQ_TRIGGER_RISING_EDGE )
            config |= GPIO_EN_INT_RISING_EDGE;
        else if ( port->interrupt == IRQ_TRIGGER_FALLING_EDGE )
            config |= GPIO_EN_INT_FALLING_EDGE;
        else if ( port->interrupt == IRQ_TRIGGER_BOTH_EDGES )
            config |= GPIO_EN_INT_BOTH_EDGE;
    }

    return config;
}

platform_result_t platform_gpio_init( const platform_gpio_t* gpio, platform_pin_config_t config )
{
    uint32_t gpio_config;
    platform_gpio_port_t* port;

    if ( gpio == NULL )
        return PLATFORM_SUCCESS;

    /* wiced_hal_gpio_init() is already done from wiced_bt_app_init()
     * called from platfrom_app_start()(platfrom_init.c)*/
    //wiced_hal_gpio_init();
    port = gpio->port;
    wiced_assert( "bad argument", ( port != NULL ) );

    port->config = config;
    port->interrupt = IRQ_TRIGGER_RISING_EDGE;
    port->pin_number = gpio->pin_number;
    port->callback = NULL;
    port->mux_mode = gpio->mux_mode;

    /* fill the config structure */
    gpio_config = gpio_init_config( gpio );

    wiced_hal_gpio_configure_pin( port->pin_number, gpio_config, GPIO_PIN_OUTPUT_LOW );

    gpio_ports[ port->pin_number ] = port;
    return PLATFORM_SUCCESS;
}

platform_result_t platform_gpio_deinit( const platform_gpio_t* gpio )
{
    uint32_t config;
    platform_gpio_port_t* port;

    wiced_assert( "bad argument", ( gpio != NULL ) );

    port = gpio->port;

    wiced_assert( "bad argument", ( port != NULL ) );

    config = GPIO_OUTPUT_DISABLE | GPIO_INTERRUPT_DISABLE | GPIO_PULL_UP_DOWN_NONE;

    wiced_hal_gpio_configure_pin( port->pin_number, config, GPIO_PIN_OUTPUT_LOW );

    gpio_ports[ port->pin_number ] = NULL;

    return PLATFORM_SUCCESS;
}

platform_result_t platform_gpio_output_high( const platform_gpio_t* gpio )
{
    platform_gpio_port_t* port;
    wiced_assert( "bad argument", ( gpio != NULL ) );
    port = gpio->port;

    wiced_assert( "bad argument", ( port != NULL ) );

    /* Set the output value of output pin P# to HIGH */
    wiced_hal_gpio_set_pin_output( port->pin_number, GPIO_PIN_OUTPUT_HIGH );

    return PLATFORM_SUCCESS;
}

platform_result_t platform_gpio_output_low( const platform_gpio_t* gpio )
{
    platform_gpio_port_t* port;
    wiced_assert( "bad argument", ( gpio != NULL ) );
    port = gpio->port;

    wiced_assert( "bad argument", ( port != NULL ) );

    wiced_hal_gpio_set_pin_output( port->pin_number, GPIO_PIN_OUTPUT_LOW );

    return PLATFORM_SUCCESS;
}

wiced_bool_t platform_gpio_input_get( const platform_gpio_t* gpio )
{

    platform_gpio_port_t* port;
    wiced_bool_t result = WICED_FALSE;
    uint32_t state;
    wiced_assert( "bad argument", ( gpio != NULL ) );
    port = gpio->port;

    wiced_assert( "bad argument", ( port != NULL ) );

    state = wiced_hal_gpio_get_pin_input_status( port->pin_number );

    if ( state == GPIO_PIN_OUTPUT_HIGH )
        result = WICED_TRUE;

    return result;
}

platform_result_t platform_gpio_irq_enable( const platform_gpio_t* gpio, platform_gpio_irq_trigger_t trigger, platform_gpio_irq_callback_t handler, void* arg )
{

    uint32_t config;
    platform_gpio_port_t* port;
    wiced_assert( "bad argument", ( gpio != NULL ) );
    port = gpio->port;

    wiced_assert( "bad argument", ( port != NULL ) );
    /* check if port is not initialized */
    if ( gpio_ports[ gpio->pin_number ] == NULL )
        return PLATFORM_NO_EFFECT;

    port->interrupt = trigger;
    port->callback = (void*) handler;
    port->arg = arg;

    config = gpio_init_config( gpio );
    wiced_hal_gpio_configure_pin( port->pin_number, config, GPIO_PIN_OUTPUT_LOW );
    wiced_hal_gpio_register_pin_for_interrupt( port->pin_number, gpio_interrupt_handler, port->arg );

    return PLATFORM_SUCCESS;
}

platform_result_t platform_gpio_irq_disable( const platform_gpio_t* gpio )
{
    uint32_t config;
    platform_gpio_port_t* port;
    wiced_assert( "bad argument", ( gpio != NULL ) );
    port = gpio->port;

    wiced_assert( "bad argument", ( port != NULL ) );

    /* check if port is not initialized */
    if ( gpio_ports[ gpio->pin_number ] == NULL )
        return PLATFORM_NO_EFFECT;

    port->interrupt = IRQ_TRIGGER_RISING_EDGE;
    port->callback = NULL;
    port->arg = NULL;

    config = GPIO_INPUT_DISABLE | GPIO_INTERRUPT_DISABLE | GPIO_PULL_UP_DOWN_NONE;

    wiced_hal_gpio_configure_pin( port->pin_number, config, GPIO_PIN_OUTPUT_LOW );

    return PLATFORM_SUCCESS;

}
platform_result_t platform_set_pin_function( const platform_gpio_t* gpio )
{
    return PLATFORM_SUCCESS;
}

