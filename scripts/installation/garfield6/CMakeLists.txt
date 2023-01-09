# ##############################################################################
# build scripts to run garfield simulations depend on: ROOT @author Klaus Zenker
cmake_minimum_required(VERSION 2.6 FATAL_ERROR)
# ##############################################################################

project(Garfield CXX Fortran)

# DEPENDENCIES #######################################
if(DEFINED ENV{ROOTSYS})
    find_program(ROOTCINT_EXECUTABLE rootcint PATHS $ENV{ROOTSYS}/bin)
    set(CMAKE_MODULE_PATH $ENV{ROOTSYS}/etc/cmake)
elseif(DEFINED ROOTSYS)
    find_program(ROOTCINT_EXECUTABLE rootcint PATHS ${ROOTSYS}/bin)
    set(CMAKE_MODULE_PATH ${ROOTSYS}/etc/cmake)
else()
    message(
        FATAL_ERROR
            "Please consider to set ROOTSYS or use -DROOTSYS=..."
            "If still there is a proplem point to the directory which includes FindROOT.cmake using -DROOT_CMAKE_DIR=..."
    )
endif(DEFINED ENV{ROOTSYS})

find_package(ROOT 5.0 REQUIRED COMPONENTS Geom)
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/CMake)
include(MacroCheckPackageLibs)
include(MacroRootDict)

# If you cannot create symbolic links turn FALSE the library version
set(BUILD_WITH_VERSION_NUMBER TRUE)
if(BUILD_WITH_VERSION_NUMBER)
    set(${PROJECT_NAME}_MAJOR_VERSION 0)
    set(${PROJECT_NAME}_MINOR_VERSION 2)
    set(${PROJECT_NAME}_PATCH_VERSION 0)
    set(${PROJECT_NAME}_VERSION
        ${${PROJECT_NAME}_MAJOR_VERSION}.${${PROJECT_NAME}_MINOR_VERSION}.${${PROJECT_NAME}_PATCH_VERSION}
    )
    set(${PROJECT_NAME}_SOVERSION ${${PROJECT_NAME}_VERSION})
    include(MacroCheckPackageVersion)
endif(BUILD_WITH_VERSION_NUMBER)

# Add root libs ##
message(" Root libraries: ${ROOT_LIBRARIES} ")
if(DEFINED ROOT_INCLUDE_DIRS)
    # ROOT 6
    include_directories(${PROJECT_SOURCE_DIR} ${ROOT_INCLUDE_DIRS})
else()
    # ROOT 5
    include_directories(${PROJECT_SOURCE_DIR} ${ROOT_INCLUDE_DIR})
    # COMPONENT mechanism in the FIND_PACKAGE is not supported
    set(ROOT_COMPONENT_LIBRARIES "-lGeom")
endif(DEFINED ROOT_INCLUDE_DIRS)

# add garfield  #######################################
aux_source_directory(Source garfield_sources)
list(APPEND ROOT_DICT_INCLUDE_DIRS Include)

# add magboltz ########################################
set(magboltz_sources Source/magboltz-9.01.f)
set_source_files_properties(${magboltz_sources} PROPERTIES LANGUAGE Fortran)

# add Heed ############################################
set(heed_dirs
    Heed/heed++/code
    Heed/wcpplib/clhep_units
    Heed/wcpplib/geometry
    Heed/wcpplib/ioniz
    Heed/wcpplib/math
    Heed/wcpplib/matrix
    Heed/wcpplib/matter
    Heed/wcpplib/particle
    Heed/wcpplib/random
    Heed/wcpplib/safetl
    Heed/wcpplib/stream
    Heed/wcpplib/util)
foreach(heed_dir ${heed_dirs})
    aux_source_directory(${heed_dir} heed_sources)
endforeach()
# Don't build HeedParticle_BGM.c.o because it will not compile
list(REMOVE_ITEM heed_sources Heed/heed++/code/HeedParticle_BGM.c)
set_source_files_properties(${heed_sources} PROPERTIES LANGUAGE CXX)

# Flags to pass to the compiler #######################
set(CMAKE_CXX_FLAGS
    "-Wall -Wextra -pedantic -ansi -Wabi -Wno-long-long -Woverloaded-virtual -fno-common -pg "
)

# Add common flags for gfortran and g++
add_definitions("-c")
# Allow to use debug symbols
if(CMAKE_BUILD_TYPE STREQUAL "Debug" OR CMAKE_BUILD_TYPE STREQUAL
                                        "RelWithDebInfo")
    add_definitions("-ggdb -O0")
else()
    add_definitions("-O2")
endif()
if(ROOT_VERSION VERSION_GREATER 6.0)
    add_definitions("-std=c++1y")
endif()

# Add switches used in HEED -DNOT_INCLUDE_GASLIB_IN_PACSLIB is used in
# Heed/heed++/code/PhotoAbsCS.c
set_source_files_properties(
    ${heed_sources}
    PROPERTIES
        COMPILE_FLAGS
        "-DNOT_INCLUDE_GASLIB_IN_PACSLIB -DGARFIELD_HEED_INTERFACE -DUSE_SRANLUX -DEXCLUDE_FUNCTIONS_WITH_HISTDEF -DINS_CRETURN -DFUNNAMESTACK"
)

# create dictionary  ##################################
set(ROOT_DICT_CINT_DEFINITIONS "-DHANDLE_DICT_EXCEPTIONS=IGNORED_FOR_CINT")
prepare_root_dict_headers(Include)
gen_root_dict_sources(GarfieldDict.cxx)
list(APPEND garfield_sources ${ROOT_DICT_OUTPUT_SOURCES})

message(STATUS "###########################################################")
message(STATUS "Considered Garfield files :             ${garfield_sources}")
message(STATUS "-----------------------------------------------------------")
message(STATUS "Considered heed files :                 ${heed_sources}")
message(STATUS "-----------------------------------------------------------")
message(STATUS "Considered magboltz files :             ${magboltz_sources}")
message(STATUS "-----------------------------------------------------------")
message(
    STATUS "Directory used to find root dict files: ${ROOT_DICT_INCLUDE_DIRS}")
message(STATUS "###########################################################")

# SET output path for libraries
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib/)
# SET include directories
include_directories(${ROOT_DICT_INCLUDE_DIRS} Heed)
# build the library
add_library(${PROJECT_NAME} SHARED ${garfield_sources} ${magboltz_sources}
                                   ${heed_sources})
target_link_libraries(${PROJECT_NAME} ${ROOT_LIBRARIES}
                      ${ROOT_COMPONENT_LIBRARIES})
if(BUILD_WITH_VERSION_NUMBER)
    set_target_properties(
        ${PROJECT_NAME} PROPERTIES VERSION ${${PROJECT_NAME}_VERSION}
                                   SOVERSION ${${PROJECT_NAME}_SOVERSION})
endif(BUILD_WITH_VERSION_NUMBER)
# EXPORT PACKAGE INFORMATION ##########################
export(PACKAGE ${PROJECT_NAME})

# Configure the config files
configure_file(CMake/GarfieldConfig.cmake.in
               "${PROJECT_BINARY_DIR}/GarfieldConfig.cmake" @ONLY)
configure_file(CMake/GarfieldConfigVersion.cmake.in
               "${PROJECT_BINARY_DIR}/GarfieldConfigVersion.cmake" @ONLY)

message(STATUS "Install path: ${CMAKE_INSTALL_PREFIX}")
message(
    STATUS
        "If you want to change this path call cmake -DCMAKE_INSTALL_PREFIX=my_install_path ..."
)
set(CMAKE_INSTALL_RPATH ${CMAKE_INSTALL_PREFIX})

# decide were the library will end up and to write info in
# GarfieldLibraryDepends.cmake
install(
    TARGETS ${PROJECT_NAME}
    DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/
    EXPORT GarfieldLibraryDepends)

# decide were to install GarfieldLibraryDepends.camke
install(
    EXPORT GarfieldLibraryDepends
    DESTINATION ${CMAKE_INSTALL_PREFIX}/cmake
    COMPONENT cmakefiles)

# now install cofigured config files
install(
    FILES ${PROJECT_BINARY_DIR}/GarfieldConfig.cmake
          ${PROJECT_BINARY_DIR}/GarfieldConfigVersion.cmake
    DESTINATION ${CMAKE_INSTALL_PREFIX}/cmake
    COMPONENT cmakefiles)
# install pcm file in case of root 6
if(ROOT_VERSION VERSION_GREATER 6.0)
    install(
        FILES ${PROJECT_BINARY_DIR}/rootdict/${PROJECT_NAME}Dict_rdict.pcm
        DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/
        COMPONENT dict)
endif()
