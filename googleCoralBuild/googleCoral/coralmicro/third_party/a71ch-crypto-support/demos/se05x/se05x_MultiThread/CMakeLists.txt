# Copyright 2020 NXP
#
# SPDX-License-Identifier: Apache-2.0
#
#
PROJECT(se05x_MultiThread)

FILE(
    GLOB
    files
    *.c
    *.h
)
ADD_EXECUTABLE(${PROJECT_NAME} ${KSDK_STARTUP_FILE} ${files})

TARGET_INCLUDE_DIRECTORIES(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/sss/ex/inc)

TARGET_LINK_LIBRARIES(
    ${PROJECT_NAME}
    SSS_APIs
    ex_common
)

IF(CMAKE_COMPILER_IS_GNUCC AND (NOT WithKSDK))
    TARGET_LINK_LIBRARIES(
        ${PROJECT_NAME}
        -lpthread
    )
ENDIF()

EXPOSE_SE05X_AUTH_STRATEGY()
CREATE_BINARY(${PROJECT_NAME})
