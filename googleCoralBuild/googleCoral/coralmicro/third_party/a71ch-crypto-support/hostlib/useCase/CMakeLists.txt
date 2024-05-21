#
# Copyright 2019,2020 NXP
# SPDX-License-Identifier: Apache-2.0
#
#

IF(WithApplet_A71CH_SIM AND NXPInternal)
    IF(
        "${SCP}"
        STREQUAL
        "SCP03_HostCrypto"
    )

    ENDIF()
ENDIF()

IF(
    WithApplet_LoopBack
    AND WithHost_frdmk64f
    AND NXPInternal
)
    ADD_SUBDIRECTORY(WithApplet_LoopBack_use)
ENDIF()
