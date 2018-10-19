#
# Find common-api sources
#
# This module defines these variables:
#
#  ${PARAMS_TARGET}_GEN_HEADERS
#      A list with generated headers
#  ${PARAMS_TARGET}_GEN_SOURCES
#      A list with generated sources
#  ${PARAMS_TARGET}_GEN_INCLUDE_DIR
#     A list with include directories

include(CMakeParseArguments)

# generate common-api sources and retreive a list with them
macro(GENERATE_FILES)

	message(STATUS "GENERATE_FILES-->>...")
	# searching for common-api-generator executable ...
	message(STATUS "searching for common-api-generator executable ...")

	find_program(COMMONAPI_GENERATOR_EXE
				  NAMES
					  "commonapi-generator-linux-x86_64"
				  PATHS
					  $ENV{PATH}
					  ${CMAKE_CURRENT_SOURCE_DIR}/../commonapi-generator
					  )


	if(COMMONAPI_GENERATOR_EXE)
		message(STATUS "find common-api generator")

		# execute the generate command ...
		message(STATUS "execute the commonapi generate command ...")
		message(STATUS "command:${COMMONAPI_GENERATOR_EXE} ${PARAMS_FIDLS} -d ${PARAMS_DESTINATION} -sk")
		foreach(_var RANGE 1 5)
			execute_process(COMMAND ${COMMONAPI_GENERATOR_EXE} ${PARAMS_FIDLS} -d ${PARAMS_DESTINATION} -sk
							WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
							RESULT_VARIABLE EXIT_CODE
							OUTPUT_VARIABLE GENERATOR_OUTPUT
							ERROR_VARIABLE GENERATOR_OUTPUT
							OUTPUT_STRIP_TRAILING_WHITESPACE
							ERROR_STRIP_TRAILING_WHITESPACE)
			if(EXIT_CODE)
				message(FATAL_ERROR "Failed to generate files from FIDL:\n ${GENERATOR_OUTPUT}")
			else()
				message(STATUS "has generate core files into ${PARAMS_DESTINATION}")
				break()
			endif()
		endforeach()
	else()
		# if the generator is not found, print to tell devolper
		message(STATUS "Couldn't find a common-api generator...use common-api-generator directly!")
		# common-api-generator can be used directly.
		message(STATUS "common-api-generator can be used directly. ...")
		# execute the generate command ...
		message(STATUS "execute the commonapi generate command directly...")
		message(STATUS "command:commonapi-generator-linux-x86_64 ${PARAMS_FIDLS} -d ${PARAMS_DESTINATION} -sk")
		foreach(_var RANGE 1 5)
			execute_process(COMMAND commonapi-generator-linux-x86_64 ${PARAMS_FIDLS} -d ${PARAMS_DESTINATION} -sk
							WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
							RESULT_VARIABLE EXIT_CODE
							OUTPUT_VARIABLE GENERATOR_OUTPUT
							ERROR_VARIABLE GENERATOR_OUTPUT
							OUTPUT_STRIP_TRAILING_WHITESPACE
							ERROR_STRIP_TRAILING_WHITESPACE
							OUTPUT_QUIET
							ERROR_QUIET)
			if(EXIT_CODE)
				message(FATAL_ERROR "Failed to generate use common-api-generator directly:\n ${GENERATOR_OUTPUT}")
			else()
				message(STATUS "has generate core files into ${PARAMS_DESTINATION}")
				break()
			endif()
		endforeach()
	endif()

	# searching for common-api-dbus-generator executable ...
	message(STATUS "searching for common-api-dbus-generator executable ...")

	find_program(COMMONAPI_DBUS_GENERATOR_EXE
				  NAMES
					  "commonapi-dbus-generator-linux-x86_64"
				  PATHS
					  $ENV{PATH}
					  ${CMAKE_CURRENT_SOURCE_DIR}/../commonapi-dbus-generator
					  )


	if(COMMONAPI_DBUS_GENERATOR_EXE)
		message(STATUS "find common-dbus-api generator")

		# execute the generate command ...
		message(STATUS "execute the commonapi dbus generate command ...")
		message(STATUS "command:${COMMONAPI_DBUS_GENERATOR_EXE} ${PARAMS_FIDLS} -d ${PARAMS_DESTINATION}")
		foreach(_var RANGE 1 5)
			execute_process(COMMAND ${COMMONAPI_DBUS_GENERATOR_EXE} ${PARAMS_FIDLS} -d ${PARAMS_DESTINATION}
							WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
							RESULT_VARIABLE EXIT_CODE
							OUTPUT_VARIABLE GENERATOR_OUTPUT
							ERROR_VARIABLE GENERATOR_OUTPUT
							OUTPUT_STRIP_TRAILING_WHITESPACE
							ERROR_STRIP_TRAILING_WHITESPACE)
			if(EXIT_CODE)
				message(FATAL_ERROR "Failed to generate dbus files from FIDL:\n ${GENERATOR_OUTPUT}")
			else()
				message(STATUS "has generate dbus files into ${PARAMS_DESTINATION}")
				break()
			endif()
		endforeach()
	else()
		# if the generator is not found, print to tell devolper
		message(STATUS "Couldn't find a common-api generator...use common-api-generator directly!")
		# common-api-dbus-generator can be used directly.
		message(STATUS "common-api-dbus-generator can be used directly. ...")
		# execute the generate command ...
		message(STATUS "execute the common-api-dbus-generator command directly...")
		message(STATUS "command:commonapi-dbus-generator-linux-x86_64 ${PARAMS_FIDLS} -d ${PARAMS_DESTINATION} -sk")
		foreach(_var RANGE 1 5)
			execute_process(COMMAND commonapi-dbus-generator-linux-x86_64 ${PARAMS_FIDLS} -d ${PARAMS_DESTINATION} -sk
							WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
							RESULT_VARIABLE EXIT_CODE
							OUTPUT_VARIABLE GENERATOR_OUTPUT
							ERROR_VARIABLE GENERATOR_OUTPUT
							OUTPUT_STRIP_TRAILING_WHITESPACE
							ERROR_STRIP_TRAILING_WHITESPACE)
			if(EXIT_CODE)
				message(FATAL_ERROR "Failed to generate use commonapi-dbus-generator-linux-x86_64 directly:\n ${GENERATOR_OUTPUT}")
			else()
				message(STATUS "has generate dbus files into ${PARAMS_DESTINATION}")
				break()
			endif()
		endforeach()
	endif()

endmacro()

function(COMMON_API_GENERATE_SOUCRES)
	#parse the input parameters
	set(options DBUS)
	set(oneValueArgs TARGET DESTINATION ALT_DESTINATION HEADER_TEMPLATE)
	set(multiValueArgs FIDLS FIDL_DEPENDS)

	cmake_parse_arguments(PARAMS "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

	message(STATUS "Will generate common-api files...")
	if(NOT PARAMS_FIDLS)
		message(FATAL_ERROR "FIDLS must be specified")
	endif()

	if(NOT PARAMS_TARGET)
		message(FATAL_ERROR "TARGET must be specified")
	endif()

	foreach(FIDL ${PARAMS_FIDLS})
		get_filename_component(FIDL_PATH ${FIDL} ABSOLUTE)

		message(STATUS "FIDL_PATH:${FIDL_PATH}")
	endforeach()

	message(STATUS "Determining list of generated files for ${PARAMS_FIDLS}")

	GENERATE_FILES()

endfunction()
