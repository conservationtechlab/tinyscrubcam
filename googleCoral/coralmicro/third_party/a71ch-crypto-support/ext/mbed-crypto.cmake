# Copyright 2020 NXP
#
# SPDX-License-Identifier: Apache-2.0
#
#
# Manually create project. mbedTLS has it's own CMakeLists.txt
#
PROJECT(mbedtls)

FILE(
    GLOB
    mbedtls_sources
    mbed-crypto/library/*.c
    mbed-crypto/library/*.h
    mbed-crypto/include/mbedtls/*.h
)

IF(NOT WithHost_lpcxpresso55s_ns)

    GET_FILENAME_COMPONENT(
        full_path_psa_its_file
        ${CMAKE_CURRENT_SOURCE_DIR}/mbed-crypto/library/psa_its_file.c
        ABSOLUTE
    )

    LIST(
        REMOVE_ITEM
        mbedtls_sources
        "${full_path_psa_its_file}"
    )

    LIST(
        APPEND
        mbedtls_sources
        mbed-crypto/port/sss_psa_its_file.c
    )

ELSE()

    GET_FILENAME_COMPONENT(
        full_path_psa_crypto_file
        ${CMAKE_CURRENT_SOURCE_DIR}/mbed-crypto/library/psa_crypto.c
        ABSOLUTE
    )

    LIST(
        REMOVE_ITEM
        mbedtls_sources
        "${full_path_psa_crypto_file}"
    )

    GET_FILENAME_COMPONENT(
        full_path_psa_crypto_slot_mgmt_file
        ${CMAKE_CURRENT_SOURCE_DIR}/mbed-crypto/library/psa_crypto_slot_management.c
        ABSOLUTE
    )

    LIST(
        REMOVE_ITEM
        mbedtls_sources
        "${full_path_psa_crypto_slot_mgmt_file}"
    )

ENDIF()

IF(WithmbedTLS_ALT_A71CH)
    FILE(
        GLOB
        mbedtls_alt
        ${CMAKE_SOURCE_DIR}/hostlib/hostLib/mbedtls/src/*.c
        ${CMAKE_SOURCE_DIR}/hostlib/hostLib/mbedtls/src/*.h
        ${CMAKE_SOURCE_DIR}/hostlib/hostLib/mbedtls/inc/*.h
    )
ENDIF()

IF(WithmbedTLS_ALT_SSS)
    FILE(
        GLOB
        mbedtls_alt
        ${CMAKE_SOURCE_DIR}/sss/plugin/mbedtls/*.c
        ${CMAKE_SOURCE_DIR}/hostlib/hostLib/mbedtls/src/*_alt.c
        mbedtls/port/ksdk/ecp_curves_alt.c
    )
ENDIF()

IF(WithmbedTLS_ALT_A71CH)
    FILE(
        GLOB
        mbedtls_alt
        ${CMAKE_SOURCE_DIR}/hostlib/hostLib/mbedtls/src/*_alt.c
        ${CMAKE_SOURCE_DIR}/hostlib/hostLib/mbedtls/src/*_alt_ax.c
        #mbedtls/port/ksdk/ecp_curves_alt.c
    )
ENDIF()

IF(WithKSDK)
    FILE(
        GLOB
        mbed_port_sources
        ${CMAKE_SOURCE_DIR}/demos/tfm_port/*.c
        ${CMAKE_SOURCE_DIR}/demos/tfm_port/*.h
    )

    FILE(
        GLOB
        mbedtls_ksdk_sources
        mbedtls/port/ksdk/ksdk_mbedtls.c
        mbedtls/port/ksdk/ksdk_mbedtls.h
    )
ENDIF()

IF(WithHost_Android)
    ADD_LIBRARY(
        ${PROJECT_NAME} SHARED
        ${mbed_port_sources}
        ${mbedtls_ksdk_sources}
        ${mbedtls_sources}
        ${mbedtls_alt}
    )
ELSE()
    ADD_LIBRARY(
        ${PROJECT_NAME}
        ${mbed_port_sources}
        ${mbedtls_ksdk_sources}
        ${mbedtls_sources}
        ${mbedtls_alt}
    )
ENDIF()

TARGET_INCLUDE_DIRECTORIES(
    ${PROJECT_NAME}
    PUBLIC mbed-crypto/include
    PUBLIC mbed-crypto/library
)

IF(WithKSDK)
    TARGET_INCLUDE_DIRECTORIES(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/demos/tfm_port)

    TARGET_INCLUDE_DIRECTORIES(${PROJECT_NAME} PUBLIC mbedtls/port/ksdk)

    IF(WithmbedTLS_ALT_A71CH)
        TARGET_COMPILE_DEFINITIONS(
            ${PROJECT_NAME} PUBLIC MBEDTLS_CONFIG_FILE=\"a71ch_mbedtls_${KSDK_BoardName}_config.h\"
        )

        TARGET_INCLUDE_DIRECTORIES(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/hostlib/hostLib/mbedtls/inc)
        TARGET_LINK_LIBRARIES(
            ${PROJECT_NAME}
            smCom
            SSS_APIs
        )
    ELSEIF(WithmbedTLS_ALT_SSS)
        TARGET_COMPILE_DEFINITIONS(
            ${PROJECT_NAME} PUBLIC MBEDTLS_CONFIG_FILE=\"sss_mbedtls_${KSDK_BoardName}_config.h\"
        )
        TARGET_INCLUDE_DIRECTORIES(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/sss/plugin/mbedtls)
        TARGET_LINK_LIBRARIES(
            ${PROJECT_NAME}
            smCom
            SSS_APIs
        )
    ELSE()
        TARGET_COMPILE_DEFINITIONS(${PROJECT_NAME} PUBLIC MBEDTLS_CONFIG_FILE=\"mbedcrypto_mcux_config.h\")
    ENDIF()

    TARGET_LINK_LIBRARIES(
        ${PROJECT_NAME}
        board
        _mmcau
    )
ELSE() # KSDK
    IF(WithmbedTLS_ALT_A71CH)
        TARGET_COMPILE_DEFINITIONS(${PROJECT_NAME} PUBLIC MBEDTLS_CONFIG_FILE=\"a71ch_mbedtls_x86_config.h\")
        TARGET_INCLUDE_DIRECTORIES(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/hostlib/hostLib/mbedtls/inc)
        TARGET_LINK_LIBRARIES(
            ${PROJECT_NAME}
            smCom
            SSS_APIs
        )
    ELSEIF(WithmbedTLS_ALT_SSS)
        TARGET_COMPILE_DEFINITIONS(${PROJECT_NAME} PUBLIC MBEDTLS_CONFIG_FILE=\"sss_mbedtls_x86_config.h\")
        TARGET_INCLUDE_DIRECTORIES(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/sss/plugin/mbedtls)
        TARGET_LINK_LIBRARIES(
            ${PROJECT_NAME}
            smCom
            SSS_APIs
        )
    ENDIF()
ENDIF()

IF(NOT WithHost_lpcxpresso55s_s)
    TARGET_LINK_LIBRARIES(
        ${PROJECT_NAME}
        PSA_ALT
        SSS_APIs
    )
ENDIF()

TARGET_INCLUDE_DIRECTORIES(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/sss/plugin/psa/inc)

IF(
    CMAKE_CXX_COMPILER
    MATCHES
    ".*clang"
    OR CMAKE_CXX_COMPILER_ID
       STREQUAL
       "AppleClang"
)
    TARGET_COMPILE_OPTIONS(
        ${PROJECT_NAME}
        PRIVATE -Wno-unused-function
        PRIVATE -Wno-error=pointer-sign
        PRIVATE -Wno-error=format
        PRIVATE -Wno-format
        PRIVATE -Wno-error=unused-const-variable
        PRIVATE -Wno-unused-const-variable
    )
ENDIF()

IF(
    "${CMAKE_CXX_COMPILER_ID}"
    MATCHES
    "MSVC"
)
    IF(NXPInternal)
        TARGET_COMPILE_OPTIONS(
            ${PROJECT_NAME}
            PRIVATE /wd4245 # '=': conversion from 'int' to 'mbedtls_mpi_uint', signed/unsigned misma
            PRIVATE /wd4310 # cast truncates constant value
            PRIVATE /wd4389 # '==': signed/unsigned mismatch
            PRIVATE /wd4132 # const object should be initialized
            PRIVATE /wd4127 # conditional expression is constant
            PRIVATE /wd4701 # potentially uninitialized local variable
            PRIVATE /wd4477 # 'printf' : format string '%d'
            PRIVATE /wd4200 # nonstandard extension used
            PRIVATE /wd4703 # potentially unintialized local pointer
        )
    ENDIF()
ENDIF()

IF(
    "${CMAKE_CXX_COMPILER_ID}"
    STREQUAL
    "GNU"
)
    TARGET_COMPILE_OPTIONS(
        ${PROJECT_NAME}
        PRIVATE -Wno-unused-function
        PRIVATE -Wno-error=pointer-sign
        PRIVATE -Wno-error=format
        PRIVATE -Wno-format
    )

    SET(GCC_VERSION_WITH_UNUSED_CONST 6.3.0)
    IF(
        GCC_VERSION_WITH_UNUSED_CONST
        VERSION_LESS
        CMAKE_CXX_COMPILER_VERSION
    )
        TARGET_COMPILE_OPTIONS(
            ${PROJECT_NAME}
            PRIVATE -Wno-error=unused-const-variable
            PRIVATE -Wno-unused-const-variable
        )
    ENDIF()
ENDIF()

IF(
    "${CMAKE_CXX_COMPILER_ID}"
    STREQUAL
    "MSVC"
)
    ADD_SUBDIRECTORY(mbed-crypto/tests)
ENDIF()
