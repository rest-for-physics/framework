cmake_minimum_required(VERSION 2.8)
project(myREST)
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH})
include(FindROOT)
include(MacroRootDict)
include(FindREST)
include_directories(${ROOT_INCLUDE_DIRS})
include_directories(${REST_INCLUDE_DIRS})
SET(CMAKE_CXX_FLAGS "-std=c++11 -Wall -march=native")
set(CMAKE_INSTALL_PREFIX ${REST_INSTLL_PATH})
message ("oo Install prefix : ${CMAKE_INSTALL_PREFIX}\n" )

#Generate CINT file
set(CLASS_DICT mySignalProcess myMetadata )

file(GLOB MyRESTLibrarySrc
        "src/*.cxx"
        )

foreach (CLASS ${CLASS_DICT})
    set(ROOT_DICT_INCLUDE_DIRS ${REST_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS})
    set(ROOT_DICT_INPUT_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/inc/${CLASS}.h)
    GEN_ROOT_DICT_SOURCES(${CLASS}.cxx)
    LIST(APPEND MyRESTLibrarySrc ${ROOT_DICT_OUTPUT_SOURCES})
endforeach (CLASS)

include_directories(inc)
add_library(MyRESTLibrary SHARED ${MyRESTLibrarySrc})
set(target_libraries ${target_libraries} ${REST_LIBRARIES} -lGui ${ROOT_LIBRARIES} -lEve -lGeom)
target_link_libraries(MyRESTLibrary ${target_libraries})

install(TARGETS MyRESTLibrary
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib/static)
