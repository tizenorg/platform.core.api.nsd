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

############################# Check minimum CMake version #####################

CMAKE_MINIMUM_REQUIRED(VERSION 2.8.3)
PROJECT(capi-network-nsd)

############################# cmake packages ##################################

INCLUDE(FindPkgConfig)

########################## search for packages ################################

SET(COMMON_DEPS "dlog glib-2.0 gio-2.0 capi-base-common gio-unix-2.0")
SET(PC_DEPS "capi-base-common")


PKG_CHECK_MODULES(NSD_DEPS REQUIRED ${COMMON_DEPS})

########################  directory configuration  ############################

SET(LIB_DIR
    "${CMAKE_INSTALL_PREFIX}/lib"
    CACHE PATH
    "Object code libraries directory")

SET(BIN_DIR
    "${CMAKE_INSTALL_PREFIX}/bin"
    CACHE PATH
    "User executables directory")

SET(SBIN_DIR
    "${CMAKE_INSTALL_PREFIX}/sbin"
    CACHE PATH
    "System admin executables directory")

SET(SYS_CONFIG_DIR
    "${CMAKE_INSTALL_PREFIX}/etc"
    CACHE PATH
    "Read-only single-machine data directory")

SET(INCLUDE_DIR
    "${CMAKE_INSTALL_PREFIX}/include"
    CACHE PATH
    "Header files directory")

SET(LOCAL_STATE_DIR
    "${CMAKE_INSTALL_PREFIX}/var"
    CACHE PATH
    "Modifiable single-machine data directory")

SET(DATA_ROOT_DIR
    "${CMAKE_INSTALL_PREFIX}/share"
    CACHE PATH
    "Read-only architecture-independent data root directory")

ADD_DEFINITIONS("-DLIB_DIR=\"${LIB_DIR}\"")
ADD_DEFINITIONS("-DBIN_DIR=\"${BIN_DIR}\"")
ADD_DEFINITIONS("-DSBIN_DIR=\"${SBIN_DIR}\"")
ADD_DEFINITIONS("-DSYS_CONFIG_DIR=\"${SYS_CONFIG_DIR}\"")
ADD_DEFINITIONS("-DINCLUDE_DIR=\"${INCLUDE_DIR}\"")
ADD_DEFINITIONS("-DLOCAL_STATE_DIR=\"${LOCAL_STATE_DIR}\"")
ADD_DEFINITIONS("-DDATA_ROOT_DIR=\"${DATA_ROOT_DIR}\"")

############################# compiler flags ##################################
FOREACH(flag ${NSD_DEPS_CFLAGS})
    SET(EXTRA_CFLAGS "${EXTRA_CFLAGS} ${flag}")
ENDFOREACH(flag)


SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${EXTRA_CFLAGS} -fPIC -Wall -ldns_sd")
SET(CMAKE_C_FLAGS_DEBUG      "-O0 -g")
SET(CMAKE_C_FLAGS_RELEASE    "-O2 -g")

# Don't export symbols by default
ADD_DEFINITIONS("-fvisibility=hidden")

SET(CMAKE_EXE_LINKER_FLAGS "-Wl,--as-needed -Wl,--rpath=${LIBDIR}")

SET(TARGET_NSD "nsd")
SET(TARGET_LIB_NSD_DNS_SD "nsd-dns-sd")
SET(TARGET_NSD_TESTS "nsd-tests")


ADD_SUBDIRECTORY(include)
