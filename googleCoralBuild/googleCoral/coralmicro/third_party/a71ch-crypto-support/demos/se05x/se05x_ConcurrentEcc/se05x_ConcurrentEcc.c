/*
 *
 * Copyright 2018, 2019, 2020 NXP
 * SPDX-License-Identifier: Apache-2.0
 */

/* ************************************************************************** */
/* Includes                                                                   */
/* ************************************************************************** */

#include <ex_sss.h>
#include <ex_sss_boot.h>
#include <fsl_sss_se05x_apis.h>
#include <nxEnsure.h>
#include <nxLog_App.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

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

uint32_t gAuthid = 0;
uint32_t gKeyid = 0;
char *gportName = 0;
uint32_t gLoopCount = 0;


/* clang-format off */
const uint8_t keyPairData[] = { 0x30, 0x81, 0x87, 0x02, 0x01, 0x00, 0x30, 0x13,
    0x06, 0x07, 0x2A, 0x86, 0x48, 0xCE, 0x3D, 0x02,
    0x01, 0x06, 0x08, 0x2A, 0x86, 0x48, 0xCE, 0x3D,
    0x03, 0x01, 0x07, 0x04, 0x6D, 0x30, 0x6B, 0x02,
    0x01, 0x01, 0x04, 0x20, 0x78, 0xE5, 0x20, 0x6A,
    0x08, 0xED, 0xD2, 0x52, 0x36, 0x33, 0x8A, 0x24,
    0x84, 0xE4, 0x2F, 0x1F, 0x7D, 0x1F, 0x6D, 0x94,
    0x37, 0xA9, 0x95, 0x86, 0xDA, 0xFC, 0xD2, 0x23,
    0x6F, 0xA2, 0x87, 0x35, 0xA1, 0x44, 0x03, 0x42,
    0x00, 0x04, 0xED, 0xA7, 0xE9, 0x0B, 0xF9, 0x20,
    0xCF, 0xFB, 0x9D, 0xF6, 0xDB, 0xCE, 0xF7, 0x20,
    0xE1, 0x23, 0x8B, 0x3C, 0xEE, 0x84, 0x86, 0xD2,
    0x50, 0xE4, 0xDF, 0x30, 0x11, 0x50, 0x1A, 0x15,
    0x08, 0xA6, 0x2E, 0xD7, 0x49, 0x52, 0x78, 0x63,
    0x6E, 0x61, 0xE8, 0x5F, 0xED, 0xB0, 0x6D, 0x87,
    0x92, 0x0A, 0x04, 0x19, 0x14, 0xFE, 0x76, 0x63,
    0x55, 0xDF, 0xBD, 0x68, 0x61, 0x59, 0x31, 0x8E,
    0x68, 0x7C };

const uint8_t extPubKeyData[] = {
    0x30, 0x59, 0x30, 0x13, 0x06, 0x07, 0x2A, 0x86,
    0x48, 0xCE, 0x3D, 0x02, 0x01, 0x06, 0x08, 0x2A,
    0x86, 0x48, 0xCE, 0x3D, 0x03, 0x01, 0x07, 0x03,
    0x42, 0x00, 0x04, 0xED, 0xA7, 0xE9, 0x0B, 0xF9,
    0x20, 0xCF, 0xFB, 0x9D, 0xF6, 0xDB, 0xCE, 0xF7,
    0x20, 0xE1, 0x23, 0x8B, 0x3C, 0xEE, 0x84, 0x86,
    0xD2, 0x50, 0xE4, 0xDF, 0x30, 0x11, 0x50, 0x1A,
    0x15, 0x08, 0xA6, 0x2E, 0xD7, 0x49, 0x52, 0x78,
    0x63, 0x6E, 0x61, 0xE8, 0x5F, 0xED, 0xB0, 0x6D,
    0x87, 0x92, 0x0A, 0x04, 0x19, 0x14, 0xFE, 0x76,
    0x63, 0x55, 0xDF, 0xBD, 0x68, 0x61, 0x59, 0x31,
    0x8E, 0x68, 0x7C
};

/* clang-format on */

static ex_sss_boot_ctx_t gex_ecc_cc_boot_ctx;

/* ************************************************************************** */
/* Static function declarations                                               */
/* ************************************************************************** */

static sss_status_t parse_command_line_args(int argc, const char **argv);
/* ************************************************************************** */
/* Private Functions                                                          */
/* ************************************************************************** */

/* ************************************************************************** */
/* Public Functions                                                           */
/* ************************************************************************** */

#define EX_SSS_BOOT_PCONTEXT (&gex_ecc_cc_boot_ctx)
#define EX_SSS_BOOT_DO_ERASE 0
#define EX_SSS_BOOT_EXPOSE_ARGC_ARGV 1

#include <ex_sss_main_cc_inc.h>

void usage()
{
    LOG_W(
        "\n\
    usage:\n\
    se05x_ConcurrentEcc.exe \n\
        -authid <auth object id to open Session >\n\
        -keyid <Key id to store Key>\n\
        -cnt <no of times to loop the operation >\n \
        -port <port to be connected >\n");
    return;
}

sss_status_t ex_sss_entry(ex_sss_boot_ctx_t *pCtx)
{
    sss_status_t status = kStatus_SSS_Success;
    int argc = gex_sss_argc;
    const char **argv = gex_sss_argv;
    uint8_t digest[32] = { 0 };
    size_t digestLen = sizeof(digest);
    uint8_t idigestcnt = 0;
    uint8_t signature[256] = { 0 };
    size_t signatureLen;
    sss_object_t keyPair;
    sss_object_t key_pub;
    sss_asymmetric_t ctx_asymm = { 0 };
    sss_asymmetric_t ctx_verify = { 0 };
    uint32_t uLoopCnt = 0;
    uint32_t uLooplimit = 200;

    LOG_I("\n\n");
    LOG_I("Running Elliptic Curve Cryptography Example se05x_ConcurrentEcc");
    LOG_I("\n\n");

    /* Parse commandline arguments */
    status = parse_command_line_args(argc, argv);
    ENSURE_OR_GO_EXIT(status == kStatus_SSS_Success);
    const char *portName = gportName;

    if (gLoopCount != 0)
    {
        /*Accept the loopcount from user*/
        uLooplimit = gLoopCount;
    }

    /*Open session at given Auth ID*/
    status = ex_sss_boot_open_on_id(pCtx, portName, gAuthid);

    status = ex_sss_key_store_and_object_init((pCtx));
    if (kStatus_SSS_Success != status) {
        LOG_E("ex_sss_key_store_and_object_init Failed");
        goto exit;
    }

    //keep the crypto operations in loop
    for (uLoopCnt = 0; uLoopCnt < uLooplimit; uLoopCnt++)
    {
        LOG_I("LoopCount (loop = %d)\n", uLoopCnt);
        /* Pre-requisite for Signing Part*/
        status = sss_key_object_init(&keyPair, &pCtx->ks);
        ENSURE_OR_GO_CLEANUP(status == kStatus_SSS_Success);

        status = sss_key_object_allocate_handle(&keyPair,
            gKeyid,
            kSSS_KeyPart_Pair,
            kSSS_CipherType_EC_NIST_P,
            sizeof(keyPairData),
            kKeyObject_Mode_Persistent);
        ENSURE_OR_GO_CLEANUP(status == kStatus_SSS_Success);

        status = sss_key_store_set_key(&pCtx->ks, &keyPair, keyPairData, sizeof(keyPairData), EC_KEY_BIT_LEN, NULL, 0);
        ENSURE_OR_GO_CLEANUP(status == kStatus_SSS_Success);


        status = sss_asymmetric_context_init(&ctx_asymm, &pCtx->session, &keyPair, kAlgorithm_SSS_SHA256, kMode_SSS_Sign);
        ENSURE_OR_GO_CLEANUP(status == kStatus_SSS_Success);

        signatureLen = sizeof(signature);
        /* Do Signing */
        LOG_I("Do Signing");

        /* Changing  the digest each iteration */
        for (idigestcnt = 0; idigestcnt < 32; idigestcnt++)
        {
            digest[idigestcnt] = idigestcnt;
        }

        LOG_MAU8_I("digest", digest, digestLen);
        status = sss_asymmetric_sign_digest(&ctx_asymm, digest, digestLen, signature, &signatureLen);
        ENSURE_OR_GO_CLEANUP(status == kStatus_SSS_Success);
        LOG_MAU8_I("signature", signature, signatureLen);
        LOG_I("Signing Successful !!!");
        sss_asymmetric_context_free(&ctx_asymm);

        /* Pre requiste for Verifying Part*/
        status = sss_key_object_init(&key_pub, &pCtx->ks);
        ENSURE_OR_GO_CLEANUP(status == kStatus_SSS_Success);

        status = sss_key_object_allocate_handle(&key_pub,
            MAKE_TEST_ID(__LINE__),
            kSSS_KeyPart_Public,
            kSSS_CipherType_EC_NIST_P,
            sizeof(extPubKeyData),
            kKeyObject_Mode_Persistent);
        ENSURE_OR_GO_CLEANUP(status == kStatus_SSS_Success);

        status = sss_key_store_set_key(&pCtx->ks, &key_pub, extPubKeyData, sizeof(extPubKeyData), EC_KEY_BIT_LEN, NULL, 0);
        ENSURE_OR_GO_CLEANUP(status == kStatus_SSS_Success);

        status =
            sss_asymmetric_context_init(&ctx_verify, &pCtx->session, &key_pub, kAlgorithm_SSS_SHA256, kMode_SSS_Verify);
        ENSURE_OR_GO_CLEANUP(status == kStatus_SSS_Success);


        LOG_MAU8_I("digest", digest, digestLen);
        LOG_MAU8_I("signature", signature, signatureLen);
        status = sss_asymmetric_verify_digest(&ctx_verify, digest, digestLen, signature, signatureLen);
        ENSURE_OR_GO_CLEANUP(status == kStatus_SSS_Success);
        LOG_I("Verification Successful !!!");
        sss_asymmetric_context_free(&ctx_asymm);
        sss_asymmetric_context_free(&ctx_verify);
    }


cleanup:
    if (kStatus_SSS_Success == status) {
        LOG_I("se05x_ConcurrentEcc Example Success !!!...");
    }
    else {
        LOG_E("se05x_ConcurrentEcc Example Failed !!!...");
    }
    if (ctx_asymm.session != NULL)
        sss_asymmetric_context_free(&ctx_asymm);
    if (ctx_verify.session != NULL)
        sss_asymmetric_context_free(&ctx_verify);
exit:
    return status;
}


static sss_status_t parse_command_line_args(int argc, const char **argv)
{
    bool authid_passed = false;
    bool keyid_passed = false;
    bool port_passed = false;

    for (int j = 1; j < argc; j++) {
        if (strcmp(argv[j], "-authid") == 0) {
            if (++j < argc) {
                gAuthid = (uint32_t)strtoul(argv[j], NULL, 16);
                authid_passed = true;
            }
        }
        else if (strcmp(argv[j], "-keyid") == 0) {
            if (++j < argc) {
                gKeyid = (uint32_t)strtoul(argv[j], NULL, 16);
                keyid_passed = true;
            }
        }
        else if (strcmp(argv[j], "-cnt") == 0) {
            if (++j < argc) {
                gLoopCount = (uint32_t)strtoul(argv[j], NULL, 10);
            }
        }
        else if (strcmp(argv[j], "-port") == 0) {
            if (++j < argc) {
                gportName = (char *)argv[j];
                port_passed = true;
            }
        }
    }
    if (!authid_passed) {
        LOG_E("Auth key Id not passed");
        usage();
        return kStatus_SSS_Fail;
    }
    if (!keyid_passed) {
        LOG_E("Object key Id  not passed");
        usage();
        return kStatus_SSS_Fail;
    }
    if (!port_passed) {
        LOG_E("Port  not passed");
        usage();
        return kStatus_SSS_Fail;
    }

    return kStatus_SSS_Success;
}
