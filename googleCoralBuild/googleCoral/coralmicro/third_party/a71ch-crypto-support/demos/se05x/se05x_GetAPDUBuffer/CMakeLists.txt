# Copyright 2020 NXP
#
# SPDX-License-Identifier: Apache-2.0
#
#

PROJECT(se05x_GetAPDUBuffer)

FILE(
    GLOB
    ${PROJECT_NAME}_files
    *.c
)

ADD_EXECUTABLE(${PROJECT_NAME} ${KSDK_STARTUP_FILE} ${${PROJECT_NAME}_files})

TARGET_LINK_LIBRARIES(
    ${PROJECT_NAME}
    SSS_APIs
    ex_common
)

CREATE_BINARY(${PROJECT_NAME})
