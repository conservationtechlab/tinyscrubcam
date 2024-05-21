#
# Copyright 2018-2020 NXP
# SPDX-License-Identifier: Apache-2.0
#
#

"""License text"""

import os
import sys
import ctypes
import logging
import binascii
from . import sss_api as apis
from .keystore import KeyStore
from .keyobject import KeyObject
from .asymmetric import Asymmetric
from .util import file_write, transform_int_list_to_hex_str

log = logging.getLogger(__name__)


class Crypt:
    """
    Encrypt and Decypt operation
    """

    def __init__(self, session_obj):
        """
        constructor
        :param session_obj: Instance of session
        """
        self._session = session_obj
        self._ctx_ks = KeyStore(session_obj)
        self._ctx_key = KeyObject(self._ctx_ks)
        self.encrypt_data = None
        self.decrypt_data = None
        self.algorithm = apis.kAlgorithm_SSS_RSAES_PKCS1_V1_5

    def do_encryption(self, key_id, pre_encrypted_data, encrypted_data_file):  # pylint: disable=too-many-locals
        """
        Perform Encryption
        :param key_id: Key index
        :param pre_encrypted_data: Input data to encrypt
        :param encrypted_data_file: File name to store encrypted data
        :return: Status
        """
        if not os.path.isfile(pre_encrypted_data):
            src_data = str.encode(pre_encrypted_data)
        else:
            with open(pre_encrypted_data, 'rb') as raw_data:
                src_data = raw_data.read()

        # convert pre encrypted data format to list
        src_data_list = []
        for ch in src_data:
            if sys.hexversion < 0x3000000:
                src_data_list.append(ord(ch))
            else:
                src_data_list.append(ord(chr(ch)))
        src_data_len = len(src_data)

        src_data_ctype = (ctypes.c_ubyte * src_data_len)(*src_data_list)

        mode = apis.kMode_SSS_Encrypt
        encrypt_data_len = 1024
        encrypt_data_buf = (ctypes.c_uint8 * encrypt_data_len)(0)
        encrypt_data_len_ctype = ctypes.c_size_t(encrypt_data_len)

        status, obj_type, cypher_type = self._ctx_key.get_handle(key_id)  # pylint: disable=unused-variable
        if status != apis.kStatus_SSS_Success:
            return status

        if cypher_type not in [apis.kSSS_CipherType_RSA_CRT, apis.kSSS_CipherType_RSA]:
            log.warning("Encrypt operation supported only for RSA keys")
            return apis.kStatus_SSS_Fail

        ctx_asymm = Asymmetric(self._session, self._ctx_key, self.algorithm, mode)
        status, encrypt_data = ctx_asymm.encrypt(
            src_data_ctype, src_data_len, encrypt_data_buf, encrypt_data_len_ctype)
        if status != apis.kStatus_SSS_Success:
            return status
        if encrypt_data is None:
            log.error("Received encrypted data is empty")
            return status

        encrypt_data_full_list = list(encrypt_data)
        # remove extra zero padding
        encrypt_data_list = encrypt_data_full_list[:int(encrypt_data_len_ctype.value)]

        self.encrypt_data = (ctypes.c_uint8 * int(encrypt_data_len_ctype.value))(*encrypt_data_list)

        if encrypted_data_file is not None:
            crypt_hex_str = transform_int_list_to_hex_str(encrypt_data_list)
            crypt_der_str = binascii.unhexlify(crypt_hex_str)
            file_write(crypt_der_str, encrypted_data_file)
        return status

    def do_decryption(self, key_id, encrypted_data, decrypt_file):  # pylint: disable=too-many-locals
        """
        Perform decryption
        :param key_id: Key Index
        :param encrypted_data: Encrypted raw data or in file
        :param decrypt_file: Output file name to store decrypted data
        :return: Status
        """
        if os.path.isfile(encrypted_data):
            key_in = open(encrypted_data, 'rb')
            src_data = key_in.read()
        else:
            src_data = encrypted_data

        src_data_len = len(src_data)

        src_data_ctype = (ctypes.c_ubyte * src_data_len)(*src_data)
        mode = apis.kMode_SSS_Decrypt
        decrypt_data_len = 1024
        decrypt_data_buf = (ctypes.c_uint8 * decrypt_data_len)(0)
        decrypt_data_len_ctype = ctypes.c_size_t(decrypt_data_len)

        status, obj_type, cypher_type = self._ctx_key.get_handle(key_id)  # pylint: disable=unused-variable
        if status != apis.kStatus_SSS_Success:
            return status

        if cypher_type not in [apis.kSSS_CipherType_RSA_CRT, apis.kSSS_CipherType_RSA]:
            log.warning("Decrypt operation supported only for RSA keys")
            return apis.kStatus_SSS_Fail

        ctx_asymm = Asymmetric(self._session, self._ctx_key, self.algorithm, mode)
        status, decrypt_data = ctx_asymm.decrypt(
            src_data_ctype, src_data_len, decrypt_data_buf, decrypt_data_len_ctype)

        if status != apis.kStatus_SSS_Success:
            return status
        if decrypt_data is None:
            log.error("Received decrypted data is empty")
            return status
        decrypt_data_full_list = list(decrypt_data)
        decrypt_data_list = decrypt_data_full_list[:int(decrypt_data_len_ctype.value)]
        self.decrypt_data = ''.join(chr(i) for i in decrypt_data_list)

        if decrypt_file is not None:
            file_write(self.decrypt_data, decrypt_file)

        return status
