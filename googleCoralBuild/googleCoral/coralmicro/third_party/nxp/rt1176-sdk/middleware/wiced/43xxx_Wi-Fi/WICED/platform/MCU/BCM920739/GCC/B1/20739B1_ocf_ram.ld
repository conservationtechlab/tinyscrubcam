/* Linker script for ARM Simulator
 *
 * Version: Sourcery G++ Lite 2011.03-42
 * Support: https://support.codesourcery.com/GNUToolchain/
 *
 * Copyright (c) 2007, 2008, 2009, 2010 CodeSourcery, Inc.
 *
 * The authors hereby grant permission to use, copy, modify, distribute,
 * and license this software and its documentation for any purpose, provided
 * that existing copyright notices are retained in all copies and that this
 * notice is included verbatim in any distributions.  No written agreement,
 * license, or royalty fee is required for any of the authorized uses.
 * Modifications to this software may be copyrighted by their authors
 * and need not follow the licensing terms described here, provided that
 * the new terms are clearly indicated on the first page of each file where
 * they apply.
 */
/*
 * Linker script for SPAR in RAM application. Modified from generic.ld
 */
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
OUTPUT_FORMAT ("elf32-littlearm", "elf32-bigarm", "elf32-littlearm")
SEARCH_DIR(.)
ENTRY( __entry_func__ );
/* GROUP(-lgcc -lc) */
__STACKSIZE__ = 3024;
MEMORY
{
  ram (rwx) : ORIGIN = IRAM_BEGIN, LENGTH = IRAM_LEN
  aon (rwx) : ORIGIN = IAON_BEGIN, LENGTH = IAON_LEN
  pram (rwx) : ORIGIN = PRAM_BEGIN + PRAM_LEN, LENGTH = PRAM_FREE_LEN
  flash (rx) : ORIGIN = LINKER_SFLASH_XIP_ADDR, LENGTH = LINKER_SFLASH_XIP_LENGTH
  /* LINKER_SFLASH_XIP_ADDR refers to a location in OCF here */
}

/*
 * Extern the linker generated symbols
 */
EXTERN(spar_irom_begin spar_irom_end spar_irom_length);
EXTERN(spar_iram_begin spar_iram_end spar_iram_length);
EXTERN(spar_iram_data_begin spar_iram_data_end spar_iram_data_length);
EXTERN(spar_iram_bss_begin spar_iram_bss_end spar_iram_bss_length);
EXTERN(spar_irom_data_begin);
EXTERN(aon_iram_end);
EXTERN(spar_sram_begin);
EXTERN(aon_iram_begin);

/*
 * Provide some symbols for spar init code to use
 */
PROVIDE(spar_irom_length = spar_irom_end - spar_irom_begin);
PROVIDE(spar_iram_length = spar_iram_end - spar_iram_begin);
PROVIDE(spar_iram_data_length = spar_iram_data_end - spar_iram_data_begin);
PROVIDE(spar_iram_bss_length = spar_iram_bss_end - spar_iram_bss_begin);

SECTIONS
{
  spar_irom_begin = IRAM_BEGIN;

 /* #include "spar_ram_overlays.ld"*/

#ifdef DIRECT_LOAD
  /*  When direct loading, don't overlap init code with dynamic allocation.
  */
  .setup : ALIGN (4)
  {
    CREATE_OBJECT_SYMBOLS
    *(.init_code)
    *(.init_text)
    *(.spar_setup)
  } >ram
#endif
  .pram_text :
  {
    *(.text.spi_irq*)
    *(.text.host_rtos*)
    *(.text.wwd_thread*)
    *(.text.wwd_management*)
  } > pram

  .pram_rodata :
  {
      *(.rodata.wwd*)
  } > pram

  .text :
  {
    CREATE_OBJECT_SYMBOLS
    spar_sram_begin = .;
    *(.text.mpi*)
  } >ram
  .flash : ALIGN (4)
  {
    CREATE_OBJECT_SYMBOLS
    flash_begin = .;
    *(.text.*)
    *(.init_code)
    *(.init_text)
    *(.spar_setup)
    *(.app_init_code)
    *(.sleep_event_handlers.*)
    *(.emb_text)
    *(.text.* .gnu.linkonce.t.*)
    *(.glue_7t) *(.glue_7) *(.vfp11_veneer) *(.v4_bx)
    *(.rodata.*)
    flash_end = .;
  } >flash

  /*
   * The .ARM.exidx and .ARM.extab sections are used for C++ exception handling.
   * It is located here for completeness. Bare-metal ARM projects
   * typically cannot afford the overhead associated with C++
   * exceptions handling.
   */
  .ARM.exidx :
  {
    __exidx_start = ALIGN(4);
    *(.ARM.exidx* .gnu.linkonce.armexidx.*)
    __exidx_end = .;
  } >flash

  .ARM.extab :
  {
    __extab_start = ALIGN(4);
    *(.ARM.extab*)
    __extab_end = .;
  } >flash

  .rodata : ALIGN (4)
  {
    CREATE_OBJECT_SYMBOLS
    *(const_drom_var)
    *(.rodata)
    *(.constdata)
    *(.rodata.* .gnu.linkonce.r.*)
    *(.data.rom)
    . = ALIGN(4);
    link_sleep_event_registrations_location = .;
    KEEP(*(.sleep_event_registrations.*))
    link_sleep_event_registrations_end = .;
    spar_irom_end = .;
  } >ram

  .data : ALIGN (4)
  {
    CREATE_OBJECT_SYMBOLS
    spar_iram_begin = .;
    spar_iram_data_begin = .;
    *(.data)
    *(.data.* .gnu.linkonce.d.*)
    spar_iram_data_end = .;
  } >ram

  /* Nothing to load because it is loaded from EEPROM/SF at boot. */
  spar_irom_data_begin = spar_iram_data_begin;

  .bss : ALIGN (4)
  {
    CREATE_OBJECT_SYMBOLS
    spar_iram_bss_begin = .;
    *(.bss)
    *(.bss.*  .gnu.linkonce.b.*)
    *(COMMON)
    spar_iram_bss_end = .;
    spar_iram_end = .;
  } >ram

 #ifndef DIRECT_LOAD
  /*  Place the setup area after bss so that when dynamic allocation occurs
   *  after spar setup, it will reclaim the RAM taken up by the setup function.
  */
  .setup : ALIGN (4)
  {
    CREATE_OBJECT_SYMBOLS
  } >ram
#endif

  .aon : ALIGN (4)
  {
    CREATE_OBJECT_SYMBOLS
    aon_iram_begin = .;
    *(always_on_ram_var)
    *(.data_in_retention_ram)
    *(.code_in_retention_ram)
    aon_iram_end = .;
  } >aon

}

/* Declare libc Heap to start at end of allocated RAM */

/*PROVIDE( _heap = spar_iram_end );*/

/* End of the heap is top of RAM, aligned 8 byte */

/*PROVIDE( _eheap = ALIGN( IRAM_BEGIN + (IRAM_LEN-0x100) - 8, 8 ) );*/

