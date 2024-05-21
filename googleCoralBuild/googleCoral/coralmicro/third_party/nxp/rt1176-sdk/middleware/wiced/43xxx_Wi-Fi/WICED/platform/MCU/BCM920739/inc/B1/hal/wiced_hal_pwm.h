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
* List of parameters and defined functions needed to access the
* Pulse-Width Modulation (PWM) driver.
*
*/

#ifndef __WICED_PWM_H__
#define __WICED_PWM_H__

#include "brcm_fw_types.h"

/**  \addtogroup PwmDriver
* \ingroup HardwareDrivers
*/
/*! @{ */
/**
* Defines a driver to facilitate interfacing with the Pulse-Width
* Modulation (PWM) driver.
*
* Use this driver to output a PWM signal to a GPIO pin for external use. There
* are four, 10-bit hardware PWM channels avaliable (0-3). Typical use-cases
* include fine-control over small devices such as LEDs. Please reference the
* User Documentation for more information.
*
*/

/******************************************************************************
*** Parameters.
***
*** The following parameters are used to configure the driver or define
*** return status. They are not modifiable.
******************************************************************************/

/// PWM HW block has 4 PWM channels each with its own 10 bit counter.
/// The first PWM channel is PWM0.
enum
{
    PWM0  = 0,
    PWM1  = 1,
    PWM2  = 2,
    PWM3  = 3,
    MAX_PWMS = 4

};

/// Clock used for PWM. When LHL_CLK is set, 128 KHz is used.
/// PMU_CLK requires aclk to be configured first.
typedef enum
{
    LHL_CLK,
    PMU_CLK
} PwmClockType;

/******************************************************************************
*** Function prototypes and defines.
******************************************************************************/
extern void pwm_enableChannel(UINT32 mask);
extern void pwm_disableChannel(UINT32 mask);
///////////////////////////////////////////////////////////////////////////////
/// Configures, enables, and starts the PWM to be active on a
/// preconfigured GPIO pin.
///
/// (!) Note that the desired GPIO pin must have already been configured
/// as output. See the HW muxing section in the User Documentation for
/// more information.
///
/// The following example outputs a signal on P28 using the PMU clock:
///
/// \verbatim
/// UINT16 init_cnt = 0x3C0;
/// UINT16 toggle_cnt = 0x3DF;
/// wiced_hal_gpio_configurePin(28, GPIO_OUTPUT_ENABLE, 0);
/// wiced_hal_aclk_enable(256000, ACLK1, ACLK_FREQ_24_MHZ);
/// wiced_hal_pwm_start(PWM2, PMU_CLK, toggle_cnt, init_cnt, 0);
/// \endverbatim
///
/// (!) Note that the maximum width or period of the PWM is 0x3FF (10-bits).
///
/// (!) Note that if you use PMU_CLK instead of LHL_CLK, a call to
///     wiced_hal_aclk_enable() is required. When configuring aclk,
///     ACLK0 is not avaliable for use with PWM--only ACLK1.
///
/// (!) Note that each PWM<#> channel corresponds to a specific GPIO pin,
///     and said pin must be configured for output before PWM may use it.
///     Please reference the User Documentation for more information on which
///     pins support PWM on your platform.
///
/// \param channel     - Desired PWM channel to use [0-3].
/// \param clk         - PMU_CLK or LHL_CLK
/// \param toggleCount - The number of ticks to wait before toggling the signal.
/// \param initCount   - Initial value for the counter.
/// \param invert      - 1 to invert the signal.
///
/// \return 1 if PWM was successfully started, 0 otherwise.
///////////////////////////////////////////////////////////////////////////////
BOOL32 wiced_hal_pwm_start(UINT8        channel,
                                         PwmClockType clk,
                                         UINT32       toggleCount,
                                         UINT32       initCount,
                                         BOOL32       invert);

///////////////////////////////////////////////////////////////////////////////
/// Changes the PWM settings after the PWM HW has already been started.
///
/// (!) Note that the maximum width or period of the PWM is 0x3FF (10-bits).
///
/// \param channel     - Desired PWM channel to set [0-3].
/// \param toggleCount - The number of ticks to wait before toggling the signal.
/// \param initCount   - Initial value for the counter.
///
/// \return 1 if PWM was successfully changed, 0 otherwise.
///////////////////////////////////////////////////////////////////////////////
BOOL32 wiced_hal_pwm_change_values(UINT8 channel,
                                                 UINT32 toggleCount,
                                                 UINT32 initCount);

///////////////////////////////////////////////////////////////////////////////
/// Returns the current toggle count setting for the corresponding PWM channel.
///
/// \param channel - Desired PWM channel from which to obtain the toggle count.
///
/// \return The value at which the PWM is going to toggle.
///////////////////////////////////////////////////////////////////////////////
UINT32 wiced_hal_pwm_get_toggle_count(UINT8 channel);


///////////////////////////////////////////////////////////////////////////////
/// Returns the current initial count setting for the corresponding PWM channel.
///
/// \param channel - Desired PWM channel from which to obtain the initial count.
///
/// \return The initial count value of the PWM.
///////////////////////////////////////////////////////////////////////////////
UINT32 wiced_hal_pwm_get_init_value(UINT8 channel);

///////////////////////////////////////////////////////////////////////////////
/// Disables the PWM channel.
///
/// \param channel - Desired PWM channel to stop/disable.
///
/// \return VOID
///////////////////////////////////////////////////////////////////////////////
static INLINE void wiced_hal_pwm_disable(UINT8 channel)
{
    pwm_disableChannel(1<<channel);
}

///////////////////////////////////////////////////////////////////////////////
/// Enables the PWM channel which is already preconfigured.
///
/// \param channel - Desired PWM channel to enable.
///
/// \return VOID
///////////////////////////////////////////////////////////////////////////////
static INLINE void wiced_hal_pwm_enable(UINT8 channel)
{
    pwm_enableChannel(1<<channel);
}

/* @} */

#endif // __WICED_PWM_H__
