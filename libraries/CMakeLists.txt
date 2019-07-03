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
  
list(REMOVE_ITEM dirlist detector dummy muon)
message(${dirlist})

FOREACH(dir ${dirlist})
    add_subdirectory(${dir})
ENDFOREACH()


set(rest_include_dirs ${rest_include_dirs} PARENT_SCOPE)
set(rest_libraries ${rest_libraries} PARENT_SCOPE)