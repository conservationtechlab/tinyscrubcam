#
# Copyright 2018-2020 NXP
# SPDX-License-Identifier: Apache-2.0
#
# #############################################################
# This file is generated using a script
# #############################################################
#

SET(
    Applet
    "SE05X_C"
    CACHE
        STRING
        "The Secure Element Applet"
)

IF(NXPInternal)
    SET_PROPERTY(
        CACHE Applet
        PROPERTY
            STRINGS
            "None;A71CH;A71CL;A71CH_SIM;SE05X_A;SE05X_B;SE05X_C;SE05X_L;LoopBack;"
    )
ELSE()
    SET_PROPERTY(
        CACHE Applet
        PROPERTY
            STRINGS
            "None;A71CH;SE05X_A;SE05X_B;SE05X_C;"
    )
ENDIF()

SET(
    SE05X_Ver
    "03_XX"
    CACHE
        STRING
        "SE05X Applet version."
)

IF(NXPInternal)
    SET_PROPERTY(
        CACHE SE05X_Ver
        PROPERTY
            STRINGS
            "03_XX;06_00;"
    )
ELSE()
    SET_PROPERTY(
        CACHE SE05X_Ver
        PROPERTY
            STRINGS
            "03_XX;06_00;"
    )
ENDIF()

SET(
    Host
    "PCWindows"
    CACHE
        STRING
        "Host where the software stack is running"
)

IF(NXPInternal)
    SET_PROPERTY(
        CACHE Host
        PROPERTY
            STRINGS
            "Darwin;PCLinux32;PCLinux64;PCWindows;Cygwin;frdmk64f;evkmimxrt1060;lpcxpresso55s;lpcxpresso55s_ns;lpcxpresso55s_s;iMXLinux;Raspbian;Android;Win10IoT;"
    )
ELSE()
    SET_PROPERTY(
        CACHE Host
        PROPERTY
            STRINGS
            "Darwin;PCLinux32;PCLinux64;PCWindows;Cygwin;frdmk64f;evkmimxrt1060;lpcxpresso55s;lpcxpresso55s_ns;lpcxpresso55s_s;iMXLinux;Raspbian;Android;"
    )
ENDIF()

SET(
    SMCOM
    "VCOM"
    CACHE
        STRING
        "Communication Interface"
)

IF(NXPInternal)
    SET_PROPERTY(
        CACHE SMCOM
        PROPERTY
            STRINGS
            "None;JRCP_V2;JRCP_V1;VCOM;SCI2C;T1oI2C;T1oI2C_GP1_0;RC663_VCOM;PN7150;Thread;PCSC;"
    )
ELSE()
    SET_PROPERTY(
        CACHE SMCOM
        PROPERTY
            STRINGS
            "None;JRCP_V2;JRCP_V1;VCOM;SCI2C;T1oI2C;PCSC;"
    )
ENDIF()

SET(
    HostCrypto
    "MBEDTLS"
    CACHE
        STRING
        "Counterpart Crypto on Host"
)

IF(NXPInternal)
    SET_PROPERTY(
        CACHE HostCrypto
        PROPERTY
            STRINGS
            "MBEDTLS;MBEDCRYPTO;OPENSSL;User;None;"
    )
ELSE()
    SET_PROPERTY(
        CACHE HostCrypto
        PROPERTY
            STRINGS
            "MBEDTLS;MBEDCRYPTO;OPENSSL;User;None;"
    )
ENDIF()

SET(
    RTOS
    "Default"
    CACHE
        STRING
        "Choice of Operating system"
)

IF(NXPInternal)
    SET_PROPERTY(
        CACHE RTOS
        PROPERTY
            STRINGS
            "Default;FreeRTOS;"
    )
ELSE()
    SET_PROPERTY(
        CACHE RTOS
        PROPERTY
            STRINGS
            "Default;FreeRTOS;"
    )
ENDIF()

SET(
    mbedTLS_ALT
    "None"
    CACHE
        STRING
        "ALT Engine implementation for mbedTLS"
)

IF(NXPInternal)
    SET_PROPERTY(
        CACHE mbedTLS_ALT
        PROPERTY
            STRINGS
            "SSS;A71CH;None;"
    )
ELSE()
    SET_PROPERTY(
        CACHE mbedTLS_ALT
        PROPERTY
            STRINGS
            "SSS;A71CH;None;"
    )
ENDIF()

SET(
    SCP
    "None"
    CACHE
        STRING
        "Secure Channel Protocol"
)

IF(NXPInternal)
    SET_PROPERTY(
        CACHE SCP
        PROPERTY
            STRINGS
            "None;SCP03_SSS;SCP03_HostCrypto;"
    )
ELSE()
    SET_PROPERTY(
        CACHE SCP
        PROPERTY
            STRINGS
            "None;SCP03_SSS;SCP03_HostCrypto;"
    )
ENDIF()

SET(
    FIPS
    "None"
    CACHE
        STRING
        "Enable or disable FIPS "
)

IF(NXPInternal)
    SET_PROPERTY(
        CACHE FIPS
        PROPERTY
            STRINGS
            "None;SE050;140_2;140_3;"
    )
ELSE()
    SET_PROPERTY(
        CACHE FIPS
        PROPERTY
            STRINGS
            "None;SE050;"
    )
ENDIF()

SET(
    SE05X_Auth
    "None"
    CACHE
        STRING
        "SE050 Authentication"
)

IF(NXPInternal)
    SET_PROPERTY(
        CACHE SE05X_Auth
        PROPERTY
            STRINGS
            "None;UserID;PlatfSCP03;AESKey;ECKey;UserID_PlatfSCP03;AESKey_PlatfSCP03;ECKey_PlatfSCP03;"
    )
ELSE()
    SET_PROPERTY(
        CACHE SE05X_Auth
        PROPERTY
            STRINGS
            "None;UserID;PlatfSCP03;AESKey;ECKey;UserID_PlatfSCP03;AESKey_PlatfSCP03;ECKey_PlatfSCP03;"
    )
ENDIF()

SET(
    A71CH_AUTH
    "None"
    CACHE
        STRING
        "A71CH Authentication"
)

IF(NXPInternal)
    SET_PROPERTY(
        CACHE A71CH_AUTH
        PROPERTY
            STRINGS
            "None;SCP03;"
    )
ELSE()
    SET_PROPERTY(
        CACHE A71CH_AUTH
        PROPERTY
            STRINGS
            "None;SCP03;"
    )
ENDIF()

SET(
    Log
    "Default"
    CACHE
        STRING
        "Logging"
)

IF(NXPInternal)
    SET_PROPERTY(
        CACHE Log
        PROPERTY
            STRINGS
            "Default;Verbose;Silent;SeggerRTT;"
    )
ELSE()
    SET_PROPERTY(
        CACHE Log
        PROPERTY
            STRINGS
            "Default;Verbose;Silent;"
    )
ENDIF()

SET(
    CMAKE_BUILD_TYPE
    "Debug"
    CACHE
        STRING
        "See https://cmake.org/cmake/help/latest/variable/CMAKE_BUILD_TYPE.html"
)

IF(NXPInternal)
    SET_PROPERTY(
        CACHE CMAKE_BUILD_TYPE
        PROPERTY
            STRINGS
            "Debug;Release;RelWithDebInfo;;"
    )
ELSE()
    SET_PROPERTY(
        CACHE CMAKE_BUILD_TYPE
        PROPERTY
            STRINGS
            "Debug;Release;RelWithDebInfo;;"
    )
ENDIF()
IF("${Applet}" STREQUAL "None")
    SET(WithApplet_None ON)
    SET(SSS_HAVE_APPLET_NONE "1")
ELSE()
    SET(WithApplet_None OFF)
    SET(SSS_HAVE_APPLET_NONE "0")
ENDIF()

IF("${Applet}" STREQUAL "A71CH")
    SET(WithApplet_A71CH ON)
    SET(SSS_HAVE_APPLET_A71CH "1")
ELSE()
    SET(WithApplet_A71CH OFF)
    SET(SSS_HAVE_APPLET_A71CH "0")
ENDIF()

IF("${Applet}" STREQUAL "A71CL")
    SET(WithApplet_A71CL ON)
    SET(SSS_HAVE_APPLET_A71CL "1")
ELSE()
    SET(WithApplet_A71CL OFF)
    SET(SSS_HAVE_APPLET_A71CL "0")
ENDIF()

IF("${Applet}" STREQUAL "A71CH_SIM")
    SET(WithApplet_A71CH_SIM ON)
    SET(SSS_HAVE_APPLET_A71CH_SIM "1")
ELSE()
    SET(WithApplet_A71CH_SIM OFF)
    SET(SSS_HAVE_APPLET_A71CH_SIM "0")
ENDIF()

IF("${Applet}" STREQUAL "SE05X_A")
    SET(WithApplet_SE05X_A ON)
    SET(SSS_HAVE_APPLET_SE05X_A "1")
ELSE()
    SET(WithApplet_SE05X_A OFF)
    SET(SSS_HAVE_APPLET_SE05X_A "0")
ENDIF()

IF("${Applet}" STREQUAL "SE05X_B")
    SET(WithApplet_SE05X_B ON)
    SET(SSS_HAVE_APPLET_SE05X_B "1")
ELSE()
    SET(WithApplet_SE05X_B OFF)
    SET(SSS_HAVE_APPLET_SE05X_B "0")
ENDIF()

IF("${Applet}" STREQUAL "SE05X_C")
    SET(WithApplet_SE05X_C ON)
    SET(SSS_HAVE_APPLET_SE05X_C "1")
ELSE()
    SET(WithApplet_SE05X_C OFF)
    SET(SSS_HAVE_APPLET_SE05X_C "0")
ENDIF()

IF("${Applet}" STREQUAL "SE05X_L")
    SET(WithApplet_SE05X_L ON)
    SET(SSS_HAVE_APPLET_SE05X_L "1")
ELSE()
    SET(WithApplet_SE05X_L OFF)
    SET(SSS_HAVE_APPLET_SE05X_L "0")
ENDIF()

IF("${Applet}" STREQUAL "LoopBack")
    SET(WithApplet_LoopBack ON)
    SET(SSS_HAVE_APPLET_LOOPBACK "1")
ELSE()
    SET(WithApplet_LoopBack OFF)
    SET(SSS_HAVE_APPLET_LOOPBACK "0")
ENDIF()

IF("${Applet}" STREQUAL "None")
    # OK
ELSEIF("${Applet}" STREQUAL "A71CH")
    # OK
ELSEIF("${Applet}" STREQUAL "A71CL")
    # OK
ELSEIF("${Applet}" STREQUAL "A71CH_SIM")
    # OK
ELSEIF("${Applet}" STREQUAL "SE05X_A")
    # OK
ELSEIF("${Applet}" STREQUAL "SE05X_B")
    # OK
ELSEIF("${Applet}" STREQUAL "SE05X_C")
    # OK
ELSEIF("${Applet}" STREQUAL "SE05X_L")
    # OK
ELSEIF("${Applet}" STREQUAL "LoopBack")
    # OK
ELSE()
    MESSAGE(SEND_ERROR "For 'Applet' '${Applet}' is invalid.")
    MESSAGE(STATUS "Only supported values are 'None, A71CH, A71CL, A71CH_SIM, SE05X_A, SE05X_B, SE05X_C, SE05X_L, LoopBack'")
ENDIF()

IF("${SE05X_Ver}" STREQUAL "03_XX")
    SET(WithSE05X_Ver_03_XX ON)
    SET(SSS_HAVE_SE05X_VER_03_XX "1")
ELSE()
    SET(WithSE05X_Ver_03_XX OFF)
    SET(SSS_HAVE_SE05X_VER_03_XX "0")
ENDIF()

IF("${SE05X_Ver}" STREQUAL "06_00")
    SET(WithSE05X_Ver_06_00 ON)
    SET(SSS_HAVE_SE05X_VER_06_00 "1")
ELSE()
    SET(WithSE05X_Ver_06_00 OFF)
    SET(SSS_HAVE_SE05X_VER_06_00 "0")
ENDIF()

IF("${SE05X_Ver}" STREQUAL "03_XX")
    # OK
ELSEIF("${SE05X_Ver}" STREQUAL "06_00")
    # OK
ELSE()
    MESSAGE(SEND_ERROR "For 'SE05X_Ver' '${SE05X_Ver}' is invalid.")
    MESSAGE(STATUS "Only supported values are '03_XX, 06_00'")
ENDIF()

IF("${Host}" STREQUAL "Darwin")
    SET(WithHost_Darwin ON)
    SET(SSS_HAVE_HOST_DARWIN "1")
ELSE()
    SET(WithHost_Darwin OFF)
    SET(SSS_HAVE_HOST_DARWIN "0")
ENDIF()

IF("${Host}" STREQUAL "PCLinux32")
    SET(WithHost_PCLinux32 ON)
    SET(SSS_HAVE_HOST_PCLINUX32 "1")
ELSE()
    SET(WithHost_PCLinux32 OFF)
    SET(SSS_HAVE_HOST_PCLINUX32 "0")
ENDIF()

IF("${Host}" STREQUAL "PCLinux64")
    SET(WithHost_PCLinux64 ON)
    SET(SSS_HAVE_HOST_PCLINUX64 "1")
ELSE()
    SET(WithHost_PCLinux64 OFF)
    SET(SSS_HAVE_HOST_PCLINUX64 "0")
ENDIF()

IF("${Host}" STREQUAL "PCWindows")
    SET(WithHost_PCWindows ON)
    SET(SSS_HAVE_HOST_PCWINDOWS "1")
ELSE()
    SET(WithHost_PCWindows OFF)
    SET(SSS_HAVE_HOST_PCWINDOWS "0")
ENDIF()

IF("${Host}" STREQUAL "Cygwin")
    SET(WithHost_Cygwin ON)
    SET(SSS_HAVE_HOST_CYGWIN "1")
ELSE()
    SET(WithHost_Cygwin OFF)
    SET(SSS_HAVE_HOST_CYGWIN "0")
ENDIF()

IF("${Host}" STREQUAL "frdmk64f")
    SET(WithHost_frdmk64f ON)
    SET(SSS_HAVE_HOST_FRDMK64F "1")
ELSE()
    SET(WithHost_frdmk64f OFF)
    SET(SSS_HAVE_HOST_FRDMK64F "0")
ENDIF()

IF("${Host}" STREQUAL "evkmimxrt1060")
    SET(WithHost_evkmimxrt1060 ON)
    SET(SSS_HAVE_HOST_EVKMIMXRT1060 "1")
ELSE()
    SET(WithHost_evkmimxrt1060 OFF)
    SET(SSS_HAVE_HOST_EVKMIMXRT1060 "0")
ENDIF()

IF("${Host}" STREQUAL "lpcxpresso55s")
    SET(WithHost_lpcxpresso55s ON)
    SET(SSS_HAVE_HOST_LPCXPRESSO55S "1")
ELSE()
    SET(WithHost_lpcxpresso55s OFF)
    SET(SSS_HAVE_HOST_LPCXPRESSO55S "0")
ENDIF()

IF("${Host}" STREQUAL "lpcxpresso55s_ns")
    SET(WithHost_lpcxpresso55s_ns ON)
    SET(SSS_HAVE_HOST_LPCXPRESSO55S_NS "1")
ELSE()
    SET(WithHost_lpcxpresso55s_ns OFF)
    SET(SSS_HAVE_HOST_LPCXPRESSO55S_NS "0")
ENDIF()

IF("${Host}" STREQUAL "lpcxpresso55s_s")
    SET(WithHost_lpcxpresso55s_s ON)
    SET(SSS_HAVE_HOST_LPCXPRESSO55S_S "1")
ELSE()
    SET(WithHost_lpcxpresso55s_s OFF)
    SET(SSS_HAVE_HOST_LPCXPRESSO55S_S "0")
ENDIF()

IF("${Host}" STREQUAL "iMXLinux")
    SET(WithHost_iMXLinux ON)
    SET(SSS_HAVE_HOST_IMXLINUX "1")
ELSE()
    SET(WithHost_iMXLinux OFF)
    SET(SSS_HAVE_HOST_IMXLINUX "0")
ENDIF()

IF("${Host}" STREQUAL "Raspbian")
    SET(WithHost_Raspbian ON)
    SET(SSS_HAVE_HOST_RASPBIAN "1")
ELSE()
    SET(WithHost_Raspbian OFF)
    SET(SSS_HAVE_HOST_RASPBIAN "0")
ENDIF()

IF("${Host}" STREQUAL "Android")
    SET(WithHost_Android ON)
    SET(SSS_HAVE_HOST_ANDROID "1")
ELSE()
    SET(WithHost_Android OFF)
    SET(SSS_HAVE_HOST_ANDROID "0")
ENDIF()

IF("${Host}" STREQUAL "Win10IoT")
    SET(WithHost_Win10IoT ON)
    SET(SSS_HAVE_HOST_WIN10IOT "1")
ELSE()
    SET(WithHost_Win10IoT OFF)
    SET(SSS_HAVE_HOST_WIN10IOT "0")
ENDIF()

IF("${Host}" STREQUAL "Darwin")
    # OK
ELSEIF("${Host}" STREQUAL "PCLinux32")
    # OK
ELSEIF("${Host}" STREQUAL "PCLinux64")
    # OK
ELSEIF("${Host}" STREQUAL "PCWindows")
    # OK
ELSEIF("${Host}" STREQUAL "Cygwin")
    # OK
ELSEIF("${Host}" STREQUAL "frdmk64f")
    # OK
ELSEIF("${Host}" STREQUAL "evkmimxrt1060")
    # OK
ELSEIF("${Host}" STREQUAL "lpcxpresso55s")
    # OK
ELSEIF("${Host}" STREQUAL "lpcxpresso55s_ns")
    # OK
ELSEIF("${Host}" STREQUAL "lpcxpresso55s_s")
    # OK
ELSEIF("${Host}" STREQUAL "iMXLinux")
    # OK
ELSEIF("${Host}" STREQUAL "Raspbian")
    # OK
ELSEIF("${Host}" STREQUAL "Android")
    # OK
ELSEIF("${Host}" STREQUAL "Win10IoT")
    # OK
ELSE()
    MESSAGE(SEND_ERROR "For 'Host' '${Host}' is invalid.")
    MESSAGE(STATUS "Only supported values are 'Darwin, PCLinux32, PCLinux64, PCWindows, Cygwin, frdmk64f, evkmimxrt1060, lpcxpresso55s, lpcxpresso55s_ns, lpcxpresso55s_s, iMXLinux, Raspbian, Android, Win10IoT'")
ENDIF()

IF("${SMCOM}" STREQUAL "None")
    SET(WithSMCOM_None ON)
    SET(SSS_HAVE_SMCOM_NONE "1")
ELSE()
    SET(WithSMCOM_None OFF)
    SET(SSS_HAVE_SMCOM_NONE "0")
ENDIF()

IF("${SMCOM}" STREQUAL "JRCP_V2")
    SET(WithSMCOM_JRCP_V2 ON)
    SET(SSS_HAVE_SMCOM_JRCP_V2 "1")
ELSE()
    SET(WithSMCOM_JRCP_V2 OFF)
    SET(SSS_HAVE_SMCOM_JRCP_V2 "0")
ENDIF()

IF("${SMCOM}" STREQUAL "JRCP_V1")
    SET(WithSMCOM_JRCP_V1 ON)
    SET(SSS_HAVE_SMCOM_JRCP_V1 "1")
ELSE()
    SET(WithSMCOM_JRCP_V1 OFF)
    SET(SSS_HAVE_SMCOM_JRCP_V1 "0")
ENDIF()

IF("${SMCOM}" STREQUAL "VCOM")
    SET(WithSMCOM_VCOM ON)
    SET(SSS_HAVE_SMCOM_VCOM "1")
ELSE()
    SET(WithSMCOM_VCOM OFF)
    SET(SSS_HAVE_SMCOM_VCOM "0")
ENDIF()

IF("${SMCOM}" STREQUAL "SCI2C")
    SET(WithSMCOM_SCI2C ON)
    SET(SSS_HAVE_SMCOM_SCI2C "1")
ELSE()
    SET(WithSMCOM_SCI2C OFF)
    SET(SSS_HAVE_SMCOM_SCI2C "0")
ENDIF()

IF("${SMCOM}" STREQUAL "T1oI2C")
    SET(WithSMCOM_T1oI2C ON)
    SET(SSS_HAVE_SMCOM_T1OI2C "1")
ELSE()
    SET(WithSMCOM_T1oI2C OFF)
    SET(SSS_HAVE_SMCOM_T1OI2C "0")
ENDIF()

IF("${SMCOM}" STREQUAL "T1oI2C_GP1_0")
    SET(WithSMCOM_T1oI2C_GP1_0 ON)
    SET(SSS_HAVE_SMCOM_T1OI2C_GP1_0 "1")
ELSE()
    SET(WithSMCOM_T1oI2C_GP1_0 OFF)
    SET(SSS_HAVE_SMCOM_T1OI2C_GP1_0 "0")
ENDIF()

IF("${SMCOM}" STREQUAL "RC663_VCOM")
    SET(WithSMCOM_RC663_VCOM ON)
    SET(SSS_HAVE_SMCOM_RC663_VCOM "1")
ELSE()
    SET(WithSMCOM_RC663_VCOM OFF)
    SET(SSS_HAVE_SMCOM_RC663_VCOM "0")
ENDIF()

IF("${SMCOM}" STREQUAL "PN7150")
    SET(WithSMCOM_PN7150 ON)
    SET(SSS_HAVE_SMCOM_PN7150 "1")
ELSE()
    SET(WithSMCOM_PN7150 OFF)
    SET(SSS_HAVE_SMCOM_PN7150 "0")
ENDIF()

IF("${SMCOM}" STREQUAL "Thread")
    SET(WithSMCOM_Thread ON)
    SET(SSS_HAVE_SMCOM_THREAD "1")
ELSE()
    SET(WithSMCOM_Thread OFF)
    SET(SSS_HAVE_SMCOM_THREAD "0")
ENDIF()

IF("${SMCOM}" STREQUAL "PCSC")
    SET(WithSMCOM_PCSC ON)
    SET(SSS_HAVE_SMCOM_PCSC "1")
ELSE()
    SET(WithSMCOM_PCSC OFF)
    SET(SSS_HAVE_SMCOM_PCSC "0")
ENDIF()

IF("${SMCOM}" STREQUAL "None")
    # OK
ELSEIF("${SMCOM}" STREQUAL "JRCP_V2")
    # OK
ELSEIF("${SMCOM}" STREQUAL "JRCP_V1")
    # OK
ELSEIF("${SMCOM}" STREQUAL "VCOM")
    # OK
ELSEIF("${SMCOM}" STREQUAL "SCI2C")
    # OK
ELSEIF("${SMCOM}" STREQUAL "T1oI2C")
    # OK
ELSEIF("${SMCOM}" STREQUAL "T1oI2C_GP1_0")
    # OK
ELSEIF("${SMCOM}" STREQUAL "RC663_VCOM")
    # OK
ELSEIF("${SMCOM}" STREQUAL "PN7150")
    # OK
ELSEIF("${SMCOM}" STREQUAL "Thread")
    # OK
ELSEIF("${SMCOM}" STREQUAL "PCSC")
    # OK
ELSE()
    MESSAGE(SEND_ERROR "For 'SMCOM' '${SMCOM}' is invalid.")
    MESSAGE(STATUS "Only supported values are 'None, JRCP_V2, JRCP_V1, VCOM, SCI2C, T1oI2C, T1oI2C_GP1_0, RC663_VCOM, PN7150, Thread, PCSC'")
ENDIF()

IF("${HostCrypto}" STREQUAL "MBEDTLS")
    SET(WithHostCrypto_MBEDTLS ON)
    SET(SSS_HAVE_HOSTCRYPTO_MBEDTLS "1")
ELSE()
    SET(WithHostCrypto_MBEDTLS OFF)
    SET(SSS_HAVE_HOSTCRYPTO_MBEDTLS "0")
ENDIF()

IF("${HostCrypto}" STREQUAL "MBEDCRYPTO")
    SET(WithHostCrypto_MBEDCRYPTO ON)
    SET(SSS_HAVE_HOSTCRYPTO_MBEDCRYPTO "1")
ELSE()
    SET(WithHostCrypto_MBEDCRYPTO OFF)
    SET(SSS_HAVE_HOSTCRYPTO_MBEDCRYPTO "0")
ENDIF()

IF("${HostCrypto}" STREQUAL "OPENSSL")
    SET(WithHostCrypto_OPENSSL ON)
    SET(SSS_HAVE_HOSTCRYPTO_OPENSSL "1")
ELSE()
    SET(WithHostCrypto_OPENSSL OFF)
    SET(SSS_HAVE_HOSTCRYPTO_OPENSSL "0")
ENDIF()

IF("${HostCrypto}" STREQUAL "User")
    SET(WithHostCrypto_User ON)
    SET(SSS_HAVE_HOSTCRYPTO_USER "1")
ELSE()
    SET(WithHostCrypto_User OFF)
    SET(SSS_HAVE_HOSTCRYPTO_USER "0")
ENDIF()

IF("${HostCrypto}" STREQUAL "None")
    SET(WithHostCrypto_None ON)
    SET(SSS_HAVE_HOSTCRYPTO_NONE "1")
ELSE()
    SET(WithHostCrypto_None OFF)
    SET(SSS_HAVE_HOSTCRYPTO_NONE "0")
ENDIF()

IF("${HostCrypto}" STREQUAL "MBEDTLS")
    # OK
ELSEIF("${HostCrypto}" STREQUAL "MBEDCRYPTO")
    # OK
ELSEIF("${HostCrypto}" STREQUAL "OPENSSL")
    # OK
ELSEIF("${HostCrypto}" STREQUAL "User")
    # OK
ELSEIF("${HostCrypto}" STREQUAL "None")
    # OK
ELSE()
    MESSAGE(SEND_ERROR "For 'HostCrypto' '${HostCrypto}' is invalid.")
    MESSAGE(STATUS "Only supported values are 'MBEDTLS, MBEDCRYPTO, OPENSSL, User, None'")
ENDIF()

IF("${RTOS}" STREQUAL "Default")
    SET(WithRTOS_Default ON)
    SET(SSS_HAVE_RTOS_DEFAULT "1")
ELSE()
    SET(WithRTOS_Default OFF)
    SET(SSS_HAVE_RTOS_DEFAULT "0")
ENDIF()

IF("${RTOS}" STREQUAL "FreeRTOS")
    SET(WithRTOS_FreeRTOS ON)
    SET(SSS_HAVE_RTOS_FREERTOS "1")
ELSE()
    SET(WithRTOS_FreeRTOS OFF)
    SET(SSS_HAVE_RTOS_FREERTOS "0")
ENDIF()

IF("${RTOS}" STREQUAL "Default")
    # OK
ELSEIF("${RTOS}" STREQUAL "FreeRTOS")
    # OK
ELSE()
    MESSAGE(SEND_ERROR "For 'RTOS' '${RTOS}' is invalid.")
    MESSAGE(STATUS "Only supported values are 'Default, FreeRTOS'")
ENDIF()

IF("${mbedTLS_ALT}" STREQUAL "SSS")
    SET(WithmbedTLS_ALT_SSS ON)
    SET(SSS_HAVE_MBEDTLS_ALT_SSS "1")
ELSE()
    SET(WithmbedTLS_ALT_SSS OFF)
    SET(SSS_HAVE_MBEDTLS_ALT_SSS "0")
ENDIF()

IF("${mbedTLS_ALT}" STREQUAL "A71CH")
    SET(WithmbedTLS_ALT_A71CH ON)
    SET(SSS_HAVE_MBEDTLS_ALT_A71CH "1")
ELSE()
    SET(WithmbedTLS_ALT_A71CH OFF)
    SET(SSS_HAVE_MBEDTLS_ALT_A71CH "0")
ENDIF()

IF("${mbedTLS_ALT}" STREQUAL "None")
    SET(WithmbedTLS_ALT_None ON)
    SET(SSS_HAVE_MBEDTLS_ALT_NONE "1")
ELSE()
    SET(WithmbedTLS_ALT_None OFF)
    SET(SSS_HAVE_MBEDTLS_ALT_NONE "0")
ENDIF()

IF("${mbedTLS_ALT}" STREQUAL "SSS")
    # OK
ELSEIF("${mbedTLS_ALT}" STREQUAL "A71CH")
    # OK
ELSEIF("${mbedTLS_ALT}" STREQUAL "None")
    # OK
ELSE()
    MESSAGE(SEND_ERROR "For 'mbedTLS_ALT' '${mbedTLS_ALT}' is invalid.")
    MESSAGE(STATUS "Only supported values are 'SSS, A71CH, None'")
ENDIF()

IF("${SCP}" STREQUAL "None")
    SET(WithSCP_None ON)
    SET(SSS_HAVE_SCP_NONE "1")
ELSE()
    SET(WithSCP_None OFF)
    SET(SSS_HAVE_SCP_NONE "0")
ENDIF()

IF("${SCP}" STREQUAL "SCP03_SSS")
    SET(WithSCP_SCP03_SSS ON)
    SET(SSS_HAVE_SCP_SCP03_SSS "1")
ELSE()
    SET(WithSCP_SCP03_SSS OFF)
    SET(SSS_HAVE_SCP_SCP03_SSS "0")
ENDIF()

IF("${SCP}" STREQUAL "SCP03_HostCrypto")
    SET(WithSCP_SCP03_HostCrypto ON)
    SET(SSS_HAVE_SCP_SCP03_HOSTCRYPTO "1")
ELSE()
    SET(WithSCP_SCP03_HostCrypto OFF)
    SET(SSS_HAVE_SCP_SCP03_HOSTCRYPTO "0")
ENDIF()

IF("${SCP}" STREQUAL "None")
    # OK
ELSEIF("${SCP}" STREQUAL "SCP03_SSS")
    # OK
ELSEIF("${SCP}" STREQUAL "SCP03_HostCrypto")
    # OK
ELSE()
    MESSAGE(SEND_ERROR "For 'SCP' '${SCP}' is invalid.")
    MESSAGE(STATUS "Only supported values are 'None, SCP03_SSS, SCP03_HostCrypto'")
ENDIF()

IF("${FIPS}" STREQUAL "None")
    SET(WithFIPS_None ON)
    SET(SSS_HAVE_FIPS_NONE "1")
ELSE()
    SET(WithFIPS_None OFF)
    SET(SSS_HAVE_FIPS_NONE "0")
ENDIF()

IF("${FIPS}" STREQUAL "SE050")
    SET(WithFIPS_SE050 ON)
    SET(SSS_HAVE_FIPS_SE050 "1")
ELSE()
    SET(WithFIPS_SE050 OFF)
    SET(SSS_HAVE_FIPS_SE050 "0")
ENDIF()

IF("${FIPS}" STREQUAL "140_2")
    SET(WithFIPS_140_2 ON)
    SET(SSS_HAVE_FIPS_140_2 "1")
ELSE()
    SET(WithFIPS_140_2 OFF)
    SET(SSS_HAVE_FIPS_140_2 "0")
ENDIF()

IF("${FIPS}" STREQUAL "140_3")
    SET(WithFIPS_140_3 ON)
    SET(SSS_HAVE_FIPS_140_3 "1")
ELSE()
    SET(WithFIPS_140_3 OFF)
    SET(SSS_HAVE_FIPS_140_3 "0")
ENDIF()

IF("${FIPS}" STREQUAL "None")
    # OK
ELSEIF("${FIPS}" STREQUAL "SE050")
    # OK
ELSEIF("${FIPS}" STREQUAL "140_2")
    # OK
ELSEIF("${FIPS}" STREQUAL "140_3")
    # OK
ELSE()
    MESSAGE(SEND_ERROR "For 'FIPS' '${FIPS}' is invalid.")
    MESSAGE(STATUS "Only supported values are 'None, SE050, 140_2, 140_3'")
ENDIF()

IF("${SE05X_Auth}" STREQUAL "None")
    SET(WithSE05X_Auth_None ON)
    SET(SSS_HAVE_SE05X_AUTH_NONE "1")
ELSE()
    SET(WithSE05X_Auth_None OFF)
    SET(SSS_HAVE_SE05X_AUTH_NONE "0")
ENDIF()

IF("${SE05X_Auth}" STREQUAL "UserID")
    SET(WithSE05X_Auth_UserID ON)
    SET(SSS_HAVE_SE05X_AUTH_USERID "1")
ELSE()
    SET(WithSE05X_Auth_UserID OFF)
    SET(SSS_HAVE_SE05X_AUTH_USERID "0")
ENDIF()

IF("${SE05X_Auth}" STREQUAL "PlatfSCP03")
    SET(WithSE05X_Auth_PlatfSCP03 ON)
    SET(SSS_HAVE_SE05X_AUTH_PLATFSCP03 "1")
ELSE()
    SET(WithSE05X_Auth_PlatfSCP03 OFF)
    SET(SSS_HAVE_SE05X_AUTH_PLATFSCP03 "0")
ENDIF()

IF("${SE05X_Auth}" STREQUAL "AESKey")
    SET(WithSE05X_Auth_AESKey ON)
    SET(SSS_HAVE_SE05X_AUTH_AESKEY "1")
ELSE()
    SET(WithSE05X_Auth_AESKey OFF)
    SET(SSS_HAVE_SE05X_AUTH_AESKEY "0")
ENDIF()

IF("${SE05X_Auth}" STREQUAL "ECKey")
    SET(WithSE05X_Auth_ECKey ON)
    SET(SSS_HAVE_SE05X_AUTH_ECKEY "1")
ELSE()
    SET(WithSE05X_Auth_ECKey OFF)
    SET(SSS_HAVE_SE05X_AUTH_ECKEY "0")
ENDIF()

IF("${SE05X_Auth}" STREQUAL "UserID_PlatfSCP03")
    SET(WithSE05X_Auth_UserID_PlatfSCP03 ON)
    SET(SSS_HAVE_SE05X_AUTH_USERID_PLATFSCP03 "1")
ELSE()
    SET(WithSE05X_Auth_UserID_PlatfSCP03 OFF)
    SET(SSS_HAVE_SE05X_AUTH_USERID_PLATFSCP03 "0")
ENDIF()

IF("${SE05X_Auth}" STREQUAL "AESKey_PlatfSCP03")
    SET(WithSE05X_Auth_AESKey_PlatfSCP03 ON)
    SET(SSS_HAVE_SE05X_AUTH_AESKEY_PLATFSCP03 "1")
ELSE()
    SET(WithSE05X_Auth_AESKey_PlatfSCP03 OFF)
    SET(SSS_HAVE_SE05X_AUTH_AESKEY_PLATFSCP03 "0")
ENDIF()

IF("${SE05X_Auth}" STREQUAL "ECKey_PlatfSCP03")
    SET(WithSE05X_Auth_ECKey_PlatfSCP03 ON)
    SET(SSS_HAVE_SE05X_AUTH_ECKEY_PLATFSCP03 "1")
ELSE()
    SET(WithSE05X_Auth_ECKey_PlatfSCP03 OFF)
    SET(SSS_HAVE_SE05X_AUTH_ECKEY_PLATFSCP03 "0")
ENDIF()

IF("${SE05X_Auth}" STREQUAL "None")
    # OK
ELSEIF("${SE05X_Auth}" STREQUAL "UserID")
    # OK
ELSEIF("${SE05X_Auth}" STREQUAL "PlatfSCP03")
    # OK
ELSEIF("${SE05X_Auth}" STREQUAL "AESKey")
    # OK
ELSEIF("${SE05X_Auth}" STREQUAL "ECKey")
    # OK
ELSEIF("${SE05X_Auth}" STREQUAL "UserID_PlatfSCP03")
    # OK
ELSEIF("${SE05X_Auth}" STREQUAL "AESKey_PlatfSCP03")
    # OK
ELSEIF("${SE05X_Auth}" STREQUAL "ECKey_PlatfSCP03")
    # OK
ELSE()
    MESSAGE(SEND_ERROR "For 'SE05X_Auth' '${SE05X_Auth}' is invalid.")
    MESSAGE(STATUS "Only supported values are 'None, UserID, PlatfSCP03, AESKey, ECKey, UserID_PlatfSCP03, AESKey_PlatfSCP03, ECKey_PlatfSCP03'")
ENDIF()

IF("${A71CH_AUTH}" STREQUAL "None")
    SET(WithA71CH_AUTH_None ON)
    SET(SSS_HAVE_A71CH_AUTH_NONE "1")
ELSE()
    SET(WithA71CH_AUTH_None OFF)
    SET(SSS_HAVE_A71CH_AUTH_NONE "0")
ENDIF()

IF("${A71CH_AUTH}" STREQUAL "SCP03")
    SET(WithA71CH_AUTH_SCP03 ON)
    SET(SSS_HAVE_A71CH_AUTH_SCP03 "1")
ELSE()
    SET(WithA71CH_AUTH_SCP03 OFF)
    SET(SSS_HAVE_A71CH_AUTH_SCP03 "0")
ENDIF()

IF("${A71CH_AUTH}" STREQUAL "None")
    # OK
ELSEIF("${A71CH_AUTH}" STREQUAL "SCP03")
    # OK
ELSE()
    MESSAGE(SEND_ERROR "For 'A71CH_AUTH' '${A71CH_AUTH}' is invalid.")
    MESSAGE(STATUS "Only supported values are 'None, SCP03'")
ENDIF()

IF("${Log}" STREQUAL "Default")
    SET(WithLog_Default ON)
    SET(SSS_HAVE_LOG_DEFAULT "1")
ELSE()
    SET(WithLog_Default OFF)
    SET(SSS_HAVE_LOG_DEFAULT "0")
ENDIF()

IF("${Log}" STREQUAL "Verbose")
    SET(WithLog_Verbose ON)
    SET(SSS_HAVE_LOG_VERBOSE "1")
ELSE()
    SET(WithLog_Verbose OFF)
    SET(SSS_HAVE_LOG_VERBOSE "0")
ENDIF()

IF("${Log}" STREQUAL "Silent")
    SET(WithLog_Silent ON)
    SET(SSS_HAVE_LOG_SILENT "1")
ELSE()
    SET(WithLog_Silent OFF)
    SET(SSS_HAVE_LOG_SILENT "0")
ENDIF()

IF("${Log}" STREQUAL "SeggerRTT")
    SET(WithLog_SeggerRTT ON)
    SET(SSS_HAVE_LOG_SEGGERRTT "1")
ELSE()
    SET(WithLog_SeggerRTT OFF)
    SET(SSS_HAVE_LOG_SEGGERRTT "0")
ENDIF()

IF("${Log}" STREQUAL "Default")
    # OK
ELSEIF("${Log}" STREQUAL "Verbose")
    # OK
ELSEIF("${Log}" STREQUAL "Silent")
    # OK
ELSEIF("${Log}" STREQUAL "SeggerRTT")
    # OK
ELSE()
    MESSAGE(SEND_ERROR "For 'Log' '${Log}' is invalid.")
    MESSAGE(STATUS "Only supported values are 'Default, Verbose, Silent, SeggerRTT'")
ENDIF()

IF("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
    SET(WithCMAKE_BUILD_TYPE_Debug ON)
    SET(SSS_HAVE_CMAKE_BUILD_TYPE_DEBUG "1")
ELSE()
    SET(WithCMAKE_BUILD_TYPE_Debug OFF)
    SET(SSS_HAVE_CMAKE_BUILD_TYPE_DEBUG "0")
ENDIF()

IF("${CMAKE_BUILD_TYPE}" STREQUAL "Release")
    SET(WithCMAKE_BUILD_TYPE_Release ON)
    SET(SSS_HAVE_CMAKE_BUILD_TYPE_RELEASE "1")
ELSE()
    SET(WithCMAKE_BUILD_TYPE_Release OFF)
    SET(SSS_HAVE_CMAKE_BUILD_TYPE_RELEASE "0")
ENDIF()

IF("${CMAKE_BUILD_TYPE}" STREQUAL "RelWithDebInfo")
    SET(WithCMAKE_BUILD_TYPE_RelWithDebInfo ON)
    SET(SSS_HAVE_CMAKE_BUILD_TYPE_RELWITHDEBINFO "1")
ELSE()
    SET(WithCMAKE_BUILD_TYPE_RelWithDebInfo OFF)
    SET(SSS_HAVE_CMAKE_BUILD_TYPE_RELWITHDEBINFO "0")
ENDIF()

IF("${CMAKE_BUILD_TYPE}" STREQUAL "")
    SET(WithCMAKE_BUILD_TYPE_ ON)
    SET(SSS_HAVE_CMAKE_BUILD_TYPE_ "1")
ELSE()
    SET(WithCMAKE_BUILD_TYPE_ OFF)
    SET(SSS_HAVE_CMAKE_BUILD_TYPE_ "0")
ENDIF()

IF("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
    # OK
ELSEIF("${CMAKE_BUILD_TYPE}" STREQUAL "Release")
    # OK
ELSEIF("${CMAKE_BUILD_TYPE}" STREQUAL "RelWithDebInfo")
    # OK
ELSEIF("${CMAKE_BUILD_TYPE}" STREQUAL "")
    # OK
ELSE()
    MESSAGE(SEND_ERROR "For 'CMAKE_BUILD_TYPE' '${CMAKE_BUILD_TYPE}' is invalid.")
    MESSAGE(STATUS "Only supported values are 'Debug, Release, RelWithDebInfo, '")
ENDIF()


# Add Options
# SE05X
SET(SSSFTR_SE05X_AES ON
    CACHE BOOL
    "SE05X Secure Element : Symmetric AES")
SET(SSSFTR_SE05X_ECC ON
    CACHE BOOL
    "SE05X Secure Element : Elliptic Curve Cryptography")
SET(SSSFTR_SE05X_RSA ON
    CACHE BOOL
    "SE05X Secure Element : RSA")
SET(SSSFTR_SE05X_KEY_SET ON
    CACHE BOOL
    "SE05X Secure Element : KEY operations : SET Key")
SET(SSSFTR_SE05X_KEY_GET ON
    CACHE BOOL
    "SE05X Secure Element : KEY operations : GET Key")
SET(SSSFTR_SE05X_AuthECKey ON
    CACHE BOOL
    "SE05X Secure Element : Authenticate via ECKey")
SET(SSSFTR_SE05X_AuthSession ON
    CACHE BOOL
    "SE05X Secure Element : Allow creation of user/authenticated session.")
SET(SSSFTR_SE05X_CREATE_DELETE_CRYPTOOBJ ON
    CACHE BOOL
    "SE05X Secure Element : Allow creation/deletion of Crypto Objects")
# SW
SET(SSSFTR_SW_AES ON
    CACHE BOOL
    "Software : Symmetric AES")
SET(SSSFTR_SW_ECC ON
    CACHE BOOL
    "Software : Elliptic Curve Cryptography")
SET(SSSFTR_SW_RSA ON
    CACHE BOOL
    "Software : RSA")
SET(SSSFTR_SW_KEY_SET ON
    CACHE BOOL
    "Software : KEY operations : SET Key")
SET(SSSFTR_SW_KEY_GET ON
    CACHE BOOL
    "Software : KEY operations : GET Key")
SET(SSSFTR_SW_TESTCOUNTERPART ON
    CACHE BOOL
    "Software : Used as a test counterpart")

# Add Values
# Symmetric AES
IF(SSSFTR_SE05X_AES OR SSSFTR_SW_AES)
    SET(SSSFTR_AES ON)
ELSE()
    SET(SSSFTR_AES OFF)
ENDIF()
# Elliptic Curve Cryptography
IF(SSSFTR_SE05X_ECC OR SSSFTR_SW_ECC)
    SET(SSSFTR_ECC ON)
ELSE()
    SET(SSSFTR_ECC OFF)
ENDIF()
# RSA
IF(SSSFTR_SE05X_RSA OR SSSFTR_SW_RSA)
    SET(SSSFTR_RSA ON)
ELSE()
    SET(SSSFTR_RSA OFF)
ENDIF()
# KEY operations : SET Key
IF(SSSFTR_SE05X_KEY_SET OR SSSFTR_SW_KEY_SET)
    SET(SSSFTR_KEY_SET ON)
ELSE()
    SET(SSSFTR_KEY_SET OFF)
ENDIF()
# KEY operations : GET Key
IF(SSSFTR_SE05X_KEY_GET OR SSSFTR_SW_KEY_GET)
    SET(SSSFTR_KEY_GET ON)
ELSE()
    SET(SSSFTR_KEY_GET OFF)
ENDIF()
# KEY operations
IF(SSSFTR_KEY_SET OR SSSFTR_KEY_GET)
    SET(SSSFTR_KEY ON)
ELSE()
    SET(SSSFTR_KEY OFF)
ENDIF()
# KEY operations
IF(SSSFTR_SE05X_KEY_SET OR SSSFTR_SE05X_KEY_GET)
    SET(SSSFTR_SE05X_KEY ON)
ELSE()
    SET(SSSFTR_SE05X_KEY OFF)
ENDIF()
# KEY operations
IF(SSSFTR_SW_KEY_SET OR SSSFTR_SW_KEY_GET)
    SET(SSSFTR_SW_KEY ON)
ELSE()
    SET(SSSFTR_SW_KEY OFF)
ENDIF()
