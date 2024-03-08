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
* Inter-Integrated Circuit (I2C, IIC) Electrically Erasable
* Programmable Read-Only Memory (EEPROM) interface driver.
*
*/

#ifndef __WICED_I2CEEPROM_H__
#define __WICED_I2CEEPROM_H__

#include "brcm_fw_types.h"

/**  \addtogroup I2CEEPROMInterfaceDriver
* \ingroup HardwareDrivers
*/
/*! @{ */
/**
* Defines a driver for the I2C EEPROM interface. The driver is responsible for
* interfacing with an EEPROM module via the I2C bus, with its main purpose
* being data handling operations. For instance, any unused space (from the
* system partition) can be used for any purpose to store and access data. It
* is a handy way for an app to save information to non-volatile storage.
*
* Similar to the Serial Flash Interface Driver, this driver includes checks to
* ensure safe data handling operation--it will not allow any write operations
* to take place within active sections (i.e., sections that the system
* currently uses for boot, etc). Note that read operations are unrestricted.
* Please reference the User Documentation for more information regarding
* these active sections, their importance, and what roles they play in the
* system.
*
*/

/******************************************************************************
*** Parameters.
***
*** The following parameters are used to configure the driver or define
*** return status. They are not modifiable.
******************************************************************************/

/**
// Status for EEPROM operations.
#define DRV_STATUS_SUCCESS           0x00
#define DRV_STATUS_NO_ACK            0x01
#define DRV_STATUS_INVALID_ADDR      0xFF
**/

/******************************************************************************
*** Function prototypes.
******************************************************************************/

///////////////////////////////////////////////////////////////////////////////
/// Initialize the I2C lines and low-level EEPROM Driver.
/// Call this before performing any operations.
///
/// \param none
///
/// \return none
///////////////////////////////////////////////////////////////////////////////
void wiced_hal_ieeprom_init(void);

///////////////////////////////////////////////////////////////////////////////
/// Sets the size of the EEPROM module in bytes, used for bounds checking.
/// The default is 32768 bytes (256 kBit)
///
/// \param size - Size of the installed module in bytes.
///
/// \return none
///////////////////////////////////////////////////////////////////////////////
void wiced_hal_ieeprom_set_size(UINT32 size);

///////////////////////////////////////////////////////////////////////////////
/// Load data from a certain location on the EEPROM module into
/// memory.
///
/// \param eepromAddress - The starting source address on the EEPROM.
/// \param readCount     - The number of bytes to read.
/// \param buffer        - Pointer to destination data buffer.
/// \param deviceAddress - The source slave address.
///
/// \return Success[0] or no ack[1].
///////////////////////////////////////////////////////////////////////////////
UINT8 wiced_hal_ieeprom_read(UINT16 eepromAddress,
                             UINT16 readCount,
                             UINT8* buffer,
                             UINT8  deviceAddress);


///////////////////////////////////////////////////////////////////////////////
/// Write data from memory to a certain location on the EEPROM module.
///
/// (!) Please ensure that the address and (address + length) of data to be
/// written does not go beyond the size of the memory module.
///
/// (!) Note that this function will not allow corruption of certain memory
/// locations, such as currently active sections (boot sectors) and sections
/// required for the proper function of the Bluetooth subsystem.
///
/// \param eepromAddress - The starting destination address on the EEPROM.
/// \param writeCount    - The number of bytes to write.
/// \param buffer        - Pointer to destination data buffer.
/// \param deviceAddress - The destination slave address.
///
/// \return Success[0x00] or no ack[0x01] or invalid eepromAddress/invalid
/// length[0xFF].
///////////////////////////////////////////////////////////////////////////////
UINT8 wiced_hal_ieeprom_write(UINT16 eepromAddress,
                              UINT16 writeCount,
                              UINT8* buffer,
                              UINT8  deviceAddress);

/* @} */

#endif // __WICED_I2CEEPROM_H__
