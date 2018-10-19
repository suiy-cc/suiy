cmake_minimum_required (VERSION 2.8.1)

######################### Project settings #####################################
project(autoTestTool)
set(LICENSE "TBD")

######## config inc&src&linklib settings and build #############################
include_directories(
        inc
        ${EASYSQLITE_INCLUDE_PATH}
        ${AMA_LOG_INCLUDE_PATH}
        ${AMA_MEM_INCLUDE_PATH}
        ${AMA_APP_COMMON_INCLUDE_PATH}
        ${ama-app-sdk_SOURCE_DIR}/inc/AppSDK/
        ${AMA_UTILITY_INCLUDE_PATH}
)

link_directories(
        ${LINK_DIRECTORIES}
        ${CMAKE_CURRENT_BINARY_DIR}/lib
)

file(GLOB SOURCES
        "src/*"
        )

add_executable(autoTestTool
        ${SOURCES}
        )

add_dependencies(autoTestTool
        ama-log
        easySQLite
        appSDK
        )

target_link_libraries(autoTestTool
        ama-log
        ama-app-common
        appSDK
        easySQLite
        ${SQLITE3_LIBRARIES}
        )

######## Install targets ########
install(TARGETS autoTestTool
        RUNTIME DESTINATION  local/bin/
        )
