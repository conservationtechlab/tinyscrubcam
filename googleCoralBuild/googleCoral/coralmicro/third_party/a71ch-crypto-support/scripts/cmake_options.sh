#
# Copyright 2018-2020 NXP
# SPDX-License-Identifier: Apache-2.0
#
# #############################################################
# This file is generated using a script
# #############################################################
#


### Applet : The Secure Element Applet
#
# You can compile host library for different Applets listed below.
# Please note, some of these Applets may be for NXP Internal use only.
doApplet_None_ON="-DApplet=None" #Compiling without any Applet Support
doApplet_A71CH_ON="-DApplet=A71CH" #A71CH (ECC)
doApplet_A71CL_ON="-DApplet=A71CL" #A71CL (RSA)
doApplet_A71CH_SIM_ON="-DApplet=A71CH_SIM" #Similar to A71CH
doApplet_SE05X_A_ON="-DApplet=SE05X_A" #SE050 Type A (ECC)
doApplet_SE05X_B_ON="-DApplet=SE05X_B" #SE050 Type B (RSA)
doApplet_SE05X_C_ON="-DApplet=SE05X_C" #SE050 (Super set of A + B)
doApplet_SE05X_L_ON="-DApplet=SE05X_L" #SE050 (Similar to A71CL)
doApplet_LoopBack_ON="-DApplet=LoopBack" #NXP Internal testing Applet


### SE05X_Ver : SE05X Applet version.
#
# Selection of Applet version 03_XX enables SE050 features.
# Selection of Applet version 06_00 enables SE051 features.
#
doSE05X_Ver_03_XX_ON="-DSE05X_Ver=03_XX" #SE050
doSE05X_Ver_06_00_ON="-DSE05X_Ver=06_00" #SE051


### Host : Host where the software stack is running
#
# e.g. Windows, PC Linux, Embedded Linux, Kinetis like embedded platform
doHost_Darwin_ON="-DHost=Darwin" #OS X / Macintosh
doHost_PCLinux32_ON="-DHost=PCLinux32" #PC/Laptop Linux with 32bit libraries
doHost_PCLinux64_ON="-DHost=PCLinux64" #PC/Laptop Linux with 64bit libraries
doHost_PCWindows_ON="-DHost=PCWindows" #PC/Laptop Windows
doHost_Cygwin_ON="-DHost=Cygwin" #Using Cygwin
doHost_frdmk64f_ON="-DHost=frdmk64f" #Embedded Kinetis Freedom K64F
doHost_evkmimxrt1060_ON="-DHost=evkmimxrt1060" #Embedded Kinetis i.MX RT 1060
doHost_lpcxpresso55s_ON="-DHost=lpcxpresso55s" #Embedded LPCXpresso55s (No demarcation of secure/non-secure world)
doHost_lpcxpresso55s_ns_ON="-DHost=lpcxpresso55s_ns" #Non Secure world of LPCXpresso55s
doHost_lpcxpresso55s_s_ON="-DHost=lpcxpresso55s_s" #Secure world of LPCXpresso55s
doHost_iMXLinux_ON="-DHost=iMXLinux" #Embedded Linux on i.MX
doHost_Raspbian_ON="-DHost=Raspbian" #Embedded Linux on RaspBerry PI
doHost_Android_ON="-DHost=Android" #Android
doHost_Win10IoT_ON="-DHost=Win10IoT" #Windows 10 IoT Core


### SMCOM : Communication Interface
#
# How the host library communicates to the Secure Element.
# This may be directly over an I2C interface on embedded platform.
# Or sometimes over Remote protocol like JRCP_V1 / JRCP_V2 / VCOM from PC.
doSMCOM_None_ON="-DSMCOM=None" #Not using any Communication layer
doSMCOM_JRCP_V2_ON="-DSMCOM=JRCP_V2" #Socket Interface New Implementation

#Socket Interface Old Implementation.
# This is the interface used from Host PC when when we run jrcpv1_server
# from the linux PC.
doSMCOM_JRCP_V1_ON="-DSMCOM=JRCP_V1"
doSMCOM_VCOM_ON="-DSMCOM=VCOM" #Virtual COM Port
doSMCOM_SCI2C_ON="-DSMCOM=SCI2C" #Smart Card I2C for A71CH and A71CH
doSMCOM_T1oI2C_ON="-DSMCOM=T1oI2C" #T=1 over I2C for SE050
doSMCOM_T1oI2C_GP1_0_ON="-DSMCOM=T1oI2C_GP1_0" #GP Spec
doSMCOM_RC663_VCOM_ON="-DSMCOM=RC663_VCOM" #Via RC663 Over VCOM Interface from Windows PC
doSMCOM_PN7150_ON="-DSMCOM=PN7150" #NFC Interface using PN7150
doSMCOM_Thread_ON="-DSMCOM=Thread" #Thread Mode interface
doSMCOM_PCSC_ON="-DSMCOM=PCSC" #CCID PC/SC reader interface


### HostCrypto : Counterpart Crypto on Host
#
# What is being used as a cryptographic library on the host.
# As of now only OpenSSL / mbedTLS is supported
doHostCrypto_MBEDTLS_ON="-DHostCrypto=MBEDTLS" #Use mbedTLS as host crypto

#Use mbed-crypto as host crypto
# Required for ARM-PSA / TF-M
doHostCrypto_MBEDCRYPTO_ON="-DHostCrypto=MBEDCRYPTO"
doHostCrypto_OPENSSL_ON="-DHostCrypto=OPENSSL" #Use OpenSSL as host crypto

#User Implementation of Host Crypto
# e.g. Files at ``sss/src/user/crypto`` have low level AES/CMAC primitives.
# The files at ``sss/src/user`` use those primitives.
# This becomes an example for users with their own AES Implementation
# This then becomes integration without mbedTLS/OpenSSL for SCP03 / AESKey.
#
# .. note:: ECKey abstraction is not implemented/available yet.
doHostCrypto_User_ON="-DHostCrypto=User"

#NO Host Crypto
# Note, this is unsecure and only provided for experimentation
# on platforms that do not have an mbedTLS PORT
# Many :ref:`sssftr-control` have to be disabled to have a valid build.
doHostCrypto_None_ON="-DHostCrypto=None"


### RTOS : Choice of Operating system
#
# Default would mean nothing special.
# i.e. Without any RTOS on embedded system, or default APIs on PC/Linux
doRTOS_Default_ON="-DRTOS=Default" #No specific RTOS. Either bare matal on embedded system or native linux or Windows OS
doRTOS_FreeRTOS_ON="-DRTOS=FreeRTOS" #Free RTOS for embedded systems


### mbedTLS_ALT : ALT Engine implementation for mbedTLS
#
# When set to None, mbedTLS would not use ALT Implementation to connect to / use Secure Element.
# This needs to be set to SSS for Cloud Demos over SSS APIs
dombedTLS_ALT_SSS_ON="-DmbedTLS_ALT=SSS" #Use SSS Layer ALT implementation
dombedTLS_ALT_A71CH_ON="-DmbedTLS_ALT=A71CH" #Legacy implementation

#Not using any mbedTLS_ALT
#
# When this is selected, cloud demos can not work with mbedTLS
dombedTLS_ALT_None_ON="-DmbedTLS_ALT=None"


### SCP : Secure Channel Protocol
#
# In case we enable secure channel to Secure Element, which interface to be used.
doSCP_None_ON="-DSCP=None" #
doSCP_SCP03_SSS_ON="-DSCP=SCP03_SSS" #Use SSS Layer for SCP.  Used for SE050 family.
doSCP_SCP03_HostCrypto_ON="-DSCP=SCP03_HostCrypto" #Use Host Crypto Layer for SCP03. Legacy implementation. Used for older demos of A71CH Family.


### FIPS : Enable or disable FIPS
#
# This selection mostly impacts tests, and generally not the actual Middleware
doFIPS_None_ON="-DFIPS=None" #NO FIPS
doFIPS_SE050_ON="-DFIPS=SE050" #SE050 IC FIPS
doFIPS_140_2_ON="-DFIPS=140_2" #FIPS 140-2
doFIPS_140_3_ON="-DFIPS=140_3" #FIPS 140-3


### SE05X_Auth : SE050 Authentication
#
# This settings is used by examples to connect using various options
# to authenticate with the Applet.
# The SE05X_Auth options can be changed for KSDK Demos and Examples.
# To change SE05X_Auth option follow below steps.
# Set flag ``SSS_HAVE_SCP_SCP03_SSS`` to 1 and Reset flag ``SSS_HAVE_SCP_NONE`` to 0.
# To change SE05X_Auth option other than ``None`` and  ``PlatfSCP03``,
# execute se05x_Delete_and_test_provision.exe in order to provision the Authentication Key.
# To change SE05X_Auth option to ``ECKey`` or ``ECKey_PlatfSCP03``,
# Set additional flag ``SSS_HAVE_HOSTCRYPTO_ANY`` to 1.
doSE05X_Auth_None_ON="-DSE05X_Auth=None" #Use the default session (i.e. session less) login
doSE05X_Auth_UserID_ON="-DSE05X_Auth=UserID" #Do User Authentication with UserID
doSE05X_Auth_PlatfSCP03_ON="-DSE05X_Auth=PlatfSCP03" #Use Platform SCP for connection to SE

#Do User Authentication with AES Key
# Earlier this was called AppletSCP03
doSE05X_Auth_AESKey_ON="-DSE05X_Auth=AESKey"

#Do User Authentication with EC Key
# Earlier this was called FastSCP
doSE05X_Auth_ECKey_ON="-DSE05X_Auth=ECKey"
doSE05X_Auth_UserID_PlatfSCP03_ON="-DSE05X_Auth=UserID_PlatfSCP03" #UserID and PlatfSCP03
doSE05X_Auth_AESKey_PlatfSCP03_ON="-DSE05X_Auth=AESKey_PlatfSCP03" #AESKey and PlatfSCP03
doSE05X_Auth_ECKey_PlatfSCP03_ON="-DSE05X_Auth=ECKey_PlatfSCP03" #ECKey and PlatfSCP03


### A71CH_AUTH : A71CH Authentication
#
# This settings is used by SSS-API based examples to connect using either plain or authenticated to the A71CH.
doA71CH_AUTH_None_ON="-DA71CH_AUTH=None" #Plain communication, not authenticated or encrypted
doA71CH_AUTH_SCP03_ON="-DA71CH_AUTH=SCP03" #SCP03 enabled


### Log : Logging
doLog_Default_ON="-DLog=Default" #Default Logging
doLog_Verbose_ON="-DLog=Verbose" #Very Verbose logging
doLog_Silent_ON="-DLog=Silent" #Totally silent logging
doLog_SeggerRTT_ON="-DLog=SeggerRTT" #Segger Real Time Transfer (For Test Automation, NXP Internal)


### CMAKE_BUILD_TYPE : See https://cmake.org/cmake/help/latest/variable/CMAKE_BUILD_TYPE.html
#
# For embedded builds, this choices sets optimization levels.
# For MSVC builds, build type is selected from IDE As well
doCMAKE_BUILD_TYPE_Debug_ON="-DCMAKE_BUILD_TYPE=Debug" #For developer
doCMAKE_BUILD_TYPE_Release_ON="-DCMAKE_BUILD_TYPE=Release" #Optimization enabled and debug symbols removed
doCMAKE_BUILD_TYPE_RelWithDebInfo_ON="-DCMAKE_BUILD_TYPE=RelWithDebInfo" #Optimization enabled but with debug symbols
doCMAKE_BUILD_TYPE__ON="-DCMAKE_BUILD_TYPE=" #Empty Allowed
# SE05X Secure Element : Symmetric AES
doSSSFTR_SE05X_AES_ON="-DSSSFTR_SE05X_AES=ON"
doSSSFTR_SE05X_AES_OFF="-DSSSFTR_SE05X_AES=OFF"
# SE05X Secure Element : Elliptic Curve Cryptography
doSSSFTR_SE05X_ECC_ON="-DSSSFTR_SE05X_ECC=ON"
doSSSFTR_SE05X_ECC_OFF="-DSSSFTR_SE05X_ECC=OFF"
# SE05X Secure Element : RSA
doSSSFTR_SE05X_RSA_ON="-DSSSFTR_SE05X_RSA=ON"
doSSSFTR_SE05X_RSA_OFF="-DSSSFTR_SE05X_RSA=OFF"
# SE05X Secure Element : KEY operations : SET Key
doSSSFTR_SE05X_KEY_SET_ON="-DSSSFTR_SE05X_KEY_SET=ON"
doSSSFTR_SE05X_KEY_SET_OFF="-DSSSFTR_SE05X_KEY_SET=OFF"
# SE05X Secure Element : KEY operations : GET Key
doSSSFTR_SE05X_KEY_GET_ON="-DSSSFTR_SE05X_KEY_GET=ON"
doSSSFTR_SE05X_KEY_GET_OFF="-DSSSFTR_SE05X_KEY_GET=OFF"
# SE05X Secure Element : Authenticate via ECKey
doSSSFTR_SE05X_AuthECKey_ON="-DSSSFTR_SE05X_AuthECKey=ON"
doSSSFTR_SE05X_AuthECKey_OFF="-DSSSFTR_SE05X_AuthECKey=OFF"
# SE05X Secure Element : Allow creation of user/authenticated session.
#
# If the intended deployment only uses Platform SCP
# Or it is a pure session less integration, this can
# save some code size.
doSSSFTR_SE05X_AuthSession_ON="-DSSSFTR_SE05X_AuthSession=ON"
doSSSFTR_SE05X_AuthSession_OFF="-DSSSFTR_SE05X_AuthSession=OFF"
# SE05X Secure Element : Allow creation/deletion of Crypto Objects
#
# If disabled, new Crytpo Objects are neither created and
# old/existing Crypto Objects are not deleted.
# It is assumed that during provisioning phase, the required
# Crypto Objects are pre-created or they are never going to
# be needed.
doSSSFTR_SE05X_CREATE_DELETE_CRYPTOOBJ_ON="-DSSSFTR_SE05X_CREATE_DELETE_CRYPTOOBJ=ON"
doSSSFTR_SE05X_CREATE_DELETE_CRYPTOOBJ_OFF="-DSSSFTR_SE05X_CREATE_DELETE_CRYPTOOBJ=OFF"
# Software : Symmetric AES
doSSSFTR_SW_AES_ON="-DSSSFTR_SW_AES=ON"
doSSSFTR_SW_AES_OFF="-DSSSFTR_SW_AES=OFF"
# Software : Elliptic Curve Cryptography
doSSSFTR_SW_ECC_ON="-DSSSFTR_SW_ECC=ON"
doSSSFTR_SW_ECC_OFF="-DSSSFTR_SW_ECC=OFF"
# Software : RSA
doSSSFTR_SW_RSA_ON="-DSSSFTR_SW_RSA=ON"
doSSSFTR_SW_RSA_OFF="-DSSSFTR_SW_RSA=OFF"
# Software : KEY operations : SET Key
doSSSFTR_SW_KEY_SET_ON="-DSSSFTR_SW_KEY_SET=ON"
doSSSFTR_SW_KEY_SET_OFF="-DSSSFTR_SW_KEY_SET=OFF"
# Software : KEY operations : GET Key
doSSSFTR_SW_KEY_GET_ON="-DSSSFTR_SW_KEY_GET=ON"
doSSSFTR_SW_KEY_GET_OFF="-DSSSFTR_SW_KEY_GET=OFF"
# Software : Used as a test counterpart
#
# e.g. Major part of the mebdTLS SSS layer is purely used for
# testing of Secure Element implementation, and can be avoided
# fully during many production scenarios.
doSSSFTR_SW_TESTCOUNTERPART_ON="-DSSSFTR_SW_TESTCOUNTERPART=ON"
doSSSFTR_SW_TESTCOUNTERPART_OFF="-DSSSFTR_SW_TESTCOUNTERPART=OFF"

# Compile in NXP NFC RdLib support
doWithNXPNFCRdLib_ON="-DWithNXPNFCRdLib=ON"
doWithNXPNFCRdLib_OFF="-DWithNXPNFCRdLib=OFF"

# Compile With open62541 Support
doWithOPCUA_open62541_ON="-DWithOPCUA_open62541=ON"
doWithOPCUA_open62541_OFF="-DWithOPCUA_open62541=OFF"

# Create and use shared libraries
doWithSharedLIB_ON="-DWithSharedLIB=ON"
doWithSharedLIB_OFF="-DWithSharedLIB=OFF"

# NXP Internal
doNXPInternal_ON="-DNXPInternal=ON"
doNXPInternal_OFF="-DNXPInternal=OFF"

# Compile with Code Coverage
doWithCodeCoverage_ON="-DWithCodeCoverage=ON"
doWithCodeCoverage_OFF="-DWithCodeCoverage=OFF"

# Include code from ../customer
doWithExtCustomerCode_ON="-DWithExtCustomerCode=ON"
doWithExtCustomerCode_OFF="-DWithExtCustomerCode=OFF"
