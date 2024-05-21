..
    Copyright 2019,2020 NXP


.. _cli-tool-provisioning:

======================================================================
 CLI Provisioning
======================================================================

Generating keys and certificates
======================================================================

For generating keys and certificates, following scripts generates using openssl.

- ``GenerateAWSCredentials.py``
- ``GenerateAZURECredentials.py``
- ``GenerateGCPCredentials.py``
- ``GenerateIBMCredentials.py``

The generated keys and certificates shall be available in
``pycli/Provisioning/gcp``, ``pycli/Provisioning/ibm``, ``pycli/Provisioning/azure``
and ``pycli/Provisioning/aws`` directories.

Provisioning for the demo
======================================================================

Generated keys and certificates are used to provision the secure element using ``ResetAndUpdate_GCP.py``, ``ResetAndUpdate_IBM.py``,
``ResetAndUpdate_AZURE.py`` and ``ResetAndUpdate_AWS.py`` scripts for gcp, ibm, azure and aws cloud demo respectively.

.. note:: Default auth type in provisoning script is always ``None``

Steps to provision your device for demo on Windows
======================================================================

Provisioning on windows can be done in two ways.

- Using precompiled binaries
- Using python scripts

Using precompiled binaries
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Precompiled binaries available in ``binaries/pySSSCLI`` directory.
Can generate certificates and provision the secure element by simply running these binaries.

1) For GCP, create certificate and provision, call::

    Provision_GCP.exe <COM_PORT>

#) For IBM, create certificate and provision, call::

    Provision_IBM.exe <COM_PORT>

#) For AWS, create certificate and provision, call::

    Provision_AWS.exe <COM_PORT>

#) For AZURE, create certificate and provision, call::

    Provision_AZURE.exe <COM_PORT>

The generated keys and certificates shall be available in
``binaries/pySSSCLI/gcp``, ``binaries/pySSSCLI/ibm``, ``binaries/pySSSCLI/aws``
and ``binaries/pySSSCLI/azure`` directories.

Using Python scripts
^^^^^^^^^^^^^^^^^^^^^^^^^

1) Complete :numref:`cli-doc-pre-steps` :ref:`cli-doc-pre-steps`

#)  from ``pycli`` directory, run::

        call venv\Scripts\activate.bat
        cd Provisioning

#)  Check the vcom port number

#)  For GCP, create certificate and provision, call::

        python GenerateGCPCredentials.py <COM_PORT>
        python ResetAndUpdate_GCP.py <COM_PORT>

#)  For IBM, create certificate and provision, call::

        python GenerateIBMCredentials.py <COM_PORT>
        python ResetAndUpdate_IBM.py <COM_PORT>

#)  For AWS, create certificate and provision, call::

        python GenerateAWSCredentials.py <COM_PORT>
        python verification_certificate.py <interCA_Certificate> <interCA_Keypair> <verification_code>
        python ResetAndUpdate_AWS.py <COM_PORT>

#)  For AZURE, create certificate and provision, call::

        python GenerateAZURECredentials.py <COM_PORT>
        python verification_certificate.py <interCA_Certificate> <interCA_Keypair> <verification_code>
        python ResetAndUpdate_AZURE.py <COM_PORT>

#)  Flash the demo on to the board


Steps to provision your device for demo on iMX or Raspberry Pi
======================================================================

1) Complete :numref:`cli-doc-pre-steps` :ref:`cli-doc-pre-steps`

#)  from ``pycli`` directory, run::

        cd Provisioning

#)  For GCP, create certificate and provision, call::

        python3 GenerateGCPCredentials.py
        python3 ResetAndUpdate_GCP.py

#)  For IBM, create certificate and provision, call::

        python3 GenerateIBMCredentials.py
        python3 ResetAndUpdate_IBM.py

#)  For AWS, create certificate and provision, call::

        python3 GenerateAWSCredentials.py
        python3 verification_certificate.py <interCA_Certificate> <interCA_Keypair> <verification_code>
        python3 ResetAndUpdate_AWS.py

#)  For AZURE, create certificate and provision, call::

        python3 GenerateAZURECredentials.py
        python3 verification_certificate.py <interCA_Certificate> <interCA_Keypair> <verification_code>
        python3 ResetAndUpdate_AZURE.py

#)  Flash the demo on to the board
