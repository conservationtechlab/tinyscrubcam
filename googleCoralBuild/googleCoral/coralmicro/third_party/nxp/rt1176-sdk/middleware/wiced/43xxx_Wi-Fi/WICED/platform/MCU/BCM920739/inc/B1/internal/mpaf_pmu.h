/*
********************************************************************
* THIS INFORMATION IS PROPRIETARY TO
* BROADCOM CORP.
*-------------------------------------------------------------------
*                                                                        
*           Copyright (c) 2013 Broadcom Corp.
*                      ALL RIGHTS RESERVED                              
*                                                                       
********************************************************************

********************************************************************
*    File Name: mpaf_pmu.h
*
*    Abstract: This file defines the low level mpaf pmu and lpo 
*               functionality
*
*
********************************************************************
*/
#ifndef __MPAF_PMU_H__
#define __MPAF_PMU_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "foundation/brcm_fw_types.h"
#include "foundation/hal/pmu/pmu_sleep.h"
#include "foundation/hal/pmu/pmu_lpo.h"

// wakeup sources in cr_wake_int_en_2_adr
#define MPAF_PMU_WAKE_SOURCE2   (MPAF_PMU_WAKE_SOURCE_KEYSCAN | MPAF_PMU_WAKE_SOURCE_QUAD)

// wakeup sources in cr_wake_int_en_4_adr
#define MPAF_PMU_WAKE_SOURCE4   MPAF_PMU_WAKE_SOURCE_LHL

//sr_pmu_status_adr[21] = combined mia + lhl
#define MPAF_PMU_SR_MIA_LHL_WAKE_SRC                 (1 << 21)
#define MPAF_PMU_SR_MIA_LHL_WAKE_CLR                 (1 << 21)

#define MPAF_PMU_ENABLE_MIA_LPO_SELECT					0x01
#define MPAF_PMU_SELECT_32K_OSC_LPO					    0x02
#define MPAF_PMU_POWER_UP_RTC_COUNTER_AND_32K_OSC       0x04

// bit flags definitions for mpaf_pmu_Control
#define MPAF_PMU_CONTROL_INTERNAL_LPO_OFF_ENABLE        0x01 // power down internal LPO
#define MPAF_PMU_CONTROL_XTAL_LOW_POWER_ENABLE          0x02 // put XTAL in low power mode


typedef void (*MPAF_PMU_EARLY_WAKE_NOTIFICATION_CALLBACK)(void);
void mpaf_pmu_registerForEarlyWakeNotification(MPAF_PMU_EARLY_WAKE_NOTIFICATION_CALLBACK pcb);
void mpaf_pmu_setMinTimeToSleepForEnablingEarlyWake(UINT32 sleepTimeInUsecs);
void mpaf_pmu_sleepHandlerInit(void);
void mpaf_pmu_changeLPOSourceSelection(PMU_LPO_CLK_SOURCE lpoSource, UINT16 driftRate);
void mpaf_pmu_selectEnhancedLPOSource(PMU_LPO_CLK_SOURCE pmu_clkSource);

#ifdef __cplusplus
}
#endif

#endif // __MPAF_PMU_H__

