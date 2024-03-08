# Copyright 2019 NXP
#
# SPDX-License-Identifier: Apache-2.0
#

SET(DEMOS_KSDK_DIR ${CMAKE_SOURCE_DIR}/demos/ksdk)

PROJECT(freertos-kernel)

FILE(
    GLOB
    files
    amazon-freertos/freertos_kernel/*.c
    amazon-freertos.cmake
)

IF(WithHost_PCWindows)
    FILE(
        GLOB
        port_files
        ${CMAKE_SOURCE_DIR}/demos/ksdk/common/amazon-freertos/lib/FreeRTOS/portable/MSVC-MingW/*.c
        amazon-freertos/freertos_kernel/portable/MemMang/heap_3.c
        ${DEMOS_KSDK_DIR}/x86pc/*.h
        amazon-freertos/libraries/3rdparty/tracealyzer_recorder/Include/*.h
        amazon-freertos/lib/include/private/*.h
    )
ENDIF()

IF(WithHost_frdmk64f)
    FILE(
        GLOB
        port_files
        amazon-freertos/freertos_kernel/portable/GCC/ARM_CM4F/*.c
        amazon-freertos/freertos_kernel/portable/MemMang/heap_3.c
        ${CMAKE_SOURCE_DIR}/demos/ksdk/common/freertos/*.c
    )
ENDIF()


IF(WithHost_evkmimxrt1060)
    FILE(
        GLOB
        port_files
        amazon-freertos/freertos_kernel/portable/GCC/ARM_CM4F/*.c
        amazon-freertos/freertos_kernel/portable/MemMang/heap_3.c
        ${CMAKE_SOURCE_DIR}/demos/ksdk/common/freertos/*.c
    )
ENDIF()

IF(WithHost_lpcxpresso55s)
    FILE(
        GLOB
        port_files
        amazon-freertos/freertos_kernel/portable/GCC/ARM_CM33_NTZ/non_secure/*.c
        amazon-freertos/freertos_kernel/portable/MemMang/heap_4.c
        ${CMAKE_SOURCE_DIR}/demos/ksdk/common/freertos/*.c
    )
ENDIF()

IF(
    WithApplet_A71XX
    OR WithApplet_SE050_EAR
    OR WithApplet_SE05X
    OR WithApplet_LoopBack
)
    LIST(
        APPEND
        port_files
        ${CMAKE_SOURCE_DIR}/hostlib/hostLib/libCommon/infra/sm_demo_utils_rtos.c
    )
ENDIF()

ADD_LIBRARY(${PROJECT_NAME} ${files} ${port_files})

TARGET_INCLUDE_DIRECTORIES(
    ${PROJECT_NAME}
    PUBLIC amazon-freertos/freertos_kernel/include
    #PUBLIC amazon-freertos/lib/include/private
    PUBLIC amazon-freertos/libraries/abstractions/wifi/include
    PUBLIC ${CMAKE_SOURCE_DIR}/sss/ex/inc
)

IF(WithHost_PCWindows)
    TARGET_INCLUDE_DIRECTORIES(
        ${PROJECT_NAME}
        PUBLIC ${DEMOS_KSDK_DIR}/x86pc
        PUBLIC amazon-freertos/libraries/3rdparty/tracealyzer_recorder/Include
        PUBLIC ${CMAKE_SOURCE_DIR}/demos/ksdk/common/amazon-freertos/lib/FreeRTOS/portable/MSVC-MingW/
    )
ENDIF()

IF(WithHost_frdmk64f)
    TARGET_INCLUDE_DIRECTORIES(${PROJECT_NAME} PUBLIC amazon-freertos/freertos_kernel/portable/GCC/ARM_CM4F)
ENDIF()

IF(WithHost_evkmimxrt1060)
    TARGET_INCLUDE_DIRECTORIES(${PROJECT_NAME} PUBLIC amazon-freertos/freertos_kernel/portable/GCC/ARM_CM4F)
ENDIF()

IF(WithHost_lpcxpresso55s)
    TARGET_INCLUDE_DIRECTORIES(
        ${PROJECT_NAME} PUBLIC amazon-freertos/freertos_kernel/portable/GCC/ARM_CM33_NTZ/non_secure
    )
ENDIF()

ADD_DEFINITIONS(-DSSS_USE_FTR_FILE)

#IF(WithHost_lpcxpresso55s)
#  MESSAGE("No FreeRTOS IP")
#ELSE()

PROJECT(freertos-ip)

FILE(
    GLOB
    files
    #amazon-freertos/lib/bufferpool/aws_bufferpool_static_thread_safe.c
    amazon-freertos/libraries/freertos_plus/standard/crypto/src/iot_crypto.c
    #amazon-freertos/lib/crypto/*.c
    amazon-freertos/libraries/freertos_plus/standard/freertos_plus_tcp/source/*.c
    #amazon-freertos/lib/FreeRTOS-Plus-TCP/source/*.c
    amazon-freertos/libraries/freertos_plus/standard/freertos_plus_tcp/source/portable/BufferManagement/BufferAllocation_2.c
    #amazon-freertos/lib/FreeRTOS-Plus-TCP/source/portable/BufferManagement/BufferAllocation_2.c
    #amazon-freertos/lib/mqtt/*.c
    amazon-freertos/libraries/3rdparty/jsmn/*.c
    amazon-freertos/libraries/freertos_plus/standard/tls/src/iot_tls.c
    amazon-freertos/libraries/freertos_plus/standard/pkcs11/src/iot_pkcs11.c
    #amazon-freertos/lib/tls/*.c
    amazon-freertos/libraries/freertos_plus/standard/utils/src/iot_system_init.c
    #amazon-freertos/lib/utils/aws_system_init.c
    #amazon-freertos/libraries/abstractions/secure_sockets/lwip/iot_secure_sockets.c
    amazon-freertos/libraries/c_sdk/standard/common/iot_init.c
    amazon-freertos/libraries/c_sdk/standard/common/iot_device_metrics.c
    amazon-freertos/libraries/c_sdk/standard/mqtt/src/iot_mqtt_agent.c
    amazon-freertos/libraries/c_sdk/standard/mqtt/src/iot_mqtt_api.c
    amazon-freertos/libraries/c_sdk/standard/mqtt/src/iot_mqtt_operation.c
    amazon-freertos/libraries/c_sdk/standard/mqtt/src/iot_mqtt_serialize.c
    amazon-freertos/libraries/c_sdk/standard/mqtt/src/iot_mqtt_subscription.c
    amazon-freertos/libraries/c_sdk/standard/mqtt/src/iot_mqtt_validate.c
    amazon-freertos/libraries/c_sdk/standard/mqtt/src/iot_mqtt_network.c
    amazon-freertos/libraries/c_sdk/standard/common/taskpool/*.c
    amazon-freertos/libraries/abstractions/platform/freertos/*.c
    amazon-freertos/libraries/c_sdk/standard/common/logging/iot_logging.c
    amazon-freertos/libraries/freertos_plus/standard/utils/src/iot_pki_utils.c
    amazon-freertos.cmake
)

IF(NOT WithHost_PCWindows)
    LIST(
        APPEND
        files
        amazon-freertos/libraries/c_sdk/standard/common/logging/iot_logging_task_dynamic_buffers.c
    )
ENDIF()

IF(WithHost_PCWindows)
    FILE(
        GLOB
        port_files
        # amazon-freertos/lib/secure_sockets/portable/pc/windows/aws_secure_
        # sockets.c
        # amazon-freertos/lib/pkcs11/mbedtls/aws_pkcs11_mbedtls.c
        ${DEMOS_KSDK_DIR}/x86pc/*.h
        ${DEMOS_KSDK_DIR}/x86pc/*.c
        ${DEMOS_KSDK_DIR}/x86pc/win_pcap/*.h
        ${CMAKE_SOURCE_DIR}/demos/ksdk/common/amazon-freertos/lib/FreeRTOS-Plus-TCP/Source/portable/Compiler/MSVC/*.h
        ${CMAKE_SOURCE_DIR}/demos/ksdk/common/amazon-freertos/lib/FreeRTOS-Plus-TCP/source/portable/NetworkInterface/WinPCap/NetworkInterface.c
        # amazon-freertos/lib/pkcs11/portable/pc/windows/*.c
        amazon-freertos/libraries/abstractions/secure_sockets/freertos_plus_tcp/iot_secure_sockets.c
        # amazon-freertos/lib/secure_sockets/portable/freertos_plus_tcp/*.c
        amazon-freertos/libraries/3rdparty/tracealyzer_recorder/*.c
        # amazon-freertos/lib/third_party/tracealyzer_recorder/*.c
    )
ENDIF()

IF(WithKSDK)
    IF(NOT WithHost_lpcxpresso55s)
        FILE(
            GLOB
            port_files
            mcu-sdk/middleware/lwip/src/api/*.c
            #amazon-freertos/libraries/abstractions/secure_sockets/lwip/iot_secure_sockets.c
            #amazon-freertos/lib/secure_sockets/portable/nxp/lwip/*.c
            amazon-freertos/vendors/nxp/secure_sockets/lwip/iot_secure_sockets.c
            mcu-sdk/middleware/lwip/src/core/*.c
            mcu-sdk/middleware/lwip/src/core/ipv4/*.c
            mcu-sdk/middleware/lwip/src/api/*.c
            mcu-sdk/middleware/lwip/src/netif/ethernet.c
            mcu-sdk/middleware/lwip/port/enet_ethernetif.c
            mcu-sdk/middleware/lwip/port/enet_ethernetif_kinetis.c
            mcu-sdk/middleware/lwip/port/sys_arch.c
            mcu-sdk/devices/${KSDK_CPUName}/drivers/fsl_enet.c
        )
    ENDIF()
ENDIF()

IF(WithHost_lpcxpresso55s)
    FILE(
        GLOB
        port_files
        mcu-sdk/middleware/wifi_qca/common_src/api_interface/*.c
        mcu-sdk/middleware/wifi_qca/common_src/bmi/*.c
        mcu-sdk/middleware/wifi_qca/common_src/driver/*.c
        mcu-sdk/middleware/wifi_qca/common_src/hcd/*.c
        mcu-sdk/middleware/wifi_qca/common_src/htc/*.c
        mcu-sdk/middleware/wifi_qca/common_src/hw_interface/*.c
        mcu-sdk/middleware/wifi_qca/common_src/qapi/*.c
        mcu-sdk/middleware/wifi_qca/common_src/reorder/*.c
        mcu-sdk/middleware/wifi_qca/common_src/stack_common/*.c
        mcu-sdk/middleware/wifi_qca/common_src/storerecall/*.c
        mcu-sdk/middleware/wifi_qca/common_src/util/*.c
        mcu-sdk/middleware/wifi_qca/common_src/wmi/*.c
        mcu-sdk/middleware/wifi_qca/custom_src/api_interface/*.c
        mcu-sdk/middleware/wifi_qca/custom_src/driver/*.c
        mcu-sdk/middleware/wifi_qca/custom_src/hw_interface/*.c
        mcu-sdk/middleware/wifi_qca/custom_src/qapi/*.c
        mcu-sdk/middleware/wifi_qca/custom_src/stack_custom/*.c
        mcu-sdk/middleware/wifi_qca/port/drivers/flexcomm_freertos/*.c
        mcu-sdk/middleware/wifi_qca/port/env/freertos/*.c
        amazon-freertos/vendors/nxp/wifi/qca_400x/iot_wifi.c
        amazon-freertos/vendors/nxp/secure_sockets/wifi_qca/iot_secure_sockets.c
        #amazon-freertos/lib/wifi/portable/nxp/qca_400x/*.c
        #amazon-freertos/lib/secure_sockets/portable/nxp/wifi_qca/*.c
    )
ENDIF()

IF(WithmbedTLS_ALT)
    FILE(
        GLOB
        alt_files
        ${CMAKE_SOURCE_DIR}/sss/plugin/pkcs11/*.c
        #amazon-freertos/lib/pkcs11/portable/nxp/se_hostlib/*.c
    )
ENDIF()

ADD_LIBRARY(
    ${PROJECT_NAME}
    ${files}
    ${port_files}
    ${alt_files}
)

TARGET_INCLUDE_DIRECTORIES(
    ${PROJECT_NAME}
    PRIVATE amazon-freertos/libraries/3rdparty/pkcs11
    #PRIVATE amazon-freertos/lib/third_party/pkcs11
    PUBLIC amazon-freertos/libraries/freertos_plus/standard/freertos_plus_tcp/include
    #PUBLIC amazon-freertos/lib/FreeRTOS-Plus-TCP/include
    #PUBLIC amazon-freertos/lib/FreeRTOS-Plus-TCP/Source/portable/BufferManagement
    PUBLIC amazon-freertos/libraries/3rdparty/jsmn
    PRIVATE amazon-freertos/libraries/freertos_plus/standard/crypto/include
    PRIVATE amazon-freertos/libraries/abstractions/pkcs11/mbedtls
    PRIVATE amazon-freertos/libraries/freertos_plus/standard/tls/include
    PRIVATE amazon-freertos/libraries/freertos_plus/standard/pkcs11/include
    PRIVATE amazon-freertos/demos/include
    PUBLIC amazon-freertos/libraries/c_sdk/standard/common/include/private
    PRIVATE amazon-freertos/libraries/freertos_plus/standard/utils/include
    PUBLIC amazon-freertos/libraries/abstractions/secure_sockets/include
    PUBLIC amazon-freertos/libraries/c_sdk/standard/common/include
    PUBLIC amazon-freertos/libraries/abstractions/platform/freertos/include
    PUBLIC amazon-freertos/libraries/c_sdk/standard/mqtt/include
    PUBLIC amazon-freertos/libraries/abstractions/platform/include
)

IF(WithHost_PCWindows)
    TARGET_INCLUDE_DIRECTORIES(
        ${PROJECT_NAME}
        PUBLIC ${DEMOS_KSDK_DIR}/x86pc
        PUBLIC ${DEMOS_KSDK_DIR}/common
        PUBLIC amazon-freertos/libraries/3rdparty/win_pcap
        # PUBLIC amazon-freertos/lib/third_party/win_pcap
        PUBLIC ${CMAKE_SOURCE_DIR}/demos/ksdk/common/amazon-freertos/lib/FreeRTOS-Plus-TCP/Source/portable/Compiler/MSVC
    )
ENDIF()

IF(WithmbedTLS_ALT)
    TARGET_INCLUDE_DIRECTORIES(
        ${PROJECT_NAME}
        #PUBLIC amazon-freertos/lib/pkcs11/portable/nxp/se_hostlib/
        #PUBLIC amazon-freertos/lib/third_party/mcu_vendor/nxp/mflash/se_hostlib
        PUBLIC ../sss/ex/inc
        PUBLIC ${CMAKE_SOURCE_DIR}/sss/plugin/pkcs11
    )
ENDIF()

IF(WithKSDK)
    TARGET_INCLUDE_DIRECTORIES(
        ${PROJECT_NAME}
        PUBLIC amazon-freertos/libraries/freertos_plus/standard/freertos_plus_tcp/source/portable/Compiler/GCC
        #PUBLIC amazon-freertos/lib/FreeRTOS-Plus-TCP/source/portable/Compiler/GCC
        PUBLIC ../demos/ksdk/common
        PUBLIC ../demos/ksdk/gcp
        #PUBLIC amazon-freertos/lib/pkcs11/portable/nxp/se_hostlib
        PUBLIC ../mcu-sdk/middleware/lwip/src/include
    )
ENDIF()

TARGET_INCLUDE_DIRECTORIES(${PROJECT_NAME} PUBLIC ${DEMOS_KSDK_DIR}/common)

TARGET_LINK_LIBRARIES(
    ${PROJECT_NAME}
    mbedtls
    freertos-kernel
    ex_common
)

IF(WithHost_lpcxpresso55s)
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} wifi_qca)
ENDIF()

IF(WithApplet_A71XX OR WithApplet_SE050_EAR)
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} a7x_utils)
ENDIF()

IF(WithHost_PCWindows)
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} wpcap)
ENDIF()

#ENDIF() #FreeRTOS IP

IF(
    "${CMAKE_CXX_COMPILER_ID}"
    STREQUAL
    "GNU"
)
    TARGET_COMPILE_OPTIONS(
        ${PROJECT_NAME}
        PRIVATE -Wno-error=unused-variable
        PRIVATE -Wno-unused-variable
        PRIVATE -Wno-address-of-packed-member
        PRIVATE -Wno-unused-function
        PRIVATE -Wno-array-bounds 
    )
ENDIF()

IF(
    "${CMAKE_CXX_COMPILER_ID}"
    MATCHES
    "MSVC"
)
    TARGET_COMPILE_OPTIONS(
        ${PROJECT_NAME}
        PRIVATE
            /wd4310 # cast truncates constant value
            /wd4127 # conditional expression is constant
            /wd4189 # local variable is initialized but not referenced
            /wd4005 # macro redefinition
            /wd4456 # hides previous local declaration
            /wd4101 # unreferenced local variable
            /wd4267 # possible loss of data
            /wd4295 # array is too small to include a terminating null character
            /wd4057 # differs in indirection to slightly different base types
            /wd4200 # nonstandard extension used: zero-sized array in struct/union
            /wd4055 # type cast: from data pointer to function pointer
    )
ENDIF()
