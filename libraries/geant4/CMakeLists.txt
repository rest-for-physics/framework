#----------------------------------------------------------------------------
# Setup the project
cmake_minimum_required(VERSION 2.6 FATAL_ERROR)
set(CMAKE_MACOSX_RPATH 1)

## --------------------------------------------------------------------------
## ---------- ONLY need to change THIS_LIBRARY and LibraryVersion -----------
## ---------- to adapt this dummy CMakeLists to a new library name.  --------
## ---------- The version value should be assigned to any metadata ----------
## ---------- structure inside the Initialize method              -----------
## ----------
## ---------- THIS_LIBRARY should contain `Rest` or `REST` in the -----------
## ---------- name to be recognized by TRestTools::GetListOfRestLibraries. --
## ---------- You might use any string you like in the version field --------
## ----------
set( THIS_LIBRARY "RestGeant4" )
set( LibraryVersion "1.1" )
## ----------
## --------------------------------------------------------------------------

add_definitions(-DLIBRARY_VERSION="${LibraryVersion}")
project(${THIS_LIBRARY} CXX)
message("===============  ${THIS_LIBRARY}  ==============")


set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH})

# find REST
IF(NOT DEFINED REST_PATH)
	IF(DEFINED ENV{REST_PATH})
		set(REST_PATH $ENV{REST_PATH})
	ELSE()
		message(FATAL_ERROR "ERROR ::: REST_PATH must be defined as an environment variable and point to REST install directory")
		return()
	ENDIF()
ENDIF()
set(CMAKE_INSTALL_PREFIX ${REST_PATH})
message(STATUS "Using REST PATH: " ${REST_PATH})

SET(CMAKE_CXX_FLAGS "-std=c++1y -Wno-deprecated")

#----------------------------------------------------------------------------
# Find package ROOT
include(FindROOT)
include(MacroRootDict)


#----------------------------------------------------------------------------
# Set include and lib
if(DEFINED rest_include_dirs)
    set(incdir ${PROJECT_SOURCE_DIR}/inc ${ROOT_INCLUDE_DIRS} ${rest_include_dirs} /usr/local/include)
else()
    set(incdir ${PROJECT_SOURCE_DIR}/inc ${ROOT_INCLUDE_DIRS} ${REST_PATH}/include /usr/local/include)
endif()

if(DEFINED rest_libraries)
    set(lnklib ${rest_libraries} ${ROOT_LIBRARIES} -L/usr/local/lib -lGui -lEve -lRGL -lGeom -lGdml -lMinuit ${external_libs})
	list( APPEND rest_libraries ${THIS_LIBRARY} )
else()
    set(lnklib -L/usr/local/lib -L${REST_PATH}/lib/ -lRestTools -lRestCore -lRestEvents -lRestMetadata ${ROOT_LIBRARIES} -lGui -lEve -lRGL -lGeom -lGdml -lMinuit ${external_libs})
message( "\n++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n
          After \"make install\" this package will add additional library in ${REST_PATH}/lib\n\n
          This will take effect in the next launch of restManager/restRoot
          \n++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n" )
endif()

set(incdir ${incdir} ${external_include_dirs})
include_directories(${incdir})

#----------------------------------------------------------------------------
# Locate sources and headers for this project
#
file(GLOB sources ${PROJECT_SOURCE_DIR}/src/*.cxx)
file(GLOB headers ${PROJECT_SOURCE_DIR}/inc/*.h)
foreach (header ${headers})
	get_filename_component(barename ${header} NAME)
	string(REPLACE ".h" "" class ${barename})

	set(ROOT_DICT_INCLUDE_DIRS ${incdir})
	set(ROOT_DICT_INPUT_HEADERS ${header} ${ROOT_DICT_OUTPUT_DIR}/${class}_LinkDef.h)
	GEN_ROOT_DICT_LINKDEF_HEADER( ${class} ${header})
	GEN_ROOT_DICT_SOURCES(CINT_${class}.cxx ${ROOT_DICT_OUTPUT_DIR}/${class}_LinkDef.h)
	set(newsources ${newsources} ${ROOT_DICT_OUTPUT_SOURCES})
endforeach (header ${headers})

#----------------------------------------------------------------------------
# Add the library, and link it to the Geant4 libraries
#
add_library(${THIS_LIBRARY} SHARED ${sources} ${newsources})
target_link_libraries(${THIS_LIBRARY} ${lnklib})


#----------------------------------------------------------------------------
# Install the library to 'lib' directory under REST_PATH
#
#begin installation
install(TARGETS ${THIS_LIBRARY} DESTINATION lib)

file(GLOB_RECURSE Headers "${CMAKE_CURRENT_SOURCE_DIR}/*.h")

INSTALL(FILES ${Headers} DESTINATION include)

install(CODE
"
file(GLOB PCMFiles \"\${CMAKE_CURRENT_SOURCE_DIR}/rootdict/*.pcm\")
file(COPY \${PCMFiles} DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
"

)

#
# USE this if the library contains data directory that needs to be placed
# at the install directory REST_PATH/data/geant4
#
#install(
#	DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/data
#    DESTINATION .
#    COMPONENT install
#        )

#----------------------------------------------------------------------------
# Example programs for this library
#----------------------------------------------------------------------------
#add_subdirectory(examples)
