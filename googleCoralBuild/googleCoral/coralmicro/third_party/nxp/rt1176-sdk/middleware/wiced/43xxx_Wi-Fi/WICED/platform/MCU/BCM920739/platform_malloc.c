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

/** @file
 * Define platform specific malloc and free functions
 * We are overriding the std libc malloc() and free() with
 * memory management functions supported from 20739 WICED HAL.
 * Memory allocation will happen from the dynamic memory pool
 * allocated during the BT stack init from the App.
 * It is important that you declare required buffer pool sizes
 * and numbers in the BT config during the BT stack init.
 *
 */
#include <stdlib.h>
#include <string.h>
#include "data_types.h"
#include "wiced_gki.h"

/******************************************************
 *                      Macros
 ******************************************************/

/******************************************************
 *                    Constants
 ******************************************************/

/******************************************************
 *                   Enumerations
 ******************************************************/

/******************************************************
 *                 Type Definitions
 ******************************************************/

/******************************************************
 *                    Structures
 ******************************************************/

/******************************************************
 *               Static Function Declarations
 ******************************************************/

/******************************************************
 *               Variable Definitions
 ******************************************************/

/******************************************************
 *               Function Definitions
 ******************************************************/
/** Get dynamic memory from buffer pool
 *  which will be greater or equal to the requested size.
 *
 * @param[in] size : size of the buffer
 *
 * @return  pointer to the buffer, if buffer is available
 *              NULL, if buffer is not available
 */
void* malloc( size_t size )
{
    return wiced_bt_get_buffer((uint16_t)size);
}

void* calloc( size_t count, size_t eltsize )
{
    void *ptr;
    ptr = wiced_bt_get_buffer((uint16_t)(count*eltsize));
    if(!ptr)
        return NULL;
    else
        memset(ptr,0,(count*eltsize));
    return ptr;
}
/** Free the buffer
 *
 * @param[in] p_buf:address of the beginning of a buffer
 *
 * @return    None
 */
void free(void *ptr)
{
    if(ptr)
        wiced_bt_free_buffer(ptr);
}
