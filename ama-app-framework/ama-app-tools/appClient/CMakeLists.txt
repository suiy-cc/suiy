cmake_minimum_required (VERSION 2.8.1)

######################### Project settings #####################################
project(appClient)
set(LICENSE "TBD")

######## config inc&src&linklib settings and build #############################
include_directories(
        inc
        ${AMA_APP_COMMON_INCLUDE_PATH}
        ${CAPI_INCLUDE_DIRS}
        ${APPMGR_CAPI_INCLUDE_PATH}
        ${AMA_UTILITY_INCLUDE_PATH}
        ${AMA_LOG_INCLUDE_PATH}
        ${AMA_MEM_INCLUDE_PATH}
)

link_directories(
        ${LINK_DIRECTORIES}
        ${CMAKE_CURRENT_BINARY_DIR}/lib
)

file(GLOB SOURCES
        "src/*"
        )

add_executable(appClient
        ${SOURCES}
        )

add_dependencies(appClient
        AppMgr-dbus
        ama-log)

target_link_libraries(appClient
        AppMgr-dbus
        ama-log
        ama-utility
        ama-app-common
        )

######## Install targets ########
install(TARGETS appClient
        RUNTIME DESTINATION  local/bin/
        )
