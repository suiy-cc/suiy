cmake_minimum_required (VERSION 2.8.1)

######################### Project settings #####################################
project(ama-mem)
include(OptionDefine)

############################## config inc&src ##################################
include_directories(
        inc/
        ${AMA_UTILITY_INCLUDE_PATH}
        ${DLT_INCLUDE_DIRS}
        ${GLIB2_INCLUDE_DIRS}
        )


file(GLOB SOURCES
        "src/*.cpp")

################################ Build library  ################################
set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR}/lib)

add_library(ama-mem SHARED
        ${SOURCES})

link_directories(
        ${LINK_DIRECTORIES}
        ${CMAKE_CURRENT_BINARY_DIR}/lib)

target_link_libraries(ama-mem
        ${DLT_LIBRARIES}
        ${GLIB2_LIBRARIES}
        )

################################ Build test  ################################
add_executable(ama-test-mem-summary
                src/test.cpp)

add_dependencies(ama-test-mem-summary
                ama-mem)

target_link_libraries(ama-test-mem-summary
                ama-mem)

##################### Install library targets ###############################
install(TARGETS ama-mem
        LIBRARY DESTINATION lib/)

install(FILES
        inc/ama_mem.h
        DESTINATION include/appSDK)

install(FILES libama-mem.pc
        DESTINATION lib/pkgconfig)

install(TARGETS ama-test-mem-summary
        RUNTIME DESTINATION  local/bin/
)
