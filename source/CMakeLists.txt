set(rest_include_dirs)
set(rest_libraries)
set(rest_macros)
set(rest_libs_index)
set(restlibHeaders)

add_subdirectory(framework)
add_subdirectory(../macros tasks.out)

## There is a problem with this variable because if used it is cached.
## And then there is no option to enable all but one library
## We need a system that enables all RESTLIB_ but it is just valid on 
## the cmake session we are running
IF( DEFINED REST_ALL_LIBS )
	if (${REST_ALL_LIBS} MATCHES "ON")
		set(REST_ALL_LIBS ON)
	endif (${REST_ALL_LIBS} MATCHES "ON")
		set(RESTLIB_DETECTOR ON)
		set(RESTLIB_GEANT4 ON)
		set(RESTLIB_RAW ON)
		set(RESTLIB_TRACK ON)
		set(RESTLIB_AXION ON)
		set(RESTLIB_MODEL ON)
ENDIF ()


########################################## Detector library ###################################
IF( NOT DEFINED RESTLIB_DETECTOR )
	set(RESTLIB_DETECTOR OFF)
ENDIF ()

if (${RESTLIB_DETECTOR} MATCHES "ON")
	add_definitions(-DREST_DetectorLib)
	add_subdirectory(libraries/detector)
	list( APPEND rest_libs_index "RestDetector" )

	install(DIRECTORY libraries/detector/macros/
		DESTINATION ./macros/detector/
		COMPONENT install
		)

	file(GLOB_RECURSE libHeaders "libraries/detector/inc/*.h")
	set(restlibHeaders ${restlibHeaders} ${libHeaders} )

endif (${RESTLIB_DETECTOR} MATCHES "ON")

########################################## Geant4 library ###################################
IF( NOT DEFINED RESTLIB_GEANT4 )
	set(RESTLIB_GEANT4 OFF)

	# If restG4 package was enabled we need RestGeant4Lib
	if (${REST_G4} MATCHES "ON")
		set(RESTLIB_GEANT4 ON)
	endif (${REST_G4} MATCHES "ON")
ENDIF ()

if (${RESTLIB_GEANT4} MATCHES "ON")
	add_definitions(-DREST_Geant4Lib)
	add_subdirectory(libraries/geant4)
	list( APPEND rest_libs_index "RestGeant4" )

	install(DIRECTORY libraries/geant4/macros/
		DESTINATION ./macros/geant4/
		COMPONENT install
		)

	file(GLOB_RECURSE libHeaders "libraries/geant4/inc/*.h")
	set(restlibHeaders ${restlibHeaders} ${libHeaders} )
endif (${RESTLIB_GEANT4} MATCHES "ON")

########################################## Track library ###################################
IF( NOT DEFINED RESTLIB_TRACK )
	set(RESTLIB_TRACK OFF)
ENDIF ()

if (${RESTLIB_TRACK} MATCHES "ON")
	add_definitions(-DREST_TrackLib)
	add_subdirectory(libraries/track)
	list( APPEND rest_libs_index "RestTrack" )

	file(GLOB_RECURSE libHeaders "libraries/track/inc/*.h")
	set(restlibHeaders ${restlibHeaders} ${libHeaders} )

endif (${RESTLIB_TRACK} MATCHES "ON")

########################################## Axion library ###################################
IF( NOT DEFINED RESTLIB_AXION )
	set(RESTLIB_AXION OFF)
ENDIF ()

if (${RESTLIB_AXION} MATCHES "ON")
	add_definitions(-DREST_AxionLib)
	add_subdirectory(libraries/axion)
	list( APPEND rest_libs_index "RestAxion" )

	file(GLOB_RECURSE libHeaders "libraries/axion/inc/*.h")
	set(restlibHeaders ${restlibHeaders} ${libHeaders} )

	install(DIRECTORY libraries/axion/data/
		DESTINATION ./data/axion/
		COMPONENT install
		)
endif (${RESTLIB_AXION} MATCHES "ON")

########################################## Raw library ###################################
IF( NOT DEFINED RESTLIB_RAW )
	set(RESTLIB_RAW OFF)
ENDIF ()

if (${RESTLIB_RAW} MATCHES "ON")
	add_definitions(-DREST_RawLib)
	add_subdirectory(libraries/raw)
	list( APPEND rest_libs_index "RestRaw" )

	install(DIRECTORY libraries/raw/macros/
		DESTINATION ./macros/raw/
		COMPONENT install
		)

	file(GLOB_RECURSE libHeaders "libraries/raw/inc/*.h")
	set(restlibHeaders ${restlibHeaders} ${libHeaders} )
endif (${RESTLIB_RAW} MATCHES "ON")

########################################## Model library ###################################
IF( NOT DEFINED RESTLIB_MODEL )
	set(RESTLIB_MODEL OFF)
ENDIF ()

if (${RESTLIB_MODEL} MATCHES "ON")
	add_definitions(-DREST_ModelLib)
	add_subdirectory(libraries/model)
	list( APPEND rest_libs_index "RestModel" )

	file(GLOB_RECURSE libHeaders "libraries/model/inc/*.h")
	set(restlibHeaders ${restlibHeaders} ${libHeaders} )

	install(DIRECTORY libraries/model/macros/
		DESTINATION ./macros/model/
		COMPONENT install
		)
endif (${RESTLIB_MODEL} MATCHES "ON")

#message( STATUS "rest_include_dirs: ${rest_include_dirs}" )

#SUBDIRLIST(SUBDIRS libraries)

########################################## Connectors library ###################################
list( LENGTH rest_libs_index N_libs )

## The detector library is always involved. Right now, if we enable only raw and track then,
## there is no need for connectors.
if( N_libs GREATER 1 AND DEFINED RESTLIB_DETECTOR AND ${RESTLIB_DETECTOR} MATCHES "ON" )
    add_definitions(-DREST_ConnectorsLib)
	add_subdirectory(libraries/connectors)
    list( APPEND rest_libs_index "RestConnectors" )

	file(GLOB_RECURSE libHeaders "libraries/connectors/inc/*.h")
	set(restlibHeaders ${restlibHeaders} ${libHeaders} )
endif( N_libs GREATER 1 AND DEFINED RESTLIB_DETECTOR AND ${RESTLIB_DETECTOR} MATCHES "ON" )

############################################################################################

############################################################################################
#### This must be placed after all targets!!!
set(rest_include_dirs ${local_include_dirs})
set(rest_libraries ${local_libraries})
set(Headers ${restlibHeaders})
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
