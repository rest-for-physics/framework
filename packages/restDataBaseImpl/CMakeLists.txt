#----------------------------------------------------------------------------
# Setup the project
cmake_minimum_required(VERSION 2.6 FATAL_ERROR)
project(restDataBaseIO)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH})

IF(DEFINED ENV{REST_PATH})
    message("Using environment REST_PATH variable $ENV{REST_PATH}")
    set(REST_PATH $ENV{REST_PATH})
ELSE()
    message("ERROR ::: REST_PATH must be defined as an environment variable and point to REST install directory")
    return()
ENDIF()


SET(CMAKE_CXX_FLAGS "-std=c++1y")

include(FindPostgreSQL)
include(FindROOT)

include(MacroRootDict)

include_directories(${PROJECT_SOURCE_DIR}/source ${ROOT_INCLUDE_DIRS} ${REST_PATH}/include ${PostgreSQL_INCLUDE_DIRS})

file(GLOB headers ${PROJECT_SOURCE_DIR}/source/TRestDataBase.hh)

foreach (header ${headers})
set(ROOT_DICT_INPUT_HEADERS ${header})
set(ROOT_DICT_INCLUDE_DIRS ${PostgreSQL_INCLUDE_DIRS} ${PROJECT_SOURCE_DIR}/source/)
GEN_ROOT_DICT_SOURCES(CINT_${header}.cxx)
set(newsources ${newsources} ${ROOT_DICT_OUTPUT_SOURCES})
endforeach (header ${headers})

file(GLOB sources ${PROJECT_SOURCE_DIR}/source/*cc)

add_library(restDataBaseIO SHARED ${sources} ${newsources})
target_link_libraries(restDataBaseIO ${PostgreSQL_LIBRARIES} -L${REST_PATH}/lib/ -lRestTools -lRestCore -lRestEvents -lRestMetadata ${ROOT_LIBRARIES} -lGui -lEve -lRGL -lGeom -lGdml -lMinuit)


#----------------------------------------------------------------------------
# Install the executable to 'lib' directory under REST_PATH
#
install(TARGETS restDataBaseIO DESTINATION ${REST_PATH}/lib)

install(CODE
"
file(GLOB PCMFiles \"\${CMAKE_CURRENT_SOURCE_DIR}/rootdict/*.pcm\")
file(COPY \${PCMFiles} DESTINATION \${REST_PATH}/lib)

"

)