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
 *    File Name: syncedpulse.h
 *
 *    Abstract: This file defines the synced pulse driver
 *
 *
 ********************************************************************
 */

#ifndef __SYNCED_PULSE_H__
#define __SYNCED_PULSE_H__

#include "brcm_fw_types.h"
#include "gpiodriver.h"

/**  \addtogroup SyncedPulse
 *  \ingroup HardwareDrivers
 */
/*! @{ */

/// optctl_adr[11:8] defines.  Since these are also used by the IR RX module,
/// don't make them part of the SyncedPulse class. 
enum
{
    HW_LHL_OPTCTL_IR_SRC_MASK = 0x0F00, HW_LHL_OPTCTL_IR_SRC_IR2_3V_ALT3 = 0x0F00, HW_LHL_OPTCTL_IR_SRC_IR2_3V_ALT2 = 0x0E00, HW_LHL_OPTCTL_IR_SRC_P15_ALT3 = 0x0D00, HW_LHL_OPTCTL_IR_SRC_IR1_3V_ALT1 = 0x0C00, HW_LHL_OPTCTL_IR_SRC_IR2_3V_ALT1 = 0x0B00, HW_LHL_OPTCTL_IR_SRC_IR2_3V = 0x0A00, HW_LHL_OPTCTL_IR_SRC_P15_ALT2 = 0x0900, HW_LHL_OPTCTL_IR_SRC_P0_ALT1 = 0x0800, HW_LHL_OPTCTL_IR_SRC_P39_ALT3 = 0x0700, HW_LHL_OPTCTL_IR_SRC_P39_ALT2 = 0x0600, HW_LHL_OPTCTL_IR_SRC_P15_ALT1 = 0x0500, HW_LHL_OPTCTL_IR_SRC_IR1_3V = 0x0400, HW_LHL_OPTCTL_IR_SRC_P39_ALT1 = 0x0300, HW_LHL_OPTCTL_IR_SRC_P39 = 0x0200, HW_LHL_OPTCTL_IR_SRC_P15 = 0x0100, HW_LHL_OPTCTL_IR_SRC_P0_0 = 0x0000,
};

enum
{
    HW_LPO_SEL_MAIN60HZ_CNT_READBACK_MASK = 0x0004, HW_LPO_SEL_MAIN60HZ_CNT_READBACK_ENABLE = 0x0004, HW_LPO_SEL_MAIN60HZ_CNT_READBACK_DISABLE = 0x0000, HW_LPO_SEL_TRIAC_OUTPUT_INVERT_ALL_MASK = 0x00F0, HW_LPO_SEL_TRIAC1_OUTPUT_INVERT_SHIFT = 4,

};

/// Number of synced pulse channels hardware supports
#define NUM_SYNCED_PULSE_HW_CHANS   4

// needs to be outside of structure so can be in C accessible structure
// chan 0 must be gpio 26
// chan 1 must be gpio 27
// chan 2 can be either gpio 13 or 21
// chan 3 can be either gpio 14 or 22

/// Channel configuration info
typedef struct
{
        /// gpio / 16
        UINT8 port;
        /// gpio  % 16
        UINT8 pin;
        /// level on pin when chan is disabled
        UINT8 offState;
        /// invert triac output
        UINT8 invertOutput;
} syncp_chanConfig;

/// Configuration info
typedef struct
{
        /// Associated gpio configuration info
        syncp_chanConfig chan[ NUM_SYNCED_PULSE_HW_CHANS ];
        /// btclk trigger (e.g. 3D glasses) or input port trigger (e.g. dimmer)
        UINT8 mode;
        /// gpio / 16 -- only applicable for input port trigger mode
        UINT8 inputPort;
        /// gpio % 16 -- only applicable for input port trigger mode
        UINT8 inputPin;
        // True if should readback current main60hz count
        // False if should readback terminal count
        UINT8 enableMain60HzCntReadback;
        UINT8 usingA0Hardware;
} SyncedPulseConfig;

/* @} */

extern SyncedPulseConfig syncedPulseConfig;

/**  \addtogroup SyncedPulse
 *  \ingroup HardwareDrivers
 */
/*! @{ */
/**
 * Defines a bt resyncable synced pulse driver class. The BCM 3D glasses or
 * other application use this driver to control the duration of repetitive
 * pulses on the synced pulse output channels lines
 */

/// Indicates how output ports are to be treated when associated chan
/// is not enabled
typedef enum
{
    SYNCED_PULSE_OFF_STATE_LOW = 0, SYNCED_PULSE_OFF_STATE_HIGH = 1, SYNCED_PULSE_CHAN_NOT_USED
} SyncedPulseOffState;

/// Input trigger mode--either btclk or input port
typedef enum
{
    TRIGGER_MODE_BT_CLK, TRIGGER_MODE_INPUT_PORT
} SyncedPulseTriggerMode;

void syncp_init( SyncedPulseConfig *config );
void syncp_enableInputPortTriggerMode( void );
void syncp_enableBtClkTriggerMode( UINT32 intervalThreshold, UINT32 piconetIndex, UINT32 clk, UINT32 phaseCount );
void syncp_disableModule( void );
void syncp_enableChan( UINT32 chanNum, UINT32 onDelay, UINT32 offDelay );
void syncp_disableChan( UINT32 chanNum );
void syncp_setChanOnOffDelays( UINT32 chanNum, UINT32 onDelay, UINT32 offDelay );
void syncp_getChanOnOffDelays( UINT32 chanNum, UINT32 *pOnDelay, UINT32 *pOffDelay );
BOOL32 syncp_isChanControlEnabled( UINT32 chanNum );
BOOL32 syncp_isChanCurrentlyEnabled( UINT32 chanNum );
BOOL32 syncp_isModuleEnabled( void );
UINT32 syncp_getCurrentIntervalCounter( void );
UINT32 syncp_getIntervalThreshold( void );
SyncedPulseConfig *syncp_getConfig( void );
void syncp_setIntervalThreshold( UINT32 intervalThreshold );
void syncp_btPiconetClkSync( UINT32 piconetIndex, UINT32 clk, UINT32 phaseCount );
void syncp_btNativeClkSync( UINT32 piconetIndex, UINT32 clk, UINT32 phaseCount );
void syncp_convertFutureBtClk( UINT32 piconetIndex, UINT32 *pFutureClk, UINT32 *pFuturePhaseCount, UINT32 direction );
/// main60hz_ctl_adr bit defines
enum
{
    HW_MIA_60HZ_CTL_MODE_BT_CLK_MASK = 0x0100, HW_MIA_60HZ_CTL_MODE_BT_CLK_ENABLE = 0x0100, HW_MIA_60HZ_CTL_MODE_BT_CLK_DISABLE = 0x0000,

    HW_MIA_60HZ_CTL_TRIAC4_MASK = 0x0080, HW_MIA_60HZ_CTL_TRIAC4_ENABLE = 0x0080, HW_MIA_60HZ_CTL_TRIAC4_DISABLE = 0x0000,

    HW_MIA_60HZ_CTL_TRIAC3_MASK = 0x0040, HW_MIA_60HZ_CTL_TRIAC3_ENABLE = 0x0040, HW_MIA_60HZ_CTL_TRIAC3_DISABLE = 0x0000,

    HW_MIA_60HZ_CTL_MODE_DIMMER_MASK = 0x0020, HW_MIA_60HZ_CTL_MODE_DIMMER_ENABLE = 0x0020, HW_MIA_60HZ_CTL_MODE_DIMMER_DISABLE = 0x0000,

    HW_MIA_60HZ_CTL_TRIAC2_MASK = 0x0018, HW_MIA_60HZ_CTL_TRIAC2_ENABLE = 0x0008, HW_MIA_60HZ_CTL_TRIAC2_DISABLE = 0x0000,

    HW_MIA_60HZ_CTL_TRIAC1_MASK = 0x0004, HW_MIA_60HZ_CTL_TRIAC1_ENABLE = 0x0004, HW_MIA_60HZ_CTL_TRIAC1_DISABLE = 0x0000,

    HW_MIA_60HZ_CTL_SRC_MASK = 0x0002, HW_MIA_60HZ_CTL_SRC_P6 = 0x0002, HW_MIA_60HZ_CTL_SC_OPT_CTL_IR_SRC = 0x0000,

    HW_MIA_60HZ_CTL_MODULE_MASK = 0x0001, HW_MIA_60HZ_CTL_MODULE_ENABLE = 0x0001, HW_MIA_60HZ_CTL_MODULE_DISABLE = 0x0000,
};

enum
{
    HIDDCFA_PCX_TO_NAT, HIDDCFA_NAT_TO_PCX
};

enum
{
    /// Only the bottom 5 bits of this register are used
    BTCLK_VSYNC_VAL2_MASK = 0x1F,
    /// Maximum value that can assign to control.intervalThreshold
    MAX_INTERVAL_THRESHOLD = 65535
};

/// Control information that changes during operation
/// (as opposed to configuration information that is read 
/// in at startup and does not change)
typedef struct
{
        /// Need to save this value since write this value to main_debounce_val_adr,
        /// but when read that address will get current count
        UINT16 intervalThreshold;

        /// True if user wants chan to be enabled.  The chan will be in this
        /// state if the module is enabled.  The chan will always be disabled
        /// if the module is disabled, but that doesn't affect this value.
        BOOL8 chanEnable[ NUM_SYNCED_PULSE_HW_CHANS ];

        /// True if module should be enabled
        BOOL8 enable;
} SyncedPulseControl;

/// The internal runtime state of the synced pulse driver
typedef struct
{
        /// Pointer to associated gpio configuration info that usually don't change
        SyncedPulseConfig *config;

        /// Maintains user controlled settings that are more likely to be changed
        SyncedPulseControl control;
} SyncedPulseState;

void syncp_updateChanState( UINT32 chanNum );
void syncp_updateAllChanStates( void );
void syncp_writeTargetSyncTime( UINT32 clk, UINT32 phaseCount );

/* @} */

#endif

