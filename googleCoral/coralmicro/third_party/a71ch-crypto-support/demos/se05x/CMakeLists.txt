#
# Copyright 2019,2020 NXP
# SPDX-License-Identifier: Apache-2.0
#
#

IF(WithRTOS_Default)
    ADD_SUBDIRECTORY(se05x_ConcurrentEcc)
    ADD_SUBDIRECTORY(se05x_ConcurrentSymm)

    IF(NOT WithHost_Win10IoT)
        ADD_SUBDIRECTORY(se05x_Minimal)
    ENDIF()

    IF(WithHost_WithFileSystem)
        ADD_SUBDIRECTORY(se05x_export)
        ADD_SUBDIRECTORY(se05x_import)
        IF(SSS_HAVE_HOSTCRYPTO_ANY)
            IF(SSS_HAVE_SE05X_Auth_ECKey)
                ADD_SUBDIRECTORY(se05x_ImportExternalObjectPrepare)
            ENDIF()
        ENDIF(SSS_HAVE_HOSTCRYPTO_ANY)
    ENDIF()

    IF(NOT WithKSDK)
        ADD_SUBDIRECTORY(se05x_Personalization)
        IF(CMAKE_COMPILER_IS_GNUCC)
            ADD_SUBDIRECTORY(se05x_MultiThread)
        ENDIF()
    ENDIF()

    IF(SSSFTR_SE05X_AuthSession AND SSS_HAVE_HOSTCRYPTO_ANY)
        IF(SSS_HAVE_SE05X_Auth_None OR SSS_HAVE_SE05X_Auth_PlatfSCP03)
            ADD_SUBDIRECTORY(se05x_Delete_and_test_provision)
            ADD_SUBDIRECTORY(se05x_MandatePlatformSCP)
        ELSE()
            MESSAGE(STATUS "se05x_Delete_and_test_provision is not built because SE05X_Auth is '${SE05X_Auth}'.")
        ENDIF()
    ENDIF(SSSFTR_SE05X_AuthSession AND SSS_HAVE_HOSTCRYPTO_ANY)

    IF(SSS_HAVE_SE05X_Auth_PlatfSCP03)
        ADD_SUBDIRECTORY(se05x_RotatePlatformSCP03Keys)
    ENDIF()

    IF(SSS_HAVE_SE05X_VER_GTE_06_00)
        ADD_SUBDIRECTORY(se05x_Perso_Delete_Mod_RSAKeyGen)
        IF(WithHost_WithFileSystem)
            IF(SSS_HAVE_HOSTCRYPTO_ANY)
                IF(SSS_HAVE_SE05X_Auth_ECKey OR SSS_HAVE_SE05X_Auth_ECKey_PlatfSCP03)
                    ADD_SUBDIRECTORY(se05x_DesfireCRC)
                ENDIF()
            ENDIF(SSS_HAVE_HOSTCRYPTO_ANY)
        ENDIF()
    ENDIF()

    IF(SSSFTR_SE05X_AuthSession AND SSS_HAVE_HOSTCRYPTO_ANY)
        IF(SSS_HAVE_SE05X_Auth_None OR SSS_HAVE_SE05X_Auth_PlatfSCP03)
            ADD_SUBDIRECTORY(se05x_TransportLock)
            ADD_SUBDIRECTORY(se05x_TransportUnLock)
        ENDIF()
    ENDIF(SSSFTR_SE05X_AuthSession AND SSS_HAVE_HOSTCRYPTO_ANY)

    IF(NOT WithHost_Win10IoT)
        ADD_SUBDIRECTORY(ex_se05x_WiFiKDF)
    ENDIF()

    ADD_SUBDIRECTORY(se05x_GetInfo)
    ADD_SUBDIRECTORY(se05x_PCR)
    ADD_SUBDIRECTORY(se05x_I2cMaster)

    IF(SSS_HAVE_SE05X_VER_GTE_06_00)
        ADD_SUBDIRECTORY(se05x_ReadState)
    ENDIF()

    IF(SSS_HAVE_ECC)
        ADD_SUBDIRECTORY(se05x_policy)
    ENDIF()

    IF(SSS_HAVE_HOSTCRYPTO_ANY)
        ADD_SUBDIRECTORY(se05x_ReadWithAttestation)
    ENDIF()

    IF(SSS_HAVE_SCP_None)
        ADD_SUBDIRECTORY(se05x_GetAPDUBuffer)
    ENDIF()
    IF(SSS_HAVE_HOSTCRYPTO_ANY)
        ADD_SUBDIRECTORY(se05x_TimeStamp)
    ENDIF()
    ADD_SUBDIRECTORY(se05x_InjectCertificate)

    IF(SSS_HAVE_SE05X_Auth_ECKey OR SSS_HAVE_SE05X_Auth_ECKey_PlatfSCP03)
        ADD_SUBDIRECTORY(se05x_SetAppletFeatures)
        # ADD_SUBDIRECTORY(se05x_DesfireCRC)
    ENDIF()

    ADD_SUBDIRECTORY(se05x_InvokeGarbageCollection)
ENDIF()

IF(WithRTOS_FreeRTOS)
    ADD_SUBDIRECTORY(se05x_Minimal)
    IF(WithKSDK)
        ADD_SUBDIRECTORY(se05x_MultiThread)
    ENDIF()
ENDIF()

IF(WithHostCrypto_OPENSSL)
    ADD_SUBDIRECTORY(certificate_demo)
    ADD_SUBDIRECTORY(seTool)
ENDIF()

IF(WithHostCrypto_MBEDTLS)
    ADD_SUBDIRECTORY(se05x_GetCertificate)
ENDIF()

IF(NXPProprietary)
    IF(WithHostCrypto_MBEDCRYPTO)
        IF(WithHost_lpcxpresso55s)
            ADD_SUBDIRECTORY(tfm_example)
        ENDIF()
        IF(WithHost_lpcxpresso55s_ns)
            ADD_SUBDIRECTORY(psa_nonsecure)
        ENDIF()
    ENDIF()
ENDIF()

IF(WithRTOS_FreeRTOS AND WithmbedTLS_ALT_SSS)
    ADD_SUBDIRECTORY(pkcs11)
ENDIF()
