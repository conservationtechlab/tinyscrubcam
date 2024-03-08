/**
 * \file mbedtls_wiced_config.h
 *
 * \brief Configuration options (set of defines)
 *
 *  This set of compile-time options may be used to enable
 *  or disable features selectively, and reduce the global
 *  memory footprint.
 */
/*
 *  Copyright (C) 2006-2018, ARM Limited, All Rights Reserved
 *  SPDX-License-Identifier: Apache-2.0
 *
 *  Licensed under the Apache License, Version 2.0 (the "License"); you may
 *  not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 *  WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *  This file is part of mbed TLS (https://tls.mbed.org)
 */

/* This configuration file is specific to enable/disable some of the MBEDTLS features to run MBEDTLS stack on low
 * memory WICED platforms. Major changes done to original 2.16.3 MBEDTLS as below.
 *
 * 1) Modified MBEDTLS security stack to use packets from network stack buffer pool (ex. NetX_Duo) to send and receive packets
 *    which are of MTU size. If received packet is more than MTU size, then defragmentation buffer is used. As by default MBEDTLS stack
 *    during initialization requires ~16K of input and output buffers to send & receive packets and that is not practical to use it
 *    for low memory platforms.
 * 2) Enabled Macros MBEDTLS_AES_ROM_TABLES & MBEDTLS_AES_FEWER_TABLES to save some code space with compromise on performance.
 *    The performance gain/loss depends on the system and memory details.
 * 3) Integrated micro-ECC with MBEDTLS for ECC operations which improves TLS handshake timings for ECC cipher suite. by default
 *    MBEDTLS uses its own ECC crypto.
 * 4) Added SHA1, SHA256 & SHA512 HMAC functions.
 * 5) Fixed coverity issues.
 * 6) Added helper functions for DTLS.
 *
 */

#include "wiced_defaults.h"

/* Provided MBEDTLS implementation only works POSIX/Unix (including Linux,
 * BSD and OS X) and Windows. Hence disabled */
#undef MBEDTLS_TIMING_C
#undef MBEDTLS_NET_C

/* Dont enable MBEDTLS_HAVE_TIME & MBEDTLS_HAVE_TIME_DATE. As we dont support ALT implementation for
 * both of them and internally using own time APIs */
#undef MBEDTLS_HAVE_TIME
#undef MBEDTLS_HAVE_TIME_DATE

/* Disabled to save code space */
#undef MBEDTLS_FS_IO
#undef MBEDTLS_SSL_RENEGOTIATION
#undef MBEDTLS_SELF_TEST

/* Though MBEDTLS supports various PSK cipher suites, WICED TLS interface doesnt have support for
 * loading PSK parameters. Hence disabled PSK related cipher suites for TLS. DTLS support
 * PSK and hence it is only enabled for DTLS */
#undef MBEDTLS_KEY_EXCHANGE_DHE_PSK_ENABLED
#undef MBEDTLS_KEY_EXCHANGE_ECDHE_PSK_ENABLED
#undef MBEDTLS_KEY_EXCHANGE_RSA_PSK_ENABLED

/* Disabled to save some code space */
#undef MBEDTLS_PK_RSA_ALT_SUPPORT
#undef MBEDTLS_DEBUG_C
#undef MBEDTLS_CERTS_C
#undef MBEDTLS_PLATFORM_C
#undef MBEDTLS_BLOWFISH_C

/* Enabling this macro improves TLS handshake performance. If this macro is enabled
 * mbedtls will invoke uECC functions for ECC crypto operations, else it will use the
 * mbedtls ecc functions. uECC does not support advanced security curves, so if
 * this flag is enabled advanced security curves are not supported.
 * Also this flag will not have any impact with deterministic ecdsa signing.
 * if MBEDTLS_ECDSA_DETERMINISTIC flag is enabled mbedtls ecc functions are used.
 */
#ifdef WICED_CONFIG_ENABLE_MBEDTLS_ECC_ALT
#define MBEDTLS_WICED_ECC_ALT
#endif

/* Enabling below macro improves the TLS handshake performance for DHE cipher suites. If this macro is enabled,
 * private key exponent x, required to compute g^X mod p in DHE operation, will be 256 bits(32 bytes).To get the
 * 128 bit of symmetric security 256 bits of private key sufficient. Disable flag, if requirement for symmetric
 * key size > 128 bits. Below are the TLS handshake time taken with local server with and without enabling below flag
 *
 *   TLS-DHE-RSA-WITH-AES-256-CBC-SHA256 ( Disabling below flag ) - 1842 ms
 *   TLS-DHE-RSA-WITH-AES-256-CBC-SHA256 ( Enabling below flag )  - 723 ms
 */
#define MBEDTLS_WICED_FAST_DHE

/* Disable some of curves to save memory */
#undef  MBEDTLS_ECP_DP_SECP384R1_ENABLED
#undef  MBEDTLS_ECP_DP_SECP521R1_ENABLED
#undef  MBEDTLS_ECP_DP_SECP192K1_ENABLED
#undef  MBEDTLS_ECP_DP_SECP224K1_ENABLED
#undef  MBEDTLS_ECP_DP_BP256R1_ENABLED
#undef  MBEDTLS_ECP_DP_BP384R1_ENABLED
#undef  MBEDTLS_ECP_DP_BP512R1_ENABLED
#define MBEDTLS_ECP_DP_CURVE25519_ENABLED
#undef  MBEDTLS_ECP_DP_CURVE448_ENABLED
#define MBEDTLS_ECP_DP_SECP192R1_ENABLED
#define MBEDTLS_ECP_DP_SECP224R1_ENABLED
#define MBEDTLS_ECP_DP_SECP256R1_ENABLED
#define MBEDTLS_ECP_DP_SECP256K1_ENABLED

/* Enable advance eliptic curves */
#ifndef WICED_CONFIG_DISABLE_ADVANCED_SECURITY_CURVES
#define MBEDTLS_ECP_DP_BP256R1_ENABLED
#define MBEDTLS_ECP_DP_SECP384R1_ENABLED
#endif

/* Refer wiced_defaults.h to configure WICED_TLS_MINOR_VERSION_MIN and WICED_TLS_MINOR_VERSION_MAX */
#undef MBEDTLS_SSL_PROTO_TLS1
#if (WICED_TLS_MINOR_VERSION_MIN == 0)
#define MBEDTLS_SSL_PROTO_TLS1      /* TLSv1_0 */
#endif

#undef MBEDTLS_SSL_PROTO_TLS1_1
#if ( ((WICED_TLS_MINOR_VERSION_MIN <= 1) && (WICED_TLS_MINOR_VERSION_MAX >= 1)) )
#define MBEDTLS_SSL_PROTO_TLS1_1    /* TLSv1_1 */
#endif

#undef MBEDTLS_SSL_PROTO_TLS1_2
#if ( ((WICED_TLS_MINOR_VERSION_MIN <= 2) && (WICED_TLS_MINOR_VERSION_MAX >= 2)) )
#define MBEDTLS_SSL_PROTO_TLS1_2   /* TLSv1_2 */
#endif


#undef MBEDTLS_SSL_PROTO_DTLS
#undef MBEDTLS_SSL_DTLS_HELLO_VERIFY
#undef MBEDTLS_SSL_COOKIE_C
#undef MBEDTLS_SSL_ALL_ALERT_MESSAGES
#undef MBEDTLS_SSL_DTLS_ANTI_REPLAY
#undef MBEDTLS_SSL_DTLS_CLIENT_PORT_REUSE
#undef MBEDTLS_SSL_DTLS_BADMAC_LIMIT
#undef MBEDTLS_KEY_EXCHANGE_PSK_ENABLED

/* Define WICED_CONFIG_DISABLE_DTLS to remove DTLS feature and save some memory */
#ifndef WICED_CONFIG_DISABLE_DTLS
#define MBEDTLS_SSL_PROTO_DTLS
#define MBEDTLS_SSL_DTLS_HELLO_VERIFY
#define MBEDTLS_SSL_COOKIE_C
#define MBEDTLS_SSL_ALL_ALERT_MESSAGES
#define MBEDTLS_SSL_DTLS_ANTI_REPLAY
#define MBEDTLS_SSL_DTLS_CLIENT_PORT_REUSE
#define MBEDTLS_SSL_DTLS_BADMAC_LIMIT
#define MBEDTLS_KEY_EXCHANGE_PSK_ENABLED
#endif /* WICED_CONFIG_DISABLE_DTLS */

#ifdef PLATFORM_HAS_HW_CRYPTO_SUPPORT
#ifndef WICED_CONFIG_DONOT_USE_HW_CRYPTO
#define MBEDTLS_AES_ALT
/* Crypto driver doesnt support XTS and OFB modes of AES, Hence cannot be used with HW crypto */
#undef  MBEDTLS_CIPHER_MODE_XTS
#undef  MBEDTLS_CIPHER_MODE_OFB
#define MBEDTLS_DES_ALT
#define MBEDTLS_MD5_ALT
#define MBEDTLS_SHA256_ALT
#define MBEDTLS_SHA1_ALT
#define MBEDTLS_SELF_TEST
#endif /* #ifndef WICED_CONFIG_DONOT_USE_HW_CRYPTO */
#endif /* PLATFORM_HAS_HW_CRYPTO_SUPPORT */

#undef MBEDTLS_SSL_CLI_C
#ifndef WICED_CONFIG_DISABLE_SSL_CLIENT
#define MBEDTLS_SSL_CLI_C
#endif

#undef MBEDTLS_SSL_SRV_C
#undef MBEDTLS_SSL_CACHE_C
#ifndef WICED_CONFIG_DISABLE_SSL_SERVER
#define MBEDTLS_SSL_SRV_C
#define MBEDTLS_SSL_CACHE_C
#endif

/* To use the session resumption feature of TLS, apps just need to enable WICED_TLS_CLI_CACHE_SESSION flag without
 * worrying about storing connection info. BESL library takes care of storing the connection info(ip, port, session info)
 * and resuming the connections. Number of entries to be stored is determined by WICED_TLS_CLI_CACHE_ENTRIES macro.
 */
//#define WICED_TLS_CLI_CACHE_SESSION

#ifdef WICED_TLS_CLI_CACHE_SESSION

#ifndef WICED_TLS_CLI_CACHE_ENTRIES
#define WICED_TLS_CLI_CACHE_ENTRIES 16
#endif

#endif

/**
 * \def MBEDTLS_AES_ROM_TABLES
 *
 * Use precomputed AES tables stored in ROM.
 *
 * Uncomment this macro to use precomputed AES tables stored in ROM.
 * Comment this macro to generate AES tables in RAM at runtime.
 *
 * Tradeoff: Using precomputed ROM tables reduces RAM usage by ~8kb
 * (or ~2kb if \c MBEDTLS_AES_FEWER_TABLES is used) and reduces the
 * initialization time before the first AES operation can be performed.
 * It comes at the cost of additional ~8kb ROM use (resp. ~2kb if \c
 * MBEDTLS_AES_FEWER_TABLES below is used), and potentially degraded
 * performance if ROM access is slower than RAM access.
 *
 * This option is independent of \c MBEDTLS_AES_FEWER_TABLES.
 *
 */
#define MBEDTLS_AES_ROM_TABLES

/**
 * \def MBEDTLS_AES_FEWER_TABLES
 *
 * Use less ROM/RAM for AES tables.
 *
 * Uncommenting this macro omits 75% of the AES tables from
 * ROM / RAM (depending on the value of \c MBEDTLS_AES_ROM_TABLES)
 * by computing their values on the fly during operations
 * (the tables are entry-wise rotations of one another).
 *
 * Tradeoff: Uncommenting this reduces the RAM / ROM footprint
 * by ~6kb but at the cost of more arithmetic operations during
 * runtime. Specifically, one has to compare 4 accesses within
 * different tables to 4 accesses with additional arithmetic
 * operations within the same table. The performance gain/loss
 * depends on the system and memory details.
 *
 * This option is independent of \c MBEDTLS_AES_ROM_TABLES.
 *
 */
#define MBEDTLS_AES_FEWER_TABLES


/* Save RAM by adjusting to our exact needs */
#define MBEDTLS_ECP_MAX_BITS    512
#define MBEDTLS_MPI_MAX_SIZE    1024 // 384 bits is 48 bytes

/* Save RAM at the expense of speed, see ecp.h */
#define MBEDTLS_ECP_WINDOW_SIZE        4
#define MBEDTLS_ECP_FIXED_POINT_OPTIM  0

/* Significant speed benefit at the expense of some ROM */
//#define MBEDTLS_ECP_NIST_OPTIM - enabled

/*
 * You should adjust this to the exact number of sources you're using: default
 * is the "mbedtls_platform_entropy_poll" source, but you may want to add other ones.
 * Minimum is 2 for the entropy test suite.
 */
#define MBEDTLS_ENTROPY_MAX_SOURCES 2

/**
 * \def MBEDTLS_ENTROPY_HARDWARE_ALT
 *
 * Uncomment this macro to let mbed TLS use your own implementation of a
 * hardware entropy collector.
 *
 * Your function must be called \c mbedtls_hardware_poll(), have the same
 * prototype as declared in entropy_poll.h, and accept NULL as first argument.
 *
 * Uncomment to use your own hardware entropy collector.
 */
#define MBEDTLS_ENTROPY_HARDWARE_ALT

/**
 * \def MBEDTLS_NO_PLATFORM_ENTROPY
 *
 * Do not use built-in platform entropy functions.
 * This is useful if your platform does not support
 * standards like the /dev/urandom or Windows CryptoAPI.
 *
 * Uncomment this macro to disable the built-in platform entropy functions.
 */
#define MBEDTLS_NO_PLATFORM_ENTROPY

/* Save ROM and a few bytes of RAM by specifying our own ciphersuite list
 * Currently with DTLS only MBEDTLS_TLS_PSK_WITH_AES_128_CCM_8 and MBEDTLS_TLS_ECDHE_ECDSA_WITH_AES_128_CCM_8 cipher suites are enabled.
 * PSK cipher suite is not supported for TLS.
 * */

#define MBEDTLS_SSL_CIPHERSUITES                        \
    MBEDTLS_TLS_ECDHE_RSA_WITH_AES_256_GCM_SHA384,      \
    MBEDTLS_TLS_ECDHE_ECDSA_WITH_AES_128_CCM_8,         \
    MBEDTLS_TLS_PSK_WITH_AES_128_CCM_8,                 \
    MBEDTLS_TLS_DHE_RSA_WITH_AES_256_CBC_SHA256,        \
    MBEDTLS_TLS_DHE_RSA_WITH_CAMELLIA_256_CBC_SHA256,   \
    MBEDTLS_TLS_DHE_RSA_WITH_AES_256_CBC_SHA,           \
    MBEDTLS_TLS_DHE_RSA_WITH_CAMELLIA_256_CBC_SHA,      \
    MBEDTLS_TLS_DHE_RSA_WITH_AES_128_CBC_SHA256,        \
    MBEDTLS_TLS_DHE_RSA_WITH_CAMELLIA_128_CBC_SHA256,   \
    MBEDTLS_TLS_DHE_RSA_WITH_AES_128_CBC_SHA,           \
    MBEDTLS_TLS_DHE_RSA_WITH_CAMELLIA_128_CBC_SHA,      \
    MBEDTLS_TLS_ECDHE_ECDSA_WITH_AES_256_GCM_SHA384,    \
    MBEDTLS_TLS_ECDHE_ECDSA_WITH_AES_128_GCM_SHA256,    \
    MBEDTLS_TLS_ECDHE_ECDSA_WITH_AES_256_CCM_8,         \
    MBEDTLS_TLS_ECDHE_ECDSA_WITH_AES_128_CBC_SHA256,    \
    MBEDTLS_TLS_ECDHE_ECDSA_WITH_AES_128_CBC_SHA,       \
    MBEDTLS_TLS_ECDHE_ECDSA_WITH_AES_256_CBC_SHA,       \
    MBEDTLS_TLS_ECDH_ECDSA_WITH_AES_128_CBC_SHA,        \
    MBEDTLS_TLS_ECDH_ECDSA_WITH_AES_256_CBC_SHA,        \
    MBEDTLS_TLS_ECDHE_RSA_WITH_AES_128_GCM_SHA256,      \
    MBEDTLS_TLS_RSA_WITH_AES_256_CBC_SHA256,            \
    MBEDTLS_TLS_RSA_WITH_CAMELLIA_256_CBC_SHA256,       \
    MBEDTLS_TLS_RSA_WITH_AES_256_CBC_SHA,               \
    MBEDTLS_TLS_RSA_WITH_CAMELLIA_256_CBC_SHA,          \
    MBEDTLS_TLS_RSA_WITH_AES_128_CBC_SHA256,            \
    MBEDTLS_TLS_RSA_WITH_CAMELLIA_128_CBC_SHA256,       \
    MBEDTLS_TLS_RSA_WITH_AES_128_CBC_SHA,               \
    MBEDTLS_TLS_RSA_WITH_CAMELLIA_128_CBC_SHA

/**
 * Allow SHA-1 in the default TLS configuration for certificate signing.
 * Without this build-time option, SHA-1 support must be activated explicitly
 * through mbedtls_ssl_conf_cert_profile. Turning on this option is not
 * recommended because of it is possible to generte SHA-1 collisions, however
 * this may be safe for legacy infrastructure where additional controls apply.
 */
#define MBEDTLS_TLS_DEFAULT_ALLOW_SHA1_IN_CERTIFICATES

/* Un-comment below macros to enable MBEDTLS for debug log
 * MBEDTLS_DEBUG_C
 * MBEDTLS_SSL_DEBUG_ALL
 * MBEDTLS_DEBUG_LOG_LEVEL
 * Also Enable WPRINT_SECURITY_DEBUG in wiced_defaults.h
 */
//#define MBEDTLS_DEBUG_C

//#define MBEDTLS_SSL_DEBUG_ALL

/* MBEDTLS Different Debug log levels
 *  - 0 No debug
 *  - 1 Error
 *  - 2 State change
 *  - 3 Informational
 *  - 4 Verbose
 */
#define MBEDTLS_DEBUG_LOG_LEVEL 3

/* TODO :MD4 is considered as weak digest and used by BESL supplicant
 * hence enabled MD4. Should fix BESL supplicant to use other strong digest
 */
#define MBEDTLS_MD4_C

#if defined(MBEDTLS_HAVE_TIME) || defined(MBEDTLS_HAVE_TIME_DATE)
#error "Dont enable MBEDTLS_HAVE_TIME or MBEDTLS_HAVE_TIME_DATE. Platform doesnt support MBEDTLS time functions and internally using own time functions"
#endif
