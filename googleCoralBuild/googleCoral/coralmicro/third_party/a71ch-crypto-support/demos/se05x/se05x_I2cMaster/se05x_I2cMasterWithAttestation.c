/* Copyright 2019 NXP
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
#include <se05x_const.h>
#include <string.h>

/* ************************************************************************** */
/* Local Defines                                                              */
/* ************************************************************************** */
#define I2C_SENSOR_BUS_ADDRESS 0x1D /* I2C bus address of sensor */
#define KEYSIZE 256
#define MAX_SIGNATURE_LEN 256
#define I2C_MAX_DATA 32
#define FXOS8700CQ_STATUS 0x00
#define FXOS8700CQ_WHOAMI 0x0D
#define FXOS8700CQ_XYZ_DATA_CFG 0x0E
#define FXOS8700CQ_CTRL_REG1 0x2A
#define FXOS8700CQ_WHOAMI_VAL 0xC7
#define ACCEL_READ_TIMES 100

/* ************************************************************************** */
/* Static Defines                                                              */
/* ************************************************************************** */
static ex_sss_boot_ctx_t gex_sss_boot_ctx;

/* ************************************************************************** */
/* Global Variables                                                           */
/* ************************************************************************** */
#define EX_SSS_BOOT_PCONTEXT (&gex_sss_boot_ctx)
#define EX_SSS_BOOT_DO_ERASE 1
#define EX_SSS_BOOT_EXPOSE_ARGC_ARGV 0

SE05x_I2CM_cmd_t TLV[5];

#include <ex_sss_main_inc.h>
/* ************************************************************************** */
/* Static function declarations                                               */
/* ************************************************************************** */
static sss_status_t i2cm_ConfigureAttestation(ex_sss_boot_ctx_t *pCtx, sss_object_t *key)
{
    sss_status_t ret = kStatus_SSS_Success;
    //sss_object_t &key;
    size_t keylen                    = KEYSIZE;
    uint32_t keyId                   = __LINE__;
    const sss_policy_u key_withPol   = {.type = KPolicy_Asym_Key,
        .auth_obj_id                        = 0,
        .policy                             = {.asymmkey = {
                       .can_Sign          = 1,
                       .can_Verify        = 1,
                       .can_Encrypt       = 1,
                       .can_Decrypt       = 1,
                       .can_KD            = 1,
                       .can_Wrap          = 1,
                       .can_Write         = 1,
                       .can_Gen           = 1,
                       .can_Import_Export = 1,
                       .can_KA            = 1,
                       .can_Read          = 1,
                       .can_Attest        = 1,
                   }}};
    const sss_policy_u common        = {.type = KPolicy_Common,
        .auth_obj_id                   = 0,
        .policy                        = {.common = {
                       .can_Delete = 1,
                       .forbid_All = 0,
                   }}};
    sss_policy_t policy_for_asym_key = {.nPolicies = 2, .policies = {&key_withPol, &common}};
    ret                              = sss_key_object_init(key, &pCtx->ks);
    ENSURE_OR_GO_CLEANUP(kStatus_SSS_Success == ret);

    ret = sss_key_object_allocate_handle(
        key, keyId, kSSS_KeyPart_Pair, kSSS_CipherType_EC_NIST_P, keylen, kKeyObject_Mode_Persistent);
    ENSURE_OR_GO_CLEANUP(kStatus_SSS_Success == ret);

    /*ret = sss_key_object_get_handle(&key, keyId);
    if (ret == kStatus_SSS_Success) {
        LOG_W("KeyId 0x%8X Already Exsist", keyId);
        ret = sss_key_store_erase_key(&pCtx->ks, &key);
        ENSURE_OR_GO_CLEANUP(ret == kStatus_SSS_Success);
    }*/

    ENSURE_OR_GO_CLEANUP(kStatus_SSS_Success == ret);

    ret = sss_key_object_allocate_handle(
        key, keyId, kSSS_KeyPart_Pair, kSSS_CipherType_EC_NIST_P, keylen, kKeyObject_Mode_Persistent);
    ENSURE_OR_GO_CLEANUP(kStatus_SSS_Success == ret);

    /*ret = sss_key_object_get_handle(&key, keyId);
    if (ret == kStatus_SSS_Success) {
        LOG_W("KeyId 0x%8X Already Exsist", keyId);
        ret = sss_key_store_erase_key(&pCtx->ks, &key);
        ENSURE_OR_GO_CLEANUP(ret == kStatus_SSS_Success);
    }*/

    ret = sss_key_store_generate_key(&pCtx->ks, key, keylen, &policy_for_asym_key);
    ENSURE_OR_GO_CLEANUP(kStatus_SSS_Success == ret);

    return kStatus_SSS_Success;
cleanup:
    return kStatus_SSS_Fail;
}

static smStatus_t i2cm_AttestedWrite(ex_sss_boot_ctx_t *pCtx,
    uint8_t *random,
    size_t randomLen,
    sss_object_t *key,
    SE05x_AttestationAlgo_t attest_algo,
    uint8_t *writebuf,
    uint32_t writeLength)
{
    smStatus_t status;
    size_t iCnt;
    uint8_t outrandom[16] = {0};
    /* clang-format on */
    size_t outrandomLen                        = sizeof(outrandom);
    uint8_t chipId[SE050_MODULE_UNIQUE_ID_LEN] = {0};
    size_t chipIdLen                           = sizeof(chipId);
    uint8_t signature[256]                     = {0};
    size_t signatureLen                        = MAX_SIGNATURE_LEN;
    SE05x_TimeStamp_t timeStamp;
    size_t timeStampLen         = sizeof(timeStamp);
    TLV[0].type                 = kSE05x_I2CM_Configure;
    TLV[0].cmd.cfg.I2C_addr     = I2C_SENSOR_BUS_ADDRESS;
    TLV[0].cmd.cfg.I2C_baudRate = kSE05x_I2CM_Baud_Rate_400Khz;

    TLV[1].type              = kSE05x_I2CM_Write;
    TLV[1].cmd.w.writeLength = writeLength;
    TLV[1].cmd.w.writebuf    = writebuf;

    status = Se05x_i2c_master_attst_txn(&pCtx->session,
        key,
        &TLV[0],
        random,
        randomLen,
        attest_algo,
        &timeStamp,
        &timeStampLen,
        outrandom,
        &outrandomLen,
        chipId,
        &chipIdLen,
        signature,
        &signatureLen,
        2);
    ENSURE_OR_GO_EXIT(SE05X_I2CM_MAX_TIMESTAMP_SIZE == timeStampLen);
    ENSURE_OR_GO_EXIT(SE05X_I2CM_MAX_CHIP_ID_SIZE == chipIdLen);
    ENSURE_OR_GO_EXIT(SE05X_I2CM_MAX_FRESHNESS_SIZE == outrandomLen);
    for (iCnt = 0; iCnt < outrandomLen; iCnt++) {
        if (random[iCnt] == outrandom[iCnt])
            continue;
        else {
            LOG_E("failed to verify freshness data");
            return status;
        }
    }
    return status;
exit:
    return SM_NOT_OK;
}
static smStatus_t i2cm_AttestedRead(ex_sss_boot_ctx_t *pCtx,
    uint8_t *random,
    size_t randomLen,
    sss_object_t *key,
    SE05x_AttestationAlgo_t attest_algo,
    uint8_t *readbuf,
    uint32_t readLength)
{
    smStatus_t status;
    uint32_t iCnt;
    uint8_t outrandom[16] = {0};
    /* clang-format on */
    size_t outrandomLen                        = sizeof(outrandom);
    uint8_t chipId[SE050_MODULE_UNIQUE_ID_LEN] = {0};
    size_t chipIdLen                           = sizeof(chipId);
    uint8_t signature[256]                     = {0};
    size_t signatureLen                        = MAX_SIGNATURE_LEN;
    SE05x_TimeStamp_t timeStamp;
    size_t timeStampLen         = sizeof(timeStamp);
    TLV[0].type                 = kSE05x_I2CM_Configure;
    TLV[0].cmd.cfg.I2C_addr     = I2C_SENSOR_BUS_ADDRESS;
    TLV[0].cmd.cfg.I2C_baudRate = kSE05x_I2CM_Baud_Rate_400Khz;

    TLV[1].type              = kSE05x_I2CM_Read;
    TLV[1].cmd.rd.readLength = readLength;
    TLV[1].cmd.rd.rdBuf      = readbuf;

    status = Se05x_i2c_master_attst_txn(&pCtx->session,
        key,
        &TLV[0],
        random,
        randomLen,
        attest_algo,
        &timeStamp,
        &timeStampLen,
        outrandom,
        &outrandomLen,
        chipId,
        &chipIdLen,
        signature,
        &signatureLen,
        2);
    ENSURE_OR_GO_EXIT(SE05X_I2CM_MAX_TIMESTAMP_SIZE == timeStampLen);
    ENSURE_OR_GO_EXIT(SE05X_I2CM_MAX_CHIP_ID_SIZE == chipIdLen);
    ENSURE_OR_GO_EXIT(SE05X_I2CM_MAX_FRESHNESS_SIZE == outrandomLen);
    for (iCnt = 0; iCnt < outrandomLen; iCnt++) {
        if (random[iCnt] == outrandom[iCnt])
            continue;
        else {
            LOG_E("failed to verify freshness data");
            return status;
        }
    }
    return status;
exit:
    return SM_NOT_OK;
}
static smStatus_t i2cm_AttestedWriteRead(ex_sss_boot_ctx_t *pCtx,
    uint8_t *random,
    size_t randomLen,
    sss_object_t *key,
    SE05x_AttestationAlgo_t attest_algo,
    uint8_t *writebuf,
    uint32_t writeLength,
    uint8_t *readbuf,
    uint32_t readLength)
{
    smStatus_t status;
    uint32_t iCnt;
    uint8_t outrandom[16] = {0};
    /* clang-format on */
    size_t outrandomLen                        = sizeof(outrandom);
    uint8_t chipId[SE050_MODULE_UNIQUE_ID_LEN] = {0};
    size_t chipIdLen                           = sizeof(chipId);
    uint8_t signature[256]                     = {0};
    size_t signatureLen                        = MAX_SIGNATURE_LEN;
    SE05x_TimeStamp_t timeStamp;
    size_t timeStampLen         = sizeof(timeStamp);
    TLV[0].type                 = kSE05x_I2CM_Configure;
    TLV[0].cmd.cfg.I2C_addr     = I2C_SENSOR_BUS_ADDRESS;
    TLV[0].cmd.cfg.I2C_baudRate = kSE05x_I2CM_Baud_Rate_400Khz;

    TLV[1].type              = kSE05x_I2CM_Write;
    TLV[1].cmd.w.writeLength = writeLength;
    TLV[1].cmd.w.writebuf    = writebuf;

    TLV[2].type              = kSE05x_I2CM_Read;
    TLV[2].cmd.rd.readLength = readLength;
    TLV[2].cmd.rd.rdBuf      = readbuf;

    status = Se05x_i2c_master_attst_txn(&pCtx->session,
        key,
        &TLV[0],
        random,
        randomLen,
        attest_algo,
        &timeStamp,
        &timeStampLen,
        outrandom,
        &outrandomLen,
        chipId,
        &chipIdLen,
        signature,
        &signatureLen,
        3);
    ENSURE_OR_GO_EXIT(SE05X_I2CM_MAX_TIMESTAMP_SIZE == timeStampLen);
    ENSURE_OR_GO_EXIT(SE05X_I2CM_MAX_CHIP_ID_SIZE == chipIdLen);
    ENSURE_OR_GO_EXIT(SE05X_I2CM_MAX_FRESHNESS_SIZE == outrandomLen);
    for (iCnt = 0; iCnt < outrandomLen; iCnt++) {
        if (random[iCnt] == outrandom[iCnt])
            continue;
        else {
            LOG_E("failed to verify freshness data");
            return status;
        }
    }
    return status;
exit:
    return SM_NOT_OK;
}

/* ************************************************************************** */
/* Public Functions                                                           */
/* ************************************************************************** */

sss_status_t ex_sss_entry(ex_sss_boot_ctx_t *pCtx)
{
    smStatus_t status;
    sss_status_t ret;
    sss_object_t key;
    uint8_t pAcclwhoAmIreg[]  = {0x0D};
    uint8_t pAcclCtrlReg1[]   = {0x2A, 0x00};
    uint8_t pAcclCtrlReg2[]   = {0x2A, 0x0D, 0x00};
    uint8_t pAcclxyzDataCfg[] = {0x0E, 0x01};
    uint8_t pacclRead[I2C_MAX_DATA];
    uint8_t status_value = 0;
    uint32_t i           = 0;
#if LOG_INFO_ENABLED
    int16_t x, y, z;
#endif
    SE05x_AttestationAlgo_t attest_algo = kSE05x_AttestationAlgo_EC_SHA_512;
    uint8_t random[16]                  = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0xa, 0xb, 0xc, 0xd, 0xe, 0xf};
    size_t randomLen                    = sizeof(random);

    LOG_I("I2CM example to read attested Accelerometer value");
    /* ------------------------------------------------------------------------------------------------------------------- */
    ret = i2cm_ConfigureAttestation(pCtx, &key);
    ENSURE_OR_GO_EXIT(kStatus_SSS_Success == ret);

    status = i2cm_AttestedWriteRead(
        pCtx, random, randomLen, &key, attest_algo, pAcclwhoAmIreg, sizeof(pAcclwhoAmIreg), pacclRead, 1);
    ENSURE_OR_GO_EXIT(SM_OK == status);
    ENSURE_OR_GO_EXIT(kSE05x_I2CM_Success == TLV[1].cmd.w.wrStatus);

    status = i2cm_AttestedWrite(pCtx, random, randomLen, &key, attest_algo, pAcclCtrlReg1, sizeof(pAcclCtrlReg1));
    ENSURE_OR_GO_EXIT(SM_OK == status);
    ENSURE_OR_GO_EXIT(kSE05x_I2CM_Success == TLV[1].cmd.w.wrStatus);

    status = i2cm_AttestedWrite(pCtx, random, randomLen, &key, attest_algo, pAcclxyzDataCfg, sizeof(pAcclxyzDataCfg));
    ENSURE_OR_GO_EXIT(SM_OK == status);
    ENSURE_OR_GO_EXIT(kSE05x_I2CM_Success == TLV[1].cmd.w.wrStatus);

    status = i2cm_AttestedWrite(pCtx, random, randomLen, &key, attest_algo, pAcclCtrlReg2, sizeof(pAcclCtrlReg2));
    ENSURE_OR_GO_EXIT(SM_OK == status);
    ENSURE_OR_GO_EXIT(kSE05x_I2CM_Success == TLV[1].cmd.w.wrStatus);

    for (i = 0; i < ACCEL_READ_TIMES; i++) {
        /*  wait for new data are ready. */
        while (status_value != 0xff) {
            status = i2cm_AttestedRead(pCtx, random, randomLen, &key, attest_algo, pacclRead, 1);
            ENSURE_OR_GO_EXIT(SM_OK == status);
            status_value = TLV[1].cmd.rd.rdBuf[0];
        }
        status = i2cm_AttestedRead(pCtx, random, randomLen, &key, attest_algo, pacclRead, 7);
        ENSURE_OR_GO_EXIT(SM_OK == status);

#if LOG_INFO_ENABLED
        x = ((int16_t)(((TLV[1].cmd.rd.rdBuf[1] * 256U) | TLV[1].cmd.rd.rdBuf[2]))) / 4U;
        y = ((int16_t)(((TLV[1].cmd.rd.rdBuf[3] * 256U) | TLV[1].cmd.rd.rdBuf[4]))) / 4U;
        z = ((int16_t)(((TLV[1].cmd.rd.rdBuf[5] * 256U) | TLV[1].cmd.rd.rdBuf[6]))) / 4U;

        LOG_I("x = %5d , y = %5d , z = %5d", x, y, z);
#endif
    }

    LOG_I("I2CM test completed !!!...");

    return kStatus_SSS_Success;

exit:
    return kStatus_SSS_Fail;
}
