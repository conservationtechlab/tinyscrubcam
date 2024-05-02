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
ENTRY(Sample_App_Entry)
SEARCH_DIR(.)

RAM_size = 1M;
RAM_base = 0x800000; /*ORIGIN(RAM);*/

ROMCODE_RAM_USE_size         = 64K;
ROMCODE_RAM_USE_base         = (RAM_base + ROMCODE_RAM_USE_size);

APP_TOTAL_MEMORY            = 768K - ( ROMCODE_RAM_USE_size );
APP_RO_VIRTUAL_size         = 50K;
APP_RO_VIRTUAL_base         = ROMCODE_RAM_USE_base;
APP_RW_VIRTUAL_size         = APP_TOTAL_MEMORY - APP_RO_VIRTUAL_size;
APP_RW_VIRTUAL_base         = APP_RO_VIRTUAL_base + APP_RO_VIRTUAL_size;

intvec_start     = APP_RO_VIRTUAL_base;    /* interrupt vectors    */
size_cstack     = 4K;
size_heap       = 160K;

SECTIONS
{
  . = intvec_start;
  /* The startup code goes first into FLASH */
  .isr_vector :
  {
    . = intvec_start;
    KEEP(*(.isr_vector)) /* Startup code */
    . = ALIGN(4);
  }

  . = intvec_start + 1K;
  /* The program code and other data goes into LOADABLE_RAM_BASE */
  .text :
  {
    . = ALIGN(4);
    *(.text)           /* .text sections (code) */
    *(.text*)          /* .text* sections (code) */
    *(.rodata)         /* .rodata sections (constants, strings, etc.) */
    *(.rodata*)        /* .rodata* sections (constants, strings, etc.) */
    *(.glue_7)         /* glue arm to thumb code */
    *(.glue_7t)        /* glue thumb to arm code */
  }

  /* Initialized data sections goes into RAM */
  . = APP_RW_VIRTUAL_base;
  .data :
  {
    *(.data)           /* .data sections */
    *(.data*)          /* .data* sections */

  }

  /* Uninitialized data section */
  .bss :
  {
    /*  Used by the startup in order to initialize the .bss secion */
    *(.bss)
    *(.bss*)
    *(COMMON)
  }

  .heap :
    {
        _heap_start = .;
        . = . + size_heap;
                _heap_end = .;
    }
}