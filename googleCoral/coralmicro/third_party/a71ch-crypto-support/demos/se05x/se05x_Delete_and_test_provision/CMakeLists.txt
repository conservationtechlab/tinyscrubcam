#
# Copyright 2019,2020 NXP
# SPDX-License-Identifier: Apache-2.0
#
#

PROJECT(se05x_Delete_and_test_provision)

FILE(
    GLOB
    files
    *.c
)

ADD_EXECUTABLE(${PROJECT_NAME} ${KSDK_STARTUP_FILE} ${files})

TARGET_INCLUDE_DIRECTORIES(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/sss/ex/inc)

TARGET_LINK_LIBRARIES(
    ${PROJECT_NAME}
    ex_common
    SSS_APIs
)

CREATE_BINARY(${PROJECT_NAME})

IF(
    CMAKE_BUILD_TYPE
    STREQUAL
    "Release"
)
    COPY_TO_SOURCEDIR(
        ${PROJECT_NAME}
        binaries/tst
        ${SMCOM}-${SE05X_Auth}
    )
ENDIF()

IF(
    "${CMAKE_CXX_COMPILER_ID}"
    MATCHES
    "MSVC"
)
    # 1900 matches VS2015
    IF(
        ${MSVC_VERSION}
        LESS_EQUAL
        1900
    )
        TARGET_COMPILE_OPTIONS(${PROJECT_NAME} PUBLIC /wd4127 # conditional expression is constant
        )
    ENDIF()
ENDIF()
