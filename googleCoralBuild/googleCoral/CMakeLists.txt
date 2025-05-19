# Copyright 2022 Google LLC
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

cmake_minimum_required(VERSION 3.13)

# Toolchain must be set before project() call.
if (NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    set(CMAKE_TOOLCHAIN_FILE ${CMAKE_CURRENT_LIST_DIR}/coralmicro/cmake/toolchain-arm-none-eabi-gcc.cmake)
endif()

project(detect_objects_http_usb)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include_directories(coralmicro)
add_subdirectory(coralmicro)

set(OBJECT_DETECT_LINK_LIBRARIES
    libs_base-m7_freertos
    libs_base-m7_http_server
    libs_rpc_http_server
    libs_jpeg_m7
)

#USB
add_executable_m7(detect_objects_http_usb
    detect_objects.cc
    DATA
    ${CMAKE_CURRENT_SOURCE_DIR}/web/Image_view.html>/Image_view.html
    ${PROJECT_SOURCE_DIR}/models/tf2_ssd_mobilenet_v2_coco17_ptq_edgetpu.tflite
)

target_link_libraries(detect_objects_http_usb
    ${OBJECT_DETECT_LINK_LIBRARIES}
)
