PROJECT(REST)
CMAKE_MINIMUM_REQUIRED(VERSION 3.5)

set(REST_G4)


#minimum compiler version
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    # require at least gcc 4.8
    if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS 4.8)
        message(FATAL_ERROR "GCC version must be at least 4.8!")
    endif ()
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    # require at least clang 3.2
    if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS 3.2)
        message(FATAL_ERROR "Clang version must be at least 3.2!")
    endif ()
endif ()
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED YES)
IF (CMAKE_SYSTEM_NAME MATCHES "Darwin")
    SET(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-stdlib=libc++")
ENDIF (CMAKE_SYSTEM_NAME MATCHES "Darwin")


#install path
IF (NOT DEFINED INSTALL_PREFIX)
    IF (DEFINED ENV{REST_PATH})
        set(INSTALL_PREFIX $ENV{REST_PATH})
    else ()
        set(INSTALL_PREFIX ${CMAKE_CURRENT_SOURCE_DIR}/install)
    endif ()
ENDIF ()
get_filename_component(INSTALL_PREFIX_ABSOLUTE ${INSTALL_PREFIX} ABSOLUTE BASE_DIR "${CMAKE_BINARY_DIR}")
set(REST_PATH ${INSTALL_PREFIX_ABSOLUTE})
set(CMAKE_INSTALL_PREFIX ${INSTALL_PREFIX_ABSOLUTE})


#auto schema evolution for ROOT
IF (NOT DEFINED REST_SE)
    set(REST_SE ON)
ENDIF ()
set(SCHEMA_EVOLUTION ${REST_SE})

#welcome message
IF (NOT DEFINED REST_WELCOME)
    set(REST_WELCOME ON)
ENDIF ()

#find ROOT libs and includes
set(external_include_dirs)
set(external_libs)
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake ${DECAY_PATH}/cmake ${CMAKE_MODULE_PATH})
set(ROOT_FIND_REQUIRED true)
set(CMAKE_MACOSX_RPATH 1)

include(FindROOT)
set(external_include_dirs ${external_include_dirs} ${ROOT_INCLUDE_DIRS})
set(external_libs ${external_libs} "${ROOT_LIBRARIES} -lGui -lGeom -lGdml -lMinuit -lSpectrum")


#switch to enable ROOT Eve functionality.
IF( NOT DEFINED REST_EVE)
   set(REST_EVE ON)
ENDIF()
if (${REST_EVE} MATCHES "ON")
    add_definitions(-DUSE_Eve)
	list( APPEND rest_features "Eve" )
    set(external_libs "${external_libs} -lEve -lRGL")
else()
    set(excludes ${excludes} TRestEveEventViewer )
endif (${REST_EVE} MATCHES "ON")

#this is to enable linking of SQL libraries. I.e. for process TRestSQLToAnalysisProccess.
IF (NOT DEFINED SQL)
    set(SQL OFF)
ENDIF ()

if (${SQL} MATCHES "ON" )
	list( APPEND rest_features "SQL" )
	add_definitions(-DUSE_SQL)
	include(FindSQL)
	set(external_include_dirs ${external_include_dirs} ${SQL_INCLUDE_DIR})
	set(external_libs "${external_libs} ${SQL_LIBRARIES}")
	message(STATUS "SQL include directory: " ${SQL_INCLUDE_DIR})
	set(SQL ON)
else()
	set(SQL OFF)
endif (${SQL} MATCHES "ON" )

#include dir for compilation
include_directories(${external_include_dirs})
message(STATUS "external include directory: " ${external_include_dirs})
message(STATUS "external libraries to link: " ${external_libs})

#gether some infomation
string(TIMESTAMP date "%Y-%m-%d %H:%M")

#start compile
include(MacroRootDict)
add_subdirectory(source)
message("")
message(STATUS "rest Libraries : " ${rest_libraries_regular})
message(STATUS "rest executables: " ${rest_exes})
message(STATUS "rest macros: " ${rest_macros_str})
message("")

#begin installation
message("")
message("REST will be installed in : ${CMAKE_INSTALL_PREFIX}")
message("===========================================================")
message("System is : ${CMAKE_SYSTEM_NAME}")
message("Packages : ${rest_packages}")
message("Libraries : ${rest_libraries}" )
message("Features : ${rest_features}" )

message("===========================================================")
message("Check main project README.md to see all available compilation options, packages and libraries")
message("")

file(GLOB_RECURSE Headers "${CMAKE_CURRENT_SOURCE_DIR}/source/framework/*.h")
#set(Headers ${Headers} ${Headers1} ${Headers2} ${Headers3} ${Headers4} ${Headers5} )

# clear the install dir
IF (NOT DEFINED INSTALL_CLEARDIR)
    set(INSTALL_CLEARDIR ON)
ENDIF ()
if (${INSTALL_CLEARDIR} MATCHES "ON")
  install(CODE "execute_process(COMMAND rm -r ${CMAKE_INSTALL_PREFIX})")
endif()

# install the files of the main framework
INSTALL(FILES ${Headers} DESTINATION include)

INSTALL(FILES ${CMAKE_CURRENT_SOURCE_DIR}/LICENCE DESTINATION .)

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/examples
        DESTINATION .
        COMPONENT install
        )

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/macros
        DESTINATION .
        COMPONENT install
        )

install(
        DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/data
        DESTINATION .
        COMPONENT install
)

#copy pcm files
install(CODE
        "
file(GLOB PCMFiles \"\${CMAKE_CURRENT_SOURCE_DIR}/rootdict/*.pcm\")
file(COPY \${PCMFiles} DESTINATION \${CMAKE_INSTALL_PREFIX}/lib)
"
        )

#create thisREST.sh
include(thisREST)

#add permissions
install(CODE "execute_process(COMMAND chmod 755 ${CMAKE_INSTALL_PREFIX}/bin/rest-config)")
install(CODE "execute_process(COMMAND chmod 777 ${CMAKE_INSTALL_PREFIX})")
