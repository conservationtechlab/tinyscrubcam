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
* Peripheral Universal Asynchronous Receiver/Transmitter (PUART) driver.
*
*/

#ifndef __WICED_PUART_H__
#define __WICED_PUART_H__

#include "brcm_fw_types.h"

/**  \addtogroup PUARTDriver
* \ingroup HardwareDrivers
*/
/*! @{ */
/**
* Defines a driver to facilitate interfacing with the UART hardware to send
* and receive bytes or a stream of bytes over the UART hardware. Typical
* use-cases involve printing messages over UART/RS-232, or to communicate
* with peripheral devices.
*
*/

/**
/// Example Usage:
    void testPUARTDriver(void)
    {
        UINT8 readbyte;
        UINT8 loopCtrl = 1;
        char printBuffer[50];

        wiced_hal_puart_init();

        // Possible uart tx and rx combination.
        // Pin for Rx: p34, Pin for Tx: p31
        // Note that p34 and p31 might not be avaliable for use on your
        // specific hardware platform.
        // Please see the User Documentation to reference the valid pins.
        wiced_hal_puart_selectUartPads(34, 31, 0, 0)

        wiced_hal_puart_flowOff();  //turn off flow control
        wiced_hal_puart_enableTx();
        wiced_hal_puart_enableRx();

        while(loopCtrl)
        {
            while(wiced_hal_puart_read(&readbyte))
            {
                wiced_hal_puart_write(readbyte);

                if(readbyte == 'S')
                {
                    wiced_hal_puart_print("\nYou typed 'S'.");

                    sprintf(printBuffer, "\nThis message sprintf'ed here.");
                    wiced_hal_puart_print(printBuffer);
                }

                if(readbyte == 'E') // End.
                {
                    loopCtrl = 0;
                }
            }
        }
    }
**/

/******************************************************************************
*** Function prototypes.
******************************************************************************/

///////////////////////////////////////////////////////////////////////////////
/// Initialize the Peripheral UART interface with the default configuration
/// parameters. This must be invoked once at boot before using any of PUART's
/// services.
///
/// Default baud rate is 115200 Bd. This can be changed by calling
/// "wiced_hal_puart_setBaudrate()" as described later, after this
/// initialization function.
///
/// \param none
///
/// \return none
///////////////////////////////////////////////////////////////////////////////
void wiced_hal_puart_init(void);

///////////////////////////////////////////////////////////////////////////////
/// Enable flow control.
///
/// \param none
///
/// \return none
///////////////////////////////////////////////////////////////////////////////
void wiced_hal_puart_flow_on(void);


///////////////////////////////////////////////////////////////////////////////
/// Disable flow control.
///
/// \param none
///
/// \return none
///////////////////////////////////////////////////////////////////////////////
void wiced_hal_puart_flow_off(void);


///////////////////////////////////////////////////////////////////////////////
/// Select the TX/RX and optional CTS/RTS pins (P<pin>) for the UART hardware
/// to use.
///
/// Please follow the guidelines set in the User Documentation when
/// selecting pins, as not all pins are avaliable for the PUART driver;
/// they depend on the specific hardware platform.
///
/// Remember that P# is the physical pin number on the board minus 1.
///         Ex: Board Pin 33 = P32.
///
/// \param rxdPin - RX Pin
/// \param txdPin - TX Pin
/// \param ctsPin - CTS Pin
/// \param rtsPin - RTS Pin
///
/// \return TRUE if pads were successfully set, FALSE if pads were not set.
/// If FALSE, make sure input port/pin parameters are correct.
///////////////////////////////////////////////////////////////////////////////
BOOL32 wiced_hal_puart_select_uart_pads(UINT8 rxdPin, UINT8 txdPin,
                                        UINT8 ctsPin, UINT8 rtsPin);

///////////////////////////////////////////////////////////////////////////////
/// Print/send a string of characters via the TX line.
///
/// \param string - A string of characters to send.
///
/// \return none
///////////////////////////////////////////////////////////////////////////////
void wiced_hal_puart_print(char * string);


///////////////////////////////////////////////////////////////////////////////
/// Print/send one byte via the TX line.
///
/// \param byte - Byte to send on the TX line.
///
/// \return none
///////////////////////////////////////////////////////////////////////////////
void wiced_hal_puart_write(UINT8 byte);


///////////////////////////////////////////////////////////////////////////////
/// Read one byte via the RX line.
///
/// \param rxByte - Destination byte to hold received data byte from RX FIFO.
///
/// \return TRUE if data was successfully read, FALSE if
/// not (RX FIFO was empty).
///////////////////////////////////////////////////////////////////////////////
BOOL32 wiced_hal_puart_read(UINT8* rxByte);


///////////////////////////////////////////////////////////////////////////////
/// Disable transmit capability.
///
/// \param none
///
/// \return none
///////////////////////////////////////////////////////////////////////////////
void wiced_hal_puart_disable_tx(void);


///////////////////////////////////////////////////////////////////////////////
/// Enable transmit capability.
///
/// \param none
///
/// \return none
///////////////////////////////////////////////////////////////////////////////
void wiced_hal_puart_enable_tx(void);


///////////////////////////////////////////////////////////////////////////////
/// Enable receive capability (specifically, by enabling PUART RX interrupts
/// through the MIA driver).
///
/// \param none
///
/// \return none
///////////////////////////////////////////////////////////////////////////////
void wiced_hal_puart_enable_rx(void);

///////////////////////////////////////////////////////////////////////////////
/// Set the baud rate (Bd) to a value other than the default 115200 Bd.
///
/// \param baudrate - Desired rate in symbols per second, e.g. 9600.
///
/// \return none
///////////////////////////////////////////////////////////////////////////////
void wiced_hal_puart_set_baudrate(UINT32 baudrate);


///////////////////////////////////////////////////////////////////////////////
/// Read in a set of bytes sequentially.
///
/// \param buffer - Destination buffer to hold incoming bytes.
/// \param lenth  - Number of bytes to read from the RX FIFO.
///
/// \return none
///////////////////////////////////////////////////////////////////////////////
void wiced_hal_puart_synchronous_read(UINT8* buffer, UINT32 length);


///////////////////////////////////////////////////////////////////////////////
/// Write a set of bytes sequentially.
///
/// \param buffer - Source buffer to hold outgoing bytes.
/// \param lenth  - Number of bytes to write to the TX FIFO.
///
/// \return none
///////////////////////////////////////////////////////////////////////////////
void wiced_hal_puart_synchronous_write(UINT8* buffer,
                                                     UINT32 length);

///////////////////////////////////////////////////////////////////////////////
/// Check to see if there is any data ready in the RX FIFO.
///
/// \param none
///
/// \return TRUE if bytes are avaliable, FALSE if the FIFO is empty.
///////////////////////////////////////////////////////////////////////////////
BOOL32 wiced_hal_puart_rx_fifo_not_empty(void);


/* @} */

#endif // __WICED_PUART_H__
