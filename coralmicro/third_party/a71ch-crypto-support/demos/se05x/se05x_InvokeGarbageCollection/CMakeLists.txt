#
# Copyright 2019 NXP
# SPDX-License-Identifier: Apache-2.0
#
#

PROJECT(se05x_InvokeGarbageCollection)
FILE(
    GLOB
    files
    *.c
)

ADD_EXECUTABLE(${PROJECT_NAME} ${KSDK_STARTUP_FILE} ${files})

TARGET_INCLUDE_DIRECTORIES(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/sss/ex/inc)

TARGET_LINK_LIBRARIES(
    ${PROJECT_NAME}
    SSS_APIs
    ex_common
)

CREATE_BINARY(${PROJECT_NAME})

IF(WithHost_LinuxLike)
    INSTALL(TARGETS ${PROJECT_NAME} DESTINATION bin)
ENDIF()
