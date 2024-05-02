/* Copyright 2019,2020 NXP
 * SPDX-License-Identifier: Apache-2.0
 */

#include <ex_sss_boot.h>
#include <fsl_sss_se05x_apis.h>
#include <nxLog_App.h>
#include <se05x_APDU.h>
#include <se05x_const.h>
#include <se05x_ecc_curves.h>
#include <se05x_ecc_curves_values.h>
#include <se05x_tlv.h>
#include <stdio.h>

#include "ex_se05x_WiFiKDF.h"

#define WIFI_PASSWRD "some-wifi-password"

static ex_sss_boot_ctx_t gex_sss_wifi_kdf;

#define EX_SSS_BOOT_PCONTEXT (&gex_sss_wifi_kdf)
#define EX_SSS_BOOT_DO_ERASE 1
#define EX_SSS_BOOT_EXPOSE_ARGC_ARGV 0

#include <ex_sss_main_inc.h>

sss_status_t ex_sss_entry(ex_sss_boot_ctx_t *pCtx)
{
    sss_status_t sss_status = kStatus_SSS_Fail;
    smStatus_t status;
    sss_se05x_session_t *pSession = (sss_se05x_session_t *)&pCtx->session;
    SE05x_Result_t pExists        = 0;
    const uint8_t wifi_password[] = WIFI_PASSWRD;

    status = Se05x_API_CheckObjectExists(&pSession->s_ctx, WIFI_OBJ_ID, &pExists);

    if (status == SM_OK && pExists == kSE05x_Result_SUCCESS) {
        status = Se05x_API_DeleteSecureObject(&pSession->s_ctx, WIFI_OBJ_ID);
        if (status != SM_OK) {
            LOG_W("Error, could not delete object. ");
            goto cleanup;
        }
    }
    LOG_I("Injecting wifi_password='%s'", wifi_password);
    status = Se05x_API_WriteSymmKey(&pSession->s_ctx,
        NULL,
        SE05x_MaxAttemps_UNLIMITED,
        WIFI_OBJ_ID,
        SE05x_KeyID_KEK_NONE,
        wifi_password,
        sizeof(wifi_password) - 1,
        kSE05x_INS_NA,
        kSE05x_SymmKeyType_HMAC);
    if (status != SM_OK) {
        LOG_E("Se05x_API_WriteSymmKey Failed");
        goto cleanup;
    }
    sss_status = kStatus_SSS_Success;

cleanup:
    return sss_status;
}
