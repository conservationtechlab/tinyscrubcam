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
*    File Name: pwm.h
*
*    Abstract: This file defines the pwm driver
*
*
********************************************************************
*/

#ifndef __PWM__H__
#define __PWM__H__

#include "brcm_fw_types.h"


/** \addtogroup  PWM
* \ingroup HardwareDrivers
*/
/*! @{ */
/**
* Defines the BCM standard pwm driver. It is compiled and used with
* BRCM standard applications [Mouse or Keyboard]. 
*/

/// PWM HW block has 4 PWMs Channels(10 bit)
/// This macros enable to switch from Pwm to its mask value. 
/// These mask values are used in enable/disable case.
#define pwmIdToMask(id)           (1<<id)

/// PWM HW block has 4 PWM channels each with its own 10 bit counter.
/// The first PWM id is PWM0;
enum
{
    PWM0  = 0,
    PWM1  = 1, 
    PWM2  = 2, 
    PWM3  = 3, 
    PWM4  = 4,
    PWM5  = 5,
    MAX_PWMS = 6
};
/// Clock used for PWM. When LHL_CLK is set, 128 KHz is used. When PMU_CLK is set, 1 MHz or 8 MHz.  
typedef enum
{
    LHL_CLK,
    PMU_CLK
} PwmClockType;

enum
{
    // PWM Channel Mask. 

    PWM_CHANNEL_MASK        =   0x300F
};

enum
{
    MAX_TOGGLE_COUNT        = 0xFFFF
};


BOOL32 pwm_start( UINT8 id, PwmClockType clk, UINT32 toggleCount, UINT32 InitCount );
BOOL32 pwm_transitionToSubstituteValues(UINT8 id, UINT32 toggleCount, UINT32 InitCount );
BOOL32 pwm_startWithAlternateValues( UINT8 id, PwmClockType clk,UINT32 toggleCount, UINT32 InitCount, BOOL32 invert );
UINT32 pwm_getToggleCount(UINT8 id);
UINT32 pwm_getInitValue(UINT8 id);

/// The following methods are for advanced users only
BOOL32 pwm_setToggleCount(UINT8 id, UINT32 toggleCount);
void pwm_setInversion(UINT8 id, BOOL32 invert);
BOOL32 pwm_setValues ( UINT8 id, UINT32 toggleCount, UINT32 initCount, PwmClockType clk, BOOL32 invert);
void pwm_setClock(UINT8 id, PwmClockType clk);
void pwm_setInitValue( UINT8 id, UINT32 InitCount );
void pwm_resetInternalCount(UINT32 mask);
void pwm_enableChannel(UINT32 mask);
void pwm_disableChannel(UINT32 mask);
void pwm_setReset(UINT32 mask, BOOL32 resetEnable);
/* @} */

#endif
