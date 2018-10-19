cmake_minimum_required (VERSION 2.8.1)

######################### Project settings #####################################
project(ama-app-picture-display)
include(OptionDefine)

############################## config inc&src ##################################
include_directories(
        inc/
)

# lib source
file(GLOB SOURCES
        "src/*.cpp"
)

################################ Build library  ################################
set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR}/lib)

add_library(ama-app-picture-display SHARED
        ${SOURCES})

link_directories(
        ${LINK_DIRECTORIES}
        ${CMAKE_CURRENT_BINARY_DIR}/lib)

##################### Install library targets ###############################
install(TARGETS ama-app-picture-display
        LIBRARY DESTINATION lib/)

install(FILES inc/AppPictureDisplay.h
        DESTINATION include/appSDK)

install(FILES libama-app-picture-display.pc
        DESTINATION lib/pkgconfig)

##################### Install exe targets ###################################
install(PROGRAMS bin/ama-app-picture-display-demo
        DESTINATION local/bin/ama-app-picture-display/)

install(FILES
        bin/application.cfg
        bin/HS7VideoBg.kzb
        bin/HS7VideoBg.kzb.cfg
        DESTINATION local/bin/ama-app-picture-display)
