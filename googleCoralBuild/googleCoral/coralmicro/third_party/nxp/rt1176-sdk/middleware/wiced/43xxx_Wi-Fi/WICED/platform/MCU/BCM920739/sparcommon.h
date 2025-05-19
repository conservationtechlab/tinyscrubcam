/*
 * Copyright 2021, Cypress Semiconductor Corporation or a subsidiary of 
 * Cypress Semiconductor Corporation. All Rights Reserved.
 * 
 * This software, associated documentation and materials ("Software"),
 * is owned by Cypress Semiconductor Corporation
 * or one of its subsidiaries ("Cypress") and is protected by and subject to
 * worldwide patent protection (United States and foreign),
 * United States copyright laws and international treaty provisions.
 * Therefore, you may use this Software only as provided in the license
 * agreement accompanying the software package from which you
 * obtained this Software ("EULA").
 * If no EULA applies, Cypress hereby grants you a personal, non-exclusive,
 * non-transferable license to copy, modify, and compile the Software
 * source code solely for use in connection with Cypress's
 * integrated circuit products. Any reproduction, modification, translation,
 * compilation, or representation of this Software except as specified
 * above is prohibited without the express written permission of Cypress.
 *
 * Disclaimer: THIS SOFTWARE IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, NONINFRINGEMENT, IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. Cypress
 * reserves the right to make changes to the Software without notice. Cypress
 * does not assume any liability arising out of the application or use of the
 * Software or any product or circuit described in the Software. Cypress does
 * not authorize its products for use in any products where a malfunction or
 * failure of the Cypress product may reasonably be expected to result in
 * significant property damage, injury or death ("High Risk Product"). By
 * including Cypress's product in a High Risk Product, the manufacturer
 * of such system or application assumes all risk of such use and in doing
 * so agrees to indemnify Cypress against all liability.
 */
/*******************************************************************************
 * All common definitions for this SPAR
 *******************************************************************************/

#ifndef _SPAR_COMMON_H_
#define _SPAR_COMMON_H_

#include "brcm_fw_types.h"

#ifdef __cplusplus
#define EXTERN_C extern "C"
#else
#define EXTERN_C extern
#endif

EXTERN_C void (*g_rf4ce_pre_init)( void );
EXTERN_C void (*g_rf4ce_start)( void );
EXTERN_C void (*g_6loapp_application_init)( void );
EXTERN_C UINT32 (*g_zbstack_pre_init)( void );
EXTERN_C UINT32 (*g_zbstack_post_init)( void );
EXTERN_C void (*g_zbstack_start)( void );
EXTERN_C void (*g_zbapp_add_ao)( void );
EXTERN_C void (*g_zbapp_add_clst)( void );
EXTERN_C void (*wiced_bt_app_pre_init)( void );
EXTERN_C void application_start( void );

#define APPLICATION_START() __attribute__((section(".app_init_code"))) \
void application_start( void )
void wiced_bt_set_app_start_function( void (*v)( void ) );

INLINE void wiced_bt_set_app_start_function(void (*v)(void))
{
    wiced_bt_app_pre_init = v;
}

#endif
