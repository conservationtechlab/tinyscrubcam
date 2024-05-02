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
*Defines the interfaces for Buffer , Timer and Event Management Services
*/
#ifndef _WICED_GKI_H_
#define _WICED_GKI_H_

#include "wiced.h"

/*
 * Function prototype for the timer call backs.
 */
typedef void ( *wiced_timer_callback_t )( uint32_t cb_params );

/** Forward typedefinition of the wiced transport buffer pool */
typedef struct _wiced_trans_buffer_pool_t wiced_trans_buffer_pool_t;

typedef struct wiced_pool_t wiced_bt_buffer_pool_t;

/*****************************************************************************
**                                                 Function Declarations
*****************************************************************************/


/** Get a free buffer which  is of size greater or equal to the requested size.
 *
 * @param[in]    size              : size of the buffer
 *
 * @return  pointer to the buffer, if buffer is available
 *              NULL, if buffer is not available
 */
void* wiced_bt_get_buffer( UINT16 size );


/** Free the buffer
 *
 * @param[in]    p_buf              :address of the beginning of a buffer
 *
 * @return    None
 */
void wiced_bt_free_buffer( void *p_buf );


/**
 * Function         wiced_bt_did_stack_overflow
 *
 * Checks if the application thread stack overflowed at some point
 *
 * @return    TRUE : on stack overflow;
 *            FALSE : if no stack overflow
 *
 */
uint8_t wiced_bt_did_stack_overflow(void);

/**
 * Function         wiced_bt_stack_check_init
 *
 * Prepares the stack to allow the app to check for stack overflow.
 *
 */
void wiced_bt_stack_check_init(void);

/**
 * Function         wiced_trans_create_buffer_pool
 *
 * Creates a buffer pool
 *
 * * @param[in]     buffer_size         : Size of each buffer in pool
 * * @param[in]     buffer_count        : Number of buffers in the pool
 *
 * @return     :  pointer to the buffer pool on success, NULL on failure
 * 
 */
wiced_trans_buffer_pool_t * wiced_trans_create_buffer_pool( int32_t buffer_size, int32_t buffer_count );

/**
 * Function         wiced_trans_destroy_buffer_pool
 *
 * destoryes specified buffer pool
 *
 * * @param[in]     p_pool               : pointer to the pool
 * * @param[in]     buffer_size         : Size of each buffer in pool
 * * @param[in]     buffer_count        : Number of buffers in the pool
 *
 * @return     :  None
 * 
 */
void wiced_trans_destroy_buffer_pool( wiced_trans_buffer_pool_t *p_pool, int32_t buffer_size, int32_t buffer_count );

/**
 * Function         wiced_trans_buffer_alloc
 *
 * Returns a buffer from the pool
 *
 * * @param[in]    p_pool           : Pointer to buffer pool returned from wiced_trans_create_buffer_pool
 *
 * @return     : pointer to the buffer from the pool on success, NULL on failure
 */
void* wiced_trans_buffer_alloc( wiced_trans_buffer_pool_t * p_pool );

/**
 * Function         wiced_trans_get_buffer_size
 *
 * Returns the size of buffer of the pool
 *
 * * @param[in]    p_pool           : Pointer to buffer pool returned from wiced_trans_create_buffer_pool
 *
 * @return     : size of the buffers of the pool
 */
uint16_t wiced_trans_get_buffer_size ( wiced_trans_buffer_pool_t *p_pool );

/**
 * Function         wiced_trans_get_available_buffers
 *
 * To get the number of buffers available in the pool
 *
 * * @param[in]    p_pool           : Pointer to buffer pool created using wiced_trans_create_buffer_pool
 *
 * @return     : the number of buffers available in the pool
 */
uint32_t wiced_trans_get_available_buffers( wiced_trans_buffer_pool_t *p_pool );

/**
 * Function         wiced_trans_create_event_buffer_pool
 *
 * Creates a buffer pool for sending the event data over uart to the host
 *
 * * @param[in]     buffer_size           : Size of each buffer in pool
 * * @param[in]     buffer_count          : Number of buffers in the pool
 *
 * @return     :  pointer to the buffer pool on success, NULL on failure
 */
wiced_trans_buffer_pool_t * wiced_trans_create_event_buffer_pool( int32_t buffer_size, int32_t buffer_count );

/**
 * Function         wiced_trans_event_buffer_alloc
 *
 * Returns a buffer from the event buffer pool
 *
 * * @param[in]    p_pool           : Pointer to buffer pool returned from wiced_trans_create_event_buffer_pool
 *
 * @return     : pointer to the buffer from the pool on success, NULL on failure
 */
void* wiced_trans_event_buffer_alloc( wiced_trans_buffer_pool_t * p_pool );

/**
 * Function         wiced_trans_get_event_buffer_size
 *
 * Returns the size of buffer of the pool
 *
 * * @param[in]    p_pool           : Pointer to buffer pool returned from wiced_trans_create_event_buffer_pool
 *
 * @return     : size of the buffers of the pool
 */
uint16_t wiced_trans_get_event_buffer_size ( wiced_trans_buffer_pool_t *p_pool );

/** Send the event to the host over the hci uart. This function creates the custom hci header internally.
 * Note - This function should be used only for buffer allocated using wiced_trans_event_buffer_alloc
 *
 *@param[in]    code                 :Class code and command opcode
 *@param[in]    p_buf                :Pointer to the event payload
 *@param[in]    length               :Event payload length
 *
 * @return   wiced_result_t
 */
wiced_result_t wiced_trans_send_event_buffer_data ( uint16_t code, uint8_t* p_buf, uint16_t length );

/**
 * Function         wiced_bt_ble_get_available_tx_buffers
 *
 * Used to get the available number of ble tx buffers
 *
 * Return           the available number of ble tx buffers
 */
uint32_t wiced_bt_ble_get_available_tx_buffers( void );

void wdog_generate_hw_reset(void);
uint32_t wiced_get_free_memory(void);

#endif //_WICED_GKI_H_

