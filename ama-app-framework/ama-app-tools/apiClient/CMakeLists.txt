cmake_minimum_required (VERSION 2.8.1)

######################### Project settings #####################################
project(apiClient)
set(LICENSE "TBD")

######## config inc&src&linklib settings and build #############################
include_directories(
	inc
	${AMA_LOG_INCLUDE_PATH}
	${AMA_MEM_INCLUDE_PATH}
	${AMA_UTILITY_INCLUDE_PATH}
	${AMA_APP_COMMON_INCLUDE_PATH}
    ${ama-app-sdk_SOURCE_DIR}/inc/AppSDK/
    ${ama-app-sdk_SOURCE_DIR}/inc/HPSDK/
	${IS_SDK_INCLUDE_PATH}
	${APPDB_INCLUDE_PATH}
)

link_directories(
	${LINK_DIRECTORIES}
    ${CMAKE_CURRENT_BINARY_DIR}/lib
)

file(GLOB SOURCES
	"src/*"
)

add_executable(apiClient
		${SOURCES}
)

add_dependencies(apiClient
                ama-log
                appSDK
                ama-app-common
				appDB
				appISSDK
        )

target_link_libraries(apiClient
                        ama-log
                        appSDK
						ama-app-common
						appDB
						appISSDK
)

######## Install targets ########
install(TARGETS apiClient
	RUNTIME DESTINATION  local/bin/
)
