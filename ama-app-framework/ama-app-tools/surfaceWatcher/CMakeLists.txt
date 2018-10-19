cmake_minimum_required (VERSION 2.8.1)

######################### Project settings #####################################
project(surfaceWatcher)
set(LICENSE "TBD")

######## config inc&src&linklib settings and build #############################
include_directories(
        inc
        ${AMA_LOG_INCLUDE_PATH}
        ${AMA_MEM_INCLUDE_PATH}
        ${ILM_INCLUDE_DIRS}
)

link_directories(
        ${LINK_DIRECTORIES}
        ${CMAKE_CURRENT_BINARY_DIR}/lib
)

file(GLOB SOURCES
        "src/*"
        )

add_executable(surfaceWatcher
        ${SOURCES}
        )

add_dependencies(surfaceWatcher
        ama-log
        )

target_link_libraries(surfaceWatcher
        ama-log
        ${ILM_LIBRARIES}
        )

######## Install targets ########
install(TARGETS surfaceWatcher
        RUNTIME DESTINATION  local/bin/
        )
