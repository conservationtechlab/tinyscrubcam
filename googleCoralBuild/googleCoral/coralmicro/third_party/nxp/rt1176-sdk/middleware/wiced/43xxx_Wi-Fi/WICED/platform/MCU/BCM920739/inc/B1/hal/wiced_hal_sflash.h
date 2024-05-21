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
* Serial Flash interface driver.
*
*/

#ifndef __WICED_SFLASH_H__
#define __WICED_SFLASH_H__

/**  \addtogroup SerialFlashInterfaceDriver
* \ingroup HardwareDrivers
*/
/*! @{ */
/**
* Defines a driver for the Serial Flash interface. The driver is responsible
* for interfacing with a Serial Flash memory module via the second SPI bus,
* with its main purpose being data handling operations. For instance, any
* unused space (from the system partition) can be used for any purpose to
* store and access data. It is a handy way for an app to save information to
* non-volatile storage.
*
* Similar to the I2C EEPROM Interface Driver, this driver includes checks to
* ensure safe data handling operation--it will not allow any write or erase
* operations to take place within active sections (i.e., sections that the
* system currently uses for boot, etc). Note that read operations are
* unrestricted. Please reference the User Documentation for more information
* regarding these active sections, their importance, and what roles they
* play in the system.
*
*/

/******************************************************************************
*** Function prototypes.
******************************************************************************/

///////////////////////////////////////////////////////////////////////////////
/// Initialize the SPI lines and low-level Serial Flash Interface Driver.
/// Call this before performing any SF operations.
///
/// \param none
///
/// \return none
///////////////////////////////////////////////////////////////////////////////
void wiced_hal_sflash_init(void);

///////////////////////////////////////////////////////////////////////////////
/// Returns the installed SF module size. The low-level Serial Flash Interface
/// Driver sends specific commands to the chip, which then reports its size.
///
/// Note that this function is a good way to make sure that the SF module
/// is installed and is communicating correctly with the system.
///
/// \param none
///
/// \return none
///////////////////////////////////////////////////////////////////////////////
uint32_t wiced_hal_sflash_get_size(void);

///////////////////////////////////////////////////////////////////////////////
/// Load data from a certain location on the serial flash module into
/// memory. To have a better read performance, place the destination data
/// buffer at a word boundary.
///
/// \param addr - The starting source address on the serial flash.
/// \param len  - The number of bytes to read.
/// \param buf  - Pointer to destination data buffer.
///
/// \return The number of bytes read.
///////////////////////////////////////////////////////////////////////////////
uint32_t wiced_hal_sflash_read(uint32_t addr, uint32_t len, uint8_t *buf);

///////////////////////////////////////////////////////////////////////////////
/// Write data from memory to a certain location on the serial flash module.
///
/// (!) Please ensure that the address and (address + length) of data to be
/// written does not go beyond the size of the memory module. If they do,
/// the write operation will "wrap around" and start corrupting the starting
/// address of the memory (boot sector), rendering the device inoperable.
///
/// (!) Note that this function will not allow corruption of certain memory
/// locations, such as currently active sections (boot sectors) and sections
/// required for the proper function of the Bluetooth subsystem.
///
/// \param addr - The starting destination address on the serial flash.
/// \param len  - The number of bytes to write.
/// \param buf  - Pointer to source data buffer.
///
/// \return The number of bytes written.
///////////////////////////////////////////////////////////////////////////////
uint32_t wiced_hal_sflash_write(uint32_t addr, uint32_t len, uint8_t *buf);

///////////////////////////////////////////////////////////////////////////////
/// Erase len number of bytes from the serial flash. Depending on the starting
/// address and length, it calls sector or block erase to do the work.
///
/// (!) Please ensure that the address and (address + length) of data to be
/// written does not go beyond the size of the memory module. If they do,
/// the erase operation will "wrap around" and start erasing the starting
/// address of the memory (boot sector), rendering the device inoperable.
///
/// (!) Note that due to the nature of Serial Flash memory and the limitations
/// of sector and/or block boundaries, it is possible that the number of bytes
/// erased could be greater than len.
///
/// \param addr   - The starting erase address on the serial flash.
/// \param len    - The number of bytes to erase.
///
/// \return none
///////////////////////////////////////////////////////////////////////////////
void wiced_hal_sflash_erase(uint32_t addr, uint32_t len);

/* @} */

#endif // __WICED_SFLASH_H__
