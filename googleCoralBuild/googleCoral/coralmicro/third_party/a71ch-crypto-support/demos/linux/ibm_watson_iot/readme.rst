..
    Copyright 2019,2020 NXP



.. highlight:: shell

.. _linux-demos-watson:

==================================================
IBM Watson Demo for iMX Linux / Raspberry Pi
==================================================

This demo demonstrates connection to IBM Watson IoT platform
using pre-provisioned device credentials and publish/subscribe
procedure using MQTT

Prerequisites
==================================================

- IBM Cloud account
- SD Card image with SE050 Middleware pre-installed. The application is built on the iMX platform.
- IMX6UL-EVK platform connected to the Internet

For additional information:

- Refer to :ref:`dev-platforms` for hardware setup and iMX setup
- Refer to :ref:`cli-tool` for ssscli tool setup

Preparing the credentials
==================================================

1) Complete :numref:`cli-doc-pre-steps` :ref:`cli-doc-pre-steps`

#)  To create certificates on imx and Raspberry Pi, call::

        cd simw-top/pycli/Provisioning
        python3 GenerateIBMCredentials.py
        python3 ResetAndUpdate_IBM.py

#) Certificates and Keys are generated at ``simw-top/pycli/Provisioning/ibm``

- The subject and subject alternative name of the device certificate must adhere to specific conventions. Both subject and
  subject alternative name contain the 10 byte UID value. In addition the Subject Alternative Name contains the device type.


Build the OpenSSL engine [Optional]
==================================================

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

.. _linux-watson-run:

Running the Demo on iMX/Raspberry Pi
==================================================

1) Use 'buildScript.sh' script at ``<MW_SRC_DIR>/simw-top/demos/linux/ibm_watson_iot`` to download all dependencies and build the mqtt application for ibm_watson call::

    cd /simw-top/demos/linux/ibm_watson_iot
    ./buildScript.sh

#) Based on OpenSSL version and applicable Secure Element, select the appropriate configuration file in
   ``<MW_SRC_DIR>/simw-top/demos/linux/common`` directory::

        openssl11_sss_a71ch.cnf   ----- OpenSSL 1.1.1 and A71CH
        openssl11_sss_se050.cnf   ----- OpenSSL 1.1.1 and SE050
        openssl_sss_a71ch.cnf     ----- OpenSSL 1.0.0 and A71CH
        openssl_sss_se050.cnf     ----- OpenSSL 1.0.0 and SE050

#) Set the openssl config path. Skip if already done::

    $ export OPENSSL_CONF=<MW_SRC_DIR>/simw-top/demos/linux/common/<appropriate-cnf-file>

#) Upload the root certificate (<MW_SRC_DIR>/simw-top/pycli/Provisioning/ibm/rootCA_certificate.crt) to your IBM account.
   Refer to :ref:`prepare-ibm-cloud` for instructions on uploading the Root CA certificate and registering the device. Skip if already done.

#) Run the application in either of the following two ways:

   -  Parameters via commandline::

        ./watson_imx_linux --org <ORG> --keypath <MW_SRC_DIR>/simw-top/pycli/Provisioning/ibm/<UID>_device_reference_key.pem --devcert simw-top/pycli/Provisioning/ibm/<UID>_device_certificate.cer --topic "iot-2/evt/status/fmt/json" --payload ""{\"d\"\ :\ {\"SensorID\":\ \"Test\"\,\ \"Reading\":\ 7\ }}""

      where *ORG* is the organization ID, *keypath* is the path to reference key corresponding to the device key and *devcert* is the path to device certificate.

   -  Parameters via json file::

        ./watson_imx_linux --json <input.txt>

      Sample JSON file::

        {
          "hostname": "orgID.messaging.internetofthings.ibmcloud.com",
          "protocol": "MQTTS",
          "port": "8443",
          "devcert": "cert_0000000000000000000000000000000000000000000000000000000000000092.pem",
          "keypath": "keyref_0000000000000000000000000000000000000000000000000000000000000092.pem",
          "payload": "HelloMessage",
          "topic": "iot-2/evt/status/fmt/string",
          "rootpath": "rootCA.pem"
        }


.. note::
    1) The above example invocation is for illustrative purpose.
    #) Export the open ssl conf path to the exact location of the file.
    #) While executing the application, use the appropriate values for org, keypath and devcert.


Appendix
==================================================

1. For more information, refer to https://cloud.ibm.com/docs/services/IoT?topic=iot-platform-about_iotplatform

2. <MW_SRC_DIR> is a placeholder for the path to the Plug & Trust MW. It would typically be /home/root/se050_mw_v02.08.00 (or a later version) on i.MX.
