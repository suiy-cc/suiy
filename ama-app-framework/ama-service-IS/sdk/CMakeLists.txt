cmake_minimum_required (VERSION 2.8.1)

######################### Project settings #####################################
PROJECT(ama-app-is-sdk)
set(IS_CAPI_INCLUDE_PATH "${ama-app-framework_SOURCE_DIR}/ama-service-IS/commonAPI/gen-src")

######## config inc#############################################################
include_directories(
    inc/
    ${CAPI_INCLUDE_DIRS}
    ${AMA_UTILITY_INCLUDE_PATH}
    ${IS_CAPI_INCLUDE_PATH}
    ${AMA_LOG_INCLUDE_PATH}
    ${AMA_MEM_INCLUDE_PATH}
)

##################### Build appSDK library  ####################################
set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR}/lib)

FILE(GLOB_RECURSE appISSDK_LIB_SRC
                    "src/*.cpp"
)

add_library(appISSDK SHARED
			${appISSDK_LIB_SRC}
        )

add_dependencies(appISSDK
                InformingSound-dbus
                ama-log)

link_directories(
	${LINK_DIRECTORIES}
    ${CMAKE_CURRENT_BINARY_DIR}/lib
)

target_link_libraries(appISSDK
                        InformingSound-dbus
                        ama-log)

##################### Install appSDK targets ###################################

install(TARGETS appISSDK
        LIBRARY DESTINATION lib/
)

install(FILES   inc/AppIS.h
	DESTINATION include/appSDK
)


install(FILES 	libappISSDK.pc
	DESTINATION lib/pkgconfig
)
