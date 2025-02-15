#
# Copyright 2019 NXP
# SPDX-License-Identifier: Apache-2.0
#
#


# This script is used to generate keys and CA certificates using openssl engine.
# The generated keys shall be available in ibm directory
# These keys are used to provision to the secure element


import subprocess
from .Provision_util import *
from . import cloud_credentials


# create certificate extensions
def create_conf_file(extfile, extensions):
    common_extensions = "[ v3_req ] \n\
# Extensions to add to a certificate request    \n\
basicConstraints = CA:FALSE \n\
keyUsage = digitalSignature \n"

    f = open(extfile, "w")
    f.write(common_extensions)
    f.write("subjectAltName = %s"%extensions)
    f.close()


def generateCredentials(cur_dir):

    # Session Open
    session_obj = session_open()
    if session_obj is None:
        return STATUS_FAIL

    # Read UID from the device
    SE_UID = get_unique_id(session_obj)

    # Session Close
    session_close(session_obj)

    if SE_UID is None:
        return STATUS_FAIL

    ibm_dir = cur_dir + os.sep + "ibm"
    if not os.path.exists(ibm_dir):
        try:
            os.makedirs(ibm_dir)
        except OSError:
            print("Error! Failed to create 'ibm' directory")
            return STATUS_FAIL

    OUTPUT_DIR = ibm_dir + os.sep
    if "linux" in sys.platform:
        OPENSSL_EXE = 'openssl'
    else:
        OPENSSL_EXE = os.path.join(cur_dir, '..', '..', 'ext', 'openssl', 'bin', 'openssl.exe')
    OPENSSL_CONFIG = "-config \"%s\"" % (os.path.join(cur_dir, "..", "..", "ext", "openssl", "ssl", "openssl.cnf"))
    EXT_FILE = OUTPUT_DIR + "v3_ext.cnf"
    rootca = OUTPUT_DIR + "rootCA"
    ecc_param_pem = OUTPUT_DIR + "prime256v1.pem"
    ROOT_CA_CN = "NXP Semiconductors DEMO rootCA v Ek"
    CA_CERT_VALIDITY = 7300
    CLIENT_CERT_VALIDITY = 7300
    devType = "NXP-SE050-EC-D"
    gwType = "NXP-SE050-EC-G"
    credentials = cloud_credentials.create_credentials(SE_UID)

    deviceSerial = "0x01%d" % SE_UID
    gatewaySerial = "0x02%d" % SE_UID
    uidKey = OUTPUT_DIR + credentials["DEVICE_KEY"]
    gatewayKey = OUTPUT_DIR + credentials["GATEWAY_KEY"]
    uidCsr = OUTPUT_DIR + "%d_device.csr" % SE_UID
    gatewayCsr = OUTPUT_DIR + "%d_gateway.csr" % SE_UID
    deviceCert = OUTPUT_DIR + credentials["DEVICE_CERT"]
    gatewayCert = OUTPUT_DIR + credentials["GATEWAY_CERT"]
    rootCAKey = OUTPUT_DIR + credentials["ROOT_CA_KEY"]
    rootCACert = OUTPUT_DIR + credentials["ROOT_CA_CERT_CRT"]
    CN_val = "%s" % SE_UID
    GENERIC_val = "URI:NXP:%d" % SE_UID
    SAN_D_val = "email:d:%s:%d, %s" % (devType, SE_UID, GENERIC_val)
    SAN_G_val = "email:g:%s:%d, %s" % (gwType, SE_UID, GENERIC_val)

    error_file = cur_dir + os.sep + "error_log.txt"
    err_write = open(error_file, 'wb+')

    output_file = cur_dir + os.sep + "output_log.txt"
    output_write = open(output_file, 'wb+')

    # create EC parameters with the group 'prime256v1'
    if not os.path.isfile(ecc_param_pem):
        a = subprocess.Popen("\"%s\" ecparam -name prime256v1 -out \"%s\"" % (OPENSSL_EXE, ecc_param_pem), shell=True,
                             stdout=output_write, stderr=err_write)
        a.wait()

    # rootca private Key generation and convert to DER format
    if not os.path.isfile(rootCAKey):
        a = subprocess.Popen(
            "\"%s\" ecparam -in \"%s\" -genkey -out \"%s\"" % (OPENSSL_EXE, ecc_param_pem, rootCAKey),
            shell=True, stdout=output_write, stderr=err_write)
        a.wait()

    # create the rootca certificate
    if not os.path.isfile(rootCACert):
        a = subprocess.Popen("\"%s\" req -x509 -new -nodes -key \"%s\" -sha256 -days %d -subj \"%s\" -out \"%s\" %s"
                             % (OPENSSL_EXE, rootCAKey, CA_CERT_VALIDITY, "/CN=%s" % ROOT_CA_CN,
                                rootCACert, OPENSSL_CONFIG),
                             shell=True, stdout=output_write, stderr=err_write)
        a.wait()
        a = subprocess.Popen("\"%s\" x509 -in \"%s\" -text -noout" % (OPENSSL_EXE, rootCACert), shell=True,
                             stdout=output_write, stderr=err_write)
        a.wait()

    """
    UID Specific
    """
    # re-create EC parameters with the group 'prime256v1'
    if not os.path.isfile(ecc_param_pem):
        a = subprocess.Popen("\"%s\" ecparam -name prime256v1 -out \"%s\"" % (OPENSSL_EXE, ecc_param_pem),
                             shell=True, stdout=output_write, stderr=err_write)
        a.wait()

    # Check if the rootca key and certificate is available
    if not os.path.isfile(rootCAKey):
        print("First create a RootCA key (%s) and matching RootCA certificate (%s)" % (rootCAKey, rootCACert))
        output_write.close()
        err_write.close()
        sys.exit()

    if not os.path.isfile(rootCACert):
        print("First create a RootCA certificate (%s)" % (rootCACert))
        output_write.close()
        err_write.close()
        sys.exit()

    # Generate device private Key
    a = subprocess.Popen("\"%s\" ecparam -genkey -name prime256v1 -out \"%s\"" % (OPENSSL_EXE, uidKey),
                         shell=True, stdout=output_write, stderr=err_write)
    a.wait()
    # Create device certificate signing request
    a = subprocess.Popen("\"%s\" req -new -key \"%s\" -out \"%s\" -subj \"%s\" %s" %
                         (OPENSSL_EXE, uidKey, uidCsr, "/CN=%s" % CN_val, OPENSSL_CONFIG),
                         shell=True, stdout=output_write, stderr=err_write)
    a.wait()

    create_conf_file(EXT_FILE, SAN_D_val)

    # Create a signed device certificate
    a = subprocess.Popen(
        "\"%s\" x509 -req -days %d -in \"%s\" -set_serial %s -CA \"%s\" -CAkey \"%s\" -extfile %s -extensions v3_req -out \"%s\""
        % (OPENSSL_EXE, CLIENT_CERT_VALIDITY, uidCsr, deviceSerial, rootCACert, rootCAKey, EXT_FILE,
           deviceCert),
        shell=True, stdout=output_write, stderr=err_write)
    a.wait()

    a = subprocess.Popen("\"%s\" ecparam -genkey -name prime256v1 -out \"%s\"" % (OPENSSL_EXE, gatewayKey),
                         shell=True, stdout=output_write, stderr=err_write)
    a.wait()
    # Create device certificate signing request
    a = subprocess.Popen("\"%s\" req -new -key \"%s\" -out \"%s\" -subj \"%s\" %s" %
                         (OPENSSL_EXE, gatewayKey, gatewayCsr, "/CN=%s" % CN_val, OPENSSL_CONFIG),
                         shell=True, stdout=output_write, stderr=err_write)
    a.wait()

    create_conf_file(EXT_FILE, SAN_G_val)
    # Create a signed gateway certificate
    a = subprocess.Popen(
        "\"%s\" x509 -req -days %d -in \"%s\" -set_serial %s -CA \"%s\" -CAkey \"%s\" -extfile %s -extensions v3_req -out \"%s\""
        % (OPENSSL_EXE, CLIENT_CERT_VALIDITY, gatewayCsr, gatewaySerial, rootCACert, rootCAKey, EXT_FILE,
           gatewayCert),
        shell=True, stdout=output_write, stderr=err_write)
    a.wait()

    output_write.close()
    err_write.close()

    os.remove(uidCsr)
    os.remove(gatewayCsr)
    os.remove(EXT_FILE)
    print("Generating Credentials Successfull")
    return STATUS_SUCCESS

