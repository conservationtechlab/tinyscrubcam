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
 * Defines WWD SPI functions for BCM920739 MCU
 */
#if defined ( IAR_TOOLCHAIN )
#include "platform_cmis.h"
#endif
#include "string.h" /* for memcpy */
//#include "wifi_nvram_image.h"
#include "wwd_bus_protocol.h"
#include "wwd_assert.h"
#include "wwd_rtos.h"
#include "wwd_platform_common.h"
#include "network/wwd_buffer_interface.h"
#include "platform/wwd_platform_interface.h"
#include "platform/wwd_bus_interface.h"
#include "platform/wwd_spi_interface.h"
//#include "platform_cmsis.h"
#include "platform_config.h"
#include "platform_peripheral.h"
#include "platform.h"
#include "brcm_fw_types.h"
#include "wiced_hal_gpio.h"
/******************************************************
*             Constants
******************************************************/
#define WLAN_SPI_BAUD_RATE_BPS (24000000)/* It is actually prescale factor*/


/******************************************************
*             Structures
******************************************************/

/******************************************************
*               Static Function Declarations
******************************************************/
extern const platform_spi_t platform_spi_peripherals[];
extern const platform_gpio_t* wifi_spi_pins_p[];
extern void wiced_hal_mia_init(void) ;
extern void wiced_hal_mia_enable_mia_interrupt(BOOL32 enable) ;
extern void wiced_hal_mia_enable_lhl_interrupt(BOOL32 enable) ;
extern void wiced_app_event_init_serialization(void) ;

/******************************************************
*             Variables
******************************************************/
extern const platform_spi_t platform_spi_peripherals[];
extern const platform_gpio_t* wifi_spi_pins_p[];
platform_spi_config_t wifi_config;
/******************************************************
*             Static Function Declarations
******************************************************/
static void spi_irq_handler(void *data, uint8_t port_pin);

/******************************************************
*             Function definitions
******************************************************/
extern void gpio_clearPinInterruptStatus(BYTE port, BYTE pin);

static void gpio_set_input_interrupt( void )
{
    uint16_t gpio_num = WICED_WIFI_BT_SPI_IRQ_PIN;

    wiced_hal_gpio_register_pin_for_interrupt( gpio_num, spi_irq_handler, NULL );

#ifndef WWD_SPI_IRQ_FALLING_EDGE
    /* Configure GPIO PIN# as input, pull down and interrupt on rising edge and output value as low */
    wiced_hal_gpio_configure_pin( gpio_num, (GPIO_INPUT_ENABLE | GPIO_PULL_DOWN | GPIO_EN_INT_RISING_EDGE),
                                  GPIO_PIN_OUTPUT_LOW );
#else
    wiced_hal_gpio_configure_pin( gpio_num, (GPIO_INPUT_ENABLE | GPIO_PULL_UP | GPIO_EN_INT_FALLING_EDGE),
                                      GPIO_PIN_OUTPUT_HIGH );
#endif
    gpio_clearPinInterruptStatus((BYTE)PIN_TO_PORT(gpio_num), (BYTE)PIN_TO_PIN(gpio_num));
}

wwd_result_t host_platform_bus_init(void)
{
    /* Init SPI port. CPHA = 0 and CPOL = 0 */
    wifi_config.speed = WLAN_SPI_BAUD_RATE_BPS;
    wifi_config.bits = 0; // not used
    wifi_config.chip_select = wifi_spi_pins_p[WWD_PIN_SPI_CS];
    wifi_config.mode = 0; // not used
    return (wwd_result_t)platform_spi_init(&platform_spi_peripherals[WICED_WIFI_SPI_PERIPHERAL], &wifi_config);
}

wwd_result_t host_platform_bus_deinit(void)
{
    return (wwd_result_t)platform_spi_deinit(&platform_spi_peripherals[WICED_WIFI_SPI_PERIPHERAL]);
}

wwd_result_t host_platform_spi_transfer(wwd_bus_transfer_direction_t dir, uint8_t* buffer, uint16_t buffer_length)
{
    platform_spi_message_segment_t xfer[1];

    memset(&xfer[0], 0x00, sizeof(xfer[0]));

    xfer[0].length = buffer_length;
    xfer[0].rx_buffer = (dir == BUS_WRITE) ? (NULL) : (buffer);
    xfer[0].tx_buffer = buffer;

    return (wwd_result_t)platform_spi_transfer(&platform_spi_peripherals[WICED_WIFI_SPI_PERIPHERAL],&wifi_config,xfer,1);
}

wwd_result_t host_platform_bus_enable_interrupt( void )
{
//    platform_gpio_irq_enable( wifi_spi_pins_p[WWD_PIN_SPI_IRQ], IRQ_TRIGGER_RISING_EDGE, spi_irq_handler, 0 );
    wiced_app_event_init_serialization();
    wiced_hal_mia_init();
    wiced_hal_mia_enable_mia_interrupt(TRUE);
    wiced_hal_mia_enable_lhl_interrupt(TRUE);
    gpio_set_input_interrupt();

    return  WWD_SUCCESS;
}

wwd_result_t host_platform_bus_disable_interrupt( void )
{
//    platform_gpio_irq_disable( wifi_spi_pins_p[WWD_PIN_SPI_IRQ] );
    return  WWD_SUCCESS;
}

void host_platform_bus_buffer_freed( wwd_buffer_dir_t direction )
{
    UNUSED_PARAMETER(direction);
}

static void spi_irq_handler(void *data, uint8_t port_pin)
{
    UNUSED_PARAMETER(data);
    UNUSED_PARAMETER(port_pin);
    wwd_thread_notify_irq();
}
