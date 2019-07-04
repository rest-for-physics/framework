#compile the packages

# RestDetectorLib is added by default
add_subdirectory(detector)

FILE(GLOB children RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/*)
SET(dirlist "")
FOREACH(child ${children})
IF(IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/${child})
    LIST(APPEND dirlist ${child})
ENDIF()
ENDFOREACH()
  
list(REMOVE_ITEM dirlist detector dummy)
message(${dirlist})

FOREACH(dir ${dirlist})	
	string(TOUPPER ${dir} dir_upper)
	string(REPLACE "REST" "" name ${dir_upper})

	IF(NOT DEFINED REST_${name})
		set(REST_${name} ON)
	ENDIF()

	if (${REST_${name}} MATCHES "ON")
		add_subdirectory(${dir})
	endif()
ENDFOREACH()


set(rest_include_dirs ${rest_include_dirs} PARENT_SCOPE)
set(rest_libraries ${rest_libraries} PARENT_SCOPE)

set(rest_exes ${rest_exes})
set(rest_exes ${rest_exes} PARENT_SCOPE)