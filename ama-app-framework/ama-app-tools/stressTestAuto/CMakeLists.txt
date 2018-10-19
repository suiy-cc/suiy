cmake_minimum_required(VERSION 3.3)

######################### Project settings #####################################
project(stressTestAuto)

######## config inc&src&linklib settings and build #############################
include_directories(
    inc
	${AMA_LOG_INCLUDE_PATH}
	${AMA_MEM_INCLUDE_PATH}
	${AMA_UTILITY_INCLUDE_PATH}
	${AMA_APP_COMMON_INCLUDE_PATH}
	${ama-app-sdk_SOURCE_DIR}/inc/AppSDK/
	${ama-app-sdk_SOURCE_DIR}/inc/HPSDK/
	${AUDIO_HUB_INCLUDE_DIRS}
)

file(GLOB SOURCES
    "src/*.cpp"
)

add_executable(stressTestAuto
               ${SOURCES}
               )

add_dependencies(stressTestAuto
					ama-log
					appSDK
					ama-app-common
                )

target_link_libraries(stressTestAuto
						ama-log
						appSDK
						ama-app-common
					   ${AUDIO_HUB_LIBRARIES}
        )

####################### Install targets ########################################
install(TARGETS stressTestAuto
    RUNTIME DESTINATION  local/bin/
)
################################# end ##########################################
