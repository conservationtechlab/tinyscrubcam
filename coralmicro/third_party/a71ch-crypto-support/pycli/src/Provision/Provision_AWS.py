#
# Copyright 2019 NXP
# SPDX-License-Identifier: Apache-2.0
#
#

import os
import traceback
import sys
sss_dir = os.path.abspath(os.getcwd()
                                + os.sep + ".."
                                + os.sep + ".."
                                + os.sep + "pycli"
                                + os.sep + "src")
sys.path.append(sss_dir)

from Provision import GenerateAWSCredentials
from Provision import ResetAndUpdate_AWS
from Provision.Provision_util import *

# Keypair, intermediate keypair, certificate and intermediate certificate index for AWS
# (simw-top\demos\ksdk\common\aws_iot_config.h)
KEYPAIR_INDEX_CLIENT_PRIVATE = 0x20181005
KEYPAIR_INDEX_CLIENT_INTERMEDIATE = 0x20181006
CERTIFICATE_INDEX_CLIENT = 0x20181007
CERTIFICATE_INDEX_INTERMEDIATE = 0x20181008


def main():
    try:
        # working directory for SIMW-TOP/binaries/pySSSCLI
        cur_dir = os.getcwd()
        status = GenerateAWSCredentials.generateCredentials(cur_dir)
        if status == STATUS_SUCCESS:
            ResetAndUpdate_AWS.reset_and_update(cur_dir,
                                                KEYPAIR_INDEX_CLIENT_PRIVATE,
                                                KEYPAIR_INDEX_CLIENT_INTERMEDIATE,
                                                CERTIFICATE_INDEX_CLIENT,
                                                CERTIFICATE_INDEX_INTERMEDIATE)
    except Exception as exc:
        log.error("%s" % str(exc))
        error_file = os.getcwd() + os.sep + "error_log.txt"
        if not os.path.isfile(error_file):
            err_write = open(error_file, 'w+')
        else:
            err_write = open(error_file, 'a+')
            err_write.write("\n\n")
        traceback.print_exc(None, err_write)
        err_write.close()


if __name__ == '__main__':
    main()
