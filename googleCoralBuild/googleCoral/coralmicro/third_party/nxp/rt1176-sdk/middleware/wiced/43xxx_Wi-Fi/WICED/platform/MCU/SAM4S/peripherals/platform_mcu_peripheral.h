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
 * Defines STM32F2xx common peripheral structures, macros, constants and declares STM32F2xx peripheral API
 */
#pragma once
#include "platform_cmsis.h"
#include "platform_constants.h"

/* ASF library headers */
#include "sam4s.h"
#include "sysclk.h"
#include "adc.h"
#include "ioport.h"
#include "usart.h"
#include "spi.h"
#include "twi.h"
#include "supc.h"
#include "pdc.h"
#include "pwm.h"
#include "wdt.h"
#include "efc.h"
#include "rtt.h"
#include "supc.h"
#include "matrix.h"

#include "wwd_constants.h"
#include "RTOS/wwd_rtos_interface.h"
#include "ring_buffer.h"

#ifdef __cplusplus
extern "C"
{
#endif

/******************************************************
 *                      Macros
 ******************************************************/

/******************************************************
 *                    Constants
 ******************************************************/
/* Default STDIO buffer size */
#ifndef STDIO_BUFFER_SIZE
#define STDIO_BUFFER_SIZE        (64)
#endif


/******************************************************
 *                   Enumerations
 ******************************************************/

/******************************************************
 *                 Type Definitions
 ******************************************************/

/******************************************************
 *                    Structures
 ******************************************************/


typedef struct
{
    wiced_bool_t is_wakeup_pin;
    uint8_t      wakeup_pin_number; /* wakeup pin number: 0 .. 15                     */
    uint8_t      trigger;           /* wakeup trigger: IOPORT_SENSE_FALLING or RISING */
} platform_wakeup_pin_config_t;


typedef struct
{
    const platform_wakeup_pin_config_t* wakeup_pin_config;
    ioport_pin_t pin;
} platform_gpio_t;


typedef enum
{
    UART_0,
    UART_1,
    USART_0,
    USART_1,
} platform_uart_id_t;

typedef enum
{
    TWI_0,
    TWI_1,
} platform_i2c_id_t;

typedef struct
{
    Adc*                     peripheral;
    uint8_t                  peripheral_id;
    const platform_gpio_t*   adc_pin;
    uint32_t                 adc_clock_hz;
    enum adc_channel_num_t   channel;
    enum adc_settling_time_t settling_time;
    enum adc_resolution_t    resolution;
    enum adc_trigger_t       trigger;
} platform_adc_t;

typedef struct
{
    uint8_t unimplemented;
} platform_pwm_t;

typedef struct
{
    Spi*                   peripheral;      /* Peripheral            */
    uint8_t                peripheral_id;   /* Peripheral ID         */
    const platform_gpio_t* clk_pin;         /* CLK  pin              */
    const platform_gpio_t* mosi_pin;        /* MOSI pin              */
    const platform_gpio_t* miso_pin;        /* MISO pin              */
} platform_spi_t;

typedef struct
{
    uint8_t unimplemented;
} platform_spi_slave_driver_t;

typedef struct
{
    platform_i2c_id_t       i2c_block_id;
    Twi*                    peripheral;
    uint8_t                 peripheral_id;
    const platform_gpio_t*  sda_pin;
    const platform_gpio_t*  scl_pin;
} platform_i2c_t;

typedef struct
{
    platform_uart_id_t     uart_id;
    void*                  peripheral;       /* Usart* or Uart*  */
    uint8_t                peripheral_id;    /* Peripheral ID    */
    const platform_gpio_t* tx_pin;           /* Tx pin           */
    const platform_gpio_t* rx_pin;           /* Rx pin           */
    const platform_gpio_t* cts_pin;          /* CTS pin          */
    const platform_gpio_t* rts_pin;          /* RTS pin          */
} platform_uart_t;



typedef struct
{
    const platform_uart_t*      peripheral;
    host_semaphore_type_t tx_dma_complete;
    host_semaphore_type_t rx_dma_complete;
    wiced_ring_buffer_t*  rx_ring_buffer;
    uint32_t              rx_transfer_size;
} platform_uart_driver_t;

/******************************************************
 *                 Global Variables
 ******************************************************/

/******************************************************
 *               Function Declarations
 ******************************************************/
platform_result_t platform_powersave_enable_wakeup_pin( const platform_wakeup_pin_config_t* config );

void platform_uart_irq( platform_uart_driver_t* driver );
platform_result_t platform_gpio_peripheral_pin_init( const platform_gpio_t* gpio, ioport_mode_t pin_mode );
//platform_result_t platform_gpio_irq_enable ( const platform_gpio_t* gpio, platform_gpio_irq_trigger_t trigger, platform_gpio_irq_callback_t handler, void* arg );
platform_result_t platform_mcu_powersave_init( void );
wwd_result_t platform_bus_enter_powersave( void );
wwd_result_t platform_bus_exit_powersave ( void );

platform_result_t platform_watchdog_init( void );

platform_result_t platform_filesystem_init( void );



#ifdef __cplusplus
} /* extern "C" */
#endif

