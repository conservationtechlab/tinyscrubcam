/*
*****************************************************************
* THIS INFORMATION IS PROPRIETARY TO BROADCOM CORP.
*----------------------------------------------------------------
*			Copyright (c) 2014 Broadcom Corp.
*					   ALL RIGHTS RESERVED
*****************************************************************

*****************************************************************
*
* File Name: capscandriver.h
*
* Abstract: This file implements the 20739 FW interface for Capacitor Scan configuration.
*
* Notes:
*
*****************************************************************
*/

#ifndef __CAPSCAN_DRIVER_H__
#define __CAPSCAN_DRIVER_H__

#include "brcm_fw_types.h"


#define BIT0 	1
#define BIT1 	2
#define BIT2 	4
#define BIT3 	8
#define BIT4   	16
#define BIT5 	32
#define BIT6 	64
#define BIT7 	128
#define BIT16 	0x00010000
#define BIT17 	0x00020000
#define BIT18 	0x00040000
#define BIT19 	0x00080000

#define SHIELD_EN       0x00000002
#define CLKGATE_EN      0x00000004
#define PARAMODE_EN     0x00000008
#define SCANPEND_EN     0x00000010
#define FAILINTR_EN     0x00000020
#define FAILINTR_MSK    0x00000040
#define PORT0INTR_EN    0x00010000
#define PORT1INTR_EN    0x00020000
#define PORT2INTR_EN    0x00040000
#define PORT3INTR_EN    0x00080000
#define PORT0INTR_MSK   0x01000000
#define PORT1INTR_MSK   0x02000000
#define PORT2INTR_MSK   0x04000000
#define PORT3INTR_MSK   0x08000000

#define P0_CYC          0x0F
#define P1_CYC          0x0F
#define P2_CYC          0x0F
#define P3_CYC          0x0F
#define P4_CYC          0x0F
#define VSAMPLE         0x03
#define SAMPLE_DBCNT    0x02

#define FRAME_CNT       0x0020
#define FRAMEOVF_CNT    0x0800


typedef enum
{
    CS_POLL = 0,
    CS_INT = 1
} CAPSCAN_SCANCONFIG;

typedef enum
{
    CS_PORT0_TOUCH = 0,
    CS_PORT0_UNTOUCH = 1,
    CS_PORT1_TOUCH = 2,
    CS_PORT1_UNTOUCH = 3,
    CS_PORT2_TOUCH = 4,
    CS_PORT2_UNTOUCH = 5,
    CS_PORT3_TOUCH = 6,
    CS_PORT3_UNTOUCH = 7,
    CS_PORT_NONE_EVT = 8
} Capscan_evt;

typedef enum
{
    CS_PORT0 = 0,
    CS_PORT1 = 1,
    CS_PORT2 = 2,
    CS_PORT3 = 3
} Capscan_port;

typedef struct CapscanRegistration
{
    Capscan_evt evt;
    void (*userfn)(void*);
    void *userdata;
    struct CapscanRegistration *next;
} CapscanRegistration;

#define IOCFG_COUT0	0x00000744	//bit[2-0]=0x04: other extended I/O usage; bit[11:4]=0x74: Cout0
#define IOCFG_SOUT0	0x00000754	//bit[2-0]=0x04: other extended I/O usage; bit[11:4]=0x75: Sout0
#define IOCFG_COUT1	0x00000764	//bit[2-0]=0x04: other extended I/O usage; bit[11:4]=0x76: Cout1
#define IOCFG_SOUT1	0x00000774	//bit[2-0]=0x04: other extended I/O usage; bit[11:4]=0x77: Sout1
#define IOCFG_COUT2	0x00000784	//bit[2-0]=0x04: other extended I/O usage; bit[11:4]=0x78: Cout2
#define IOCFG_SOUT2	0x00000794	//bit[2-0]=0x04: other extended I/O usage; bit[11:4]=0x79: Sout2
#define IOCFG_COUT3	0x000007A4	//bit[2-0]=0x04: other extended I/O usage; bit[11:4]=0x7A: Cout3
#define IOCFG_SOUT3	0x000007B4	//bit[2-0]=0x04: other extended I/O usage; bit[11:4]=0x7B: Sout3

extern void capscan_init(void);
extern UINT32 capscan_GetRawCnt(UINT8 port);
extern void capscan_registerForEvtCallback(Capscan_evt evt, void (*userfn)(void*), void* userdata);
extern int capscan_HandleTouchInterrupt(void* unused);
void capscan_enable(void);
void capscan_disable(void);
void capscan_PinConfig(void);
void capscan_RegConfig(void);
void capscan_interruptHandler(void);
void capscan_HandleTouchEvt(void);
void capscan_initBaslineCnt(void);
void capscan_HandleTouchPolling(INT32 arg);

#endif

