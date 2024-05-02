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
*    File Name: i2cm.h
*
*    Abstract: This file defines an I2C master driver instance
*
*
********************************************************************
*/
#ifndef __I2CM_DRIVER_H__
#define __I2CM_DRIVER_H__

#include "brcm_fw_types.h"



/**  \addtogroup I2C
 *  \ingroup HardwareDrivers
*/
/*! @{ */
/**
* Defines the BCM I2C master driver. The   BCM standard I2CM driver provides
* the status and control for the I2C HW.
*
*/

enum
{
    /// I2C speed is 100 KHz
    I2CM_SPEED_100KHZ = 240,

    /// I2C speed is 400 KHz
    I2CM_SPEED_400KHZ = 60,

    /// I2C speed is 800 KHz
    I2CM_SPEED_800KHZ = 30,

    /// I2C speed is 1 MHz
    I2CM_SPEEC_1000KHZ = 24,

    /// I2C speed is 2 MHz
    /// NOT AVAILABLE IN ALL PARTS
    /// Contact BRCM for more info
    /// I2C will default to 100 KHz if not supported
    I2CM_SPEEC_2000KHZ = 12
};

enum
{
    /// The transaction was sucessful
    I2CM_SUCCESS,

    /// The attempted operation failed, possibly because
    /// of no ack from slave.
    I2CM_OP_FAILED,

    /// The I2C HW block is busy with another transaction.
    I2CM_BUSY
};

/// SCL speed is governed by a counter that counts the number
/// of cycles of the reference clock which is alwasy 24 MHz.
/// So, for a speed of 2.4 MHz, the counter has to be set to
/// a value of 10 while for a speed of 100 KHz, the counter
/// has to be set to 240.
enum
{
    /// The minimum value of SCL counter for maximum
    /// SCL speed of 2.4 MHz. Speeds higher than this may
    /// not be possible to achieve without restrictions.
    I2CM_SCL_SPEED_MAX = 10,

    /// The maximum value of SCL counter for minimum
    /// SCL speed of ~94.1 KHz. Speeds lower than this
    /// are not possible.
    I2CM_SCL_SPEED_MIN = 255,
};

#ifdef __cplusplus
extern "C" {
#endif

void i2cm_init(void);
void i2cm_setSpeed(UINT8 speed);
void i2cm_setTransactionSpeed(void);
UINT8 i2cm_getSpeed(void);
UINT8 i2cm_write(UINT8* data, UINT16 length, UINT8 slave);
UINT8 i2cm_read(UINT8* data, UINT16 length, UINT8 slave);
UINT8 i2cm_comboRead(UINT8* secondTranBuf, UINT16 secondTranCount, UINT8* firstTranBuf, UINT8 firstTranCount, UINT8 slaveAdr);
UINT8 i2cm_writeWithOffset(UINT8* secondTranBuf, UINT16 secondTranCount, UINT8* firstTranBuf, UINT8 firstTranCount, UINT8 slaveAdr);
BOOL32 i2cm_pingSlave(UINT8 slaveAdr);

#ifdef __cplusplus
}
#endif


/* @} */


#endif
