cmake_minimum_required (VERSION 2.8.1)

######################### Project settings #####################################
PROJECT(ama-app-sdk)
include(OptionDefine)
SET(LICENSE "TBD")

message(STATUS "IS_SDK_INCLUDE_PATH: ${IS_SDK_INCLUDE_PATH}")

######## config inc#############################################################
include_directories(
    inc/
    inc/common/
    inc/AppSDK/
    inc/implementation/
    inc/HPSDK/
	inc/NASDK/
    ${ILM_INCLUDE_DIRS}
	${LSD_INCLUDE_DIRS}
    ${CAPI_INCLUDE_DIRS}
    ${AMA_UTILITY_INCLUDE_PATH}
    ${APPMGR_CAPI_INCLUDE_PATH}
    ${AMA_APP_COMMON_INCLUDE_PATH}
	${AMA_LOG_INCLUDE_PATH}
    ${AMA_MEM_INCLUDE_PATH}
    ${IS_SDK_INCLUDE_PATH}
)

##################### Build appSDK library  ####################################
set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR}/lib)

FILE(GLOB_RECURSE APPSDK_LIB_SRC
                    "src/AppSDK/*.cpp"
                    "src/implementation/*.cpp"
                    "src/HPSDK/*.cpp"
		            "src/NASDK/*.cpp"
)

add_library(appSDK SHARED
			${APPSDK_LIB_SRC}
        )

add_dependencies(appSDK
                AppMgr-dbus
                ama-utility
                ama-app-common
                ama-log
                appISSDK)



link_directories(
	${LINK_DIRECTORIES}
    ${CMAKE_CURRENT_BINARY_DIR}/lib
)

target_link_libraries(appSDK
                        AppMgr-dbus
                        ama-utility
                        ama-app-common
						${LSD_LIBRARIES}
                        ama-log
                        appISSDK
)

##################### Install appSDK targets ###################################

install(TARGETS appSDK
        LIBRARY DESTINATION lib/
)

install(FILES   inc/AppSDK/AppPLC.h
                inc/AppSDK/AppSDK.h
                inc/AppSDK/AppSM.h
		        inc/AppSDK/AppAudioStream.h
                inc/AppSDK/AudioStream.h
                inc/AppSDK/AppAudio.h
                inc/AppSDK/AppCS.h
                inc/AppSDK/AppUI.h
                inc/AppSDK/AppTools.h
                inc/AppSDK/AppIME.h
                inc/AppSDK/AppShare.h
                inc/AppSDK/AppSet.h
                inc/AppSDK/AppPower.h
                inc/AppSDK/AppBroadcast.h
	DESTINATION include/appSDK
)

install(FILES 	res/SpongeBob.png
        DESTINATION /home/
)

install(FILES 	libappSDK.pc
	DESTINATION lib/pkgconfig
)
