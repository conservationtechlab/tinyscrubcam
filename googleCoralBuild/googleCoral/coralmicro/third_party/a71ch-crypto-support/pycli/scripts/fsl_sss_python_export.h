/* Copyright 2019,2020 NXP
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef SSS_APIS_INC_FSL_SSS_PYTHON_EXPORT_H_
#define SSS_APIS_INC_FSL_SSS_PYTHON_EXPORT_H_

/* ************************************************************************** */
/* Includes                                                                   */
/* ************************************************************************** */
#include "../../sss/inc/fsl_sss_se05x_types.h"


smStatus_t Se05x_API_DeleteAll_Iterative(pSe05xSession_t session_ctx);

smStatus_t Se05x_API_WritePCR(pSe05xSession_t session_ctx,
    pSe05xPolicy_t policy,
    uint32_t pcrID,
    const uint8_t *initialValue,
    size_t initialValueLen,
    const uint8_t *inputData,
    size_t inputDataLen);

smStatus_t Se05x_API_ReadIDList(pSe05xSession_t session_ctx,
    uint16_t outputOffset,
    uint8_t filter,
    uint8_t *pmore,
    uint8_t *idlist,
    size_t *pidlistLen);

smStatus_t Se05x_API_ReadSize(
    pSe05xSession_t session_ctx, uint32_t objectID, uint16_t *psize);

smStatus_t Se05x_API_ReadCryptoObjectList(
    pSe05xSession_t session_ctx, uint8_t *idlist, size_t *pidlistLen);

sss_status_t sss_util_openssl_write_pkcs12(sss_session_t *session,
	sss_key_store_t *ks,
	sss_object_t *obj,
	const char *pkcs12_cert,
	const char *password,
	const char *ref_key,
	long ref_key_length,
	const char* cert_bytes,
	const char* cert_subject);

sss_status_t sss_util_openssl_read_pkcs12(const char* pkcs12_cert, const char* password, uint8_t * private_key, uint8_t * cert);

smStatus_t Se05x_API_WriteSymmKey(pSe05xSession_t session_ctx,
    pSe05xPolicy_t policy,
    SE05x_MaxAttemps_t maxAttempt,
    uint32_t objectID,
    SE05x_KeyID_t kekID,
    const uint8_t *keyValue,
    size_t keyValueLen,
    const SE05x_INS_t ins_type,
    const SE05x_SymmKeyType_t type);

smStatus_t Se05x_API_WriteECKey(pSe05xSession_t session_ctx,
    pSe05xPolicy_t policy,
    SE05x_MaxAttemps_t maxAttempt,
    uint32_t objectID,
    SE05x_ECCurve_t curveID,
    const uint8_t *privKey,
    size_t privKeyLen,
    const uint8_t *pubKey,
    size_t pubKeyLen,
	const SE05x_INS_t ins_type,
    const SE05x_KeyPart_t key_part);
    

sss_status_t sss_se05x_create_object_policy_buffer(
    sss_policy_t *policies, uint8_t *pbuff, size_t *buf_len);


sss_status_t sss_se05x_refresh_session(
    sss_se05x_session_t *session, void *connectionData);

/** Similar to @ref sss_se05x_asymmetric_sign_digest,
 *
 * but hashing/digest done by SE
 */
sss_status_t sss_se05x_asymmetric_sign(sss_se05x_asymmetric_t *context,
    uint8_t *srcData,
    size_t srcLen,
    uint8_t *signature,
    size_t *signatureLen);

/** Similar to @ref sss_se05x_asymmetric_verify_digest,
 * but hashing/digest done by SE
 *
 */
sss_status_t sss_se05x_asymmetric_verify(sss_se05x_asymmetric_t *context,
    uint8_t *srcData,
    size_t srcLen,
    uint8_t *signature,
    size_t signatureLen);

smStatus_t Se05x_API_GetVersion(pSe05xSession_t session_ctx, uint8_t *pappletVersion, size_t *appletVersionLen);

#endif /* SSS_APIS_INC_FSL_SSS_PYTHON_EXPORT_H_ */