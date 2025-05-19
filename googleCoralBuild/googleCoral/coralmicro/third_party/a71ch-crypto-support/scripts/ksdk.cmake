#
# Copyright 2018 NXP
# SPDX-License-Identifier: Apache-2.0
#
IF(NOT WithKSDK)
    MESSAGE(FATAL_ERROR "Not to be included without KSDK")
ENDIF()

SET(CMAKE_EXECUTABLE_SUFFIX ".axf")

IF(NXPInternal)
    SET(
        WARNINGS_FLAGS
        "\
        -Werror=implicit-function-declaration \
        -Werror=incompatible-pointer-types \
        -Werror=unused-function"
    )
    SET(WARNINGS_FLAGS "${WARNINGS_FLAGS} -Werror")
ENDIF()

# IF(WithHost_frdmk64f) LINK_DIRECTORIES(${CMAKE_SOURCE_DIR}/ext/mcu-sdk/middleware/mmcau/asm-cm4-cm7) ENDIF()

MACRO(CREATE_BINARY PROJECT_NAME)
    SET(_FLAGS_L_MAP " -Xlinker -Map=${CMAKE_BINARY_DIR}/bin/${PROJECT_NAME}.map ")

    IF(WithHost_lpcxpresso55s_s)
        SET(
            _FLAGS_L_IMPLIB
            " \
        -Xlinker \
        --out-implib=${CMAKE_SOURCE_DIR}/binaries/${PROJECT_NAME}_CMSE_lib.o \
    "
        )
    ENDIF()

    SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${_FLAGS_L_IMPLIB} ${_FLAGS_L_MAP} ")

    ADD_CUSTOM_COMMAND(
        TARGET ${PROJECT_NAME}
        POST_BUILD
        COMMAND ${CMAKE_OBJCOPY} -O binary ${PROJECT_NAME}${CMAKE_EXECUTABLE_SUFFIX} ${PROJECT_NAME}.bin
        WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
        COMMENT "${PROJECT_NAME}${CMAKE_EXECUTABLE_SUFFIX} -> ${PROJECT_NAME}.bin"
    )
ENDMACRO()

MACRO(
    COPY_TO_SOURCEDIR
    PROJECT_NAME
    TARGET_DIRNAME
    TARGET_SUFFIX
)
    SET(eSEName "eSEName-NA")
    IF(WithApplet_A71XX)
        SET(eSEName "A71XX")
    ENDIF()
    IF(WithApplet_SE050_EAR)
        SET(eSEName "SE050_EAR")
    ENDIF()
    IF(SSS_HAVE_APPLET_SE05X_IOT)
        SET(eSEName "SE050x")
    ENDIF()
    IF(NXPInternal)
        IF(
            CMAKE_BUILD_TYPE
            STREQUAL
            "Release"
        )
            ADD_CUSTOM_COMMAND(
                TARGET ${PROJECT_NAME}
                POST_BUILD
                COMMAND
                    ${CMAKE_COMMAND} -E copy ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${PROJECT_NAME}.bin
                    ${CMAKE_SOURCE_DIR}/${TARGET_DIRNAME}/${PROJECT_NAME}-${TARGET_SUFFIX}-${KSDK_BoardName}-${eSEName}.bin
                COMMENT
                    "Copy ${PROJECT_NAME}.bin to ${TARGET_DIRNAME}/${PROJECT_NAME}-${TARGET_SUFFIX}-${KSDK_BoardName}-${eSEName}.bin"
            )
        ENDIF()
    ELSE()
        ADD_CUSTOM_COMMAND(
            TARGET ${PROJECT_NAME}
            POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${PROJECT_NAME}.bin
                    ${CMAKE_SOURCE_DIR}/${TARGET_DIRNAME}/${PROJECT_NAME}-${eSMCOM}-${KSDK_BoardName}-${eSEName}.bin
            COMMENT
                "Copy ${PROJECT_NAME}.bin to ${TARGET_DIRNAME}/${PROJECT_NAME}-${eSMCOM}-${KSDK_BoardName}-${eSEName}.bin"
        )
    ENDIF()

ENDMACRO()

ENABLE_LANGUAGE(ASM)

INCLUDE_DIRECTORIES(
    ${CMAKE_SOURCE_DIR}/demos/ksdk/common/freertos/boards/${KSDK_BoardName}
    ${CMAKE_SOURCE_DIR}/demos/ksdk/common/boards/${KSDK_BoardName}/project_template
    ${CMAKE_SOURCE_DIR}/demos/ksdk/common/boards/${KSDK_BoardName}
    ${CMAKE_SOURCE_DIR}/ext/mcu-sdk/CMSIS/Include
    ${CMAKE_SOURCE_DIR}/ext/mcu-sdk/middleware/mmcau
    ${CMAKE_SOURCE_DIR}/ext/mcu-sdk/platform/utilities/debug_console
    ${CMAKE_SOURCE_DIR}/ext/mcu-sdk/platform/utilities/shell
    ${CMAKE_SOURCE_DIR}/ext/mcu-sdk/platform/utilities/str
    ${CMAKE_SOURCE_DIR}/ext/mcu-sdk/platform/utilities
    ${CMAKE_SOURCE_DIR}/ext/mcu-sdk/devices/${KSDK_CPUName}
    ${CMAKE_SOURCE_DIR}/ext/mcu-sdk/devices/${KSDK_CPUName}/drivers
    ${CMAKE_SOURCE_DIR}/ext/mcu-sdk/devices/${KSDK_CPUName}/utilities
    ${CMAKE_SOURCE_DIR}/ext/mcu-sdk/components/serial_manager
    ${CMAKE_SOURCE_DIR}/ext/mcu-sdk/components/uart
)

LINK_DIRECTORIES(${CMAKE_SOURCE_DIR}/ext/mcu-sdk/middleware/mmcau/asm-cm4-cm7)

ADD_DEFINITIONS(
    -D__NEWLIB__
    -D__USE_CMSIS
    -DPRINTF_ADVANCED_ENABLE=1
    -DSCANF_ADVANCED_ENABLE=0
    -DSCANF_FLOAT_ENABLE=0
    -DSDK_DEBUGCONSOLE=1
    -DSDK_DEBUGCONSOLE_UART=1
    -DDEBUG_CONSOLE_RX_ENABLE=0
)

SET(
    _FLAGS_COMMON
    " \
    -Wall \
    -fno-common \
    -ffunction-sections \
    -fdata-sections \
    -ffreestanding \
    -fno-builtin \
    -mthumb \
    -mapcs"
)

SET(_FLAGS_COMMON "${_FLAGS_COMMON} ${WARNINGS_FLAGS}")

SET(
    _FLAGS_C_COMMON
    " ${_FLAGS_COMMON} \
    -D_POSIX_SOURCE \
    -MMD -MP \
    -std=gnu99 \
    -fomit-frame-pointer "
)
SET(
    _FLAGS_CXX_COMMON
    " ${_FLAGS_COMMON} \
    -MMD -MP \
    -fno-rtti -fno-exceptions "
)
SET(_FLAGS_DEBUG_COMMON " -g -DDEBUG -O0 ")
SET(_FLAGS_RELEASE_COMMON " -DNDEBUG -Os ")
SET(
    _FLAGS_L_COMMON
    " \
    -Xlinker --gc-sections \
    -Xlinker -static \
    -Xlinker --sort-section=alignment "
)
# -Xlinker -z \ -Xlinker muldefs ")

IF(WithHost_frdmk64f)
    INCLUDE(scripts/ksdk_frdmk64f.cmake)
ELSEIF(WithHost_evkmimxrt1060)
    INCLUDE(scripts/ksdk_evkmimxrt1060.cmake)
ELSEIF(
    WithHost_lpcxpresso55s
    OR WithHost_lpcxpresso55s_ns
    OR WithHost_lpcxpresso55s_s
)
    SET(_FLAGS_DEBUG_COMMON " -O1 ")
    INCLUDE(scripts/ksdk_lpcxpresso55s.cmake)
ELSEIF(WithHost_lpcxpresso55s_s)
    INCLUDE(scripts/ksdk_lpcxpresso55s.cmake)
ENDIF()

SET(CMAKE_ASM_FLAGS "${CMAKE_ASM_FLAGS} \
    -D__STARTUP_CLEAR_BSS ${_FLAGS_COMMON} ${_FLAGS_CPU} "
)
SET(CMAKE_ASM_FLAGS_DEBUG "${CMAKE_ASM_FLAGS_DEBUG} \
    ${CMAKE_ASM_FLAGS} -DDEBUG -g"
)
SET(CMAKE_ASM_FLAGS_RELEASE "${CMAKE_ASM_FLAGS_RELEASE} \
    ${CMAKE_ASM_FLAGS} -DNDEBUG "
)
SET(CMAKE_ASM_FLAGS_RELWITHDEBINFO "${CMAKE_ASM_FLAGS_RELWITHDEBINFO} \
    ${CMAKE_ASM_FLAGS} -DNDEBUG "
)

SET(CMAKE_C_FLAGS " ${CMAKE_C_FLAGS} ${_FLAGS_C_COMMON} ${_FLAGS_CPU} ")
SET(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} \
    ${_FLAGS_DEBUG_COMMON} "
)
SET(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} \
    ${_FLAGS_RELEASE_COMMON} "
)
SET(CMAKE_C_FLAGS_RELWITHDEBINFO "${CMAKE_C_FLAGS_RELWITHDEBINFO} \
    ${_FLAGS_RELEASE_COMMON} "
)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} \
    ${_FLAGS_CXX_COMMON} ${_FLAGS_CPU} "
)
SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} \
    ${_FLAGS_DEBUG_COMMON}"
)
SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} \
    ${_FLAGS_RELEASE_COMMON}"
)
SET(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} \
    ${_FLAGS_RELEASE_COMMON}"
)
SET(
    _FLAGS_L_ALL
    "${_FLAGS_L_ALL} \
    ${_FLAGS_L_COMMON} \
    ${_FLAGS_L_SPECS} \
    ${_FLAGS_L_MEM} \
    ${_FLAGS_L_LD} \
    "
)
SET(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} \
    -g ${_FLAGS_L_ALL}"
)
SET(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} \
    ${_FLAGS_L_ALL}"
)
SET(CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO "${CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO} \
    ${_FLAGS_L_ALL}"
)
