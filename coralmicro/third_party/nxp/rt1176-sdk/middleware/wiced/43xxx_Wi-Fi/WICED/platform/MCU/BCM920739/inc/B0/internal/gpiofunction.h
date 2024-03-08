/*
********************************************************************
* THIS INFORMATION IS PROPRIETARY TO
* BROADCOM CORP.
*-------------------------------------------------------------------
*
*           Copyright (c) 2009 Broadcom Corp.
*                      ALL RIGHTS RESERVED
*
********************************************************************

********************************************************************
*    File Name: gpiofunction.h
*
*    Abstract: This file implements a function controller to GPIO mapping
*
*
********************************************************************
*/
#ifndef __GPIO_FUNCTION_H__
#define __GPIO_FUNCTION_H__

#include "brcm_fw_types.h"
#include "gpiodriver.h"

/*
    Description of the values in function_numbers_t
    The names themselves explain the purpose.  The one case that needs a description is UNAVAILABLE
    It was intended that the g_GPIO_function_map would be initialized with UNAVAILABLE for two cases
    The first case is that the pin doesn't leave the package and therefore is UNAVAILABLE
    The second case is that the reference board does not expose that pin and therefore it is UNAVAILABLE
    That implies a matrix of g_GPIO_function_map arrays that are package and platform aware which may not be practical
    But the capability is built into the code anyway
*/

typedef enum function_numbers_e
{
    NONE = 0,
    I2C_1_SCL,
    I2C_1_SDA,
    I2C_2_SCL,
    I2C_2_SDA,
    SPI_1_CLK,
    SPI_1_CS,
    SPI_1_MOSI,
    SPI_1_MISO,
    SPI_1_IO2,
    SPI_1_IO3,
    SPI_1_INT,
    SPI_1_DCX,
    SPI_2_CLK,
    SPI_2_CS,
    SPI_2_MOSI,
    SPI_2_MISO,
    SPI_2_IO2,
    SPI_2_IO3,
    SPI_2_INT,
    SPI_2_DCX,
    SPI_3_CLK,
    SPI_3_CS,
    SPI_3_MOSI,
    SPI_3_MISO,
    SPI_3_INT,
    SWDCK,
    SWDIO,
    UART_1_TXD,
    UART_1_RXD,
    UART_1_CTS,
    UART_1_RTS,
    UART_2_TXD,
    UART_2_RXD,
    UART_2_CTS,
    UART_2_RTS,
    AOA_0,
    AOA_1,
    AOA_2,
    AOD_0,
    UNAVAILABLE = 0xFF
} function_numbers_t;

/*
    Description of return values in GPIO_status_t
    GPIO_FAILURE    The requested pin and function mapping is not supported by hardware
    GPIO_SUCCESS    The requested pin and function mapping is complete
                    The pin was previously not used and the function was previously not mapped
    GPIO_REMAPPED   The requested pin and function mapping is complete
                    The pin was previousely used by another function
                    That function was disabled and the new function applied
    GPIO_MOVED      The requested pin and function mapping is complete
                    The requested function was already mapped to a different pin
                    That pin was disabled and the function moved to the new pin
    Callers only need to check for the GPIO_FAILURE case since any other status means success
*/

typedef enum GPIO_STATUS_e
{
    GPIO_FAILURE,
    GPIO_SUCCESS,
    GPIO_REMAPPED,
    GPIO_MOVED
} GPIO_status_t;

typedef struct
{
    BOOL8 RetVal;
    UINT8 shift;
    UINT16 fcn;
    UINT32 premux_adr;
    UINT32 enable_adr;
    UINT32 enable_value;
    UINT32 enable_mask;
    UINT32 pin_config;
} function_config_info_t;

// Return which GPIO has the requested function, GPIO_MAX_NUM if not found
gpio_numbers_t gpio_function_Get_GPIO_from_Function(function_numbers_t function);

// Return the current function on the requested GPIO, NONE if not assigned
function_numbers_t gpio_function_Get_Function_from_GPIO(gpio_numbers_t gpio);

// Assign the requested function to the requested GPIO
GPIO_status_t gpio_function_Set_GPIO_Function(gpio_numbers_t gpio, function_numbers_t newfunction);

#endif
