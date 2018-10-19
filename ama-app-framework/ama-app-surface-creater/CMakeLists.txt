# Copyright (C) 2015-2017 Neusoft changchun
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

cmake_minimum_required (VERSION 2.8.1)

project(ama-app-surface-creater)
include(OptionDefine)

##################### include cmake of project #################################

include_directories(
        inc/
        ${WESTON_INCLUDE_DIRS}
        ${WAYLAND_CLIENT_INCLUDE_DIRS}
        ${WAYLAND_SERVER_INCLUDE_DIRS}
        ${AMA_LOG_INCLUDE_PATH}
        ${AMA_APP_COMMON_INCLUDE_PATH}
        ${AMA_MEM_INCLUDE_PATH}
)

file(GLOB SOURCES
        "src/*.cpp"
        "src/*.c"
        )

set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR}/lib)

add_library( ama-app-surface-creater SHARED
        ${SOURCES}
        )

add_dependencies(ama-app-surface-creater
        ama-app-common
        ama-mem
        )

target_link_libraries(ama-app-surface-creater
        ${WESTON_LIBRARIES}
        ${WAYLAND_CLIENT_LIBRARIES}
        ${WAYLAND_SERVER_LIBRARIES}
        ama-app-common
        ama-mem
        )

##################### Install appSDK targets ###################################
install(TARGETS ama-app-surface-creater
        LIBRARY DESTINATION lib/
        )

install(FILES   inc/SurfaceCreater.h
	DESTINATION include/appSDK
)

install(FILES 	libsurfaceCreater.pc
	DESTINATION lib/pkgconfig
)
