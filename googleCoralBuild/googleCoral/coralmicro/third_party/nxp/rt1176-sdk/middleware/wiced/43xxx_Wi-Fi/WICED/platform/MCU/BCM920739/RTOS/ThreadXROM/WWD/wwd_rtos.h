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
#pragma once

//#include "wiced_bt_dev.h"
//#include "wiced_rtos.h"
//#include "wiced_bt_cfg.h"
//#include "wiced_bt_trace.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif


/******************************************************
 *                      Macros
 ******************************************************/
#define NULL_MAC( a )  (((((unsigned char*)a)[0])==0)&& \
                            ((((unsigned char*)a)[1])==0)&& \
                            ((((unsigned char*)a)[2])==0)&& \
                            ((((unsigned char*)a)[3])==0)&& \
                            ((((unsigned char*)a)[4])==0)&& \
                            ((((unsigned char*)a)[5])==0))

    /* Macros for comparing MAC addresses */
#define CMP_MAC( a, b )  (((((unsigned char*)a)[0])==(((unsigned char*)b)[0]))&& \
                              ((((unsigned char*)a)[1])==(((unsigned char*)b)[1]))&& \
                              ((((unsigned char*)a)[2])==(((unsigned char*)b)[2]))&& \
                              ((((unsigned char*)a)[3])==(((unsigned char*)b)[3]))&& \
                              ((((unsigned char*)a)[4])==(((unsigned char*)b)[4]))&& \
                              ((((unsigned char*)a)[5])==(((unsigned char*)b)[5])))

/******************************************************
 *                    Constants
 ******************************************************/
#define RTOS_HIGHER_PRIORTIY_THAN(x)     ((x) > RTOS_HIGHEST_PRIORITY ? (x)-1 : RTOS_HIGHEST_PRIORITY)
#define RTOS_LOWER_PRIORTIY_THAN(x)      ((x) < RTOS_LOWEST_PRIORITY  ? (x)+1 : RTOS_LOWEST_PRIORITY )
#define RTOS_LOWEST_PRIORITY             (1023)
/*! Minimum value for the "above normal but below GUI" priority range, which spans 13 to 16.
 * value taken from fw2
 */
#define THREAD_PRIORITY_ABOVE_NORMAL_MIN        13
#define RTOS_HIGHEST_PRIORITY            THREAD_PRIORITY_ABOVE_NORMAL_MIN
#define RTOS_DEFAULT_THREAD_PRIORITY     (4)

#define RTOS_USE_STATIC_THREAD_STACK

/* The number of system ticks per second */
#define SYSTICK_FREQUENCY  (100)
#define OS_TIMER_TICK_PERIOD_IN_MS     (10)

#ifndef WWD_LOGGING_STDOUT_ENABLE
#ifdef DEBUG
#define WWD_THREAD_STACK_SIZE        (1248 + 1400) /*(632)*/   /* Stack checking requires a larger stack */
#else /* ifdef DEBUG */
#define WWD_THREAD_STACK_SIZE        (1024 + 1400) /*(544)*/
#endif /* ifdef DEBUG */
#else /* if WWD_LOGGING_STDOUT_ENABLE */
#define WWD_THREAD_STACK_SIZE        (544 + 4096 + 1400)   /* WWD_LOG uses printf and requires minimum 4K stack space */
#endif /* WWD_LOGGING_STDOUT_ENABLE */


/******************************************************
 *                   Enumerations
 ******************************************************/
typedef enum
{
    ALLOW_THREAD_TO_SLEEP,
    KEEP_THREAD_ACTIVE,
}wiced_delay_type_t;
/******************************************************
 *                 Type Definitions
 ******************************************************/


/******************************************************
 *                    Structures
 ******************************************************/
typedef struct
{
    uint32_t time_slice;    /* 0 to disable timeslice among same prio threads; other value to specify slice length */
    uint32_t arg;
} host_rtos_thread_config_type_t;

typedef struct
{
    void *semaphore;
}host_semaphore_type_t;

typedef struct
{
    void *thread;
}host_thread_type_t;

typedef struct
{
    void *queue;
}host_queue_type_t;

typedef struct
{
    void *mutex;
}host_mutex_type_t;     /** ThreadX definition of a mutex */


/******************************************************
 *                 Global Variables
 ******************************************************/

/******************************************************
 *               Function Declarations
 ******************************************************/

#ifdef __cplusplus
} /* extern "C" */
#endif
