/*
*
* Copyright 2019,2020 NXP
* SPDX-License-Identifier: Apache-2.0
*/

#include "ex_sss_boot.h"
#include "fsl_sss_api.h"
#include "fsl_sss_ftr.h"

sss_status_t session_open_with_feature_id(ex_sss_boot_ctx_t *pCtx);
sss_status_t ex_sss_se05x_prepare_host_EX(sss_session_t *host_session,
    sss_key_store_t *host_ks,
    SE_Connect_Ctx_t *se05x_open_ctx,
    ex_SE05x_authCtx_t *se05x_auth_ctx,
    SE_AuthType_t auth_type);
sss_status_t ex_sss_se05x_prepare_host_eckey_EX(
    SE05x_AuthCtx_ECKey_t *pAuthCtx, ex_SE05x_authCtx_t *pEx_auth, sss_key_store_t *pKs);
sss_status_t Alloc_ECKeykey_toSE05xAuthctx_EX(
    sss_object_t *keyObject, sss_key_store_t *pKs, uint32_t keyId, sss_key_part_t keypart);
sss_status_t Alloc_Scp03key_toSE05xAuthctx_EX(sss_object_t *keyObject, sss_key_store_t *pKs, uint32_t keyId);
