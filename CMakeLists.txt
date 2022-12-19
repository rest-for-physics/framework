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
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address")
else ()
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -lstdc++fs")
endif (CMAKE_SYSTEM_NAME MATCHES "Darwin")

if ( CMAKE_BUILD_TYPE MATCHES "Release" )
	message( STATUS "Enabling warnings as errors" )
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror -Wno-cast-function-type -Wno-unused-function -Wno-unknown-pragmas -Wno-delete-non-virtual-dtor")
endif ( CMAKE_BUILD_TYPE MATCHES "Release" )

if ( CMAKE_BUILD_TYPE MATCHES "Debug" )
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
endif ( CMAKE_BUILD_TYPE MATCHES "Debug" )

# https://github.com/rest-for-physics/framework/issues/162 & https://github.com/rest-for-physics/framework/issues/236
if (${CMAKE_VERSION} VERSION_LESS "3.23")
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
if ( CMAKE_INSTALL_PREFIX STREQUAL "/usr/local" )
    message(FATAL_ERROR "An install location has not been defined!  Please, define an installation directory using -DCMAKE_INSTALL_PREFIX. Do not use a system directory as installation path (such as /usr/local/). REST has not been designed to be installed as system libraries. \n Re-run cmake again using : cmake -DCMAKE_INSTALL_PREFIX=../install ..\n")

    set(CMAKE_INSTALL_PREFIX ${INSTALL_PREFIX_ABSOLUTE})
endif ()

# Welcome message
if (NOT DEFINED REST_WELCOME)
    set(REST_WELCOME ON)
endif ()

set(external_include_dirs)
set(external_libs)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake ${DECAY_PATH}/cmake ${CMAKE_MODULE_PATH})
set(CMAKE_MACOSX_RPATH 1)

#####  ROOT libs and includes  #####
find_package(ROOT 6.24 REQUIRED)
set(ROOT_REQUIRED_LIBRARIES Core RIO Geom Gdml Minuit Spectrum XMLIO)

# Auto schema evolution for ROOT
if (NOT DEFINED REST_SE)
    set(REST_SE ON)
endif ()
set(SCHEMA_EVOLUTION ${REST_SE})

# Switch to enable ROOT Eve functionality.
if (NOT DEFINED REST_EVE)
    set(REST_EVE ON)
endif ()
if (${REST_EVE} MATCHES "ON")
    add_definitions(-DUSE_Eve)
    list(APPEND rest_features "Eve")
    set(ROOT_REQUIRED_LIBRARIES ${ROOT_REQUIRED_LIBRARIES} Eve RGL Gui)
else ()
    set(excludes ${excludes} TRestEveEventViewer)
endif (${REST_EVE} MATCHES "ON")

find_package(ROOT REQUIRED COMPONENTS ${ROOT_REQUIRED_LIBRARIES})

message(STATUS "ROOT LIBRARIES: ${ROOT_LIBRARIES}")

set(ROOTCINT_EXECUTABLE ${ROOT_rootcint_CMD})

set(external_libs "${external_libs};${ROOT_LIBRARIES}")

if (CMAKE_SYSTEM_NAME MATCHES "Windows")
    set(external_libs "${external_libs};Ws2_32")
    set(CMAKE_CXX_STANDARD 17)
    add_definitions(-D_HAS_STD_BYTE=0)
else ()

    if (NOT CMAKE_SYSTEM_NAME MATCHES "Darwin")
        set(external_libs "${external_libs};stdc++fs")
    endif ()

    execute_process(COMMAND root-config --has-cxx20 OUTPUT_VARIABLE HASCXX20)
    execute_process(COMMAND root-config --has-cxx17 OUTPUT_VARIABLE HASCXX17)
    execute_process(COMMAND root-config --has-cxx14 OUTPUT_VARIABLE HASCXX14)
    execute_process(COMMAND root-config --has-cxx11 OUTPUT_VARIABLE HASCXX11)

    execute_process(COMMAND root-config --cflags OUTPUT_VARIABLE ROOT_CFLAGS)

    set(CMAKE_CXX_STANDARD_REQUIRED YES)

    if (${HASCXX20} MATCHES "yes")
        set(CMAKE_CXX_STANDARD 20)
    elseif (${HASCXX17} MATCHES "yes")
        set(CMAKE_CXX_STANDARD 17)
    else ()
        message(FATAL_ERROR "Minimum C++ standard version is 17,
                while the root version is compiled with an older
                C++ standard, check root-config --cflags ${ROOT_CFLAGS}"
                )
    endif ()
endif ()

set(external_include_dirs ${external_include_dirs} ${ROOT_INCLUDE_DIRS})


#####  mySQL  #####
# This is to enable linking of SQL libraries. I.e. for process TRestSQLToAnalysisProcess.
if (NOT DEFINED REST_SQL)
    set(REST_SQL OFF)
endif ()
if (${REST_SQL} MATCHES "ON")
    list(APPEND rest_features "mySQL")
    add_definitions(-DUSE_SQL)
    include(FindSQL)
    set(external_include_dirs ${external_include_dirs} ${SQL_INCLUDE_DIR})
    set(external_libs "${external_libs} ${SQL_LIBRARIES}")
    message(STATUS "SQL include directory: " ${SQL_INCLUDE_DIR})
    set(SQL ON)
else ()
    set(REST_SQL OFF)
endif (${REST_SQL} MATCHES "ON")

#####  MPFR  #####
if (NOT DEFINED REST_MPFR)
    set(REST_MPFR OFF)
endif ()
if (${REST_MPFR} MATCHES "ON")
    find_path(MPFR_INCLUDE mpfr.h HINTS ${MPFR_PATH} ${MPFR_PATH}/include) # path to include directory
    find_path(MPFR_LIB_PATH libmpfr.so HINTS ${MPFR_PATH} ${MPFR_PATH}/lib)
    find_library(MPFR_LIB mpfr HINTS ${MPFR_PATH} ${MPFR_PATH}/lib) # path to .so file
    if (NOT MPFR_INCLUDE MATCHES "MPFR_INCLUDE-NOTFOUND" AND NOT MPFR_LIB MATCHES "MPFR_LIB-NOTFOUND")
        # MPFR is found
        list(APPEND rest_features "MPFR")
        add_compile_definitions(USE_MPFR)
        set(external_include_dirs ${external_include_dirs} ${MPFR_INCLUDE})
        set(external_libs "${external_libs};${MPFR_LIB}")
        link_directories(${MPFR_LIB})
        message(STATUS "MPFR library : ${MPFR_LIB}")
    else ()
        message(ERROR "MPFR library was not found. Adding the location to the installation path may solve this problem.\n Use -DMPFR_PATH=/path/to/mpfr/")
    endif ()
else ()
    set(REST_MPFR OFF)
    set(excludes ${excludes} TRestComplex)
endif (${REST_MPFR} MATCHES "ON")

#####  CURL  #####
find_library(CURL_LIB curl)
if (NOT ${CURL_LIB} STREQUAL "CURL_LIB-NOTFOUND")
    list(APPEND rest_features "CURL")
    add_compile_definitions(USE_Curl)
    message(STATUS "Found curl library: ${CURL_LIB}")
    set(external_libs "${external_libs};-lcurl")
else ()
    message(WARNING "CURL library was not found. ")
endif ()


# Include directories for compilation
include_directories(${external_include_dirs})
message(STATUS "external include directory: " ${external_include_dirs})
message(STATUS "external libraries to link: " ${external_libs})

# Gather some information
string(TIMESTAMP date "%Y-%m-%d %H:%M")

# Enable testing (-DTEST=ON flag, it is OFF by default)
include(Testing)

# Includes githooks, now using clang-format
include(GitHooks)

# Start compile
include(MacroRootDict)
add_subdirectory(source)

message("")
message(STATUS "rest libraries: " ${rest_libraries_regular})
message(STATUS "rest executables: " ${rest_exes_str})
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
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/data
        DESTINATION .
        COMPONENT install
        )
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/cmake/scripts
        DESTINATION .
        COMPONENT install
        )
file(GLOB cmake_scripts
        "cmake/scripts/*"
        )
install(FILES ${cmake_scripts} DESTINATION cmake)

if (CMAKE_SYSTEM_NAME MATCHES "Windows" ) # we must call library install here in windows
    foreach (lib ${rest_libraries})
        install(TARGETS ${lib}
                RUNTIME DESTINATION bin
                LIBRARY DESTINATION bin
                ARCHIVE DESTINATION lib)
    endforeach ()

    foreach (exe ${rest_exes})
        install(TARGETS ${exe}
                RUNTIME DESTINATION bin
                LIBRARY DESTINATION bin
                ARCHIVE DESTINATION lib)
    endforeach ()

    # Copy pcm files to bin
    install(CODE
            "
    file(GLOB PCMFiles \"\${CMAKE_CURRENT_SOURCE_DIR}/rootdict/*.pcm\")
    file(COPY \${PCMFiles} DESTINATION \${CMAKE_INSTALL_PREFIX}/bin)
    "
            )


else ()
    # Copy pcm files to lib
    install(CODE
            "
    file(GLOB PCMFiles \"\${CMAKE_CURRENT_SOURCE_DIR}/rootdict/*.pcm\")
    file(COPY \${PCMFiles} DESTINATION \${CMAKE_INSTALL_PREFIX}/lib)
    "
            )

    add_subdirectory(python-bindings)

    # Create thisREST.sh
    include(thisREST)

    # Add permissions
    install(CODE "execute_process(COMMAND chmod 755 ${CMAKE_INSTALL_PREFIX}/bin/rest-config)")
    install(CODE "execute_process(COMMAND chmod 755 ${CMAKE_INSTALL_PREFIX})")

endif ()

if (CMAKE_SYSTEM_NAME MATCHES "Darwin" ) # we must call library install here in MacOs
    foreach (lib ${rest_libraries})
        install(TARGETS ${lib}
                RUNTIME DESTINATION bin
                LIBRARY DESTINATION bin
                ARCHIVE DESTINATION lib)
    endforeach ()

    foreach (exe ${rest_exes})
        install(TARGETS ${exe}
                RUNTIME DESTINATION bin
                LIBRARY DESTINATION bin
                ARCHIVE DESTINATION lib)
    endforeach ()

    # Copy pcm files to bin
    install(CODE
            "
    file(GLOB PCMFiles \"\${CMAKE_CURRENT_SOURCE_DIR}/rootdict/*.pcm\")
    file(COPY \${PCMFiles} DESTINATION \${CMAKE_INSTALL_PREFIX}/bin)
    "
            )

endif()
