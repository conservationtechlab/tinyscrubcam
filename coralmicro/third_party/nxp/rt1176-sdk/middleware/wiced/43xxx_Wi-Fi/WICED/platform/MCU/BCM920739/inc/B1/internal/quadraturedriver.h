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
*    File Name: quadraturedriver.h
*
*    Abstract: BCM 2073xx quadrature driver 
*
*
********************************************************************
*/

#ifndef __QUADRATURE_DRIVER_H__
#define __QUADRATURE_DRIVER_H__

#include "brcm_fw_types.h"
#include "gpiodriver.h"

/** \addtogroup  Quadrature
* \ingroup HardwareDrivers
*/
/*! @{ */
/**
* Defines a quadrature driver provided by the 20732 core.

* 
*/
enum
{
    ENABLE_PORT_2_PINS_AS_QUAD_INPUT    =   0,
    ENABLE_PORT_0_PINS_AS_QUAD_INPUT    =   1,
};

enum
{
    QUADRATURE_LED_CONFIG_OPEN_SOURCE   =   0x00,
    QUADRATURE_LED_CONFIG_SOURCE        =   0x01,
    QUADRATURE_LED_CONFIG_OPEN_DRAIN    =   0x02,
    QUADRATURE_LED_CONFIG_DRAIN         =   0x03
};
enum
{
    // Quadrature Sign bit
    QFLAG_SIGN                          = 0x0100,
    // Quadrature flag register bits qflag[2:0]
    MIA_QFLAG_OVERFLOW_X                = 0x01,
    MIA_QFLAG_OVERFLOW_Y                = 0x02,
    MIA_QFLAG_OVERFLOW_Z                = 0x04

};

enum
{
    CTRL_CLR_KEYS                  = 0x0004
};

enum
{
    CH_XY_128_KHZ_FIXED_SAMPLE_RATE    = 0x00,
    CH_XY_64_KHZ_FIXED_SAMPLE_RATE     = 0x01,
    CH_XY_32_KHZ_FIXED_SAMPLE_RATE     = 0x02,
    CH_XY_16_KHZ_FIXED_SAMPLE_RATE     = 0x03,

    CH_XY_SEL_LHL_PWM_RATE             = 0x00,
    CH_XY_SEL_FIXED_RATE               = 0x04,

    CH_XY_DISABLE                      = 0x00,
    CH_XY_ENABLE                       = 0x08,

    CH_Z_SAMPLE_ONCE_PER_LHL_PWM       = 0x00,
    CH_Z_SAMPLE_ONCE_PER_2_LHL_PWM     = 0x10,
    CH_Z_SAMPLE_ONCE_PER_4_LHL_PWM     = 0x20,
    CH_Z_SAMPLE_ONCE_PER_8_LHL_PWM     = 0x30,
    CH_Z_SAMPLE_ONCE_PER_16_LHL_PWM    = 0x40,
    CH_Z_SAMPLE_ONCE_PER_32_LHL_PWM    = 0x50,
    CH_Z_SAMPLE_ONCE_PER_64_LHL_PWM    = 0x60,
    CH_Z_SAMPLE_ONCE_PER_128_LHL_PWM   = 0x70,

    CH_Z_DISABLE                       = 0x00,
    CH_Z_ENABLE                        = 0x80
    
};

enum
{
    CONTROL_DISABLE                    = 0x00,
    CONTROL_ENABLE                     = 0x01,
    HW_CTRL_INTERRUPT_ENABLE           = 0x02,
    WAKEUP_INT_DISABLE                 = 0x00,
    WAKEUP_INT_ENABLE                  = 0x02,

    SCROLL_WITH_NO_DETENT_DISABLE      = 0x00,
    SCROLL_WITH_NO_DETENT_ENABLE       = 0x08

};

// Quadrature enable signal for LHL Domain (lhl_ctl_adr)
// Used for HID OFF support. These bits need to be set to enable
// waking up from HID OFF. 
enum
{
    QUAD_ENABLE_X_LHL                   = 0x0400,
    QUAD_ENABLE_Y_LHL                   = 0x0200,
    QUAD_ENABLE_Z_LHL                   = 0x0100

};

// Channel Selection for quadrature input
enum
{
    CH_SEL_FOR_QUAD_INPUT_PORT0 = 0x0000,
    CH_SEL_FOR_QUAD_INPUT_PORT2 = 0x0002
};

// Quadrature Interrupt Masks
enum
{
    HW_MIA_STATUS_QUAD_SET_MASK          = 0x00010,
    HW_MIA_STATUS_QUAD_INT_SET_MASK      = 0x00020
};

// Config for Muxing GPIO output to QOC
enum
{
    GPIO_OUTPUT_MUX_TO_QOC         = 0x0020
};

enum
{
    INTERRUPT_CONFIG_DISABLE            = 0x00,
    INTERRUPT_CONFIG_ENABLE             = 0x01
};

// RESERVED42 bit definitions
enum 
{

    QUADRATURE_RESERVED42_X_CTL_BIT_MASK    =  0x01,
    QUADRATURE_RESERVED42_Y_CTL_BIT_MASK    =  0x02,
    QUADRATURE_RESERVED42_Z_CTL_BIT_MASK    =  0x04,

    QUADRATURE_RESERVED42_ENABLE_X          =  0x01,
    QUADRATURE_RESERVED42_ENABLE_Y          =  0x02,
    QUADRATURE_RESERVED42_ENABLE_Z          =  0x04,

    QUADRATURE_RESERVED42_DISABLE_X         =  0x00,
    QUADRATURE_RESERVED42_DISABLE_Y         =  0x00,
    QUADRATURE_RESERVED42_DISABLE_Z         =  0x00,

    // Completely arbitrary! 
    QUADRATURE_RESERVED42_MAGIC_CONFIG      =  0xB3
};

// Index into the config array for Reserved42
enum
{
    QUADRATURE_CONFIG_RESERVED42_X_INDEX = 0,
    QUADRATURE_CONFIG_RESERVED42_Y_INDEX = 1,
    QUADRATURE_CONFIG_RESERVED42_Z_INDEX = 2
};
enum
{
    RESERVED42_PORT0_MUX    = 0x0010,
    RESERVED42_PORT2_MUX    = 0x0020
};


typedef struct QuadRegistration
{
    void (*userfn)(void*);
    void *userdata;
    struct QuadRegistration *next;
} QuadRegistration;


/// Return previously accumulated motion for a specific axis
INT16 quad_getXMotion(void);
INT16 quad_getYMotion(void);
INT16 quad_getZMotion(void);

/// Adds any currently accumulated X/Y motion to the X/Y
/// output variables. Note that the motion is added to the variables,
/// so the incoming value is important.
 void quad_getXYMotion(INT16 *x, INT16 *y);

// Disable the quadrature interface. This disables the quadrature interface polling and
// also disables wakeups from the quadrature
void quad_disable(void);

// Configures the quadrature interface enable/wakeup functionality to what has been configured
void quad_restoreActivity(void);

// Clears the HW queue
void quad_flushHwEvents(void);

// The 207xx HW requires us to read the qudarature counts
// whenever the MIA clock is frozen or the counts are lost. 
// This function should be tied to the MIA clock freeze notification
// if the quadrature interface is in use. Note that the axis polled
// can be configured in the config record. 
void quad_miaFreezeCallBack(void);

// Called to inform the quadrature interface that MIA clock is now unfrozen
void quad_miaUnfreezeCallBack(void); 

// Initialize the quadrature HW based on the configuration.
void quad_hwInit(void);

// Enables quadrature interrupt
void quad_enableInterrupt(void);

// Disables quadrature interrupt
void quad_disableInterrupt(void);

// Handle Quadrature Interrupt and Notify the relevant subscriber.
void quad_quadInterrupt(void);

// Register for notification of change(callback function)
void quad_registerForEventNotification(void (*userfn)(void*),void* userdata);

// Initialize the quadrature driver based on the configuration.
// Will also initialize the HW if the last reset was because of a
// power up condition
void quad_init(void);

// Configures upstream interrupt
void quad_enableEventDetection(void);

///  Configure the pins for the specified port for quadrature
/// operation. Also set the quadrature input select register to
/// use the specified pins. Port and pin values taken from the
/// congig
void quad_configurePortPins(void);

/// Quadrature driver data
typedef struct 
{
    /// X Motion detected since last time it was retrieved
    INT16   accumulatedXMotion;
    /// Y Motion detected since last time it was retrieved
    INT16   accumulatedYMotion;
    /// Z Motion detected since last time it was retrieved
    INT16   accumulatedZMotion;
    /// Flag indicating that the clock freeze was performed by the 
    /// quadrature interface and we shouldn't poll for motion
    BYTE    miaClkFrozenByQuadrature;
}QuadratureData;

/// Quadrature runtime state
typedef struct
{
    /// Event registration slist
    QuadRegistration* quadFirstReg;

    /// Quadrature interface run time data
    QuadratureData quadratureData;
} QuadratureState;


/* @}  */
#endif


