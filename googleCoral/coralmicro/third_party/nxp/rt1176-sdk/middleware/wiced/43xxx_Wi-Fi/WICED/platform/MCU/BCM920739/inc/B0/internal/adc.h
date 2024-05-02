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
*    File Name: adc.h
*
*    Abstract: This file defines the ADC driver
*
*
********************************************************************
*/

#ifndef __ADC__H__
#define __ADC__H__

#include "brcm_fw_types.h"
#include "adc_cs.h"
#ifdef __cplusplus
extern "C" {
#endif

/**  \addtogroup ADC
 *  \ingroup HardwareDrivers
*/

// config ADC block to default/userconfig state 
void adc_config(void);

// calibration for the referenceInput
void adc_adcCalibrate(UINT32 refVoltageInMicroVolts, ADC_INPUT_CHANNEL_SEL referenceInput);

// read the adc value from hardware register as it is
INT16 adc_readSampleRaw(ADC_INPUT_CHANNEL_SEL channel_idx);


// read converted adc voltage
UINT32 adc_readVoltage(ADC_INPUT_CHANNEL_SEL idx);

ADC_POWER_TYPE adc_setPower(ADC_POWER_TYPE pwr);

void adc_SetInputRange(ADC_INPUT_RANGE_SEL range_idx);

/// Convert GPIO to ADC input
UINT8 adc_convertGPIOtoADCInput(UINT8 gpio);

/// Convert ADC input to GPIO
UINT8 adc_convertADCInputtoGPIO(UINT8 adcInput);


/* @} */

#ifdef __cplusplus
}
#endif

#endif
