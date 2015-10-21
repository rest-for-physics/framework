#----------------------------------------------------------------------------
# Setup the project
cmake_minimum_required(VERSION 2.6 FATAL_ERROR)
project(restG4)

IF(DEFINED ENV{REST_PATH})
    message("...using environment REST_PATH variable $ENV{REST_PATH}")
    set(REST_PATH $ENV{REST_PATH})
ELSE()
    message("ERROR ::: REST_PATH must be defined as an environment variable and point to new_trunk source directory")
    return()
ENDIF()


#----------------------------------------------------------------------------
# Find Geant4 package, activating all available UI and Vis drivers by default
# You can set WITH_GEANT4_UIVIS to OFF via the command line or ccmake/cmake-gui
# to build a batch mode only executable
#
option(WITH_GEANT4_UIVIS "Build example with Geant4 UI and Vis drivers" ON)
if(WITH_GEANT4_UIVIS)
  find_package(Geant4 REQUIRED ui_all vis_all)
else()
  find_package(Geant4 REQUIRED)
endif()

#----------------------------------------------------------------------------
# Setup Geant4 include directories and compile definitions
#
include(${Geant4_USE_FILE})
find_package(ROOT REQUIRED)

#----------------------------------------------------------------------------
# Locate sources and headers for this project
#
include_directories(${PROJECT_SOURCE_DIR}/include 
    ${Geant4_INCLUDE_DIR}
    ${ROOT_INCLUDE_DIR}
    ${REST_PATH}/source/restcore/inc ${REST_PATH}/source/events/geant4/inc ${REST_PATH}/source/metadata/geant4/inc )

file(GLOB sources ${PROJECT_SOURCE_DIR}/src/*.cc)
file(GLOB headers ${PROJECT_SOURCE_DIR}/include/*.hh)

#----------------------------------------------------------------------------
# Add the executable, and link it to the Geant4 libraries
#
add_executable(restG4 restG4.cc ${sources} ${headers})
target_link_libraries(restG4 ${Geant4_LIBRARIES} -L${ROOT_LIBRARY_DIR} ${ROOT_LIBRARIES} -L${REST_PATH}/lib/ -lRestCore -lRestEvents -lRestMetadata -lEve -lGeom) 



#----------------------------------------------------------------------------
# Copy all scripts to the build directory, i.e. the directory in which we
# build sphereSimLab. This is so that we can run the executable directly because it
# relies on these scripts being in the current working directory.

file(MAKE_DIRECTORY ${PROJECT_BINARY_DIR}/geometry)
file(MAKE_DIRECTORY ${PROJECT_BINARY_DIR}/macros)

set(G4Rest_GEOMETRY
    materials.xml
    myGeometryTemplate.gdml
    )

set(G4Rest_MACROS
    visualtrack.mac 
    visualizacion.mac 
    vis.mac
    )

set(G4Rest_GEO_TEMPLATE
    mySetupTemplate.gdml
    )


#
foreach(_script ${G4Rest_GEO_TEMPLATE})
    configure_file(
        ${PROJECT_SOURCE_DIR}/${_script}
        ${PROJECT_BINARY_DIR}/${_script}
        COPYONLY
        )
endforeach()

foreach(_script ${G4Rest_CONFIG})
    configure_file(
        ${PROJECT_SOURCE_DIR}/config/${_script}
        ${PROJECT_BINARY_DIR}/config/${_script}
        COPYONLY
        )
endforeach()

foreach(_script ${G4Rest_GEOMETRY})
    configure_file(
        ${PROJECT_SOURCE_DIR}/geometry/${_script}
        ${PROJECT_BINARY_DIR}/geometry/${_script}
        COPYONLY
        )
endforeach()

foreach(_script ${G4Rest_MACROS})
    configure_file(
        ${PROJECT_SOURCE_DIR}/macros/${_script}
        ${PROJECT_BINARY_DIR}/macros/${_script}
        COPYONLY
        )
endforeach()

foreach(_script ${sphereSimLab_SCRIPTS})
  configure_file(
    ${PROJECT_SOURCE_DIR}/${_script}
    ${PROJECT_BINARY_DIR}/${_script}
    COPYONLY
    )
endforeach()

#----------------------------------------------------------------------------
# Install the executable to 'bin' directory under CMAKE_INSTALL_PREFIX
#
install(TARGETS restG4 DESTINATION bin)

