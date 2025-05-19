
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
* Auxiliary Clock (aclk) driver.
*
*/

#ifndef __WICED_ACLK_H__
#define __WICED_ACLK_H__


/**  \addtogroup AuxClkDriver
* \ingroup HardwareDrivers
*/
/*! @{ */
/**
* Defines a driver to facilitate interfacing with the auxiliary clock.
*
* Use this driver to output a clock to a GPIO pin for external use. Two clock
* sources are avaliable: clk0 or clk1, as well as two base frequencies used
* to calulate the prescaler : 1 MHz or 24 MHz. Note that the desired GPIO pin
* must have already been configured to output for aclk. See the HW muxing
* section in the User Documentation for more information.
*
*/

/******************************************************************************
*** Parameters.
***
*** The following parameters are used to configure the driver or define
*** return status. They are not modifiable.
******************************************************************************/

// Available clock sources.
enum CLK_SRC_SEL
{
    ACLK0,
    ACLK1,
};

// Available base clock frequencies.
enum CLK_SRC_FREQ_SEL
{
    ACLK_FREQ_1_MHZ,
    ACLK_FREQ_24_MHZ
};

/******************************************************************************
*** Function prototypes and defines.
******************************************************************************/

///////////////////////////////////////////////////////////////////////////////
/// Configures, enables, and starts the aclk to be active on a
/// preconfigured GPIO pin.
///
/// (!) Note that the desired GPIO pin must have already been configured
/// to output for aclk. See the HW muxing section in the User
/// Documentation for more information.
///
/// \param frequency   - Desired frequency to output; 0 is the same as disable.
/// \param clkSrc      - ACLK0 or ACLK1
/// \param baseFreqSel - ACLK_FREQ_1_MHZ or ACLK_FREQ_24_MHZ.
///
/// \return none
///////////////////////////////////////////////////////////////////////////////
void wiced_hal_aclk_enable(UINT32 frequency,
                                         UINT32 clkSrc,
                                         UINT32 baseFreqSel);


///////////////////////////////////////////////////////////////////////////////
/// Disables the desired auxilary clock source.
///
/// \param clkSrc - Clock source to disable (aclk0 or aclk1).
///
/// \return none
///////////////////////////////////////////////////////////////////////////////
void wiced_hal_aclk_disable(UINT32 clkSrc);

/* @} */

#endif // __WICED_ACLK_H__
