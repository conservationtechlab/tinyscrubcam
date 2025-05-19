#
# Copyright 2019,2020 NXP
# SPDX-License-Identifier: Apache-2.0
#
#

PROJECT(pkcs11)

FILE(
    GLOB
    files
    ${CMAKE_SOURCE_DIR}/sss/plugin/pkcs11/*.c
    #${CMAKE_SOURCE_DIR}/ext/amazon-freertos/lib/pkcs11/portable/nxp/se_hostlib/*.c
)

ADD_DEFINITIONS(-DPKCS11_SESSION_OPEN)

ADD_LIBRARY(${PROJECT_NAME} ${files})

TARGET_INCLUDE_DIRECTORIES(
    ${PROJECT_NAME}
    #PUBLIC ${CMAKE_SOURCE_DIR}/ext/amazon-freertos/lib/include
    #PUBLIC ${CMAKE_SOURCE_DIR}/ext/amazon-freertos/lib/include/private
    PUBLIC ${CMAKE_SOURCE_DIR}/ext/amazon-freertos/libraries/3rdparty/pkcs11
    PUBLIC ${CMAKE_SOURCE_DIR}/ext/amazon-freertos/libraries/freertos_plus/standard/crypto/include
    PUBLIC
        ${CMAKE_SOURCE_DIR}/ext/amazon-freertos/libraries/freertos_plus/standard/pkcs11/include
        #PUBLIC ${CMAKE_SOURCE_DIR}/ext/amazon-freertos/lib/third_party/mcu_vendor/nxp/mflash/se_hostlib
        ###################PRIVATE ${CMAKE_SOURCE_DIR}/demos/se05x/pkcs11_testbench
        ###################PRIVATE ${CMAKE_SOURCE_DIR}/ext/amazon-freertos/lib/third_party/pkcs11
        ###################PRIVATE ${CMAKE_SOURCE_DIR}/ext/amazon-freertos/tests/common/include
        ###################PRIVATE ${CMAKE_SOURCE_DIR}/demos/aws/pkcs11testbench
)

TARGET_INCLUDE_DIRECTORIES(
    ${PROJECT_NAME}
    PUBLIC ${DEMOS_KSDK_DIR}/common
    PUBLIC ${CMAKE_SOURCE_DIR}/sss/plugin/pkcs11
)

TARGET_LINK_LIBRARIES(
    ${PROJECT_NAME}
    ex_common
    freertos-kernel
)

#TARGET_INCLUDE_DIRECTORIES(
#    ${PROJECT_NAME}
#    PRIVATE ${CMAKE_SOURCE_DIR}/demos/se05x/pkcs11_testbench
#    PRIVATE ${CMAKE_SOURCE_DIR}/ext/amazon-freertos/lib/third_party/pkcs11
#    PRIVATE ${CMAKE_SOURCE_DIR}/ext/amazon-freertos/tests/common/include
#    PRIVATE ${CMAKE_SOURCE_DIR}/demos/aws/pkcs11testbench
#)

#TARGET_LINK_LIBRARIES(
#    ${PROJECT_NAME}
#    unity
#)
