cmake_minimum_required(VERSION 2.8)

project(REST)

IF(DEFINED INSTALL_PREFIX)
    message("\noo Using user install path.")
ELSE()
    message("\noo Using default install path.")
    set(INSTALL_PREFIX /usr/local/REST)
ENDIF()


set(CMAKE_INSTALL_PREFIX ${INSTALL_PREFIX})
message ("oo Install prefix : ${CMAKE_INSTALL_PREFIX}\n" )

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH})

include(FindROOT)
include(MacroRootDict)
include_directories(${ROOT_INCLUDE_DIRS})


SET(CMAKE_CXX_FLAGS "-std=c++11 -Wall -march=native")

set(Garfield_FIND_REQUIRED TRUE)

include(FindGarfield)
include_directories(${Garfield_INCLUDE_DIRS})


add_subdirectory(source)

########### TODO : THERE MUST BE A WAY IN CMAKE TO ENTER RECURSIVELY IN DIRECTORIES LOOKING FOR .h FILES
########### excluding external directory would be necessary.
file(GLOB_RECURSE headers "${CMAKE_CURRENT_SOURCE_DIR}/source/restcore/inc/*.h" )
set(HS ${HS} ${headers})

file(GLOB_RECURSE headers "${CMAKE_CURRENT_SOURCE_DIR}/source/metadata/general/inc/*.h" )
set(HS ${HS} ${headers})

file(GLOB_RECURSE headers "${CMAKE_CURRENT_SOURCE_DIR}/source/metadata/geant4/inc/*.h" )
set(HS ${HS} ${headers})

file(GLOB_RECURSE headers "${CMAKE_CURRENT_SOURCE_DIR}/source/metadata/daq/inc/*.h" )
set(HS ${HS} ${headers})

file(GLOB_RECURSE headers "${CMAKE_CURRENT_SOURCE_DIR}/source/metadata/readout/inc/*.h" )
set(HS ${HS} ${headers})

file(GLOB_RECURSE headers "${CMAKE_CURRENT_SOURCE_DIR}/source/events/analysis/inc/*.h" )
set(HS ${HS} ${headers})

file(GLOB_RECURSE headers "${CMAKE_CURRENT_SOURCE_DIR}/source/events/geant4/inc/*.h" )
set(HS ${HS} ${headers})

file(GLOB_RECURSE headers "${CMAKE_CURRENT_SOURCE_DIR}/source/events/general/inc/*.h" )
set(HS ${HS} ${headers})

file(GLOB_RECURSE headers "${CMAKE_CURRENT_SOURCE_DIR}/source/events/general/inc/*.h" )
set(HS ${HS} ${headers})

file(GLOB_RECURSE headers "${CMAKE_CURRENT_SOURCE_DIR}/source/processes/general/inc/*.h" )
set(HS ${HS} ${headers})

file(GLOB_RECURSE headers "${CMAKE_CURRENT_SOURCE_DIR}/source/processes/analysis/inc/*.h" )
set(HS ${HS} ${headers})

file(GLOB_RECURSE headers "${CMAKE_CURRENT_SOURCE_DIR}/source/processes/raw/inc/*.h" )
set(HS ${HS} ${headers})

file(GLOB_RECURSE headers "${CMAKE_CURRENT_SOURCE_DIR}/source/processes/signal/inc/*.h" )
set(HS ${HS} ${headers})

file(GLOB_RECURSE headers "${CMAKE_CURRENT_SOURCE_DIR}/source/viewers/general/inc/*.h" )
set(HS ${HS} ${headers})

file(GLOB_RECURSE headers "${CMAKE_CURRENT_SOURCE_DIR}/source/viewers/geant4/inc/*.h" )
set(HS ${HS} ${headers})

file(GLOB_RECURSE headers "${CMAKE_CURRENT_SOURCE_DIR}/source/tools/inc/*.h" )
set(HS ${HS} ${headers})

file(GLOB_RECURSE headers "${CMAKE_CURRENT_SOURCE_DIR}/source/tools/inc/*.h" )
set(HS ${HS} ${headers})

INSTALL(FILES ${HS} DESTINATION ${CMAKE_INSTALL_PREFIX}/include)

#########################################################


file(GLOB_RECURSE tsp_headers "${CMAKE_CURRENT_SOURCE_DIR}/source/external/tsp/inc/*.h" )
set(TSP_HS ${tsp_headers})

INSTALL(FILES ${TSP_HS} DESTINATION ${CMAKE_INSTALL_PREFIX}/include/tsp)

install(PROGRAMS scripts/thisREST.sh scripts/LoadRESTScripts.C DESTINATION .)

install( DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/scripts
         DESTINATION .
         COMPONENT install
       )

install( DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/inputData
    DESTINATION .
         COMPONENT install
       )

install( DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/config
    DESTINATION .
         COMPONENT install
       )

get_filename_component(result ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_INSTALL_PREFIX} ABSOLUTE)
set(FULL_PATH_INSTALL ${result})

message( "\n++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n
          After \"make install\" REST will be installed in ${FULL_PATH_INSTALL}\n\n
          IMPORTANT : include the following line in your .bashrc\n\n
          source ${FULL_PATH_INSTALL}/thisREST.sh\n
          \n++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n" )
