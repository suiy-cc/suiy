#
# Find package config from target sdk
#

############### config c++ flags ###############################################
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -pthread -O0 -std=c++0x -D_GLIBCXX_USE_NANOSLEEP -DLINUX -DHAVE_STRCHRNUL")
message(STATUS "Compiler options: ${CMAKE_CXX_FLAGS}")

################ use FIND_PACKAGE to find lib ##################################
include( FindPkgConfig )

pkg_check_modules(DLT REQUIRED  automotive-dlt>=2.2.0)

pkg_check_modules(PCL REQUIRED  persistence_client_library)

pkg_check_modules(WESTON REQUIRED weston)

pkg_check_modules(ILM REQUIRED  ilmControl
                                ilm)

pkg_check_modules(ILMINPUT REQUIRED ilmInput)

pkg_check_modules(UI REQUIRED   cairo
                                wayland-client
                                wayland-cursor
                                pixman-1
                                libpng)

pkg_check_modules(PIXMAN REQUIRED pixman-1)

pkg_check_modules(RPC-IPC REQUIRED librpc-ipc)

pkg_check_modules(CAPI REQUIRED CommonAPI-DBus)

pkg_check_modules(LLC REQUIRED libLifecycle )

pkg_check_modules(PKGMGR REQUIRED  libpkgmgr)
pkg_check_modules(PMINFO REQUIRED  libpminfo)


pkg_check_modules(SYS_SDK REQUIRED AMACInterface)

pkg_check_modules(CAMERA REQUIRED  libcamera-client)

pkg_check_modules(PWRMGR REQUIRED  libpwrmgr)

pkg_check_modules(LSD REQUIRED  libShutdown)

pkg_check_modules(GNSS REQUIRED  gnss-service)

pkg_check_modules(AUDIO_HUB REQUIRED audiohub)

pkg_check_modules(WAYLAND_CLIENT REQUIRED wayland-client>=1.9.0)

pkg_check_modules(WAYLAND_SERVER REQUIRED wayland-server>=1.9.0)

pkg_check_modules(PNG REQUIRED libpng)

pkg_check_modules(DEVMGR REQUIRED device-manager)

pkg_check_modules(HWPROP REQUIRED hwprop)

pkg_check_modules(TINY_XML2 REQUIRED tinyxml2)

pkg_check_modules(IVC REQUIRED libivc_if-proxy)

pkg_check_modules(SQLITE3 REQUIRED sqlite3)

#for comservice project
pkg_check_modules(GLIB REQUIRED glib-2.0)
pkg_check_modules(UDEV REQUIRED udev)
