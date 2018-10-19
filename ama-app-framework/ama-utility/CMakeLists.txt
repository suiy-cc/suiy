cmake_minimum_required (VERSION 2.8.1)

######################### Project settings #####################################
project(ama-utility)
include(OptionDefine)

############################## config inc&src ##################################
include_directories(
		${AMA_LOG_INCLUDE_PATH}
        ${AMA_MEM_INCLUDE_PATH}
        ${AMA_UTILITY_INCLUDE_PATH}
        ${AMA_APP_COMMON_INCLUDE_PATH}
        ${DLT_INCLUDE_DIRS}
        ${ILM_INCLUDE_DIRS}
        ${HWPROP_INCLUDE_DIRS}
		)

file(GLOB SOURCES
        "src/*.cpp"
		"src/COMMON/*.cpp"
        "src/USB/*.cpp"
		"src/SURFACE/*.cpp"
        "src/SURFACE/*.c"
        )

file(GLOB RPC-SRC
				"src/RPC-Wrapper/*.cpp"
)


################################ Build library  ################################
set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR}/lib)

add_library(ama-utility SHARED
			${SOURCES})

add_dependencies(ama-utility
        ama-app-common
        )

add_library(rpc-wrapper
				SHARED
			  	${RPC-SRC})

link_directories(
	${LINK_DIRECTORIES}
    ${CMAKE_CURRENT_BINARY_DIR}/lib)

target_link_libraries(ama-utility
			${ILM_LIBRARIES}
            ${AMA_LOG_LIBRARIES}
            ${PNG_LIBRARIES}
            ${HWPROP_LIBRARIES}
            ${TINY_XML2_LIBRARIES}
            ama-app-common
            )

target_link_libraries(rpc-wrapper
						${DLT_LIBRARIES}
						${RPC-IPC_LIBRARIES}
                        ${LLC_LIBRARIES}
        )

##################### Install library targets ###############################
install(TARGETS ama-utility
				LIBRARY DESTINATION lib/)

install(FILES inc/ama_types.h
                inc/ama_enum.h
                inc/ama_audioTypes.h
                inc/ama_setTypes.h
                inc/ama_stateTypes.h
				inc/ama_powerTypes.h
                DESTINATION include/appSDK)

install(TARGETS rpc-wrapper
				LIBRARY DESTINATION lib/)

install(FILES 	inc/RPC-Wrapper/rpc_proxy.h
				inc/RPC-Wrapper/rpc_proxy_def.h
				DESTINATION include/appSDK)
