cmake_minimum_required(VERSION 2.8)

project(REST)
set(CMAKE_INSTALL_PREFIX .)
message("CIP = ${CMAKE_INSTALL_PREFIX} (Install Path.)")

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH})
include(FindROOT)
include(MacroRootDict)
include_directories(${ROOT_INCLUDE_DIRS})


SET(CMAKE_CXX_FLAGS "-std=c++11 -Wall -march=native")

set(BUILD_GARFIELD)
if (NOT ${BUILD_GARFIELD})
    include(FindGarfield)
    include_directories(${Garfield_INCLUDE_DIRS})
else ()
    add_subdirectory(garfield)
    include_directories(garfield/Include)
    set(Garfield_LIBRARIES Garfield)
endif ()

add_subdirectory(source)

install(PROGRAMS scripts/thisREST.sh DESTINATION .)
