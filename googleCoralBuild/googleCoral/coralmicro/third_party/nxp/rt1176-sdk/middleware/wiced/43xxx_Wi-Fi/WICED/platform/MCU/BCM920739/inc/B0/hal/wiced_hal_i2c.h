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
* Inter-Integrated Circuit (I2C, IIC) driver.
*
*/

#ifndef __WICED_I2C_DRIVER_H__
#define __WICED_I2C_DRIVER_H__

#include "i2cm.h"

/**  \addtogroup I2CDriver
* \ingroup HardwareDrivers
*/
/*! @{ */
/**
* Defines an I2C driver to facilitate communication with other devices on an
* I2C bus (such as a temperature sensor, etc). The driver is only capable of
* assuming a master role. Applications use this driver to obtain the status
* from and control the behavior of the I2C hardware. This driver only offers
* services for speed control and data transfer operations.
*
* Please note that even though this driver can access off-chip memory
* (if installed; EEPROM, etc), please use the drivers found in
* wiced_hal_ieeprom.h to access those modules, as those drivers include
* checks to ensure safe data handling operations. This driver is intended
* only to interface with other devices on the I2C bus, such as a motion
* sensor.
*
*/

/******************************************************************************
*** Parameters.
***
*** The following parameters are used to configure the driver or define
*** return status. They are not modifiable.
******************************************************************************/

/**
/// Speed Options
enum
{
    /// I2C speed is 100 KHz
    I2CM_SPEED_100KHZ = 240,

    /// I2C speed is 400 KHz
    I2CM_SPEED_400KHZ = 60,

    /// I2C speed is 800 KHz
    I2CM_SPEED_800KHZ = 30,

    /// I2C speed is 1 MHz
    I2CM_SPEED_1000KHZ = 24,
};

/// Transaction Status Return Values
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
**/

/******************************************************************************
*** Function prototypes.
******************************************************************************/
///////////////////////////////////////////////////////////////////////////////
/// Initializes the I2C driver and its private values. This initialization
/// sets the bus speed to 100KHz by default (I2CM_SPEED_100KHZ). To make
/// the bus run at another speed, call wiced_hal_i2c_setSpeed(<speed>)
/// right after this call.
///
/// (!) Note that it is the user's responsibility to configure the desired
/// GPIOs, since different HW platforms will have different configuration
/// parameters. Please see the User Documentation for more information.
///
/// \param none
///
/// \return none
///////////////////////////////////////////////////////////////////////////////
void wiced_hal_i2c_init(void);

///////////////////////////////////////////////////////////////////////////////
/// Sets the I2C bus speed. Note that the default after initialization is
/// set to 100KHz by default (I2CM_SPEED_100KHZ). To make the bus run at
/// another speed, use one of the following parameters (also see Parameters
/// section):
///
///
///     // I2C speed is 100 KHz
///     I2CM_SPEED_100KHZ
///
///     // I2C speed is 400 KHz
///     I2CM_SPEED_400KHZ
///
///     // I2C speed is 800 KHz
///     I2CM_SPEED_800KHZ
///
///     // I2C speed is 1 MHz
///     I2CM_SPEED_1000KHZ
///
/// \param speed - The new speed to be used. This is the transport clock counter
/// that counts the number of reference clock cycles.
///
/// \return none
///////////////////////////////////////////////////////////////////////////////
void wiced_hal_i2c_set_speed(UINT8 speed);


///////////////////////////////////////////////////////////////////////////////
/// Gets the current I2C bus speed. Smaller numbers indicate higher speeds.
/// See the Parameters section for more information.
///
/// \param none
///
/// \return The SCL ref clock cycle counter value that corresponding to
/// the SCL speed.
///////////////////////////////////////////////////////////////////////////////
UINT8 wiced_hal_i2c_get_speed(void);


///////////////////////////////////////////////////////////////////////////////
/// Reads data into given buffer from the I2C HW addressing a particualr
/// slave address. The data bytes are transparent. Though any arbitrary
/// length of data may be read from the slave, atomic transactions greater
/// than HW's capability are not possible - they will be split into multiple
/// transactions. This is a blocking call. Interrupts will not affect timing
/// within a transaction.
/// Note: Please see the User Documentation for the actual limitation of
/// the part on your platform.
///
/// \param data   - Pointer to a buffer that will hold the incoming data.
/// \param length - The length of the data to read.
/// \param slave  - The source slave address.
///
/// \return The status of the transaction (success[0], failure[1], or busy[2]).
///////////////////////////////////////////////////////////////////////////////
UINT8 wiced_hal_i2c_read(UINT8* data, UINT16 length, UINT8 slave);


///////////////////////////////////////////////////////////////////////////////
/// Writes the given data to the I2C HW addressing a particualr slave address.
/// The data bytes are transparent. Though any arbitrary length of data may be
/// written to the slave, atomic transactions greater than HW's capability
/// are not possible - they will be split into multiple transactions. This is
/// a blocking call. Interrupts will not affect timing within a transaction.
/// Note: Please see the User Documentation for the actual limitation of
/// the part on your platform.
///
/// \param data   - Pointer to a buffer holding the data to write.
/// \param length - The length of the data to write.
/// \param slave  - The destination slave address.
///
/// \return The status of the transaction (success[0], failure[1], or busy[2]).
///////////////////////////////////////////////////////////////////////////////
UINT8 wiced_hal_i2c_write(UINT8* data, UINT16 length, UINT8 slave);


/* @} */

#endif // __WICED_I2C_DRIVER_H__

