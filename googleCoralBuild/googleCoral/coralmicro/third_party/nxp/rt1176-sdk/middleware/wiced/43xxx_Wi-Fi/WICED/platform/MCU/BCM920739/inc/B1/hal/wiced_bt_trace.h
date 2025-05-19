/*
 * Copyright 2015, Broadcom Corporation
 * All Rights Reserved.
 *
 * This is UNPUBLISHED PROPRIETARY SOURCE CODE of Broadcom Corporation;
 * the contents of this file may not be disclosed to third parties, copied
 * or duplicated in any form, in whole or in part, without the prior
 * written permission of Broadcom Corporation.
 */
/** @file
 *
 * WICED BT App Common Utilities. This file provides the interfaces to the utilities that
 * can be used by the applications
 */
#ifndef _WICED_BT_TRACE_H_
#define _WICED_BT_TRACE_H_

#include <stdarg.h>
#include "string.h"
#include "wiced_bt_types.h"
#ifdef ENABLE_JLINK_TRACE
#include "RTT_Logger.h"
#endif

//Enable below to get traces (For SPAR applications)
#define WICED_BT_TRACE_ENABLE

#define setvbuf(a,b,c,d)

//Enable shim traces only if required
//#ifdef WICED_BT_TRACE_ENABLE
//#define WICED_SHIM_TRACE_ENABLE
//#endif

/** Debug UARTs. Used when calling wiced_set_debug_uart.*/
typedef enum
{   
    WICED_ROUTE_DEBUG_NONE,              /** < No Traces */
    WICED_ROUTE_DEBUG_TO_WICED_UART,    /**< Set to WICED UART to send debug strings over the Wiced debug interface */
    WICED_ROUTE_DEBUG_TO_HCI_UART,      /**< By default, HCI UART will be used if the application doesn't set any other using wiced_set_debug_uart() */
    WICED_ROUTE_DEBUG_TO_DBG_UART,      /**< Use this to direct the debug traces over the debug uart for the 20 MHz board */
    WICED_ROUTE_DEBUG_TO_PUART          /**< Use this to direct the debug traces over the peripheral uart for the 24 MHz board */
}wiced_debug_uart_types_t;

void wiced_bt_trace_array( const char *string, const uint8_t* array, const uint16_t len );
void wiced_trace_array( const uint8_t* p_array, uint16_t len);
int wiced_printf(char * buffer, int len, ...);

#ifdef WICED_POWER_LOGGER_ENABLE
int wpl_wiced_printf(char * buffer, int len, ...);
#endif

#ifdef WICED_BT_TRACE_ENABLE

#ifdef ENABLE_JLINK_TRACE
#define WICED_BT_TRACE(...)                 RTT_printf(__VA_ARGS__)
#define WICED_BT_TRACE_CRIT(...)            RTT_printf(__VA_ARGS__)
#define WICED_BT_TRACE_ARRAY(ptr, len, ...) RTT_printf(__VA_ARGS__); wiced_trace_array(ptr, len);
#else

#ifdef WICED_POWER_LOGGER_ENABLE
#define WICED_BT_TRACE(...)                 wpl_wiced_printf(NULL, 0, __VA_ARGS__)
#define WICED_BT_TRACE_CRIT(...)            wpl_wiced_printf(NULL, 0, __VA_ARGS__)
#define WICED_BT_TRACE_ARRAY(ptr, len, ...) wpl_wiced_printf(NULL, 0, __VA_ARGS__); wiced_trace_array(ptr, len);
#else
#define WICED_BT_TRACE(...)                 wiced_printf(NULL, 0, __VA_ARGS__)
#define WICED_BT_TRACE_CRIT(...)            wiced_printf(NULL, 0, __VA_ARGS__)
#define WICED_BT_TRACE_ARRAY(ptr, len, ...) wiced_printf(NULL, 0, __VA_ARGS__); wiced_trace_array(ptr, len);
#endif //WICED_POWER_LOGGER_ENABLE

#endif //ENABLE_JLINK_TRACE

#else
#define WICED_BT_TRACE(format, ...)
#define WICED_BT_TRACE_ARRAY(ptr, len, ...)
#endif //WICED_BT_TRACE_ENABLE

#ifdef WICED_SHIM_TRACE_ENABLE
#define WICED_SHIM_TRACE(...)     wiced_printf(NULL, 0, __VA_ARGS__)
#else
#define WICED_SHIM_TRACE(...)
#endif

void wiced_bt_trace_enable(void);

/**
 * Function         wiced_set_debug_uart
 *
 * To specify the UART to be used for the debug traces
 *
 * @param[in]      uart        : UART to be used
 *
 * @return          void
 *
 */
void wiced_set_debug_uart ( wiced_debug_uart_types_t uart );

#endif /* _WICED_BT_TRACE_H_ */

