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

/*
 * @file
 * Interface functions for Newlib libC implementation
 */
//#include <sys/errno.h>
#include <sys/stat.h>
#include <sys/times.h>
#include <sys/unistd.h>
#include <malloc.h>
#include "platform_toolchain.h"
#include <wwd_assert.h>
#include <wwd_constants.h>
//#include "wiced_bt_trace.h"
#include "platform.h"
#include "wiced_memory.h"
#include "platform_constants.h"

#ifdef LINT  /* Some defines to keep lint happy in the absense of newlib */
typedef char *  caddr_t;
struct mallinfo { int x; };
/*@-declundef@*/ /*@-exportheader@*/ extern struct mallinfo mallinfo(void); /*@+declundef@*/ /*@+exportheader@*/
#endif /* ifdef LINT */

static unsigned char *sbrk_heap_t = NULL;
static unsigned char *sbrk_heap_e = NULL;
extern void *spar_iram_bss_end, *spar_iram_bss_begin;

void platform_mem_init( void )
{
    uint8_t *bss_start,*bss_end;

    sbrk_heap_t = NULL;
    sbrk_heap_e = NULL;

    // Get the address of sbrk_heap_t. BSS section starts.
    bss_start = (uint8_t *)&spar_iram_bss_begin;
    bss_end = (uint8_t *)&spar_iram_bss_end; // including libc COMMON address

    memset(bss_start, 0, bss_end - bss_start);
}

platform_result_t platform_allocate_heap(uint32_t heap_size)
{
    wiced_bt_buffer_pool_t *heap_pool;
    /*If we have not allocate heap allocate now*/
    if(sbrk_heap_t == NULL)
    {
        /*Allocate single buffer pool of heap size defined by platform */
        heap_pool = wiced_bt_create_pool( (uint32_t)PLATFORM_HEAP_SIZE, (uint32_t)1);
        if(heap_pool == NULL)
            return PLATFORM_ERROR;
        sbrk_heap_t = (uint8_t*)wiced_bt_get_buffer_from_pool(heap_pool);
        if(sbrk_heap_t == NULL)
            return PLATFORM_ERROR;

        sbrk_heap_e = sbrk_heap_t + PLATFORM_HEAP_SIZE;
        //WICED_BT_TRACE("sbrk_heap_t %x , sbrk_heap_e %x heap_size %d\n",sbrk_heap_t,sbrk_heap_e,(uint32_t)PLATFORM_HEAP_SIZE);
    }
   return PLATFORM_SUCCESS;
}

/*@shared@*/ caddr_t _sbrk( int incr )
{
    unsigned char *prev_heap;

    if(sbrk_heap_t == NULL)
    {
        if(PLATFORM_SUCCESS != platform_allocate_heap(PLATFORM_HEAP_SIZE))
        {
            return (caddr_t) -1;
        }
    }

    if ( (sbrk_heap_t + incr) > sbrk_heap_e )
    {
        /* Out of dynamic memory heap space */

        //volatile struct mallinfo mi = mallinfo();
        // See variable mi for malloc information:
        // Total allocated :  mi.uordblks
        // Total free :       mi.fordblks
        //WICED_BT_TRACE("!!!Out of dynamic memory heap space!!!\n");

        wiced_assert("Out of dynamic memory heap space", 0 != 0 );

        return (caddr_t) -1;
    }
    prev_heap = sbrk_heap_t;

    sbrk_heap_t += incr;

    return (caddr_t) prev_heap;
}

/* Override the default Newlib assert, since it tries to do printf stuff */

void __assert_func( const char * file, int line, const char * func, const char * failedexpr )
{
    /* Assertion failed!
     *
     * To find out where this assert was triggered, either look up the call stack,
     * or inspect the file, line and function parameters
     */
    wiced_assert("newlib assert", 0 != 0 );

    UNUSED_PARAMETER( file );
    UNUSED_PARAMETER( line );
    UNUSED_PARAMETER( func );
    UNUSED_PARAMETER( failedexpr );
    while(1);
}

/*
 * These are needed for C++ programs. They shouldn't really be here, so let's just
 * hit a breakpoint when these functions are called.
 */

int _kill( int pid, int sig )
{
    wiced_assert("", 0 != 0 );

    UNUSED_PARAMETER( pid );
    UNUSED_PARAMETER( sig );
    return 0;
}

int _getpid( void )
{
    wiced_assert("", 0 != 0 );
    return 0;
}

/*@+exportheader@*/

/* Search memory in reverse for the last instance of a character in a buffer*/
void *memrchr( const void *source_buffer, int search_character, size_t buffer_length )
{
    unsigned char * read_pos = ((unsigned char *)source_buffer + (buffer_length-1));
    while ( ( *read_pos != (unsigned char) search_character ) &&
            ( read_pos >= (unsigned char*) source_buffer ) )
    {
        read_pos--;
    }
    return ( read_pos >= (unsigned char*) source_buffer )?read_pos:NULL;
}

