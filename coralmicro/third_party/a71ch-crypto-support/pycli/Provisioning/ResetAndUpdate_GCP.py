#
# Copyright 2018-2020 NXP
# SPDX-License-Identifier: Apache-2.0
#
#

"""License text"""

# This script is used to provision key and certificate to secure element.
# These Provisioned keys and certificates are used in gcp demo

import os
import sys
import logging

log = logging.getLogger(__name__)
logging.basicConfig(format='%(message)s', level=logging.INFO)

# Keypair and certificate index for GCP
# (simw-top\demos\ksdk\gcp\gcp_iot_config.h)
KEYPAIR_INDEX_CLIENT_PRIVATE = 0x20181001
CERTIFICATE_INDEX = 0x20181002


def main():
    """
    This function provision the generated key pair and certificates for Google cloud Platform.
    :return: None
    """

    # Implementation of this function has been
    # moved to simw-top/pycli/src/Provision/ResetAndUpdate_GCP.py
    sys.path.append(os.path.abspath(os.path.dirname(__file__)
                                    + os.sep + ".."
                                    + os.sep + "src"
                                    + os.sep + "Provision"))
    import Provision.ResetAndUpdate_GCP as ResetAndUpdate_GCP  # pylint: disable=import-outside-toplevel

    cur_dir = os.getcwd()
    ResetAndUpdate_GCP.reset_and_update(cur_dir, KEYPAIR_INDEX_CLIENT_PRIVATE, CERTIFICATE_INDEX)


if __name__ == '__main__':
    main()
