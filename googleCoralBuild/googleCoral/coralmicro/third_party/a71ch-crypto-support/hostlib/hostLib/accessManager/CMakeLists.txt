#
# Copyright 2019,2020 NXP
# SPDX-License-Identifier: Apache-2.0
#
#

PROJECT(accessManager)

FILE(
    GLOB
    accessManager_sources
    src/accessManager.c
    src/accessManager_com.c
    src/accessManager_rjct.c
)

ADD_EXECUTABLE(${PROJECT_NAME} ${KSDK_STARTUP_FILE} ${accessManager_sources})

TARGET_INCLUDE_DIRECTORIES(${PROJECT_NAME} PUBLIC inc)

TARGET_LINK_LIBRARIES(${PROJECT_NAME} smCom)

IF(SSS_HAVE_SCP_SCP03_SSS)
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} ex_common SSS_APIs)
ENDIF()

CREATE_BINARY(${PROJECT_NAME})

# IF(WithHost_LinuxLike)
#     INSTALL(TARGETS ${PROJECT_NAME} DESTINATION bin)
# ENDIF()

IF(WithHost_LinuxLike)
	MESSAGE("accessManager is not copied to default binary directory upon install")
ENDIF()
