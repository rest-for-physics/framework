# - Finds ROOT instalation
# This module sets up SQL information 
# It defines:
# SQL_FOUND          If the ROOT is found
# SQL_INCLUDE_DIR    PATH to the include directory
# SQL_LIBRARIES      Libraries required to use SQL
# SQL_VERSION        Version of MySQL

find_program(SQL_CONFIG_EXECUTABLE mysql_config )

if(NOT SQL_CONFIG_EXECUTABLE)
	set(SQL_FOUND FALSE)
else()    
	set(SQL_FOUND TRUE)

	execute_process(
		COMMAND ${SQL_CONFIG_EXECUTABLE} --cxxflags
		OUTPUT_VARIABLE SQL_INCLUDE_DIR
		OUTPUT_STRIP_TRAILING_WHITESPACE)

	execute_process(
		COMMAND ${SQL_CONFIG_EXECUTABLE} --libs
		OUTPUT_VARIABLE SQL_LIBRARIES
		OUTPUT_STRIP_TRAILING_WHITESPACE)

	execute_process(
		COMMAND ${SQL_CONFIG_EXECUTABLE} --version
		OUTPUT_VARIABLE SQL_VERSION
		OUTPUT_STRIP_TRAILING_WHITESPACE)
endif()


if (NOT SQL_FOUND)
	message(FATAL_ERROR "SQL required, but not found")
else()
	message(STATUS "Found MySQL ${SQL_VERSION}")
endif()

