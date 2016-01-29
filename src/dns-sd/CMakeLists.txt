# Copyright (c) 2014-2015 Samsung Electronics Co., Ltd All Rights Reserved
#
#    Licensed under the Apache License, Version 2.0 (the "License");
#    you may not use this file except in compliance with the License.
#    You may obtain a copy of the License at
#
#        http://www.apache.org/licenses/LICENSE-2.0
#
#    Unless required by applicable law or agreed to in writing, software
#    distributed under the License is distributed on an "AS IS" BASIS,
#    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#    See the License for the specific language governing permissions and
#    limitations under the License.
#
# @file        CMakeLists.txt
#

SET(LIB_DNS_SD_PATH ${NSD_PATH}/dns-sd)

INCLUDE_DIRECTORIES(
    ${LIB_DNS_SD_PATH}
    ${NSD_PATH}/include
    )

SET(LIB_DNS_SD_SOURCES
    ${LIB_DNS_SD_PATH}/dns-sd.c
    )

ADD_DEFINITIONS("-fvisibility=default")

ADD_LIBRARY(${TARGET_LIB_NSD_DNS_SD} SHARED ${LIB_DNS_SD_SOURCES})

SET_TARGET_PROPERTIES(
    ${TARGET_LIB_NSD_DNS_SD}
    PROPERTIES
        SOVERSION ${MAJORVER}
        VERSION ${FULLVER}
    )

TARGET_LINK_LIBRARIES(${TARGET_LIB_NSD_DNS_SD}
    ${NSD_DEPS_LIBRARIES}
    dl
    )

INSTALL(TARGETS ${TARGET_LIB_NSD_DNS_SD} DESTINATION ${LIB_DIR})
