# Copyright (C) 2015 Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

cmake_minimum_required(VERSION 2.8)
set(CMAKE_VERBOSE_MAKEFILE on)

##即package，与实际情况保持一致，源于fidl文件
set(PRJ_NAME APP_NAME)
##VERSION_PATH对应demo.fidl中的version { major * minor * }
set(VERSION_PATH v0_1)

##src-gen对应生成头文件命令的路径
set(FIDL_PATH_CORE core)
set(FIDL_PATH_DBUS dbus)
set(PRJ_SRC_GEN_COMMONAPI_PATH ${FIDL_PATH_CORE}/${VERSION_PATH}/${PRJ_NAME})
set(PRJ_SRC_GEN_COMMONAPI_DBUS_PATH ${FIDL_PATH_DBUS}/${VERSION_PATH}/${PRJ_NAME})

if (MSVC)
# Visual C++ is not always sure whether he is really C++
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_CRT_SECURE_NO_WARNINGS /wd\\\"4503\\\"")
else()
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -Wall -O0 -std=c++0x -D_GLIBCXX_USE_NANOSLEEP -DLINUX")
endif()

message(STATUS "Compiler options: ${CMAKE_CXX_FLAGS}") 

if(NOT CMAKE_BUILD_TYPE)
   set(CMAKE_BUILD_TYPE "Debug" CACHE STRING
       "Choose the type of build, options are: Debug Release." FORCE)
endif(NOT CMAKE_BUILD_TYPE)
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

OPTION(USE_INSTALLED_COMMONAPI "Set to OFF to use the local (build tree) version of CommonAPI" ON)
message(STATUS "USE_INSTALLED_COMMONAPI is set to value: ${USE_INSTALLED_COMMONAPI}")
set(CMAKE_PREFIX_PATH "/home/wangqi/workspace/GDP/genivi-demo-platform/gdp-src-build/tmp/sysroots/qemux86-64/usr/lib/cmake")
if ("${USE_INSTALLED_COMMONAPI}" STREQUAL "ON")
    FIND_PACKAGE(CommonAPI 3.1.3 REQUIRED CONFIG NO_CMAKE_PACKAGE_REGISTRY)
    FIND_PACKAGE(CommonAPI-DBus 3.1.3 REQUIRED CONFIG NO_CMAKE_PACKAGE_REGISTRY)
else()
    FIND_PACKAGE(CommonAPI 3.1.3 REQUIRED CONFIG NO_SYSTEM_ENVIRONMENT_PATH NO_CMAKE_SYSTEM_PATH)
    FIND_PACKAGE(CommonAPI-DBus 3.1.3 REQUIRED CONFIG NO_SYSTEM_ENVIRONMENT_PATH NO_CMAKE_SYSTEM_PATH)
endif()

message(STATUS "CommonAPI_CONSIDERED_CONFIGS: ${CommonAPI_CONSIDERED_CONFIGS}")
message(STATUS "COMMONAPI_INCLUDE_DIRS: ${COMMONAPI_INCLUDE_DIRS}")
message(STATUS "CommonAPI-DBus_CONSIDERED_CONFIGS: ${CommonAPI-DBus_CONSIDERED_CONFIGS}")
message(STATUS "COMMONAPI_DBUS_INCLUDE_DIRS: ${COMMONAPI_DBUS_INCLUDE_DIRS}")

# CommonAPI
include(FindPkgConfig)
###############################################################################
# find DBus by using the 'pkg-config' tool
if (MSVC)
	#Not beautiful, but it works
	if (DBus_DIR)
		if (DBus_BUILD_DIR)
			set(DBus_INCLUDE_DIRS "${DBus_DIR};${DBus_BUILD_DIR};")
		else ()
			message (FATAL_ERROR "DBus_BUILD_DIR not set! Cannot continue.")
		endif ()
	else()
		message (FATAL_ERROR "DBus_DIR not set! Cannot continue.")
	endif ()
else()
	pkg_check_modules(DBus REQUIRED dbus-1>=1.4)
endif()

# Source Files
# core&DBus library
FILE(GLOB PRJ_DBUS_LIB_SRCS ${PRJ_SRC_GEN_COMMONAPI_PATH}/*cpp ${PRJ_SRC_GEN_COMMONAPI_DBUS_PATH}/*cpp)

# Boost
#find_package( Boost 1.54 COMPONENTS system thread log REQUIRED )
#include_directories( ${Boost_INCLUDE_DIR} )

# Paths
OPTION(USE_INSTALLED_DBUS "Set to OFF to use the local (patched) version of dbus" ON)
message(STATUS "USE_INSTALLED_DBUS is set to value: ${USE_INSTALLED_DBUS}")

include_directories(
    ${FIDL_PATH_CORE}
    ${FIDL_PATH_DBUS}
    ${COMMONAPI_INCLUDE_DIRS}
    ${COMMONAPI_DBUS_INCLUDE_DIRS}
    ${DBus_INCLUDE_DIRS}
)

if ("${USE_INSTALLED_DBUS}" STREQUAL "ON")
link_directories(
    ${COMMONAPI_LIBDIR}
    ${COMMONAPI_DBUS_LIBDIR}
	${Boost_LIBRARY_DIR}
)
else()
link_directories(
    ${COMMONAPI_LIBDIR}
    ${COMMONAPI_DBUS_LIBDIR}
    ${DBus_INCLUDE_DIRS}/dbus/.libs
	${Boost_LIBRARY_DIR}
)
endif()

# Build DBus library
add_library (${PRJ_NAME}-dbus SHARED ${PRJ_DBUS_LIB_SRCS})
target_link_libraries(${PRJ_NAME}-dbus CommonAPI-DBus)
