#
# Copyright 2019 NXP
# SPDX-License-Identifier: Apache-2.0
#
#

PROJECT(se05x_Personalization)

FILE(
    GLOB
    ${PROJECT_NAME}_files
    *.c
    *.h
)

ADD_EXECUTABLE(${PROJECT_NAME} ${KSDK_STARTUP_FILE} ${${PROJECT_NAME}_files})

TARGET_LINK_LIBRARIES(${PROJECT_NAME} ex_common)

CREATE_BINARY(${PROJECT_NAME})

IF(
    CMAKE_BUILD_TYPE
    STREQUAL
    "Release"
)
    COPY_TO_SOURCEDIR(
        ${PROJECT_NAME}
        binaries/ex
        ${SMCOM}
    )
ENDIF()
