PROJECT(REST)
CMAKE_MINIMUM_REQUIRED(VERSION 3.8)


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

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED NO)

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
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH})
set(ROOT_FIND_REQUIRED true)
set(CMAKE_MACOSX_RPATH 1)
include(FindROOT)
set(external_include_dirs ${external_include_dirs} ${ROOT_INCLUDE_DIRS})
set(external_libs ${external_libs} ${ROOT_LIBRARIES} -lGui -lEve -lRGL -lGeom -lGdml -lMinuit)


#find garfield libs and includes
#IF( NOT DEFINED REST_GARFIELD )
#set(REST_GARFIELD ON)
#ENDIF()

if (${REST_GARFIELD} MATCHES "ON")
    add_definitions(-DUSE_Garfield)
    include(FindGarfield)
    set(external_include_dirs ${external_include_dirs} ${Garfield_INCLUDE_DIRS})
    set(external_libs ${external_libs} ${Garfield_LIBRARIES})
else ()
    set(REST_GARFIELD OFF)
endif (${REST_GARFIELD} MATCHES "ON")


#include dir for compilation
include_directories(${external_include_dirs})
message(STATUS "external include directory: " ${external_include_dirs})
message(STATUS "external libraries to link: " ${external_libs})


#gether some infomation
string(TIMESTAMP date "%Y-%m-%d %H:%M")
message("")
include(CollectGitInfo)
message(STATUS "REST release : ${GIT_TAG}")
message(STATUS "REST date : ${GIT_DATE}")
message(STATUS "REST commit : ${GIT_COMMIT}")
message(STATUS "REST branch : ${GIT_BRANCH}")
message("")


#generate TRestVersion.h during build stage
IF (NOT CMAKE_SYSTEM_NAME MATCHES "Windows")
    add_custom_target(
            RESTVersion ALL
            COMMAND python ${CMAKE_CURRENT_SOURCE_DIR}/scripts/generateVersionHeader.py ${CMAKE_CURRENT_SOURCE_DIR}/source/restcore/inc/TRestVersion.h
            WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})

    #RESULT_VARIABLE h)
    #if(h MATCHES 0)
    #	add_definitions(-DREST_VERSION_HEADER)
    #endif()
ENDIF ()


#start compile
include(MacroRootDict)
add_subdirectory(source)
message("")
message(STATUS "rest Libraries : " ${rest_libraries_regular})
message(STATUS "rest executables: " ${rest_exes})
message(STATUS "rest macros: " ${rest_macros_str})
message("")


#compile the packages
IF (NOT DEFINED REST_G4)
    set(REST_G4 OFF)
ENDIF ()
if (${REST_G4} MATCHES "ON")
    add_subdirectory(packages/restG4)
endif (${REST_G4} MATCHES "ON")

IF (NOT DEFINED REST_DATABASE)
    set(REST_DATABASE OFF)
ENDIF ()
if (${REST_DATABASE} MATCHES "ON")
    add_subdirectory(packages/restDataBaseImpl)
endif (${REST_DATABASE} MATCHES "ON")

IF (NOT DEFINED REST_DECAY0)
    set(REST_DECAY0 OFF)
ENDIF ()
if (${REST_DECAY0} MATCHES "ON")
    add_subdirectory(packages/restDecay0)
endif (${REST_DECAY0} MATCHES "ON")

#begin installation
message("")
message("=============================")
message("REST will be installed in : ${CMAKE_INSTALL_PREFIX}")
message("Version: ${GIT_TAG}")
message("System is : ${CMAKE_SYSTEM_NAME}")
if (${REST_GARFIELD} MATCHES "ON")
    message("Garfield is : ${REST_GARFIELD}")
else ()
    message("Garfield is : ${REST_GARFIELD}. To enable run cmake adding : -DREST_GARFIELD=ON")
endif ()
message("Geant4 is : ${REST_G4}")
message("DataBase is : ${REST_DATABASE}")
message("=============================")
message("")

file(GLOB_RECURSE Headers "${CMAKE_CURRENT_SOURCE_DIR}/source/*.h")
file(GLOB_RECURSE Headers_ "${CMAKE_CURRENT_SOURCE_DIR}/source/tools/external/win/*.h")

list(REMOVE_ITEM Headers ${Headers_})

INSTALL(FILES ${Headers} DESTINATION include)

#INSTALL(FILES ${CMAKE_CURRENT_SOURCE_DIR}/CHANGELOG DESTINATION .)

INSTALL(FILES ${CMAKE_CURRENT_SOURCE_DIR}/LICENCE DESTINATION .)

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/example
        DESTINATION .
        COMPONENT install
        )

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/macros
        DESTINATION .
        COMPONENT install
        )

install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/scripts/restv2_1ToXml.py
        DESTINATION scripts
        COMPONENT install
        )

install(
        DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/data
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

include(thisREST)

#install(CODE "chmod 744 ${CMAKE_INSTALL_PREFIX}/bin/rest-config")
install(CODE "execute_process(COMMAND chmod 755 ${CMAKE_INSTALL_PREFIX}/bin/rest-config)")
install(CODE "execute_process(COMMAND chmod 755 ${CMAKE_INSTALL_PREFIX}/scripts/restv2_1ToXml.py)")
