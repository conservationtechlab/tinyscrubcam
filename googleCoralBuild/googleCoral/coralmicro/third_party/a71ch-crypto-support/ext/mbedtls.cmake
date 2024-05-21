# Copyright 2019 NXP
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
    mbedtls.cmake
    mbedtls/library/*.c
    mbedtls/library/*.h
    mbedtls/include/mbedtls/*.h
)

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
        ${CMAKE_SOURCE_DIR}/sss/plugin/mbedtls/ecdh_alt_ax.c
        ${CMAKE_SOURCE_DIR}/sss/plugin/mbedtls/ecp_alt.c
        ${CMAKE_SOURCE_DIR}/sss/plugin/mbedtls/sss_mbedtls.c
        ${CMAKE_SOURCE_DIR}/sss/plugin/mbedtls/sss_mbedtls_rsa.c
        ${CMAKE_SOURCE_DIR}/hostlib/hostLib/mbedtls/src/*_alt.c
        mbedtls/port/ksdk/ecp_curves_alt.c
        mbedtls/port/ksdk/ecp_alt.c
    )
ENDIF()

IF(WithmbedTLS_ALT_A71CH)
    FILE(
        GLOB
        mbedtls_alt
        ${CMAKE_SOURCE_DIR}/hostlib/hostLib/mbedtls/src/*_alt.c
        ${CMAKE_SOURCE_DIR}/hostlib/hostLib/mbedtls/src/*_alt_ax.c
        mbedtls/port/ksdk/ecp_curves_alt.c
    )
ENDIF()

IF(WithKSDK)
    FILE(
        GLOB
        mbedtls_ksdk_sources
        mbedtls/port/ksdk/*.c
        mbedtls/port/ksdk/*.h
    )
ENDIF()

IF(WithHost_Android)
    ADD_LIBRARY(
        ${PROJECT_NAME} SHARED
        ${mbedtls_ksdk_sources}
        ${mbedtls_sources}
        ${mbedtls_alt}
    )
ELSE()
    ADD_LIBRARY(
        ${PROJECT_NAME}
        ${mbedtls_ksdk_sources}
        ${mbedtls_sources}
        ${mbedtls_alt}
    )
ENDIF()

TARGET_INCLUDE_DIRECTORIES(${PROJECT_NAME} PUBLIC mbedtls/include)

IF(WithKSDK)
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
        TARGET_COMPILE_DEFINITIONS(${PROJECT_NAME} PUBLIC MBEDTLS_CONFIG_FILE=\"ksdk_mbedtls_config.h\")
    ENDIF()

    TARGET_LINK_LIBRARIES(
        ${PROJECT_NAME}
        board
        _mmcau
    )
ELSE() # KSDK
    IF(WithmbedTLS_ALT_A71CH)
        TARGET_COMPILE_DEFINITIONS(${PROJECT_NAME} PUBLIC MBEDTLS_CONFIG_FILE=\"a71ch_mbedtls_x86_config.h\")
        TARGET_INCLUDE_DIRECTORIES(${PROJECT_NAME}
            PUBLIC
            ${CMAKE_SOURCE_DIR}/hostlib/hostLib/mbedtls/inc
            ${CMAKE_SOURCE_DIR}/sss/inc
            ${CMAKE_SOURCE_DIR}/hostlib/hostLib/libCommon/infra
            ${CMAKE_SOURCE_DIR}/hostlib/hostLib/inc
            )
    ELSEIF(WithmbedTLS_ALT_SSS)
        TARGET_COMPILE_DEFINITIONS(${PROJECT_NAME} PUBLIC MBEDTLS_CONFIG_FILE=\"sss_mbedtls_x86_config.h\")
        TARGET_INCLUDE_DIRECTORIES(${PROJECT_NAME}
            PUBLIC
            ${CMAKE_SOURCE_DIR}/sss/plugin/mbedtls
            ${CMAKE_SOURCE_DIR}/sss/inc
            ${CMAKE_SOURCE_DIR}/hostlib/hostLib/libCommon/infra
            ${CMAKE_SOURCE_DIR}/hostlib/hostLib/inc
            )
    ENDIF()
ENDIF()

IF(
    "${CMAKE_C_COMPILER}"
    MATCHES
    ".*clang"
    OR "${CMAKE_CXX_COMPILER_ID}"
       STREQUAL
       "AppleClang"
)
    # MESSAGE(STATUS "-- No warning for mbedtls")
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
            PUBLIC /wd4127 # conditional expression is constant
            PRIVATE /wd4701 # potentially uninitialized local variable
            PRIVATE /wd4477 # 'printf' : format string '%d'
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
            PRIVATE -Wno-implicit-function-declaration
            PRIVATE -Wno-error=unused-const-variable
            PRIVATE -Wno-unused-const-variable
        )
    ENDIF()
ENDIF()
