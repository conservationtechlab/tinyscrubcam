#
# Copyright 2019 NXP
# SPDX-License-Identifier: Apache-2.0
#
# ksdk_evkmimxrt1060.cmake

FILE(
    GLOB
    KSDK_STARTUP_FILE
    ${CMAKE_SOURCE_DIR}/ext/mcu-sdk/devices/${KSDK_CPUName}/gcc/*.S
    ${CMAKE_SOURCE_DIR}/demos/ksdk/common/boards/${KSDK_BoardName}/xip/*_flexspi_nor_config.c
    ${CMAKE_SOURCE_DIR}/demos/ksdk/common/boards/${KSDK_BoardName}/xip/*_sdram_ini_dcd.c
    ${CMAKE_SOURCE_DIR}/ext/mcu-sdk/devices/${KSDK_CPUName}/xip/fsl_flexspi_nor_boot.c
)

ADD_DEFINITIONS(
    -DCPU_MIMXRT1062DVL6A
    -DCPU_MIMXRT1062DVL6A_cm7
    -DXIP_EXTERNAL_FLASH=1
    -DXIP_BOOT_HEADER_ENABLE=1
    # -DXIP_BOOT_HEADER_DCD_ENABLE=1
    -DMIMXRT1060_EVK
    -DFSL_FEATURE_PHYKSZ8081_USE_RMII50M_MODE
    -DIMX_RT
)

IF(WithmbedTLS_ALT_None)
    # With mbedTLS ALT, we could have covered this in SSS specific file, but without mbedTLS ALT, this needs to be
    # defined for mbedTLS SW Crypto

    ADD_DEFINITIONS(-DMBEDTLS_CTR_DRBG_C)
ENDIF()

IF(WithRTOS_Default)
    ADD_DEFINITIONS(-DFSL_RTOS_BM -DSDK_OS_BAREMETAL)
ENDIF()

IF(WithRTOS_FreeRTOS)
    ADD_DEFINITIONS(-DFSL_RTOS_FREE_RTOS)
ENDIF()

INCLUDE_DIRECTORIES(demos/ksdk/common/boards/evkmimxrt1060/se_hostlib_examples/mainA71CH)

SET(_FLAGS_CPU " -mcpu=cortex-m7 -mfpu=fpv5-sp-d16 -mfloat-abi=hard ")
SET(_FLAGS_L_SPECS "--specs=nano.specs --specs=nosys.specs ")
IF(WithRTOS_FreeRTOS)
    SET(
        _FLAGS_L_MEM
        " \
    -Xlinker --defsym=__ram_vector_table__=1 \
    -Xlinker --defsym=__stack_size__=0x2000 \
    -Xlinker --defsym=__heap_size__=0x8000 "
    )
    SET(
        _FLAGS_L_LD
        " \
    -T${CMAKE_SOURCE_DIR}/ext/mcu-sdk/devices/MIMXRT1062/gcc/WithRTOS_FreeRTOS/MIMXRT1062xxxxx_flexspi_nor.ld \
    -static "
    )
ENDIF()
IF(WithRTOS_Default)
    SET(
        _FLAGS_L_MEM
        " \
    -Xlinker --defsym=__ram_vector_table__=1 \
    -Xlinker --defsym=__stack_size__=0x1000 \
    -Xlinker --defsym=__heap_size__=0x1000 "
    )
    SET(
        _FLAGS_L_LD
        " \
    -T${CMAKE_SOURCE_DIR}/ext/mcu-sdk/devices/MIMXRT1062/gcc/WithRTOS_Default/MIMXRT1062xxxxx_flexspi_nor.ld \
    -static "
    )
ENDIF()
