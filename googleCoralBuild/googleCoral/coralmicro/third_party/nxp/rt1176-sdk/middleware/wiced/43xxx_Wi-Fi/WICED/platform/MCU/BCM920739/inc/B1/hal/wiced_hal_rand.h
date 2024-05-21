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
* List of parameters and defined functions needed to utilize the
*
*
*/

#ifndef __WICED_RAND_H__
#define __WICED_RAND_H__

#include "brcm_fw_types.h"

/**  \addtogroup RandomNumberGenerator
* \ingroup HardwareDrivers
*/
/*! @{ */
/**
* Defines a driver for the Random Number Generator (RNG). The RNG uses a
* special hardware module to generate either a single 32-bit random number or
* fill a given array with 32-bit random numbers. These are useful for
* applications such as authentication.
*
*/

/******************************************************************************
*** Function prototypes.
******************************************************************************/

///////////////////////////////////////////////////////////////////////////////
/// Generates and returns a random 32-bit integer. Internal functions check
/// that the generating hardware is warmed up and ready before returning
/// the random value. If the hardware is too "cold" at the time of use,
/// the function will instead use the BT clock as a "seed" and generate a
/// "soft" random number.
///
/// \param none
///
/// \return A randomly generated 32-bit integer.
///////////////////////////////////////////////////////////////////////////////
uint32_t wiced_hal_rand_gen_num(void);


///////////////////////////////////////////////////////////////////////////////
/// Fills a given array with randomly generated 32-bit integers. Uses the
/// function wiced_hal_rand_gen_num().
///
/// \param randNumberArrayPtr - Pointer to an array to be populated with
///                             the random numbers.
/// \param length             - Length of the array pointed to by
///                             randNumberArrayPtr.
///
/// \return none
///////////////////////////////////////////////////////////////////////////////
void wiced_hal_rand_gen_num_array(uint32_t* randNumberArrayPtr, uint32_t length);


#endif // __WICED_RAND_H__
