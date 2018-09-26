#----------------------------------------------------------------------------
# Setup the project
cmake_minimum_required(VERSION 2.6 FATAL_ERROR)
project(restG4)
message("!!!!!!!!restG4!!!!!!!!")
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../cmake ${CMAKE_MODULE_PATH})

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
message(STATUS "Using REST PATH: " ${REST_PATH})

SET(CMAKE_CXX_FLAGS "-std=c++1y")

#----------------------------------------------------------------------------
# Find Geant4 package, activating all available UI and Vis drivers by default
# You can set WITH_GEANT4_UIVIS to OFF via the command line or ccmake/cmake-gui
# to build a batch mode only executable
option(WITH_GEANT4_UIVIS "Build example with Geant4 UI and Vis drivers" ON)
if(WITH_GEANT4_UIVIS)
  find_package(Geant4 REQUIRED ui_all vis_all)
else()
  find_package(Geant4 REQUIRED)
endif()
message ("-- Found Geant4 in version : ${Geant4_VERSION}" )

if(${Geant4_VERSION} MATCHES "10.4")
	add_definitions(-DG4104)
elseif(${Geant4_VERSION} MATCHES "10.3.3")
	add_definitions(-DG4104)
endif(${Geant4_VERSION} MATCHES "10.4")

include(FindROOT)



#----------------------------------------------------------------------------
# Set include and lib
if(DEFINED rest_include_dirs)
set(incdir ${PROJECT_SOURCE_DIR}/include ${ROOT_INCLUDE_DIRS} ${rest_include_dirs} ${Geant4_INCLUDE_DIR})
else()
set(incdir ${PROJECT_SOURCE_DIR}/include ${ROOT_INCLUDE_DIRS} ${REST_PATH}/include ${Geant4_INCLUDE_DIR})
endif()

if(DEFINED rest_libraries)
set(lnklib ${Geant4_LIBRARIES} ${ROOT_LIBRARIES} -lGui  -lEve -lGeom -lMathMore -lGdml -lMinuit ${rest_libraries} -lgfortran)
else()
set(lnklib ${Geant4_LIBRARIES} ${ROOT_LIBRARIES} -lGui  -lEve -lGeom -lMathMore -lGdml -lMinuit -L${REST_PATH}/lib/ -lRestTools -lRestCore -lRestEvents -lRestMetadata -lgfortran)
message( "\n++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n
          After \"make install\" restG4 will be installed in ${REST_PATH}/bin\n\n
          You can directly use it!
          \n++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n" )
endif()

#----------------------------------------------------------------------------
# Locate sources and headers for this project
#
include(${Geant4_USE_FILE})
include_directories(${incdir})

file(GLOB sources ${PROJECT_SOURCE_DIR}/src/*.cc)
file(GLOB headers ${PROJECT_SOURCE_DIR}/include/*.hh ${Garfield_INCLUDE_DIRS}/*.hh )

#----------------------------------------------------------------------------
# Add the executable, and link it to the Geant4 libraries
#
add_executable(restG4 restG4.cc ${sources} ${headers})
target_link_libraries(restG4 ${lnklib})


#----------------------------------------------------------------------------
# Copy all scripts to the build directory, i.e. the directory in which we
# build sphereSimLab. This is so that we can run the executable directly because it
# relies on these scripts being in the current working directory.

install( DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/example
         DESTINATION .
         COMPONENT install
                 )

install( DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/mac
         DESTINATION macros
         COMPONENT install
                 )

#----------------------------------------------------------------------------
# Install the executable to 'bin' directory under CMAKE_INSTALL_PREFIX
#
install(TARGETS restG4 DESTINATION bin)

install(CODE "execute_process(COMMAND chmod 744 ${REST_PATH}/example/replaceRMLVars.py)")

