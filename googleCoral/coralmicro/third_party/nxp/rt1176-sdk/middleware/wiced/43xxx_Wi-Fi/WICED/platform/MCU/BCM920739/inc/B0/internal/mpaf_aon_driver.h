/*******************************************************************************
 * THIS INFORMATION IS PROPRIETARY TO BROADCOM CORP
 *
 * ------------------------------------------------------------------------------
 *
 * Copyright (c) 2015 Broadcom Corp.
 *
 *          ALL RIGHTS RESERVED
 *
 ********************************************************************************
 *
 * File Name: mpaf_aon_driver.h
 *
 * Abstract:  This file provides interface definitions for the MPAF AON Driver
 *
 *
 * Functions:
 *
 *******************************************************************************/
#ifndef _MPAF_AON_DRIVER_H_
#define _MPAF_AON_DRIVER_H_

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * \defgroup mpaf_aon_driver MPAF AON Driver Module
 *
 * Interface to access AON memory for the store/restore functionality for
 * various modules (like stack layers, applications etc).
 * The module manages to allocate the single chunk of AON memory for all
 * modules that have registered.
 * This handles notifying to each modules on mode transitions (between SDS
 * and Normal).
 */

/**
 * \addtogroup mpaf_aon_driver
 *
 * @{
 */

#include "brcm_fw_types.h"

#ifdef MPAF_ENABLE

// Used for the BT stack init
extern BOOL32 mpaf_aon_boot_mode;

/*******************************************************************************
 *                   Defines & Macros
 *******************************************************************************/
#define MPAF_AON_DRIVER_MAX_NO_OF_MODULES            (3)

/*******************************************************************************
 *                   Type Definitions
 *******************************************************************************/

/*
 * MPAF_AON_DRIVER_ACTION
 * Action type for the callback handlers.
 */
enum
{
    MPAF_AON_DRIVER_SAVE = 0, /* Save context from SRAM to AON */
    MPAF_AON_DRIVER_RESTORE, /* Restore context from AON to SRAM */
};
typedef UINT8 MPAF_AON_DRIVER_ACTION;

/*
 * Gives the AON Boot mode.
 * Use the boot_aon_Signature for signature verification instead of separate variable?
 * The HCI reset needs to be take care, for now, using separate.
 */
enum
{
    MPAF_AON_COLD_BOOT, MPAF_AON_FAST_BOOT,
};
#define mpaf_aon_get_boot_mode()            (mpaf_aon_boot_mode)
#define mpaf_aon_set_boot_mode(mode)        (mpaf_aon_boot_mode = mode)

/*
 * MPAF_AON_DRIVER_CALLBACK_FP
 * Function prototype for the store/restore callback.
 *
 * \param type Action type for the handler
 *
 * \param ptr  Pointer to the AON memory location for the handler to 
 *             store/restore the content, as per action type.
 *
 * \param size Total number of valid AON octets for the handler to access.
 *
 * \return When action type is MPAF_AON_DRIVER_SAVE:
 *              - TRUE to indicates the module successfully saved the context
 *                in AON memory and agreed to enter into SDS mode.
 *              - FALSE when not ready to go into SDS mode.
 *         Return type is ignored otherwise.
 */
typedef BOOL32 (*MPAF_AON_DRIVER_CALLBACK_FP)( MPAF_AON_DRIVER_ACTION type, void *ptr, UINT16 size );

/*******************************************************************************
 *                   Function Prototypes
 *******************************************************************************/

/*******************************************************************************
 * Function: mpaf_aon_driver_register_for_context_save
 *
 * Abstract: API to register with MPAF AON driver to save/restore the context
 *           during the SDS mode transitions (in AON memory)
 *
 * Input : size      - Total number of AON octets needed for the module to
 *                     save its context.
 *         cb_ptr    - Callback handler to be notified during the mode
 *                     transition.
 *
 * Return: TRUE if requested number of AON octets can be allocated.
 *         FALSE otherwise.
 *
 *******************************************************************************/
BOOL32 mpaf_aon_driver_register_for_context_save( UINT16 size, MPAF_AON_DRIVER_CALLBACK_FP cb_ptr );

/*******************************************************************************
 * Function: mpaf_aon_driver_register_complete
 *
 * Abstract: API to indicate the AON driver of the registration complete.
 *           When all the modules have completed the registration, the final
 *           module (application) will call this API.
 *           The AON driver will allocate the total requested number of AON
 *           memory for all the registered modules.
 *
 * Return: Total number of AON memory allocated.
 *         Returns zero if the AON memory can not be allocated. The application
 *         should handle this scenario.
 *
 *******************************************************************************/
UINT32 mpaf_aon_driver_register_complete( void );

void mpaf_aon_driver_init( void );
void mpaf_aon_driver_resume_from_uBCS_mode( void );
void mpaf_aon_driver_handle_sds_test_mode( BOOL32 mode );

/** @} */   // end: mpaf_aon_driver
#ifdef __cplusplus
}
#endif

#endif // MPAF_ENABLE

#endif   //_MPAF_AON_DRIVER_H_

