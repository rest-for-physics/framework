#----------------------------------------------------------------------------
# Setup the project
cmake_minimum_required(VERSION 2.6 FATAL_ERROR)
project(restG4)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH})

IF(DEFINED ENV{REST_PATH})
    message("Using environment REST_PATH variable $ENV{REST_PATH}")
    set(REST_PATH $ENV{REST_PATH})
ELSE()
    message("ERROR ::: REST_PATH must be defined as an environment variable and point to REST install directory")
    return()
ENDIF()

SET(CMAKE_CXX_FLAGS "-std=c++1y -w")

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

set(Garfield_FIND_REQUIRED TRUE)

include(FindGarfield)
include_directories(${Garfield_INCLUDE_DIRS})
message ("Garfield include directory : ${Garfield_INCLUDE_DIRS}\n" )

#----------------------------------------------------------------------------
# Locate sources and headers for this project
#
include_directories(${PROJECT_SOURCE_DIR}/include 
    ${Garfield_INCLUDE_DIRS}
    ${Geant4_INCLUDE_DIR}
    ${ROOT_INCLUDE_DIR}
    ${REST_PATH}/include )

file(GLOB sources ${PROJECT_SOURCE_DIR}/src/*.cc)
file(GLOB headers ${PROJECT_SOURCE_DIR}/include/*.hh ${Garfield_INCLUDE_DIRS}/*.hh )

#----------------------------------------------------------------------------
# Add the executable, and link it to the Geant4 libraries
#
add_executable(restG4 restG4.cc ${sources} ${headers})
target_link_libraries(restG4 ${Geant4_LIBRARIES} -L${ROOT_LIBRARY_DIR} ${ROOT_LIBRARIES} -L${REST_PATH}/lib/ -lRestTools -lRestCore -lRestEvents -lRestMetadata -lEve -lGeom -lGdml ${Garfield_LIBRARIES} -lgfortran )


#----------------------------------------------------------------------------
# Copy all scripts to the build directory, i.e. the directory in which we
# build sphereSimLab. This is so that we can run the executable directly because it
# relies on these scripts being in the current working directory.

file(MAKE_DIRECTORY ${PROJECT_BINARY_DIR}/macros)

set(G4Rest_MACROS
    visualtrack.mac 
    visualizacion.mac 
    vis.mac
    )


#

foreach(_script ${G4Rest_MACROS})
    configure_file(
        ${PROJECT_SOURCE_DIR}/macros/${_script}
        ${PROJECT_BINARY_DIR}/macros/${_script}
        COPYONLY
        )
endforeach()

#----------------------------------------------------------------------------
# Install the executable to 'bin' directory under CMAKE_INSTALL_PREFIX
#
install(TARGETS restG4 DESTINATION ${REST_PATH}/bin)

string(FIND ${Geant4_DIR} "/Geant4" cutStr REVERSE)

string(SUBSTRING ${Geant4_DIR} 0 ${cutStr} G4_LIBDIR)

message( "\n++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n
          After \"make install\" restG4 will be installed in ${REST_PATH}/bin\n\n
          IMPORTANT : You might need to include the following line in your .bashrc\n\n
          export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:${G4_LIBDIR}\n
          \n++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n" )

