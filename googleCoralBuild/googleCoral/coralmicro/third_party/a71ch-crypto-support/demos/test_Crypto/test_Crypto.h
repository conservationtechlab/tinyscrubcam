/*
 * Copyright 2020 NXP
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef TST_CRYPTO_H_INC
#define TST_CRYPTO_H_INC
#include <stdio.h>
#include <string.h>

#include "fsl_sss_api.h"
#include "fsl_sss_user_apis.h"
#include "fsl_sss_mbedtls_apis.h"
#include "fsl_sss_openssl_apis.h"
#include "nxLog_App.h"

typedef struct userCtx
{
    sss_session_t session;
    sss_key_store_t ks;
    sss_object_t key;
    sss_rng_context_t rng;
    sss_symmetric_t symm;
    sss_mac_t mac;
} userCtx_t;

#endif
