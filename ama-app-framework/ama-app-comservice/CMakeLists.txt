cmake_minimum_required (VERSION 2.8.1)
include(OptionDefine)

######################### Project settings #####################################
project(ama-app-comservice)
include(DefineIncludePath)
set(LICENSE "TBD")

######## config inc&src&linklib settings and build #############################
include_directories(
    src/
    ${AMA_LOG_INCLUDE_PATH}
    ${AMA_MEM_INCLUDE_PATH}
    ${AMA_APP_COMMON_INCLUDE_PATH}
    ${AMA_UTILITY_INCLUDE_PATH}
    ${AMA_APP_SDK_INCLUDE_PATH}
    ${RPC-IPC_INCLUDE_DIRS}
    ${GLIB_INCLUDE_DIRS}
    ${UDEV_INCLUDE_DIRS}
)

file(GLOB SOURCES
    "src/*.cpp"
)

add_executable(ama-app-comservice
               ${SOURCES})

add_dependencies(ama-app-comservice
       appSDK)

link_directories(${LINK_DIRECTORIES}
                ${CMAKE_CURRENT_BINARY_DIR}/lib
                /lib/)

target_link_libraries(ama-app-comservice
                        ${IVC_LIBRARIES}
                        ${RPC-IPC_LIBRARIES}
                        ${GLIB_LIBRARIES}
                        ${UDEV_LIBRARIES}
                        ama-log
                        systemd
                        AppMgr-dbus
                        appSDK
                        udev)



####################### Install targets ########################################
install(TARGETS ama-app-comservice
    RUNTIME DESTINATION bin/
)

install(FILES   ama-app-comservice.service
        DESTINATION /lib/systemd/system
)
