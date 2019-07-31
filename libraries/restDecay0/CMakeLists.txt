#----------------------------------------------------------------------------
# Setup the project
set(projname RestDecay0)
project(${projname})
message("===============  ${projname}  ==============")
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../cmake ${CMAKE_MODULE_PATH})
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED YES)

# Find additional package
find_package(BxDecay0 1.0 CONFIG COMPONENTS manager REQUIRED)
message (STATUS "BxDecay0_VERSION       ='${BxDecay0_VERSION}'")
message (STATUS "BxDecay0_INCLUDE_DIRS  ='${BxDecay0_INCLUDE_DIRS}'")
message (STATUS "BxDecay0_LIBRARIES     ='${BxDecay0_LIBRARIES}'")
set(Depend_Inc ${BxDecay0_INCLUDE_DIRS})
set(Depend_Lib ${BxDecay0_LIBRARIES})

# Library source files
set(Lib_Source TRestGeneratorInfoDecay0.cc)

# Executables source files
set(Exe_Source)

# Customize directories to install
set(Copy_Source)

# Whether to generate dictionary with ROOTCINT
set(GenDict TRUE)

include(CompileProj)
COMPILEPROJ(${projname})

set(rest_include_dirs ${rest_include_dirs} ${PROJECT_SOURCE_DIR})
set(rest_include_dirs ${rest_include_dirs} PARENT_SCOPE)
	
set(rest_libraries ${rest_libraries} ${projname})
set(rest_libraries ${rest_libraries} PARENT_SCOPE)


