#
# Copyright 2019 NXP
# SPDX-License-Identifier: Apache-2.0
#
#

import sys
import click
from .cli import policy, pass_context
import sss.sss_api as apis
from sss.policy import Policy, policy_type


@policy.command('asymkey', short_help='Create Asymmetric Key Object Policy')
@pass_context
@click.argument('policy_name', type=str, metavar='policy_name')
@click.argument('auth_obj_id', type=str, metavar='auth_obj_id')
@click.option('--sign', type=bool, default=True, help="Object policy Allow Sign. Enabled by Default")
@click.option('--verify', type=bool, default=True, help="Object policy Allow Verify. Enabled by Default")
@click.option('--encrypt', type=bool, default=True, help="Object policy Allow Encryption. Enabled by Default")
@click.option('--decrypt', type=bool, default=True, help="Object policy Allow Decryption. Enabled by Default")
@click.option('--key_derive', type=bool, default=True, help="Object policy Allow Key Derivation. Enabled by Default")
@click.option('--wrap', type=bool, default=True, help="Object policy Allow Wrap. Enabled by Default")
@click.option('--generate', type=bool, default=True, help="Object policy Allow Generate. Enabled by Default")
@click.option('--write', type=bool, default=True, help="Object policy Allow Write. Enabled by Default")
@click.option('--read', type=bool, default=True, help="Object policy Allow Read. Enabled by Default")
@click.option('--import_export', type=bool, default=True,
              help="Object policy Allow Import Export. Enabled by Default")
@click.option('--key_agreement', type=bool, default=True, help="Object policy Allow Key Agreement. Enabled by Default")
@click.option('--attest', type=bool, default=True, help="Object policy Allow attestation. Enabled by Default")
@click.option('--forbid_derived_output', type=bool, default=False,
              help="Object policy Forbid Derived Output. Disabled by Default")
def asymkey(cli_ctx, policy_name, auth_obj_id, sign, verify, encrypt, decrypt, key_derive, wrap, generate,
            write, read, import_export, key_agreement, attest, forbid_derived_output):
    """ Create Asymmetric key object policy. \n

    policy_name = File name of the policy to be created.
    This policy name should be given as input while provisioning.\n

    auth_obj_id = Auth object id for each Object Policy. \n

    """
    auth_obj_id = int(auth_obj_id, 16)
    policy_obj = Policy()
    policy_obj.sign = sign
    policy_obj.verify = verify
    policy_obj.encrypt = encrypt
    policy_obj.decrypt = decrypt
    policy_obj.key_derive = key_derive
    policy_obj.wrap = wrap
    policy_obj.generate = generate
    policy_obj.write = write
    policy_obj.read = read
    policy_obj.import_export = import_export
    policy_obj.key_agreement = key_agreement
    policy_obj.attest = attest
    policy_obj.forbid_derived_output = forbid_derived_output

    status = policy_obj.create_obj_policy(policy_type["Asymmetric_Key"], policy_name, auth_obj_id)
    if status == apis.kStatus_SSS_Success:
        cli_ctx.log("Object Policy created successfully")
        ret_value = 0
    else:
        cli_ctx.log("Error: Could not create object Policy")
        ret_value = 1
    sys.exit(ret_value)


@policy.command('symkey', short_help='Create Symmetric Key Object Policy')
@pass_context
@click.argument('policy_name', type=str, metavar='policy_name')
@click.argument('auth_obj_id', type=str, metavar='auth_obj_id')
@click.option('--sign', type=bool, default=True, help="Object policy Allow Sign. Enabled by Default")
@click.option('--verify', type=bool, default=True, help="Object policy Allow Verify. Enabled by Default")
@click.option('--encrypt', type=bool, default=True, help="Object policy Allow Encryption. Enabled by Default")
@click.option('--decrypt', type=bool, default=True, help="Object policy Allow Decryption. Enabled by Default")
@click.option('--key_derive', type=bool, default=True, help="Object policy Allow Key Derivation. Enabled by Default")
@click.option('--wrap', type=bool, default=True, help="Object policy Allow Wrap. Enabled by Default")
@click.option('--generate', type=bool, default=True, help="Object policy Allow Generate. Enabled by Default")
@click.option('--write', type=bool, default=True, help="Object policy Allow Write. Enabled by Default")
@click.option('--import_export', type=bool, default=True,
              help="Object policy Allow Import Export. Enabled by Default")
@click.option('--desfire_auth', type=bool, default=True,
              help="Object policy Allow to perform DESFire authentication. Enabled by Default")
@click.option('--desfire_dump', type=bool, default=True,
              help="Object policy Allow to dump DESFire session keys. Enabled by Default")
@click.option('--forbid_derived_output', type=bool, default=False,
              help="Object policy Forbid Derived Output. Disabled by Default")
@click.option('--kdf_ext_random', type=bool, default=True,
              help="Object policy Allow key derivation ext random. Enbaled by Default")
def symkey(cli_ctx, policy_name, auth_obj_id, sign, verify, encrypt, decrypt, key_derive, wrap, generate,
           write, import_export, desfire_auth, desfire_dump, forbid_derived_output, kdf_ext_random):
    """ Create Symmetric key object policy. \n

    policy_name = File name of the policy to be created.
    This policy name should be given as input while provisioning.\n

    auth_obj_id = Auth object id for each Object Policy. \n

    """
    auth_obj_id = int(auth_obj_id, 16)
    policy_obj = Policy()
    policy_obj.sign = sign
    policy_obj.verify = verify
    policy_obj.encrypt = encrypt
    policy_obj.decrypt = decrypt
    policy_obj.key_derive = key_derive
    policy_obj.wrap = wrap
    policy_obj.generate = generate
    policy_obj.write = write
    policy_obj.import_export = import_export
    policy_obj.desfire_auth = desfire_auth
    policy_obj.desfire_dump = desfire_dump
    policy_obj.forbid_derived_output = forbid_derived_output
    policy_obj.kdf_ext_random = kdf_ext_random
    status = policy_obj.create_obj_policy(policy_type["Symmetric_Key"], policy_name, auth_obj_id)
    if status == apis.kStatus_SSS_Success:
        cli_ctx.log("Object Policy created successfully")
        ret_value = 0
    else:
        cli_ctx.log("Error: Could not create object Policy")
        ret_value = 1
    sys.exit(ret_value)


@policy.command('userid', short_help='Create User ID Object Policy')
@pass_context
@click.argument('policy_name', type=str, metavar='policy_name')
@click.argument('auth_obj_id', type=str, metavar='auth_obj_id')
@click.option('--write', type=bool, default=True, help="Object policy Allow Write. Enabled by Default")
def userid(cli_ctx, policy_name, auth_obj_id, write):
    """ Create user id object policy. \n

    policy_name = File name of the policy to be created.
    This policy name should be given as input while provisioning.\n

    auth_obj_id = Auth object id for each Object Policy. \n

    """
    auth_obj_id = int(auth_obj_id, 16)
    policy_obj = Policy()
    policy_obj.write = write
    status = policy_obj.create_obj_policy(policy_type["user_id"], policy_name, auth_obj_id)
    if status == apis.kStatus_SSS_Success:
        cli_ctx.log("Object Policy created successfully")
        ret_value = 0
    else:
        cli_ctx.log("Error: Could not create object Policy")
        ret_value = 1
    sys.exit(ret_value)


@policy.command('file', short_help='Create Binary file Object Policy')
@pass_context
@click.argument('policy_name', type=str, metavar='policy_name')
@click.argument('auth_obj_id', type=str, metavar='auth_obj_id')
@click.option('--write', type=bool, default=True, help="Object policy Allow Write. Enabled by Default")
@click.option('--read', type=bool, default=True, help="Object policy Allow Read. Enabled by Default")
def file(cli_ctx, policy_name, auth_obj_id, write, read):
    """ Create Binary file object policy. \n

    policy_name = File name of the policy to be created.
    This policy name should be given as input while provisioning.\n

    auth_obj_id = Auth object id for each Object Policy. \n

    """
    auth_obj_id = int(auth_obj_id, 16)
    policy_obj = Policy()
    policy_obj.write = write
    policy_obj.read = read
    status = policy_obj.create_obj_policy(policy_type["file"], policy_name, auth_obj_id)
    if status == apis.kStatus_SSS_Success:
        cli_ctx.log("Object Policy created successfully")
        ret_value = 0
    else:
        cli_ctx.log("Error: Could not create object Policy")
        ret_value = 1
    sys.exit(ret_value)


@policy.command('counter', short_help='Create Counter Object Policy')
@pass_context
@click.argument('policy_name', type=str, metavar='policy_name')
@click.argument('auth_obj_id', type=str, metavar='auth_obj_id')
@click.option('--write', type=bool, default=True, help="Object policy Allow Write. Enabled by Default")
@click.option('--read', type=bool, default=True, help="Object policy Allow Read. Enabled by Default")
def counter(cli_ctx, policy_name, auth_obj_id, write, read):
    """ Create Counter object policy. \n

    policy_name = File name of the policy to be created.
    This policy name should be given as input while provisioning.\n

    auth_obj_id = Auth object id for each Object Policy. \n

    """
    auth_obj_id = int(auth_obj_id, 16)
    policy_obj = Policy()
    status = policy_obj.create_obj_policy(policy_type["counter"], policy_name, auth_obj_id, 1, 1,
                                        1, 1, 1, 1, 1, write, read, 1, 1, 1, 1, 1, 0, 1, 0, 0, 0)
    if status == apis.kStatus_SSS_Success:
        cli_ctx.log("Object Policy created successfully")
        ret_value = 0
    else:
        cli_ctx.log("Error: Could not create object Policy")
        ret_value = 1
    sys.exit(ret_value)


@policy.command('pcr', short_help='Create PCR Object Policy')
@pass_context
@click.argument('policy_name', type=str, metavar='policy_name')
@click.argument('auth_obj_id', type=str, metavar='auth_obj_id')
@click.option('--write', type=bool, default=True, help="Object policy Allow Write. Enabled by Default")
@click.option('--read', type=bool, default=True, help="Object policy Allow Read. Enabled by Default")
def counter(cli_ctx, policy_name, auth_obj_id, write, read):
    """ Create PCR object policy. \n

    policy_name = File name of the policy to be created.
    This policy name should be given as input while provisioning.\n

    auth_obj_id = Auth object id for each Object Policy. \n

    """
    auth_obj_id = int(auth_obj_id, 16)
    policy_obj = Policy()
    policy_obj.write = write
    policy_obj.read = read
    status = policy_obj.create_obj_policy(policy_type["counter"], policy_name, auth_obj_id)
    if status == apis.kStatus_SSS_Success:
        cli_ctx.log("Object Policy created successfully")
        ret_value = 0
    else:
        cli_ctx.log("Error: Could not create object Policy")
        ret_value = 1
    sys.exit(ret_value)


@policy.command('common', short_help='Create Common Object Policy')
@pass_context
@click.argument('policy_name', type=str, metavar='policy_name')
@click.argument('auth_obj_id', type=str, metavar='auth_obj_id')
@click.option('--forbid_all', type=bool, default=False, help="Object policy forbid all. Disabled by Default")
@click.option('--delete', type=bool, default=True, help="Object policy Allow Delete. Enabled by Default")
@click.option('--req_sm', type=bool, default=False, help="Object policy Allow req_sm. Disabled by Default")
def common(cli_ctx, policy_name, auth_obj_id, forbid_all, delete, req_sm):
    """ Create common object policy. \n

    policy_name = File name of the policy to be created.
    This policy name should be given as input while provisioning.\n

    auth_obj_id = Auth object id for each Object Policy. \n

    """
    auth_obj_id = int(auth_obj_id, 16)
    policy_obj = Policy()
    policy_obj.forbid_all = forbid_all
    policy_obj.delete = delete
    policy_obj.req_sm = req_sm
    status = policy_obj.create_obj_policy(policy_type["common"], policy_name, auth_obj_id)
    if status == apis.kStatus_SSS_Success:
        cli_ctx.log("Object Policy created successfully")
        ret_value = 0
    else:
        cli_ctx.log("Error: Could not create object Policy")
        ret_value = 1
    sys.exit(ret_value)


@policy.command('commonpcrvalue', short_help='Create Common PCR Value Object Policy')
@pass_context
@click.argument('policy_name', type=str, metavar='policy_name')
@click.argument('auth_obj_id', type=str, metavar='auth_obj_id')
@click.option('--pcr_obj_id', type=str, default='0', help="Object policy PCR object ID. Zero by Default")
@click.option('--pcr_expected_value', type=str, default='', help="Object policy PCR Expected Value. Zero by Default")
def commonpcrvalue(cli_ctx, policy_name, auth_obj_id, pcr_obj_id, pcr_expected_value):
    """ Create Common PCR Value object policy. \n

    policy_name = File name of the policy to be created.
    This policy name should be given as input while provisioning.\n

    auth_obj_id = Auth object id for each Object Policy. \n

    """
    auth_obj_id = int(auth_obj_id, 16)
    policy_obj = Policy()
    policy_obj.pcr_obj_id = pcr_obj_id
    policy_obj.pcr_expected_value = pcr_expected_value
    status = policy_obj.create_obj_policy(policy_type["common_pcr_value"], policy_name, auth_obj_id)
    if status == apis.kStatus_SSS_Success:
        cli_ctx.log("Object Policy created successfully")
        ret_value = 0
    else:
        cli_ctx.log("Error: Could not create object Policy")
        ret_value = 1
    sys.exit(ret_value)


@policy.command('dump', short_help='Display Created Object Policy')
@pass_context
@click.argument('policy_name', type=str, metavar='policy_name')
def dump(cli_ctx, policy_name):
    """ Display Created object policy. \n

    policy_name = File name of the policy to be displayed.\n

    """
    policy_obj = Policy()
    pol_obj_params = policy_obj.get_object_policy(policy_name)
    if pol_obj_params is not None:
        policy_obj.display_policy(pol_obj_params)
        policy_obj.display_policy_in_hex(pol_obj_params)
        ret_value = 0
    else:
        cli_ctx.log("Policy file not found. Try creating policy first")
        ret_value = 1
    sys.exit(ret_value)


