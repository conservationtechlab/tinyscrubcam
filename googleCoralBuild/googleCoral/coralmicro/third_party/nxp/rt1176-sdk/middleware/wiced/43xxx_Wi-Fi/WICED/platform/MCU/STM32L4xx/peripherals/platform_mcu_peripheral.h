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
 * Defines STM32L4xx common peripheral structures, macros, constants and declares STM32L4xx peripheral API
 */
#pragma once
#include "platform_cmsis.h"
#include "platform_constants.h"
#include "wwd_constants.h"
#include "RTOS/wwd_rtos_interface.h"
#include "ring_buffer.h"
#include "stm32l4xx.h"
#include "stm32l4xx_hal.h"
#include "stm32l4xx_hal_rcc.h"
#include "stm32l4xx_hal_iwdg.h"
#include "stm32l4xx_hal_uart.h"
#include "stm32l4xx_hal_usart.h"
#include "stm32l4xx_hal_gpio.h"
#include "stm32l4xx_hal_gpio_ex.h"

#include "platform_cache.h"

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

/* GPIOA to I */
#define NUMBER_OF_GPIO_PORTS      (9)

/* Interrupt line 0 to 15. Each line is shared among the same numbered pins across all GPIO ports */
#define NUMBER_OF_GPIO_IRQ_LINES  (16)

/* USART1, USART2, USART3, UART4, UART5 */
#define NUMBER_OF_UART_PORTS      (5)

/* Invalid UART port number */
#define INVALID_UART_PORT_NUMBER  (0xff)

/* Invalid GPIO port number */
#define INVALID_GPIO_PORT_NUMBER  (0xff)

/* Invalid I2C port number */
#define INVALID_I2C_PORT_NUMBER   (0xff)

/* Invalid PWM port number */
#define INVALID_PWM_PORT_NUMBER   (0xff)

/* WatchDog configuration */
#define WATCHDOG_INSTANCE         (IWDG)
#define WATCHDOG_PRESCALER        (IWDG_PRESCALER_256)
#define WATCHDOG_RESET_FLAG       (RCC_FLAG_IWDGRST)
#define PRESCALAR                 (256)
#define LSI_FREQ_HZ               (32000)

/* RTC configuration */
#define RTC_CLOCK_SOURCE          (RCC_RTCCLKSOURCE_LSI)
#define RTC_PRESCALER_A           (127)
#define RTC_PRESCALER_S           (255)

/* Default STDIO buffer size */
#ifndef STDIO_BUFFER_SIZE
#define STDIO_BUFFER_SIZE         (64)
#endif

/* I2C1 to I2C4 */
#define NUMBER_OF_I2C_PORTS       (4)

/* TIM1, TIM2, TIM3, TIM4, TIM5, TIM8, TIM15, TIM16, TIM17 */
#define NUMBER_OF_PWM_PORTS       (9)

/******************************************************
 *                   Enumerations
 ******************************************************/

/******************************************************
 *                 Type Definitions
 ******************************************************/

/* GPIO port */
typedef GPIO_TypeDef  platform_gpio_port_t;

/* UART port */
typedef USART_TypeDef platform_uart_port_t;

/* I2C port */
typedef I2C_TypeDef   platform_i2c_port_t;

/* PWM port */
typedef TIM_TypeDef   platform_pwm_port_t;

/* GPIO alternate function */
typedef uint8_t       platform_gpio_alternate_function_t;

typedef void (*platform_uart_clock_enable_function_t)(void);
typedef void (*platform_uart_clock_disable_function_t)(void);

typedef void (*platform_gpio_clock_enable_function_t)(void);
typedef void (*platform_gpio_clock_disable_function_t)(void);

typedef void (*platform_i2c_clock_enable_function_t)(void);
typedef void (*platform_i2c_clock_disable_function_t)(void);

typedef void (*platform_pwm_clock_enable_function_t)(void);
typedef void (*platform_pwm_clock_disable_function_t)(void);

/******************************************************
 *                    Structures
 ******************************************************/

typedef struct
{
    DMA_TypeDef*         controller;
    uint32_t             channel;
    DMA_Channel_TypeDef* stream;
    IRQn_Type            irq_vector;
    uint32_t             complete_flags;
    uint32_t             error_flags;
} platform_dma_config_t;

typedef struct
{
    platform_gpio_port_t* port;       /* GPIO port */
    uint8_t               pin_number; /* pin number */
} platform_gpio_t;

typedef struct
{
    platform_uart_port_t*           port;
    const platform_gpio_t*          tx_pin;
    const platform_gpio_t*          rx_pin;
    const platform_gpio_t*          cts_pin;
    const platform_gpio_t*          rts_pin;
    platform_dma_config_t  tx_dma_config;
    platform_dma_config_t  rx_dma_config;
} platform_uart_t;

typedef struct
{
    platform_uart_t*           peripheral;
    wiced_ring_buffer_t*       rx_buffer;
    host_semaphore_type_t      rx_complete;
    host_semaphore_type_t      tx_complete;
    volatile uint32_t          tx_size;
    volatile uint32_t          rx_size;
    volatile platform_result_t last_receive_result;
    volatile platform_result_t last_transmit_result;
} platform_uart_driver_t;

typedef struct
{
    uint32_t dummy;
} platform_adc_t;

typedef struct
{
    platform_i2c_port_t*   port;
    const platform_gpio_t* pin_scl;
    const platform_gpio_t* pin_sda;
    platform_dma_config_t  tx_dma_config;
    platform_dma_config_t  rx_dma_config;

} platform_i2c_t;

typedef struct
{
    platform_i2c_t*            i2c;
    host_semaphore_type_t      rw_complete;
    volatile platform_result_t last_result;
} platform_i2c_driver_t;

typedef struct
{
    platform_pwm_port_t*   port;
    uint32_t               channel;
    const platform_gpio_t* pin;
} platform_pwm_t;
typedef struct
{
    uint32_t dummy;
} platform_spi_t;

typedef struct
{
    uint32_t dummy;
} platform_spi_slave_driver_t;

typedef enum
{
    WICED_QSPI_PIN_CS,
    WICED_QSPI_PIN_CLK,
    WICED_QSPI_PIN_D0,
    WICED_QSPI_PIN_D1,
    WICED_QSPI_PIN_D2,
    WICED_QSPI_PIN_D3,
    WICED_QSPI_PIN_MAX,
} wiced_qspi_pin_t;

/******************************************************
 *                 Global Variables
 ******************************************************/

/******************************************************
 *               Function Declarations
 ******************************************************/

platform_result_t platform_gpio_irq_manager_init          ( void );
uint8_t           platform_gpio_get_port_number           ( platform_gpio_port_t* gpio_port );
platform_result_t platform_gpio_set_alternate_function    ( platform_gpio_port_t* gpio_port, uint8_t pin_number, uint32_t mode, uint32_t pull_up_down_type, uint32_t alternation_function );

platform_result_t platform_watchdog_init                  ( void );

platform_result_t platform_filesystem_init                ( void );

uint8_t           platform_uart_get_port_number           ( platform_uart_port_t* uart );
void              platform_uart_irq                       ( platform_uart_driver_t* driver );
void              platform_uart_rx_dma_init               ( DMA_HandleTypeDef* dma_handle_rx, UART_HandleTypeDef* uart_handle, const platform_uart_t* peripheral );
void              platform_uart_tx_dma_init               ( DMA_HandleTypeDef* dma_handle_tx, UART_HandleTypeDef* uart_handle, const platform_uart_t* peripheral );
void              platform_uart_update_rx_ring_buffer_tail( platform_uart_driver_t* driver, DMA_HandleTypeDef* dma_handle_rx );
void              platform_uart_dma_tx_irq                ( platform_uart_driver_t* driver );
void              platform_uart_dma_rx_irq                ( platform_uart_driver_t* driver );

void              platform_gpio_irq                       ( void );

uint8_t           platform_i2c_get_port_number            ( platform_i2c_port_t* i2c_port );
uint32_t          platform_i2c_get_port_timing            ( platform_i2c_port_t* i2c_port, uint32_t bus_freq );
void              platform_i2c_tx_dma_init                ( DMA_HandleTypeDef* dma_handle_tx, I2C_HandleTypeDef* i2c_handle, const platform_i2c_t* peripheral );
void              platform_i2c_rx_dma_init                ( DMA_HandleTypeDef* dma_handle_rx, I2C_HandleTypeDef* i2c_handle, const platform_i2c_t* peripheral );
void              platform_i2c_dma_tx_irq                 ( const platform_i2c_t* i2c );
void              platform_i2c_dma_rx_irq                 ( const platform_i2c_t* i2c );
void              platform_i2c_er_irq                     ( const platform_i2c_t* i2c );
void              platform_i2c_ev_irq                     ( const platform_i2c_t* i2c );

uint8_t           platform_pwm_get_port_number            ( platform_pwm_port_t* pwm_port );
platform_result_t platform_pwm_get_divider                ( platform_pwm_port_t* pwm_port, uint32_t frequency, uint32_t* div_period, uint32_t* div_prescaler );

void              platform_hal_qspi_mspinit               ( QSPI_HandleTypeDef* hqspi );
void              platform_hal_qspi_mspdeinit             ( QSPI_HandleTypeDef* hqspi );

platform_result_t platform_rtc_init                       ( void );

#ifdef __cplusplus
} /* extern "C" */
#endif
