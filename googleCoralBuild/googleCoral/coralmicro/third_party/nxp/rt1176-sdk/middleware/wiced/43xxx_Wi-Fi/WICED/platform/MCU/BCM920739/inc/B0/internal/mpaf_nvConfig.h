/*****************************************************************************
* THIS INFORMATION IS PROPRIETARY TO
* BROADCOM CORP.
*-----------------------------------------------------------------------------
*
*           Copyright (c) 2003, 2004 Broadcom Corp.
*                      ALL RIGHTS RESERVED
*
******************************************************************************

******************************************************************************
**
**  Name:          mpaf_nvConfig.h
**
**  Description:   Definitions for device specific API functions
**
******************************************************************************/
#ifndef MPAF_NVCONFIG_H
#define MPAF_NVCONFIG_H
//#include "mpaf.h"


#define MPAF_NV_CFGID_DEV_MAX_BY_CONFIG      (mpaf_pformCfg.nvMaxDevinfo)

#define MPAF_NV_CFGID_DEV_INVALID    (0xFF)

enum {
// Dev List
    MPAF_NV_CFGID_DEV_INDEX_START,
    MPAF_NV_CFGID_DEV_DEFAULT = 1,
    MPAF_NV_CFGID_DEV_MAX = 20,
// MRU Table
    MPAF_NV_CFGID_MRU_BY_CONFIG = MPAF_NV_CFGID_DEV_MAX,
#if defined(ZB_STACK_ENABLE)
//zigbee nvram data
    MPAF_ZB_NV_NIB_NEIGHBOUR_TABLE_1ST_255,
    MPAF_ZB_NV_NIB_NEIGHBOUR_TABLE_2ND_255,
    MPAF_ZB_NV_NIB_NEIGHBOUR_TABLE_3RD_255,
    MPAF_ZB_NV_NIB_ROUTING_TABLE_1ST_255,
    MPAF_ZB_NV_AIB_BINDING_TABLE_1ST_255,
    MPAF_ZB_NV_AIB_BINDING_TABLE_2ND_255,
    MPAF_ZB_NV_SSP_KEY_PAIR_DESC_1ST_255,
    MPAF_ZB_NV_SSP_KEY_PAIR_DESC_2ND_255,
    MPAF_ZB_NV_ZB_INFO,
    MPAF_ZB_NV_ZDO_DESCRIPTORS,
    MPAF_ZB_NV_APS_GROUP_TABLE,
    MPAF_ZB_NV_SIMPLE_DESC_1ST_255,
    MPAF_ZB_NV_SIMPLE_DESC_2ND_255,
#endif
#if defined(_RF4CE_)
//zigbee RF4CE Data
    MPAF_NV_CFGID_ZB_RF4CE_ATTRIBUTE_255,
    MPAF_NV_CFGID_ZB_RF4CE_PAIR_TABLE_START,
    MPAF_NV_CFGID_ZB_RF4CE_PAIR_TABLE_MAX = MPAF_NV_CFGID_ZB_RF4CE_PAIR_TABLE_START+2,
#endif
// Application data (ex: WICED apps)
    MPAF_NV_CFGID_APP_SCRATCH_DATA_INDEX_START,
    MPAF_NV_CFGID_APP_SCRATCH_DATA_INDEX_MAX = 0x3FFF,          // ID_ITEM_ID_MAX/2
// UHE Data
    MPAF_NV_CFGID_UHE_STARTUP_MODE,
    MPAF_NV_CFGID_UHE_KBD_STARTUP_MODE,
    MPAF_NV_CFGID_UHE_RADIO_STARTUP_MODE,
    MPAF_NV_CFGID_UHE_KB_HID_SVC_1ST_255_BYTES,
    MPAF_NV_CFGID_UHE_KB_HID_SVC_2ND_255_BYTES,
    MPAF_NV_CFGID_UHE_KB_HID_SVC_3RD_255_BYTES,
    MPAF_NV_CFGID_UHE_KB_HID_SVC_4TH_255_BYTES,
    MPAF_NV_CFGID_UHE_MS_HID_SVC_1ST_255_BYTES,
    MPAF_NV_CFGID_UHE_MS_HID_SVC_2ND_255_BYTES,
    MPAF_NV_CFGID_UHE_MS_HID_SVC_3RD_255_BYTES,
    MPAF_NV_CFGID_UHE_MS_HID_SVC_4TH_255_BYTES,
    MPAF_NV_CFGID_UHE_KB_PNP_SVC_1ST_255_BYTES,
    MPAF_NV_CFGID_UHE_MS_PNP_SVC_1ST_255_BYTES,
    MPAF_NV_CFGID_UHE_SECURE_PRODUCT_ID,
    MPAF_NV_CFGID_UHE_KB_RN,
    MPAF_NV_CFGID_UHE_MS_RN,
// App specific Data per device
    MPAF_NV_CFGID_APP_DATA_DEV_INDEX_START,
    MPAF_NV_CFGID_APP_DATA_DEV_MAX = MPAF_NV_CFGID_APP_DATA_DEV_INDEX_START + MPAF_NV_CFGID_DEV_MAX,
// Scratch Pad
    MPAF_NV_CFGID_SCRATCH_PAD_ITEM0 = MPAF_NV_CFGID_APP_DATA_DEV_MAX,
    MPAF_NV_CFGID_SCRATCH_PAD_ITEM_MAX = MPAF_NV_CFGID_SCRATCH_PAD_ITEM0+8,
};

#ifdef MPAF_HIDD
#define VS_HOST_LIST MPAF_NV_CFGID_UHE_STARTUP_MODE //UHE and HIDD are mutually exclusive 
#endif
#define MPAF_NV_GET_APP_DATA_CFGID_FOR_DEV(vsId) (vsId + MPAF_NV_CFGID_APP_DATA_DEV_INDEX_START)

//! Macro to catch asserts during compilation time
#define C_ASSERT(e) typedef char __C_ASSERT__[(e)?1:-1]

// NOTE: NVRAM DEV Index and App Data Index Mismatch will result in ASSERT/COMPILE ERROR
C_ASSERT((MPAF_NV_CFGID_APP_DATA_DEV_MAX - MPAF_NV_CFGID_APP_DATA_DEV_INDEX_START) \
          == (MPAF_NV_CFGID_DEV_MAX - MPAF_NV_CFGID_DEV_INDEX_START));

#ifdef SCRATCH_PAD
#define MPAF_NV_SCRATCH_PAD_MAX_STORED       (MPAF_NV_CFGID_SCRATCH_PAD_ITEM_MAX - MPAF_NV_CFGID_SCRATCH_PAD_ITEM0)  /* Maximum number of stored pad words */
#endif

#define MPAF_NVRAM_RECORD_SIZE      255

/* The size of HID LE attribute cache depends on HIDLE structure
 * Any mismatch in this value is caught in HIDLE during compilation with C_ASSERT.
 */
#define MPAF_HIDLE_ATT_CACHE_SIZE   92

#endif      /* MPAFE_NVCONFIG_H */
