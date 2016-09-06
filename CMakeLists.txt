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

set ( SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR} )

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

install(
    FILES ${CMAKE_CURRENT_SOURCE_DIR}/inputData/runNumber
    PERMISSIONS OWNER_WRITE OWNER_READ GROUP_WRITE GROUP_READ WORLD_WRITE WORLD_READ
    DESTINATION .
    COMPONENT install
   )

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
          After \"make install\" REST will be installed in ${CMAKE_INSTALL_PREFIX}\n
	  If you are installing to a system directory you will need administrative privileges\n
	  Executing : sudo make install\n\n

	  If you want to change the installation directory execute cmake as follows: \n
	  cmake -DINSTALL_PREFIX=PATH_TO_INSTALL_DIRECTORY PATH_TO_RESTv2_DIRECTORY \n\n

          IMPORTANT : include the following line in your .bashrc\n
          source ${CMAKE_INSTALL_PREFIX}/thisREST.sh\n
          \n++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n" )

#### This code gets the commit number to be inserted in thisREST.sh
set (git_cmd git)
set (git_arg rev-parse --verify HEAD )

install( CODE 
                "
execute_process(COMMAND ${git_cmd} ${git_arg}
  WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
  RESULT_VARIABLE git_result
  OUTPUT_VARIABLE git_ver)

string(SUBSTRING \${git_ver} 0 8 git_commit )
message(STATUS \"REST commit : \${git_commit}\")

"
)

#### This code gets the branch number to be inserted in thisREST.sh
install( CODE 
                "
execute_process(COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/cmake/printBranch.sh
  WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
  RESULT_VARIABLE git_result
  OUTPUT_VARIABLE git_branch)

message(STATUS \"REST branch : \${git_branch}\")
"
)


#### This code generates thisREST.sh

install( CODE
"
file( WRITE \${CMAKE_INSTALL_PREFIX}/thisREST.sh 

\"\#!/bin/bash
export REST_INSTALL=\${CMAKE_INSTALL_PREFIX}
export REST_SOURCE=${SOURCE_DIR}
export REST_PATH=\\\$REST_INSTALL
export PATH=\\\$PATH:\\\$REST_PATH/bin
export LD_LIBRARY_PATH=\\\$LD_LIBRARY_PATH:\\\$REST_PATH/lib
export LIBRARY_PATH=\\\$LIBRARY_PATH:\\\$REST_PATH/lib

alias restRoot=\\\"root -l \\\$REST_PATH/scripts/LoadRESTScripts.C\\\"



echo \\\"*****************************************************************************\\\"
echo \\\"  W E L C O M E   to  R E S T              \\\"
echo \\\"                                                                               \\\"
echo \\\"  Commit : \${git_commit} 		                        \\\"
echo \\\"  Branch : \${git_branch}                                     \\\"
echo \\\"  REST_INSTALL = $REST_PATH                                   \\\"
echo \\\"                                                                               \\\"
echo \\\"  Use restRoot command to load REST libraries and scripts inside ROOT     \\\"
echo \\\"                                                                               \\\"
echo \\\"  REST Support mail-list : rest-support@cern.ch               \\\"
echo \\\"  REST Development mail-list : rest-dev@cern.ch               \\\"
echo \\\"                                                                               \\\"
echo \\\"  You can subscribe at egroups.cern.ch                    \\\"
echo \\\"                                                                              \\\"
echo \\\"  If you have no CERN account you can get an account            \\\"
echo \\\"  linked to your usual mail address at this site:              \\\"
echo \\\"                                                                               \\\"
echo \\\"  https://account.cern.ch/account/Externals/                 \\\"
echo \\\"  *****************************************************************************\\\"
echo \\\"                                                                               \\\"

\"
)
        "
)
