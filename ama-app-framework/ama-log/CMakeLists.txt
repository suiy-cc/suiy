cmake_minimum_required (VERSION 2.8.1)

######################### Project settings #####################################
project(ama-log)
include(OptionDefine)

############################## config inc&src ##################################
include_directories(
        inc/
        ${AMA_UTILITY_INCLUDE_PATH}
        ${AMA_MEM_INCLUDE_PATH}
        ${DLT_INCLUDE_DIRS})

file(GLOB SOURCES
        "src/*.cpp")

################################ Build library  ################################
set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR}/lib)

add_library(ama-log SHARED
        ${SOURCES})

add_dependencies(ama-log
                ama-mem)

link_directories(
        ${LINK_DIRECTORIES}
        ${CMAKE_CURRENT_BINARY_DIR}/lib)

target_link_libraries(ama-log
        ${DLT_LIBRARIES}
        ama-mem
        )

##################### Install library targets ###############################
install(TARGETS ama-log
        LIBRARY DESTINATION lib/)

install(FILES inc/log.h
        inc/CDltWrapper.h
        DESTINATION include/appSDK)

install(FILES libama-log.pc
        DESTINATION lib/pkgconfig)

install(FILES logDir.txt
        DESTINATION /etc/ama.d)
