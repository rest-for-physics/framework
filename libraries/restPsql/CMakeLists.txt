#----------------------------------------------------------------------------
# Setup the project
set(projname RestPsql)
project(${projname})
message("===============  ${projname}  ==============")
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../cmake ${CMAKE_MODULE_PATH})
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED YES)

# Find additional package
include(FindPostgreSQL)
message(STATUS "PGSQL include: " ${PostgreSQL_INCLUDE_DIRS})
message(STATUS "PGSQL lib: " ${PostgreSQL_LIBRARIES})
set(Depend_Inc ${PostgreSQL_INCLUDE_DIRS})
set(Depend_Lib ${PostgreSQL_LIBRARIES})

# Library source files
file(GLOB sources ${PROJECT_SOURCE_DIR}/source/*cc)
set(Lib_Source ${sources})

# Executables source files
set(Exe_Source)

# Customize directories to install
set(Copy_Source)

# Whether to generate dictionary with ROOTCINT
set(GenDict TRUE)

include(CompileProj)
COMPILEPROJ(${projname})

set(rest_include_dirs ${rest_include_dirs} ${PROJECT_SOURCE_DIR}/source)
set(rest_include_dirs ${rest_include_dirs} PARENT_SCOPE)
	
set(rest_libraries ${rest_libraries} ${projname})
set(rest_libraries ${rest_libraries} PARENT_SCOPE)