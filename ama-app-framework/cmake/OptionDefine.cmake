#
# Find package config from target sdk
#

####################Mentor define############################
option(WITH_HV_MENTOR "if==ON compile code for mentor hypervisor" OFF)

if(WITH_HV_MENTOR)
    message(STATUS "add define CREATE_LAYERS_BY_AMGR")
    add_definitions(-DCREATE_LAYERS_BY_AMGR)
endif(WITH_HV_MENTOR)
#############################################################

option(IS_GENERATE_CPP_SOURCE_FILE_BY_FIDL "if==OFF amgr will not update cpp files from fidl" ON)

####################EV Define#############################
option(WITH_MODULE_EV "if==ON compile code for EV image" OFF)

if(WITH_MODULE_EV)
    message(STATUS "add define CODE_FOR_EV")
    add_definitions(-DCODE_FOR_EV)
endif(WITH_MODULE_EV)
##########################################################

####################AIO define###############################
option(WITH_ROOTFS_AIO "if==ON compile code for AIO" OFF)

if(WITH_ROOTFS_AIO)
    message(STATUS "add define CODE_FOR_AIO")
    add_definitions(-DCODE_FOR_AIO)
endif(WITH_ROOTFS_AIO)
#############################################################

####################GAIA Define#############################
option(WITH_HV_GAIA "if==ON compile code for Gaia image" OFF)

if(WITH_HV_GAIA)
    message(STATUS "add define CODE_FOR_GAIA")
    add_definitions(-DCODE_FOR_GAIA)
endif(WITH_HV_GAIA)
##########################################################

####################debug define##########################
option(IS_DEBUG_MODE "if==ON will print more log" ON)

if(IS_DEBUG_MODE)
    message(STATUS "enable debug mode")
    add_definitions(-DENABLE_DEBUG_MODE)
endif(IS_DEBUG_MODE)
##########################################################

####################HS5 define##########################
option(WITH_MODULE_HS5 "if==ON compile code for HS5" OFF)

if(WITH_MODULE_HS5)
    message(STATUS "compile code for HS5")
    add_definitions(-DCODE_FOR_HS5)
endif(WITH_MODULE_HS5)
##########################################################

####################HS7 define##########################
option(WITH_MODULE_HS7 "if==ON compile code for HS7" OFF)

if( (NOT WITH_MODULE_HS5) AND (NOT WITH_MODULE_EV) )
    message(STATUS "compile code for HS7")
    add_definitions(-DCODE_FOR_HS7)
endif()
##########################################################