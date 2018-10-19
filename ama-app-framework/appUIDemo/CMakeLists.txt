cmake_minimum_required (VERSION 2.8.1)

######################### Project settings #####################################
SET(PROJECT_NAME AppUIDemo)
PROJECT(${PROJECT_NAME})
SET(LICENSE "TBD")

############### config c++ flags ###############################################
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -O0 -g -DHAVE_STRCHRNUL")
SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O0 -g -DHAVE_STRCHRNUL")

################ use 'pkg-config' tool to find lib #############################
include( FindPkgConfig )

pkg_check_modules(ILM REQUIRED ilmControl
                                ilmInput
                                ilm)

pkg_check_modules(UI REQUIRED
                            cairo
                            wayland-client
                            wayland-cursor
                            pixman-1
                            libpng)

pkg_check_modules(APPSDK REQUIRED
 							libappSDK
							)

pkg_check_modules(LOG REQUIRED
		libama-log
		)

######## config inc&src&linklib settings and build #############################
include_directories(
    ${ILM_INCLUDE_DIRS}
    ${UI_INCLUDE_DIRS}
	${APPSDK_INCLUDE_DIRS}
	${LOG_INCLUDE_DIRS}
	inc
    src
    uiLib
)

file(GLOB SOURCES
	"src/*cpp"
    "uiLib/*c"
    "uiLib/*cpp"
)

link_directories(
	${LINK_DIRECTORIES}
)

add_executable(${PROJECT_NAME}
	   ${SOURCES}
)

TARGET_LINK_LIBRARIES(${PROJECT_NAME}
        	${ILM_LIBRARIES}
            ${UI_LIBRARIES}
            ${APPSDK_LIBRARIES}
			${LOG_LIBRARIES}
            jpeg
            pthread
            AppMgr-dbus
)

####################### Install targets ########################################
INSTALL(TARGETS ${PROJECT_NAME}
	RUNTIME DESTINATION local/bin
)

install(FILES 	drawable/bt.png
 				drawable/home.png
                drawable/media.png
                drawable/notify.png
                drawable/radio.png
                drawable/setting.png
                drawable/tbox.png
                drawable/vr.png
	DESTINATION share/ilm/testILM/res
)

install(FILES 	scripts/startBTApp.sh
 				scripts/startHomeApp.sh
                scripts/startMediaApp.sh
                scripts/startNotifyApp.sh
                scripts/startSettingApp.sh
                scripts/startRadioApp.sh
                scripts/startTboxApp.sh
                scripts/startVRApp.sh
	DESTINATION local/bin
)

install(FILES 	scripts/startWeston.sh
	DESTINATION bin
)

####################### pack targets ###########################################
set(CPACK_GENERATOR "RPM")

set(CPACK_RPM_PACKAGE_DESCRIPTION_SUMMARY "app demo")
set(CPACK_RPM_PACKAGE_VENDOR "neusoft changchun")
set(CPACK_RPM_PACKAGE_NAME "appUIDemo")
set(CPACK_RPM_PACKAGE_VERSION_MAJOR "0")
set(CPACK_RPM_PACKAGE_VERSION_MINOR "1")
set(CPACK_RPM_PACKAGE_VERSION_PATCH "0")

include(CPack)
