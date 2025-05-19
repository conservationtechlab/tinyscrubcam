#
# Copyright 2018-2020 NXP
# SPDX-License-Identifier: Apache-2.0
#
#

"""License text"""

import ctypes
import os
import logging
from .keystore import KeyStore
from .keyobject import KeyObject
from . import sss_api as apis
from .util import key_der_to_list, get_ecc_cypher_type, from_file_dat, \
    transform_key_to_list, to_cert_dat, ecc_curve_type_from_key, is_hex

log = logging.getLogger(__name__)


class Set:
    """
    Inject keys and certificates
    """

    def __init__(self, session_obj):
        """
        Constructor
        :param session_obj: Instance of session
        """
        self._session = session_obj
        self._ctx_ks = KeyStore(self._session)
        self._ctx_key = KeyObject(self._ctx_ks)

    def do_set_ecc_pub_key(self, key_id, key, policy, encode_format=""):
        """
        Inject ECC public key
        :param key_id: Key Index to set key
        :param key: Key value
        :param policy: Policy to be applied
        :param encode_format: Encode format of the key value. Eg: PEM, DER
        :return: status
        """
        try:
            key_type = apis.kSSS_KeyPart_Public
            if not os.path.isfile(key):
                curve_type, key_bit_len = ecc_curve_type_from_key(key, key_type)
                key_int_list = transform_key_to_list(key)
                data_len = len(key_int_list)
            else:
                key_int_list, data_len, key_bit_len, curve_type = from_file_dat(key,
                                                                                key_type,
                                                                                encode_format)
            cypher_type, key_len = get_ecc_cypher_type(curve_type)  # pylint: disable=unused-variable
        except Exception as exc:  # pylint: disable=broad-except
            if 'Could not deserialize key data' in str(exc):
                log.error("Incorrect key, try with correct ECC public key")
            elif 'Non-hexadecimal digit found' in str(exc) or 'Odd-length string' in str(exc):
                log.error('Incorrect file or path, try with correct ECC public key file')
            else:
                raise exc
            return apis.kStatus_SSS_Fail
        key_ctype = (ctypes.c_uint8 * data_len)(*key_int_list)
        status = self._set_key(key_id, key_ctype, data_len, key_bit_len,
                               key_type, cypher_type, policy)
        return status

    def do_set_ecc_key_pair(self, key_id, key, policy, encode_format=""):
        """
        Inject ECC Key pair
        :param key_id: Key Index to set key
        :param key: Key value
        :param policy: Policy to be applied
        :param encode_format: Encode format of the key value. Eg: PEM, DER
        :return: Status
        """
        key_type = apis.kSSS_KeyPart_Pair
        try:
            if not os.path.isfile(key):
                curve_type, key_bit_len = ecc_curve_type_from_key(key, key_type)
                key_int_list = transform_key_to_list(key)
                data_len = len(key_int_list)
            else:
                key_int_list, data_len, key_bit_len, curve_type = from_file_dat(key,
                                                                                key_type,
                                                                                encode_format)
            cypher_type, key_len = get_ecc_cypher_type(curve_type)  # pylint: disable=unused-variable
        except Exception as exc:  # pylint: disable=broad-except
            if 'Could not deserialize key data' in str(exc):
                log.error("Incorrect key pair, try with correct ECC key pair")
            elif 'Non-hexadecimal digit found' in str(exc) or 'Odd-length string' in str(exc):
                log.error('Incorrect file or path, try with correct ECC key pair file')
            else:
                raise exc
            return apis.kStatus_SSS_Fail

        key_ctype = (ctypes.c_uint8 * data_len)(*key_int_list)
        status = self._set_key(key_id, key_ctype, data_len, key_bit_len,
                               key_type, cypher_type, policy)
        return status

    def do_set_sym_key(self, key_id, key, policy):
        """
        Inject Symmetric key
        :param key_id: Key Index to set key
        :param key: Key value
        :param policy: Policy to be applied
        :return: Status
        """
        key_type = apis.kSSS_KeyPart_Default
        cypher_type = apis.kSSS_CipherType_AES

        if os.path.isfile(key):
            with open(key, 'rb') as aes_in:
                aes_data = aes_in.read()
        else:
            aes_data = key

        if is_hex(aes_data):
            if isinstance(aes_data, bytes):
                aes_data = bytes.decode(aes_data)
            aes_data = aes_data.replace("0x", "")
            aes_key = transform_key_to_list(aes_data)
        else:
            if isinstance(aes_data, str):
                aes_data = str.encode(aes_data)
            aes_key = key_der_to_list(aes_data)

        aes_data_len = len(aes_key)

        if aes_data_len not in [16, 24, 32]:
            log.warning("Unsupported key length: %s", (aes_data_len * 8))
            log.warning("Supported key lengths are 128, 192 and 256 bit")
            return apis.kStatus_SSS_Fail
        key_ctype = (ctypes.c_uint8 * aes_data_len)(*aes_key)
        status = self._set_key(key_id, key_ctype, aes_data_len, aes_data_len * 8,
                               key_type, cypher_type, policy)
        return status

    def do_set_hmac_key(self, key_id, key, policy):
        """
        Inject hmac key
        :param key_id: Key Index to set key
        :param key: Key value
        :param policy: Policy to be applied
        :return: Status
        """
        key_type = apis.kSSS_KeyPart_Default
        cypher_type = apis.kSSS_CipherType_HMAC

        if os.path.isfile(key):
            with open(key, 'rb') as hmac_in:
                hmac_data = hmac_in.read()
        else:
            hmac_data = key

        if is_hex(hmac_data):
            if isinstance(hmac_data, bytes):
                hmac_data = bytes.decode(hmac_data)
            hmac_data = hmac_data.replace("0x", "")
            hmac_key = transform_key_to_list(hmac_data)
        else:
            if isinstance(hmac_data, str):
                hmac_data = str.encode(hmac_data)
            hmac_key = key_der_to_list(hmac_data)

        hmac_data_len = len(hmac_key)
        if hmac_data_len > 256:
            log.warning("Unsupported key length: %s", (hmac_data_len))
            log.warning("Supported key lengths are between 1 to 256 bytes")
            return apis.kStatus_SSS_Fail

        key_ctype = (ctypes.c_uint8 * hmac_data_len)(*hmac_key)
        status = self._set_key(key_id, key_ctype, hmac_data_len, hmac_data_len * 8,
                               key_type, cypher_type, policy)
        return status

    def do_set_rsa_pub_key(self, key_id, key, policy, encode_format=""):
        """
        Inject RSA Public key
        :param key_id: Key Index to set key
        :param key: Key value
        :param policy: Policy to be applied
        :param encode_format: Encode format of the key value. Eg: PEM, DER
        :return: Status
        """
        key_type = apis.kSSS_KeyPart_Public
        cypher_type = apis.kSSS_CipherType_RSA_CRT
        try:
            if not os.path.isfile(key):
                key_int_list = transform_key_to_list(key)
                data_len = len(key)
                key_bit_len = data_len * 8
            else:
                key_int_list, data_len, key_bit_len, curve_type = from_file_dat(key,  # pylint: disable=unused-variable
                                                                                key_type,
                                                                                encode_format)
        except Exception as exc:  # pylint: disable=broad-except
            if 'Could not deserialize key data' in str(exc):
                log.error("Incorrect key, try with correct RSA public key")
            elif 'invalid literal for int' in str(exc):
                log.error('Incorrect file or path, try with correct RSA public key file')
            else:
                raise exc
            return apis.kStatus_SSS_Fail

        key_ctype = (ctypes.c_uint8 * data_len)(*key_int_list)
        status = self._set_key(key_id, key_ctype, data_len, key_bit_len,
                               key_type, cypher_type, policy)
        return status

    def do_set_rsa_key_pair(self, key_id, key, policy, encode_format=""):
        """
        Inject RSA key pair
        :param key_id: Key index to set key
        :param key: Key value
        :param policy: Policy to be applied
        :param encode_format: Encode format of the key value. Eg: PEM, DER
        :return: Status
        """
        key_type = apis.kSSS_KeyPart_Pair
        cypher_type = apis.kSSS_CipherType_RSA_CRT
        try:
            if not os.path.isfile(key):
                key_int_list = transform_key_to_list(key)
                data_len = len(key)
                key_bit_len = data_len * 8
            else:

                key_int_list, data_len, key_bit_len, curve_type = from_file_dat(key,  # pylint: disable=unused-variable
                                                                                key_type,
                                                                                encode_format)
        except Exception as exc:  # pylint: disable=broad-except
            if 'Could not deserialize key data' in str(exc):
                log.error("Incorrect key pair, try with correct RSA key pair")
            elif 'invalid literal for int' in str(exc):
                log.error('Incorrect file or path, try with correct RSA key pair file')
            else:
                raise exc
            return apis.kStatus_SSS_Fail

        key_ctype = (ctypes.c_uint8 * data_len)(*key_int_list)
        status = self._set_key(key_id, key_ctype, data_len, key_bit_len,
                               key_type, cypher_type, policy)
        return status

    def do_set_cert(self, cert_id, raw_cert, policy, encode_format=""):
        """
        Inject Certificate
        :param cert_id: Index to set certificate
        :param raw_cert: Certificate value
        :param policy: Policy to be applied
        :param encode_format: Encode format of the key value. Eg: PEM, DER
        :return: Status
        """
        key_type = apis.kSSS_KeyPart_Default
        cypher_type = apis.kSSS_CipherType_Binary
        if not os.path.isfile(raw_cert):
            cert_data = to_cert_dat(raw_cert)
            cert_len = len(cert_data)
            key_bit_len = cert_len * 8
        else:

            cert_data, cert_len, key_bit_len, curve = from_file_dat(raw_cert,  # pylint: disable=unused-variable
                                                                    key_type,
                                                                    encode_format)

        cert_ctype = (ctypes.c_uint8 * cert_len)(*cert_data)

        status = self._set_key(cert_id, cert_ctype, cert_len, key_bit_len, key_type,
                               cypher_type, policy)

        return status

    def _set_key(self, key_id, data, data_len, key_bit_len,  # pylint: disable=too-many-arguments
                 key_type, cypher_type, policy):
        """
        Inject keys and certificate
        :param key_id: Index to set key/certificate
        :param data: key value to set
        :param data_len: length of the key value
        :param key_bit_len: bit length of the key value
        :param key_type: key type
        :param cypher_type: cipher type
        :param policy: Policy to be applied
        :return: Status
        """

        status = self._ctx_key.allocate_handle(key_id, key_type, cypher_type, data_len,
                                               apis.kKeyObject_Mode_Persistent)
        if status != apis.kStatus_SSS_Success:
            return status

        status = self._ctx_ks.set_key(self._ctx_key, data, data_len, key_bit_len, policy, 0)
        if status != apis.kStatus_SSS_Success:
            return status

        status = self._ctx_ks.save_key_store()
        return status
