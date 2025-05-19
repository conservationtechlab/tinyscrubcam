/*
 * Copyright 2015, Cypress Corporation
 * All Rights Reserved.
 *
 * This is UNPUBLISHED PROPRIETARY SOURCE CODE of Cypress Corporation;
 * the contents of this file may not be disclosed to third parties, copied
 * or duplicated in any form, in whole or in part, without the prior
 * written permission of Cypress Corporation.
 */ 
 /** @file
 *
 *  Defines functions for using the WICED Sleep Framework
 */

#ifndef _WICED_SLEEP_H_
#define _WICED_SLEEP_H_

#define WICED_SLEEP_MAX_TIME_TO_SLEEP   ~0

/** Wake sources.*/
#define WICED_SLEEP_WAKE_SOURCE_KEYSCAN  (1<<0)  /**< Enable wake from keyscan */
#define WICED_SLEEP_WAKE_SOURCE_QUAD     (1<<1)  /**< Enable wake from quadrature sensor */
#define WICED_SLEEP_WAKE_SOURCE_GPIO     (1<<2)  /**< Enable wake from GPIO */
#define WICED_SLEEP_WAKE_SOURCE_MASK     (WICED_SLEEP_WAKE_SOURCE_GPIO | \
                                         WICED_SLEEP_WAKE_SOURCE_KEYSCAN | \
                                         WICED_SLEEP_WAKE_SOURCE_QUAD) /**< All wake sources */

/** Boot mode */
typedef enum
{
    WICED_SLEEP_COLD_BOOT, /**< Cold boot */
    WICED_SLEEP_FAST_BOOT  /**< Fast boot */
}wiced_sleep_boot_type_t;

/** Sleep modes */
typedef enum
{
    WICED_SLEEP_MODE_NO_TRANSPORT, /**< Used for HID use cases. When a transport is connected, sleep is always disallowed*/
    WICED_SLEEP_MODE_TRANSPORT     /**< This mode allows sleep when transport is connected and uses device wake line to wake up*/
}wiced_sleep_mode_type_t;

/** Active interrupt level for Wake through GPIO*/
typedef enum
{
    WICED_SLEEP_WAKE_ACTIVE_LOW, /**< Active low interrupt wakes the chip */
    WICED_SLEEP_WAKE_ACTIVE_HIGH /**< Active high interrupt wakes the chip*/
}wiced_sleep_wake_type_t;

/** Sleep poll type */
typedef enum 
{
    WICED_SLEEP_POLL_TIME_TO_SLEEP,      /**< Polling for maximum allowed sleep duration */
    WICED_SLEEP_POLL_SLEEP_PERMISSION    /**< Polling for permission to sleep */
} wiced_sleep_poll_type_t;

/** Sleep permission 
*    Note: SHUTDOWN mode sleep puts the chip in the lowest power saving sleep mode. This turns off most
*    hardware including parts of SRAM. Hence, if the application requires to preserve some context data for it
*    to resume normal operation after wake up, app should store this context data in Always On Memory (the part that is not turned off).
*    This is done by declaring the context variable with the attribute  __attribute__ ((section(".data_in_retention_ram"))). 
*    The memory in AON available is limited, the maximum that application can use are 256 bytes.
*    When mode WICED_SLEEP_ALLOWED_WITH_SHUTDOWN is selected, the FW puts the chip in Shutdown sleep mode
*    if that is possible, else the chip will be put in a non-shutdown sleep mode.
*    To prevent FW from attempting to put the chip in Shutdown sleep mode, select WICED_SLEEP_ALLOWED_WITHOUT_SHUTDOWN.
*/
typedef enum
{
    WICED_SLEEP_NOT_ALLOWED,               /**< Sleep is not allowed */
    WICED_SLEEP_ALLOWED_WITHOUT_SHUTDOWN,  /**< Sleep is allowed, but shutdown mode sleep is not allowed */
    WICED_SLEEP_ALLOWED_WITH_SHUTDOWN      /**< Slepp allowed, including shutdown mode sleep */
}wiced_sleep_permission_type_t;

/**
 *
 *  Application implements call back of this type to allow or disallow the chip to go to sleep.
 *
 * @param[in]       type:  Poll type (see #wiced_sleep_poll_type_t)
 *
 * @return          if type == WICED_SLEEP_POLL_TIME_TO_SLEEP, application should return the maximum time allowed to sleep in micro seconds. 
 *                     WICED_SLEEP_MAX_TIME_TO_SLEEP allows the Firmware to determine the duration it can sleep.
 *                  if type == WICED_SLEEP_POLL_SLEEP_PERMISSION, application should return one of the values in wiced_sleep_permission_type_t
 */
typedef uint32_t (*wiced_sleep_allow_check_callback ) (wiced_sleep_poll_type_t type );


/** Sleep configuration parameters */
typedef struct
{
    wiced_sleep_mode_type_t                  sleep_mode;             /**< Requested sleep mode */
    wiced_sleep_wake_type_t                  host_wake_mode;         /**< Active level for host wake */
    wiced_sleep_wake_type_t                  device_wake_mode;       /**< Active level for device wake */
    uint8_t                                  device_wake_source;     /**< Device wake source(s). GPIO mandatory for  
                                                                          WICED_SLEEP_MODE_TRANSPORT */
    uint32_t                                 device_wake_gpio_num;   /**< GPIO# for host wake, mandatory for 
                                                                          WICED_SLEEP_MODE_TRANSPORT */
    wiced_sleep_allow_check_callback         sleep_permit_handler;   /**< Call back to be called by sleep framework 
                                                                          to poll for sleep permission */
}wiced_sleep_config_t;


/** API to configure sleep mode parameters.
 *
 * @param[in]       p_sleep_config: see @wiced_sleep_config_t
 * 
 * @return          WICED_SUCCESS or WICED_ERROR
 */
wiced_result_t wiced_sleep_configure( wiced_sleep_config_t *p_sleep_config );


/** API to request reboot type.
 *
 * Returns Cold boot or Warm/Fast boot.
 *
 * @return          wiced_sleep_boot_type_t
 */
wiced_sleep_boot_type_t wiced_sleep_get_boot_mode(void);
#endif //_WICED_SLEEP_H_
