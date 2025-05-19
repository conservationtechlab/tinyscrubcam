#
# Copyright 2018-2020 NXP
# SPDX-License-Identifier: Apache-2.0
#
#

"""License text"""

# This script is used to generate keys and certificates using openssl engine.
# The generated keys shall be available in gcp directory
# These keys are used to provision to the secure element

import sys
import os
import logging
logging.basicConfig(format='%(message)s', level=logging.INFO)
log = logging.getLogger(__name__)


def main():
    """
    This function generates key pair and certificates for Google cloud platform.
    :return: None
    """

    # Implementation of this function has been
    # moved to simw-top/pycli/src/Provision/GenerateGCPCredentials.py
    sys.path.append(os.path.abspath(os.path.dirname(__file__)
                                    + os.sep + ".."
                                    + os.sep + "src"))
    import Provision.GenerateGCPCredentials as GenerateGCPCredentials  # pylint: disable=import-outside-toplevel

    cur_dir = os.path.abspath(os.getcwd())
    GenerateGCPCredentials.generateCredentials(cur_dir)


if __name__ == "__main__":
    main()
