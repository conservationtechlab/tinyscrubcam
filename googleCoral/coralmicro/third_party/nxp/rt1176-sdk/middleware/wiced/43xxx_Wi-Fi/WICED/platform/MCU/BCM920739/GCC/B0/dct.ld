MEMORY
{
    DCT1_FLASH                 (rx)  : ORIGIN = 0x0, LENGTH = 16K
}

PHDRS
{
    rodata PT_LOAD ;
}

SECTIONS
{
    .rodata :
    {
        dct_location = .;
        KEEP(*(.rodata.initial_dct))
        KEEP(*(.rodata._app_dct))
        KEEP(*(.rodata.wiced_apps_lut))
        KEEP(*(.rodata.wiced_apps_lut_fill))
        dct_end = .;

    } >DCT1_FLASH  AT> DCT1_FLASH :rodata
}

PROVIDE( dct_full_size_loc = LENGTH( DCT1_FLASH ));
PROVIDE( dct_used_size_loc = (dct_end - ORIGIN( DCT1_FLASH )));