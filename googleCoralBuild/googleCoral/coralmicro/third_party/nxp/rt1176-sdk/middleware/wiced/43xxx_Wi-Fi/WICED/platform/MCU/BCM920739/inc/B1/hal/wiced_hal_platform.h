/*
 * Copyright 2014, Broadcom Corporation
 * All Rights Reserved.
 *
 * This is UNPUBLISHED PROPRIETARY SOURCE CODE of Broadcom Corporation;
 * the contents of this file may not be disclosed to third parties, copied
 * or duplicated in any form, in whole or in part, without the prior
 * written permission of Broadcom Corporation.
 */

/** @file
*
* Defines peripherals available for use on BCM920706_P49/BCM920707_P49 and BCM92070X_B49 board.
* Use B49=1 in make target for 2070x_eval_B49 package.
*
*/

#pragma once

#include "wiced_hal_gpio.h"

/** \addtogroup Platfrom config - Peripherals pin configuration
*   \ingroup HardwareDrivers
*/
/*! @{ */

/******************************************************
 *                   Enumerations
 ******************************************************/

#define WICED_GPIO_PIN_BUTTON                     WICED_P01      /* pin for button interrupts */
#define WICED_GPIO_BUTTON_SETTINGS                ( GPIO_INPUT_ENABLE | GPIO_PULL_DOWN | GPIO_EN_INT_RISING_EDGE )

#define WICED_BUTTON_PRESSED_VALUE                 1

#define WICED_GPIO_LED_SETTINGS                   ( GPIO_OUTPUT_ENABLE )
#define WICED_GPIO_LED_ON_VAL                     0
#define WICED_GPIO_PIN_LED1                       WICED_P26      /* pin for LED               */
#define WICED_GPIO_PIN_LED2                       WICED_P27      /* pin for LED               */ //Note: when using this, WICED_PUART_TXD cannot be 31

#define WICED_PUART_TXD                           WICED_P33      /* pin for PUART TXD         */
#define WICED_PUART_RXD                           WICED_P34      /* pin for PUART RXD         */

#define HCI_UART_MAX_BAUD   4000000

/* @} */
