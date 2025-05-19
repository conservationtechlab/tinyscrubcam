#
# Copyright 2019,2020 NXP
# SPDX-License-Identifier: Apache-2.0
#
#

PROJECT(ex_a71ch_sss_ecc)

ADD_EXECUTABLE(${PROJECT_NAME} ${KSDK_STARTUP_FILE} ${PROJECT_NAME}.c)

TARGET_LINK_LIBRARIES(
    ${PROJECT_NAME}
    SSS_APIs
    ex_common
    a7x_utils
    a71ch
)

CREATE_BINARY(${PROJECT_NAME})

PROJECT(ex_a71ch_sss_aes_wrap_key)

ADD_EXECUTABLE(${PROJECT_NAME} ${KSDK_STARTUP_FILE} ${PROJECT_NAME}.c)

TARGET_LINK_LIBRARIES(
    ${PROJECT_NAME}
    SSS_APIs
    ex_common
    a7x_utils
    a71ch
)

CREATE_BINARY(${PROJECT_NAME})

IF(NXPInternal)
    ADD_SUBDIRECTORY(tst)
ENDIF(NXPInternal)
