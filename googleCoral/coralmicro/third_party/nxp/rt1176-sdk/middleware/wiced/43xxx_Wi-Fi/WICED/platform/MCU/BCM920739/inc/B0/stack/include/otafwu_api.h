/*****************************************************************************
**
**  Name:           otafwu_api.h
**
**  Description:    This file contains definitions and interfaces used by the
**                  Broadcom propriatary OTAFWU (Over The Air Firmware Upgrade)
**                  protocol.
**
**  Copyright (c) 2013, Broadcom Corp., All Rights Reserved.
**
*****************************************************************************/
#ifndef OTAFWU_API_H
#define OTAFWU_API_H

#include "mpaf/apps/bt_target.h"

/*****************************************************************************
**  Constants and Types
*****************************************************************************/
/* OTAFWU callback events */
enum
{
    OTAFWU_EVT_ENABLE_FWU = 1,
    OTAFWU_EVT_LAUNCH,
    //Add any more events if needed.
    OTAFWU_EVT_MAX
};

/* Callback function for OTAFWU events
 * TBD: No callback data available now, may be used if required
 */
typedef void (tOTAFWU_CBACK)(UINT16 event, void *p_data);

/*****************************************************************************
**  External Function Declarations
*****************************************************************************/
#ifdef __cplusplus
extern "C"
{
#endif

/*******************************************************************************
**
** Function         otafwu_register_notify
**
** Description      This function can be used to register for the notifications
**                  from the OTAFWU module to the application layer.
**                  This will help the application to make any decisions
**                  pre and post firmware download over the air.
**
** Returns          TRUE if success, FALSE otherwise.
**
*******************************************************************************/
BT_API extern BOOLEAN otafwu_register_notify(tOTAFWU_CBACK *p_cback);

/*******************************************************************************
**
** Function         otafwu_launchRam
**
** Description      This function is used to launch the ram downloaded config
**                  record through OTA
**
** Returns          Never.
**
*******************************************************************************/
BT_API extern void otafwu_launchRam(void);

#ifdef __cplusplus
}
#endif

#endif /* OTAFWU_API_H */
