/*******************************************************************************
* ------------------------------------------------------------------------------
*
* Copyright (c), 2011 BROADCOM Corp.
*
*          ALL RIGHTS RESERVED
*
********************************************************************************
*
* File Name: gpiodriver.h
*
* This file implements the 20730 FW interface for GPIO configuration.
*******************************************************************************/

#ifndef __GPIO_DRIVER_H__
#define __GPIO_DRIVER_H__

#include "brcm_fw_types.h"
#include "hidddriversconfig.h"

#ifdef __cplusplus
extern "C" {
#endif

/**  \addtogroup GpioDriver
* \ingroup HardwareDrivers
*/
/*! @{ */
/**
* Defines a GPIO driver class. The BCM keyboard application or other applications use this
* driver to obtain the status from and control the behavior of the GPIO driver.
*/

///GPIO port and pin number constraints expressed in values.
enum 
{
    GPIO_NUM_PORTS = GPIO_NUMBER_OF_PORTS,
    GPIO_MAX_NUM_PINS_PER_PORT = GPIO_MAX_PINS_PER_PORT,
    GPIO_NUM_PINS = GPIO_NUMBER_OF_PINS
};

/// The following enum defines the constant values for the GPIO driver.
/// The first twelve entries are the configuration masks for GPIO.
/// A combination of them might be used together; for example a value
/// INPUT_DISABLE_MASK|OUTPUT_ENABLE_MASK can be used to set a pin.
/// The "configValue" of the struct "GpioConfigEntry" can be set
/// using that value (the logical or of configuration options).
/// Each of the pin can be configured in a specific way [using the value].
enum 
{
    /// iocfg_p#_adr ; 
    /// # = [0,1,2....37,38,39]
    /// Each GPIO has a set of configuration signals in one register ; 

    /// iocfg_p#_adr[0] = "edge_trigger"
    /// GPIO configuration bit 0, Interrupt type defines 
    /// Comments from User Guide: none
    GPIO_EDGE_TRIGGER_MASK       = 0x0001,
    GPIO_EDGE_TRIGGER            = 0x0001,
    GPIO_LEVEL_TRIGGER           = 0x0000,

    /// iocfg_p#_adr[1] = "neg_edge"
    /// GPIO configuration bit 1, Interrupt polarity defines
    /// Comments from User Guide: none
    GPIO_TRIGGER_POLARITY_MASK   = 0x0002,
    GPIO_TRIGGER_NEG             = 0x0002,

    /// iocfg_p#_adr[2] = "dual_edge"
    /// GPIO configuration bit 2, single/dual edge defines    
    /// Comments from User Guide: none
    GPIO_DUAL_EDGE_TRIGGER_MASK  = 0x0004,
    GPIO_EDGE_TRIGGER_BOTH       = 0x0004,
    GPIO_EDGE_TRIGGER_SINGLE     = 0x0000,


    /// iocfg_p#_adr[3] = "interrupt_en"
    /// GPIO configuration bit 3, interrupt enable/disable defines
    /// Comments from User Guide: This bit is used to mask GPIO asynch wake up (to Baseband PMU) and 
    /// synchronous interrupt status - gpio_int_st_px
    GPIO_INTERRUPT_ENABLE_MASK   = 0x0008,
    GPIO_INTERRUPT_ENABLE        = 0x0008,
    GPIO_INTERRUPT_DISABLE       = 0x0000,


    /// iocfg_p#_adr[0:3]
    /// GPIO configuration bit 0:3, Summary of Interrupt enabling type
    GPIO_EN_INT_MASK             = GPIO_EDGE_TRIGGER_MASK | GPIO_TRIGGER_POLARITY_MASK | GPIO_DUAL_EDGE_TRIGGER_MASK | GPIO_INTERRUPT_ENABLE_MASK,
    GPIO_EN_INT_LEVEL_HIGH       = GPIO_INTERRUPT_ENABLE | GPIO_LEVEL_TRIGGER,
    GPIO_EN_INT_LEVEL_LOW        = GPIO_INTERRUPT_ENABLE | GPIO_LEVEL_TRIGGER | GPIO_TRIGGER_NEG, 
    GPIO_EN_INT_RISING_EDGE      = GPIO_INTERRUPT_ENABLE | GPIO_EDGE_TRIGGER, 
    GPIO_EN_INT_FALLING_EDGE     = GPIO_INTERRUPT_ENABLE | GPIO_EDGE_TRIGGER | GPIO_TRIGGER_NEG,
    GPIO_EN_INT_BOTH_EDGE        = GPIO_INTERRUPT_ENABLE | GPIO_EDGE_TRIGGER | GPIO_EDGE_TRIGGER_BOTH,


    /// iocfg_p#_adr[4:5] = "GPIO o/p buffer control" AND 
    /// iocfg_p#_adr[14] = "o/p value multiplexing control"
    /// GPIO configuration bit 4:5,and 14 output enable control and muxing control
    /// Comments from the User Guide : none
    #if defined BCM20739
    GPIO_INPUT_ENABLE            = 0x0000,
    GPIO_OUTPUT_DISABLE          = 0x0000,
    GPIO_OUTPUT_ENABLE           = 0x0000,
    GPIO_KS_OUTPUT_ENABLE        = 0x0001, //Keyscan Output enable
    GPIO_OUTPUT_FN_SEL_MASK      = 0x0000,
    GPIO_OUTPUT_FN_SEL_SHIFT     = 0,
    #else
    GPIO_INPUT_ENABLE            = 0x0000,
    GPIO_OUTPUT_DISABLE          = 0x0000,
    GPIO_OUTPUT_ENABLE           = 0x4000,
    GPIO_KS_OUTPUT_ENABLE        = 0x0010, //Keyscan Output enable
    GPIO_OUTPUT_FN_SEL_MASK      = 0x0030,
    GPIO_OUTPUT_FN_SEL_SHIFT     = 4,
    #endif

    /// iocfg_p#_adr[6]
    /// GPIO configuration bit 6, "Global_input_disable" Disable bit
    /// This bit when set to "1" , P0 input_disable signal will control ALL GPIOs. 
    /// Default value (after power up or a reset event) is "0".
    GPIO_GLOBAL_INPUT_ENABLE     = 0x0000,
    GPIO_GLOBAL_INPUT_DISABLE    = 0x0040,

    /// iocfg_p#_adr[7]
    /// GPIO configruation bit 7, 
    /// reserved

    /// iocfg_p#_adr[8]
    /// GPIO configruation bit 8,  
    /// reserved

    /// iocfg_p#_adr[10:9] = "pullup/pulldown"
    /// GPIO configuration bit 9 and bit 10, pull- up and pull-down enable
    /// Comments from the User Guide: default value is [0,0] , means no pull resistor.
    GPIO_PULL_UP_DOWN_NONE       = 0x0000,   //[0,0]
    GPIO_PULL_UP                 = 0x0400,   //[1,0]
    GPIO_PULL_DOWN               = 0x0200,   //[0,1]
    GPIO_INPUT_DISABLE           = 0x0600,   //[1,1] //input disables the GPIO

    /// iocfg_p#_adr[11] =
    /// GPIO configuration bit 11, 
    /// iocfg_p0[11] controls the drive strength of the following GPIOs p[0:25], p[30:39];
    /// iocfg_p26[11] controls the drive strength of the GPIOs p[26:29]
    GPIO_DRIVE_SEL_MASK         = 0x0800,  
    GPIO_DRIVE_SEL_LOWEST       = 0x0000,  // 2mA @ 1.8V
    GPIO_DRIVE_SEL_MIDDLE_0     = 0x0000,  // 4mA @ 3.3v
    GPIO_DRIVE_SEL_MIDDLE_1     = 0x0800,  // 4mA @ 1.8v
    GPIO_DRIVE_SEL_HIGHEST      = 0x0800,  // 8mA @ 3.3v

    /// iocfg_p#_adr[12]
    /// GPIO configruation bit 12,  
    /// reserved

    /// iocfg_p#_adr[13] = "Input Hystersis"
    /// GPIO configuration bit 13, hysteresis control
    /// iocfg_p0[13] controls the hysteresis of the following GPIOs p[0:25], p[30:39];
    /// iocfg_p26[13] controls the hysteresis of the GPIOs p[26:29]
    GPIO_HYSTERESIS_MASK         = 0x2000,        
    GPIO_HYSTERESIS_ON           = 0x2000,
    GPIO_HYSTERESIS_OFF          = 0x0000,
};

/// Pin output config
typedef enum 
{
    GPIO_PIN_OUTPUT_LOW,
    GPIO_PIN_OUTPUT_HIGH
}tGPIO_PIN_OUTPUT_CONFIG;


typedef struct GpioRegistration
{
    UINT16 masks[GPIO_NUM_PORTS];
    void (*userfn)(void*, UINT8);
    void *userdata;
    struct GpioRegistration *next;
} GpioRegistration;

void gpio_init(void);
/// ConfigurePin maps a GPIO pin to a config value.
void gpio_configurePin(BYTE port, BYTE pin, UINT16 config, BYTE outputVal);
void gpio_configurePinWithSingleBytePortPinNum(BYTE gpio, UINT16 config, BYTE outputVal);

/// Configures all GPIO pins according to the values from the configuration record
void gpio_configureAll(void);

/// Retrieve the configuration of the specified pin
UINT16 gpio_getPinConfig(BYTE port, BYTE pin);

/// Set pin output for a specific pin.
void gpio_setPinOutput(BYTE port, BYTE pin, BYTE val);

void gpio_lhlInterrupt(void);

void gpio_registerForInterrupt(UINT16 masks[], 
                               void (*userfn)(void*, UINT8), 
                                 void* userdata); 

// Validates the port and pin number
BOOL32 gpio_validatePortPin(BYTE port, BYTE pin);

/// Handle GPIO entries from config record. The number of
/// entries is passed in along with a pointer to the entries.
/// This function updates the internal default configuration for the pins.
/// It does not change the HW configuration. Hardware configuration is only changed
/// when "gpioConfigureAll" is called. Pins which do not have an entry
/// are set to "input disable"    
void gpio_processConfigEntry(BYTE numEntries, GpioConfigEntry *configEntries);

////////////////////////////////////////////////////////////////////////////////////
/// Get the programmed output value of a pin. Note that this does not return the 
/// current pin value. It returns the value that the pin would attempt to drive 
/// if it is configured as output. 
/////////////////////////////////////////////////////////////////////////////////
BYTE gpio_getPinOutput(BYTE port, BYTE pin);

//////////////////////////////////////////////////////////////////////////////////
/// Read the current value at a pin. Note that for this to be valid, the pin
/// must be configured with input enabled.
///////////////////////////////////////////////////////////////////////////////////
BYTE gpio_getPinInput(BYTE port, BYTE pin);

///////////////////////////////////////////////////////////////////////////////////
///  Get the interrupt status of a pin
///////////////////////////////////////////////////////////////////////////////////
BYTE gpio_getPinInterruptStatus(BYTE port, BYTE pin);

///////////////////////////////////////////////////////////////////////////////////
/// Clear the interrupt status of a pin
///////////////////////////////////////////////////////////////////////////////////
void gpio_clearPinInterruptStatus(BYTE port, BYTE pin);

///////////////////////////////////////////////////////////////////////////////////
///  Get the programmed output value of a port.
///  \param port - port id(0-4). Must be in range or results are undefined.
///  \return  Returns the value that the pins on the port would attempt to 
///              drive if they are configured as outputs. 
///              It does not return the current port value.
///////////////////////////////////////////////////////////////////////////////////
UINT16 gpio_getPortOutput(BYTE port);

///////////////////////////////////////////////////////////////////////////////////
///  Set the output value of a port. This only affects pins which are configured as 
/// outputs. Pins which are not outputs will still get the specified value written
/// in the output register, but since they are not configured as utputs, the value
/// will not be driven out on the pin.
///  \param port - port id(0-3). Must be in range or results are undefined.
///  \param val  - value to be set
////////////////////////////////////////////////////////////////////////////////////
void gpio_setPortOutput(BYTE port, UINT16 val) ;

///////////////////////////////////////////////////////////////////////////////////
///  Read the current value at a port. For this to be valid, the pins must be
/// configured with input enabled. Pins which are configured with input disabled 
/// will always return the same value (0 or 1) but the value is not specified.
///  \param port - port id(0-1). Must be in range or results are undefined.
///  \return Port input value.
////////////////////////////////////////////////////////////////////////////////////

UINT16 gpio_getPortInput(BYTE port);

///////////////////////////////////////////////////////////////////////////////////
///  Get the interrupt status of a port. Pins which are not used for interrupts and 
/// do not have input disabled may report interrupts. Its the user's responsibility
/// to mask out these bits.
///  \param port - port id(0-4). Must be in range or results are undefined.
///  \return  Interrupt status for the port.
///////////////////////////////////////////////////////////////////////////////////
UINT16 gpio_getPortInterruptStatus(BYTE port);

/////////////////////////////////////////////////////////////////////////////////////
///  Clear the interrupt status of multiple bits in a port. The mask defines which 
/// interrupt bits are cleared. Only bits that are used for edge/level detection/interrupt 
/// should be cleared. Other bits should be left alone.
///  \param port - port id(0-4). Must be in range or results are undefined.
///  \param mask -  mask of which bits are cleared
/////////////////////////////////////////////////////////////////////////////////////
void gpio_clearPortInterruptStatus(BYTE port, UINT16 mask);


///////////////////////////////////////////////////////////////////////////////
// Configures all the Gpios between startPin to endPin of a particular port 
////////////////////////////////////////////////////////////////////////////////
void gpio_configurePortPins(BYTE port, BYTE startPin, BYTE endPin, UINT16 Config, BYTE outputVal);


///////////////////////////////////////////////////////////////////////////////
// Configures all gpios to be INPUT DISABLED
///////////////////////////////////////////////////////////////////////////////
void gpio_inputDisableAll(void);


/// @}

#ifdef __cplusplus
}
#endif

#endif

