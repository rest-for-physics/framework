PROJECT(main)
CMAKE_MINIMUM_REQUIRED(VERSION 2.8)



#set other flags here

IF(NOT DEFINED INSTALL_PREFIX)
	IF(DEFINED ENV{REST_INSTALL_PATH})
		set(INSTALL_PREFIX $ENV{REST_INSTALL_PATH})
	else()
		set(INSTALL_PREFIX ${CMAKE_CURRENT_SOURCE_DIR}/install)
	endif()
ENDIF()

#message($ENV{REST_INSTALL_PATH})
set(CMAKE_INSTALL_PREFIX ${INSTALL_PREFIX})
message ("oo REST will be installed in : ${CMAKE_INSTALL_PREFIX}\n" )

SET(CMAKE_CXX_FLAGS "-std=c++1y")

#define c++ pragmas
IF( NOT DEFINED REST_WELCOME )
    set(REST_WELCOME ON)
ENDIF()

if(${REST_GARFIELD} MATCHES "ON")
	add_definitions(-DUSE_Garfield)
endif(${REST_GARFIELD} MATCHES "ON")



#find includes and libs
set(external_include_dirs)
set(external_libs)
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH})
IF (CMAKE_SYSTEM_NAME MATCHES "Linux")
	include(FindROOT)
	set(external_include_dirs ${external_include_dirs} ${ROOT_INCLUDE_DIRS})
    #file(GLOB rootlibs ${ROOT_LIBRARY_DIR}/*.so)
	set(external_libs ${external_libs} ${ROOT_LIBRARIES} -lGui  -lEve -lGeom -lMathMore -lGdml)

	if(${REST_GARFIELD} MATCHES "ON")
	include(FindGarfield)
	set(external_include_dirs ${external_include_dirs} ${Garfield_INCLUDE_DIRS})
	set(external_libs ${external_libs} ${Garfield_LIBRARIES})
	endif(${REST_GARFIELD} MATCHES "ON")

	find_library(tiny libtinyxml.so)

	if(NOT tiny)
		message(FATAL_ERROR "library \"tinyxml\" is required, but not found!\ntype \"sudo apt-get install libtinyxml-dev\" to install it.")
	else()
		message("-- Found tinyxml library in ${tiny}")
	endif(NOT tiny)
	set(external_libs ${external_libs} ${tiny})


	#Here Windows is just for test. It is for the convenience of vs to make gramma check
ELSE (CMAKE_SYSTEM_NAME MATCHES "Windows")
	#set(ROOT_INCLUDE_DIR $ENV{ROOTSYS}/include)
	#set(ROOT_LIBRARY_DIR $ENV{ROOTSYS}/lib)
	#include_directories(${ROOT_INCLUDE_DIR})
	#find_program(ROOTCINT_EXECUTABLE rootcint PATHS $ENV{ROOTSYS}/bin)
    #file(GLOB rootlibs ${ROOT_LIBRARY_DIR}/*.lib)

	#set(Garfield_INCLUDE_DIRS $ENV{GARFIELD_HOME})
	#message(${Garfield_INCLUDE_DIRS})
	#include_directories(${Garfield_INCLUDE_DIRS})

	set(USR_INCLUDE_DIRS $ENV{usrinclude})
	message(${USR_INCLUDE_DIRS})
	include_directories(${USR_INCLUDE_DIRS})
ENDIF (CMAKE_SYSTEM_NAME MATCHES "Linux")
include(MacroRootDict)
include(CompileDir)
include_directories(${external_include_dirs})

#gether some infomation
IF (CMAKE_SYSTEM_NAME MATCHES "Linux")
    #get time
    execute_process(COMMAND date OUTPUT_VARIABLE d)
    string(REPLACE "\n" "" date ${d})

    file(STRINGS ${CMAKE_CURRENT_SOURCE_DIR}/cmake/version version )

	#get git branch
    execute_process(COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/cmake/printVersionDate.sh
	WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
	OUTPUT_VARIABLE git_date)

	if(DEFINED git_date)#git is available
	string(REPLACE "\n" "" git_date ${git_date})
	message(STATUS "REST date : ${git_date}")

	#get git number	
	set (git_cmd git)
	set (git_arg rev-parse --verify HEAD )
	execute_process(COMMAND ${git_cmd} ${git_arg}
	WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
	OUTPUT_VARIABLE git_ver)
	string(SUBSTRING ${git_ver} 0 8 git_commit )
	message(STATUS "REST commit : ${git_commit}")

	#get git branch
	execute_process(COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/cmake/printBranch.sh
	WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
	OUTPUT_VARIABLE b)
	string(REPLACE "\n" "" git_branch ${b})
	message(STATUS "REST branch : ${git_branch}")
	
	endif(DEFINED git_date)

ENDIF (CMAKE_SYSTEM_NAME MATCHES "Linux")





#start compile
add_subdirectory(source)

message("rest Libraries : " ${rest_libraries_regular})
message("rest executables: " ${rest_exes})
message("rest macros: " ${rest_macros_str})

#begin installation
file(GLOB_RECURSE Headers "${CMAKE_CURRENT_SOURCE_DIR}/source/*.h")

INSTALL(FILES ${Headers} DESTINATION ${CMAKE_INSTALL_PREFIX}/include)

INSTALL(FILES ${CMAKE_CURRENT_SOURCE_DIR}/CHANGELOG DESTINATION ${CMAKE_INSTALL_PREFIX})

INSTALL(FILES ${CMAKE_CURRENT_SOURCE_DIR}/LICENCE DESTINATION ${CMAKE_INSTALL_PREFIX})

install( DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/scripts
         DESTINATION .
         COMPONENT install
                 )

install( DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/macros
         DESTINATION .
         COMPONENT install
                 )

install(
    DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/inputData
    DESTINATION .
    COMPONENT install
        )

#copying pcm files

install(CODE
"
file(GLOB PCMFiles \"\${CMAKE_CURRENT_SOURCE_DIR}/rootdict/*.pcm\")
file(COPY \${PCMFiles} DESTINATION \${CMAKE_INSTALL_PREFIX}/lib)

"

)


install( CODE
"
file( WRITE \${CMAKE_INSTALL_PREFIX}/thisREST.sh 

\"\#!/bin/bash
export REST_INSTALL=\${CMAKE_INSTALL_PREFIX}
export REST_SOURCE=${SOURCE_DIR}
export REST_PATH=\\\$REST_INSTALL
export REST_GARFIELD_INCLUDE=${Garfield_INCLUDE_DIRS}
export REST_GARFIELD_LIB=${Garfield_LIBRARIES}
export PATH=\\\$REST_PATH/bin:\\\$PATH
export LD_LIBRARY_PATH=\\\$REST_PATH/lib:\\\$LD_LIBRARY_PATH
export LIBRARY_PATH=\\\$LIBRARY_PATH:\\\$REST_PATH/lib

\#alias restRoot=\\\"root -l \\\$REST_PATH/scripts/LoadRESTScripts.C\\\"

if [ \\\$(rest-config --flags | grep \\\"REST_WELCOME=ON\\\") ];then
rest-config --welcome
fi
\"
)
        "
)


foreach(mac ${rest_macros})

string(REPLACE " " "" mac ${mac})
string(REPLACE "rest" "" m ${mac})

install( CODE
"
file( APPEND \${CMAKE_INSTALL_PREFIX}/thisREST.sh 

\"
alias ${mac}=\\\"restManager ${m}\\\"
\"
)
        "
)

endforeach(mac ${rest_macros})



install( CODE
"


file( WRITE \${CMAKE_INSTALL_PREFIX}/bin/rest-config

\"

if [ $# -ne 1 ] ; then 

echo \\\"  Use restRoot command to load REST libraries and scripts inside ROOT          \\\"
echo \\\"  Use restManager command to manage the configurations and start REST          \\\"
echo \\\"  Type \\\\\\\"rest-config --help\\\\\\\" for more info                        \\\"    

else

option=$1

if [ $option = \\\"--incdir\\\" ] ; then
echo ${CMAKE_INSTALL_PREFIX}/include

fi

if [ $option = \\\"--libdir\\\" ] ; then
echo ${CMAKE_INSTALL_PREFIX}/lib

fi

if [ $option = \\\"--libs\\\" ] ; then
echo ${rest_libraries_regular}

fi

if [ $option = \\\"--exes\\\" ] ; then
echo ${rest_exes} 
echo -------------------------------------------
echo ${rest_macros_str}

fi

if [ $option = \\\"--version\\\" ] ; then
echo ${version}

fi

if [ $option = \\\"--flags\\\" ] ; then
echo REST_WELCOME=${REST_WELCOME}
echo REST_GARFIELD=${REST_GARFIELD}
echo CMAKE_CXX_FLAGS=${CMAKE_CXX_FLAGS}
echo CMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}

fi

if [ $option = \\\"--welcome\\\" ] ; then

echo \\\"  *****************************************************************************\\\"
echo \\\"  W E L C O M E   to  R E S T                                                  \\\"
echo \\\"                                                                               \\\"
echo \\\"  Commit  : ${git_commit} (${git_date})                                        \\\"
echo \\\"  Last update  :  ${git_date}                                                  \\\"
echo \\\"  Branch/Version : ${git_branch}/${version}                                    \\\"
echo \\\"  Compilation date : ${date}                                                   \\\"
echo \\\"								                                                \\\"
echo \\\"  Installed at : $REST_PATH                                                    \\\"
echo \\\"                                                                               \\\"
echo \\\"  REST news/dev/support mail-list : rest-dev@cern.ch                           \\\"
echo \\\"                                                                               \\\"
echo \\\"  You can subscribe at egroups.cern.ch                                         \\\"
echo \\\"                                                                               \\\"
echo \\\"  If you have no CERN account you can get an account                           \\\"
echo \\\"  linked to your usual mail address at this site:                              \\\"
echo \\\"                                                                               \\\"
echo \\\"  https://account.cern.ch/account/Externals/                                   \\\"
echo \\\"  *****************************************************************************\\\"
echo \\\"                                                                               \\\" 

fi

if [ $option = \\\"--help\\\" ] ; then
echo \\\"  Usage :                                                                      \\\"
echo \\\"  rest-config [--incdir]  : Show the directory of headers                      \\\"
echo \\\"  rest-config [--libdir]  : Show the directory of library                      \\\"
echo \\\"  rest-config [--libs]    : Print regular REST libraries                       \\\"
echo \\\"  rest-config [--exes]    : Print a list of REST executables with alias        \\\"
echo \\\"  rest-config [--version] : Print the version of REST                          \\\"
echo \\\"  rest-config [--welcome] : Print the welcome message                          \\\"
echo \\\"  rest-config [--flags]   : Print cmake flags defined when installing          \\\"

fi


fi





\"
)
        "
)

#install(CODE "chmod 744 ${CMAKE_INSTALL_PREFIX}/bin/rest-config")
install(CODE "execute_process(COMMAND chmod 744 ${CMAKE_INSTALL_PREFIX}/bin/rest-config)")
