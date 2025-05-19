#
# Copyright 2018-2020 NXP
# SPDX-License-Identifier: Apache-2.0
#
#

"""License text"""

# This script is used to generate keys and CA certificates using openssl engine.
# The generated keys shall be available in ibm directory
# These keys are used to provision to the secure element

import os
import sys
import logging
logging.basicConfig(format='%(message)s', level=logging.INFO)
log = logging.getLogger(__name__)


def main():
    """
    This function generates key pair and certificates for IBM Watson cloud.
    :return: None
    """

    # Implementation of this function has been
    # moved to simw-top/pycli/src/Provision/GenerateIBMCredentials.py
    sys.path.append(os.path.abspath(os.path.dirname(__file__)
                                    + os.sep + ".."
                                    + os.sep + "src"))
    import Provision.GenerateIBMCredentials as GenerateIBMCredentials  # pylint: disable=import-outside-toplevel

    cur_dir = os.path.abspath(os.getcwd())
    GenerateIBMCredentials.generateCredentials(cur_dir)


if __name__ == "__main__":
    main()
