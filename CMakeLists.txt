PROJECT(main)
CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

set(REST_WELCOME)
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

SET(CMAKE_CXX_FLAGS "-std=c++1y -Werror")




#find includes and libs
set(external_include_dirs)
set(external_libs)
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH})
IF (CMAKE_SYSTEM_NAME MATCHES "Linux")
	include(FindROOT)
	set(external_include_dirs ${external_include_dirs} ${ROOT_INCLUDE_DIRS})
    #file(GLOB rootlibs ${ROOT_LIBRARY_DIR}/*.so)
	set(external_libs ${external_libs} ${ROOT_LIBRARIES} -lGui  -lEve -lGeom -lMathMore)

	include(FindGarfield)
	set(external_include_dirs ${external_include_dirs} ${Garfield_INCLUDE_DIRS})
	set(external_libs ${external_libs} ${Garfield_LIBRARIES})

	find_library(tiny libtinyxml.so)

	if(NOT tiny)
		message(FATAL_ERROR "library \"tinyxml\" is required, but not found!\ntype \"sudo apt-get install libtinyxml-dev\" to install it.")
	else()
		message("-- Found tinyxml library in ${tiny}")
	endif(NOT tiny)
	set(external_libs ${external_libs} ${tiny})


	execute_process(COMMAND date OUTPUT_VARIABLE d)
	string(REPLACE "\n" "" date ${d})
	file(STRINGS ${CMAKE_CURRENT_SOURCE_DIR}/CHANGELOG v 
		LIMIT_COUNT 2
	)
list(GET v 1 version)

	#Here Windows is just for test. It is for the convenience of vs to make gramma check
ELSE (CMAKE_SYSTEM_NAME MATCHES "Windows")
	set(ROOT_INCLUDE_DIR $ENV{ROOTSYS}/include)
	set(ROOT_LIBRARY_DIR $ENV{ROOTSYS}/lib)
	include_directories(${ROOT_INCLUDE_DIR})
	find_program(ROOTCINT_EXECUTABLE rootcint PATHS $ENV{ROOTSYS}/bin)
    file(GLOB rootlibs ${ROOT_LIBRARY_DIR}/*.lib)

	set(Garfield_INCLUDE_DIRS $ENV{GARFIELD_HOME})
	message(${Garfield_INCLUDE_DIRS})
	include_directories(${Garfield_INCLUDE_DIRS})

	set(USR_INCLUDE_DIRS $ENV{include})
	message(${USR_INCLUDE_DIRS})
	include_directories(${USR_INCLUDE_DIRS})
ENDIF (CMAKE_SYSTEM_NAME MATCHES "Linux")
include(MacroRootDict)
include(CompileDir)
include_directories(${external_include_dirs})

#start compile
add_subdirectory(source)

message("rest Libraries : " ${rest_libraries_regular})
message("rest executables: " ${rest_exes})

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

fi

if [ $option = \\\"--methods\\\" ] ; then
echo 2333

fi

if [ $option = \\\"--version\\\" ] ; then
echo ${version}

fi

if [ $option = \\\"--flags\\\" ] ; then
echo REST_WELCOME=${REST_WELCOME}
echo CMAKE_CXX_FLAGS=${CMAKE_CXX_FLAGS}
echo CMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}

fi

if [ $option = \\\"--welcome\\\" ] ; then

echo \\\"  *****************************************************************************\\\"
echo \\\"  W E L C O M E   to  R E S T                                                  \\\"
echo \\\"                                                                               \\\"
echo \\\"  Commit : \${git_commit}                                                      \\\"
echo \\\"  Branch : \${git_branch}                                                      \\\"
echo \\\"  Version : ${version}                                                         \\\"
echo \\\"								                                                \\\"
echo \\\"  Installed In : $REST_PATH                                                    \\\"
echo \\\"  At : ${date}                                                                 \\\"            
echo \\\"  -----------------------------------------------------------------------------\\\"
echo \\\"  REST Support mail-list : rest-support@cern.ch                                \\\"
echo \\\"  REST Development mail-list : rest-dev@cern.ch                                \\\"
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
echo \\\"  rest-config [--exes]    : Print a list of REST executables                   \\\"
echo \\\"  rest-config [--methods] : Print a list of methods avaliable in restRoot      \\\"
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
