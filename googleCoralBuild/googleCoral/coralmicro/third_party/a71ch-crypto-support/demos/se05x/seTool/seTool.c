/* Copyright 2019 NXP
 * SPDX-License-Identifier: Apache-2.0
 */

#include "seTool.h"

#include <ex_sss_boot.h>
#include <fsl_sss_se05x_apis.h>
#include <nxEnsure.h>
#include <nxLog_App.h>
#include <openssl/bio.h>
#include <openssl/evp.h>
#include <openssl/pem.h>
#include <se05x_APDU.h>
#include <se05x_const.h>
#include <se05x_ecc_curves.h>
#include <se05x_ecc_curves_values.h>
#include <se05x_tlv.h>
#include <stdio.h>
#include <string.h>

static ex_sss_boot_ctx_t gex_sss_se_tool;

#define EX_SSS_BOOT_PCONTEXT (&gex_sss_se_tool)
#define EX_SSS_BOOT_DO_ERASE 0
#define EX_SSS_BOOT_EXPOSE_ARGC_ARGV 1

#include <ex_sss_main_inc.h>

void usage()
{
    LOG_W("Usage:");
    LOG_W("\tseTool genECC <keyId>");
    LOG_W("\tseTool setECC <keyId> <filename>");
    LOG_W("\tseTool getPublic <keyId> <filename>");
    LOG_W("\tseTool getRef <keyId> <filename>");
}

sss_status_t ex_sss_entry(ex_sss_boot_ctx_t *pCtx)
{
    sss_status_t status = kStatus_SSS_Fail;
    int argc            = gex_sss_argc;
    const char **argv   = gex_sss_argv;
    int genECC          = 0;
    int getPublic       = 0;
    int getRef          = 0;
    int setECC          = 0;

    if (argc < 2) {
        usage();
        goto exit;
    }

    if (strncmp(argv[1], "genECC", sizeof("genECC")) == 0) {
        genECC = 1;
    }
    else if (strncmp(argv[1], "getPublic", sizeof("getPublic")) == 0) {
        getPublic = 1;
    }
    else if (strncmp(argv[1], "getRef", sizeof("getRef")) == 0) {
        getRef = 1;
    }
    else if (strncmp(argv[1], "setECC", sizeof("setECC")) == 0) {
        setECC = 1;
    }
    else {
        LOG_E("Invalid command line parameter");
        usage();
        goto exit;
    }

    if (genECC) {
        if (argc < 3) {
            usage();
            goto exit;
        }

        long int id    = strtoul(argv[2], NULL, 0);
        uint32_t keyId = (uint32_t)(id);
        /*We demonstrate for cipher type and key size corresponding to NISTP-256 keypair only*/
        sss_object_t obj;
        sss_key_part_t keyPart       = kSSS_KeyPart_Pair;
        sss_cipher_type_t cipherType = kSSS_CipherType_EC_NIST_P;
        size_t keyBitLen             = 256;

        status = sss_key_object_init(&obj, &pCtx->ks);
        ENSURE_OR_GO_EXIT(status == kStatus_SSS_Success);

        status =
            sss_key_object_allocate_handle(&obj, keyId, keyPart, cipherType, keyBitLen * 8, kKeyObject_Mode_Persistent);
        ENSURE_OR_GO_EXIT(status == kStatus_SSS_Success);

        status = sss_key_store_generate_key(&pCtx->ks, &obj, keyBitLen, NULL);
        ENSURE_OR_GO_EXIT(status == kStatus_SSS_Success);
    }

    else if (getPublic) {
        if (argc < 4) {
            usage();
            goto exit;
        }

        long int id    = strtoul(argv[2], NULL, 0);
        uint32_t keyId = (uint32_t)(id);
        /*We demonstrate for cipher type and key size corresponding to NISTP-256 keypair only*/
        sss_object_t obj;
        size_t keyBitLen  = 256;
        uint8_t key[256]  = {0};
        size_t keyByteLen = sizeof(key);

        status = sss_key_object_init(&obj, &pCtx->ks);
        ENSURE_OR_GO_EXIT(status == kStatus_SSS_Success);

        status = sss_key_object_get_handle(&obj, keyId);
        ENSURE_OR_GO_EXIT(status == kStatus_SSS_Success);

        status = sss_key_store_get_key(&pCtx->ks, &obj, key, &keyByteLen, &keyBitLen);
        ENSURE_OR_GO_EXIT(status == kStatus_SSS_Success);

        BIO *bio = BIO_new_mem_buf(key, (int)sizeof(key));
        if (bio == NULL) {
            LOG_E("Unable to initialize BIO");
            status = kStatus_SSS_Fail;
            goto exit;
        }

        EVP_PKEY *pKey = d2i_PUBKEY_bio(bio, NULL);
        if (!pKey) {
            LOG_E("Failed to load public key");
            status = kStatus_SSS_Fail;
            goto exit;
        }

        char file_name[MAX_FILE_NAME_SIZE] = {0};
        FILE *fp                           = NULL;
        strncpy(file_name, argv[3], sizeof(file_name) - 1);
        fp = fopen(file_name, "wb+");
        if (fp == NULL) {
            LOG_E("Can not open the file");
            status = kStatus_SSS_Fail;
            goto exit;
        }

        else {
            PEM_write_PUBKEY(fp, pKey);
            fclose(fp);
        }
    }

    else if (getRef) {
        if (argc < 4) {
            usage();
            goto exit;
        }

        long int id    = strtoul(argv[2], NULL, 0);
        uint32_t keyId = (uint32_t)(id);
        /*We demonstrate for cipher type and key size corresponding to NISTP-256 keypair only*/
        sss_object_t obj;
        size_t keyBitLen      = 256;
        uint8_t key[256]      = {0};
        size_t keyByteLen     = sizeof(key);
        uint8_t priv_header[] = PRIV_PREFIX_NIST_P_256;
        uint8_t pub_header[]  = PUBLIC_PREFIX_NIST_P_256;
        uint8_t magic[]       = MAGIC_BYTES_SE05X_OPENSSL_ENGINE;
        uint8_t ref[256]      = {0};
        size_t ref_len        = 0;
        uint8_t id_array[4]   = {0};

        status = sss_key_object_init(&obj, &pCtx->ks);
        ENSURE_OR_GO_EXIT(status == kStatus_SSS_Success);

        status = sss_key_object_get_handle(&obj, keyId);
        ENSURE_OR_GO_EXIT(status == kStatus_SSS_Success);

        status = sss_key_store_get_key(&pCtx->ks, &obj, key, &keyByteLen, &keyBitLen);
        ENSURE_OR_GO_EXIT(status == kStatus_SSS_Success);

        memcpy(&ref[ref_len], priv_header, sizeof(priv_header));
        ref_len += sizeof(priv_header);

        id_array[0] = (keyId & 0xFF000000) >> (8 * 3);
        id_array[1] = (keyId & 0x00FF0000) >> (8 * 2);
        id_array[2] = (keyId & 0x0000FF00) >> (8 * 1);
        id_array[3] = (keyId & 0x000000FF) >> (8 * 0);

        memcpy(&ref[ref_len], id_array, sizeof(id_array));
        ref_len += sizeof(id_array);

        memcpy(&ref[ref_len], magic, sizeof(magic));
        ref_len += sizeof(magic);

        ref[ref_len++] = 0x10; // Hardcoded for Keypair
        ref[ref_len++] = 0x00; // Hardcoded as 0x00

        memcpy(&ref[ref_len], pub_header, sizeof(pub_header));
        ref_len += sizeof(pub_header);

        if ((ref_len + (keyByteLen - 22)) > sizeof(ref)) {
            status = kStatus_SSS_Fail;
            goto exit;
        }

        if ((23 + (keyByteLen - 22)) > sizeof(key)) {
            status = kStatus_SSS_Fail;
            goto exit;
        }

        /* Copy only public key part */
        memcpy(&ref[ref_len], &key[23], keyByteLen - 22);
        ref_len += (keyByteLen - 22);

        BIO *bio = BIO_new_mem_buf(ref, (int)(ref_len));
        if (bio == NULL) {
            LOG_E("Unable to initialize BIO");
            status = kStatus_SSS_Fail;
            goto exit;
        }

        EVP_PKEY *pKey = d2i_PrivateKey_bio(bio, NULL);
        if (!pKey) {
            LOG_E("Failed to load public key");
            status = kStatus_SSS_Fail;
            goto exit;
        }

        char file_name[MAX_FILE_NAME_SIZE] = {0};
        FILE *fp                           = NULL;
        strncpy(file_name, argv[3], sizeof(file_name) - 1);
        fp = fopen(file_name, "wb+");
        if (fp == NULL) {
            LOG_E("Can not open the file");
            status = kStatus_SSS_Fail;
            goto exit;
        }

        else {
            PEM_write_PrivateKey(fp, pKey, NULL, NULL, 0, NULL, NULL);
            fclose(fp);
        }
    }

    else if (setECC) {
        if (argc < 4) {
            usage();
            goto exit;
        }

        long int id    = strtoul(argv[2], NULL, 0);
        uint32_t keyId = (uint32_t)(id);
        /*We demonstrate for cipher type and key size corresponding to NISTP-256 keypair only*/
        sss_object_t obj;
        sss_key_part_t keyPart       = kSSS_KeyPart_Pair;
        sss_cipher_type_t cipherType = kSSS_CipherType_EC_NIST_P;
        size_t keyBitLen             = 256;
        uint8_t key[256]             = {0};
        size_t keyByteLen            = sizeof(key);
        unsigned char *data          = &key[0];

        EVP_PKEY *pKey = NULL;

        char file_name[MAX_FILE_NAME_SIZE] = {0};
        FILE *fp                           = NULL;
        strncpy(file_name, argv[3], sizeof(file_name) - 1);
        fp = fopen(file_name, "rb");
        if (fp == NULL) {
            LOG_E("Can not open the file");
            status = kStatus_SSS_Fail;
            goto exit;
        }

        else {
            PEM_read_PrivateKey(fp, &pKey, NULL, NULL);
            fclose(fp);
            if (pKey == NULL) {
                LOG_E("Failed to read Keypair");
                status = kStatus_SSS_Fail;
                goto exit;
            }
        }

        keyByteLen = i2d_PrivateKey(pKey, &data);

        status = sss_key_object_init(&obj, &pCtx->ks);
        ENSURE_OR_GO_EXIT(status == kStatus_SSS_Success);

        status =
            sss_key_object_allocate_handle(&obj, keyId, keyPart, cipherType, keyBitLen * 8, kKeyObject_Mode_Persistent);
        ENSURE_OR_GO_EXIT(status == kStatus_SSS_Success);

        status = sss_key_store_set_key(&pCtx->ks, &obj, key, keyByteLen, keyBitLen, NULL, 0);
        ENSURE_OR_GO_EXIT(status == kStatus_SSS_Success);
    }

exit:
    return status;
}
