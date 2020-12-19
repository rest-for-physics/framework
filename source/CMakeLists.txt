set(rest_include_dirs)
set(rest_libraries)
set(rest_macros)
set(rest_libs_index)
set(restlibHeaders)

add_subdirectory(tools)
add_subdirectory(restcore)
add_subdirectory(metadata)
add_subdirectory(events)
add_subdirectory(processes)
add_subdirectory(../macros tasks.out)

IF( DEFINED REST_ALL_LIBS )
	if (${REST_ALL_LIBS} MATCHES "ON")
		set(REST_ALL_LIBS ON)
	endif (${REST_ALL_LIBS} MATCHES "ON")
		set(REST_GEANT4_LIB ON)
		set(REST_RAW_LIB ON)
		set(REST_TRACK_LIB ON)
		set(REST_AXION_LIB ON)
		set(REST_MODEL_LIB ON)
ENDIF ()

########################################## Geant4 library ###################################
IF( NOT DEFINED REST_GEANT4_LIB )
	set(REST_GEANT4_LIB OFF)

	# If restG4 package was enabled we need RestGeant4Lib
	if (${REST_G4} MATCHES "ON")
		set(REST_GEANT4_LIB ON)
	endif (${REST_G4} MATCHES "ON")
ENDIF ()

if (${REST_GEANT4_LIB} MATCHES "ON")
	add_definitions(-DREST_Geant4Lib)
	add_subdirectory(libraries/geant4)
	list( APPEND rest_libs_index "RestGeant4" )

	install(DIRECTORY libraries/geant4/macros/
		DESTINATION ./macros/geant4/
		COMPONENT install
		)

	file(GLOB_RECURSE libHeaders "libraries/geant4/inc/*.h")
	set(restlibHeaders ${restlibHeaders} ${libHeaders} )
endif (${REST_GEANT4_LIB} MATCHES "ON")

########################################## Track library ###################################
IF( NOT DEFINED REST_TRACK_LIB )
	set(REST_TRACK_LIB OFF)
ENDIF ()

if (${REST_TRACK_LIB} MATCHES "ON")
	add_definitions(-DREST_TrackLib)
	add_subdirectory(libraries/track)
	list( APPEND rest_libs_index "RestTrack" )

	file(GLOB_RECURSE libHeaders "libraries/track/inc/*.h")
	set(restlibHeaders ${restlibHeaders} ${libHeaders} )

endif (${REST_TRACK_LIB} MATCHES "ON")

########################################## Axion library ###################################
IF( NOT DEFINED REST_AXION_LIB )
	set(REST_AXION_LIB OFF)
ENDIF ()

if (${REST_AXION_LIB} MATCHES "ON")
	add_definitions(-DREST_AxionLib)
	add_subdirectory(libraries/axion)
	list( APPEND rest_libs_index "RestAxion" )

	file(GLOB_RECURSE libHeaders "libraries/axion/inc/*.h")
	set(restlibHeaders ${restlibHeaders} ${libHeaders} )

	install(DIRECTORY libraries/axion/data/
		DESTINATION ./data/axion/
		COMPONENT install
		)
endif (${REST_AXION_LIB} MATCHES "ON")

########################################## Raw library ###################################
IF( NOT DEFINED REST_RAW_LIB )
	set(REST_RAW_LIB OFF)
ENDIF ()

if (${REST_RAW_LIB} MATCHES "ON")
	add_definitions(-DREST_RawLib)
	add_subdirectory(libraries/raw)
	list( APPEND rest_libs_index "RestRaw" )

	install(DIRECTORY libraries/raw/macros/
		DESTINATION ./macros/raw/
		COMPONENT install
		)

	file(GLOB_RECURSE libHeaders "libraries/raw/inc/*.h")
	set(restlibHeaders ${restlibHeaders} ${libHeaders} )
endif (${REST_RAW_LIB} MATCHES "ON")

########################################## Model library ###################################
IF( NOT DEFINED REST_MODEL_LIB )
	set(REST_MODEL_LIB OFF)
ENDIF ()

if (${REST_MODEL_LIB} MATCHES "ON")
	add_definitions(-DREST_ModelLib)
	add_subdirectory(libraries/model)
	list( APPEND rest_libs_index "RestModel" )

	file(GLOB_RECURSE libHeaders "libraries/model/inc/*.h")
	set(restlibHeaders ${restlibHeaders} ${libHeaders} )

	install(DIRECTORY libraries/model/macros/
		DESTINATION ./macros/model/
		COMPONENT install
		)
endif (${REST_MODEL_LIB} MATCHES "ON")

#### This must be placed after all targets!!!
set(rest_include_dirs ${local_include_dirs})
set(rest_libraries ${local_libraries})
set(Headers ${restlibHeaders})

#message( STATUS "rest_include_dirs: ${rest_include_dirs}" )

#SUBDIRLIST(SUBDIRS libraries)

############################################################################################

set(Headers ${Headers} PARENT_SCOPE)
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
