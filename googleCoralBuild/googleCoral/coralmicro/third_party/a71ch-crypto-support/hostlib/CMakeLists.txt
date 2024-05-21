#
# Copyright 2019,2020 NXP
# SPDX-License-Identifier: Apache-2.0
#
#

ADD_SUBDIRECTORY(hostLib)

IF(NXPInternal)
    ADD_SUBDIRECTORY(tst)
ENDIF()

#
# ADD_SUBDIRECTORY(hostlib/a71cl/tst)
#
# ADD_SUBDIRECTORY(hostlib/a71cl/ex_hlse)
#

IF(WithApplet_A71CH OR WithApplet_A71CH_SIM)
    IF(WithRTOS_Default)
        ADD_SUBDIRECTORY(useCase)
    ENDIF()
ENDIF()
