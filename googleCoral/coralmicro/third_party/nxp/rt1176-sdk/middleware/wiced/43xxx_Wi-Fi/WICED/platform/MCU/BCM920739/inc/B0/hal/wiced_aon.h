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
 *  Defines functions for using the WICED AON Framework
 */

#ifndef _WICED_AON_H_
#define _WICED_AON_H_

/*
 * WICED_AON_ACTION
 * Action type for the callback handlers.
 */
enum
{
    WICED_AON_CONTEXT_SAVE = 0,   /* Save context from SRAM to AON */
    WICED_AON_CONTEXT_RESTORE,    /* Restore context from AON to SRAM */
};
typedef uint8_t wiced_aon_action_type_t;

/*
 * Gives the AON Boot mode.
 *
 */
enum wiced_aon_boot_type_e
{
    WICED_AON_COLD_BOOT,
    WICED_AON_FAST_BOOT,
};

/*
 *
 *  Call back to save or restore AON context.
 *
 * @param[in]       type:  Action type for the handler (see #wiced_aon_action_type_t)
 * @param[in]       ptr:   Pointer to the AON memory location for the handler to
 *                         store/restore the content, as per action type.
 * @param[in]       size:  Total number of valid AON octets for the handler to access.
 *
 * @return          TRUE to indicates the module successfully saved the context
 *                  Return type is ignored otherwise.
 */
typedef wiced_bool_t (*wiced_aon_power_callback_fp)(
                                            wiced_aon_action_type_t type,
                                            void                    *ptr,
                                            uint16_t                size);

/*
 *
 * Function         wiced_aon_register_power_callback
 *
 *
 * @param[in]       size:    Total number of AON octets needed for the module
 *                           to save its context.
 * @param[in]       cb_ptr:  Callback handler to be notified during the mode transition.
 *
 * @return          TRUE if requested number of AON octets can be allocated.
 *                  FALSE otherwise.
 */
wiced_bool_t wiced_aon_register_power_callback (uint16_t  size,
                                    wiced_aon_power_callback_fp    cb_ptr);

/*
 *
 * Function         wiced_aon_get_boot_mode
 *
 * Returns Cold boot or Warm/Fast boot.
 *
 * @return          wiced_aon_boot_type_e
 */
wiced_bool_t wiced_aon_get_boot_mode(void);

/*
 *
 * Function         wiced_aon_allow_sleep
 *
 * Application to let the AON when to allow sleep.
 *
 * @param[in]       allow_sleep: WICED_TRUE to enable sleep. WICED_FALSE to disable sleep
 *
 */
void wiced_aon_allow_sleep( wiced_bool_t allow_sleep);
#endif //_WICED_AON_H_
