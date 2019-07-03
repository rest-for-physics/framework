#compile the packages

add_subdirectory(detector)


IF (NOT DEFINED REST_GARFIELD)
    set(REST_GARFIELD OFF)
ENDIF ()
if (${REST_GARFIELD} MATCHES "ON")
    add_subdirectory(garfield)
endif (${REST_GARFIELD} MATCHES "ON")


IF (NOT DEFINED REST_G4)
    set(REST_G4 OFF)
ENDIF ()
if (${REST_G4} MATCHES "ON")
    add_subdirectory(geant4)
endif (${REST_G4} MATCHES "ON")


IF (NOT DEFINED REST_PQSL)
    set(REST_PQSL OFF)
ENDIF ()
if (${REST_PQSL} MATCHES "ON")
    add_subdirectory(restDataBasePsql)
endif (${REST_PQSL} MATCHES "ON")


IF (NOT DEFINED REST_DECAY0)
    set(REST_DECAY0 OFF)
ENDIF ()
if (${REST_DECAY0} MATCHES "ON")
    add_subdirectory(restDecay0)
endif (${REST_DECAY0} MATCHES "ON")


set(rest_include_dirs ${rest_include_dirs} PARENT_SCOPE)
set(rest_libraries ${rest_libraries} PARENT_SCOPE)