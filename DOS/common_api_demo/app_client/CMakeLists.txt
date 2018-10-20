# Copyright (C) 2015 Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

cmake_minimum_required(VERSION 2.8)
set(CMAKE_VERBOSE_MAKEFILE on)

set(PRJ_NAME APP_NAME)
set(LINK_LIBRARIES -Wl,--as-needed CommonAPI /usr/local/lib/lib${PRJ_NAME}-dbus.so)

set(PRJ_SRC_PATH src)
set(PRJ_NAME_CLIENT ${PRJ_NAME}Client)

##头文件命令的路径
set(INCLUDE_PATH include)

if (MSVC)
# Visual C++ is not always sure whether he is really C++
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D_CRT_SECURE_NO_WARNINGS /EHsc /wd\\\"4503\\\"")
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
set(PRJ_CLIENT_SRCS ${PRJ_SRC_PATH}/${PRJ_NAME_CLIENT}.cpp)

# Boost
#find_package( Boost 1.54 COMPONENTS system thread log REQUIRED )
#include_directories( ${Boost_INCLUDE_DIR} )

# Paths
OPTION(USE_INSTALLED_DBUS "Set to OFF to use the local (patched) version of dbus" ON)
message(STATUS "USE_INSTALLED_DBUS is set to value: ${USE_INSTALLED_DBUS}")

include_directories(
    ${INCLUDE_PATH}
    ${COMMONAPI_INCLUDE_DIRS}
    ${COMMONAPI_DBUS_INCLUDE_DIRS}
    ${DBus_INCLUDE_DIRS}
)

if ("${USE_INSTALLED_DBUS}" STREQUAL "ON")
link_directories(
	${Boost_LIBRARY_DIR}
)
else()
link_directories(
    ${DBus_INCLUDE_DIRS}/dbus/.libs
	${Boost_LIBRARY_DIR}
)
endif()

# Build Client
add_executable(${PRJ_NAME_CLIENT} ${PRJ_CLIENT_SRCS})
target_link_libraries(${PRJ_NAME_CLIENT} ${LINK_LIBRARIES})
