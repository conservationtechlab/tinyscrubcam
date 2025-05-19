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
 * Defines PSoC 6 WICED application framework functions
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

#define APP_CODE_START_ADDR   ((uint32_t)&app_code_start_addr_loc)
#define SRAM_START_ADDR       ((uint32_t)&sram_start_addr_loc)
extern void* app_code_start_addr_loc;
extern void* sram_start_addr_loc;

#define INTERNAL_FLASH_START_ADDRESS     (uint32_t)( 0x10000000 )
#define INTERNAL_FLASH_END_ADDRESS       (uint32_t)( 0x10100000 )
#define SFLASH_START_ADDRESS             (uint32_t)( 0x18000000 )
#define SFLASH_END_ADDRESS               (uint32_t)( 0x19000000 )

#if defined ( __ICCARM__ )

static inline void __jump_to( uint32_t addr )
{
    __asm( "ORR R0, R0, #1" );  /* Last bit of jump address indicates whether destination is Thumb or ARM code */
    __asm( "BX R0" );
}

#elif defined ( __GNUC__ )

__attribute__( ( always_inline ) ) static __INLINE void __jump_to( uint32_t addr )
{
    addr |= 0x00000001;  /* Last bit of jump address indicates whether destination is Thumb or ARM code */
  __ASM volatile ("BX %0" : : "r" (addr) );
}

#endif

void platform_start_app( uint32_t entry_point )
{

    /* Simulate a reset for the app: */
    /*   Switch to Thread Mode, and the Main Stack Pointer */
    /*   Change the vector table offset address to point to the app vector table */
    /*   Set other registers to reset values (esp LR) */
    /*   Jump to the reset vector */


    if ( entry_point == 0 )
    {
        uint32_t* vector_table =  (uint32_t*) APP_CODE_START_ADDR;
        entry_point = vector_table[1];
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

/* Setting Linker attributes to execute this and some of the following function from RAM and not XIP from Flash
 * while Flash is being written - safety precaution due to read-while-write limitation of flash.
 */
__attribute__ ((section (".fast")))
platform_result_t platform_erase_flash( uint16_t start_sector, uint16_t end_sector )
{
    /* Need some check for SECTOR_SIZE to use on stack variable or allocation */
    uint32_t buff[SECTOR_SIZE / 4];
    cy_en_flashdrv_status_t status;
    uint32_t i, rowaddr;

    for ( i = 0; i < SECTOR_SIZE / 4; i++ )
    {
        buff[i] = 0xffffffff;
    }

    for ( i = start_sector; i <= end_sector; i++ )
    {

       rowaddr = ( CY_FLASH_BASE + ( i * CY_FLASH_SIZEOF_ROW ) );

       if ( ( i % 100 ) == 0 )      /* Kick Watchdog every 1.5 seconds - flash page write take 15 milliseconds */
       {
          platform_watchdog_kick( );
       }

       status = Cy_Flash_WriteRow ( rowaddr, buff );

       if ( status != CY_FLASH_DRV_SUCCESS )
       {
           return ( PLATFORM_ERROR );
       }
    }
    return PLATFORM_SUCCESS;
}

__attribute__ ((section (".fast")))
static void platform_read_page( uint16_t sector, uint32_t *dest )
{
    uint32_t i;
    uint32_t* read_ptr;

    if ( dest == NULL )
    {
       return;
    }

    read_ptr = ( uint32_t* ) ( CY_FLASH_BASE + ( sector * CY_FLASH_SIZEOF_ROW ) );

    for ( i = 0; i < CY_FLASH_SIZEOF_ROW / 4; i++ )
    {
        *dest++ = *read_ptr++;
    }
}

__attribute__ ((section (".fast")))
static wiced_bool_t platform_is_page_aligned_addr ( uint32_t address )
{
    if ( ( address & SECTOR_MASK ) != 0 )
    {
        return ( WICED_FALSE );
    }
    return ( WICED_TRUE );
}

__attribute__ ((section (".fast")))
static
cy_en_flashdrv_status_t platform_write_complete_page( uint32_t address, const unsigned char *data )
{
    cy_en_flashdrv_status_t status;

    platform_watchdog_kick( );

    status = Cy_Flash_WriteRow(address, ( uint32_t* ) data );

    return ( status );
}

__attribute__ ((section (".fast")))
static
cy_en_flashdrv_status_t platform_write_sector_aligned_sub_page( uint32_t address, const unsigned char* data, uint32_t size )
{
    unsigned char buff[SECTOR_SIZE];
    cy_en_flashdrv_status_t status = 0;
    uint32_t cp_size_remain = size;
    uint32_t cur_address = address;
    const unsigned char* cur_data = data;
    uint16_t sector;
    uint32_t i;

    sector = ( uint16_t ) ( ( cur_address - CY_FLASH_BASE ) / SECTOR_SIZE );
    platform_read_page( sector, ( uint32_t* ) buff );

    for ( i = 0; i < cp_size_remain; i++ )
    {
       buff[i] = cur_data[i];
    }

    platform_watchdog_kick( );

    status = Cy_Flash_WriteRow( cur_address, ( uint32_t* ) buff );

    return ( status );
}

__attribute__ ((section (".fast")))
static
cy_en_flashdrv_status_t platform_write_sector_non_aligned_sub_page( uint32_t address, const unsigned char *data, uint32_t size )
{
    unsigned char buff[SECTOR_SIZE];
    cy_en_flashdrv_status_t status = 0;
    uint32_t cur_address = address;
    const unsigned char* cur_data = data;
    uint16_t sector;
    uint32_t sector_offset_addr = cur_address & SECTOR_MASK;
    uint32_t sector_align_addr = cur_address & ( ~SECTOR_MASK );
    uint32_t write_size = size;
    uint32_t i;

    sector = ( uint16_t ) ( ( cur_address - CY_FLASH_BASE ) / SECTOR_SIZE );
    platform_read_page( sector, ( uint32_t* ) buff );

    for ( i = 0; i < write_size; i++ )
    {
       buff[sector_offset_addr + i] = cur_data[i];
    }

    platform_watchdog_kick( );

    status = Cy_Flash_WriteRow( sector_align_addr, ( uint32_t* ) buff );

    return ( status );
}

__attribute__ ((section (".fast")))
platform_result_t platform_write_flash_chunk( uint32_t address, const void* data, uint32_t size )
{
    cy_en_flashdrv_status_t status = 0;
    uint32_t cp_size_remain = size;
    uint32_t cur_address = address;
    const unsigned char* cur_data = data;

    while (cp_size_remain > 0)
    {
        if ( platform_is_page_aligned_addr( cur_address ) == WICED_TRUE )
        {
           if ( cp_size_remain >= SECTOR_SIZE )
           {
               status = platform_write_complete_page( cur_address, cur_data );

               if ( status != CY_FLASH_DRV_SUCCESS )
               {
                   return ( PLATFORM_ERROR );
               }
               cur_address += SECTOR_SIZE;
               cur_data += SECTOR_SIZE;
               cp_size_remain -= SECTOR_SIZE;
           }
           else
           {
               status = platform_write_sector_aligned_sub_page( cur_address, cur_data, cp_size_remain );

               if ( status != CY_FLASH_DRV_SUCCESS )
               {
                   return ( PLATFORM_ERROR );
               }
               cur_address += cp_size_remain;
               cur_data += cp_size_remain;
               cp_size_remain = 0;
          }
        }
        else
        {
            uint32_t write_size;
            write_size = MIN ( ( SECTOR_MASK - ( cur_address & SECTOR_MASK ) + 1 ), cp_size_remain );
            status = platform_write_sector_non_aligned_sub_page( cur_address, cur_data, write_size );

            if ( status != CY_FLASH_DRV_SUCCESS )
            {
                return ( PLATFORM_ERROR );
            }
            cur_address += write_size;
            cur_data += write_size;
            cp_size_remain -= write_size;
        }
    }

    return PLATFORM_SUCCESS;
}

void platform_erase_app_area( uint32_t physical_address, uint32_t size );
__attribute__ ((section (".fast")))
void platform_erase_app_area( uint32_t physical_address, uint32_t size )
{

    /* For Quick check assuming SECTOR aligned address and size passed * Will fix for non-aligned later */
    if (( physical_address >= INTERNAL_FLASH_START_ADDRESS ) && ( physical_address < INTERNAL_FLASH_END_ADDRESS ) )
    {
       uint32_t start_sector = ( physical_address - INTERNAL_FLASH_START_ADDRESS ) / SECTOR_SIZE;
       uint32_t end_sector = start_sector + size / SECTOR_SIZE;

       platform_erase_flash( (uint16_t) start_sector, (uint16_t) end_sector );
    } else if ( ( physical_address >= SFLASH_START_ADDRESS ) && ( physical_address < SFLASH_END_ADDRESS ) )
    {
       platform_sflash_erase( physical_address, size );
    }
    else
    {
    }
}

platform_result_t platform_is_load_permitted( void* physical_address, uint32_t size );
__attribute__ ((section (".fast")))
platform_result_t platform_is_load_permitted( void* physical_address, uint32_t size )
{
   if (((uint32_t) physical_address >= (uint32_t) 0x0800000) && (((uint32_t) physical_address + size) < (uint32_t) 0x08046000))
   {
      return PLATFORM_SUCCESS;
   }
      return ( PLATFORM_ERROR );
}

/* The function would copy data from serial flash to internal flash.
 * The function assumes that the program area is already erased (for now).
 * TODO: Adding erasing the required area
 */
static wiced_result_t platform_copy_app_to_iflash( const image_location_t* app_header_location, uint32_t offset, uint32_t physical_address, uint32_t size )
{
    /* Bootloader doesn't support BSS sections. */
    uint8_t buff[ 64 ];

    while ( size > 0 )
    {
        uint32_t write_size = MIN( sizeof(buff), size);
        wiced_apps_read( app_header_location, buff, offset, write_size );
        platform_write_flash_chunk( (uint32_t) physical_address, buff, write_size );
        if (memcmp((char *)physical_address, buff, write_size))
        {
            offset = 0;
            return WICED_ERROR;
        }
        offset           += write_size;
        physical_address += write_size;
        size             -= write_size;
    }
    return WICED_SUCCESS;
}

__attribute__ ((section (".fast")))
void platform_load_app_chunk( const image_location_t* app_header_location, uint32_t offset, void* physical_address, uint32_t size )
{
    if (( (uint32_t)physical_address >= INTERNAL_FLASH_START_ADDRESS ) && ( (uint32_t)physical_address < INTERNAL_FLASH_END_ADDRESS ) )
    {
        platform_copy_app_to_iflash( app_header_location, offset, (uint32_t) physical_address, size );
    }
    else
    {
        if ( PLATFORM_SUCCESS == platform_is_load_permitted( physical_address, size ) )
        {
            wiced_apps_read( app_header_location, physical_address, offset, size );
        }
    }
}
