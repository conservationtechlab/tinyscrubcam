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

NAME = PSoC6_Peripheral_Libraries

GLOBAL_INCLUDES :=  cmsis/include                                                             \
                    devices/psoc6/ip                                                          \
                    devices/psoc6/$(HOST_MCU_PART_FAMILY)/include                             \
                    drivers/peripheral                                                        \
                    drivers/peripheral/creator                                                \
                    drivers/peripheral/crypto                                                 \
                    drivers/peripheral/ctb                                                    \
                    drivers/peripheral/ctdac                                                  \
                    drivers/peripheral/dma                                                    \
                    drivers/peripheral/efuse                                                  \
                    drivers/peripheral/flash                                                  \
                    drivers/peripheral/gpio                                                   \
                    drivers/peripheral/i2s                                                    \
                    drivers/peripheral/ipc                                                    \
                    drivers/peripheral/lpcomp                                                 \
                    drivers/peripheral/lvd                                                    \
                    drivers/peripheral/mcwdt                                                  \
                    drivers/peripheral/pdm_pcm                                                \
                    drivers/peripheral/profile                                                \
                    drivers/peripheral/prot                                                   \
                    drivers/peripheral/rtc                                                    \
                    drivers/peripheral/sar                                                    \
                    drivers/peripheral/scb                                                    \
                    drivers/peripheral/smif                                                   \
                    drivers/peripheral/sysanalog                                              \
                    drivers/peripheral/sysclk                                                 \
                    drivers/peripheral/sysint                                                 \
                    drivers/peripheral/syslib                                                 \
                    drivers/peripheral/syspm                                                  \
                    drivers/peripheral/systick                                                \
                    drivers/peripheral/tcpwm                                                  \
                    drivers/peripheral/trigmux                                                \
                    drivers/peripheral/wdt

GLOBAL_INCLUDES +=  drivers/peripheral/sdio

$(NAME)_SOURCES := devices/psoc6/$(HOST_MCU_PART_FAMILY)/common/$(HOST_MCU_PART_FAMILY)_cm0plus_image.c          \
                   devices/psoc6/$(HOST_MCU_PART_FAMILY)/common/system_$(HOST_MCU_PART_FAMILY)_cm4.c             \
                   drivers/peripheral/creator/cyfitter_amux.c                                                    \
                   drivers/peripheral/creator/cyfitter_cfg.c                                                     \
                   drivers/peripheral/creator/cyfitter_sysint_cfg.c                                              \
                   drivers/peripheral/creator/cymetadata.c                                                       \
                   drivers/peripheral/crypto/cy_crypto_config.c                                                  \
                   drivers/peripheral/ctb/cy_ctb.c                                                               \
                   drivers/peripheral/ctdac/cy_ctdac.c                                                           \
                   drivers/peripheral/dma/cy_dma.c                                                               \
                   drivers/peripheral/efuse/cy_efuse.c                                                           \
                   drivers/peripheral/flash/cy_flash.c                                                           \
                   drivers/peripheral/gpio/cy_gpio.c                                                             \
                   drivers/peripheral/i2s/cy_i2s.c                                                               \
                   drivers/peripheral/ipc/cy_ipc_config.c                                                        \
                   drivers/peripheral/ipc/cy_ipc_drv.c                                                           \
                   drivers/peripheral/ipc/cy_ipc_pipe.c                                                          \
                   drivers/peripheral/ipc/cy_ipc_sema.c                                                          \
                   drivers/peripheral/lpcomp/cy_lpcomp.c                                                         \
                   drivers/peripheral/lvd/cy_lvd.c                                                               \
                   drivers/peripheral/mcwdt/cy_mcwdt.c                                                           \
                   drivers/peripheral/pdm_pcm/cy_pdm_pcm.c                                                       \
                   drivers/peripheral/profile/cy_profile.c                                                       \
                   drivers/peripheral/prot/cy_prot.c                                                             \
                   drivers/peripheral/rtc/cy_rtc.c                                                               \
                   drivers/peripheral/sar/cy_sar.c                                                               \
                   drivers/peripheral/scb/cy_scb_common.c                                                        \
                   drivers/peripheral/scb/cy_scb_ezi2c.c                                                         \
                   drivers/peripheral/scb/cy_scb_i2c.c                                                           \
                   drivers/peripheral/scb/cy_scb_spi.c                                                           \
                   drivers/peripheral/scb/cy_scb_uart.c                                                          \
                   drivers/peripheral/smif/cy_smif.c                                                             \
                   drivers/peripheral/smif/cy_smif_memslot.c                                                     \
                   drivers/peripheral/sysanalog/cy_sysanalog.c                                                   \
                   drivers/peripheral/sysclk/cy_sysclk.c                                                         \
                   drivers/peripheral/sysint/cy_sysint.c                                                         \
                   drivers/peripheral/syslib/cy_syslib.c                                                         \
                   drivers/peripheral/syslib/gcc/cy_syslib_gcc.S                                                 \
                   drivers/peripheral/syspm/cy_syspm.c                                                           \
                   drivers/peripheral/systick/cy_systick.c                                                       \
                   drivers/peripheral/tcpwm/cy_tcpwm_counter.c                                                   \
                   drivers/peripheral/tcpwm/cy_tcpwm_pwm.c                                                       \
                   drivers/peripheral/tcpwm/cy_tcpwm_quaddec.c                                                   \
                   drivers/peripheral/trigmux/cy_trigmux.c                                                       \
                   drivers/peripheral/wdt/cy_wdt.c

$(NAME)_SOURCES += drivers/peripheral/sdio/SDIO_HOST.c                                                           \
                   drivers/peripheral/sdio/SDIO_HOST_CMD_DMA.c                                                   \
                   drivers/peripheral/sdio/SDIO_HOST_Read_DMA.c                                                  \
                   drivers/peripheral/sdio/SDIO_HOST_Resp_DMA.c                                                  \
                   drivers/peripheral/sdio/SDIO_HOST_Write_DMA.c

$(NAME)_PREBUILT_LIBRARY := drivers/peripheral/crypto/libs/cy_crypto_client_gcc.a          \
                            drivers/peripheral/crypto/libs/cy_crypto_server_gcc_base.a     \
                            drivers/peripheral/crypto/libs/cy_crypto_server_gcc_extra.a    \
                            drivers/peripheral/crypto/libs/cy_crypto_server_gcc_full.a

ifeq ($(TOOLCHAIN_NAME),GCC)
GLOBAL_CFLAGS += -fms-extensions -fno-strict-aliasing -Wno-missing-braces # Required for compiling unnamed structure and union fields
endif
