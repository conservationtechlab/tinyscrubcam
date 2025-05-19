#
# Copyright 2021, Cypress Semiconductor Corporation or a subsidiary of 
 # Cypress Semiconductor Corporation. All Rights Reserved.
 # This software, including source code, documentation and related
 # materials ("Software"), is owned by Cypress Semiconductor Corporation
 # or one of its subsidiaries ("Cypress") and is protected by and subject to
 # worldwide patent protection (United States and foreign),
 # United States copyright laws and international treaty provisions.
 # Therefore, you may use this Software only as provided in the license
 # agreement accompanying the software package from which you
 # obtained this Software ("EULA").
 # If no EULA applies, Cypress hereby grants you a personal, non-exclusive,
 # non-transferable license to copy, modify, and compile the Software
 # source code solely for use in connection with Cypress's
 # integrated circuit products. Any reproduction, modification, translation,
 # compilation, or representation of this Software except as specified
 # above is prohibited without the express written permission of Cypress.
 #
 # Disclaimer: THIS SOFTWARE IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND,
 # EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, NONINFRINGEMENT, IMPLIED
 # WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. Cypress
 # reserves the right to make changes to the Software without notice. Cypress
 # does not assume any liability arising out of the application or use of the
 # Software or any product or circuit described in the Software. Cypress does
 # not authorize its products for use in any products where a malfunction or
 # failure of the Cypress product may reasonably be expected to result in
 # significant property damage, injury or death ("High Risk Product"). By
 # including Cypress's product in a High Risk Product, the manufacturer
 # of such system or application assumes all risk of such use and in doing
 # so agrees to indemnify Cypress against all liability.
#

NAME = STM32L4xx_Peripheral_Libraries

GLOBAL_INCLUDES := . \
                   Drivers/CMSIS/Device/ST/STM32L4xx/Include \
                   Drivers/STM32L4xx_HAL_Driver/Inc \
                   ../../../$(HOST_ARCH)/CMSIS

$(NAME)_SOURCES := \
                   Drivers/CMSIS/Device/ST/STM32L4xx/Source/Templates/system_stm32l4xx.c \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal.c \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_adc.c \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_adc_ex.c \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_can.c \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_comp.c \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_cortex.c \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_crc.c \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_crc_ex.c \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_cryp.c \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_cryp_ex.c \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_dac.c \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_dac_ex.c \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_dcmi.c \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_dfsdm.c \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_dfsdm_ex.c \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_dma.c \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_dma2d.c \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_dma_ex.c \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_dsi.c \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_firewall.c \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_flash.c \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_flash_ex.c \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_flash_ramfunc.c \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_gfxmmu.c \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_gpio.c \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_hash.c \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_hash_ex.c \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_hcd.c \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_i2c.c \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_i2c_ex.c \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_irda.c \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_iwdg.c \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_lcd.c \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_lptim.c \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_ltdc.c \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_ltdc_ex.c \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_msp_template.c \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_nand.c \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_nor.c \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_opamp.c \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_opamp_ex.c \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_ospi.c \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_pcd.c \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_pcd_ex.c \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_pwr.c \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_pwr_ex.c \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_qspi.c \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_rcc.c \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_rcc_ex.c \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_rng.c \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_rtc.c \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_rtc_ex.c \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_sai.c \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_sai_ex.c \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_sd.c \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_sd_ex.c \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_smartcard.c \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_smartcard_ex.c \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_smbus.c \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_spi.c \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_spi_ex.c \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_sram.c \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_swpmi.c \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_tim.c \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_tim_ex.c \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_timebase_tim_template.c \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_tsc.c \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_uart.c \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_uart_ex.c \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_usart.c \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_usart_ex.c \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_wwdg.c \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_ll_adc.c \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_ll_comp.c \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_ll_crc.c \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_ll_crs.c \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_ll_dac.c \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_ll_dma.c \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_ll_dma2d.c \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_ll_exti.c \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_ll_fmc.c \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_ll_gpio.c \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_ll_i2c.c \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_ll_lptim.c \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_ll_lpuart.c \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_ll_opamp.c \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_ll_pwr.c \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_ll_rcc.c \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_ll_rng.c \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_ll_rtc.c \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_ll_sdmmc.c \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_ll_spi.c \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_ll_swpmi.c \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_ll_tim.c \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_ll_usart.c \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_ll_usb.c \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_ll_utils.c
