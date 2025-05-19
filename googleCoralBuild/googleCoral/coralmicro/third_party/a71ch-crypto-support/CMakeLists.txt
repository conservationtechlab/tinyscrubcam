# Copyright 2018,2020 NXP
#
# SPDX-License-Identifier: Apache-2.0
#

CMAKE_MINIMUM_REQUIRED(VERSION 3.5.0)

IF(
    ${CMAKE_SOURCE_DIR}
    STREQUAL
    ${CMAKE_BINARY_DIR}
)
    MESSAGE("\nUSAGE_ERROR:\n\n\tRun with cmake -S <SOURCE_DIR> -B <BUILD_DIR>\n")
    MESSAGE(
        FATAL_ERROR
            "In-source builds not allowed. Please make a new directory and run CMake from there.\n!!!IMPORTANT!!! Remove CMakeCache.txt and CMakeFiles!"
    )
ENDIF()

PROJECT(PlugAndTrustMW)

INCLUDE(scripts/cmake_options.cmake)

SET(CMAKE_MODULE_PATH "./scripts")

INCLUDE_DIRECTORIES(${CMAKE_BINARY_DIR})
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

CONFIGURE_FILE(sss/inc/fsl_sss_ftr.h.in fsl_sss_ftr.h)
CONFIGURE_FILE(scripts/cmake_options.mak.in cmake_options.mak)
CONFIGURE_FILE(scripts/cmake_options_installed.cmake.in cmake_options_installed.cmake)

IF(NXPInternal)
    CONFIGURE_FILE(sss/tst/inc/pt_tst_ftr.h.in pt_tst_ftr.h)
ENDIF()

INSTALL(FILES ${CMAKE_BINARY_DIR}/fsl_sss_ftr.h DESTINATION ${SIMW_INSTALL_INC_DIR})
INSTALL(FILES ${CMAKE_BINARY_DIR}/cmake_options.mak DESTINATION ${SIMW_INSTALL_SHARE_DIR})
INSTALL(FILES ${CMAKE_BINARY_DIR}/cmake_options_installed.cmake DESTINATION ${SIMW_INSTALL_SHARE_DIR})

# EXT and DOC should not be dependent on any compile time variables/depednencies
# ESPECIALLY, SSS_USE_FTR_FILE and FLOW_VERBOSE

ADD_DEFINITIONS(-DSSS_USE_FTR_FILE)
ADD_DEFINITIONS(-DmqttconfigENABLE_METRICS=0)

IF(WithLog_Silent)
    MESSAGE(STATUS "FLOW_SILENT is ON!!!")
    ADD_DEFINITIONS(-DFLOW_SILENT)
ENDIF()

ADD_SUBDIRECTORY(ext)

IF(NXPInternal)
    IF(
        "${CMAKE_CXX_COMPILER_ID}"
        STREQUAL
        "MSVC"
    )
        # Macros won't work for MSVC
    ELSE()
        ADD_SUBDIRECTORY(exhaledocs)
    ENDIF()
ENDIF()

IF(WithLog_Verbose)
    MESSAGE(STATUS "FLOW_VERBOSE is ON!!!")
    ADD_DEFINITIONS(-DFLOW_VERBOSE)
ENDIF()

IF(WithCodeCoverage)
    IF(CMAKE_COMPILER_IS_GNUCXX)
        INCLUDE(scripts/CodeCoverage.cmake)
        APPEND_COVERAGE_COMPILER_FLAGS()
    ENDIF()
ENDIF()

ADD_SUBDIRECTORY(hostlib)

ADD_SUBDIRECTORY(sss)

IF(NXPProprietary)
    IF(NOT SSS_HAVE_HOST_LPCXPRESSO55S_NS)
        IF(SSS_HAVE_SE05X_VER_GTE_06_00)
            ADD_SUBDIRECTORY(semslite)
        ENDIF()
    ENDIF()
ENDIF()

IF(WithExtCustomerCode)
    add_subdirectory(../customer extcust)
ENDIF()

ADD_SUBDIRECTORY(demos)

IF(NXPProprietary)
    IF(WithHostCrypto_OPENSSL)
        IF(WithApplet_SE05X)
            IF(WithHost_PCWindows AND "${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
                # NOTE-PGH: 2020.01.32 SimpleServer.exe does not compile
            ELSE()
                ADD_SUBDIRECTORY(nxp_iot_agent)
            ENDIF()
        ENDIF()
    ELSEIF(WithHostCrypto_MBEDTLS AND WithmbedTLS_ALT_SSS AND WithApplet_SE05X)
        # Build for FreeRTOS
        IF(WithHost_PCWindows OR WithRTOS_FreeRTOS)
            # Build for Windows or Windows 10 IoT or RTOS
            ADD_SUBDIRECTORY(nxp_iot_agent)
        ENDIF()
    ENDIF()
ENDIF()

IF(WithHostCrypto_MBEDCRYPTO)
    IF(
        "${CMAKE_CXX_COMPILER_ID}"
        STREQUAL
        "MSVC"
    )
        SET(MBEDTLS_DIR ${CMAKE_SOURCE_DIR}/ext/mbed-crypto)
        SET(USE_CRYPTO_SUBMODULE "1")
    ENDIF()
ENDIF()

IF(NXPProprietary)
    IF(WithApplet_SE05X)
        ADD_SUBDIRECTORY(nxp_iot_agent/src/common)
    ENDIF()
ENDIF()
