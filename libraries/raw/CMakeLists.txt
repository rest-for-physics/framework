#----------------------------------------------------------------------------
# Setup the project
set(projname RestRaw)
project(${projname})
message("===============  ${projname}  ==============")
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../cmake ${CMAKE_MODULE_PATH})
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED YES)

# Find additional package
set(Depend_Inc)
set(Depend_Lib)

# Library source files
file(GLOB sources ${PROJECT_SOURCE_DIR}/src/*.cxx)
set(Lib_Source ${sources})

# Executables source files
set(Exe_Source)

# Customize directories to install
set(Copy_Source data examples macros)

# Whether to generate dictionary with ROOTCINT
set(GenDict TRUE)

include(CompileProj)
COMPILEPROJ(${projname})

set(rest_include_dirs ${rest_include_dirs} ${PROJECT_SOURCE_DIR}/inc)
set(rest_include_dirs ${rest_include_dirs} PARENT_SCOPE)
	
set(rest_libraries ${rest_libraries} ${projname})
set(rest_libraries ${rest_libraries} PARENT_SCOPE)
