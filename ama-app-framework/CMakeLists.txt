# Copyright (C) 2015-2018 Neusoft changchun
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

cmake_minimum_required (VERSION 2.8.1)

project(ama-app-framework)

##################### include cmake of project #################################
#SET(CMAKE_VERBOSE_MAKEFILE on)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${ama-app-framework_SOURCE_DIR}/cmake)
include(FindPackage)
include(DefineIncludePath)
include(OptionDefine)

if(WITH_DEBUG_MEMORY)
    add_definitions(-D_DEBUG_MEMORY_)
    pkg_check_modules(GLIB2 REQUIRED glib-2.0)
    message(STATUS "OPT WITH_DEBUG_MEMORY=${WITH_DEBUG_MEMORY}")
endif()

if(WITH_IAS_BACKEND)
    add_definitions(-DWITH_IAS_BACKEND)
    message(STATUS "OPT WITH_IAS_BACKEND=${WITH_IAS_BACKEND}")
endif()

add_subdirectory(ama-mem)
add_subdirectory(ama-log)
add_subdirectory(ama-utility) # framework's interal shared libraries
add_subdirectory(ama-app-common) # public useful shared libraries
add_subdirectory(ama-app-manager)
add_subdirectory(ama-app-sdk)
add_subdirectory(appMgrCAPI)
add_subdirectory(ama-app-tools/apiClient)
add_subdirectory(ama-app-tools/appClient)
add_subdirectory(ama-app-tools/stressTestAuto)
add_subdirectory(ama-app-tools/surfaceWatcher)
add_subdirectory(ama-app-tools/autoTestTool)
add_subdirectory(ama-app-picture-display)
add_subdirectory(ama-app-surface-creater)
add_subdirectory(ama-app-double-click-scanner)
#add_subdirectory(ama-app-comservice)
add_subdirectory(ama-service-IS)
add_subdirectory(easySQLitePro)
add_subdirectory(ama-sdk-DB)
if(WITH_HV_MENTOR)
    message(STATUS "add_subdirectory ivi-control-preload-winston")
    add_subdirectory(ivi-control-preload-winston)
else()
    message(STATUS "add_subdirectory ivi-control-preload")
    add_subdirectory(ivi-control-preload)
endif(WITH_HV_MENTOR)

##################### config CPack #############################################
set(CPACK_GENERATOR "RPM")

set(CPACK_RPM_PACKAGE_DESCRIPTION_SUMMARY "ama app framework sdk library")
set(CPACK_RPM_PACKAGE_VENDOR "neusoft changchun")
set(CPACK_RPM_PACKAGE_NAME "appSDK")
set(CPACK_RPM_PACKAGE_VERSION_MAJOR "0")
set(CPACK_RPM_PACKAGE_VERSION_MINOR "55")
set(CPACK_RPM_PACKAGE_VERSION_PATCH "1")

include(CPack)
