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
* Battery Monitor.
*
*/

#ifndef __WICED_BATMON_H__
#define __WICED_BATMON_H__

#include "brcm_fw_types.h"

/**  \addtogroup BatteryMonitorDriver
* \ingroup HardwareDrivers
*/
/*! @{ */
/**
* Defines a driver to facilitate interfacing with the Battery Monitor.
* The driver uses the Analog-to-Digital Converter (ADC) driver to take a
* moving average of the battery voltage, and decides when to suspend or
* shutdown the system based upon the threshold values defined by the user.
* This watchful protector ensures that the system is able to cleanly shut
* down and notify the application and/or host before there is not enough
* power necessary for proper system functionality. Note that the Battery
* Monitor relies on polling to check on the status of the battery.
*
*/

/******************************************************************************
*** Parameters.
***
*** The following parameters are used to configure the driver or define
*** return status. They are not modifiable.
******************************************************************************/

/**
/// Supported ADC input channel selection for battery connection.
typedef enum ADC_INPUT_CHANNEL_SEL {
    ADC_INPUT_P17           =   0x0,    //GPIO 16
    ADC_INPUT_P16           =   0x1,    //GPIO 17
    ADC_INPUT_P15           =   0x2,    //GPIO 18
    ADC_INPUT_P14           =   0x3,    //GPIO 19
    ADC_INPUT_P13           =   0x4,    //GPIO 20
    ADC_INPUT_P12           =   0x5,    //GPIO 21
    ADC_INPUT_P11           =   0x6,    //GPIO 22
    ADC_INPUT_P10           =   0x7,    //GPIO 23
    ADC_INPUT_P9            =   0x8,    //GPIO 24
    ADC_INPUT_P8            =   0x9,    //GPIO 25
    ADC_INPUT_P1            =   0xA,    //GPIO 26
    ADC_INPUT_P0            =   0xB,    //GPIO 27
    ADC_INPUT_VDDIO         =   0xC,    //ADC_INPUT_VBAT_VDDIO
    ADC_INPUT_VDD_CORE      =   0xD,    //ADC_INPUT_VDDC
    ADC_INPUT_ADC_BGREF     =   0xE,
    ADC_INPUT_ADC_REFGND    =   0xF,
    ADC_INPUT_P38           =   0x10,   //GPIO 0
    ADC_INPUT_P37           =   0x11,   //GPIO 1
    ADC_INPUT_P36           =   0x12,   //GPIO 2
    ADC_INPUT_P35           =   0x13,   //GPIO 3
    ADC_INPUT_P34           =   0x14,   //GPIO 4
    ADC_INPUT_P33           =   0x15,   //GPIO 5
    ADC_INPUT_P32           =   0x16,   //GPIO 6
    ADC_INPUT_P31           =   0x17,   //GPIO 7
    ADC_INPUT_P30           =   0x18,   //GPIO 8
    ADC_INPUT_P29           =   0x19,   //GPIO 9
    ADC_INPUT_P28           =   0x1A,   //GPIO 10
    ADC_INPUT_P23           =   0x1B,   //GPIO 11
    ADC_INPUT_P22           =   0x1C,   //GPIO 12
    ADC_INPUT_P21           =   0x1D,   //GPIO 13
    ADC_INPUT_P19           =   0x1E,   //GPIO 14
    ADC_INPUT_P18           =   0x1F,   //GPIO 15
    ADC_INPUT_CHANNEL_MASK  =   0x1f,
}ADC_INPUT_CHANNEL_SEL;
**/

/******************************************************************************
*** Function prototypes and defines.
******************************************************************************/
///////////////////////////////////////////////////////////////////////////////
/// Sets the configuration parameters of the Battery Monitor. This function is
/// *optional*, since the Battery Monitor parameters are pre-populated with
/// default values on startup. See the param descriptions below for the default
/// values.
///
/// \param adcInputConnected    - HW line connected to battery,
///                               used by ADC. Default = ADC_INPUT_VDDIO (P15).
/// \param measurementInterval  - Period in millisecs between battery
///                               measurements. Default = 60000 (60 seconds).
/// \param numMeasurementsToAvg - Number of measurements averaged for a report,
///                               max 16. Default = 8.
/// \param fullVoltage          - The nominal full battery voltage in
///                               millivolts. Should be greater than
///                               emptyVoltage and shutdownVoltage.
///                               Default = 3200 (3.2v).
/// \param emptyVoltage         - The voltage at which the battery is
///                               considered drained in millivolts. Should be
///                               less than fullVoltage but greater than
///                               shutdownVoltage. Default = 1800 (1.8v).
/// \param shutdownVoltage      - System should shutdown if it detects battery
///                               voltage at or below this value in millivolts.
///                               0 disables shutdown. Should be less than
///                               fullVoltage and emptyVoltage.
///                               Default = 1700 (1.7v).
/// \param maxLevel             - Sets the range of the reported number of
///                               steps. Set it to 100 to report battery as a
///                               percentage. Default = 100.
/// \param reportID             - ID of the battery report. Default = 0x03.
/// \param reportLength         - Length of the battery report in bytes.
///                               Default = 8.
/// \param reportOnConnect      - Flag indicating that a report should be sent
///                               to the host when a connection is established.
///                               Default = 1.
///
/// \return 1 if all parameters were successfully set, 0 if one or more
///         failures--check that the inputs were within their specified bounds.
///////////////////////////////////////////////////////////////////////////////
BYTE wiced_hal_batmon_config(UINT8  adcInputConnected,
                             UINT32 measurementInterval,
                             UINT8  numMeasurementsToAvg,
                             UINT16 fullVoltage,
                             UINT16 emptyVoltage,
                             UINT16 shutdownVoltage,
                             UINT8  maxLevel,
                             UINT8  reportID,
                             UINT8  reportLength,
                             UINT8  reportOnConnect);

///////////////////////////////////////////////////////////////////////////////
/// Initialize and then start the Battery Monitor. Also initializes the ADC
/// driver.
///
/// \param none
///
/// \return none
///////////////////////////////////////////////////////////////////////////////
void wiced_hal_batmon_init(void);


///////////////////////////////////////////////////////////////////////////////
/// Polls the battery by monitoring the voltage via the ADC. Takes a running
/// average of the measured voltage and determines whether to suspend or
/// shut down the system based upon the provided threshold values.
/// See wiced_hal_batmon_config().
///
/// \param none
///
/// \return none
///////////////////////////////////////////////////////////////////////////////
void wiced_hal_batmon_poll_monitor(void);


/* @} */

#endif // __WICED_BATMON_H__
