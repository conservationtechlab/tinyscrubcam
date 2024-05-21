#
# Copyright 2019,2020 NXP
# SPDX-License-Identifier: Apache-2.0
#
#

IF(
    WithApplet_A71CH_SIM
    OR WithApplet_A71XX
    OR WithApplet_SE05X
    OR WithApplet_SE050_L
    OR WithApplet_LoopBack
    OR WithKSDK
)
    IF(NXPProprietary)
        ADD_SUBDIRECTORY(ksdk)
    ENDIF()
ENDIF()

IF(NXPInternal AND (NOT SSS_HAVE_HOST_LPCXPRESSO55S_NS))
    ADD_SUBDIRECTORY(aws)
ENDIF()

IF(WithApplet_SE05X)
    IF(WithLargePrograms) # Skip larger programs. e.g.
        ADD_SUBDIRECTORY(se05x)
        IF(NXPInternal)
            ADD_SUBDIRECTORY(se05x_nxp)
        ENDIF()
    ENDIF()
ENDIF()

IF(WithApplet_A71CH OR WithApplet_A71CH_SIM)
    ADD_SUBDIRECTORY(a71ch)
ENDIF()

IF(NXPProprietary)
    IF(WithNXPNFCRdLib)
        ADD_SUBDIRECTORY(nxpnfcrdlib)
    ENDIF()
ENDIF()

IF(WithHost_WithFileSystem)
    IF(WithApplet_SE05X AND (SSS_HAVE_SE05X_Auth_None OR SSS_HAVE_SE05X_Auth_PlatfSCP03))
        ADD_SUBDIRECTORY(apdu_player)
    ENDIF()
ENDIF()

IF(WithHost_lpcxpresso55s_ns OR WithHost_lpcxpresso55s_s OR WithHost_lpcxpresso55s)
    ADD_SUBDIRECTORY(lpc55s)
ENDIF()

IF(WithHostCrypto_MBEDTLS)
    IF(WithRTOS_Default AND WithHost_LinuxLike)
        ADD_SUBDIRECTORY(linux/sss_pkcs11)
    ENDIF()
ENDIF()

IF(WithOPCUA_open62541)
    IF(
        WithHostCrypto_MBEDTLS
        AND
            (WithHost_PCWindows
             OR WithHost_iMXLinux
             OR WithHost_PCLinux64
             OR WithHost_Raspbian
            )
    )
        ADD_SUBDIRECTORY(opc_ua)
    ENDIF()
ENDIF()

IF(WithHostCrypto_None)
    # PASS
ELSE()
    IF(NOT SSS_HAVE_HOST_LPCXPRESSO55S_NS)
        ADD_SUBDIRECTORY(test_Crypto)
    ENDIF(NOT SSS_HAVE_HOST_LPCXPRESSO55S_NS)
ENDIF()
