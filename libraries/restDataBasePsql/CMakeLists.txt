#----------------------------------------------------------------------------
# Setup the project
cmake_minimum_required(VERSION 2.6 FATAL_ERROR)
project(RestDataBasePsql)
message("===============  RestDataBasePsql  ==============")
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../cmake ${CMAKE_MODULE_PATH})

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

SET(CMAKE_CXX_FLAGS "-std=c++1y")

#----------------------------------------------------------------------------
# Find package PostgreSQL and ROOT
include(FindPostgreSQL)
message(STATUS "PGSQL include: " ${PostgreSQL_INCLUDE_DIRS})
message(STATUS "PGSQL lib: " ${PostgreSQL_LIBRARIES})
include(FindROOT)
include(MacroRootDict)


#----------------------------------------------------------------------------
# Set include and lib
if(DEFINED rest_include_dirs)
set(incdir ${PROJECT_SOURCE_DIR}/source ${ROOT_INCLUDE_DIRS} ${rest_include_dirs} ${PostgreSQL_INCLUDE_DIRS})
else()
set(incdir ${PROJECT_SOURCE_DIR}/source ${ROOT_INCLUDE_DIRS} ${REST_PATH}/include ${PostgreSQL_INCLUDE_DIRS})
endif()

if(DEFINED rest_libraries)
set(lnklib ${PostgreSQL_LIBRARIES} ${rest_libraries} ${ROOT_LIBRARIES} -lGui -lEve -lRGL -lGeom -lGdml -lMinuit)
else()
set(lnklib ${PostgreSQL_LIBRARIES} -L${REST_PATH}/lib/ -lRestTools -lRestCore -lRestEvents -lRestMetadata ${ROOT_LIBRARIES} -lGui -lEve -lRGL -lGeom -lGdml -lMinuit)
message( "\n++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n
          After \"make install\" RestDataBasePsql will be installed in ${REST_PATH}/lib\n\n
          It will take effect in the next launch of restManager/restRoot
          \n++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n" )
endif()

include_directories(${incdir})

#----------------------------------------------------------------------------
# Locate sources and headers for this project
#
file(GLOB sources ${PROJECT_SOURCE_DIR}/source/*cc)
file(GLOB headers ${PROJECT_SOURCE_DIR}/source/TRestDataBasePsql.hh)
foreach (header ${headers})
	string(REGEX MATCH "[^/\\]*hh" temp ${header})
	string(REPLACE ".hh" "" class ${temp})

	set(ROOT_DICT_INCLUDE_DIRS ${incdir})
	set(ROOT_DICT_INPUT_HEADERS ${header} ${ROOT_DICT_OUTPUT_DIR}/${class}_LinkDef.h)
	GEN_ROOT_DICT_LINKDEF_HEADER( ${class} ${header})
	GEN_ROOT_DICT_SOURCES(CINT_${class}.cxx ${ROOT_DICT_OUTPUT_DIR}/${class}_LinkDef.h)
	set(newsources ${newsources} ${ROOT_DICT_OUTPUT_SOURCES})
endforeach (header ${headers})

#----------------------------------------------------------------------------
# Add the library, and link it to the Geant4 libraries
#
add_library(RestDataBasePsql SHARED ${sources} ${newsources})
target_link_libraries(RestDataBasePsql ${lnklib})


#----------------------------------------------------------------------------
# Install the library to 'lib' directory under REST_PATH
#
install(TARGETS RestDataBasePsql DESTINATION lib)

install(CODE
"
file(GLOB PCMFiles \"\${CMAKE_CURRENT_SOURCE_DIR}/rootdict/*.pcm\")
file(COPY \${PCMFiles} DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)

"

)