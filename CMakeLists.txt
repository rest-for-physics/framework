CMAKE_MINIMUM_REQUIRED(VERSION 3.16)

PROJECT(REST)

# Minimum compiler version
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    # require at least gcc 4.8
    if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS 4.8)
        message(FATAL_ERROR "GCC version must be at least 4.8!")
    endif ()
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    # require at least clang 3.2
    if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS 3.2)
        message(FATAL_ERROR "Clang version must be at least 3.2!")
    endif ()
endif ()

if (CMAKE_SYSTEM_NAME MATCHES "Darwin")
    SET(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-stdlib=libc++")
endif (CMAKE_SYSTEM_NAME MATCHES "Darwin")

# https://github.com/rest-for-physics/framework/issues/162
if (${CMAKE_VERSION} VERSION_LESS "3.21")
    cmake_policy(SET CMP0082 OLD)
endif ()

# Install path
if (NOT DEFINED INSTALL_PREFIX)
    if (DEFINED ENV{REST_PATH})
        set(INSTALL_PREFIX $ENV{REST_PATH})
    else ()
        set(INSTALL_PREFIX ${CMAKE_CURRENT_SOURCE_DIR}/install)
    endif ()
endif ()
get_filename_component(INSTALL_PREFIX_ABSOLUTE ${INSTALL_PREFIX} ABSOLUTE BASE_DIR "${CMAKE_BINARY_DIR}")
set(REST_PATH ${INSTALL_PREFIX_ABSOLUTE})
if (NOT DEFINED CMAKE_INSTALL_PREFIX)
    set(CMAKE_INSTALL_PREFIX ${INSTALL_PREFIX_ABSOLUTE})
endif ()

if (NOT DEFINED RESTLIB_MODEL)
    set(RESTLIB_MODEL OFF)
endif ()

# Auto schema evolution for ROOT
if (NOT DEFINED REST_SE)
    set(REST_SE ON)
endif ()
set(SCHEMA_EVOLUTION ${REST_SE})

# Welcome message
if (NOT DEFINED REST_WELCOME)
    set(REST_WELCOME ON)
endif ()

set(external_include_dirs)
set(external_libs)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake ${DECAY_PATH}/cmake ${CMAKE_MODULE_PATH})
set(CMAKE_MACOSX_RPATH 1)

# ROOT libs and includes

find_package(ROOT REQUIRED)

set(ROOT_REQUIRED_LIBRARIES Core RIO Geom Gdml Minuit Spectrum XMLIO)
# Switch to enable ROOT Eve functionality.
if (NOT DEFINED REST_EVE)
    set(REST_EVE ON)
endif ()
if (${REST_EVE} MATCHES "ON")
    add_definitions(-DUSE_Eve)
    list(APPEND rest_features "Eve")
    set(ROOT_REQUIRED_LIBRARIES ${ROOT_REQUIRED_LIBRARIES} Eve RGL)
else ()
    set(excludes ${excludes} TRestEveEventViewer)
endif (${REST_EVE} MATCHES "ON")

find_package(ROOT REQUIRED COMPONENTS ${ROOT_REQUIRED_LIBRARIES})

message(STATUS "ROOT LIBRARIES: ${ROOT_LIBRARIES}")

set(external_libs "${external_libs};${ROOT_LIBRARIES}")

set(ROOTCINT_EXECUTABLE ${ROOT_rootcint_CMD})

execute_process(COMMAND root-config --has-cxx20 OUTPUT_VARIABLE HASCXX20)
execute_process(COMMAND root-config --has-cxx17 OUTPUT_VARIABLE HASCXX17)
execute_process(COMMAND root-config --has-cxx14 OUTPUT_VARIABLE HASCXX14)
execute_process(COMMAND root-config --has-cxx11 OUTPUT_VARIABLE HASCXX11)

set(CMAKE_CXX_STANDARD_REQUIRED YES)

if (${HASCXX20} MATCHES "yes")
    set(CMAKE_CXX_STANDARD 20)
elseif (${HASCXX17} MATCHES "yes")
    set(CMAKE_CXX_STANDARD 17)
elseif (${HASCXX14} MATCHES "yes")
    set(CMAKE_CXX_STANDARD 14)
elseif (${HASCXX11} MATCHES "yes")
    set(CMAKE_CXX_STANDARD 11)
endif ()

set(external_include_dirs ${external_include_dirs} ${ROOT_INCLUDE_DIRS})

# This is to enable linking of SQL libraries. I.e. for process TRestSQLToAnalysisProcess.
if (NOT DEFINED SQL)
    set(SQL OFF)
endif ()

if (${SQL} MATCHES "ON")
    list(APPEND rest_features "SQL")
    add_definitions(-DUSE_SQL)
    include(FindSQL)
    set(external_include_dirs ${external_include_dirs} ${SQL_INCLUDE_DIR})
    set(external_libs "${external_libs} ${SQL_LIBRARIES}")
    message(STATUS "SQL include directory: " ${SQL_INCLUDE_DIR})
    set(SQL ON)
else ()
    set(SQL OFF)
endif (${SQL} MATCHES "ON")

# Include directories for compilation
include_directories(${external_include_dirs})
message(STATUS "external include directory: " ${external_include_dirs})
message(STATUS "external libraries to link: " ${external_libs})

# Gather some information
string(TIMESTAMP date "%Y-%m-%d %H:%M")

# Enable testing (-DTEST=ON flag, it is OFF by default)
include(Testing)

# Start compile
include(MacroRootDict)
add_subdirectory(source)

message("")
message(STATUS "rest Libraries: " ${rest_libraries_regular})
message(STATUS "rest executables: " ${rest_exes})
message(STATUS "rest macros: " ${rest_macros_str})
message("")

# Begin installation
message("")
message("REST will be installed in: ${CMAKE_INSTALL_PREFIX}")
message("===========================================================")
message("System is: ${CMAKE_SYSTEM_NAME}")
message("Packages: ${rest_packages}")
message("Libraries: ${rest_libraries}")
message("Features: ${rest_features}")
message("===========================================================")
message("Check main project README.md to see all available compilation options, packages and libraries")
message("")

file(GLOB_RECURSE Headers "${CMAKE_CURRENT_SOURCE_DIR}/source/framework/*.h")

# Clear the install dir
if (NOT DEFINED INSTALL_CLEARDIR)
    set(INSTALL_CLEARDIR ON)
endif ()
if (${INSTALL_CLEARDIR} MATCHES "ON")
    install(CODE "execute_process(COMMAND rm -r ${CMAKE_INSTALL_PREFIX})")
endif ()

# Install the files of the main framework
install(FILES ${Headers} DESTINATION include)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/LICENCE DESTINATION .)
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/examples
        DESTINATION .
        COMPONENT install
        )
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/macros
        DESTINATION .
        COMPONENT install
        )
install(
        DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/data
        DESTINATION .
        COMPONENT install
)
install(
        DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/cmake/scripts
        DESTINATION .
        COMPONENT install
)
file(GLOB cmake_scripts
        "cmake/scripts/*"
        )
install(FILES ${cmake_scripts} DESTINATION cmake)
# Copy pcm files
install(CODE
        "
file(GLOB PCMFiles \"\${CMAKE_CURRENT_SOURCE_DIR}/rootdict/*.pcm\")
file(COPY \${PCMFiles} DESTINATION \${CMAKE_INSTALL_PREFIX}/lib)
"
        )

# Create thisREST.sh
include(thisREST)

# Add permissions
install(CODE "execute_process(COMMAND chmod 755 ${CMAKE_INSTALL_PREFIX}/bin/rest-config)")
install(CODE "execute_process(COMMAND chmod 755 ${CMAKE_INSTALL_PREFIX})")
