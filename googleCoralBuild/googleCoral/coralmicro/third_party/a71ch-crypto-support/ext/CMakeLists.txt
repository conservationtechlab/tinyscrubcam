# Copyright 2019,2020 NXP
#
# SPDX-License-Identifier: Apache-2.0
#

ADD_SUBDIRECTORY(unity)

IF(WithSMCOM_JRCP_V2 OR WithSMCOM_VCOM)
    IF(NXPProprietary)
        ADD_SUBDIRECTORY(jrcplib)
    ENDIF()
ENDIF()

IF(WithHostCrypto_OPENSSL)
    INCLUDE(paho.cmake)
    IF(NOT WithHost_LinuxLike)
        ADD_SUBDIRECTORY(openssl)
    ENDIF()
ENDIF()


IF(WithHostCrypto_MBEDTLS)
    INCLUDE(mbedtls.cmake)
ENDIF()

IF(WithRTOS_FreeRTOS)
    INCLUDE(amazon-freertos.cmake)
ENDIF()

IF(WithApplet_A71CL OR WithApplet_SE050_L)
    IF(WithHostCrypto_MBEDTLS)
        ADD_SUBDIRECTORY(id2_client_sdk)
    ENDIF()
ENDIF()

IF(WithSMCOM_VCOM)
    IF("Windows" STREQUAL "${CMAKE_SYSTEM_NAME}")
        ADD_SUBDIRECTORY(JRCPProxyConsole)
    ENDIF()
ENDIF()

IF(WithLog_SeggerRTT)
    ADD_SUBDIRECTORY(seggerrtt)
ENDIF()

IF(WithSMCOM_PN7150)
    ADD_SUBDIRECTORY(PN7150Nfc)
ENDIF()

IF(WithNXPNFCRdLib)
    INCLUDE(nxpnfcrdlib.cmake)
ENDIF()

IF(WithOPCUA_open62541)
    IF(WithHostCrypto_MBEDTLS AND (WithHost_PCWindows OR WithHost_iMXLinux OR WithHost_PCLinux64 OR WithHost_Raspbian))
        INCLUDE(open62541.cmake)
    ENDIF()
ENDIF()

IF(WithHostCrypto_MBEDCRYPTO)
    INCLUDE(mbed-crypto.cmake)
ENDIF()

