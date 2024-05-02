..
    Copyright 2019 NXP


.. highlight:: shell

.. _se-tool:

==========================================================
 Tool to create Reference key file
==========================================================

This tool is to demonstrate how to implement a command-line utility for native
systems.  This utiltity can be used to generate/inject keypair and create
reference key files.

This is beneficial for environments which do not have python installed.

.. note:: This example is implemented only for NIST-P256 curve.
          It can only be compiled when Host Crypto is OpenSSL.

Building the example
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Use the following CMake configurations to compile the example

- CMake configurations: ``WithHostCrypto_OPENSSL``: ON

- Project: ``seTool``

How to use
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

This example provides four command-line parameters to select the
operation to perform.

1)  To **generate** a keypair, run the tool as::

        seTool genECC <keyId>

    Where:

    - *keyId* is the keypair index at which we want to generate the keypair.


#)  To **inject** a keypair, run the tool as::

        seTool setECC <keyId> <filename>

    Where:

    - *keyId* is the keypair index at which we want to inject the keypair

    - *filename* is the path of the file in which keypair is stored in PEM
      format.


#)  To **retrieve the public key**, run the tool as::

        seTool getPublic <keyId> <filename>

    Where:

    - *keyId* is the keypair index from which we want to retrieve the public
      key

    - *filename* is the path of the file in which we want to store the key in
      PEM format.


#)  To **create a reference key** for an injected keypair, run the tool as::

        seTool getRef <keyId> <filename>

    Where:

    - *keyId* is the keypair index at which we keypair is stored and

    - *filename* is the path of the file in which we want to store the
      reference key in PEM format.

The generated reference key can be used by OpenSSL Engine.
