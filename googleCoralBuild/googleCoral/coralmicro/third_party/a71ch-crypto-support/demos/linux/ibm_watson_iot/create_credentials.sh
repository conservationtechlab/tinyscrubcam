#!/bin/bash
#
#
# Copyright 2019 NXP
# SPDX-License-Identifier: Apache-2.0
#
#

# NOTE: This script is based upon 'provisionA71CH_WatsonIoT.sh' included in the project
#       hosted on https://github.com/ibm-watson-iot/iot-nxpimxa71ch-c
#
#
#########################################################################################################
##

# Set global variables to default values
gExecMsg=""
gRetValue=0

# Tools
A71CH_CONFIG_TOOL="./a71chConfig_i2c_imx"
SSS_CONFIG_TOOL="ssscli"

# Customization variables
# For Watson IoT Platform Device and Gateway types
#
devType="NXP-A71CH-D"
gwType="NXP-A71CH-G"
#
# For root CA
#
ROOT_CA_CN="NXP Semiconductors DEMO rootCA v E"
CA_EXISTS="FALSE"
CA_ENC_TYPE="ECC"             # Options are RSA or ECC
CA_ECC_CURVE="prime256v1"     # Only used if CA_ENC_TYPE is ECC
# CA_ECC_CURVE="secp384r1"    # Only used if CA_ENC_TYPE is ECC
#
# For intermediate CA - Intermediate CA will be signed by root CA
#
CREATE_INTERMEDIATE_CA="TRUE"
INTERMEDIATE_CA_CN="NXP Semiconductors DEMO intermediateCA v E"
INTERMEDIATE_CA_EXISTS="FALSE"
INTERMEDIATE_CA_ENC_TYPE="ECC"            # Supported type for intermedite CA is ECC
INTERMEDIATE_CA_ECC_CURVE="prime256v1"    # Only used if INTERMEDIATE_CA_ENC_TYPE is ECC
#
# Certificate validity
#
CA_CERT_VALIDITY=7300                     # 20 years
CLIENT_CERT_VALIDITY=7300                 # 20 years

# Root CA Files
if [ "${CA_ENC_TYPE}" == "RSA" ]; then
    rootcaKey="CACertificate_RSA.key"
    rootcaCert="CACertificate_RSA.crt"
    ca_srl="CACertificate_RSA.srl"
else
    rootcaKey="CACertificate_ECC.key"
    rootcaCert="CACertificate_ECC.crt"
    ca_srl="CACertificate_ECC.srl"
fi
# Intermediate CA files - RSA is not supported yet
intercaKey="interCACertificate_ECC.key"
intercaCsr="interCACertificate_ECC.csr"
intercaCert="interCACertificate_ECC.crt"
inter_srl="interCACertificate_ECC.srl"
# Chain of CA root and intermediate certificate
chaincaCert="CACertificateChain.crt"

# UTILITY FUNCTIONS
# -----------------
# xCmd will stop script execution when the program executed does return gRetValue (0 by default) to the shell
xCmd () {
    local command="$*"
    echo ">> ${gExecMsg}"
    echo ">> ${command}"
    ${command}
    local nRetProc="$?"
    if [ ${nRetProc} -ne ${gRetValue} ]
    then
        echo "\"${command}\" failed to run successfully, returned ${nRetProc}"
        echo "** Script execution failed **"
        exit 2
    fi
    echo ""
    # Set global variables to default values
    gExecMsg=""
    gRetValue=0
}

prog_usage() {
    echo "Usage:"
    echo "    $0 [-u <UID-value>] [ip:port]"
}

# Check command line arguments
if [ "$#" -gt 3 ]; then
    echo "Too many arguments"
    prog_usage
    exit 1
else
    # default values
    ip_addr_port_server=""
    SE_UID=""
    if [ "$1" == "-u" ]; then
        if [ "$#" -lt 2 ]; then
            prog_usage
            exit 1
        else
            SE_UID="$2"
            if [ "${#SE_UID}" -ne 20 ]; then
                echo "  The UID (provided as argument) must be exactly 20 characters long"
                echo "The current argument is ${#SE_UID} characters long"
                echo "Exiting..."
                exit 4
            fi
            if [ -n "$3" ]; then
                ip_addr_port_server="$3"
            fi
        fi
    elif [ "$#" -eq 1 ]; then
        ip_addr_port_server="$1"
    elif [ "$#" -eq 0 ]; then
        # No arguments is fine
        echo "Will fetch UID from attached sample"
    else
        # Wrong command line options
        prog_usage
        exit 1
    fi
fi

echo "SE_UID=${SE_UID}"
echo "ip_addr_port_server=${ip_addr_port_server}"

# Provide UID as argument, or
# attach the SE and have the script retrieve the UID from the IC
if [ -z ${SE_UID} ]; then
    echo "  Get UID from attached SE"
    # ${A71CH_CONFIG_TOOL} info device | grep -e "[0-9][0-9]:[0-9][0-9]:[0-9][0-9]"
    # SE_UID="$(${A71CH_CONFIG_TOOL} info device | grep -e "\([0-9][0-9]:\)\{17,\}" \
    #    | awk 'BEGIN {FS=":"}; {printf $3$4$9$10$11$12$13$14$15$16}')"
    xCmd "ssscli disconnect"
    xCmd "ssscli connect se05x t1oi2c none"
    xCmd "ssscli se05x certuid"
    SE_UID="$(ssscli se05x certuid 2>&1 | grep -e 'Cert UID' | grep -o -e '\([0-9a-zA-Z]\)\{20,\}')"
    echo "SE_UID: ${SE_UID}"
fi

# Sanity check on UID
if [ "${#SE_UID}" -ne 20 ]; then
    echo "  Could not retrieve UID from SE."
    echo "Exiting..."
    exit 5
fi

###########################################
# Create root CA key and certificate.
###########################################

if [ "${CA_EXISTS}" != "TRUE" ]; then

  if [ "${CA_ENC_TYPE}" == "RSA" ]; then

    # RSA Root Certificates
    if [ ! -e ${rootcaKey} ]; then
        # Create both the CA keypair and CA certificate
        # The root CA is RSA 4096
        echo "## Create RSA Root CA Key: (${rootcaKey})"
        xCmd "openssl genrsa -out ${rootcaKey} 4096"
        echo "## Create RSA Root CA Certificate: (${rootcaCert})"
        openssl req -x509 -new -nodes -key ${rootcaKey} -sha256 -days ${CA_CERT_VALIDITY} -out ${rootcaCert} -subj "/CN=${ROOT_CA_CN}"
    fi

    if [ ! -e ${rootcaCert} ]; then
        echo "## Create ECC Root CA Certificate: (${rootcaCert}); Root CA keypair was already present"
        openssl req -x509 -new -nodes -key ${rootcaKey} -sha256 -days ${CA_CERT_VALIDITY} -out ${rootcaCert} -subj "/CN=${ROOT_CA_CN}"
    fi

  else

    # ECC Root Certificates
    if [ ! -e ${rootcaKey} ]; then
        # Create both the CA keypair and CA certificate
        # The root CA is ECC using curve defined in CA_ECC_CURVE variable
        echo "## Create ECC Root CA Key: (${rootcaKey}) Curve: (${CA_ECC_CURVE})"
        xCmd "openssl ecparam -genkey -name ${CA_ECC_CURVE} -out ${rootcaKey}"
        echo "## Create ECC Root CA Certificate: (${rootcaCert})"
        openssl req -x509 -new -nodes -key ${rootcaKey} -sha256 -days ${CA_CERT_VALIDITY} -out ${rootcaCert} -subj "/CN=${ROOT_CA_CN}"
    fi

    if [ ! -e ${rootcaCert} ]; then
        echo "## Create ECC Root CA Certificate: (${rootcaCert}); Root CA keypair was already present"
        openssl req -x509 -new -nodes -key ${rootcaKey} -sha256 -days ${CA_CERT_VALIDITY} -out ${rootcaCert} -subj "/CN=${ROOT_CA_CN}"
    fi

  fi

else
    # Check whether RootCA key and certificate are present.
    if [ ! -e ${rootcaKey} ]; then
        if [ "${CA_ENC_TYPE}" == "RSA" ]; then
            echo "## No RSA Root CA Key available (${rootcaKey})"
        else
            echo "## No ECC Root CA Key available (${rootcaKey})"
        fi
        exit 8
    fi

    if [ ! -e ${rootcaCert} ]; then
        if [ "${CA_ENC_TYPE}" == "RSA" ]; then
            echo "## No RSA Root CA Certificate available (${rootcaCert})"
        else
            echo "## No ECC Root CA Certificate available (${rootcaCert})"
        fi
        exit 9
    fi
fi


##############################################
# Create intermediate CA key and certificate.
##############################################

# echo "Check whether a .srl file of Root CA is present"
if [ -e ${ca_srl} ]; then
    echo "## ${ca_srl} already exists, use it"
    x509_serial="-CAserial ${ca_srl}"
else
    echo "## no ${ca_srl} found, create it"
    x509_serial="-CAcreateserial"
fi

SIGN_BY_INTERMEDIATE_CA="FALSE"

echo "## CREATE_INTERMEDIATE_CA = ${CREATE_INTERMEDIATE_CA}"

if [ "${CREATE_INTERMEDIATE_CA}" == "TRUE" ]
then

  echo "## Create Intermediate CA"

  if [ "${INTERMEDIATE_CA_EXISTS}" != "TRUE" ]
  then

    # Define the v3 extension that will be included in the intermediate certificate
    echo "## Create v3 extension file for Intermediate CA Certificate"
    echo "[ v3_req ]"                                          > v3_ext.cnf
    echo "basicConstraints = CA:TRUE, pathlen:0"               >> v3_ext.cnf
    echo "keyUsage = critical, digitalSignature, keyCertSign"  >> v3_ext.cnf


    if [ "${INTERMEDIATE_CA_ENC_TYPE}" == "RSA" ]
    then

        echo "## RSA Encryption type is not supported yet."
        exit 10

    else

        # ECC Intemediate Certificates
        if [ ! -e ${intercaKey} ]
        then
            # Create both the CA keypair and CA certificate
            # The intermediate CA is ECC using curve defined in INTERMEDIATE_CA_ECC_CURVE variable
            echo "## Create ECC intermediate CA Key: (${intercaKey}) Curve: (${INTERMEDIATE_CA_ECC_CURVE})"
            xCmd "openssl ecparam -genkey -name ${INTERMEDIATE_CA_ECC_CURVE} -out ${intercaKey}"
            echo "## Create ECC Intermediate CA CSR: (${intercaCsr})"
            openssl req -new -key ${intercaKey} -out ${intercaCsr} -subj "/CN=${INTERMEDIATE_CA_CN}"
            echo "## Sign Intermediate CA CSR with (${rootcaCert})"
            openssl x509 -req -days ${CA_CERT_VALIDITY} -in ${intercaCsr} ${x509_serial} -CA ${rootcaCert} \
                -CAkey ${rootcaKey}  -extfile ./v3_ext.cnf -extensions v3_req -out ${intercaCert}
        fi

        if [ ! -e ${intercaCert} ]; then
            echo "## Create ECC Intermediate CA CSR: (${intercaCsr})"
            openssl req -new -key ${intercaKey} -out ${intercaCsr} -subj "/CN=${INTERMEDIATE_CA_CN}"
            echo "## Sign Intermediate CA CSR with (${rootcaCert})"
            openssl x509 -req -days ${CA_CERT_VALIDITY} -in ${intercaCsr} ${x509_serial} -CA ${rootcaCert} \
                -CAkey ${rootcaKey}  -extfile ./v3_ext.cnf -extensions v3_req -out ${intercaCert}
        fi

    fi

  else

    # Check whether Intermediate CA key and certificate are present.
    if [ ! -e ${intercaKey} ]; then
        if [ "${INTERMEDIATE_CA_ENC_TYPE}" == "RSA" ]; then
            echo "## RSA Encryption type is not supported yet."
        else
            echo "## No ECC Intermediate CA Key available (${intercaKey})"
        fi
        exit 11
    fi

    if [ ! -e ${intercaCert} ]; then
        if [ "${INTERMEDIATE_CA_ENC_TYPE}" == "RSA" ]; then
            echo "## RSA Encryption type is not supported yet."
        else
            echo "## No ECC Intermediate CA Certificate available (${intercaCert})"
        fi
        exit 12
    fi

  fi

  SIGN_BY_INTERMEDIATE_CA="TRUE"

fi


############################################
# Create key and device/gateway certificates
############################################

if [ "${SIGN_BY_INTERMEDIATE_CA}" == "TRUE" ]; then

    # Create Chain of CA Certificates for WIoTP CA Certificate upload step
    cat ${intercaCert} ${rootcaCert} > ${chaincaCert}
fi

# Create serial number of gateway and device certificates
# for device  1${SE_UID}
# for gateway 2${SE_UID}
deviceSerial="0x01${SE_UID}"
gatewaySerial="0x02${SE_UID}"

# Unique ID Key, reference key and CSR files
uidKey="${SE_UID}.key"
uidRefKey="${SE_UID}_ref.pem"
uidCsr="${SE_UID}.csr"

# Device certificate
deviceCert="${SE_UID}_device_ec_pem.crt"
deviceCertAscii="${SE_UID}_device_ec_crt_ascii_dump.txt"

# Gateway certificate
gatewayCert="${SE_UID}_gateway_ec_pem.crt"
gatewayCertAscii="${SE_UID}_gateway_ec_crt_ascii_dump.txt"

echo "## Preparing device certificate ${deviceCert}"
CN_val="${SE_UID}"
GENERIC_val="URI:NXP:${SE_UID}"
SAN_D_val="email:d:${devType}:${SE_UID}, ${GENERIC_val}"
SAN_G_val="email:g:${gwType}:${SE_UID}, ${GENERIC_val}"

echo "##    CN=${CN_val}"
echo "##    SAN_D_VAL=${SAN_D_val}"
echo "##    SAN_G_VAL=${SAN_G_val}"

# Create a device key pair & CSR
xCmd "openssl ecparam -genkey -name prime256v1 -out ${uidKey}"
openssl req -new -key ${uidKey} -out ${uidCsr} -subj "/CN=${CN_val}"

# Define the v3 extension that will be included in the device certificate
echo "[ v3_req ]"                                    > v3_ext.cnf
echo ""                                             >> v3_ext.cnf
echo "# Extensions to add to a certificate request" >> v3_ext.cnf
echo ""                                             >> v3_ext.cnf
echo "basicConstraints = CA:FALSE"                  >> v3_ext.cnf
echo "# keyUsage = nonRepudiation, digitalSignature, keyEncipherment" >> v3_ext.cnf
echo "keyUsage = digitalSignature"                  >> v3_ext.cnf
echo "subjectAltName = ${SAN_D_val}"                >> v3_ext.cnf

if [ "${SIGN_BY_INTERMEDIATE_CA}" == "TRUE" ]; then
    # Create a Cert signed by Intermediate CA
    xCmd "openssl x509 -req -days ${CLIENT_CERT_VALIDITY} -in ${uidCsr} -set_serial ${deviceSerial} -CA ${intercaCert} -CAkey ${intercaKey} \
        -extfile ./v3_ext.cnf -extensions v3_req -out ${deviceCert}"
else
    # Create a Cert signed by Root CA
    xCmd "openssl x509 -req -days ${CLIENT_CERT_VALIDITY} -in ${uidCsr} -set_serial ${deviceSerial} -CA ${rootcaCert} -CAkey ${rootcaKey} \
        -extfile ./v3_ext.cnf -extensions v3_req -out ${deviceCert}"
fi
openssl x509 -in ${deviceCert} -text -noout > ${deviceCertAscii}
echo "## Device certificate ${deviceCert} created successfully"


# Define the v3 extension that will be included in the gateway certificate
echo "[ v3_req ]"                                    > v3_ext.cnf
echo ""                                             >> v3_ext.cnf
echo "# Extensions to add to a certificate request" >> v3_ext.cnf
echo ""                                             >> v3_ext.cnf
echo "basicConstraints = CA:FALSE"                  >> v3_ext.cnf
echo "# keyUsage = nonRepudiation, digitalSignature, keyEncipherment" >> v3_ext.cnf
echo "keyUsage = digitalSignature"                  >> v3_ext.cnf
echo "subjectAltName = ${SAN_G_val}"                >> v3_ext.cnf
if [ "${SIGN_BY_INTERMEDIATE_CA}" == "TRUE" ]; then
    # Create a Cert signed by Intermediate CA
    xCmd "openssl x509 -req -days ${CLIENT_CERT_VALIDITY} -in ${uidCsr} -set_serial ${gatewaySerial} -CA ${intercaCert} -CAkey ${intercaKey} \
        -extfile ./v3_ext.cnf -extensions v3_req -out ${gatewayCert}"
else
    # Create a Cert signed by Root CA
    xCmd "openssl x509 -req -days ${CLIENT_CERT_VALIDITY} -in ${uidCsr} -set_serial ${gatewaySerial} -CA ${rootcaCert} -CAkey ${rootcaKey} \
        -extfile ./v3_ext.cnf -extensions v3_req -out ${gatewayCert}"
fi
openssl x509 -in ${gatewayCert} -text -noout > ${gatewayCertAscii}
echo "## Gateway certificate ${gatewayCert} created successfully"


###########################################
# Create config tool script and execute it.
###########################################

configScript="${SE_UID}_se050ConfigScript.sh"

echo "## Create config tool script ${configScript}"

echo "# ################################################"                       > ${configScript}
echo "# Name: ${configScript}"                                                 >> ${configScript}
echo "# Revision 0.9"                                                          >> ${configScript}
echo "# Purpose: Provision SE050 matching UID ("${SE_UID}") for Watson IoT"    >> ${configScript}
echo "# Pre-condition: "                                                       >> ${configScript}
echo "# Post-condition: keypair and certificate injected"                      >> ${configScript}
echo "# ################################################"                      >> ${configScript}
echo "ssscli disconnect"                                                       >> ${configScript}
echo "ssscli connect se05x t1oi2c none"                                        >> ${configScript}
echo "ssscli se05x reset   # Bring secure element in its original state"       >> ${configScript}
echo "# key_id=\"0x7DC1B000\""                                                                        >> ${configScript}
echo "ssscli set ecc pair 0x7DC1B000 ${uidKey} --format PEM"                                          >> ${configScript}
echo "ssscli refpem ecc pair 0x7DC1B000 ${uidRefKey} # Creates the reference key on the file system"  >> ${configScript}
# echo "# dev_cert_id=\"0x7DC1B010\""                                                                     >> ${configScript}
# echo "ssscli set cert 0x7DC1B010 ${deviceCert}"                                           >> ${configScript}
# echo "# gw_cert_id=\"0x7DC1B011\""                                                                    >> ${configScript}
# echo "ssscli set cert 0x7DC1B011 ${gatewayCert}"                                           >> ${configScript}

# ssscli set cert [OPTIONS] KEYID KEY

# xCmd "${A71CH_CONFIG_TOOL} script -f ${configScript}"

# echo "## Successfully configured A71CH sample ${SE_UID}"

exit 0


