/*
********************************************************************
* THIS INFORMATION IS PROPRIETARY TO
* BROADCOM CORP.
*-------------------------------------------------------------------
*                                                                        
*           Copyright (c) 2011 Broadcom Corp.
*                      ALL RIGHTS RESERVED                              
*                                                                       
********************************************************************

********************************************************************
*    File Name: scrolldriver.h
*
*    Abstract: BCM 2073xx scroll driver 
*
*
********************************************************************
*/

#ifndef __SCROLL_DRIVER_H__
#define __SCROLL_DRIVER_H__

#include "brcm_fw_types.h"


#ifdef __cplusplus
extern "C" {
#endif

/** \addtogroup  Scrolldriver
* \ingroup HardwareDrivers
*/
/*! @{ */
/**
* Defines a scroll driver that uses the quadrature HW.
*/


/// This function should turn off the scroll HW. This is used when entering
/// power-off mode (software or low battery)
void scroll_turnOff(void);

/// This function should turn on the scroll HW. This is used when exiting
/// power-off mode (software or low battery)
void scroll_turnOn(void);

/// Register for notification of changes.
void scroll_registerForInterrupt(void (*userfn)(void*), void* userdata);

/// This function should check for scroll activity and return it
/// \return scroll count.
INT16 scroll_getCount(void);
    
// Initialize the Scroll driver based on the configuration.
// Will also initialize the HW if the last reset was because of a
// power up condition
void scroll_init(void);

/* @}  */

#ifdef __cplusplus
}
#endif

#endif


