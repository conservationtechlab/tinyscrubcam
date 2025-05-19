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

#include "spi_flash.h"
#include <string.h> /* for NULL */
#include <stdio.h> /* for printf */
#include "wwd_constants.h"
#include "platform_constants.h"
#include "platform_config.h"
#include "platform_peripheral.h"
#include "platform_mcu_peripheral.h"
#include "platform_isr.h"
#include "platform_isr_interface.h"
#include "wwd_rtos_isr.h"
#include "wiced_defaults.h"
#include "wiced_framework.h"

/* PSoC 6 kit specific External Memory configuration needed by PDL */
#include "cy_smif_memconfig.h"

#define SMIF_1_DESELECT_DELAY          ( 7u )      /* Minimum duration of SPI de-selection */
#define ADDRESS_SIZE                   ( 3u )      /* Memory address size */
#define PACKET_SIZE                    ( 256u )    /* The Memory Write packet */

/* PSoC 6 Pins used for SMIF Interface */
#define PSOC_SMIF_PORT                 ( 11u )     /* Port for Quad SPI Interface Pins */
#define PSOC_SMIF_SPI_SELECT0          ( 2u )      /* Pin2 - Select0 */
#define PSOC_SMIF_SPI_DATA3            ( 3u )      /* Pin3 - Data3 */
#define PSOC_SMIF_SPI_DATA2            ( 4u )      /* Pin4 - Data2 */
#define PSOC_SMIF_SPI_DATA1            ( 5u )      /* Pin5 - Data1 */
#define PSOC_SMIF_SPI_DATA0            ( 6u )      /* Pin6 - Data0 */
#define PSOC_SMIF_SPI_CLK              ( 7u )      /* Pin7 - Clock */

#define SECTOR_SIZE_16_MEG             ( 16 * 1024 * 1024 ) /* Max SPI Flash access with 3 byte address */

#define MAX_WRITE_SIZE                 ( 0x100 )   /* PDL Max Write size */

#define MAX_DISPLAY                    ( 0x20 )    /* Data display count */

// #define PRINT_DEBUG
#ifdef PRINT_DEBUG
#if !defined ( BOOTLOADER )
#define DEBUG_PRINTS(a) printf a
#else /* !defined ( BOOTLOADER ) */
#define DEBUG_PRINTS(a)
#endif /* !defined ( BOOTLOADER ) */
#else /* PRINT_DEBUG */
#define DEBUG_PRINTS(a)
#endif /* PRINT_DEBUG */

// #define PRINT_DEBUG_CRIT
#ifdef PRINT_DEBUG_CRIT
#if !defined ( BOOTLOADER )
#define DEBUG_PRINTS_CRIT(a) printf a
#else /* !defined ( BOOTLOADER ) */
#define DEBUG_PRINTS_CRIT(a)
#endif /* !defined ( BOOTLOADER ) */
#else /* PRINT_DEBUG_CRIT */
#define DEBUG_PRINTS_CRIT(a)
#endif /* PRINT_DEBUG_CRIT */



/* Static Global variables */
static cy_stc_smif_context_t smif_block_context;
static uint32_t erase_sector_size;
static uint32_t mask_address_in_sector;
static uint32_t backup_sector_address;
static uint32_t memory_size;

static
void platform_address_to_pdl_array( uint32_t address, uint8_t pdl_addr_arr[] )
{
    pdl_addr_arr[2] = (uint8_t) ( address & 0xff );
    pdl_addr_arr[1] = (uint8_t) ( ( address & 0xff00 ) >> 8 );
    pdl_addr_arr[0] = (uint8_t) ( ( address & 0xff0000 ) >> 16 );
}

static
void RxCmpltCallback ( uint32_t event )
{
    if( 0u == event )
    {
        /*The process event is 0*/
    }
}

static
void platform_print_read_data( uint8_t * rx_buffer, uint32_t size )
{
    uint32_t index;

    UNUSED_PARAMETER( rx_buffer );
    DEBUG_PRINTS( ( "platform_print_read_data: \r\n" ) );

    for( index=0; index<size; index++ )
    {
       DEBUG_PRINTS( ( "0x%02X ", (unsigned int) rx_buffer[index] ) );
       if ( index >= MAX_DISPLAY )
       {
           break;
       }
    }

    DEBUG_PRINTS( ( "\r\n" ) );
    DEBUG_PRINTS( ( "=======================\r\n" ) );
}

static
void platform_print_write_data( uint8_t * tx_buffer, uint32_t size )
{
    uint32_t index;

    UNUSED_PARAMETER( tx_buffer );

    DEBUG_PRINTS( ( "Written Data: \r\n" ) );

    for( index=0; index<size; index++ )
    {
       DEBUG_PRINTS( ( "0x%02X ", (unsigned int) tx_buffer[index] ) );
       if ( index >= MAX_DISPLAY )
       {
           break;
       }
    }
    DEBUG_PRINTS( ( "\r\n" ) );
}

static
void platform_read_memory( uint8_t rx_buffer[],
                           uint32_t rx_size,
                           uint32_t address,
                           uint32_t print_true )
{
    cy_en_smif_status_t smif_status;
    cy_stc_smif_mem_device_cfg_t *device = smifMemConfigs[0]->deviceCfg;
    uint8_t rx_buffer_reg;
    cy_stc_smif_mem_cmd_t *cmdreadStsRegQe = device->readStsRegQeCmd;
    SMIF_Type *baseaddr = SMIF0;
    cy_stc_smif_context_t *smif_context = &smif_block_context;
    uint8_t ext_mem_address[3];

    (void) platform_watchdog_kick();

    if ( print_true )
    {
        DEBUG_PRINTS( ( "Entered %s\r\n", __func__ ) );
        DEBUG_PRINTS( ( "\r\n%s: Address: 0x%04lX, Size: %lu\r\n\r\n", __func__, address, rx_size ) );
    }

    /* Set QE */
    smif_status = Cy_SMIF_Memslot_QuadEnable( baseaddr, (cy_stc_smif_mem_config_t*)smifMemConfigs[0], smif_context );
    if( smif_status != CY_SMIF_SUCCESS )
    {
          DEBUG_PRINTS( ( "Cy_SMIF_Memslot_QuadEnable() Failed!! status: 0x%04X\r\n", smif_status ) );
          return;
    }

     /* Added to avoid delay and confirm the completion of previous API */
     while( Cy_SMIF_Memslot_IsBusy( baseaddr, ( cy_stc_smif_mem_config_t* )smifMemConfigs[0], smif_context ) )
         ;

    /* Read data from the external memory configuration register */
    smif_status = Cy_SMIF_Memslot_CmdReadSts( baseaddr, smifMemConfigs[0], &rx_buffer_reg, (uint8_t)cmdreadStsRegQe->command , smif_context );
    if( smif_status!=CY_SMIF_SUCCESS )
    {
        DEBUG_PRINTS( ( "Cy_SMIF_Memslot_CmdReadSts() Failed!! status: 0x%04X\r\n", smif_status ) );
        return;
    }

    DEBUG_PRINTS( ( "Received Data: 0x%X\r\n", (unsigned int) rx_buffer_reg ) );

    /* Added to avoid delay and confirm the completion of previous API */
    while( Cy_SMIF_Memslot_IsBusy( baseaddr, (cy_stc_smif_mem_config_t*)smifMemConfigs[0], smif_context ) )
        ;

    (void) platform_watchdog_kick();

    platform_address_to_pdl_array( address, ext_mem_address );
    /* The 4 Page program command */
    smif_status = Cy_SMIF_Memslot_CmdRead( baseaddr, smifMemConfigs[0], ext_mem_address, rx_buffer, rx_size, &RxCmpltCallback, smif_context );

    if( smif_status!=CY_SMIF_SUCCESS )
    {
        DEBUG_PRINTS( ( "\r\n\r\nSMIF Cy_SMIF_Memslot_CmdRead failed\r\n" ) );
        return;
    }

    while( Cy_SMIF_BusyCheck( baseaddr ) )
        ;


    /* Send received data to the console */
    if ( print_true )
    {
       platform_print_read_data( rx_buffer, rx_size );
    }

    (void) platform_watchdog_kick();
}

static
void platform_write_memory( uint8_t tx_buffer[], uint32_t tx_size, uint32_t address )
{
    cy_en_smif_status_t smif_status;
    uint8_t rx_buffer_reg;
    cy_stc_smif_mem_device_cfg_t *device = smifMemConfigs[0]->deviceCfg;
    cy_stc_smif_mem_cmd_t *cmdreadStsRegQe = device->readStsRegQeCmd;
    cy_stc_smif_mem_cmd_t *cmdreadStsRegWip = device->readStsRegWipCmd;
    uint8_t ext_mem_address[3];
    uint32_t completed_check_cnt = 0;
    uint32_t cur_addr;
    uint32_t cur_write_size;
    uint32_t size = tx_size;
    cy_stc_smif_context_t *smif_context = &smif_block_context;
    SMIF_Type *baseaddr = SMIF0;

    DEBUG_PRINTS( ( "Entered %s\r\n", __func__ ) );
    DEBUG_PRINTS( ( "\r\n%s: Address: 0x%04lX, Size: %lu\r\n\r\n", __func__, address, tx_size ) );

    cur_addr = address;

    while ( completed_check_cnt < size )
    {

        (void) platform_watchdog_kick();

        /* Set QE */
        smif_status = Cy_SMIF_Memslot_QuadEnable( baseaddr, (cy_stc_smif_mem_config_t*)smifMemConfigs[0], smif_context );
        if(smif_status!=CY_SMIF_SUCCESS)
        {
            DEBUG_PRINTS( ( "\r\n\r\nSMIF Cy_SMIF_Memslot_QuadEnable failed\r\n" ) );
            return;
        }

        /* Added to avoid delay and confirm the completion of previous API */
        while( Cy_SMIF_Memslot_IsBusy( baseaddr, (cy_stc_smif_mem_config_t*)smifMemConfigs[0], smif_context ) );

        /* Read data from the external memory configuration register */
        smif_status = Cy_SMIF_Memslot_CmdReadSts( baseaddr, smifMemConfigs[0], &rx_buffer_reg, (uint8_t)cmdreadStsRegQe->command , smif_context );
        if( smif_status!=CY_SMIF_SUCCESS )
        {
            DEBUG_PRINTS( ( "\r\n\r\nSMIF Cy_SMIF_Memslot_CmdReadSts failed\r\n" ) );
            return;
        }

        /* Send Write Enable to external memory */
        smif_status = Cy_SMIF_Memslot_CmdWriteEnable( baseaddr, smifMemConfigs[0], smif_context );
        if( smif_status!=CY_SMIF_SUCCESS )
        {
            DEBUG_PRINTS( ( "\r\n\r\nSMIF Cy_SMIF_Memslot_CmdWriteEnable failed\r\n" ) );
            return;
        }

        platform_address_to_pdl_array( cur_addr, ext_mem_address );

        if ( ( size - completed_check_cnt ) > MAX_WRITE_SIZE )
        {
            cur_write_size = MAX_WRITE_SIZE;
        }
        else
        {
            cur_write_size = size - completed_check_cnt;
            if ( cur_write_size < MAX_WRITE_SIZE )
            {
                DEBUG_PRINTS( ( "Writing less than 256 Bytes!! cur_write_size: 0x%02lX\n", (uint32_t) cur_write_size ) );
            }
        }

        (void) platform_watchdog_kick();

        DEBUG_PRINTS( ( "\r\n%s, Address: 0x%08lX, Size: %lu, data_ptr: 0x%08lX\r\n\r\n", __func__, cur_addr, cur_write_size, (uint32_t) &tx_buffer[completed_check_cnt] ) );
        /* Quad Page Program command */
        smif_status = Cy_SMIF_Memslot_CmdProgram( baseaddr, smifMemConfigs[0], ext_mem_address, &tx_buffer[completed_check_cnt], cur_write_size, &RxCmpltCallback, smif_context );
        if( smif_status!=CY_SMIF_SUCCESS )
        {
            DEBUG_PRINTS( ( "\r\n\r\nSMIF Cy_SMIF_Memslot_CmdProgram failed\r\n" ) );
            return;
        }

        while( Cy_SMIF_Memslot_IsBusy(baseaddr, (cy_stc_smif_mem_config_t*)smifMemConfigs[0], smif_context ) )
        {
            /* Wait until the Erase operation is completed */
        }

        /* Read data from the external memory status register */
        smif_status = Cy_SMIF_Memslot_CmdReadSts( baseaddr, smifMemConfigs[0], &rx_buffer_reg,
                                            (uint8_t)cmdreadStsRegWip->command , smif_context );
        if( smif_status!=CY_SMIF_SUCCESS )
        {
            DEBUG_PRINTS( ( "\r\n\r\nSMIF ReadStatusReg failed\r\n" ) );
            return;
        }

        (void) platform_watchdog_kick();

        platform_print_write_data( &tx_buffer[completed_check_cnt], cur_write_size );

        completed_check_cnt += cur_write_size;
        cur_addr += cur_write_size;
    }
}




int sflash_read( const sflash_handle_t* const handle, unsigned long device_address, /*@out@*/ /*@dependent@*/ void* const data_addr, unsigned int size )
{
    UNUSED_PARAMETER( handle );

    DEBUG_PRINTS( ( "\r\n%s, Address: 0x%04lX, Size: %u\r\n\r\n", __func__, device_address, size ) );

    platform_read_memory( data_addr, size, device_address, 0 );

    return ( 0 );
}



int sflash_write( const sflash_handle_t* const handle, unsigned long device_address, /*@observer@*/ const void* const data_addr, unsigned int size )
{
    void *data = (void *) data_addr;

    UNUSED_PARAMETER( handle );

    DEBUG_PRINTS( ( "\r\n%s: Calling platform_print_read_data for display data to be written\r\n", __func__ ) );
    platform_print_read_data( (uint8_t *)data, (uint32_t)size );


    DEBUG_PRINTS( ( "\r\n%s: Address: 0x%04lX, Size: %u, data_ptr: 0x%08lX\r\n\r\n", __func__, device_address, size, (uint32_t) data ) );
    platform_write_memory( data, size, device_address );

    return ( 0 );
}

static
int platform_check_for_non_ff ( uint8_t *loc, uint32_t size )
{
    uint32_t i;

    for ( i = 0; i < size; i++ )
    {
        if ( loc[i] != 0xff )
        {
            return ( 1 );
        }
    }
    return ( 0 );
}

static
int sflash_need_to_erase ( unsigned long device_address, unsigned int size )
{
    uint8_t read_mem_buf[PACKET_SIZE];
    uint32_t completed_check_cnt = 0;
    uint32_t cur_addr = device_address;
    uint32_t cur_read_size;
    while ( completed_check_cnt < size )
    {

        if ( ( size - completed_check_cnt ) > PACKET_SIZE )
        {
            cur_read_size = PACKET_SIZE;
        }
        else
        {
            cur_read_size = size - completed_check_cnt;
        }

        platform_read_memory( read_mem_buf, cur_read_size, cur_addr, 0 );

        if ( platform_check_for_non_ff ( read_mem_buf, cur_read_size ) )
        {
             DEBUG_PRINTS(("\r\nNEED TO ERASE!!!!! %s: Address: 0x%04lX, Size: %u\r\n\r\n", __func__, device_address, size));
             return ( 1 );
        }
        completed_check_cnt += cur_read_size;
        cur_addr += cur_read_size;
    }

    return ( 0 );
}

static
int platform_get_backup_sector( uint32_t* backup_sector_start, uint32_t* backup_sector_size )
{
   *backup_sector_start = backup_sector_address;
   *backup_sector_size = erase_sector_size;

   return ( 0 );
}

static
int platform_copy_sflash_src_to_dest ( unsigned long src, unsigned long dest, unsigned int size )
{
    uint8_t read_mem_buf[PACKET_SIZE];
    uint32_t completed_check_cnt = 0;
    uint32_t cur_src_addr = src;
    uint32_t cur_dst_addr = dest;
    uint32_t cur_read_size;
    while ( completed_check_cnt < size )
    {
        if ( ( size - completed_check_cnt ) > PACKET_SIZE )
        {
            cur_read_size = PACKET_SIZE;
        }
        else
        {
            cur_read_size = size - completed_check_cnt;
        }

        platform_read_memory( read_mem_buf, cur_read_size, cur_src_addr, 0 );

        if ( platform_check_for_non_ff ( read_mem_buf, cur_read_size ) )
        {
             platform_write_memory( read_mem_buf, cur_read_size, cur_dst_addr );
        }
        else
        {
             DEBUG_PRINTS_CRIT( ( "%s: No need to copy from: 0x%04lX to: 0x%04lX\r\n", __func__, cur_src_addr, cur_dst_addr ) );
        }
        completed_check_cnt += cur_read_size;
        cur_src_addr += cur_read_size;
        cur_dst_addr += cur_read_size;
    }

    return ( 0 );
}

int platform_sflash_erase( unsigned long device_address, unsigned int size )
{
    cy_en_smif_status_t status;
    uint8_t ext_mem_address[3] = { 0x0, 0x0, 0x0};

    platform_address_to_pdl_array( device_address, ext_mem_address );

    /* Erase */

    UNUSED_PARAMETER( size );

    status = Cy_SMIF_Memslot_CmdWriteEnable( SMIF0, smifMemConfigs[0], &smif_block_context );

    if( status!=CY_SMIF_SUCCESS )
    {
        DEBUG_PRINTS( ( "\r\n\r\nSMIF Cy_SMIF_Memslot_CmdWriteEnable failed\r\n" ) );
        return ( status );
    }

    status = Cy_SMIF_Memslot_CmdSectorErase( SMIF0, (cy_stc_smif_mem_config_t*)smifMemConfigs[0], ext_mem_address,&smif_block_context );

    if( status!=CY_SMIF_SUCCESS )
    {
        DEBUG_PRINTS( ( "\r\n\r\nSMIF Cy_SMIF_Memslot_CmdSectorErase failed: 0x%04X\r\n", status ) );
        return ( status );
    }


    /* Wait until the memory is erased */
    while( Cy_SMIF_Memslot_IsBusy( SMIF0, (cy_stc_smif_mem_config_t*)smifMemConfigs[0], &smif_block_context ) )
    {
       /* Wait until the Erase operation is completed */
    }
    return ( 0 );
}

static
int sflash_erase( const sflash_handle_t* const handle, unsigned long device_address, unsigned int size )
{
    cy_en_smif_status_t status = CY_SMIF_SUCCESS;
    uint32_t backup_sector_start, backup_sector_size, offset_in_sector;
    uint32_t save_required_flash_content = 0;
    uint32_t save_first_start_address, save_first_end_address, save_second_start_address, save_second_end_address;
    uint32_t save_first_copy_size, save_second_copy_size;

    UNUSED_PARAMETER( handle );

    DEBUG_PRINTS( ( "\r\n%s: Address: 0x%04lX, Size: %u\r\n\r\n", __func__, device_address, size ) );

    if ( sflash_need_to_erase ( device_address, size ) == 0 )
    {
         DEBUG_PRINTS_CRIT( ( "\r\n%s: Address: 0x%04lX, Size: %u, No Need to Earse!!!!!\r\n\r\n", __func__, device_address, size ) );
         return ( 0 );
    }

    DEBUG_PRINTS( ( "\r\n%s: Address: 0x%04lX, Size: %u\r\n\r\n", __func__, device_address, size ) );

    if ( ( device_address % erase_sector_size == 0 ) && ( size == erase_sector_size ) )
    {
        DEBUG_PRINTS_CRIT( ( "\r\n%s: Address: 0x%04lX, Size: %u, Only Erase - No need to save/restore other parts of flash!!!!!\r\n\r\n", __func__, device_address, size) );
    } else {
        DEBUG_PRINTS_CRIT( ( " Sector Erase With Store & Restore Needed!!\r\n" ) );
        save_required_flash_content = 1;
        save_first_start_address = ( device_address / erase_sector_size ) * erase_sector_size;
        save_first_end_address = device_address;
        save_first_copy_size = save_first_end_address - save_first_start_address;
        save_second_start_address = device_address + size;
        save_second_end_address = ( ( device_address / erase_sector_size ) + 1 ) * erase_sector_size;
        save_second_copy_size = save_second_end_address - save_second_start_address;
        DEBUG_PRINTS_CRIT( ( "\r\n%s: Address: 0x%08lX, Size: %u, Needs save/restore other parts of flash!!!!!\r\n\r\n", __func__, device_address, size ) );
        DEBUG_PRINTS_CRIT( ( "\r\n%s: Save first start Address: 0x%08lX, Save first Address End: 0x%08lX\r\n", __func__, save_first_start_address, save_first_end_address ) );
        DEBUG_PRINTS_CRIT( ( "\r\n%s: Save second start Address: 0x%08lX, Save second Address End: 0x%08lX\r\n", __func__, save_second_start_address, save_second_end_address ) );
    }

    if ( save_required_flash_content )
    {
        DEBUG_PRINTS_CRIT( ( " Sector Erase With Store & Restore Started!!\r\n" ) );

        platform_get_backup_sector( &backup_sector_start, &backup_sector_size );

        DEBUG_PRINTS_CRIT( ( " Erasing Backup Sector start: 0x%08lX,  size: 0x%08lX!!\r\n", backup_sector_start, backup_sector_size ) );

        status = platform_sflash_erase( backup_sector_start, backup_sector_size );

        DEBUG_PRINTS_CRIT( ( "\r\n%s: Backup Sector Start: 0x%08lX, Backup Sector Size: 0x%08lX\r\n", __func__, backup_sector_start, backup_sector_size ) );

        DEBUG_PRINTS_CRIT( ( "Saving Sector contents to Backup before Erase!!\r\n" ) );

        DEBUG_PRINTS_CRIT( ( "\r\n%s: First Part!! Copy from: 0x%08lX, Copy To: 0x%08lX, Size: 0x%08lX\r\n", __func__,
                        save_first_start_address, backup_sector_start, save_first_copy_size ) );

        platform_copy_sflash_src_to_dest( save_first_start_address, backup_sector_start, save_first_copy_size );

        offset_in_sector = ( save_second_start_address & mask_address_in_sector );

        DEBUG_PRINTS_CRIT( ( "\r\n%s: Second Part!! Copy from: 0x%08lX, Copy To: 0x%08lX, Size: 0x%08lX\r\n", __func__,
                     save_second_start_address, ( backup_sector_start + offset_in_sector ), save_second_copy_size ) );

        platform_copy_sflash_src_to_dest( save_second_start_address, ( backup_sector_start + offset_in_sector ),
                     save_second_copy_size );
    }

    status = platform_sflash_erase( device_address, size );

    if ( save_required_flash_content )
    {
        DEBUG_PRINTS_CRIT( ( "Restoring Sector contents from Backup after Erase!!\r\n" ) );

        DEBUG_PRINTS_CRIT( ( "\r\n%s: First Part!! Copy from: 0x%08lX, Copy To: 0x%08lX, Size: 0x%08lX\r\n", __func__,
                      backup_sector_start, save_first_start_address, save_first_copy_size ) );

        platform_copy_sflash_src_to_dest( backup_sector_start, save_first_start_address, save_first_copy_size );

        DEBUG_PRINTS_CRIT( ( "\r\n%s: Second Part!! Copy from: 0x%08lX, Copy To: 0x%08lX, Size: 0x%08lX\r\n", __func__,
                     ( backup_sector_start + offset_in_sector ), save_second_start_address, save_second_copy_size ) );

        platform_copy_sflash_src_to_dest ( ( backup_sector_start + offset_in_sector ), save_second_start_address, save_second_copy_size );
        DEBUG_PRINTS_CRIT( ( " Sector Erase With Store & Restore Completed!!\r\n" ) );
    }

    return ( status );
}


int deinit_sflash( /*@out@*/ sflash_handle_t* const handle)
{
    UNUSED_PARAMETER( handle );
    return 0;
}

int init_sflash( /*@out@*/ sflash_handle_t* const handle, /*@shared@*/ void* peripheral_id, sflash_write_allowed_t write_allowed_in )
{
    cy_stc_smif_config_t smif;
    cy_en_smif_status_t status;
    GPIO_PRT_Type* port_base;

    UNUSED_PARAMETER( handle );
    UNUSED_PARAMETER( peripheral_id );
    UNUSED_PARAMETER( write_allowed_in );


    DEBUG_PRINTS( ( "Entered %s\r\n", __func__ ) );

    /* Port11 configuration */

    port_base = Cy_GPIO_PortToAddr( PSOC_SMIF_PORT );

    /* Configure all the SMIF Pins */
    Cy_GPIO_Pin_FastInit( port_base, PSOC_SMIF_SPI_SELECT0, CY_GPIO_DM_STRONG_IN_OFF, 0x1, P11_2_SMIF_SPI_SELECT0 );
    Cy_GPIO_Pin_FastInit( port_base, PSOC_SMIF_SPI_DATA3,   CY_GPIO_DM_STRONG,        0x1, P11_3_SMIF_SPI_DATA3 );
    Cy_GPIO_Pin_FastInit( port_base, PSOC_SMIF_SPI_DATA2,   CY_GPIO_DM_STRONG,        0x1, P11_4_SMIF_SPI_DATA2 );
    Cy_GPIO_Pin_FastInit( port_base, PSOC_SMIF_SPI_DATA1,   CY_GPIO_DM_STRONG,        0x1, P11_5_SMIF_SPI_DATA1 );
    Cy_GPIO_Pin_FastInit( port_base, PSOC_SMIF_SPI_DATA0,   CY_GPIO_DM_STRONG,        0x1, P11_6_SMIF_SPI_DATA0 );
    Cy_GPIO_Pin_FastInit( port_base, PSOC_SMIF_SPI_CLK,     CY_GPIO_DM_STRONG_IN_OFF, 0x1, P11_7_SMIF_SPI_CLK );

    smif.mode = CY_SMIF_NORMAL;
    smif.deselectDelay = SMIF_1_DESELECT_DELAY;     /* Just for testing */
    smif.rxClockSel = (uint32_t)CY_SMIF_SEL_INV_INTERNAL_CLK;     /* Just for testing */
    smif.blockEvent = (uint32_t)CY_SMIF_BUS_ERROR;

    status = Cy_SMIF_Init ( SMIF0, &smif, 1000, &smif_block_context );

    if ( status != CY_SMIF_SUCCESS )
    {
        DEBUG_PRINTS( ( "Cy_SMIF_Init() Failed!!, status: %d\n", status ) );
        return ( status );
    }

    Cy_SMIF_SetDataSelect ( SMIF0, CY_SMIF_SLAVE_SELECT_0, CY_SMIF_DATA_SEL0 );

    Cy_SMIF_Enable ( SMIF0, &smif_block_context );

    NVIC_EnableIRQ( smif_interrupt_IRQn );

    memory_size = smifMemConfigs[0]->deviceCfg->memSize;
    erase_sector_size= smifMemConfigs[0]->deviceCfg->eraseSize;
    mask_address_in_sector = erase_sector_size - 1;
    /* Hueristic logic below */
    if ( memory_size > SECTOR_SIZE_16_MEG )
    {
        memory_size = SECTOR_SIZE_16_MEG;
    }

    if ( ( memory_size / erase_sector_size ) > 16 )
    {
       backup_sector_address = ( ( ( memory_size / erase_sector_size )  - 4 ) * erase_sector_size );
    } else
    {
       backup_sector_address = ( ( (memory_size / erase_sector_size )  - 1 ) * erase_sector_size );
    }

    DEBUG_PRINTS( ( "Leaving %s\r\n", __func__ ) );

    return 0;
}

int sflash_get_size ( const sflash_handle_t* handle, unsigned long* size )
{
    UNUSED_PARAMETER( handle );

    *size = memory_size;
    return ( 0 );
}

int sflash_chip_erase ( const sflash_handle_t* handle )
{
    cy_en_smif_status_t status = CY_SMIF_SUCCESS;

    UNUSED_PARAMETER( handle );
    status = Cy_SMIF_Memslot_CmdWriteEnable( SMIF0, smifMemConfigs[0], &smif_block_context );

    if( status!=CY_SMIF_SUCCESS )
    {
        DEBUG_PRINTS( ( "\r\n\r\nSMIF Cy_SMIF_Memslot_CmdWriteEnable failed\r\n" ) );
        return ( status );
    }

    status = Cy_SMIF_Memslot_CmdChipErase( SMIF0, (cy_stc_smif_mem_config_t*)smifMemConfigs[0],
                                          &smif_block_context );

    if( status!=CY_SMIF_SUCCESS )
    {
        DEBUG_PRINTS( ( "\r\n\r\nSMIF Cy_SMIF_Memslot_CmdChipErase() failed: 0x%04X\r\n", status ) );
        return ( status );
    }

    /* Wait until the memory is erased */
    while( Cy_SMIF_Memslot_IsBusy( SMIF0, (cy_stc_smif_mem_config_t*)smifMemConfigs[0], &smif_block_context ) )
    {
       /* Wait until the Erase operation is completed */
    }
    return ( status );
}

int sflash_sector_erase ( const sflash_handle_t* handle, unsigned long device_address )
{
    int result;

    UNUSED_PARAMETER( handle );
    result =  sflash_erase( handle, device_address, 4096 );      /* Stubbing something for compilation and testing */
    return result;
}

void platform_smif_irq( void );
void platform_smif_irq( void )
{
    Cy_SMIF_Interrupt( SMIF0, &smif_block_context );

}

/*@noaccess sflash_handle_t@*/
int sflash_sector_support_erase_range ( void );
int sflash_sector_support_erase_range ( void )
{
    return (WICED_TRUE);
}
#define ALIGN_RANGE_ADDRESS            ( 4096 ) /* Have write/restore address aligned to 4096 */
#define ALIGN_RANGE_MASK              (~( ALIGN_RANGE_ADDRESS - 1 ))  /* Mask to have write/restore address aligned to 4096 */

int sflash_sector_erase_range ( const sflash_handle_t* handle, unsigned long device_address, unsigned long device_address_end );
int sflash_sector_erase_range ( const sflash_handle_t* handle, unsigned long device_address, unsigned long device_address_end )
{
    int result = 0;
    int still_to_do = 1;
    uint32_t start_address_sector_num, end_address_sector_num;
    uint32_t cur_start_addr, cur_end_addr;

    UNUSED_PARAMETER( handle );
    if ( !( ( device_address_end % ALIGN_RANGE_ADDRESS ) == 0 ) )
    {
       DEBUG_PRINTS( ("Seeing End Address Not Aligned to %d!! device_address: 0x%08lX, device_address_end: 0x%08lX\n", ALIGN_RANGE_ADDRESS, device_address, device_address_end ) );
       device_address_end += ALIGN_RANGE_ADDRESS;
       device_address_end = (device_address_end & (uint32_t)(ALIGN_RANGE_MASK));
       DEBUG_PRINTS( ("After Aligning End Address to %d!! device_address: 0x%08lX, device_address_end: 0x%08lX\n", ALIGN_RANGE_ADDRESS, device_address, device_address_end ) );
    }

    start_address_sector_num = device_address / erase_sector_size;
    end_address_sector_num = device_address_end / erase_sector_size;
    if ( start_address_sector_num == end_address_sector_num )  /* Same Sector */
    {
       result =  sflash_erase( handle, device_address, ( device_address_end - device_address ) );      /* Stubbing something for compilation and testing */
       return result;
    }
    else   /* Crossing Sector boundary - so handle each sector independently */
    {
        DEBUG_PRINTS( ("Crossing Sector boundary!! device_address: 0x%08lX, device_address_end: 0x%08lX\n", device_address, device_address_end ) );
        cur_start_addr = device_address;
        cur_end_addr = (start_address_sector_num * erase_sector_size) + erase_sector_size - 1;
        cur_end_addr = (start_address_sector_num * erase_sector_size) + erase_sector_size;

        while ( still_to_do )
        {
        DEBUG_PRINTS( ("Crossing Sector boundary!! Erasing!! cur_start_addr: 0x%08lX, cur_end_addr: 0x%08lX\n", cur_start_addr, cur_end_addr ) );
            result =  sflash_erase( handle, cur_start_addr, ( cur_end_addr - cur_start_addr ) );      /* Stubbing something for compilation and testing */
            if ( result != 0 )
            {
                return ( result );
            }

            start_address_sector_num++;

            if ( start_address_sector_num > end_address_sector_num )
            {
                return ( result );
            }

            cur_start_addr = cur_end_addr;

            if ( end_address_sector_num == start_address_sector_num )
            {
               cur_end_addr = device_address_end;
            }
            else
            {
               cur_end_addr = (start_address_sector_num * erase_sector_size);
            }
        }
    }
    return ( result );
}
