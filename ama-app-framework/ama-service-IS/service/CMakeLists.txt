cmake_minimum_required (VERSION 2.8.1)

######################### Project settings #####################################
project(ama-service-IS)
set(LICENSE "TBD")
set(IS_CAPI_INCLUDE_PATH "${ama-app-framework_SOURCE_DIR}/ama-service-IS/commonAPI/gen-src")

######## config inc&src&linklib settings and build #############################
include_directories(
    inc/
    ${CAPI_INCLUDE_DIRS}
    ${IS_CAPI_INCLUDE_PATH}
    ${AMA_LOG_INCLUDE_PATH}
    ${AMA_MEM_INCLUDE_PATH}
    ${AMA_APP_COMMON_INCLUDE_PATH}
    ${AMA_UTILITY_INCLUDE_PATH}
    ${AMA_APP_SDK_INCLUDE_PATH}
    ${AMA_APP_COMMON_INCLUDE_PATH}
    ${AUDIO_HUB_INCLUDE_DIRS}
)

file(GLOB SOURCES
    "src/*.cpp"
)

add_executable(ama-service-IS
               ${SOURCES})

add_dependencies(ama-service-IS
        ama-app-common
        ama-utility
        ama-log
        InformingSound-dbus
        appSDK)

link_directories(${LINK_DIRECTORIES}
                ${CMAKE_CURRENT_BINARY_DIR}/lib
                /lib/)

target_link_libraries(ama-service-IS
                        ama-app-common
                        ama-utility
                        ama-log
                        systemd
                        InformingSound-dbus
                        appSDK
                        ${AUDIO_HUB_LIBRARIES})



####################### Install targets ########################################
install(TARGETS ama-service-IS
    RUNTIME DESTINATION bin/
)

install(FILES   ama-service-IS.service
        DESTINATION /lib/systemd/system
)

install(FILES   res/FAW_Alert.wav
                res/FAW_prompt.wav
                res/FAW_radarAlert.wav
                res/FAW_radarFar.wav
                res/FAW_radarMid.wav
                res/FAW_radarNear.wav
                res/FAW_softkey.wav
                res/FAW_voice_feedback.wav
        DESTINATION /etc/ama.d
        )
