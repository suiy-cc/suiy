cmake_minimum_required (VERSION 2.8.1)

######################### Project settings #####################################
project(ama-app-common)
include(OptionDefine)

############################## config inc&src ##################################
include_directories(
        inc/
        ${AMA_UTILITY_INCLUDE_PATH}
		${AMA_LOG_INCLUDE_PATH}
        ${AMA_MEM_INCLUDE_PATH}
        ${ILM_INCLUDE_DIRS}
		${LLC_INCLUDE_DIRS}
        ${PNG_INCLUDE_DIRS}
        ${GLIB2_INCLUDE_DIRS}
		)

file(GLOB SOURCES
        "src/*.cpp"
        "src/ILM/*.cpp"
        "src/COMMON/*.cpp"
        "src/GR/*.cpp"
		"src/LLC/*.cpp"
        "src/PIC/*.cpp"
        "src/TASK/*.cpp"
        "src/FILESYS/*.cpp"
	"src/MSG/*.c"
        )


################################ Build library  ################################
set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR}/lib)

add_library(ama-app-common SHARED
			${SOURCES})

add_dependencies(ama-app-common
		ama-log
        ama-mem
		)

link_directories(
	${LINK_DIRECTORIES}
    ${CMAKE_CURRENT_BINARY_DIR}/lib)

target_link_libraries(ama-app-common
			${ILM_LIBRARIES}
            ${PNG_LIBRARIES}
            ${LLC_LIBRARIES}
            ${ILMINPUT_LIBRARIES}
            ${GLIB2_LIBRARIES}
            ama-log
            ama-mem
            )

##################### Install library targets ###############################
# install library's .pc file
install(FILES 	libama-app-common.pc
		DESTINATION lib/pkgconfig)

# install library's binary file
install(TARGETS ama-app-common
				LIBRARY DESTINATION lib/)

# install library's header file
# common's headers
file(GLOB COMMON_HEADERS
        "inc/COMMON/*.hpp"
        "inc/COMMON/*.h"
        )
install(FILES ${COMMON_HEADERS}
        		DESTINATION include/ama-app-common/COMMON/)

# filesys' headers
file(GLOB FILESYS_HEADERS
        "inc/FILESYS/*.hpp"
        "inc/FILESYS/*.h"
        )
install(FILES ${FILESYS_HEADERS}
        DESTINATION include/ama-app-common/FILESYS/)

# gestrue recognizer's headers
file(GLOB GR_HEADERS
        "inc/GR/*.hpp"
        "inc/GR/*.h"
        )
install(FILES ${GR_HEADERS}
		DESTINATION include/ama-app-common/GR/)

# ILM's headers
file(GLOB ILM_HEADERS
        "inc/ILM/*.hpp"
        "inc/ILM/*.h"
        )
install(FILES ${ILM_HEADERS}
		DESTINATION include/ama-app-common/ILM/)

# LLC's headers
file(GLOB LLC_HEADERS
        "inc/LLC/*.hpp"
        "inc/LLC/*.h"
        )
install(FILES ${LLC_HEADERS}
		DESTINATION include/ama-app-common/LLC/)

# PIC's headers
file(GLOB PIC_HEADERS
        "inc/PIC/*.hpp"
        "inc/PIC/*.h"
        )
install(FILES ${PIC_HEADERS}
		DESTINATION include/ama-app-common/PIC/)

# TASK's headers
file(GLOB TASK_HEADERS
        "inc/TASK/*.hpp"
        "inc/TASK/*.h"
        )
install(FILES ${TASK_HEADERS}
        DESTINATION include/ama-app-common/TASK/)

# Message's headers
file(GLOB MSG_HEADERS
        "inc/MSG/*.h"
        )
install(FILES ${MSG_HEADERS}
        DESTINATION include/ama-app-common/MSG/)
