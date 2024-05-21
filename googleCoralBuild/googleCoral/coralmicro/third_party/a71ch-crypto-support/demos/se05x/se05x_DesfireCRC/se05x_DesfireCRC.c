/*
*
* Copyright 2019,2020 NXP
* SPDX-License-Identifier: Apache-2.0
*/

#include "se05x_DesfireCRC.h"

#include <ex_sss_boot.h>
#include <nxLog_App.h>
#include <se05x_APDU.h>
#include <se05x_tlv.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <nxEnsure.h>

#include "smCom.h"

static ex_sss_boot_ctx_t gex_sss_features_boot_ctx;

#define EX_SSS_BOOT_PCONTEXT (&gex_sss_features_boot_ctx)
#define EX_SSS_BOOT_DO_ERASE 0
#define EX_SSS_BOOT_EXPOSE_ARGC_ARGV 1

#include <ex_sss_main_inc.h>

sss_status_t ex_sss_entry(ex_sss_boot_ctx_t *pCtx)
{
    sss_status_t status;
    sss_object_t desfire_object   = {0};
    uint32_t desfire_object_keyid = 0x7FFF020B;

    status = sss_key_object_init(&desfire_object, &pCtx->ks);
    ENSURE_OR_GO_CLEANUP(status == kStatus_SSS_Success);
    status = sss_key_object_get_handle(&desfire_object, desfire_object_keyid);
    ENSURE_OR_GO_CLEANUP(status == kStatus_SSS_Success);
    status = sss_key_store_erase_key(&pCtx->ks, &desfire_object);
    ENSURE_OR_GO_CLEANUP(status == kStatus_SSS_Fail);

    /** Close session and Open session with feature ID */
    sss_session_close(&pCtx->session);
    status = session_open_with_feature_id(pCtx);
    if (status != kStatus_SSS_Success) {
        goto cleanup;
    }

    status = sss_key_object_init(&desfire_object, &pCtx->ks);
    ENSURE_OR_GO_CLEANUP(status == kStatus_SSS_Success);
    status = sss_key_object_get_handle(&desfire_object, desfire_object_keyid);
    ENSURE_OR_GO_CLEANUP(status == kStatus_SSS_Success);
    status = sss_key_store_erase_key(&pCtx->ks, &desfire_object);
    ENSURE_OR_GO_CLEANUP(status == kStatus_SSS_Success);

cleanup:
    if (kStatus_SSS_Success == status) {
        LOG_I("se05x_DesfireCRC Example Success !!!...");
    }
    else {
        LOG_E("se05x_DesfireCRC Example Failed !!!...");
    }
    return status;
}

sss_status_t session_open_with_feature_id(ex_sss_boot_ctx_t *pCtx)
{
    sss_status_t status = kStatus_SSS_Fail;
#if (SSS_HAVE_SE05X_AUTH_ECKEY_PLATFSCP03)
    SE05x_Connect_Ctx_t *pchannlCtxt = &pCtx->se05x_open_ctx;
    pchannlCtxt->auth.authType       = kSSS_AuthType_ECKey;
    ex_SE05x_authCtx_t ex_se05x_auth;

    status = ex_sss_se05x_prepare_host_EX(
        &pCtx->host_session, &pCtx->host_ks, pchannlCtxt, &ex_se05x_auth, kSSS_AuthType_ECKey);
    if (kStatus_SSS_Success != status) {
        LOG_E("ex_sss_se05x_prepare_host_EX failed");
        goto cleanup;
    }

    pchannlCtxt->connType  = kType_SE_Conn_Type_Channel;
    pchannlCtxt->tunnelCtx = pCtx->pTunnel_ctx;

    status = sss_session_open(
        &pCtx->session, kType_SSS_SE_SE05x, kSE05x_AppletResID_FEATURE, kSSS_ConnectionType_Encrypted, pchannlCtxt);
    if (kStatus_SSS_Success != status) {
        LOG_E("sss_session_open failed");
    }
#else
    sss_session_t *pSession          = NULL;
    SE05x_Connect_Ctx_t *pConnectCtx = NULL;
    pConnectCtx                      = &pCtx->se05x_open_ctx;
    pSession                         = &pCtx->session;
    status                           = ex_sss_se05x_prepare_host_EX(
        &pCtx->host_session, &pCtx->host_ks, pConnectCtx, &pCtx->ex_se05x_auth, kSSS_AuthType_ECKey);
    if (kStatus_SSS_Success != status) {
        LOG_E("ex_sss_se05x_prepare_host_EX failed");
        goto cleanup;
    }

    status = sss_session_open(
        pSession, kType_SSS_SE_SE05x, kSE05x_AppletResID_FEATURE, kSSS_ConnectionType_Encrypted, pConnectCtx);
    if (kStatus_SSS_Success != status) {
        LOG_E("sss_session_open failed");
    }
#endif

cleanup:
    return status;
}

sss_status_t ex_sss_se05x_prepare_host_EX(sss_session_t *host_session,
    sss_key_store_t *host_ks,
    SE05x_Connect_Ctx_t *se05x_open_ctx,
    ex_SE05x_authCtx_t *se05x_auth_ctx,
    SE_AuthType_t auth_type)
{
    sss_status_t status = kStatus_SSS_Fail;

    if (host_session->subsystem == kType_SSS_SubSystem_NONE) {
        sss_type_t hostsubsystem = kType_SSS_SubSystem_NONE;

#if SSS_HAVE_MBEDTLS
        hostsubsystem = kType_SSS_mbedTLS;
#endif

#if SSS_HAVE_OPENSSL
        hostsubsystem = kType_SSS_OpenSSL;
#endif
        status = sss_session_open(host_session, hostsubsystem, 0, kSSS_ConnectionType_Plain, NULL);
        if (kStatus_SSS_Success != status) {
            LOG_E("Failed to open Host Session");
            goto cleanup;
        }
        status = sss_key_store_context_init(host_ks, host_session);
        if (kStatus_SSS_Success != status) {
            LOG_E("Host: sss_key_store_context_init failed");
            goto cleanup;
        }
        status = sss_key_store_allocate(host_ks, __LINE__);
        if (kStatus_SSS_Success != status) {
            LOG_E("Host: sss_key_store_allocate failed");
            goto cleanup;
        }
    }
    switch (auth_type) {
#if SSS_HAVE_SCP_SCP03_SSS
    case kSSS_AuthType_ECKey:
        status = ex_sss_se05x_prepare_host_eckey_EX(&se05x_open_ctx->auth.ctx.eckey, se05x_auth_ctx, host_ks);
        break;
#endif
    default:
        status = kStatus_SSS_Fail;
        LOG_E("Not handled");
    }

    if (kStatus_SSS_Success != status) {
        LOG_E(
            "Host: ex_sss_se05x_prepare_host_<type=(SE_AuthType_t)%d> "
            "failed",
            auth_type);
        goto cleanup;
    }
    se05x_open_ctx->auth.authType = auth_type;

cleanup:
    return status;
}

sss_status_t ex_sss_se05x_prepare_host_eckey_EX(
    SE05x_AuthCtx_ECKey_t *pAuthCtx, ex_SE05x_authCtx_t *pEx_auth, sss_key_store_t *pKs)
{
    sss_status_t status        = kStatus_SSS_Fail;
    uint8_t hostEcdsakey[1024] = {0};

    int argc          = gex_sss_argc;
    const char **argv = gex_sss_argv;

    for (int j = 1; j < argc; j++) {
        if (strcmp(argv[j], "-file") == 0) {
            j++;
            const char *filename = argv[j];
            FILE *fp             = fopen(filename, "rb");
            if (fp == NULL) {
                LOG_E("Cannot open file");
                return kStatus_SSS_Fail;
            }
            fread(hostEcdsakey, sizeof(hostEcdsakey), 1, fp);
        }
    }

    NXECKey03_StaticCtx_t *pStatic_ctx = pAuthCtx->pStatic_ctx;
    NXSCP03_DynCtx_t *pDyn_ctx         = pAuthCtx->pDyn_ctx;

    /* Init allocate Host ECDSA Key pair */
    status =
        Alloc_ECKeykey_toSE05xAuthctx_EX(&pStatic_ctx->HostEcdsaObj, pKs, MAKE_TEST_ID(__LINE__), kSSS_KeyPart_Pair);
    /* Set Host ECDSA Key pair */
    status = sss_key_store_set_key(pKs, &pStatic_ctx->HostEcdsaObj, hostEcdsakey, sizeof(hostEcdsakey), 256, NULL, 0);
    if (status == kStatus_SSS_Fail) {
        return status;
    }

    /* Init allocate Host ECKA Key pair */
    status =
        Alloc_ECKeykey_toSE05xAuthctx_EX(&pStatic_ctx->HostEcKeypair, pKs, MAKE_TEST_ID(__LINE__), kSSS_KeyPart_Pair);
    /* Generate Host EC Key pair */
    status = sss_key_store_generate_key(pKs, &pStatic_ctx->HostEcKeypair, 256, NULL);
    if (status != kStatus_SSS_Success) {
        return status;
    }

    /* Init allocate SE ECKA Public Key */
    status =
        Alloc_ECKeykey_toSE05xAuthctx_EX(&pStatic_ctx->SeEcPubKey, pKs, MAKE_TEST_ID(__LINE__), kSSS_KeyPart_Public);
    if (status != kStatus_SSS_Success) {
        return status;
    }

    /* Init Allocate Master Secret */
    status = Alloc_Scp03key_toSE05xAuthctx_EX(&pStatic_ctx->masterSec, pKs, MAKE_TEST_ID(__LINE__));
    if (status != kStatus_SSS_Success) {
        return status;
    }

    /* Init Allocate ENC Session Key */
    status = Alloc_Scp03key_toSE05xAuthctx_EX(&pDyn_ctx->Enc, pKs, MAKE_TEST_ID(__LINE__));
    if (status != kStatus_SSS_Success) {
        return status;
    }
    /* Init Allocate MAC Session Key */
    status = Alloc_Scp03key_toSE05xAuthctx_EX(&pDyn_ctx->Mac, pKs, MAKE_TEST_ID(__LINE__));
    if (status != kStatus_SSS_Success) {
        return status;
    }
    /* Init Allocate DEK Session Key */
    status = Alloc_Scp03key_toSE05xAuthctx_EX(&pDyn_ctx->Rmac, pKs, MAKE_TEST_ID(__LINE__));

    return status;
}

sss_status_t Alloc_ECKeykey_toSE05xAuthctx_EX(
    sss_object_t *keyObject, sss_key_store_t *pKs, uint32_t keyId, sss_key_part_t keypart)
{
    sss_status_t status = kStatus_SSS_Fail;
    status              = sss_key_object_init(keyObject, pKs);
    if (status != kStatus_SSS_Success) {
        return status;
    }
    status = sss_key_object_allocate_handle(
        keyObject, keyId, keypart, kSSS_CipherType_EC_NIST_P, 256, kKeyObject_Mode_Persistent);
    return status;
}

sss_status_t Alloc_Scp03key_toSE05xAuthctx_EX(sss_object_t *keyObject, sss_key_store_t *pKs, uint32_t keyId)
{
    sss_status_t status = kStatus_SSS_Fail;
    status              = sss_key_object_init(keyObject, pKs);
    if (status != kStatus_SSS_Success) {
        return status;
    }

    status = sss_key_object_allocate_handle(
        keyObject, keyId, kSSS_KeyPart_Default, kSSS_CipherType_AES, 16, kKeyObject_Mode_Transient);
    return status;
}
