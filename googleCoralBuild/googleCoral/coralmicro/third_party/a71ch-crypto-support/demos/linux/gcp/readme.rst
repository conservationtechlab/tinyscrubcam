..
    Copyright 2019,2020 NXP



.. highlight:: shell

.. _linux-demos-gcp:

==================================================
GCP Demo for iMX Linux / Raspberry Pi
==================================================

This demo demonstrates connection to Google Cloud Platform using
pre-provisioned device credentials and publish/subscribe procedure
using MQTT.

Prerequisites
==================================================

- GCP account
- SD Card image with SE050 Middleware pre-installed. The application is built on the iMX platform.
- IMX6UL-EVK/RPi platform connected to the Internet
- Install autoconf and libtool. Execute - ``sudo apt-get install autoconf libtool``

For additional information:

- Refer to :ref:`dev-platforms` for hardware setup and iMX setup
- Refer to :ref:`cli-tool` for ssscli tool setup

Preparing the credentials and Provision the SE
==================================================

1) Complete :numref:`cli-doc-pre-steps` :ref:`cli-doc-pre-steps`

#)  To create certificates on imx and Raspberry Pi, call::

        cd simw-top/pycli/Provisioning
        python3 GenerateGCPCredentials.py
        python3 ResetAndUpdate_GCP.py

#) Certificates and Keys are generated at ``simw-top/pycli/Provisioning/gcp``


Build the OpenSSL engine [Optional]
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. note::
    This step is optional in case you are using a prepared
    SD card image from NXP.

The OpenSSL engine uses the sss abstraction layer to access the crypto
services of the secure element, the implementation remains dependent on
the secure element attached. The following illustrates compiling the
OpenSSL engine for SE050 connected over I2C.

::

   cd simw-top
   python scripts/create_cmake_projects.py
   cd ../simw-top_build/imx_native_se050_t1oi2c
   cmake --build .
   make install
   ldconfig /usr/local/lib

.. note::
    Replace ``imx_native_se050_t1oi2c`` with ``raspbian_native_se050_t1oi2c``
    when building for Raspberry Pi.


.. _linux-gcp-run:

Building the application
==================================================

#) Use 'buildScript.sh' script at simw-top/demos/linux/gcp/ to download all dependencies and build the mqtt application for gcp call::

    cd /simw-top/demos/linux/gcp/
    ./buildScript.sh

#) Search for `default_algorithms` in ``/simw-top/demos/linux/common/openssl_sss_se050.cnf`` file and set it as ::

        default_algorithms = RSA,RAND,ECDSA,ECDH    ----- For openssl 1.0.0
        default_algorithms = RSA,RAND,EC            ----- For openssl 1.1.1

#) Set the openssl config path. Skip if already done::

    $ export OPENSSL_CONF=/simw-top/demos/linux/common/openssl_sss_se050.cnf

#) Upload the root certificate (/simw-top/pycli/Provisioning/gcp/rootCA_certificate.cer) and device certificate (/simw-top/pycli/Provisioning/gcp/<UID>_device_certificate.cer) to GCP account. Refer :ref:`prepare-gcp-cloud`. Skip if already done.

#) Run the application call::

    cd /simw-top/demos/linux/gcp/gcp/cpp-docs-samples/iot/mqtt-ciotc
    $ ./mqtt_ciotc --deviceid "nxp-ecc-dev-01" --region "us-central1" --registryid "nxp-se-demo-reg" --projectid "pgh-cloud-iot" --keypath /simw-top/pycli/Provisioning/gcp/<UID>_device_reference_key.pem --rootpath /simw-top/demos/linux/gcp/keys/roots.pem --algorithm ES256


.. note::
  #. The above example is for illustrative purpose
  #. Export the open ssl conf path to the exact location of the file.
  #. While executing the application, use the appropriate values for registryid, projectid, keypath, rootpath and algorithm


Appendix
========

1. For more information, refer to https://github.com/GoogleCloudPlatform/cpp-docs-samples/tree/master/iot/mqtt-ciotc

