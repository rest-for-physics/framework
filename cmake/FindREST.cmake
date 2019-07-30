set(REST_IN_CONFIGURATION FALSE)
set(REST_FOUND FALSE)
# find REST 
IF(NOT DEFINED REST_PATH)
	IF(DEFINED ENV{REST_PATH})
		set(REST_PATH $ENV{REST_PATH})
	ELSE()
		message(FATAL_ERROR "ERROR ::: REST_PATH must be defined as an environment variable and point to REST install directory")
		return()
	ENDIF()
ENDIF()
set(CMAKE_INSTALL_PREFIX ${REST_PATH})
#message(STATUS "Found REST in: " ${REST_PATH})
set(REST_FOUND TRUE)

find_program(REST_CONFIG_EXECUTABLE rest-config
  PATHS ${REST_PATH}/bin $ENV{REST_PATH}/bin)

if(DEFINED rest_include_dirs)
	#this means it is during installation
	#the lower case variable is from cmake config process
	set(REST_INCLUDE_DIRS ${rest_include_dirs})
	set(REST_IN_CONFIGURATION TRUE)
else()
	#this means REST is already installed
    execute_process(
      COMMAND ${REST_CONFIG_EXECUTABLE} --incdir
      OUTPUT_VARIABLE REST_INCLUDE_DIRS
      OUTPUT_STRIP_TRAILING_WHITESPACE)
endif()
#message (STATUS "REST Include Directory: ${REST_INCLUDE_DIRS}")


if(DEFINED REST_MAIN_COMPILE)
	#this means it is during installation
	set(REST_LIBRARIES ${rest_libraries})
else()
    execute_process(
      COMMAND ${REST_CONFIG_EXECUTABLE} --libs
      OUTPUT_VARIABLE REST_LIBRARIES
      OUTPUT_STRIP_TRAILING_WHITESPACE)
endif()
#message (STATUS "REST Libraries: ${REST_LIBRARIES}")