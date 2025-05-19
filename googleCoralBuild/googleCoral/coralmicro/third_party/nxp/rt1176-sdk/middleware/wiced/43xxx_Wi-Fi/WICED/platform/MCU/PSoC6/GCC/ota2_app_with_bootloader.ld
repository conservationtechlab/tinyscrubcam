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

reset_handler = _start;

ENTRY( _start );

/* Include memory map */
INCLUDE ota2_memory_with_bootloader.ld

PHDRS
{
    text PT_LOAD;
    data PT_LOAD;
    bss  PT_LOAD;
    rodata PT_LOAD;
}

SECTIONS
{

    .vectors :
    {
        link_interrupt_vectors_location = .;
        KEEP(*(*.interrupt_vector_table))
    }>APP_CODE  AT>APP_CODE :text

    .text :
    {
        . = ALIGN(4);

        link_code_location = .;
        KEEP(*(.text.irq ))
        *(.text .text.* .gnu.linkonce.t.*)
        link_code_end = .;

        . = ALIGN(0x4);

        link_const_variable_data_location = .;
        wifi_firmware_image_location = .;
        *(.rodata.wifi_firmware_image)
        wifi_firmware_image_end = .;
        . = ALIGN(0x4);
        *(.rodata.wifi_nvram_image)
        *(.rodata .rodata.* .gnu.linkonce.r.*)
        link_const_variable_data_end = .;

        . = ALIGN(0x4);

        link_constructors_location = .;
        KEEP(*(.preinit_array))
        KEEP(*(.init_array))
        KEEP (*crtbegin.o(.ctors))
        KEEP (*(EXCLUDE_FILE (*crtend.o) .ctors))
        KEEP (*(SORT(.ctors.*)))
        KEEP (*crtend.o(.ctors))
        link_constructors_end = .;

        . = ALIGN(0x4);

        link_destructors_location = .;
        KEEP (*crtbegin.o(.dtors))
        KEEP (*(EXCLUDE_FILE (*crtend.o) .dtors))
        KEEP (*(SORT(.dtors.*)))
        KEEP (*crtend.o(.dtors))
        link_destructors_end = .;

        . = ALIGN(16);

    }>APP_CODE  AT>APP_CODE :text

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
    } > APP_CODE  AT>APP_CODE :text

    .ARM.extab :
    {
        __extab_start = ALIGN(4);
        *(.ARM.extab*)
        __extab_end = .;
    } > APP_CODE  AT>APP_CODE :text

    .fast : /* This section contains code that is run from RAM after being loaded from flash - functions can be put in this section with the C attribute: __attribute__ ((section (".fast"))) */
    {
        link_run_from_ram_code_flash_location = LOADADDR( .fast ); /* This is the location in flash of the code */
        link_run_from_ram_code_ram_location = .;
        *(.fast .fast.* .text.fastcode .cy_ramfunc __ramfunc)
        link_run_from_ram_code_ram_end = .;
    }> SRAM AT>APP_CODE :data

    .data : /* Contains the non-zero initialised global variables */
    {
        link_global_data_initial_values = LOADADDR( .data ); /* This is the location in flash of the initial values of global variables */
        link_global_data_start = .;
        *(.data*)
        link_global_data_end = .;
        . = ALIGN(., 4);
    }> SRAM AT>APP_CODE :data


    .bss : /* Zero initialised memory used for zero initialised variables */
    {
        link_bss_location = ALIGN(., 4);
        *(.bss*)
        *(COMMON)
        link_bss_end = .;
        . = ALIGN(., 4);
    }> SRAM AT>SRAM :bss


    .stack : /* Contains the initial stack */
    {
        link_stack_location = ALIGN(., 4);
        *(.stack)
        . = ALIGN(MAX(link_stack_location + __STACKSIZE__ , .), 4);
        link_stack_end = .;
    }> SRAM AT>SRAM :bss

    .rodata :
    {
        /* contains the DCTs in external FLASH */
        dct_location = .;
        KEEP(*(.rodata.initial_dct))
        dct_end = .;

    } >DCT1_FLASH  AT> DCT1_FLASH :rodata

    /DISCARD/ :
    {
        *(.ARM.attributes*)
        *(.comment)
        *(.init)
        *(.preinit)
        *(.fini)
        *(.fini_array)
        *(.ARM.exidx*)
        *(.gnu.linkonce.armexidx.*)
        *(.eh_frame_hdr)
        *(.eh_frame)
        *(.gnu.linkonce.armextab.*)
        *(.v4_bx)
        *(.vfp11_veneer)
        *(.gcc_except_table)
        *(.eh_frame_hdr)
        *(.eh_frame)
        *(.glue*)
    }

}

/* Declare libc Heap to start at end of allocated RAM */

PROVIDE( _heap = link_stack_end );

/* End of the heap is top of RAM, aligned 8 byte */

PROVIDE( _eheap = ALIGN( ORIGIN( SRAM ) + LENGTH( SRAM ) - 8, 8 ) );
PROVIDE( total_app_image_size = LOADADDR( .data ) - ORIGIN( APP_CODE ) + link_global_data_end - link_global_data_start );
PROVIDE( wifi_firmware_image_size_from_link = wifi_firmware_image_end - wifi_firmware_image_location );
/* PROVIDE( vectors_offset = link_interrupt_vectors_location - bootloader_app_header_location ); */
PROVIDE( shared_api_addr_loc     = ORIGIN( SHARED_API ));

PROVIDE( app_code_start_addr_loc = ORIGIN( APP_CODE ));
PROVIDE( sram_start_addr_loc     = ORIGIN( SRAM ));
PROVIDE( sram_size_loc           = LENGTH( SRAM ));
PROVIDE( dct1_start_addr_loc     = ORIGIN( DCT1_FLASH ));
PROVIDE( dct1_size_loc           = LENGTH( DCT1_FLASH ));
PROVIDE( dct2_start_addr_loc     = ORIGIN( DCT2_FLASH ));
PROVIDE( dct2_size_loc           = LENGTH( DCT2_FLASH ));
PROVIDE( dct_save_start_addr_loc = ORIGIN( SAVE_FLASH ));
PROVIDE( dct_save_size_loc       = LENGTH( SAVE_FLASH ));
/* ThreadX aliases */
PROVIDE( __RAM_segment_used_end__ = link_stack_end );
PROVIDE( __tx_free_memory_start = link_stack_end );
PROVIDE( __tx_vectors = link_interrupt_vectors_location );

PROVIDE( __Vectors = link_interrupt_vectors_location );
PROVIDE( __ramVectors = ORIGIN( SRAM ));





