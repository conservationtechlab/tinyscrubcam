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

#pragma once
#include "platform_constants.h"
#include "wwd_constants.h"
#include "ring_buffer.h"
#include "wwd_rtos.h"
//#include "wiced_io_manager.h"
#include "platform_constants.h"
#include "wiced_block_device.h"
#include "platform.h"
#include "platform_toolchain.h"

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
#define NUMBER_OF_GPIO_PORTS      (43)

#define NUMBER_OF_UART_PORTS      (2)
/* Default STDIO buffer size */
#ifndef STDIO_BUFFER_SIZE
#define STDIO_BUFFER_SIZE         (64)
#endif
#ifdef WICED_PLATFORM_INCLUDES_SPI_FLASH
#define PLATFORM_SFLASH_FREE_OFFSET (0)
#define PLATFORM_SFLASH_SIZE PLATFORM_SERIAL_FLASH_SIZE
#endif
#if 0
/* GPIOA to K */
#define NUMBER_OF_GPIO_PORTS      (11)

/* Interrupt line 0 to 15. Each line is shared among the same numbered pins across all GPIO ports */
#define NUMBER_OF_GPIO_IRQ_LINES  (16)

/* USART1 to 8 */
#define NUMBER_OF_UART_PORTS      (2)

/* SPI1 to SPI6 */
#define NUMBER_OF_SPI_PORTS       (6)

/* I2C1 to I2C3 */
#define NUMBER_OF_I2C_PORTS       (3)
#endif

/* Invalid UART port number */
#define INVALID_UART_PORT_NUMBER  (0xff)

#define SPI1_CLK_MUX     0x504
#define SPI1_CS_MUX        0x514
#define SPI1_MOSI_MUX    0x524
#define SPI1_MISO_MUX    0x534

#define SPI2_CLK_MUX     0x584
#define SPI2_CS_MUX        0x594
#define SPI2_MOSI_MUX    0x5a4
#define SPI2_MISO_MUX    0x5b4

/******************************************************
 *                   Enumerations
 ******************************************************/
typedef enum
{
    WICED_PLATFORM_INTERFACE_GPIO, WICED_PLATFORM_INTERFACE_I2C, WICED_PLATFORM_INTERFACE_PWM, WICED_PLATFORM_INTERFACE_SPI, WICED_PLATFORM_INTERFACE_MAX, /* Denotes the total number of UART port aliases. Not a valid UART alias */
} platform_interface_t;

/******************************************************
 *                 Type Definitions
 ******************************************************/
typedef enum
{
    SPI1, SPI2, SPI_MAX
} platform_spi_port_t;

typedef struct
{
        int dummy;
} USART_TypeDef;
typedef struct
{
        int dummy;
} SPI_TypeDef;
typedef struct
{
        int dummy;
} I2C_TypeDef;
typedef struct
{
        int dummy;
} ADC_TypeDef;

typedef struct
{
        uint32_t config;
        uint32_t pin_number;
        uint16_t mux_mode;
        uint8_t interrupt;
        void* callback;
        void* arg;
} platform_gpio_port_t;

/* UART port */
typedef USART_TypeDef platform_uart_port_t;

/* I2C port */
typedef I2C_TypeDef platform_i2c_port_t;

/* GPIO alternate function */
typedef uint8_t platform_gpio_alternate_function_t;

typedef enum
{
    PLATFORM_TICK_POWERSAVE_MODE_TICKLESS_ALWAYS,
    PLATFORM_TICK_POWERSAVE_MODE_TICKLESS_NEVER,
    PLATFORM_TICK_POWERSAVE_MODE_MAX /* Denotes max value. Not a valid mode */
} platform_tick_powersave_mode_t;

typedef enum
{
    PLATFORM_MCU_POWERSAVE_MODE_DEEP_SLEEP,
    PLATFORM_MCU_POWERSAVE_MODE_SLEEP,
    PLATFORM_MCU_POWERSAVE_MODE_MAX /* Denotes max value. Not a valid mode */
} platform_mcu_powersave_mode_t;


/******************************************************
 *                    Structures
 ******************************************************/

typedef struct
{
        int dummy;
} platform_dma_config_t;

typedef struct
{
        platform_gpio_port_t* port;
        uint8_t pin_number;
        uint16_t mux_mode;
} platform_gpio_t;

/*
 * channel - specifies the ADC channel
 * Pin - specifies the corresponding GPIO pin,
 * this we may need to set the mux if required.
 * */
typedef struct
{
        uint8_t channel;
        const platform_gpio_t* pin;
} platform_adc_t;
/*ADC input voltage range selection
 *this needs to match with MCU supported range
 *defined in wiced_hal_adc.h */
typedef enum
{
    PLATFORM_ADC_RANGE_0_3P6V = 0, PLATFORM_ADC_RANGE_0_1P8V = 1,
} PLATFORM_ADC_VOLRANGE_SEL;

typedef struct
{
        uint8_t channel;
        uint8_t invert;
        const platform_gpio_t* pin;
} platform_pwm_t;

/* DMA can be enabled by setting SPI_USE_DMA */
typedef struct
{
        //int dummy;
        platform_spi_port_t port;
        const platform_gpio_t* pin_mosi;
        const platform_gpio_t* pin_miso;
        const platform_gpio_t* pin_clock;
} platform_spi_t;

typedef struct
{
        void* peripheral;
        const struct platform_spi_slave_config* config;
        host_semaphore_type_t transfer_complete;
//    platform_spi_slave_state_t              state;
} platform_spi_slave_driver_t;

typedef struct
{
        uint32_t bus_id;
        const platform_gpio_t* pin_scl;
        const platform_gpio_t* pin_sda;
} platform_i2c_t;

typedef struct
{
        uint32_t device_id;
        const platform_gpio_t* tx_pin;
        const platform_gpio_t* rx_pin;
        const platform_gpio_t* cts_pin;
        const platform_gpio_t* rts_pin;
} platform_uart_t;

typedef struct
{
        uint32_t baud_rate;
        uint32_t data_width;
        uint32_t parity;
        uint32_t stop_bits;
        uint32_t flow_control;
} platform_uart_conf_t;

typedef enum
{
    PLATFORM_UART_IOCTL_SET_SPEED,                  ///< Command for setting the UART bus's speed.
    PLATFORM_UART_IOCTL_GET_SPEED,                  ///< Command for reading the UART current bus's speed.
    PLATFORM_UART_IOCTL_FLUSH_RX_BUF,               ///< Flush UART rx buffer.
    PLATFORM_UART_IOCTL_ENABLE_HW_FLOW_CTRL,        ///< Enable HW Flow Control.
    PLATFORM_UART_IOCTL_DISABLE_HW_FLOW_CTRL,       ///< Disable HW Flow Control.
    PLATFORM_UART_IOCTL_ENABLE_SW_FLOW_CTRL,        ///< Enable SW Flow Control.
    PLATFORM_UART_IOCTL_DISABLE_SW_FLOW_CTRL        ///< Disable SW Flow Control.
} platform_uart_ioctl_cmd_t;

typedef struct
{
        platform_uart_t peripheral;
        platform_uart_conf_t uart_config;
        wiced_ring_buffer_t* rx_buffer;
} platform_uart_driver_t;

typedef struct
{
        int dummy;
        /*DMA_TypeDef*                         dma_register;
         DMA_Stream_TypeDef*                  stream;
         uint32_t                             channel;
         peripheral_clock_t                   peripheral_clock;
         platform_peripheral_clock_function_t peripheral_clock_func;
         irq_vector_t                         irq;
         */
} platform_dma_t;

typedef struct
{
        int dummy;
        /*
         SPI_TypeDef*                         spi;
         uint8_t                              gpio_af;
         unsigned                             is_master   : 1;
         unsigned                             enable_mclk : 1;
         peripheral_clock_t                   peripheral_clock;
         platform_peripheral_clock_function_t peripheral_clock_func;
         const platform_gpio_t*               pin_ck;
         const platform_gpio_t*               pin_sd;
         const platform_gpio_t*               pin_ws;
         const platform_gpio_t*               pin_mclk;
         platform_dma_t                       tx_dma;
         platform_dma_t                       rx_dma;
         */
} platform_i2s_t;

typedef struct
{
        platform_gpio_t vol_up;
        platform_gpio_t vol_down;
        platform_gpio_t tap_key;
        platform_gpio_t back_key;
        platform_gpio_t power_key;
} platform_keypad_t;

typedef union
{
        platform_spi_t spi_config;
        platform_i2c_t i2c_config;
        platform_pwm_t pwm_config;
        platform_gpio_t gpio_config;
        platform_keypad_t keypad_config;
} platform_io_device_config_t;

typedef struct platform_io_device
{
        platform_interface_t io_interface;
//    wiced_io_device_common_t io_device;
        platform_io_device_config_t platform_config;
} platform_io_device_t;

typedef struct
{
        wiced_block_device_write_mode_t write_mode;
        uint32_t offset;
} sflash_block_device_specific_data_t;

typedef struct
{
        wiced_block_device_write_mode_t write_mode;
        uint32_t offset;
} ocf_block_device_specific_data_t;

/******************************************************
 *                 Global Variables
 ******************************************************/

/******************************************************
 *               Function Declarations
 ******************************************************/
void platform_init_external_devices( void );
void platform_adc_setinput_range( PLATFORM_ADC_VOLRANGE_SEL range );
platform_result_t platform_pwm_clk_init( void );
platform_result_t platform_set_pin_function( const platform_gpio_t* gpio );
platform_result_t platform_gpio_irq_manager_init( void );
uint8_t platform_gpio_get_port_number( platform_gpio_port_t* gpio_port );
platform_result_t platform_gpio_set_alternate_function( platform_gpio_port_t* gpio_port, uint8_t pin_number, uint32_t pull_up_down_type, uint32_t alternation_function );

platform_result_t platform_watchdog_init( void );
void platform_watchdog_reset_system( void ) NORETURN;
void platform_watchdog_disable( void );
platform_result_t platform_mcu_powersave_init( void );
void platform_mcu_powersave_set_tick_mode( platform_tick_powersave_mode_t mode );
platform_tick_powersave_mode_t platform_mcu_powersave_get_tick_mode( void );
void platform_mcu_powersave_set_mode( platform_mcu_powersave_mode_t );
platform_mcu_powersave_mode_t platform_mcu_powersave_get_mode( void );

void platform_mcu_release_power_lock( void );
void platform_mcu_acquire_power_lock( void );
void platform_mcu_pm_enable( void );
platform_result_t platform_rtc_init( void );

wiced_bool_t platform_mcu_powersave_is_warmboot( void );

uint8_t platform_uart_get_port_number( platform_uart_port_t* uart );
void platform_uart_irq( platform_uart_driver_t* driver );
void platform_uart_tx_dma_irq( platform_uart_driver_t* driver );
void platform_uart_rx_dma_irq( platform_uart_driver_t* driver );
platform_result_t platform_uart_ioctl( platform_uart_driver_t* driver, platform_uart_ioctl_cmd_t cmd, platform_uart_conf_t* conf );

void platform_i2s_irq( uint32_t i2s );
void platform_i2s_tx_dma_irq( uint32_t i2s );

uint8_t platform_spi_get_port_number( platform_spi_port_t* spi );
#ifdef __cplusplus
} /* extern "C" */
#endif

