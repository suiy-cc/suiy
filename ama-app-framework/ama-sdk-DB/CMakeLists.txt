cmake_minimum_required (VERSION 2.8.1)

######################### Project settings #####################################
PROJECT(ama-sdk-DB)

######## config inc#############################################################
include_directories(
    inc/
    ${EASYSQLITE_INCLUDE_PATH}
    ${AMA_LOG_INCLUDE_PATH}
    ${AMA_MEM_INCLUDE_PATH}
    ${AMA_APP_COMMON_INCLUDE_PATH}
)

##################### Build appSDK library  ####################################
set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR}/lib)

FILE(GLOB_RECURSE AppDB_LIB_SRC
                    "src/*.cpp"
)

add_library(appDB SHARED
			${AppDB_LIB_SRC}
        )

add_dependencies(appDB
                ama-log
                easySQLite
                ama-app-common)

link_directories(
	${LINK_DIRECTORIES}
    ${CMAKE_CURRENT_BINARY_DIR}/lib
)

target_link_libraries(appDB
                        easySQLite
                        ama-log
                        ama-app-common
                        ${SQLITE3_LIBRARIES})

##################### Install appSDK targets ###################################

install(TARGETS appDB
        LIBRARY DESTINATION lib/
)

install(FILES   inc/AppDB.h
                inc/AppDBDef.h
	DESTINATION include/appDB
)


install(FILES 	libappDB.pc
	DESTINATION lib/pkgconfig
)
