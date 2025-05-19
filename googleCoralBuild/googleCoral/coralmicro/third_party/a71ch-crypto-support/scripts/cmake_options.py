# Copyright 2019,2020 NXP
#
# SPDX-License-Identifier: Apache-2.0
#

import os

import cmake_features
from pprint import pprint

__VERSION__ = "2020.01.31_00"
#
# 2020.06.30_00 : Pradnya : SE05X_Auth can be selected from Header file option for KSDK examples
#               :
# 2020.01.31_00 : Purnank : Limit options in customer selection
#               :         :
# 2019.09.17_00 : Purnank : Add SSSFTR to cmake_options_installed.cmake.in
#               :         :
# 2019.09.13_00 : Marc    : Create cmake_options_installed.cmake.in
#               :         :
# 2019.08.18_00 : Purnank : Added cmake_features
#               :         :
# 2019.07.28_00 : Purnank : Removed 02_02_00
#               :         :
# 2019.07.23_00 : Purnank : LPC55S Trust Zone added
#               :         :
# 2019.03.31_00 : Purnank : Handle drop down options.
#               :         :
# 2019.03.11_00 : Sujay   : For OpenSSL Engine support both OpenSSL and mbedTLS
#               :         : conditionally.
#               :         :
# 2019.03.02_00 : Purnank : Add AR 02_01_00 (and later)
#               :         :
# 2019.02.02_00 : Purnank : Document cmake_options.cmake as well
#               :         :
# 2019.01.17_00 : Purnank : Re-Integrated logic for WithOptionCheck
#               :         :
# 2018.12.10_00 : Purnank : With Option check context
#               :         :
# 2018.11.06_00 : Purnank : Create Groovy Helper as well
#               :         :
# 2018.10.23_00 : Sujay   : Added Azure checks
#               :         :
# 2018.08.13_00 : Purnank : separate cmake_otions_check.py
#                         : file
#

IMPL_TYPES = ["sscp", "se05x", "mbedtls", "openssl"]

LIST_APPLETS = [
    ("None", "Compiling without any Applet Support", True),
    ("A71CH", "A71CH (ECC)", True),
    ("A71CL", "A71CL (RSA)", False),
    ("A71CH_SIM", "Similar to A71CH", False),

    ("SE05X_A", "SE050 Type A (ECC)", True),
    ("SE05X_B", "SE050 Type B (RSA)", True),
    ("SE05X_C", "SE050 (Super set of A + B)", True),
    ("SE05X_L", "SE050 (Similar to A71CL)", False),
    # ("SE051_C", "SE051 With Iot and SEMS Lite Applet", True),
    ("LoopBack", "NXP Internal testing Applet", False),
]

LIST_FIPS = [
  ("None", "NO FIPS", True),
  ("SE050", "SE050 IC FIPS", True),
  ("140_2", "FIPS 140-2", False),
  ("140_3", "FIPS 140-3", False),
]

DEPREATCED_LIST = [
    ("WithApplet_SE05X", "SSS_HAVE_APPLET_SE05X_IOT"),
    ("WithApplet_SE050_A", "SSS_HAVE_APPLET_SE05X_A"),
    ("WithApplet_SE050_B", "SSS_HAVE_APPLET_SE05X_B"),
    ("WithApplet_SE050_C", "SSS_HAVE_APPLET_SE05X_C"),
    ("SSS_HAVE_SE050_A", "SSS_HAVE_APPLET_SE05X_A"),
    ("SSS_HAVE_SE050_B", "SSS_HAVE_APPLET_SE05X_B"),
    ("SSS_HAVE_SE050_C", "SSS_HAVE_APPLET_SE05X_C"),
    ("SSS_HAVE_SE05X", "SSS_HAVE_APPLET_SE05X_IOT"),
    ("SSS_HAVE_SE", "SSS_HAVE_APPLET"),

    # ("SSS_HAVE_A71CH", "SSS_HAVE_APPLET_A71CH"),
    # ("SSS_HAVE_A71CL", "SSS_HAVE_APPLET_A71CL"),
    # ("SSS_HAVE_A71CH_SIM", "SSS_HAVE_APPLET_A71CH_SIM"),
    ("SSS_HAVE_LOOPBACK", "SSS_HAVE_APPLET_LOOPBACK"),

    # ("SSS_HAVE_OPENSSL", "SSS_HAVE_HOSTCRYPTO_OPENSSL"),
    # ("SSS_HAVE_MBEDTLS", "SSS_HAVE_HOSTCRYPTO_MBEDTLS"),
    # ("SSS_HAVE_MBEDCRYPTO", "SSS_HAVE_HOSTCRYPTO_MBEDCRYPTO"),
    # ("SSS_HAVE_ALT_A71CH", "SSS_HAVE_MBEDTLS_ALT_A71CH"),
    #("SSS_HAVE_ALT_SSS", "SSS_HAVE_MBEDTLS_ALT_SSS"),
    ("SSS_HAVE_ALT", "SSS_HAVE_MBEDTLS_ALT"),
]

for e, _, _ in LIST_APPLETS:
    DEPREATCED_LIST.append(
        (
            "WithApplet_%s"%e,
            "SSS_HAVE_APPLET_%s"%e.upper(),
        ))
    DEPREATCED_LIST.append(
        (
            "SSS_HAVE_%s"%e,
            "SSS_HAVE_APPLET_%s"%e.upper(),
        ))


#
# THIS is used for CMake GUI
#

ENABLED_FOR_SE051 = True

LIST_SE05X_APPLET_VERSIONS = [
#   ("Any",   "Any version of SE05X Applet", True),
    ("03_XX", "SE050", True),
    # ("04_04", "NXP Internal - 4.4", False),
    # # ("04_08", "NXP Internal - 4.8", False),
    # # ("04_12", "NXP Internal - 4.12", False),
    # ("05_00", "NXP Internal - 5.00", ENABLED_FOR_SE051),
    # ("05_02", "NXP Internal - 5.02", ENABLED_FOR_SE051),
    # ("05_04", "NXP Internal - 5.04", ENABLED_FOR_SE051),
    # ("05_06", "NXP Internal - 5.06", ENABLED_FOR_SE051),
    # ("05_08", "NXP Internal - 5.08", ENABLED_FOR_SE051),
    # ("05_10", "NXP Internal - 5.10", ENABLED_FOR_SE051),
    # ("05_12", "NXP Internal - 5.12", ENABLED_FOR_SE051),
    ("06_00", "SE051", ENABLED_FOR_SE051),
#    ("ANY", ("Any version", "",
#             "When this is selected, compatibility decisions are taken at run time.",
# #             "And MW tries to be compatible to a wider extent across applet versions.",
# #             "NOTE: any feature not supported by Applet may still be tried to be exercise,
# #             "and such operations will fail at applet level",), False),
]

#
# THIS is used for GTE Checks
# Greater than equal to!
#
LIST_SE05X_APPLET_VERSIONS_ALL = [
    "03_XX",
    # "04_04",
    # "04_08",
    # "04_12",
    # "05_00",
    # "05_02",
    # "05_04",
    # "05_06",
    # "05_08",
    # "05_10",
    # "05_12",
    "06_00",
]

# LIST_OPENSSL = [
#     ("1_1_1", "Use latest 1.1.1 version (Only applicable on PC)", True),
#     ("1_0_2", "Use older 1.0.2 version (Only NXP Internal)", False),
# ]

LIST_SMCOM = [
    ("None", "Not using any Communication layer", True),
    ("JRCP_V2", "Socket Interface New Implementation", True),
    ("JRCP_V1", (
        "Socket Interface Old Implementation.",
        "This is the interface used from Host PC when when we run jrcpv1_server",
        "from the linux PC."), True),
    ("VCOM", "Virtual COM Port", True),
    ("SCI2C", "Smart Card I2C for A71CH and A71CH", True),
    ("T1oI2C", "T=1 over I2C for SE050", True),
    ("T1oI2C_GP1_0", "GP Spec", False),
    ("RC663_VCOM", "Via RC663 Over VCOM Interface from Windows PC", False),
    ("PN7150", "NFC Interface using PN7150", False),
    ("Thread", "Thread Mode interface", False),
    ("PCSC", "CCID PC/SC reader interface", True),
]

LIST_HOST = [
    ("Darwin", "OS X / Macintosh", True),
    ("PCLinux32", "PC/Laptop Linux with 32bit libraries", True),
    ("PCLinux64", "PC/Laptop Linux with 64bit libraries", True),
    ("PCWindows", "PC/Laptop Windows", True),
    ("Cygwin", "Using Cygwin", True),
    ("frdmk64f", "Embedded Kinetis Freedom K64F", True),
    ("evkmimxrt1060", "Embedded Kinetis i.MX RT 1060", True),
    ("lpcxpresso55s", "Embedded LPCXpresso55s (No demarcation of secure/non-secure world)", True),
    ("lpcxpresso55s_ns", "Non Secure world of LPCXpresso55s", True),
    ("lpcxpresso55s_s", "Secure world of LPCXpresso55s", True),
    ("iMXLinux", "Embedded Linux on i.MX", True),
    ("Raspbian", "Embedded Linux on RaspBerry PI", True),
    ("Android", "Android", True),
    ("Win10IoT", "Windows 10 IoT Core", False),
]

# Update cmake_options_check.py ==> LIST_APPLET_SE05X_VERSION as well.

# LIST_APPLET_SE05X_VERSION = [
#     # "02_01_00",
#     ("03_XX_XX", "RFP", True),
#     # ("02_05_XX", "AR6"),
#     # ("02_04_XX", "AR6"),
#     # ("02_03_XX", "AR6"),
#     # ("02_02_00", "Older release"),
# ]

LIST_RTOS = [
    ("Default", "No specific RTOS. Either bare matal on embedded system" +
     " or native linux or Windows OS", True),
    ("FreeRTOS", "Free RTOS for embedded systems", True),
]

LIST_HOSTCRYPTO = [
    ("MBEDTLS", "Use mbedTLS as host crypto", True),
    ("MBEDCRYPTO", (
        "Use mbed-crypto as host crypto",
        "Required for ARM-PSA / TF-M"
        ), True),
    ("OPENSSL", "Use OpenSSL as host crypto", True),
    ("User",    (
        "User Implementation of Host Crypto",
        "e.g. Files at ``sss/src/user/crypto`` have low level AES/CMAC primitives.",
        "The files at ``sss/src/user`` use those primitives.",
        "This becomes an example for users with their own AES Implementation",
        "This then becomes integration without mbedTLS/OpenSSL for SCP03 / AESKey.",
        "",
        ".. note:: ECKey abstraction is not implemented/available yet.",
        ), True),
    ("None",
        ("NO Host Crypto",
          "Note, this is unsecure and only provided for experimentation",
          "on platforms that do not have an mbedTLS PORT",
          "Many :ref:`sssftr-control` have to be disabled to have a valid build."

          ), True),
]

for e, _, _ in LIST_HOSTCRYPTO:
    DEPREATCED_LIST.append(
        (
            "SSS_HAVE_%s"%e.upper(),
            "SSS_HAVE_HOSTCRYPTO_%s"%e.upper(),
        ))


LIST_MBEDTLS_ALT = [
    ("SSS", "Use SSS Layer ALT implementation", True),
    ("A71CH", "Legacy implementation", True),
    ("None", ("Not using any mbedTLS_ALT", "", "When this is selected, cloud demos can not work with mbedTLS"), True),
]

for e, _, _ in LIST_MBEDTLS_ALT:
    DEPREATCED_LIST.append(
        (
            "SSS_HAVE_ALT_%s"%e.upper(),
            "SSS_HAVE_MBEDTLS_ALT_%s"%e.upper(),
        ))


LIST_SCP = [
    ("None", "", True),
    ("SCP03_SSS", "Use SSS Layer for SCP.  Used for SE050 family.", True),
    ("SCP03_HostCrypto",
     "Use Host Crypto Layer for SCP03. Legacy implementation. "
     "Used for older demos of A71CH Family.", True),
    # ("SCP02_HostCrypto", "Use Host Crypto Layer for SCP02. Used only for NXP internal testing.", False),
]

LIST_A71CH_AUTH = [
    ("None", "Plain communication, not authenticated or encrypted", True),
    ("SCP03", "SCP03 enabled", True),
]

LIST_SE05X_AUTHLIST = [
    ("None", "Use the default session (i.e. session less) login", True),
    ("UserID", "Do User Authentication with UserID", True),
    ("PlatfSCP03", "Use Platform SCP for connection to SE", True),
    ("AESKey", ("Do User Authentication with AES Key", "Earlier this was called AppletSCP03"), True),
    ("ECKey", ("Do User Authentication with EC Key", "Earlier this was called FastSCP"), True),
    ("UserID_PlatfSCP03", "UserID and PlatfSCP03", True),
    ("AESKey_PlatfSCP03", "AESKey and PlatfSCP03", True),
    ("ECKey_PlatfSCP03", "ECKey and PlatfSCP03", True),
]

for e, _, _ in LIST_SE05X_AUTHLIST:
    DEPREATCED_LIST.append(
        (
            "SSS_HAVE_SE05X_Auth_%s"%e,
            "SSS_HAVE_SE05X_AUTH_%s"%e.upper(),
        ))


LIST_LOG = [
    ("Default", "Default Logging", True),
    ("Verbose", "Very Verbose logging", True),
    ("Silent", "Totally silent logging", True),

    # #> .. note:: This is NXP internal Option
    ("SeggerRTT", "Segger Real Time Transfer (For Test Automation, NXP Internal)", False),
]

CMAKE_BUILD_TYPE = [
    # See https://cmake.org/cmake/help/latest/variable/CMAKE_BUILD_TYPE.html"
    ("Debug", "For developer", True),
    ("Release", "Optimization enabled and debug symbols removed", True),
    ("RelWithDebInfo", "Optimization enabled but with debug symbols", True),
    ("", "Empty Allowed", True),
    # ("MinSizeRel", "For Developer"),
]

ALL_VALUES = [
    ("Applet", "SE05X_C",
     ("The Secure Element Applet", "",
      "You can compile host library for different Applets listed below.",
      "Please note, some of these Applets may be for NXP Internal use only.",
      ), LIST_APPLETS),
    ("SE05X_Ver", "03_XX",
      (
        "SE05X Applet version.", "", "Selection of Applet version 03_XX enables SE050 features.",
        "Selection of Applet version 06_00 enables SE051 features.",
        "",
      ), LIST_SE05X_APPLET_VERSIONS),
    # ("Applet_SE05X_Ver", "03_XX_XX",
    #  ("Applet version of SE050", "",
    #   "You can compile host library for different versions/variants of the SE05X Applets.",
    #   "Please note, some of these Versions of the Applets may be for NXP Internal use only.",
    #   ), LIST_APPLET_SE05X_VERSION),
    ("Host", "PCWindows",
     ("Host where the software stack is running", "",
      "e.g. Windows, PC Linux, Embedded Linux, Kinetis like embedded platform",
      ), LIST_HOST),
    ("SMCOM", "VCOM",
     ("Communication Interface", "",
      "How the host library communicates to the Secure Element.",
      "This may be directly over an I2C interface on embedded platform.",
      "Or sometimes over Remote protocol like JRCP_V1 / JRCP_V2 / VCOM from PC."
      ), LIST_SMCOM),
    ("HostCrypto", "MBEDTLS",
     ("Counterpart Crypto on Host", "",
      "What is being used as a cryptographic library on the host.",
      "As of now only OpenSSL / mbedTLS is supported",
      ), LIST_HOSTCRYPTO),
    ("RTOS", "Default",
     ("Choice of Operating system", "",
      "Default would mean nothing special.",
      "i.e. Without any RTOS on embedded system, or default APIs on PC/Linux",
      ), LIST_RTOS),
    ("mbedTLS_ALT", "None",
     ("ALT Engine implementation for mbedTLS", "",
      "When set to None, mbedTLS would not use ALT "
      "Implementation to connect to / use Secure Element.",
      "This needs to be set to SSS for Cloud Demos over SSS APIs",
      ), LIST_MBEDTLS_ALT),
    ("SCP", "None",
     ("Secure Channel Protocol", "",
      "In case we enable secure channel to Secure Element, which interface to be used.",
      ), LIST_SCP),
    ("FIPS", "None",
      ("Enable or disable FIPS ", "",
        "This selection mostly impacts tests, and generally not the actual Middleware"),
        LIST_FIPS),
    ("SE05X_Auth", "None",
     ("SE050 Authentication", "",
      "This settings is used by examples to connect using various options",
      "to authenticate with the Applet.",
      "The SE05X_Auth options can be changed for KSDK Demos and Examples.",
      "To change SE05X_Auth option follow below steps.",
	  "Set flag ``SSS_HAVE_SCP_SCP03_SSS`` to 1 and Reset flag ``SSS_HAVE_SCP_NONE`` to 0.",
      "To change SE05X_Auth option other than ``None`` and  ``PlatfSCP03``,",
      "execute se05x_Delete_and_test_provision.exe in order to provision the Authentication Key.",
	  "To change SE05X_Auth option to ``ECKey`` or ``ECKey_PlatfSCP03``,",
	  "Set additional flag ``SSS_HAVE_HOSTCRYPTO_ANY`` to 1."
      ), LIST_SE05X_AUTHLIST),
    ("A71CH_AUTH", "None",
     ("A71CH Authentication", "",
      "This settings is used by SSS-API based examples to connect using either plain "
      "or authenticated to the A71CH."
      ), LIST_A71CH_AUTH),
    ("Log", "Default",
     ("Logging",
      ), LIST_LOG),
    ("CMAKE_BUILD_TYPE", "Debug",
     ("See https://cmake.org/cmake/help/latest/variable/CMAKE_BUILD_TYPE.html", "",
      "For embedded builds, this choices sets optimization levels.",
      "For MSVC builds, build type is selected from IDE As well",
      ), CMAKE_BUILD_TYPE),
    # ("OpenSSL", "1_1_1",
    #  ("For PC, which OpenSSL to pick up", "",
    #   "On Linux based builds, this option has no impact, because the build system",
    #   "picks up the default available/installed OpenSSL from the system directly.",
    #   ), LIST_OPENSSL),
]

CMAKE_OPTIONS_CHECK_GROOVY_FOOTER = r'''
// Manual
doWithApplet_SE050_EAR_CH_ON=doWithApplet_A71CH_SIM_ON


def VCOM_Delete_and_provision(binary_path="") {
    try
    {
        timeout(15) {
            if (binary_path == ""){
                binary_path="%WORKSPACE%\\src\\binaries\\tst"
            }
            bat """
                cd ${WORKSPACE}\\Automation_tools\\SE050_Automation
                SET PATH=C:\\Python37;%PATH%
                start Jlink_term_click.exe
                python program_and_terminate.py utils\\a7x_vcom-T1oI2C-frdmk64f-SE050x.bin
                ping 127.0.0.1
                ping 127.0.0.1
                ping 127.0.0.1
                python get_comport_frdmk64f.py
                IF NOT EXIST setenv_comport.bat echo "Missing setenv_comport.bat"
                IF NOT EXIST setenv_comport.bat exit 1
                call setenv_comport.bat
                call "${binary_path}\\VCOM-None-se05x_Delete_and_test_provision.exe" %VCOM_PORT%
                call "${binary_path}\\VCOM-None-se05x_FactoryReset.exe" %VCOM_PORT%
                call "${binary_path}\\VCOM-None-se05x_Delete_and_test_provision.exe" %VCOM_PORT%
            """
        }
    }
    catch(err) {
        echo "Delete all and Test Provisioning Failed Continue to run the test"
    }
}


def VCOM_Get_AC_Log(binary_path="", ac_log_path, testname) {
    try
    {
        timeout(15) {
            if (ac_log_path == ""){
                echo "AC Log path not SET"
            } else {
                if (binary_path == ""){
                    binary_path="%WORKSPACE%/src/binaries/tst/nxp"
                }
                bat """
                    cd ${WORKSPACE}\\Automation_tools\\SE050_Automation
                    SET PATH=C:\\Python37;%PATH%
                    start Jlink_term_click.exe
                    python program_and_terminate.py utils\\a7x_vcom-T1oI2C-frdmk64f-SE050x.bin
                    ping 127.0.0.1
                    ping 127.0.0.1
                    ping 127.0.0.1
                    python get_comport_frdmk64f.py
                    IF NOT EXIST setenv_comport.bat echo "Missing setenv_comport.bat"
                    IF NOT EXIST setenv_comport.bat exit 1
                    call setenv_comport.bat
                    call "${binary_path}\\VCOM-se05x_Log_AC.exe" -file ${ac_log_path} -testname ${testname} %VCOM_PORT%
                """
            }
        }
    }
    catch(err) {
        echo "Delete all and Test Provisioning Failed Continue to run the test"
    }
}


'''

COPYRIGHT = ["",
             " Copyright 2018-2020 NXP",
             "",
             " This software is owned or controlled by NXP and may only be used",
             " strictly in accordance with the applicable license terms.  By expressly",
             " accepting such terms or by downloading, installing, activating and/or",
             " otherwise using the software, you are agreeing that you have read, and",
             " that you agree to comply with and are bound by, such license terms.  If",
             " you do not agree to be bound by the applicable license terms, then you",
             " may not retain, install, activate or otherwise use the software.",
             "",
             " #############################################################",
             " This file is generated using a script",
             " #############################################################",
             "",
             # " Update `cmake_options_check.py` and `cmake_options_check.py`",
             # " instead of modifying this file.",
             # "",
             ]

class CMakeOptionsValue:  # pylint: disable=too-many-instance-attributes
    """
    Generates skeleton files for various APIs in SSS Layer
    """

    # cm_val : Cmake File object
    # cm_gv : Groovy File object
    def __init__(self, cm_val, cm_gv, cm_rst_v, cm_sh, cm_makin, cm_cmiin):  # pylint: disable=too-many-arguments
        self.cm_gv = cm_gv
        self.cm_val = cm_val
        self.cm_rst_v = cm_rst_v
        self.cm_sh = cm_sh
        self.cm_makin = cm_makin
        self.cm_cmiin = cm_cmiin
        self.prepend = ""
        self.condition = ""

        self.cm_f = cmake_features.CMakeFeatures(
            self.cm_val, self.cm_gv, self.cm_rst_v, self.cm_sh, self.cm_makin, self.cm_cmiin)
        # Header fle generated by cmake_features()
        self.cm_hin = None

    def run(self):
        self.cm_f.open()
        self.cm_hin = self.cm_f.get_h_file()
        for f in (self.cm_sh, self.cm_val, self.cm_makin, self.cm_cmiin):
            f.write(("\n#".join(COPYRIGHT)).strip())
            f.write("\n")
        for option, default_value, description, option_list in ALL_VALUES:
            self.write_option(option, default_value, description, option_list)
        for option, default_value, description, option_list in ALL_VALUES:
            self.write_with_cmake_names(option, description, option_list)
        for option, default_value, description, option_list in ALL_VALUES:
            self.write_option_description(option, description, option_list)

        self.cm_f.write_features()


        for sh in (self.cm_makin, self.cm_cmiin, self.cm_rst_v, self.cm_hin):
            sh.write("\n\n")
        self.cm_cmiin.write("# ")
        import cmake_options_check

        cmake_options_check.generate_cmake_options_check_file(self.cm_hin, self.cm_cmiin)

        self.cm_hin.write("/** ")

        for sh in (self.cm_makin, self.cm_cmiin, ):
            sh.write("\n# ")
        for f in (self.cm_makin, self.cm_cmiin, self.cm_hin):
            f.write("Deprecated items. Used here for backwards compatibility.")
        self.cm_hin.write(" */")
        self.cm_rst_v.write("""
.. _deprecated-defines:

Deprecated Defines
========================

Keept and for time being for backwards compatibility.  They will be removed in
some future release.

""")
        for sh in (self.cm_makin, self.cm_cmiin, self.cm_rst_v, self.cm_hin):
            sh.write("\n\n")
        for old_name, new_name  in DEPREATCED_LIST:
            self.cm_rst_v.write("- ``%s`` is renamed to ``%s``\n"%(old_name, new_name))
            self.cm_makin.write("%s := ${%s}\n"%(old_name, new_name))
            self.cm_cmiin.write("SET(%s %s)\n"%(old_name, new_name))
            self.cm_hin.write("#define %s (%s)\n"%(old_name, new_name))

        self.cm_f.close()

    def write_option(self, option, default_value, description, option_list):
        option_values_internal = []
        option_values_public = []

        for value, _, public in option_list:
            option_values_internal.append(value)
            self.cm_gv.write("doWith%s_%s_ON=\" -D%s=%s\"\n" % (
                option, value, option, value))
            if public:
                option_values_public.append(value)
        if default_value not in option_values_internal:
            raise Exception("option_list=%s does not have default_value=%s" % (
                str(option_list), default_value))
        cm_description = description[0]
        self.cm_val.write(SET_AND_SET_PROPERTY % (
            option, default_value, cm_description, option, ";".join(option_values_internal),
            option, ";".join(option_values_public)))

    @classmethod
    def get_sss_have_opt_val(cls, option, option_value):
        uoption = option.upper()
        uoption_value = option_value.upper()
        return "SSS_HAVE_%s_%s"%(uoption, uoption_value)

    def write_with_cmake_names(self, option, description, option_list):
        option_values = []
        self.cm_makin.write("\n# ")
        self.cm_makin.write("\n# ".join(description,))
        self.cm_makin.write("\n")
        for value, _, public in option_list:
            option_values.append(value)
        for option_value in option_values:
            uoption_value = ""
            uoption = option.upper()
            if option_value:
                uoption_value = option_value.upper()
            sss_have_opt_val = self.get_sss_have_opt_val(option, option_value)
            self.cm_makin.write("%s := ${%s}\n" % (sss_have_opt_val, sss_have_opt_val))
            self.cm_val.write("IF(\"${%s}\" STREQUAL \"%s\")\n" % (option, option_value))
            self.cm_val.write("    SET(With%s_%s ON)\n" % (option, option_value))
            self.cm_val.write("    SET(%s \"1\")\n" % (sss_have_opt_val, ))
            self.cm_val.write("ELSE()\n")
            self.cm_val.write("    SET(With%s_%s OFF)\n" % (option, option_value))
            self.cm_val.write("    SET(%s \"0\")\n" % (sss_have_opt_val, ))
            self.cm_val.write("ENDIF()\n\n")
        for option_value in option_values[:1]:
            self.cm_val.write("IF(\"${%s}\" STREQUAL \"%s\")\n" % (option, option_value))
            self.cm_val.write("    # OK\n")
        for option_value in option_values[1:]:
            self.cm_val.write("ELSEIF(\"${%s}\" STREQUAL \"%s\")\n" % (option, option_value))
            self.cm_val.write("    # OK\n")
        self.cm_val.write("ELSE()\n")
        self.cm_val.write("    MESSAGE(SEND_ERROR \"For '%s' '${%s}' is invalid.\")\n"
                          % (option, option))
        self.cm_val.write("    MESSAGE(STATUS \"Only supported values are '%s'\")\n"
                          % (", ".join(option_values)))
        self.cm_val.write("ENDIF()\n\n")

    def write_option_description(self, option, o_description, option_list):
        self.cm_rst_v.write("\n")
        self.cm_rst_v.write(".. _cmake-option-%s:\n\n"%(option,))
        self.cm_rst_v.write("%s\n" % (option,))
        self.cm_rst_v.write("%s\n" % ("=" * (20 + len(option)),))
        if tuple == type(o_description):
            sh_description = "\n# ".join(o_description)
            rst_description = "\n    ".join(o_description)
        else:
            sh_description = o_description
            rst_description = o_description
        self.cm_cmiin.write("\n# %s\n" % (sh_description,))
        self.cm_sh.write("\n\n### %s : %s\n" % (option, sh_description))
        if self.isHFileOption(option):
            self.cm_hin.write("\n\n/** %s : %s\n */\n" % (option, sh_description.replace("\n#","\n *")))
        self.cm_rst_v.write("\n")
        self.cm_rst_v.write(".. option:: %s\n" % (option,))
        self.cm_rst_v.write("\n")
        self.cm_rst_v.write("    %s\n" % (rst_description,))
        self.cm_rst_v.write("\n")
        for option_value, o_value_description, public in option_list:
            if public:
                self.cm_rst_v.write("    ``-D%s=%s``" % (option, option_value,))
            if tuple == type(o_value_description):
                sh_value_description = "\n# ".join(o_value_description)
                rst_value_description = "\n        ".join(o_value_description)
                if not public:
                    sh_value_description += "\n# NXP Internal\n"
                self.cm_sh.write("\n#")
                self.cm_sh.write(sh_value_description)
                self.cm_sh.write("\n")
            else:
                sh_value_description = o_value_description
                rst_value_description = o_value_description
            for e in (self.cm_makin, self.cm_cmiin):
                if sh_value_description:
                    e.write("\n# ")
                    e.write(sh_value_description)
                    e.write("\n")

            if public:
                if rst_value_description:
                    self.cm_rst_v.write(": %s" % (rst_value_description,))
            self.cm_rst_v.write("\n\n")
            if tuple == type(o_value_description):
                self.cm_sh.write("do%s_%s_ON=\"-D%s=%s\"\n" % (option, option_value, option, option_value))
            else:
                self.cm_sh.write("do%s_%s_ON=\"-D%s=%s\" #%s\n" % (option,
                                                                   option_value,
                                                                   option,
                                                                   option_value,
                                                                   sh_value_description))
            if option_value:
                sss_have_opt_val = self.get_sss_have_opt_val(option, option_value)
                if self.isHFileOption(option):
                    self.cm_hin.write("\n/** %s */\n"%sh_value_description.replace("\n#","\n *"))
                    self.cm_hin.write("#cmakedefine01 %s\n" % (sss_have_opt_val, ))
                self.cm_cmiin.write("SET(%s ${%s})\n" % (
                    sss_have_opt_val, sss_have_opt_val))
                self.cm_makin.write("%s := ${%s}\n" % (
                    sss_have_opt_val, sss_have_opt_val))
        if self.isHFileOption(option):
            self.cm_hin.write("\n#if (( 0                             \\\n")
            for option_value, _, _ in option_list:
                sss_have_opt_val = self.get_sss_have_opt_val(option, option_value)
                self.cm_hin.write("    + %-30s \\\n"%(sss_have_opt_val))
            self.cm_hin.write("    ) > 1)\n")
            self.cm_hin.write("#        error \"Enable only one of '%s'\"\n"%(option,))
            self.cm_hin.write("#endif\n\n")
            self.cm_hin.write("\n#if (( 0                             \\\n")
            for option_value, _, _ in option_list:
                sss_have_opt_val = self.get_sss_have_opt_val(option, option_value)
                self.cm_hin.write("    + %-30s \\\n"%(sss_have_opt_val))
            self.cm_hin.write("    ) == 0)\n")
            self.cm_hin.write("#        error \"Enable at-least one of '%s'\"\n"%(option,))
            self.cm_hin.write("#endif\n\n")

    def _NOT_USED_write_values(self, prefix, impl_types):
        for impl_type in impl_types:
            uhave_type = "%s_%s" % (prefix.upper(), impl_type.upper())
            have_type = "%s%s" % (prefix, impl_type)
            value_type = "SSS_HAVE_%s" % (uhave_type,)
            sss_have_opt_val = "SSS_HAVE_%s_%s"%(uoption, uoption_value)
            self.cm_val.write("\n")
            self.cm_val.write("IF(%s)\n" % (have_type,))
            self.cm_val.write("    SET(%s \"1\")\n" % (value_type,))
            self.cm_val.write("ELSE()\n")
            self.cm_val.write("    SET(%s \"0\")\n" % (value_type,))
            self.cm_val.write("ENDIF()\n")

    # def xset_when_either_of(self, prefix, impl_types, new_variable):
    #     have_types = []
    #     for impl_type in impl_types:
    #         have_type = "%s%s" % (prefix, impl_type)
    #         have_types.append(have_type)
    #     if len(have_types) > 4:
    #         self.cm_val.write("%sIF(\n    %s\n)\n" % (self.prepend,
    #                                                   "\n     OR ".join(have_types)))
    #     else:
    #         self.cm_val.write("%sIF(%s)\n" % (self.prepend,
    #                                           " OR ".join(have_types)))
    #     self.cm_val.write("%s    SET(%s ON)\n" % (self.prepend, new_variable))
    #     self.cm_val.write("%sELSE()\n" % (self.prepend,))
    #     self.cm_val.write("%s    SET(%s OFF)\n" % (self.prepend, new_variable))
    #     self.cm_val.write("%sENDIF()\n" % (self.prepend,))

    def mark_as_depreceted(self, key_words):
        for key_word in key_words:
            self.cm_val.write("\n")
            self.cm_val.write("IF(%s)\n" % (key_word,))
            self.cm_val.write('    MESSAGE(WARNING "Keyword \'%s\' is deprecated.")\n' % (key_word,))
            self.cm_val.write("ENDIF()\n")

    def isHFileOption(self, option):
        if option in (
                "RTOS",
                "CMAKE_BUILD_TYPE",
                "Host",
                "Log",
#               "SE05X_Auth",
                "SMCOM",
                ):
            return False
        return True


def generate_cmake_options_value_files():
    cm_gv = open("jenkins/cmake_options_check.groovy", "w")
    cm_val = open("cmake_options_values.cmake", "w")
    cm_rst_v = open("cmake_options_values.rst.txt", "w")
    cm_sh = open("cmake_options.sh", "w")
    cm_makin = open("cmake_options.mak.in", "w")
    cm_cmiin = open("cmake_options_installed.cmake.in", "w")
    cm_check = CMakeOptionsValue(cm_val, cm_gv, cm_rst_v, cm_sh, cm_makin, cm_cmiin)
    cm_check.run()
    CMakeOptionsFileToRST(cm_gv, cm_sh).run()
    cm_gv.write(CMAKE_OPTIONS_CHECK_GROOVY_FOOTER)
    cm_gv.write("\nreturn this;\n")

    for k in (cm_gv, cm_val, cm_rst_v, cm_sh):
        k.close()


class CMakeOptionsFileToRST:
    """
    Write CMake options to RST file.
    """
    def __init__(self, cm_gv, cm_sh):
        self.to_write = []
        self.cm_gv = cm_gv
        self.cm_sh = cm_sh
        self.cm_cmake = None
        self.cm_rst = None

    def run(self):
        self.cm_cmake = open("cmake_options.cmake")
        self.cm_rst = open("cmake_options.rst", "w")
        self.cmake_to_rst()
        self.cm_cmake.close()
        self.cm_rst.close()

    def cmake_to_rst(self):
        for l in self.cm_cmake:
            if l.startswith("##"):
                pass
            elif l.startswith("# "):
                self.cm_rst.write(l[2:])
            elif l.strip() == "#":
                self.cm_rst.write("\n")
            elif l.startswith("#> "):
                self.to_write.append(l[3:])
            elif l.startswith("#"):
                self.cm_rst.write(l[1:])
            elif l.startswith("OPTION("):
                self.cmake_option_to_rst(l)
            elif l.strip() == "":
                self.cm_rst.write("\n")
            else:
                pass

    def cmake_option_to_rst(self, op_line):
        assert op_line.startswith("OPTION(")
        (eoption, description, evalue) = op_line.split('"')
        option = eoption[7:].strip()
        if "NXPInternal" == option:  # pylint: disable=misplaced-comparison-constant
            value = "OFF. (ON only within NXP)"
        elif "ON" in evalue:
            value = "ON"
        elif "OFF" in evalue:
            value = "OFF"
        else:
            value = "Unknown"

        self.cm_rst.write(".. option:: %s\n\n" % (option,))
        self.cm_rst.write("    - %s" % (description,))
        self.cm_rst.write("\n")
        self.cm_rst.write("    - Default is %s\n" % (value,))
        if self.to_write:
            self.cm_rst.write("\n")
            while self.to_write:
                v = self.to_write.pop().strip()
                if v.startswith(":") or v.startswith("."):
                    self.cm_rst.write("    %s" % (v,))
                else:
                    self.cm_rst.write("    - %s" % (v,))
            self.cm_rst.write("\n")
        self.cm_rst.write("\n")
        self.cm_rst.write("\n")
        self.cm_gv.write('do%s_ON="-D%s=ON"\n' % (option, option,))
        self.cm_gv.write('do%s_OFF="-D%s=OFF"\n' % (option, option,))
        self.cm_sh.write("\n# %s\n"%description)
        self.cm_sh.write('do%s_ON="-D%s=ON"\n' % (option, option,))
        self.cm_sh.write('do%s_OFF="-D%s=OFF"\n' % (option, option,))


SET_AND_SET_PROPERTY = """
SET(
    %s
    "%s"
    CACHE
        STRING
        "%s"
)

IF(NXPInternal)
    SET_PROPERTY(
        CACHE %s
        PROPERTY
            STRINGS
            "%s;"
    )
ELSE()
    SET_PROPERTY(
        CACHE %s
        PROPERTY
            STRINGS
            "%s;"
    )
ENDIF()
"""



def main():
    script_root_dir = os.path.dirname(os.path.realpath(__file__))
    if os.path.abspath(os.curdir) != script_root_dir:
        os.chdir(script_root_dir)
    generate_cmake_options_value_files()


if __name__ == "__main__":
    main()
