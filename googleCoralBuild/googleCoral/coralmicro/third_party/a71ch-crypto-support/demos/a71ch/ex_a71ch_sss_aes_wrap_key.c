/*
 *
 * Copyright 2018-2020 NXP
 * SPDX-License-Identifier: Apache-2.0
 */

/* ************************************************************************** */
/* Includes                                                                   */
/* ************************************************************************** */

#include <a71ch_api.h>
#include <ex_sss.h>
#include <ex_sss_boot.h>
#include <fsl_sss_se05x_apis.h>
#include <nxEnsure.h>
#include <nxLog_App.h>

/* ************************************************************************** */
/* Local Defines                                                              */
/* ************************************************************************** */
#define EC_KEY_BIT_LEN 256
/* ************************************************************************** */
/* Structures and Typedefs                                                    */
/* ************************************************************************** */

/* ************************************************************************** */
/* Global Variables                                                           */
/* ************************************************************************** */

/* clang-format off */
const uint8_t aesKey[32] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                           0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
                           0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
                           0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
                         };

const uint8_t aesKey1[32] = { 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27,
                               0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f,
                               0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
                               0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f,
                             };

/* wrapped first 16 bytes of aesKey1 key with first 16 bytes of aesKey*/
const uint8_t wapped_AesKey1_0[24] = { 0xb2, 0x68, 0x75, 0x57, 0xa1, 0x4e, 0xc8, 0xa8,
                                          0x2f, 0xe0, 0x7b, 0x8d, 0xc1, 0xe6, 0xf9, 0xf9,
                                          0x8c, 0xe7, 0x4a, 0x4b, 0xf5, 0x7, 0xf3, 0xa9
                                        };
/* wrapped last 16 bytes of aesKey1 key with last 16 bytes of aesKey*/
const uint8_t wapped_AesKey1_1[24] = { 0x6b, 0x53, 0x16, 0xe4, 0x72, 0x64, 0x02, 0x18,
                                          0x7f, 0x7c, 0x1e, 0x5b, 0x70, 0x16, 0xab, 0xab,
                                          0xa2, 0x35, 0xc8, 0xd2, 0x06, 0xaa, 0xb3, 0x82
                                        };

/* clang-format on */

static ex_sss_boot_ctx_t gex_sss_aes_wrap_key_boot_ctx;

/* ************************************************************************** */
/* Static function declarations                                               */
/* ************************************************************************** */

/* ************************************************************************** */
/* Private Functions                                                          */
/* ************************************************************************** */

/* ************************************************************************** */
/* Public Functions                                                           */
/* ************************************************************************** */

#define EX_SSS_BOOT_PCONTEXT (&gex_sss_aes_wrap_key_boot_ctx)
#define EX_SSS_BOOT_DO_ERASE 1
#define EX_SSS_BOOT_EXPOSE_ARGC_ARGV 0

#include <ex_sss_main_inc.h>
#include "a71ch_util.h"
#include "sm_types.h"
#include "tst_sm_util.h"

sss_status_t calculate_hkdf_key(
    ex_sss_boot_ctx_t *pCtx, sss_object_t aeskeyObject, uint32_t hkdfKeyId, uint8_t *outHkdfKey, size_t *outHkdfKeyLen);

sss_status_t ex_sss_entry(ex_sss_boot_ctx_t *pCtx)
{
    sss_status_t status = kStatus_SSS_Success;
    sss_object_t aesObj1;
    sss_object_t aesObj2;
    uint8_t HkdfKey1[128] = {
        0,
    };
    size_t HkdfKey1Len    = sizeof(HkdfKey1);
    uint8_t HkdfKey2[128] = {
        0,
    };
    size_t HkdfKey2Len = sizeof(HkdfKey2);

    // Variables used by calls to legacy API
    U16 sw             = 0;
    SST_Index_t keyIdx = 0;

    LOG_I(
        "Running Set AES Wrapped Key Example "
        "ex_a71ch_sss_aes_wrap_key.c");

    /* doc:start ex_sss-set-aes-key */
    status = sss_key_object_init(&aesObj1, &pCtx->ks);
    ENSURE_OR_GO_CLEANUP(status == kStatus_SSS_Success);

    status = sss_key_object_allocate_handle(&aesObj1,
        MAKE_TEST_ID(__LINE__),
        kSSS_KeyPart_Default,
        kSSS_CipherType_AES,
        sizeof(aesKey),
        kKeyObject_Mode_Persistent);
    ENSURE_OR_GO_CLEANUP(status == kStatus_SSS_Success);

    status = sss_key_store_set_key(&pCtx->ks, &aesObj1, aesKey, sizeof(aesKey), sizeof(aesKey) * 8, NULL, 0);
    ENSURE_OR_GO_CLEANUP(status == kStatus_SSS_Success);
    /* doc:end ex_sss-set-aes-key */

    /* doc:start ex_hostlib-set-wrapped-aes-key */
    keyIdx = (((sss_sscp_object_t *)&aesObj1)->slotId) & 0x0F;

    /* Set wrapped aes key - aesKey1 */
    sw     = A71_SetRfc3394WrappedAesKey(keyIdx, wapped_AesKey1_0, sizeof(wapped_AesKey1_0));
    status = ((sw == SW_OK) ? kStatus_SSS_Success : kStatus_SSS_Fail);
    ENSURE_OR_GO_CLEANUP(status == kStatus_SSS_Success);

    sw     = A71_SetRfc3394WrappedAesKey(keyIdx + 1, wapped_AesKey1_1, sizeof(wapped_AesKey1_1));
    status = ((sw == SW_OK) ? kStatus_SSS_Success : kStatus_SSS_Fail);
    ENSURE_OR_GO_CLEANUP(status == kStatus_SSS_Success);
    /* doc:end ex_hostlib-set-wrapped-aes-key */

    /* Verify if the wrapped key is injected properly */

    /* doc:start ex_verify-wrapped-key */
    /* 1 - Calculate HKDF key with wrapped AES key injected - aesKey1 */
    status = calculate_hkdf_key(pCtx, aesObj1, MAKE_TEST_ID(__LINE__), HkdfKey1, &HkdfKey1Len);

    /* 2 - Inject aesKey1 AES key and calculate HKDF key */
    status = sss_key_object_init(&aesObj2, &pCtx->ks);
    ENSURE_OR_GO_CLEANUP(status == kStatus_SSS_Success);

    status = sss_key_object_allocate_handle(&aesObj2,
        MAKE_TEST_ID(__LINE__),
        kSSS_KeyPart_Default,
        kSSS_CipherType_AES,
        sizeof(aesKey1),
        kKeyObject_Mode_Persistent);
    ENSURE_OR_GO_CLEANUP(status == kStatus_SSS_Success);

    status = sss_key_store_set_key(&pCtx->ks, &aesObj2, aesKey1, sizeof(aesKey1), sizeof(aesKey1) * 8, NULL, 0);
    ENSURE_OR_GO_CLEANUP(status == kStatus_SSS_Success);

    status = calculate_hkdf_key(pCtx, aesObj2, MAKE_TEST_ID(__LINE__), HkdfKey2, &HkdfKey2Len);

    /* 3 - compare both hkdf keys generated */
    if (0 != memcmp(HkdfKey1, HkdfKey2, HkdfKey1Len)) {
        status = kStatus_SSS_Fail;
    }
    /* doc:end ex_verify-wrapped-key */

cleanup:
    if (kStatus_SSS_Success == status) {
        LOG_I("ex_a71ch_sss_aes_wrap_key Example Success !!!...");
    }
    else {
        LOG_E("ex_a71ch_sss_aes_wrap_key Example Failed !!!...");
    }
    return status;
}

sss_status_t calculate_hkdf_key(
    ex_sss_boot_ctx_t *pCtx, sss_object_t aeskeyObject, uint32_t hkdfKeyId, uint8_t *outHkdfKey, size_t *outHkdfKeyLen)
{
    sss_status_t status;
    /* HKDF Extract and Expand*/
    sss_algorithm_t algorithm                                = kAlgorithm_SSS_HMAC_SHA256;
    uint8_t info[DERIVE_KEYDATA_FROM_SHARED_SECRET_MAX_INFO] = {
        0,
    };
    uint8_t infoLen = sizeof(info);
    sss_object_t derived_key;
    sss_derive_key_t ctx_derv = {0};
    uint16_t deriveDataLen    = 128;

    /* clang-format off */
    static uint8_t salt[32] = { 0xAA, 0x1A, 0x2A, 0xE3, 0xB2, 0x76, 0x15, 0x4D,
        0x67, 0xF9, 0xD8, 0x4C, 0xB9, 0x35, 0x54, 0x56,
        0xBB, 0x1B, 0x2B, 0x03, 0x04, 0x05, 0x06, 0x07,
        0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
    };
    /* clang-format on */

    /* Store derived key in host */
    status = sss_key_object_init(&derived_key, &pCtx->host_ks);
    ENSURE_OR_GO_CLEANUP(status == kStatus_SSS_Success);

    status = sss_key_object_allocate_handle(
        &derived_key, hkdfKeyId, kSSS_KeyPart_Default, kSSS_CipherType_AES, *outHkdfKeyLen, kKeyObject_Mode_Transient);
    ENSURE_OR_GO_CLEANUP(status == kStatus_SSS_Success);

    status =
        sss_derive_key_context_init(&ctx_derv, &pCtx->session, &aeskeyObject, algorithm, kMode_SSS_ComputeSharedSecret);
    ENSURE_OR_GO_CLEANUP(status == kStatus_SSS_Success);

    status = sss_derive_key_go(
        &ctx_derv, salt, sizeof(salt), info, infoLen, &derived_key, deriveDataLen, outHkdfKey, outHkdfKeyLen);

#if SSS_HAVE_FIPS
    ENSURE_OR_GO_CLEANUP(status != kStatus_SSS_Success);
    status = kStatus_SSS_Success;
    goto cleanup;
#else

    size_t hkdfKeyLenBits = *outHkdfKeyLen * 8;
    ENSURE_OR_GO_CLEANUP(status == kStatus_SSS_Success);

    status = sss_key_store_get_key(&pCtx->host_ks, &derived_key, outHkdfKey, outHkdfKeyLen, &hkdfKeyLenBits);
    ENSURE_OR_GO_CLEANUP(status == kStatus_SSS_Success);

    LOG_I(" Key Derivation successful !!!");
    LOG_MAU8_I("hkdfOutput", outHkdfKey, *outHkdfKeyLen);
#endif

cleanup:
    if (ctx_derv.session != NULL)
        sss_derive_key_context_free(&ctx_derv);
    return status;
}
