/*******************************************************************************
* ------------------------------------------------------------------------------
*
* Copyright (c), 2013 BROADCOM Corp.
*
*          ALL RIGHTS RESERVED
*
* This file contains all event items used by hidd drivers
********************************************************************************/
#ifndef __HIDD_DRIVERS_EVENT__
#define __HIDD_DRIVERS_EVENT__

#include "brcm_fw_types.h"


// must be same as BLE_APP_EVENT_FREE_BUFFER
#define HIDD_DRIVERS_EVENT_FREE_BUFFER 42
// must be same as BLE_APP_EVENT_NO_ACTION
#define HIDD_DRIVERS_EVENT_NO_ACTION   43

BOOL32 hidddriversevt_serialize(int (*fn)(void*), void* data);

#endif


