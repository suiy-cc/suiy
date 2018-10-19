# Copyright (C) 2015 Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

cmake_minimum_required(VERSION 2.8)
include(OptionDefine)

#####general variable
set(FIRST_PACKAGE_NAME org)
set(SECOND_PACKAGE_NAME neusoft)
set(VERSION_PATH v0_1)

######################### Project  START #####################################
set(PRJ AppMgr)
set(PRJ_IME ImeIf)

set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(COMMONAPI_SRC_PATH ${SRC_DIR}/${VERSION_PATH}/${FIRST_PACKAGE_NAME}/${SECOND_PACKAGE_NAME})

SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/../cmake)

INCLUDE(CommonAPIGenerator)

if(IS_GENERATE_CPP_SOURCE_FILE_BY_FIDL)
COMMON_API_GENERATE_SOUCRES(TARGET COMMON_API_APP_MGR
                    FIDLS fidl/AppMgr.fidl
                    DESTINATION ${SRC_DIR}
)
COMMON_API_GENERATE_SOUCRES(TARGET COMMON_API_APP_IME
                    FIDLS fidl/ime.fidl
                    DESTINATION ${SRC_DIR}
)
endif(IS_GENERATE_CPP_SOURCE_FILE_BY_FIDL)

FILE(GLOB PRJ_SRCS
    ${COMMONAPI_SRC_PATH}/*cpp
)
######################### Project  FINISHED #####################################

include_directories(
    ${SRC_DIR}
    ${COMMONAPI_SRC_PATH}
    ${CAPI_INCLUDE_DIRS}
)

set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR}/lib)


add_library (${PRJ}-dbus SHARED
            ${PRJ_SRCS}
)

target_link_libraries(${PRJ}-dbus
		${CAPI_LIBRARIES}
)

######## Install targets ########
install(TARGETS ${PRJ}-dbus
	LIBRARY DESTINATION lib/
)

install(FILES 	${COMMONAPI_SRC_PATH}/${PRJ}.hpp
                ${COMMONAPI_SRC_PATH}/${PRJ}Proxy.hpp
                ${COMMONAPI_SRC_PATH}/${PRJ}ProxyBase.hpp
                ${COMMONAPI_SRC_PATH}/${PRJ}Stub.hpp
                ${COMMONAPI_SRC_PATH}/${PRJ}StubDefault.hpp
	#DESTINATION include/appSDK/${VERSION_PATH}/${FIRST_PACKAGE_NAME}/${SECOND_PACKAGE_NAME}

                ${COMMONAPI_SRC_PATH}/${PRJ_IME}.hpp
                ${COMMONAPI_SRC_PATH}/${PRJ_IME}Proxy.hpp
                ${COMMONAPI_SRC_PATH}/${PRJ_IME}ProxyBase.hpp
                ${COMMONAPI_SRC_PATH}/${PRJ_IME}Stub.hpp
                ${COMMONAPI_SRC_PATH}/${PRJ_IME}StubDefault.hpp
    DESTINATION include/appSDK/${VERSION_PATH}/${FIRST_PACKAGE_NAME}/${SECOND_PACKAGE_NAME}
)
