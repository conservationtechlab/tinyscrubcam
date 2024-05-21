#
# Copyright 2019 NXP
# SPDX-License-Identifier: Apache-2.0
#

"""
# Used by cmake_options.py
"""

LAYERS = (
    ("SE05X", "SE05X Secure Element"),
    # ("HW", "Native HW"),
    ("SW", "Software"),
)

# High level crypto operations
# For all LAYERS
COMMON_FTR_L1 = (
    ("AES", "Symmetric AES"),
    # ("DES", "Symmetric DES"),
    ("ECC", "Elliptic Curve Cryptography"),
    ("RSA", "RSA"),
    ("KEY", "KEY operations"),
)

# Sub components/features of LAYERS
COMMON_FTR_L2 = {
    # "xAES": (
    #     ("ECB", "Electronic Codebook Cipher"),
    #     ("CBC", "Cipher Blocker Chaining"),
    #     #("CTR", "Counter"),
    #     # ("GCM", "Galois/Counter Mode"),
    #     ("CCM", "Counter with CBC-MAC"),
    # ),
    "KEY": (
        ("SET", "SET Key"),
        ("GET", "GET Key"),
    ),
}

SPECIFIC_OPERATIONS_L1 = {
    "SE05X": (
        ("AuthECKey", "Authenticate via ECKey"),
        ("AuthSession",
         (
             "Allow creation of user/authenticated session.",
             "",
             "If the intended deployment only uses Platform SCP",
             "Or it is a pure session less integration, this can",
             "save some code size."
         )
        ),
        ("CREATE_DELETE_CRYPTOOBJ",
         (
             "Allow creation/deletion of Crypto Objects",
             "",
             "If disabled, new Crytpo Objects are neither created and",
             "old/existing Crypto Objects are not deleted.",
             "It is assumed that during provisioning phase, the required",
             "Crypto Objects are pre-created or they are never going to",
             "be needed."
         )
         ),
    ),
    "SW": (
        ("TESTCOUNTERPART",
         (
             "Used as a test counterpart",
             "",
             "e.g. Major part of the mebdTLS SSS layer is purely used for",
             "testing of Secure Element implementation, and can be avoided",
             "fully during many production scenarios.",
         )
         ),
    ),
}

NOT_AVAILABLE = {

}

FSL_FTR_START = r"""/*
 * Copyright 2018-2020 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef SSS_APIS_INC_FSL_SSS_FTR_H_
#define SSS_APIS_INC_FSL_SSS_FTR_H_

/* ************************************************************************** */
/* Defines                                                                    */
/* ************************************************************************** */

/* clang-format off */

"""

FSL_FTR_END = r"""
/* ========= Miscellaneous values : START =================== */

/* ECC Mode is available */
#cmakedefine01 SSS_HAVE_ECC

/* RSA is available */
#cmakedefine01 SSS_HAVE_RSA

/* TPM BARRETO_NAEHRIG Curve is enabled */
#cmakedefine01 SSS_HAVE_TPM_BN

/* Edwards Curve is enabled */
#cmakedefine01 SSS_HAVE_EC_ED

/* Montgomery Curve is enabled */
#cmakedefine01 SSS_HAVE_EC_MONT

/* TLS handshake support on SE is enabled */
#cmakedefine01 SSS_HAVE_TLS_HANDSHAKE

/* Import Export Key is enabled */
#cmakedefine01 SSS_HAVE_IMPORT

/* With NXP NFC Reader Library */
#cmakedefine01 SSS_HAVE_NXPNFCRDLIB

#define SSS_HAVE_A71XX \
    (SSS_HAVE_APPLET_A71CH | SSS_HAVE_APPLET_A71CH_SIM)

#define SSS_HAVE_SSCP  (SSS_HAVE_A71XX)

/* For backwards compatibility */
#define SSS_HAVE_TESTCOUNTERPART (SSSFTR_SW_TESTCOUNTERPART)

/* ========= Miscellaneous values : END ===================== */

/* ========= Calculated values : START ====================== */

/* Should we expose, SSS APIs */
#define SSS_HAVE_SSS ( 0             \
    + SSS_HAVE_SSCP                  \
    + SSS_HAVE_APPLET_SE05X_IOT      \
    + SSS_HAVE_HOSTCRYPTO_OPENSSL    \
    + SSS_HAVE_HOSTCRYPTO_MBEDCRYPTO \
    + SSS_HAVE_HOSTCRYPTO_MBEDTLS    \
    + SSS_HAVE_HOSTCRYPTO_USER       \
    )

/* MBEDCRYPTO is superset of MBEDTLS and exposing that way */
#if SSS_HAVE_HOSTCRYPTO_MBEDCRYPTO
#   undef SSS_HAVE_MBEDTLS
#   undef SSS_HAVE_HOSTCRYPTO_MBEDTLS

#   define SSS_HAVE_MBEDTLS 1
#   define SSS_HAVE_HOSTCRYPTO_MBEDTLS 1
#endif // SSS_HAVE_HOSTCRYPTO_MBEDCRYPTO

#if SSS_HAVE_HOSTCRYPTO_NONE
#   undef SSSFTR_SE05X_AuthSession
#   define SSSFTR_SE05X_AuthSession 0
#endif

/* Montgomery curves is not supported in SE05X_A*/
#if SSS_HAVE_APPLET_SE05X_A
#   undef SSS_HAVE_EC_MONT
#   define SSS_HAVE_EC_MONT 0
/* ED is not supported in SE050_A */
#if SSS_HAVE_SE05X_VER_03_XX
#   undef SSS_HAVE_EC_ED
#   define SSS_HAVE_EC_ED 0
#endif
#endif

/* ========= Calculated values : END ======================== */

/* clang-format on */

#endif /* SSS_APIS_INC_FSL_SSS_FTR_H_ */
"""


class CMakeFeatures:  # pylint: disable=too-few-public-methods
    """
    Generates skeleton files for various features for SSS Layer
    """

    def __init__(self, cm_val, cm_gv, cm_rst_v, cm_sh, cm_makin, cm_cmiin):  # pylint: disable=too-many-arguments
        self.cm_val = cm_val
        self.cm_rst_v = cm_rst_v
        self.fsl_f_in = None
        self.cm_gv = cm_gv
        self.cm_sh = cm_sh
        self.cm_makin = cm_makin
        self.cm_cmiin = cm_cmiin

    def open(self):
        self.fsl_f_in = open("../sss/inc/fsl_sss_ftr.h.in", "w")
        self.fsl_f_in.write(FSL_FTR_START)
        self.fsl_f_in.write("\n/* # CMake Features : Start */\n")

    def get_h_file(self):
        return self.fsl_f_in


    def close(self):
        self.fsl_f_in.write("\n/* # CMake Features : END */\n")
        self.fsl_f_in.write(FSL_FTR_END)
        self.fsl_f_in.close()

    def write_features(self):
        """
        Add cmake options
        :return: None
        """
        self.cm_rst_v.write(RSTV_HEADER_CMAKE_OPTIONS)
        self._add_values()
        self._set_composite_values()

    def _add_values(self):
        self.cm_val.write("\n# Add Options\n")
        self._header_log("Feature selection/values")
        for l, l_desc in LAYERS:
            self.cm_val.write("# %s\n" % (l,))
            for e, e_desc in COMMON_FTR_L1:
                if e in COMMON_FTR_L2:
                    self._add_sub_entry(e, l, l_desc, e_desc)
                else:
                    self._add_plain_entry(e, l, e_desc, l_desc)
            if l in SPECIFIC_OPERATIONS_L1:
                for e, e_desc in SPECIFIC_OPERATIONS_L1[l]:
                    if e in COMMON_FTR_L2:
                        self._add_sub_entry(e, l, l_desc, e_desc)
                    else:
                        self._add_plain_entry(e, l, e_desc, l_desc)

    def _header_log(self, text):
        self.fsl_f_in.write("\n")
        self.fsl_f_in.write("/* %s *\n" % ("=" * 70,))
        buffer_text = "=" * (70 - 4 - len(text))
        self.fsl_f_in.write(" * == %s %s *\n" % (text, buffer_text))
        self.fsl_f_in.write(" * %s */\n" % ("=" * 70,))
        self.fsl_f_in.write("\n")

    def _set_composite_values(self):
        self.cm_val.write("\n# Add Values\n")
        self._header_log("Computed Options")
        for e, e_desc in COMMON_FTR_L1:
            if e in COMMON_FTR_L2:
                self._set_composite_sub_entry(e, e_desc)
            else:
                self._set_composite_plain_entry(e, e_desc)
        for l, l_desc in LAYERS:  # pylint: disable=unused-variable
            for e, e_desc in COMMON_FTR_L1:
                if e in COMMON_FTR_L2:
                    options = []
                    for e2, e2_desc in COMMON_FTR_L2[e]:  # pylint: disable=unused-variable
                        options.append("SSSFTR_%s_%s_%s" % (l, e, e2))
                    self._set_or_options(options, l + "_" + e, e_desc)

    def _set_composite_sub_entry(self, e, e_desc):
        for se, se_desc in COMMON_FTR_L2[e]:
            se_e = "%s_%s" % (e, se)
            self._set_composite_plain_entry(se_e, e_desc + " : " + se_desc)
        options = []
        for se, _ in COMMON_FTR_L2[e]:
            options.append("SSSFTR_%s_%s" % (e, se))
        self._set_or_options(options, e, e_desc)

    def _set_composite_plain_entry(self, e, se_desc):
        options = []
        for l, _ in LAYERS:
            options.append("SSSFTR_%s_%s" % (l, e))
        self._set_or_options(options, e, se_desc)

    def _set_or_options(self, options, e, se_desc):
        self.cm_val.write("# %s\n" % (se_desc,))
        self.cm_val.write("IF(")
        self.cm_val.write(" OR ".join(options))
        self.cm_val.write(")\n")
        self.cm_val.write("    SET(SSSFTR_%s ON)\n" % (e,))
        self.cm_val.write("ELSE()\n")
        self.cm_val.write("    SET(SSSFTR_%s OFF)\n" % (e,))
        self.cm_val.write("ENDIF()\n")
        self.fsl_f_in.write("/** %s */\n" % (se_desc,))
        self.fsl_f_in.write("#define SSSFTR_%s %s(%s)\n" % (
            e, " " * (17 - len(e)), " + ".join(options)))

    def _add_sub_entry(self, e, l, l_desc, e_desc):
        for se, se_desc in COMMON_FTR_L2[e]:
            se_e = "%s_%s" % (e, se)
            self._add_plain_entry(se_e, l, se_desc, e_desc, l_desc)

    @classmethod
    def _rstrip_string(cls, s):
        sa = s.split('\n')
        sar = [line.rstrip() for line in sa]
        sars = '\n'.join(sar)
        return sars

    def _add_plain_entry(self, e, l, full_description, p_desc=None, pp_desc=None):  # pylint: disable=too-many-arguments
        sssftr = "SSSFTR_%s_%s" % (l, e)
        if sssftr in NOT_AVAILABLE:
            default_value = "OFF"
        else:
            default_value = "ON"
        if tuple == type(full_description):
            short_description = full_description[0]
            rst_description = self._rstrip_string("\n    ".join(full_description))
            sh_description = self._rstrip_string("\n# ".join(full_description))
            c_description = self._rstrip_string("\n * ".join(full_description))

        else:
            short_description = full_description
            rst_description = full_description
            sh_description = full_description
            c_description = full_description
        if p_desc:
            short_description = "%s : %s" % (p_desc, short_description)
            rst_description = "%s : %s" % (p_desc, rst_description)
            sh_description = "%s : %s" % (p_desc, sh_description)
            c_description = "%s : %s" % (p_desc, c_description)
        if pp_desc:
            short_description = "%s : %s" % (pp_desc, short_description)
            rst_description = "%s : %s" % (pp_desc, rst_description)
            sh_description = "%s : %s" % (pp_desc, sh_description)
            c_description = "%s : %s" % (pp_desc, c_description)

        self.cm_val.write("SET(%s %s\n" % (sssftr, default_value))
        self.cm_val.write("    CACHE BOOL\n")
        self.cm_val.write("    \"%s\")\n" % (short_description,))
        self.cm_rst_v.write(".. option:: %s\n" % (sssftr,))
        self.cm_rst_v.write("\n")
        self.cm_rst_v.write("    %s\n\n" % (rst_description,))
        self.fsl_f_in.write("\n/** %s */\n" % (c_description,))
        self.cm_gv.write("do%s_ON= '-D%s=ON'\n" % (sssftr, sssftr))
        self.cm_gv.write("do%s_OFF= '-D%s=OFF'\n" % (sssftr, sssftr))
        for e in (self.cm_sh, self.cm_makin, self.cm_cmiin):
            e.write("# %s\n" % (sh_description,))
        self.cm_sh.write("do%s_ON=\"-D%s=ON\"\n" % (sssftr, sssftr))
        self.cm_makin.write("%s := ${%s}\n" % (sssftr, sssftr))
        self.cm_cmiin.write("SET(%s ${%s})\n" % (sssftr, sssftr))
        self.cm_sh.write("do%s_OFF=\"-D%s=OFF\"\n" % (sssftr, sssftr))
        self.fsl_f_in.write("#cmakedefine01 %s\n" % (sssftr,))


RSTV_HEADER_CMAKE_OPTIONS = r"""

.. _sssftr-control:

Feature Control
=========================

Using these options, you can enable/disable individual features.

See :numref:`ftr-file-reduce-code-size` :ref:`ftr-file-reduce-code-size`
for details on it's usage and relevance.

"""

if __name__ == '__main__':
    import cmake_options

    cmake_options.main()
