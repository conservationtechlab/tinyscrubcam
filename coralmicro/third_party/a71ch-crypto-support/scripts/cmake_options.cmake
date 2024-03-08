# .. Copyright 2018,2020 NXP
# ..
# .. SPDX-License-Identifier: Apache-2.0
# ..

# .. _se05x-cmake-options:

# ============================================
#  CMake Options
# ============================================

# .. include:: cmake_options_values.rst.txt


#> Use NXP NFC RdLib. This is used mainly for RC663 + SAM Use Cases. Package available under NDA is needed to use this feature
OPTION(WithNXPNFCRdLib "Compile in NXP NFC RdLib support" OFF)

#> Compile with OPC UA.  By default it is disabled from compilation.
OPTION(WithOPCUA_open62541 "Compile With open62541 Support" OFF)

#> Create shared libraries.  Applicable for Engine DLL and other use cases.
OPTION(WithSharedLIB "Create and use shared libraries" OFF)

# NXP Internal Options
# ============================================
#
# These options are not supported outside NXP.
#
#> .. note:: For deliveries outside NXP, this option is disabled.
OPTION(NXPInternal "NXP Internal" OFF)

#> .. For internal testing.
OPTION(WithCodeCoverage "Compile with Code Coverage" OFF)


#.. Automatic generated options
INCLUDE(scripts/cmake_options_values.cmake)

# Other Variables
# ============================================


#> Include code from external folder. This way, experimental code can be included in build from outside the simw-top repository.
OPTION(WithExtCustomerCode "Include code from ../customer" OFF)

# .. option:: SIMW_INSTALL_INC_DIR
#
#     - Location where library header files are installed for linux based targets. (Used for iMX Linux)
#     - Default location is ``</usr/local/>include/se05x``
#
SET(SIMW_INSTALL_INC_DIR "include/se05x")

# .. option:: SIMW_INSTALL_SHARE_DIR
#
#     - Location where miscellaneous scripts
#       get copiled for linux based targets. (Used for iMX Linux)
#     - e.g. ``cmake_options.mak`` which has current cmake build settings.
#     - Default location is ``</usr/local/>share/se05x``
#
SET(SIMW_INSTALL_SHARE_DIR "share/se05x")

# .. # End of documented part .....

IF("${CMAKE_BUILD_TYPE}" STREQUAL "")
    SET(CMAKE_BUILD_TYPE "Debug")
ENDIF()

# .. # WithLargePrograms: Large enough programs that may
# .. # not fit in smaller size, e.g. small boot loaders.
IF(WithHost_lpcxpresso55s_s)
    SET(WithLargePrograms OFF)
ELSE()
    SET(WithLargePrograms ON)
ENDIF()

# .. To build shared libraries
IF(WithSharedLIB)
    SET(BUILD_SHARED_LIBS ON)
ELSE()
    SET(BUILD_SHARED_LIBS OFF)
ENDIF()

# .. If using NDA Version of WithNXPNFCRdLib
SET(WithNXPNFCRdLib_NDA ON) # .. else set it to off

IF(WithNXPNFCRdLib_NDA)
    SET(NXPNFCRdLib_DIRNAME nxpnfcrdlib_nda)
ELSE()
    SET(NXPNFCRdLib_DIRNAME nxpnfcrdlib_full)
ENDIF()

MESSAGE(STATUS "BUILD_TYPE: " ${CMAKE_BUILD_TYPE})

IF(WithHost_PCLinux32 OR WithHost_PCLinux64)
    SET(WithHost_PCLinux ON)
ELSE()
    SET(WithHost_PCLinux OFF)
ENDIF()

# .. for Legacy reasons, setting SSS_HAVE_OpenSSL_1_1_1 = ON
SET(SSS_HAVE_OpenSSL_1_1_1 ON)

IF(WithHost_PCLinux
   OR WithHost_Raspbian
   OR WithHost_iMXLinux
   OR WithHost_Cygwin
)
    SET(WithHost_LinuxLike ON)
ELSE()
    SET(WithHost_LinuxLike OFF)
ENDIF()

IF(
    WithHost_LinuxLike
    OR WithHost_PCWindows
    OR WithHost_Darwin
)
    SET(WithHost_WithFileSystem ON)
ELSE()
    SET(WithHost_WithFileSystem OFF)
ENDIF()

IF(WithHostCrypto_None)
    IF(WithSCP_SCP03_SSS
     OR SSSFTR_SW_AES
     OR SSSFTR_SW_ECC
     OR SSSFTR_SW_KEY_GET
     OR SSSFTR_SW_KEY_SET
     OR SSSFTR_SW_RSA
     OR SSSFTR_SW_TESTCOUNTERPART)
        MESSAGE(STATUS "Without Host Crypto Can not use any host/sw crypto")
    ENDIF()
ENDIF()


IF(WithHostCrypto_OPENSSL)
    IF(WithHost_PCLinux32)
        MESSAGE(STATUS "OpenSSL: Enforcing 32bit linux from /usr/lib/i386-linux-gnu")
        LINK_DIRECTORIES("/usr/lib/i386-linux-gnu")
        SET(OPENSSL_LIBRARIES ssl crypto)
    ENDIF()
    IF(WithHost_PCLinux64
       OR WithHost_iMXLinux
       OR WithHost_Raspbian
       OR WithHost_Cygwin
    )
        IF(OPENSSL_INSTALL_PREFIX)
            MESSAGE(STATUS "OpenSSL: Using: ${OPENSSL_INSTALL_PREFIX}")
            INCLUDE_DIRECTORIES("${OPENSSL_INSTALL_PREFIX}/include")
            SET(OPENSSL_LIBRARIES ${OPENSSL_INSTALL_PREFIX}/lib/libssl.so ${OPENSSL_INSTALL_PREFIX}/lib/libcrypto.so)
        ELSE()
            FIND_PACKAGE(OpenSSL) # Find the OpenSSL Package
            IF(OPENSSL_FOUND)
                MESSAGE(STATUS "Found: " ${OPENSSL_LIBRARIES})
                INCLUDE_DIRECTORIES(${OPENSSL_INCLUDE_DIR})
            ELSE()
                MESSAGE(WARNING "Building with OpenSSL Engine expected to fail")
            ENDIF()
        ENDIF()
    ELSEIF(WithHost_PCWindows)
        # Currently WithHost_PCWindows implies WithHostCrypto_MBEDTLS
        IF(SSS_HAVE_OpenSSL_1_1_1)
            INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/ext/openssl/include)
            IF(
                "${CMAKE_CXX_COMPILER_ID}"
                MATCHES
                "MSVC"
            )
                SET(OPENSSL_LIBRARIES
                    libssl
                    libcrypto
                    Crypt32
                    Ws2_32
                )
            ELSE()
                # MINGW
                SET(OPENSSL_LIBRARIES
                    ssl
                    crypto
                    Crypt32
                    Ws2_32
                )
            ENDIF()
        ENDIF()
        IF(SSS_HAVE_OpenSSL_1_0_2)
            INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/ext/openssl-102/include)
            SET(OPENSSL_LIBRARIES libeay32 ssleay32)
        ENDIF()
    ELSEIF(WithHost_Darwin)
        IF(OPENSSL_INSTALL_PREFIX)
            # /usr/local/Cellar/openssl/1.0.2s/
            MESSAGE(STATUS "OpenSSL: Using: ${OPENSSL_INSTALL_PREFIX}")
            INCLUDE_DIRECTORIES("${OPENSSL_INSTALL_PREFIX}/include")
            INCLUDE_DIRECTORIES("${OPENSSL_INSTALL_PREFIX}/lib")
            SET(OPENSSL_LIBRARIES ${OPENSSL_INSTALL_PREFIX}/lib/libssl.a ${OPENSSL_INSTALL_PREFIX}/lib/libcrypto.a)
        ELSE()
            FIND_PACKAGE(OpenSSL) # Find the OpenSSL Package
            IF(OPENSSL_FOUND)
                MESSAGE(STATUS "Found: " ${OPENSSL_LIBRARIES})
                INCLUDE_DIRECTORIES(${OPENSSL_INCLUDE_DIR})
            ELSE()
                MESSAGE(WARNING "Building with OpenSSL Engine expected to fail")
                MESSAGE(WARNING "You can set OPENSSL_INSTALL_PREFIX")
            ENDIF()
        ENDIF()
    ELSEIF(WithHost_Win10IoT)
        IF(SSS_HAVE_OpenSSL_1_0_2)
            INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/ext/openssl-102/Win10IoT/include)
            SET(OPENSSL_LIBRARIES libeay32 ssleay32)
        ELSE()
            MESSAGE(FATAL "WithHost_Win10IoT needs SSS_HAVE_OpenSSL_1_0_2")
        ENDIF()
    ENDIF()
ENDIF(WithHostCrypto_OPENSSL)

IF(WithmbedTLS_ALT_A71CH OR WithmbedTLS_ALT_SSS)
    SET(WithmbedTLS_ALT ON)
ELSE()
    SET(WithmbedTLS_ALT OFF)
ENDIF()

IF(WithApplet_A71CH_SIM OR WithApplet_SE050_L)
    SET(WithApplet_SE050_EAR ON)
ELSE()
    SET(WithApplet_SE050_EAR OFF)
ENDIF()

IF(WithApplet_SE05X_A
   OR WithApplet_SE05X_B
   OR WithApplet_SE05X_C
)
    SET(SSS_HAVE_APPLET_SE05X_IOT ON)
ELSE()
    SET(SSS_HAVE_APPLET_SE05X_IOT OFF)
ENDIF()

IF(WithRTOS_FreeRTOS)
    ADD_DEFINITIONS(-DHLSE_MAX_OBJECTS_IN_TABLE=9)
ENDIF()

IF(WithSCP_SCP03_HostCrypto)
    ADD_DEFINITIONS("-DUSE_HOSTCRYPTO_FOR_SCP03")
ENDIF()

IF(WithSCP02_HostCrypto)
    ADD_DEFINITIONS("-DUSE_SCP02")
ENDIF()

IF(WithApplet_A71CH OR WithApplet_A71CL)
    SET(WithApplet_A71XX ON)
ELSE()
    SET(WithApplet_A71XX OFF)
ENDIF()

IF(WithApplet_A71CH OR WithApplet_A71CH_SIM)
    ADD_DEFINITIONS("-DTGT_A71CH")
ENDIF()

IF(WithSMCOM_SCI2C)
    ADD_DEFINITIONS(-DSCI2C)
ENDIF()

IF(WithSMCOM_T1oI2C OR WithSMCOM_T1oI2C_GP1_0)
    ADD_DEFINITIONS(-DT1oI2C )
ENDIF()

IF(WithRTOS_FreeRTOS)
    ADD_DEFINITIONS(-DUSE_RTOS -DSDK_OS_FREE_RTOS -DFSL_RTOS_FREE_RTOS)
ENDIF()

IF(WithLog_SeggerRTT)
    ADD_DEFINITIONS(-DUSE_SERGER_RTT)
ENDIF()

IF(WithHost_frdmk64f)
    SET(WithKSDK ON)
    SET(KSDK_BoardName "frdmk64f")
    SET(KSDK_CPUName "MK64F12")
    ADD_DEFINITIONS(-DLPC_ENET) # .. Purnank : FIXME To be removed
ELSEIF(WithHost_evkmimxrt1060)
    SET(WithKSDK ON)
    SET(KSDK_BoardName "evkmimxrt1060")
    SET(KSDK_CPUName "MIMXRT1062")
    ADD_DEFINITIONS(-DLPC_ENET) # .. Purnank : FIXME To be removed
ELSEIF(WithHost_lpcxpresso55s)
    SET(WithKSDK ON)
    SET(KSDK_BoardName "lpcxpresso55s69")
    SET(KSDK_CPUName "LPC55S69")
    ADD_DEFINITIONS(-DLPC_WIFI)
    ADD_DEFINITIONS(-DWIFISHIELD_IS=WIFISHIELD_IS_SILEX2401)
ELSEIF(WithHost_lpcxpresso55s_ns)
    SET(WithKSDK ON)
    SET(KSDK_BoardName "lpcxpresso55s69")
    SET(KSDK_CPUName "LPC55S69")
    ADD_DEFINITIONS(-DLPC_WIFI)
    ADD_DEFINITIONS(-DWIFISHIELD_IS=WIFISHIELD_IS_SILEX2401)
ELSEIF(WithHost_lpcxpresso55s_s)
    SET(WithKSDK ON)
    SET(KSDK_BoardName "lpcxpresso55s69")
    SET(KSDK_CPUName "LPC55S69")
    ADD_DEFINITIONS(-DLPC_WIFI)
    ADD_DEFINITIONS(-DWIFISHIELD_IS=WIFISHIELD_IS_SILEX2401)
ELSE()
    SET(WithKSDK OFF)
ENDIF()

IF(WithSMCOM_JRCP_V2 AND WithSMCOM_JRCP_V1)
    MESSAGE(FATAL_ERROR "Can not select both WithSMCOM_JRCP_V2 AND WithSMCOM_JRCP_V1")
ENDIF()

IF(WithKSDK)
    INCLUDE(scripts/ksdk.cmake)
ELSEIF(
    WithHost_LinuxLike
    OR WithHost_PCWindows
    OR WithHost_Win10IoT
)
    INCLUDE(scripts/native.cmake)
ELSEIF(WithHost_Darwin)
    INCLUDE(scripts/native.cmake)
ELSEIF(WithHost_Android)
    INCLUDE(scripts/android.cmake)
ELSEIF("${CMAKE_SYSTEM_NAME}" STREQUAL "CYGWIN")
    INCLUDE(scripts/native.cmake)
ELSE()
    # .. Falling back on Native
    INCLUDE(scripts/native.cmake)
ENDIF()

MESSAGE(STATUS "CMAKE_CXX_COMPILER_ID = ${CMAKE_CXX_COMPILER_ID}")
MESSAGE(STATUS "CMAKE_SYSTEM_NAME = ${CMAKE_SYSTEM_NAME}")

IF(WithKSDK)
    INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/sss/port/ksdk)
ELSE()
    INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/sss/port/default)
ENDIF()

# .. Checks and balances ########

IF(NOT
   CMAKE_C_COMPILER_ID
   STREQUAL
   "GNU"
)
    IF(WithCodeCoverage)
        MESSAGE(FATAL_ERROR "Code coverage only with GCC")
    ENDIF()
ENDIF()

IF(WithRTOS_FreeRTOS AND WithRTOS_Default)
    MESSAGE(FATAL_ERROR "Can not set WithRTOS_FreeRTOS AND WithRTOS_Default")
ENDIF()

IF(WithSMCOM_JRCP_V2 AND WithSMCOM_JRCP_V1)
    MESSAGE(FATAL_ERROR "Can not select both WithSMCOM_JRCP_V2 AND WithSMCOM_JRCP_V1")
ENDIF()

IF(WithApplet_A71CH AND WithSMCOM_T1oI2C)
    MESSAGE(FATAL_ERROR "Can not set both 'WithApplet_A71CH' AND 'WithSMCOM_T1oI2C'")
ENDIF()

IF(WithSMCOM_RC663_VCOM)
    IF(NOT WithNXPNFCRdLib)
        MESSAGE(FATAL_ERROR "'WithSMCOM_RC663_VCOM' Needs 'WithNXPNFCRdLib'")
    ENDIF()
ENDIF()

IF(WithNXPNFCRdLib)
    IF(WithHost_PCLinux)
        MESSAGE(FATAL_ERROR "Can not set both 'WithNXPNFCRdLib' AND 'WithHost_PCLinux'")
    ENDIF()

    IF(NOT WithHost_lpcxpresso55s)
        MESSAGE(FATAL_ERROR " 'WithNXPNFCRdLib' Can only work with 'lpcxpresso55s'")
    ENDIF()

ENDIF()

IF(WithmbedTLS_ALT)
    IF(WithApplet_A71CL OR WithApplet_SE050_L)
        MESSAGE(FATAL_ERROR "Can not set both 'ALT ' AND 'CL'")
    ENDIF()
ENDIF()

IF(WithKSDK)
    IF(WithHostCrypto_OPENSSL)
        MESSAGE(FATAL_ERROR "Can not set both 'KSDK ' AND 'OPENSSL'")
    ENDIF()
ENDIF()

IF(WithApplet_A71CL OR WithApplet_SE050_L)
    IF(WithSCP_SCP03_SSS OR WithSCP_SCP03_HostCrypto)
        MESSAGE(FATAL_ERROR "Can not use SCP03 for CL")
    ENDIF()
ENDIF()

IF(WithApplet_A71CH AND WithApplet_A71CH_SIM)
    IF(WithSCP02_HostCrypto)
        MESSAGE(FATAL_ERROR "Can not use SCP02 for CH")
    ENDIF()
ENDIF()


IF(SSS_HAVE_Applet_None)
    IF(NOT WithmbedTLS_ALT_None)
        MESSAGE(FATAL_ERROR "Can not use SW Crypto with mebdTLS ALT")
    ENDIF()
ENDIF()

IF(SSS_HAVE_APPLET_SE05X_IOT)
    IF(WithSMCOM_SCI2C)
        MESSAGE(FATAL_ERROR "Can not use SCI2C with SE05X")
    ENDIF()
ENDIF()

IF(SSS_HAVE_APPLET_SE05X_IOT)
    # .. Every one must not know what is the current auth matchanism.
    # .. Only the logic which is opening or testing it knows this.
    # .. This way, we ensure at run time, the host library is able
    # .. to use selected auth strategy.
    MACRO(EXPOSE_SE05X_AUTH_STRATEGY)
        SET(NEED_AUTH OFF)
        SET(NEED_CRYPTO OFF)
        SET(NEED_ECKey OFF)
        IF("${SE05X_Auth}" STREQUAL "None")
            # OK
        ELSEIF("${SE05X_Auth}" STREQUAL "PlatfSCP03")
            SET(NEED_CRYPTO ON)
        ELSEIF("${SE05X_Auth}" STREQUAL "UserID")
            SET(NEED_AUTH ON)
        ELSEIF("${SE05X_Auth}" STREQUAL "AESKey")
            SET(NEED_AUTH ON)
            SET(NEED_CRYPTO ON)
        ELSEIF("${SE05X_Auth}" STREQUAL "ECKey")
            SET(NEED_AUTH ON)
            SET(NEED_CRYPTO ON)
            SET(NEED_ECKey ON)
        ELSEIF("${SE05X_Auth}" STREQUAL "UserID_PlatfSCP03")
            SET(NEED_AUTH ON)
            SET(NEED_CRYPTO ON)
        ELSEIF("${SE05X_Auth}" STREQUAL "AESKey_PlatfSCP03")
            SET(NEED_AUTH ON)
            SET(NEED_CRYPTO ON)
        ELSEIF("${SE05X_Auth}" STREQUAL "ECKey_PlatfSCP03")
            SET(NEED_AUTH ON)
            SET(NEED_CRYPTO ON)
            SET(NEED_ECKey ON)
        ELSE()
            MESSAGE(SEND_ERROR "For 'SE05X_Auth' '${SE05X_Auth}' is not suppored.")
        ENDIF()
        IF(${NEED_AUTH})
            IF(NOT ${SSSFTR_SE05X_AuthSession})
                MESSAGE(SEND_ERROR "For 'SE05X_Auth=${SE05X_Auth}' 'SSSFTR_SE05X_AuthSession' Should be ON")
            ENDIF()
        ENDIF()
        IF(${NEED_CRYPTO})
            IF(NOT ${WithSCP_SCP03_SSS})
                MESSAGE(SEND_ERROR "For 'SE05X_Auth=${SE05X_Auth}' 'WithSCP_SCP03_SSS' Should be ON")
            ENDIF()
            IF(NOT ${SSSFTR_SW_AES})
                MESSAGE(SEND_ERROR "For 'SE05X_Auth=${SE05X_Auth}' 'SSSFTR_SW_AES' Should be ON")
            ENDIF()
        ENDIF()
        IF(${NEED_ECKey})
            IF(NOT ${SSSFTR_SE05X_AuthECKey})
                MESSAGE(SEND_ERROR "For 'SE05X_Auth=${SE05X_Auth}' 'SSSFTR_SE05X_AuthECKey' Should be ON")
            ENDIF()
            IF(NOT ${SSSFTR_SW_ECC})
                MESSAGE(SEND_ERROR "For 'SE05X_Auth=${SE05X_Auth}' 'SSSFTR_SW_ECC' Should be ON")
            ENDIF()
        ENDIF()
    ENDMACRO()

    MESSAGE(STATUS "SE05X_Auth - ${SE05X_Auth}")
ELSE(SSS_HAVE_APPLET_SE05X_IOT)
    MACRO(EXPOSE_SE05X_AUTH_STRATEGY)

    ENDMACRO()
ENDIF(SSS_HAVE_APPLET_SE05X_IOT)

IF(WithApplet_A71CH OR WithApplet_A71CH_SIM)
    IF(NOT("${SE05X_Auth}" STREQUAL "None"))
        MESSAGE(FATAL_ERROR "Cannot use this combination for A71CH")
    ENDIF()
ENDIF()

IF(WithHost_frdmk64f OR WithHost_lpcxpresso55s_ns OR WithHost_lpcxpresso55s_s OR WithHost_lpcxpresso55s)
    MESSAGE(FATAL_ERROR "frdmk64f and lpcxpresso55s Hosts not supported.")
ENDIF()

INCLUDE(scripts/cmake_options_check.cmake)


# .. By default, we don't have it
SET(SSS_HAVE_ECC 0)
SET(SSS_HAVE_RSA 0)
# .. SET(SSS_HAVE_FIPS
# ..     OFF
# ..     CACHE BOOL "Compile Tests with/without FIPS Mode aware"
# .. )
SET(SSS_HAVE_TPM_BN 0) #TPM BARRETO_NAEHRIG Curve .
SET(SSS_HAVE_EC_ED 0) #Edwards Curve.
SET(SSS_HAVE_EC_MONT 0) #Montgomery Curve.
SET(SSS_HAVE_TLS_HANDSHAKE 0) #TLS Handshake.
SET(SSS_HAVE_IMPORT 0) #TLS IMPORT.

# .. Based on variant, we enable and expose that feature.
IF(WithApplet_A71CH OR WithApplet_A71CH_SIM)
    SET(SSS_HAVE_ECC 1)
ENDIF()

IF(WithApplet_SE05X_A)
    SET(SSS_HAVE_ECC 1)
ENDIF()

IF(WithApplet_SE05X_B)
    SET(SSS_HAVE_RSA 1)
ENDIF()

IF(WithApplet_SE05X_C)
    SET(SSS_HAVE_ECC 1)
    SET(SSS_HAVE_RSA 1)
    SET(SSS_HAVE_EC_ED 1)
    SET(SSS_HAVE_TPM_BN 1)
    SET(SSS_HAVE_EC_MONT 1)
    SET(SSS_HAVE_TLS_HANDSHAKE 1)
    SET(SSS_HAVE_IMPORT 1)
ENDIF()

IF(WithHostCrypto_OPENSSL)
    SET(SSS_HAVE_EC_MONT 1)
    SET(SSS_HAVE_EC_ED 1)
ENDIF()

IF(WithHostCrypto_MBEDTLS)
    SET(SSS_HAVE_EC_MONT 1)
ENDIF()

IF(WithApplet_None)
    SET(SSS_HAVE_RSA 1)
    SET(SSS_HAVE_ECC 1)
ENDIF()

IF(WithNXPNFCRdLib)
    SET(SSS_HAVE_NXPNFCRDLIB 1)
ELSE()
    SET(SSS_HAVE_NXPNFCRDLIB 0)
ENDIF()

IF(WithKSDK)
    SET(NFC_663_SPI ON)
    SET(NFC_663_VCOM OFF)
ELSE()
    SET(NFC_663_SPI OFF)
    SET(NFC_663_VCOM ON)
ENDIF()

IF(NXPInternal)
   SET(NXPProprietary ON)
ELSE()
   SET(NXPProprietary OFF)
ENDIF()