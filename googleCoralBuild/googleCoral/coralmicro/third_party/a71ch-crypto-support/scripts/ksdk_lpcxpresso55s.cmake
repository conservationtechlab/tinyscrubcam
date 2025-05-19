# Copyright 2019 NXP
#
# SPDX-License-Identifier: Apache-2.0
#
# ksdk_lpcxpresso55s69.cmake

IF(WithHost_lpcxpresso55s_s)
    FILE(
        GLOB
        KSDK_VENEER_FILES
        ${CMAKE_SOURCE_DIR}/demos/lpc55s/tzm/tzm_DummyTZEntry.c
        ${CMAKE_SOURCE_DIR}/demos/lpc55s/tzm/tzm_SystemInitHook.c
    )
ENDIF()

FILE(
    GLOB
    KSDK_STARTUP_FILE
    ${CMAKE_SOURCE_DIR}/ext/mcu-sdk/devices/${KSDK_CPUName}/gcc/startup_LPC55S69_cm33_core0.S
    ${KSDK_VENEER_FILES}
)

ADD_DEFINITIONS(
    -DLPC_55x
    -DA_LITTLE_ENDIAN
    -DCPU_LPC55S69JBD100_cm33_core0
    -DCPU_LPC55S69JBD100
    -DCPU_LPC55S69JBD100_cm33
    -DARM_MATH_CM33
    -D__MULTICORE_MASTER
)

IF(WithHost_lpcxpresso55s_s)
    ADD_DEFINITIONS(-mcmse)
    ADD_DEFINITIONS(-DSECURE_WORLD)
ENDIF()

IF(WithHost_lpcxpresso55s_ns)
    ADD_DEFINITIONS(-D__SEMIHOST_HARDFAULT_DISABLE)
    ADD_DEFINITIONS(-DNONSECURE_WORLD)
ENDIF()

INCLUDE_DIRECTORIES(
    demos/ksdk/common/boards/lpcxpresso55s69/se_hostlib_examples/cloud_demo hostlib/hostlib/platform/ksdk
)

SET(_FLAGS_CPU " -mcpu=cortex-m33 -mfpu=fpv5-sp-d16 -mfloat-abi=hard ")
SET(_FLAGS_L_SPECS " --specs=nano.specs --specs=nosys.specs ")

IF(WithRTOS_FreeRTOS)
    SET(
        _FLAGS_L_MEM
        " \
    -Xlinker --defsym=__ram_vector_table__=1 \
    -Xlinker --defsym=__stack_size__=0x2000 \
    -Xlinker --defsym=__heap_size__=0x8000 "
    )
ENDIF()
IF(WithRTOS_Default)
    SET(
        _FLAGS_L_MEM
        " \
    -Xlinker --defsym=__ram_vector_table__=1 \
    -Xlinker --defsym=__stack_size__=0x4000 \
    -Xlinker --defsym=__heap_size__=0x4000 "
    )
ENDIF()

IF(WithHost_lpcxpresso55s)
    SET(_LD_FILE_ForWorld "LPC55S69_cm33_core0_flash.ld")
ENDIF()

IF(WithHost_lpcxpresso55s_ns)
    SET(_LD_FILE_ForWorld "LPC55S69_cm33_core0_flash_ns.ld")
ENDIF()

IF(WithHost_lpcxpresso55s_s)
    SET(_LD_FILE_ForWorld "LPC55S69_cm33_core0_flash_s.ld")
ENDIF()

IF(WithHost_lpcxpresso55s_s)
    SET(
        _FLAGS_L_CMSE
        " \
 -Xlinker \
 --cmse-implib \
"
    )
ENDIF()

SET(
    _FLAGS_L_LD
    " -lpower_hardabi \
    ${_FLAGS_L_CMSE} \
    -T${CMAKE_SOURCE_DIR}/ext/mcu-sdk/devices/LPC55S69/gcc/${_LD_FILE_ForWorld} \
    -static "
)

LINK_DIRECTORIES(${CMAKE_SOURCE_DIR}/ext/mcu-sdk/devices/LPC55S69/gcc)
