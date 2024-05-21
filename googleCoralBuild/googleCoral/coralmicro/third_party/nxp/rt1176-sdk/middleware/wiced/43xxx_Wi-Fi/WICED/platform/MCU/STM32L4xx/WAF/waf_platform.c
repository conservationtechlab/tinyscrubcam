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
 * Defines STM32L4xx WICED application framework functions
 */
#include <string.h>
#include <stdlib.h>
#include "spi_flash.h"
#include "platform_config.h"
#include "platform_peripheral.h"
#include "wwd_assert.h"
#include "wiced_framework.h"
#include "elf.h"
#include "wiced_apps_common.h"
#include "waf_platform.h"
#include "stm32l4xx_hal_flash_ex.h"

#define PLATFORM_INTERNAL_FLASH_WRITE_SIZE_BYTES            (8)
#define PLATFORM_INTERNAL_FLASH_WRITE_SIZE_MASK             (PLATFORM_INTERNAL_FLASH_WRITE_SIZE_BYTES - 1)
#define PLATFORM_INTERNAL_FLASH_ADRESS_ROUND_UP(a)          (((a) + PLATFORM_INTERNAL_FLASH_WRITE_SIZE_MASK) & (uint32_t)(~(PLATFORM_INTERNAL_FLASH_WRITE_SIZE_MASK)))
#define PLATFORM_INTERNAL_FLASH_ADRESS_ROUND_DOWN(a)        ((a) & (uint32_t)(~(PLATFORM_INTERNAL_FLASH_WRITE_SIZE_MASK)))
#define PLATFORM_INTERNAL_FLASH_ADDRESS_PTR_ROUND_UP(p)     ((void*)PLATFORM_INTERNAL_FLASH_ADRESS_ROUND_UP((uint32_t)(p)))
#define PLATFORM_INTERNAL_FLASH_ADDRESS_PTR_ROUND_DOWN(p)   ((void*)PLATFORM_INTERNAL_FLASH_ADRESS_ROUND_DOWN((uint32_t)(p)))
#define PLATFORM_INTERNAL_FLASH_OFFSET(a)                   ((uint32_t)(a) & (PLATFORM_INTERNAL_FLASH_WRITE_SIZE_MASK) )
#define PLATFORM_INTERNAL_FLASH_WRITE_SIZE_IS_ALIGNED(a)    (((uint32_t)(a) & (PLATFORM_INTERNAL_FLASH_WRITE_SIZE_MASK)) == 0)
#define PLATFORM_INTERNAL_FLASH_WRITE_ADDRESS_IS_ALIGNED(a) (((uint32_t)(a) & (PLATFORM_INTERNAL_FLASH_WRITE_SIZE_MASK)) == 0)

#define APP_CODE_START_ADDR   ((uint32_t)&app_code_start_addr_loc)
#define SRAM_START_ADDR       ((uint32_t)&sram_start_addr_loc)
#define ERASED_DATA_64        ((uint64_t)0xffffffffffffffff )

extern void* app_code_start_addr_loc;
extern void* sram_start_addr_loc;

static uint32_t GetPage( uint32_t Addr );
static uint32_t GetBank( uint32_t Addr );
static wiced_bool_t is_flash_chunk_erased( uint8_t* aligned_address, uint32_t block_size_in_bytes );
static platform_result_t Flash_PageWrite( uint32_t page_number, uint8_t *data_ptr );

#if defined ( __ICCARM__ )

static inline void __jump_to( uint32_t addr )
{
    __asm( "ORR R0, R0, #1" ); /* Last bit of jump address indicates whether destination is Thumb or ARM code */
    __asm( "BX R0" );
}

#elif defined ( __GNUC__ )

__attribute__( ( always_inline ) ) static __INLINE void __jump_to( uint32_t addr )
{
    addr |= 0x00000001; /* Last bit of jump address indicates whether destination is Thumb or ARM code */
    __ASM volatile ("BX %0" : : "r" (addr) );
}

#endif

__attribute__ ((section (".fast")))
void platform_start_app( uint32_t entry_point )
{

    /* Simulate a reset for the app: */
    /*   Switch to Thread Mode, and the Main Stack Pointer */
    /*   Change the vector table offset address to point to the app vector table */
    /*   Set other registers to reset values (esp LR) */
    /*   Jump to the reset vector */

    if ( entry_point == 0 )
    {
        uint32_t* vector_table = (uint32_t*) APP_CODE_START_ADDR;
        entry_point = vector_table[ 1 ];
    }

    __asm( "MOV LR,        #0xFFFFFFFF" );
    __asm( "MOV R1,        #0x01000000" );
    __asm( "MSR APSR_nzcvq,     R1" );
    __asm( "MOV R1,        #0x00000000" );
    __asm( "MSR PRIMASK,   R1" );
    __asm( "MSR FAULTMASK, R1" );
    __asm( "MSR BASEPRI,   R1" );
    __asm( "MSR CONTROL,   R1" );

    /*  Now rely on the app crt0 to load VTOR / Stack pointer

     SCB->VTOR = vector_table_address; - Change the vector table to point to app vector table
     __set_MSP( *stack_ptr ); */

    __jump_to( entry_point );

}

/**
 * @brief  Gets the page of a given address
 * @param  Addr: Address of the FLASH Memory
 * @retval The page of a given address
 */
__attribute__ ((section (".fast")))
static uint32_t GetPage( uint32_t Addr )
{
    uint32_t page = 0;

    if ( Addr < ( FLASH_BASE + (uint32_t) FLASH_BANK_SIZE ) )
    {
        /* Bank 1 */
        page = ( Addr - FLASH_BASE ) / FLASH_PAGE_SIZE;
    }
    else
    {
        /* Bank 2 */
        page = ( Addr - ( FLASH_BASE + (uint32_t) FLASH_BANK_SIZE ) ) / FLASH_PAGE_SIZE;
    }

    return page;
}

/**
 * @brief  Gets the bank of a given address
 * @param  Addr: Address of the FLASH Memory
 * @retval The bank of a given address
 */
__attribute__ ((section (".fast")))
static uint32_t GetBank( uint32_t Addr )
{
    uint32_t bank = 0;

    if ( READ_BIT(SYSCFG->MEMRMP, SYSCFG_MEMRMP_FB_MODE) == 0 )
    {
        /* No Bank swap */
        if ( Addr < ( FLASH_BASE + (uint32_t) FLASH_BANK_SIZE ) )
        {
            bank = FLASH_BANK_1;
        }
        else
        {
            bank = FLASH_BANK_2;
        }
    }
    else
    {
        /* Bank swap */
        if ( Addr < ( FLASH_BASE + (uint32_t) FLASH_BANK_SIZE ) )
        {
            bank = FLASH_BANK_2;
        }
        else
        {
            bank = FLASH_BANK_1;
        }
    }

    return bank;
}

__attribute__ ((section (".fast")))
platform_result_t platform_erase_flash( uint16_t start_page, uint16_t end_page )
{
    FLASH_EraseInitTypeDef flash_erase;
    uint32_t erase_error;

    HAL_FLASH_Unlock( );

    /* Clear any error flags */
    __HAL_FLASH_CLEAR_FLAG( FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR | FLASH_FLAG_PGSERR );

    platform_watchdog_kick( );

    flash_erase.Banks     = GetBank( start_page );
    flash_erase.TypeErase = FLASH_TYPEERASE_PAGES;
    flash_erase.Page      = start_page;
    flash_erase.NbPages   = ( uint32_t )( ( end_page - start_page ) + 1 );
    if ( HAL_FLASHEx_Erase( &flash_erase, &erase_error ) != HAL_OK )
    {
        HAL_FLASH_Lock( );
        return PLATFORM_ERROR;
    }

    platform_watchdog_kick( );
    HAL_FLASH_Lock( );
    return PLATFORM_SUCCESS;
}

__attribute__ ((section (".fast")))
static wiced_bool_t is_flash_chunk_erased( uint8_t* address, uint32_t block_size_in_bytes )
{
    uint32_t byte_idx;

    for ( byte_idx = 0; byte_idx < block_size_in_bytes; byte_idx++ )
    {
        if ( ( (uint8_t*) address )[ byte_idx ] != 0xFF )
        {
            return WICED_FALSE;
        }
    }

    return WICED_TRUE;
}

__attribute__ ((section (".fast")))
static platform_result_t Flash_PageWrite( uint32_t page_number, uint8_t *data_ptr )
{
    uint32_t i;
    uint64_t *double_word;
    uint32_t write_address = FLASH_BASE + ( page_number * FLASH_PAGE_SIZE );

    HAL_FLASH_Unlock( );
    for ( i = 0; i < FLASH_PAGE_SIZE / PLATFORM_INTERNAL_FLASH_WRITE_SIZE_BYTES; i++ )
    {
        double_word = (uint64_t*) data_ptr;

        if ( *double_word != ERASED_DATA_64 )
        {
            if ( HAL_FLASH_Program( FLASH_TYPEPROGRAM_DOUBLEWORD, write_address, *double_word ) != HAL_OK )
            {
                HAL_FLASH_Lock( );
                return PLATFORM_ERROR;
            }
        }

        write_address += PLATFORM_INTERNAL_FLASH_WRITE_SIZE_BYTES;
        data_ptr      += PLATFORM_INTERNAL_FLASH_WRITE_SIZE_BYTES;
    }

    HAL_FLASH_Lock( );
    return PLATFORM_SUCCESS;
}

__attribute__ ((section (".fast")))
platform_result_t platform_write_flash_chunk( uint32_t address, const void* data, uint32_t size )
{
    uint32_t page_offset;
    uint32_t space_available_in_page;
    uint32_t space_available_in_dw;
    uint32_t page_number;
    uint32_t size_remaining;
    uint8_t *src_address;
    uint32_t dst_address;
    uint32_t start_page;
    uint32_t end_page;
    uint32_t flash_write_aligned_address;
    platform_result_t result = PLATFORM_SUCCESS;

    /* TODO - Ensure that the write is within the same page/sector/bank ? */
    /* if data to be written is already present in flash , return */
    if ( memcmp( (void*) address, (void*) data, size ) == 0 )
    {
        return PLATFORM_SUCCESS;
    }

    start_page                  = GetPage( address );
    end_page                    = GetPage( address + size );
    flash_write_aligned_address = PLATFORM_INTERNAL_FLASH_ADRESS_ROUND_DOWN( address );
    size_remaining              = size;
    src_address                 = (uint8_t*) data;
    dst_address                 = address;


    /* TOD0 - Calloc the temp_buffer */
    for ( page_number = start_page; page_number <= end_page; page_number++ )
    {
        /* is flash chunk is erased */
        if ( is_flash_chunk_erased( (uint8_t*) flash_write_aligned_address, PLATFORM_INTERNAL_FLASH_ADRESS_ROUND_UP( size ) ) == WICED_TRUE )
        {
            page_offset = ( dst_address % FLASH_PAGE_SIZE );
            space_available_in_page = MIN( ( FLASH_PAGE_SIZE - page_offset ), size_remaining );

            /* loop and write Double words in each iteration */
            while ( space_available_in_page != 0 )
            {
                uint32_t i            = 0;
                uint32_t dw_offset    = ( dst_address % PLATFORM_INTERNAL_FLASH_WRITE_SIZE_BYTES );
                uint8_t dw[ 8 ]       = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };
                uint64_t *double_word = (uint64_t*) dw;

                space_available_in_dw = MIN( ( PLATFORM_INTERNAL_FLASH_WRITE_SIZE_BYTES - dw_offset ), space_available_in_page );
                for ( i = 0; i < space_available_in_dw; i++ )
                {
                    dw[ dw_offset++ ] = *src_address++;
                }

                HAL_FLASH_Unlock( );

                if ( HAL_FLASH_Program( FLASH_TYPEPROGRAM_DOUBLEWORD, flash_write_aligned_address, *double_word ) != HAL_OK )
                {
                    result = PLATFORM_ERROR;
                    HAL_FLASH_Lock( );
                    goto exit_platform_write_flash_chunk;
                }

                HAL_FLASH_Lock( );
                size_remaining              -= space_available_in_dw;
                dst_address                 += space_available_in_dw;
                flash_write_aligned_address += PLATFORM_INTERNAL_FLASH_WRITE_SIZE_BYTES;
                space_available_in_page     -= space_available_in_dw;
            }
        }
        /* flash chunk is not erased */
        else
        {
            uint8_t temp_buffer[ FLASH_PAGE_SIZE ];

            /* copy page to temp_buffer */
            memcpy( temp_buffer, (uint8_t*) ( FLASH_BASE + page_number * FLASH_PAGE_SIZE ), FLASH_PAGE_SIZE );

            /* erase page */
            platform_erase_flash( (uint16_t) page_number, (uint16_t) page_number );

            /* modify temp buffer */
            page_offset = ( dst_address % FLASH_PAGE_SIZE );
            space_available_in_page = MIN( ( FLASH_PAGE_SIZE - page_offset ), size_remaining );
            memcpy( temp_buffer + page_offset, src_address, space_available_in_page );

            /* write temp_buffer to the page */
            if ( Flash_PageWrite( page_number, temp_buffer ) != PLATFORM_SUCCESS )
            {
                result = PLATFORM_ERROR;
                goto exit_platform_write_flash_chunk;
            }

            size_remaining              -= space_available_in_page;
            dst_address                 += space_available_in_page;
            src_address                 += space_available_in_page;
            flash_write_aligned_address += space_available_in_page;
        }
    }

    if ( memcmp( (void*) address, (void*) data, size ) != 0 )
    {
        result = PLATFORM_ERROR;
        __asm__("bkpt");
    }

exit_platform_write_flash_chunk:

    return result;
}

__attribute__ ((section (".fast")))
void platform_erase_app_area( uint32_t physical_address, uint32_t size )
{
    UNUSED_PARAMETER( physical_address );
    UNUSED_PARAMETER( size );
}

__attribute__ ((section (".fast")))
void platform_load_app_chunk( const image_location_t* app_header_location, uint32_t offset, void* physical_address, uint32_t size )
{
    UNUSED_PARAMETER( app_header_location );
    UNUSED_PARAMETER( offset );
    UNUSED_PARAMETER( physical_address );
    UNUSED_PARAMETER( size );
}
