#
# Copyright 2018 NXP
# SPDX-License-Identifier: Apache-2.0
#
#

import sss.sss_api as apis
import ctypes
import os
import pickle
import logging
log = logging.getLogger(__name__)

policy_type = {'Symmetric_Key': apis.KPolicy_Sym_Key,
               'Asymmetric_Key': apis.KPolicy_Asym_Key,
               'user_id': apis.KPolicy_UserID,
               'file': apis.KPolicy_File,
               'counter': apis.KPolicy_Counter,
               'pcr': apis.KPolicy_PCR,
               'common': apis.KPolicy_Common,
               'common_pcr_value': apis.KPolicy_Common_PCR_Value, }


class Policy(object):
    """
    Create / Display Object Policy
    """
    def __init__(self):
        self.obj_policy_file_path = ""
        self._policy_count = 0
        self.sign = 1
        self.verify = 1
        self.encrypt = 1
        self.decrypt = 1
        self.key_derive = 1
        self.wrap = 1
        self.generate = 1
        self.write = 1
        self.read = 1
        self.import_export = 1
        self.key_agreement = 1
        self.attest = 1
        self.desfire_auth = 1
        self.desfire_dump = 1
        self.forbid_all = 0
        self.delete = 1
        self.req_sm = 0
        self.pcr_obj_id = 0
        self.pcr_expected_value = 0
        self.forbid_derived_output = 1
        self.kdf_ext_random = 1

    def create_obj_policy(self, key_type, policy_name, auth_obj_id):
        """
        Create Object Policy
        :param key_type: type of object policy to be created
        :param policy_name: File name to store policy
        :param auth_obj_id: Auth Object id to apply policy
        :return: status
        """
        self._policy_count = 1
        policy_obj_params = {}

        # Check whether policy file exists. If No create it else append policy to existing file
        if not os.path.isfile(self._get_obj_policy_pkl_path(policy_name)):
            policy_obj_params['pkl_v_major'] = 1
            policy_obj_params['pkl_v_minor'] = 0
            policy_obj_params['policy_name'] = policy_name
            policy_dir = os.path.dirname(self.obj_policy_file_path)
            if not os.path.exists(policy_dir):
                os.mkdir(policy_dir)
            pkl_obj_policy = open(self.obj_policy_file_path, 'wb')
        else:
            pol_obj_exist_params = self.get_object_policy(policy_name)
            policy_count = pol_obj_exist_params['policy_count']
            if policy_count == 10:
                log.error("Maximum 10 policies can be applied to the object")
                return apis.kStatus_SSS_Fail
            policy_obj_params = pol_obj_exist_params
            self._policy_count += policy_count
            pkl_obj_policy = open(self.obj_policy_file_path, 'wb')

        policy_obj_params['policy_count'] = self._policy_count
        policy_obj_params['auth_obj_id_%s' % self._policy_count] = auth_obj_id
        policy_obj_params['key_type_%s' % self._policy_count] = key_type
        policy_obj_params['sign_%s' % self._policy_count] = self.sign
        policy_obj_params['verify_%s' % self._policy_count] = self.verify
        policy_obj_params['encrypt_%s' % self._policy_count] = self.encrypt
        policy_obj_params['decrypt_%s' % self._policy_count] = self.decrypt
        policy_obj_params['kd_%s' % self._policy_count] = self.key_derive
        policy_obj_params['wrap_%s' % self._policy_count] = self.wrap
        policy_obj_params['write_%s' % self._policy_count] = self.write
        policy_obj_params['gen_%s' % self._policy_count] = self.generate
        policy_obj_params['import_export_%s' % self._policy_count] = self.import_export
        policy_obj_params['ka_%s' % self._policy_count] = self.key_agreement
        policy_obj_params['read_%s' % self._policy_count] = self.read
        policy_obj_params['attest_%s' % self._policy_count] = self.attest
        policy_obj_params['desfire_auth_%s' % self._policy_count] = self.desfire_auth
        policy_obj_params['desfire_dump_%s' % self._policy_count] = self.desfire_dump
        policy_obj_params['forbid_all_%s' % self._policy_count] = self.forbid_all
        policy_obj_params['delete_%s' % self._policy_count] = self.delete
        policy_obj_params['req_sm_%s' % self._policy_count] = self.req_sm
        policy_obj_params['pcr_obj_id_%s' % self._policy_count] = self.pcr_obj_id
        policy_obj_params['pcr_expected_value_%s' % self._policy_count] = self.pcr_expected_value
        policy_obj_params['forbid_derived_output_%s' % self._policy_count] = self.forbid_derived_output
        policy_obj_params['kdf_ext_random_%s' % self._policy_count] = self.kdf_ext_random

        pickle.dump(policy_obj_params, pkl_obj_policy)
        pkl_obj_policy.close()
        print("Policy File path: \n%s " % (os.path.abspath(self.obj_policy_file_path)))
        return apis.kStatus_SSS_Success

    def get_object_policy(self, policy_name):
        """
        Retrieve object policy from pickle file.
        :param policy_name: Name of the policy
        :return: policy structure in list format
        """
        if os.path.isfile(self._get_obj_policy_pkl_path(policy_name)):
            print("Reading policy from file path: \n%s " % (os.path.abspath(self.obj_policy_file_path)))
            pkl_obj_policy = open(self.obj_policy_file_path, 'rb')
            pol_obj_params = pickle.load(pkl_obj_policy)
            pkl_obj_policy.close()
            return pol_obj_params
        else:
            log.info("Policy File path: \n%s " % (os.path.abspath(self.obj_policy_file_path)))
            log.error("Policy file not found. Try creating policy first")
            return None

    def convert_obj_policy_to_ctype(self, pol_obj_params):
        """
        Convert object policy from list to c structure
        :param pol_obj_params: policy in list format
        :return: policy in c type format
        """
        self._policy_count = pol_obj_params['policy_count']
        object_policy = []
        for i in range(1, self._policy_count + 1):

            policy_key = apis.sss_policy_u()
            policy_key.auth_obj_id = pol_obj_params['auth_obj_id_%s' % i]
            policy_key.type = pol_obj_params['key_type_%s' % i]

            if policy_key.type == apis.KPolicy_Asym_Key:
                policy_key.policy.asymmkey.can_Sign = pol_obj_params['sign_%s' % i]
                policy_key.policy.asymmkey.can_Verify = pol_obj_params['verify_%s' % i]
                policy_key.policy.asymmkey.can_Encrypt = pol_obj_params['encrypt_%s' % i]
                policy_key.policy.asymmkey.can_Decrypt = pol_obj_params['decrypt_%s' % i]
                policy_key.policy.asymmkey.can_KD = pol_obj_params['kd_%s' % i]
                policy_key.policy.asymmkey.can_Wrap = pol_obj_params['wrap_%s' % i]
                policy_key.policy.asymmkey.can_Write = pol_obj_params['write_%s' % i]
                policy_key.policy.asymmkey.can_Gen = pol_obj_params['gen_%s' % i]
                policy_key.policy.asymmkey.can_Import_Export = pol_obj_params['import_export_%s' % i]
                policy_key.policy.asymmkey.can_KA = pol_obj_params['ka_%s' % i]
                policy_key.policy.asymmkey.can_Read = pol_obj_params['read_%s' % i]
                policy_key.policy.asymmkey.can_Attest = pol_obj_params['attest_%s' % i]
                policy_key.policy.symmkey.forbid_Derived_Output = pol_obj_params['forbid_derived_output_%s' % i]

            elif policy_key.type == apis.KPolicy_Sym_Key:
                policy_key.policy.symmkey.can_Sign = pol_obj_params['sign_%s' % i]
                policy_key.policy.symmkey.can_Verify = pol_obj_params['verify_%s' % i]
                policy_key.policy.symmkey.can_Encrypt = pol_obj_params['encrypt_%s' % i]
                policy_key.policy.symmkey.can_Decrypt = pol_obj_params['decrypt_%s' % i]
                policy_key.policy.symmkey.can_KD = pol_obj_params['kd_%s' % i]
                policy_key.policy.symmkey.can_Wrap = pol_obj_params['wrap_%s' % i]
                policy_key.policy.symmkey.can_Write = pol_obj_params['write_%s' % i]
                policy_key.policy.symmkey.can_Gen = pol_obj_params['gen_%s' % i]
                policy_key.policy.symmkey.can_Desfire_Auth = pol_obj_params['desfire_auth_%s' % i]
                policy_key.policy.symmkey.can_Desfire_Dump = pol_obj_params['desfire_dump_%s' % i]
                policy_key.policy.symmkey.can_Import_Export = pol_obj_params['import_export_%s' % i]
                policy_key.policy.symmkey.forbid_Derived_Output = pol_obj_params['forbid_derived_output_%s' % i]
                policy_key.policy.symmkey.allow_kdf_ext_rnd = pol_obj_params['kdf_ext_random_%s' % i]

            elif policy_key.type == apis.KPolicy_File:
                policy_key.policy.file.can_Write = pol_obj_params['write_%s' % i]
                policy_key.policy.file.can_Read = pol_obj_params['read_%s' % i]

            elif policy_key.type == apis.KPolicy_PCR:
                policy_key.policy.pcr.can_Write = pol_obj_params['write_%s' % i]
                policy_key.policy.pcr.can_Read = pol_obj_params['read_%s' % i]

            elif policy_key.type == apis.KPolicy_UserID:
                policy_key.policy.pin.can_Write = pol_obj_params['write_%s' % i]

            elif policy_key.type == apis.KPolicy_Counter:
                policy_key.policy.counter.can_Write = pol_obj_params['write_%s' % i]
                policy_key.policy.counter.can_Read = pol_obj_params['read_%s' % i]

            elif policy_key.type == apis.KPolicy_Common:
                policy_key.policy.common.forbid_All = pol_obj_params['forbid_all_%s' % i]
                policy_key.policy.common.can_Delete = pol_obj_params['delete_%s' % i]
                policy_key.policy.common.req_Sm = pol_obj_params['req_sm_%s' % i]

            elif policy_key.type == apis.KPolicy_Common_PCR_Value:
                policy_key.policy.common_pcr_value.pcrObjId = pol_obj_params['pcr_obj_id_%s' % i]
                pcr_expected_value = pol_obj_params['pcr_expected_value_%s' % i]
                pcr = [pcr_expected_value[i:i + 2] for i in range(0, len(pcr_expected_value), 2)]
                for j in range(len(pcr)):
                    pcr[j] = int(pcr[j], 16)
                policy_key.policy.common_pcr_value.pcrExpectedValue = (ctypes.c_uint8 * 32)(*pcr)

            object_policy.append(ctypes.pointer(policy_key))

        policy = apis.sss_policy_t()
        policy.nPolicies = self._policy_count
        policy.policies = tuple(object_policy)

        return ctypes.byref(policy)

    def display_policy(self, policy_obj_params):
        """
        Display policy
        :param policy_obj_params: policy in list format
        :return: status
        """
        self._policy_count = policy_obj_params['policy_count']
        print("\nCreated object policy:")
        for i in range(1, self._policy_count + 1):
            key_type_int = policy_obj_params['key_type_%s' % i]
            print("\nKey_type                 : %s " % list(policy_type.keys())[list(policy_type.values()).index(key_type_int)])
            print("Auth Obj ID              : 0x%X" % policy_obj_params['auth_obj_id_%s' % i])
            if key_type_int == apis.KPolicy_Asym_Key:
                print("Sign                     : %s" % policy_obj_params['sign_%s' % i])
                print("Verify                   : %s" % policy_obj_params['verify_%s' % i])
                print("Encrypt                  : %s" % policy_obj_params['encrypt_%s' % i])
                print("Decrypt                  : %s" % policy_obj_params['decrypt_%s' % i])
                print("Key Derive               : %s" % policy_obj_params['kd_%s' % i])
                print("Wrap                     : %s" % policy_obj_params['wrap_%s' % i])
                print("Generate                 : %s" % policy_obj_params['gen_%s' % i])
                print("Write                    : %s" % policy_obj_params['write_%s' % i])
                print("Read                     : %s" % policy_obj_params['read_%s' % i])
                print("Import Export            : %s" % policy_obj_params['import_export_%s' % i])
                print("Key Agreement            : %s" % policy_obj_params['ka_%s' % i])
                print("Attestation              : %s" % policy_obj_params['attest_%s' % i])
                print("forbid_derived_output    : %s" % policy_obj_params['forbid_derived_output_%s' % i])

            elif key_type_int == apis.KPolicy_Sym_Key:
                print("Sign                     : %s" % policy_obj_params['sign_%s' % i])
                print("Verify                   : %s" % policy_obj_params['verify_%s' % i])
                print("Encrypt                  : %s" % policy_obj_params['encrypt_%s' % i])
                print("Decrypt                  : %s" % policy_obj_params['decrypt_%s' % i])
                print("KD                       : %s" % policy_obj_params['kd_%s' % i])
                print("Wrap                     : %s" % policy_obj_params['wrap_%s' % i])
                print("Write                    : %s" % policy_obj_params['write_%s' % i])
                print("Gen                      : %s" % policy_obj_params['gen_%s' % i])
                print("Desfire_Auth             : %s" % policy_obj_params['desfire_auth_%s' % i])
                print("Desfire_Dump             : %s" % policy_obj_params['desfire_dump_%s' % i])
                print("Import_Export            : %s" % policy_obj_params['import_export_%s' % i])
                print("forbid_derived_output    : %s" % policy_obj_params['forbid_derived_output_%s' % i])
                print("kdf_ext_random           : %s" % policy_obj_params['kdf_ext_random_%s' % i])

            elif key_type_int == apis.KPolicy_File:
                print("Write                    : %s" % policy_obj_params['write_%s' % i])
                print("Read                     : %s" % policy_obj_params['read_%s' % i])

            elif key_type_int == apis.KPolicy_PCR:
                print("Write                    : %s" % policy_obj_params['write_%s' % i])
                print("Read                     : %s" % policy_obj_params['read_%s' % i])

            elif key_type_int == apis.KPolicy_UserID:
                print("Write                    : %s" % policy_obj_params['write_%s' % i])

            elif key_type_int == apis.KPolicy_Counter:
                print("Write                    : %s" % policy_obj_params['write_%s' % i])
                print("Read                     : %s" % policy_obj_params['read_%s' % i])

            elif key_type_int == apis.KPolicy_Common:
                print("forbid_all               : %s" % policy_obj_params['forbid_all_%s' % i])
                print("Delete                   : %s" % policy_obj_params['delete_%s' % i])
                print("req_sm                   : %s" % policy_obj_params['req_sm_%s' % i])

            elif key_type_int == apis.KPolicy_Common_PCR_Value:
                print("pcr_obj_id               : %s" % policy_obj_params['pcr_obj_id_%s' % i])
                print("pcr_expected_value       : %s" % policy_obj_params['pcr_expected_value_%s' % i])

    def display_policy_in_hex(self, policy_obj_params):
        """
        Display policy in hex format
        :param policy_obj_params: Policy in list format
        :return: None
        """
        policy_length_int = 200
        policy_buf = (ctypes.c_uint8 * policy_length_int)(0)
        policy_length = ctypes.c_size_t(policy_length_int)
        apis.sss_se05x_create_object_policy_buffer(self.convert_obj_policy_to_ctype(policy_obj_params),
                                                   ctypes.byref(policy_buf),
                                                   ctypes.byref(policy_length))
        policy_buf_list = list(policy_buf)
        policy_buf_list_trim = policy_buf_list[:int(policy_length.value)]
        policy_hex_str = ""
        for i in range(len(policy_buf_list_trim)):
            policy_hex = format(policy_buf_list_trim[i], 'X')
            if len(policy_hex) == 1:
                policy_hex = "0" + policy_hex
            policy_hex_str += " " + policy_hex
        print("\nPolicy in hex:\n")
        print(policy_hex_str)
        print("\n")

    def _get_obj_policy_pkl_path(self, policy_name):
        """
        Retrieve object policy full path
        :param policy_name: Name of the policy
        :return: Full path of the policy
        """
        policy_dir = os.path.join(os.path.abspath(os.path.dirname(__file__)), "..", "..", "policy")
        if not os.path.exists(policy_dir):
            os.mkdir(policy_dir)
        self.obj_policy_file_path = os.path.join(policy_dir, "ssscli_obj_policy_%s.pkl" % policy_name)

        return self.obj_policy_file_path
