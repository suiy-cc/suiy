cmake_minimum_required (VERSION 2.8.1)
include(OptionDefine)

######################### Project settings #####################################
project(ama-app-manager)
set(LICENSE "TBD")

######## config inc&src&linklib settings and build #############################
include_directories(
    inc
    inc/CAPI
    inc/PLC
    inc/HMIC
    inc/SM
    inc/CS
    inc/COMMON
    inc/AC
    inc/PERS
    inc/STH
    inc/PMC
    inc/BPM
    inc/LCD

    ${ILM_INCLUDE_DIRS}
    ${PCL_INCLUDE_DIRS}
    ${LLC_INCLUDE_DIRS}
    ${PWRMGR_INCLUDE_DIRS}
    ${PKGMGR_INCLUDE_DIRS}
    ${PMINFO_INCLUDE_DIRS}
    ${DEVMGR_INCLUDE_DIRS}
    ${CAPI_INCLUDE_DIRS}
    ${SYS_SDK_INCLUDE_DIRS}
    ${LSD_INCLUDE_DIRS}
    ${AUDIO_HUB_INCLUDE_DIRS}
    ${CAMERA_INCLUDE_DIRS}
    ${GNSS_INCLUDE_DIRS}
    ${DEVMGR_INCLUDE_DIRS}
    ${APPMGR_CAPI_INCLUDE_PATH}
    ${AMA_UTILITY_INCLUDE_PATH}
    ${AMA_LOG_INCLUDE_PATH}
    ${AMA_MEM_INCLUDE_PATH}
    ${AMA_APP_COMMON_INCLUDE_PATH}
    ${AMA_APP_SURFACE_CREATER_INCLUDE_PATH}
    ${IVC_INCLUDE_PATH}
    ${APPDB_INCLUDE_PATH}
)

file(GLOB SOURCES
    "src/*.cpp"
    "src/CAPI/*.cpp"
    "src/PLC/*.cpp"
    "src/HMIC/*.cpp"
    "src/SM/*.cpp"
    "src/CS/*.cpp"
    "src/COMMON/*.cpp"
    "src/AC/*.cpp"
    "src/PERS/*.cpp"
    "src/STH/*.cpp"
    "src/PMC/*.cpp"
    "src/BPM/*.cpp"  
    "src/LCD/*.cpp"
)

add_executable(ama-app-manager
               ${SOURCES})

add_dependencies(ama-app-manager
                ama-app-common
                ama-utility
                rpc-wrapper
                ama-log
                ama-mem
                ama-app-surface-creater
                appDB
        )

link_directories(${LINK_DIRECTORIES}
                ${CMAKE_CURRENT_BINARY_DIR}/lib)

target_link_libraries(ama-app-manager
                       AppMgr-dbus
                       ama-utility
                       ${PCL_LIBRARIES}
                       ${LLC_LIBRARIES}
                       ${LSD_LIBRARIES}
                       ${PWRMGR_LIBRARIES}
                       ${PKGMGR_LIBRARIES}
                       ${PMINFO_LIBRARIES}
                       ${DEVMGR_LIBRARIES}
                       ${CAMERA_LIBRARIES}
                       ${SYS_SDK_LIBRARIES}
                       ${AUDIO_HUB_LIBRARIES}
                       ${TINY_XML2_LIBRARIES}
                       ${IVC_LIBRARIES}
                       rpc-wrapper
                       systemd
                       ama-log
                       pers_admin_access_lib
                       ${GNSS_LIBRARIES}
                       ama-app-surface-creater
                       appDB
        )

####################### Install targets ########################################
install(TARGETS ama-app-manager
    RUNTIME DESTINATION bin/
)

install(FILES   ama-app-manager.service
        DESTINATION /lib/systemd/system
)

install(FILES   pers_config.xml
        DESTINATION /etc/ama.d
        )

install(FILES   res/RSELock_C.png
                res/RSELock_E.png
        DESTINATION /etc/ama.d
        )

install(FILES   BootSequence.txt
        DESTINATION /etc/ama.d
        )

install(FILES   AlwaysOnApp.txt
        DESTINATION /etc/ama.d
        )
install(FILES   AlwaysVisibleServices.txt
        DESTINATION /etc/ama.d
        )

install(FILES   AppID.txt
        DESTINATION /etc/ama.d
        )

install(FILES   LayerID.txt
        DESTINATION /etc/ama.d
        )


install(FILES   LogID.txt
            DESTINATION /etc/ama.d
            )

install(FILES   dlt_logstorage.conf
        DESTINATION /etc/ama.d
        )

if(WITH_MODULE_EV) # EV
    message(STATUS "FastBootList=FastBootListForEV")
    install(FILES FastBootListForEV
            DESTINATION /etc/ama.d
            RENAME FastBootList
            )
    message(STATUS "frameworkConfig.xml=frameworkConfigForEV.xml")
    install(FILES   frameworkConfigForEV.xml
            DESTINATION /etc/ama.d
            RENAME frameworkConfig.xml
            )
    install(FILES   defLastSource
            DESTINATION /etc/ama.d
            )
    install(FILES   res/LoadingScreen_C.png
            res/LoadingScreen_E.png
            DESTINATION /etc/ama.d
            )
elseif(WITH_MODULE_HS5) # HS5
    message(STATUS "FastBootList=FastBootListForHS5")
    install(FILES FastBootListForHS5
            DESTINATION /etc/ama.d
            RENAME FastBootList
            )
    message(STATUS "frameworkConfig.xml=frameworkConfigForEV.xml")
    install(FILES   frameworkConfigForEV.xml
            DESTINATION /etc/ama.d
            RENAME frameworkConfig.xml
            )
    message(STATUS "defLastSource=defLastSourceForHS5")
    install(FILES   defLastSourceForHS5
            DESTINATION /etc/ama.d
            RENAME defLastSource
            )
    message(STATUS "LoadingScreen_C.png=LoadingScreen_C-HS5.png")
    install(FILES   res/LoadingScreen_C-HS5.png
            DESTINATION /etc/ama.d
            RENAME LoadingScreen_C.png
            )
    message(STATUS "LoadingScreen_E.png=LoadingScreen_E-HS5.png")
    install(FILES   res/LoadingScreen_E-HS5.png
            DESTINATION /etc/ama.d
            RENAME LoadingScreen_E.png
            )
else() # HS7
    install(FILES FastBootList
            DESTINATION /etc/ama.d
            )
    if(WITH_ROOTFS_AIO) # AIO
        message(STATUS "frameworkConfig.xml=frameworkConfigForHS7AIO.xml")
        install(FILES   frameworkConfigForHS7AIO.xml
                DESTINATION /etc/ama.d
                RENAME frameworkConfig.xml
                )
    else() # AIO
        install(FILES   frameworkConfig.xml
                DESTINATION /etc/ama.d
                )
    endif() # AIO
    install(FILES   defLastSource
            DESTINATION /etc/ama.d
            )
    install(FILES   res/LoadingScreen_C.png
            res/LoadingScreen_E.png
            DESTINATION /etc/ama.d
            )
endif(WITH_MODULE_EV)

if(NOT WITH_HV_MENTOR)
install(FILES   res/99-seats.rules
        DESTINATION /etc/udev/rules.d/
        )
endif(NOT WITH_HV_MENTOR)
