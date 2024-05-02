#
# Copyright 2018-2020 NXP
# SPDX-License-Identifier: Apache-2.0
#
#

"""License text"""

# This script is used to provision key and certificate to secure element.
# These Provisioned keys and certificates are used in ibm demo

import os
import sys
import logging

log = logging.getLogger(__name__)
logging.basicConfig(format='%(message)s', level=logging.INFO)

# Keypair and certificate index for ibm watson
# (simw-top\demos\ksdk\ibm_watson\ibm_watson_iot_config.h)
KEYPAIR_INDEX_CLIENT_PRIVATE = 0x20181003
CERTIFICATE_INDEX = 0x20181004


def main():
    """
    This function provision the generated key pair and certificates for IBM cloud.
    :return: None
    """
    # Implementation of this function has been
    # moved to simw-top/pycli/src/Provision/ResetAndUpdate_IBM.py
    sys.path.append(os.path.abspath(os.path.dirname(__file__)
                                    + os.sep + ".."
                                    + os.sep + "src"
                                    + os.sep + "Provision"))
    import Provision.ResetAndUpdate_IBM as ResetAndUpdate_IBM  # pylint: disable=import-outside-toplevel

    cur_dir = os.getcwd()
    ResetAndUpdate_IBM.reset_and_update(cur_dir, KEYPAIR_INDEX_CLIENT_PRIVATE, CERTIFICATE_INDEX)


if __name__ == '__main__':
    main()
