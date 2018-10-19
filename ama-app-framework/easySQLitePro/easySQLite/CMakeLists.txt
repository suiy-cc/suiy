cmake_minimum_required (VERSION 2.8.1)

######################### Project settings #####################################
PROJECT(easySQLite)

######## config inc#############################################################
include_directories(
    ./
    ${SQLITE3_INCLUDE_DIRS}
)

##################### Build appSDK library  ####################################
set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR}/lib)

FILE(GLOB_RECURSE easySQLite_LIB_SRC
                    "*.cpp"
)

add_library(easySQLite SHARED
			${easySQLite_LIB_SRC}
        )


link_directories(
	${LINK_DIRECTORIES}
    ${CMAKE_CURRENT_BINARY_DIR}/lib
)

target_link_libraries(easySQLite
                        ${SQLITE3_LIBRARIES})

##################### Install appSDK targets ###################################

install(TARGETS easySQLite
        LIBRARY DESTINATION lib/
)

install(FILES   SHA1.h
                SqlCommon.h
                SqlDatabase.h
                SqlField.h
                SqlFieldSet.h
                SqlRecord.h
                SqlTable.h
                SqlValue.h
	DESTINATION include/easySQLite
)


install(FILES 	libeasySQLite.pc
	DESTINATION lib/pkgconfig
)
