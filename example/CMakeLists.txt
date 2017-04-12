cmake_minimum_required(VERSION 2.8)

################################################################
# Replace "myRESTLibrary" by the name of your library
# To integrate with REST framework, the name of the library
# should contain REST or Rest in its name

set( MY_LIBRARY "MyRESTLibrary" )
project( ${MY_LIBRARY} )

################################################################

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH})

################################################################
# Including external libraries
#
include(FindROOT)
include(MacroRootDict)
include(FindREST)

include_directories(${ROOT_INCLUDE_DIRS})
include_directories(${REST_INCLUDE_DIRS})
################################################################

################################################################
# Definning the install directory
#

IF(DEFINED INSTALL_PREFIX)
    message("\noo Using user install path.")
ELSE()
    message("\noo Using default install path.")
    set(INSTALL_PREFIX ${REST_INSTLL_PATH})
ENDIF()

set(CMAKE_INSTALL_PREFIX ${INSTALL_PREFIX})
################################################################

SET(CMAKE_CXX_FLAGS "-std=c++11 -Wall -march=native")

################################################################
# Adding classes to dictionary
#
file(GLOB_RECURSE classes_list "${CMAKE_SOURCE_DIR}/src/*.cxx")

foreach (file ${classes_list})
          get_filename_component (name_without_extension ${file} NAME_WE)
          set( CLASS_DICT ${CLASS_DICT} ${name_without_extension} )
endforeach()
################################################################

file(GLOB MyRESTLibrarySrc "src/*.cxx" )

message( "\n++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n
            After \"make install\" your library will be installed in ${CMAKE_INSTALL_PREFIX}\n\n

            If you want to change the installation directory execute cmake as follows: \n
            cmake -DINSTALL_PREFIX=PATH_TO_INSTALL_DIR PATH_TO_SOURCE_DIR\n\n

            IMPORTANT : make sure you add the location of the new library to the LD_LIBRARY_PATH\n
          \n++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n" )


foreach (CLASS ${CLASS_DICT})
    set(ROOT_DICT_INCLUDE_DIRS ${REST_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS})
    set(ROOT_DICT_INPUT_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/inc/${CLASS}.h)
    GEN_ROOT_DICT_SOURCES(${CLASS}.cxx)
    LIST(APPEND MyRESTLibrarySrc ${ROOT_DICT_OUTPUT_SOURCES})
endforeach (CLASS)

include_directories(inc)
add_library( ${MY_LIBRARY} SHARED ${MyRESTLibrarySrc})
set(target_libraries ${target_libraries} ${REST_LIBRARIES} -lGui ${ROOT_LIBRARIES} -lEve -lGeom)
target_link_libraries( ${MY_LIBRARY} ${target_libraries})

install(TARGETS ${MY_LIBRARY}
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib/static)
