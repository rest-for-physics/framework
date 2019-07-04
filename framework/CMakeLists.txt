#----------------------------------------------------------------------------
# Setup the project
set(projname RestFramework)
project(${projname})
message("===============  ${projname}  ==============")
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../cmake ${CMAKE_MODULE_PATH})
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED YES)

# Find additional package
set(Depend_Inc)
set(Depend_Lib)

# Library source files
file(GLOB_RECURSE sources1 ${PROJECT_SOURCE_DIR}/tools/*cxx)

file(GLOB_RECURSE sources3 ${PROJECT_SOURCE_DIR}/core/*cxx)
file(GLOB_RECURSE sources4
        "trackMinimization.c"
        "allocrus.c"
        "edgelen.c"
        "edgeutil.c"
        "heldkarp.c"
		"tiny*.cpp")
set(Lib_Source ${sources1} ${sources2} ${sources3} ${sources4})

# Executables source files
set(Exe_Source bin/restManager.cxx bin/restRoot.cxx)

# Customize directories to install
set(Copy_Source macros)

# Whether to generate dictionary with ROOTCINT
set(GenDict TRUE)

include(CompileProj)
COMPILEPROJ(${projname})




set(rest_include_dirs ${PROJECT_SOURCE_DIR}/external/tinyxml/ ${rest_include_dirs})
set(rest_include_dirs ${PROJECT_SOURCE_DIR}/external/tsp/inc ${rest_include_dirs})
set(rest_include_dirs ${PROJECT_SOURCE_DIR}/tools/inc ${rest_include_dirs})
set(rest_include_dirs ${PROJECT_SOURCE_DIR}/core/inc ${rest_include_dirs})
set(rest_include_dirs ${rest_include_dirs} PARENT_SCOPE)

set(rest_libraries ${projname})
set(rest_libraries ${rest_libraries} PARENT_SCOPE)

set(rest_exes ${rest_exes} restManager restRoot)
set(rest_exes ${rest_exes} PARENT_SCOPE)

set(rest_macros ${rest_macros} "restFit")
set(rest_macros ${rest_macros} "restCreateHisto")
set(rest_macros ${rest_macros} "restIntegrate")
set(rest_macros ${rest_macros} "restIntegrateSmeearing")
set(rest_macros ${rest_macros} "restViewEvents")
#set(rest_macros ${rest_macros} "restViewG4Event")
#set(rest_macros ${rest_macros} "restViewHitsEvent")
set(rest_macros ${rest_macros} "restViewReadout")
#set(rest_macros ${rest_macros} "restViewGasCurve")
set(rest_macros ${rest_macros} "restViewGeometry")
set(rest_macros ${rest_macros} "restMergeFiles")
set(rest_macros ${rest_macros} "restPrintMetadata")
set(rest_macros ${rest_macros} "restPrintFileContents")
set(rest_macros ${rest_macros} "restPrintDataBase")


set(rest_macros ${rest_macros} PARENT_SCOPE)

