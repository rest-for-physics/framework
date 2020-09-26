set(rest_include_dirs)
set(rest_libraries)
set(rest_macros)
set(rest_libs_index)

add_subdirectory(tools)
add_subdirectory(restcore)
add_subdirectory(metadata)
add_subdirectory(events)
add_subdirectory(processes)
add_subdirectory(../macros tasks.out)

########################################## Geant4 library ###################################
IF( NOT DEFINED REST_G4_LIB )
	set(REST_G4_LIB ON)
ENDIF ()

if (${REST_G4_LIB} MATCHES "ON")
	add_definitions(-DREST_Geant4Lib)
	add_subdirectory(libraries/geant4)
	list( APPEND rest_libs_index "RestGeant4" )
endif (${REST_G4_LIB} MATCHES "ON")

########################################## Axion library ###################################
IF( NOT DEFINED REST_AXION_LIB )
	set(REST_AXION_LIB OFF)
ENDIF ()

if (${REST_AXION_LIB} MATCHES "ON")
	add_definitions(-DREST_AxionLib)
	add_subdirectory(libraries/axion)
	list( APPEND rest_libs_index "RestAxion" )
endif (${REST_AXION_LIB} MATCHES "ON")

#### This must be placed after all targets!!!
set(rest_include_dirs ${local_include_dirs})
set(rest_libraries ${local_libraries})

#SUBDIRLIST(SUBDIRS libraries)

############################################################################################

set(rest_include_dirs ${rest_include_dirs} PARENT_SCOPE)
set(rest_libraries ${rest_libraries} PARENT_SCOPE)

set(rest_libraries_regular "-L${CMAKE_INSTALL_PREFIX}/lib ")
foreach(lib ${rest_libraries})
  set(rest_libraries_regular "${rest_libraries_regular} -l${lib} ")
endforeach(lib)
set(rest_libraries_regular ${rest_libraries_regular} PARENT_SCOPE)
set(rest_libs_index ${rest_libs_index} PARENT_SCOPE)

add_subdirectory(bin)

set(rest_exes ${rest_exes} PARENT_SCOPE)
set(rest_macros ${rest_macros} PARENT_SCOPE)
set(rest_macros_str ${rest_macros_str} PARENT_SCOPE)
