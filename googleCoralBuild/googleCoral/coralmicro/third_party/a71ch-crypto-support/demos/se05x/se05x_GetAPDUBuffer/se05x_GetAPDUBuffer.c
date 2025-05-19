/* Copyright 2020 NXP
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <fsl_sss_se05x_policy.h>
#include <nxLog_App.h>
#include <se05x_APDU_apis.h>
#include <string.h>

void getReadOnlyFilePolicy(uint8_t *pPolicyBuffer, size_t *pPolicyBufferLen);

smStatus_t fLogTransmitBuffer(Se05xSession_t *pwrite_apdubufferctx,
    const tlvHeader_t *hdr,
    uint8_t *cmdBuf,
    size_t cmdBufLen,
    uint8_t *rsp,
    size_t *rspLen,
    uint8_t hasle);

uint8_t gTxBuffer[SE05X_MAX_BUF_SIZE_CMD];
size_t gTxBufferLen;

int main()
{
    Se05xPolicy_t policy;
    uint32_t objectID                            = 0x11223344;
    uint16_t offset                              = 0;
    const uint8_t inputData[]                    = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    uint8_t policyBuffer[MAX_POLICY_BUFFER_SIZE] = {0};
    size_t policyBufferLen                       = sizeof(policyBuffer);
    size_t inputDataLen                          = sizeof(inputData);
    uint16_t length                              = (uint16_t)inputDataLen;

    getReadOnlyFilePolicy(policyBuffer, &policyBufferLen);
    policy.value     = policyBuffer;
    policy.value_len = policyBufferLen;

    /* doc:start:use-write-apdu-buf */
    Se05xSession_t write_apdubufferctx = {0};
    write_apdubufferctx.fp_TXn         = &fLogTransmitBuffer;

    Se05x_API_WriteBinary(&write_apdubufferctx, &policy, objectID, offset, length, inputData, inputDataLen);
    /* doc:end:use-write-apdu-buf */
    LOG_AU8_I(gTxBuffer, gTxBufferLen);

    return 0;
}

// Writes to gTxBuffer and gTxBufferLen
void getReadOnlyFilePolicy(uint8_t *pPolicyBuffer, size_t *pPolicyBufferLen)
{
    sss_status_t status;
    sss_policy_t policy          = {0};
    sss_policy_u binary          = {0};
    sss_policy_u common          = {0};
    sss_policy_file_u *pFile     = &binary.policy.file;
    sss_policy_common_u *pCommon = &common.policy.common;

    LOG_I("Policy for ReadOnly File:");

    policy.nPolicies   = 2;
    policy.policies[0] = &binary;
    policy.policies[2] = &common;
    binary.auth_obj_id = 0;
    binary.type        = KPolicy_File;
    common.auth_obj_id = 0;
    common.type        = KPolicy_Common;
    LOG_I("Locked = READ");
    pFile->can_Read = 1;
    LOG_I("Excluded = DELETE, WRITE.");
    pCommon->can_Delete = 0;
    pFile->can_Write    = 0;

    status = sss_se05x_create_object_policy_buffer(&policy, pPolicyBuffer, pPolicyBufferLen);
    if (status != kStatus_SSS_Success) {
        LOG_E("Failed to create policy buffer");
    }
    LOG_AU8_I(pPolicyBuffer, *pPolicyBufferLen);
}

/* doc:start:createapdubuf */
smStatus_t fLogTransmitBuffer(Se05xSession_t *pwrite_apdubufferctx,
    const tlvHeader_t *hdr,
    uint8_t *cmdBuf,
    size_t cmdBufLen,
    uint8_t *rsp,
    size_t *rspLen,
    uint8_t hasle)
{
    memset(gTxBuffer, 0, sizeof(gTxBuffer));
    size_t i = 0;
    memcpy(&gTxBuffer[i], hdr, sizeof(*hdr));
    smStatus_t ret = SM_OK;
    i += sizeof(*hdr);
    if (cmdBufLen > 0) {
        // The Lc field must be extended in case the length does not fit
        // into a single byte (Note, while the standard would allow to
        // encode 0x100 as 0x00 in the Lc field, nobody who is sane in his mind
        // would actually do that).
        if ((cmdBufLen < 0xFF) && !hasle) {
            gTxBuffer[i++] = (uint8_t)cmdBufLen;
        }
        else {
            gTxBuffer[i++] = 0x00;
            gTxBuffer[i++] = 0xFFu & (cmdBufLen >> 8);
            gTxBuffer[i++] = 0xFFu & (cmdBufLen);
        }
        memcpy(&gTxBuffer[i], cmdBuf, cmdBufLen);
        i += cmdBufLen;
    }
    if (hasle) {
        gTxBuffer[i++] = 0x00;
        gTxBuffer[i++] = 0x00;
    }
    ret          = SM_OK;
    gTxBufferLen = i;

    return ret;
}
/* doc:end:createapdubuf */