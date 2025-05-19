..
    Copyright 2020 NXP





.. highlight:: bat

.. _cli-object-policy:

===========================================================
 Object Policies Through ssscli
===========================================================

Applying policy to objects through ssscli shall be done in two steps.

- Create object policy
- Attach policy to object


Create object policy
^^^^^^^^^^^^^^^^^^^^^^^^^

Object policy shall be created using following command::

    ssscli policy

The create command has ``symkey, asymkey, userid, file, counter, pcr, common`` and  ``commonpcrvalue`` sub commands.

- ``symkey``         -> Symmetric key object policy (AES, DES, HMAC)
- ``asymkey``        -> Asymmetric key object policy (RSA, EC)
- ``userid``         -> User ID Object Policy
- ``file``           -> Binary file Object Policy
- ``counter``        -> Counter Object Policy
- ``pcr``            -> PCR Object Policy
- ``common``         -> Common Object Policy
- ``commonpcrvalue`` -> Common PCR Value Object Policy

Each command has mandatory arguments for ``policy_name`` and ``auth_object_id``.

- ``policy_name`` -> Name of the policy to be created. This policy name should be given as input while provisioning.
- ``auth_object_id`` -> Auth object id for each Object Policy


Create policy command shall have following optional arguments based on the sub command selected:

- ``--sign`` -> Object policy Allow Sign. Enabled by Default. Parameter type is boolean.
- ``--verify`` -> Object policy Allow Verify. Enabled by Default. Parameter type is boolean.
- ``--encrypt`` -> Object policy Allow Encryption. Enabled by Default. Parameter type is boolean.
- ``--decrypt`` -> Object policy Allow Decryption. Enabled by Default. Parameter type is boolean.
- ``--key_derive`` -> Object policy Allow Key Derivation. Enabled by Default. Parameter type is boolean.
- ``--wrap`` -> Object policy Allow Wrap. Enabled by Default. Parameter type is boolean.
- ``--generate`` -> Object policy Allow Generate. Enabled by Default. Parameter type is boolean.
- ``--write`` -> Object policy Allow Write. Enabled by Default. Parameter type is boolean.
- ``--read`` -> Object policy Allow Read. Enabled by Default. Parameter type is boolean.
- ``--import_export`` -> Object policy Allow Import Export. Enabled by Default. Parameter type is boolean.
- ``--key_agreement`` -> Object policy Allow Key Agreement. Enabled by Default. Parameter type is boolean.
- ``--attest`` -> Object policy Allow attestation. Enabled by Default. Parameter type is boolean.
- ``--desfire_auth`` -> Object policy Allow to perform DESFire authentication. Enabled by Default. Parameter type is boolean.
- ``--desfire_dump`` -> Object policy Allow to dump DESFire session keys. Enabled by Default. Parameter type is boolean.
- ``--forbid_all`` -> Object policy forbid all. Disabled by Default. Parameter type is boolean.
- ``--delete`` -> Object policy Allow Delete. Enabled by Default. Parameter type is boolean.
- ``--req_sm`` -> Object policy Allow req_sm. Enabled by Default. Parameter type is boolean.
- ``--pcr_obj_id`` -> Object policy PCR object ID. Zero by Default. Parameter type is hexdecimal.
- ``--pcr_expected_value`` -> Object policy PCR Expected Value. Zero by Default. Parameter type is hexdecimal.
- ``--forbid_derived_output`` -> Object policy forbid derived output. Disabled by Default. Parameter type is boolean.
- ``--kdf_ext_random`` -> Object policy key derivation external random. Enabled by Default. Parameter type is boolean.

The created object policy stored in the system in pickle file format.

Command Sample:

.. image:: object_policy_create.jpg



Usage example::

    ssscli policy asymkey ecc_sign_policy 0x7DA00001 --sign 0
    ssscli policy common ecc_sign_policy 0x7DA00001


Created object policy shall be displayed using following command::

    ssscli policy dump <policy_name>

Usage example:

.. image:: object_policy_display.jpg



Attach policy to object
^^^^^^^^^^^^^^^^^^^^^^^^^

Created object policy shall be applied it to the object along with generate or set command using ``--policy_name`` optional parameter.

Command Sample:

.. image:: object_policy_attach.jpg



Usage example::

    ssscli generate ecc 0x20181001 NIST_P256 --policy_name ecc_sign_policy
    ssscli set ecc pair 0x20182010 nistp521_key.pem --policy_name ecc_sign_policy

