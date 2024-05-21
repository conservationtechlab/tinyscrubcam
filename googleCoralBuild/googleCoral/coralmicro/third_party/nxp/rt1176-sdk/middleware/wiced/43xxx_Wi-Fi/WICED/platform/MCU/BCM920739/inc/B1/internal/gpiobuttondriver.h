/*******************************************************************************
* ------------------------------------------------------------------------------
*
* Copyright (c), 2011 BROADCOM Corp.
*
*          ALL RIGHTS RESERVED
*
********************************************************************************
*
* File Name: gpiobuttondriver.h
*
* Abstract: This file defines the GPIO based button interface.
*
*******************************************************************************/

#ifndef __GPIO_BUTTON_DRIVER_H__
#define __GPIO_BUTTON_DRIVER_H__

#include "gpiodriver.h"


/** \addtogroup HardwareDrivers*/
/*! @{ */
/**
* This file defines the gpio based button interface. Buttons are
* implemented as keys on this platform. The hardware reports the
* state of the buttons as keys. This information is translated
* by the button interface into a byte representing various
* buttons.
*/

/*******************************************************************************
* Types and Defines
*******************************************************************************/

/*******************************************************************************
* Functions
*******************************************************************************/
// Initialize the button driver
void gpiobtn_init(UINT16 gpioMask[], UINT16 gpioConfig);

/// Disables wakeup from all buttons. Clears all pending events
void gpiobtn_wakeupDisable(void);

/// Enables wakeup from all buttons. Also clears any pending events.
/// Use config passed in constructor to configure GPIOs for wakeupEnable.
void gpiobtn_wakeupEnable(void);

/// Return the current button state as a bit mapped value. 0 for a button indicates that
/// the button is not pressed, 1 indicates the button is pressed.
/// \param port The port ID to read.
/// \return a 16 bit map representing the current state of the buttons.
UINT16 gpiobtn_getCurrentState(UINT8 port);

/// Flush any queued button activity in FW or HW and returns the current
/// state of the buttons.
void gpiobtn_flush(void);

/// Register for notification of changes.
void gpiobtn_registerForInterrupt(void (*userfn)(void*), void* userdata);

/// set the debounce delay.
void gpiobtn_setDebounce(UINT32 debDelayInUs );

/// Configure each Button GPIO
/// This configuration is used only for enabling wakeup.
void gpiobtn_configureButtonGpio(UINT16 gpioButtonConfig);

/// Interrupt Call back function
void gpiobtn_buttonPressDetected(void *userdata, UINT8 portPin);

/// Interrupt Process function
void gpiobtn_processButtonDetected(UINT8 portPin);

 /* @}  */
#endif
