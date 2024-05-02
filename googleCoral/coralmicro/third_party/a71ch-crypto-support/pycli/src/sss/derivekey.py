#
# Copyright 2018-2020 NXP
# SPDX-License-Identifier: Apache-2.0
#
#

"""License text"""

import ctypes
import logging
from . import sss_api as apis
from .keystore import KeyStore
from .keyobject import KeyObject
from .util import status_to_str

log = logging.getLogger(__name__)


class DeriveKey:  # pylint: disable=too-many-instance-attributes
    """
    Key derivation operation
    """

    def __init__(self, session_obj, host_session_obj=None):
        """
        Constuctor
        :param session_obj: Instance of session
        :param host_session_obj: Instance of host session
        """
        self._session = session_obj.session_ctx
        self.derive = apis.sss_derive_key_t()
        self.algorithm = apis.kAlgorithm_SSS_ECDH
        self.mode = apis.kMode_SSS_ComputeSharedSecret
        self.derived_key = None
        self._ctx_ks = KeyStore(session_obj)
        self._ctx_key = KeyObject(self._ctx_ks)
        self.derived_key = None
        self.object_type = apis.kSSS_KeyPart_Default
        self.cypher_type = apis.kSSS_CipherType_HMAC
        if host_session_obj is not None:
            self._host_session = host_session_obj.session_ctx
            self._host_ctx_ks = KeyStore(host_session_obj)
            self._host_ctx_key = KeyObject(self._host_ctx_ks)

    def _symmetric_context_init(self, key_id, prv_key_obj):
        """
        asymmetric context initialization
        :param key_id: Key index
        :param prv_key_obj: key object of private key
        :return: Status
        """
        data_len = 80
        cypher_type = apis.kSSS_CipherType_AES

        status = self._ctx_key.allocate_handle(key_id, self.object_type, cypher_type, data_len,
                                               apis.kKeyObject_Mode_Transient)

        if status != apis.kStatus_SSS_Success:
            return status

        status = apis.sss_derive_key_context_init(ctypes.byref(self.derive),
                                                  ctypes.byref(self._session),
                                                  ctypes.byref(prv_key_obj.keyobject),
                                                  self.algorithm, self.mode)
        if status != apis.kStatus_SSS_Success:
            log.error("sss_derive_key_context_init %s", status_to_str(status))
        return status

    def symmetric_derive_ka(self, key_id, prv_key_obj, pub_key_obj):
        """
        Asymmetric derive key agreement
        :param key_id: Key index
        :param prv_key_obj: Key object of private key
        :param pub_key_obj: Key object of public key
        :return: Status
        """
        self.algorithm = apis.kAlgorithm_SSS_ECDH

        status = self._symmetric_context_init(key_id, prv_key_obj)
        if status != apis.kStatus_SSS_Success:
            return status

        status = apis.sss_derive_key_dh(ctypes.byref(self.derive),
                                        ctypes.byref(pub_key_obj.keyobject),
                                        ctypes.byref(self._ctx_key.keyobject))
        if status != apis.kStatus_SSS_Success:
            log.error("sss_derive_key_dh %s", status_to_str(status))

        return status

    def symmetric_derive_kd(self, key_id, prv_key_obj, salt, info, req_key_len):  # pylint: disable=too-many-arguments

        """
        Asymmetric derive key derivation
        :param key_id: Key index
        :param prv_key_obj: Key object of private key
        :param salt: Salt data
        :param info: Info data
        :param req_key_len: Request key length
        :param derive_key_file: File name to store derived key
        :return: Status
        """

        self.algorithm = apis.kAlgorithm_SSS_HMAC_SHA256
        status = self._symmetric_context_init(key_id, prv_key_obj)
        if status != apis.kStatus_SSS_Success:
            return status
        salt_len = len(salt)
        info_len = len(info)
        salt_ctype = (ctypes.c_uint8 * salt_len)(*salt)
        info_ctype = (ctypes.c_uint8 * info_len)(*info)

        status = self._host_ctx_key.allocate_handle(key_id, self.object_type,
                                                    self.cypher_type, req_key_len,
                                                    apis.kKeyObject_Mode_Persistent)
        if status != apis.kStatus_SSS_Success:
            return status

        status = apis.sss_derive_key_one_go(ctypes.byref(self.derive), salt_ctype, salt_len,
                                            info_ctype, info_len,
                                            ctypes.byref(self._host_ctx_key.keyobject),
                                            req_key_len)
        if status != apis.kStatus_SSS_Success:
            log.error("sss_derive_key_dh %s", status_to_str(status))

        return status
