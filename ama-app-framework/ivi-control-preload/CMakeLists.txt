############################################################################
#
# Copyright (C) 2016 Neusoft
#
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#		http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
############################################################################

cmake_minimum_required (VERSION 2.6)

project(ivi-control-preload)
include(OptionDefine)

#find_package(PkgConfig REQUIRED)
#find_package(Threads REQUIRED)

include_directories(
        inc/
        protocol
        ${PIXMAN_INCLUDE_DIRS}
        ${WESTON_INCLUDE_DIRS}
        ${AMA_UTILITY_INCLUDE_PATH}
)

file(GLOB SOURCES
        "src/*.cpp"
        "src/*.c"
        )

add_library( ivi-control-preload MODULE
        ${SOURCES}
        )

add_dependencies(ivi-control-preload
        ama-utility)

target_link_libraries(ivi-control-preload
        ama-utility)

set_target_properties( ivi-control-preload PROPERTIES PREFIX "" )
set_target_properties( ivi-control-preload PROPERTIES OUTPUT_NAME "ivi-control-preload")

install (
        TARGETS             ivi-control-preload
        LIBRARY DESTINATION lib${LIB_SUFFIX}/weston
)
