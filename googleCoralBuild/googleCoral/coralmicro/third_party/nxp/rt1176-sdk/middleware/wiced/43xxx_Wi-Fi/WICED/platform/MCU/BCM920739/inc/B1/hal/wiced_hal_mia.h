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
* Multiple Interface Adapter (MIA) driver.
*
*/

#ifndef __WICED_MIA_H__
#define __WICED_MIA_H__

#include "brcm_fw_types.h"

/**  \addtogroup MIADriver
* \ingroup HardwareDrivers
*/
/*! @{ */
/**
* Defines a driver to facilitate interfacing with various components of the
* hardware. The MIA driver controls aspects such as GPIO pin muxing, interrupt
* managment, and timing.
*
* Use this driver to enable and disable interrupts, along with managing
* hardware events (such as PUART RX). There are many possibilities--please
* look over the following API functions to see what can be done.
*/

/******************************************************************************
*** Function prototypes and defines.
******************************************************************************/

///////////////////////////////////////////////////////////////////////////////
/// Initializes the MIA driver and its private values. Also programs all
/// relevant GPIOs to be ready for use. This must be invoked before
/// accessing any MIA driver services, typically at boot.
/// This is independent of other drivers and must be one of the first to
/// be initialized.
///
/// \param none
///
/// \return none
///////////////////////////////////////////////////////////////////////////////
void wiced_hal_mia_init(void);


///////////////////////////////////////////////////////////////////////////////
/// Determines and returns if the reason for the last reset was from Power
/// On (Power On Reset).
///
/// \param none
///
/// \return reason - 1 if reset was caused by Power On Reset, 0 otherwise.
///////////////////////////////////////////////////////////////////////////////
BOOL32 wiced_hal_mia_is_reset_reason_por(void);


///////////////////////////////////////////////////////////////////////////////
/// Enable or disable all MIA-based interrupts.
///
/// \param enable - 1 to enable interrupts, 0 to disable interrupts.
///
/// \return none
///////////////////////////////////////////////////////////////////////////////
void wiced_hal_mia_enable_mia_interrupt(BOOL32 enable);


///////////////////////////////////////////////////////////////////////////////
/// Enable or disable all LHL-based interrupts.
///
/// \param enable - 1 to enable interrupts, 0 to disable interrupts.
///
/// \return none
///////////////////////////////////////////////////////////////////////////////
void wiced_hal_mia_enable_lhl_interrupt(BOOL32 enable);


///////////////////////////////////////////////////////////////////////////////
/// Get the MIA interrupt state.  This state may not be the
/// actual hardware MIA interrupt setting, since the interrupt
/// context interrupt handler will disable the MIA interrupt
/// without updating the state variable, which is a good thing
/// since it makes it easy to restore the correct hardware
/// setting from this state variable when done handling the
/// serialized interrupt by calling wiced_hal_mia_restoreMiaInterruptState().
///
/// \return 1 if MIA interrupts should be enabled, 0 if MIA interrupts should
/// be disabled.
///////////////////////////////////////////////////////////////////////////////
BOOL32 wiced_hal_mia_get_mia_interrupt_state(void);


///////////////////////////////////////////////////////////////////////////////
/// Get the LHL interrupt state.  This state may not be the
/// actual hardware LHL interrupt setting, since the interrupt
/// context interrupt handler will disable the LHL interrupt
/// without updating the state variable, which is a good thing
/// since it makes it easy to restore the correct hardware
/// setting from this state variable when done handling the
/// serialized interrupt by calling wiced_hal_restoreLhlInterruptSetting().
///
/// \return 1 if lhl interrupts should be enabled, 0 if lhl interrupts should
/// be disabled.
///////////////////////////////////////////////////////////////////////////////
BOOL32 wiced_hal_mia_get_lhl_interrupt_state(void);


///////////////////////////////////////////////////////////////////////////////
/// Restore the possibly overridden MIA interrupt setting to
/// that of the MIA interrupt state.  See wiced_hal_getMiaInterruptState()
/// for more details.
///
/// \return none
///////////////////////////////////////////////////////////////////////////////
void wiced_hal_mia_restore_mia_interrupt_state(void);


///////////////////////////////////////////////////////////////////////////////
/// Restore the possibly overridden LHL interrupt setting to
/// that of the LHL interrupt state.  See wiced_hal_getLhlInterruptState()
/// for more details.
///
/// \return none
///////////////////////////////////////////////////////////////////////////////
void wiced_hal_mia_restore_lhl_interrupt_state(void);


/* @} */

#endif // __WICED_MIA_H__
