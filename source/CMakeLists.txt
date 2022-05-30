#####################################################
# Start to add sub dirs

set(rest_macros)
set(rest_macros_str)

add_subdirectory(framework)
add_subdirectory(../macros tasks.out)

set(rest_framework_include_dirs ${local_include_dirs})
set(rest_framework_libraries ${local_libraries})
set(rest_include_dirs ${rest_framework_include_dirs})
set(rest_libraries ${rest_framework_libraries})
set(rest_packages)
set(rest_features ${rest_features})

add_subdirectory(bin)

# ============================================================================
# Set REST CMake options for submodules, and add subdir if it's ON
# If the directory is empty, it will set the corresponding option to "OFF"
# If the directory is non-empty, but the option is not defined, then,
# IF REST_ALL_LIBS is defined, it will set the corresponding option to "ON",
# otherwise it will set them to "OFF".
#
## There is a problem with REST_ALL_LIBS because if used it is cached.
## And then there is no option to enable all but one library
## We need a system that enables all RESTLIB_ but it is just valid on 
## the cmake session we are running

# clear the cache first
SUBDIRLIST(subdirs "${CMAKE_CURRENT_SOURCE_DIR}/libraries")
foreach (subdir ${subdirs})
    DIRNAME2LIBNAME(libname ${subdir})
    set("${libname}_COMPILED" "FALSE")
endforeach ()

# loop all the dirs and set the option
set(dirs "libraries" "packages")
foreach (dir ${dirs})
    SUBDIRLIST(subdirs "${CMAKE_CURRENT_SOURCE_DIR}/${dir}")
    foreach (subdir ${subdirs})
        set(fullname "${dir}/${subdir}")
        DIRNAME2OPTION(option ${fullname})

        file(GLOB ff "*/${subdir}/CMakeLists.txt")
        if (ff) # if dir not empty, set the option to ON
            if (NOT DEFINED ${option})
                IF (${REST_ALL_LIBS} MATCHES "ON")
                    set(${option} ON)
                else ()
                    set(${option} OFF)
                endif ()
            endif ()
            message("Submodule found: ${fullname}. Option: ${option}=${${option}}")
        else ()
            set(${option} OFF)
            message("Empty submodule dir: ${fullname}. Option: ${option}=${${option}}")
        endif ()
    endforeach ()
endforeach ()

if (${REST_G4} MATCHES ON)
    set(RESTLIB_GEANT4 ON)
endif ()

if( ${REST_DAQ} MATCHES ON )
	set( RESTLIB_RAW ON )
endif()

if (${RESTLIB_DETECTOR} MATCHES ON)
    if (${RESTLIB_RAW} MATCHES ON)
        set(RESTLIB_CONNECTORS ON)
    endif ()
endif ()

# loop all the dirs and compile if option is ON
foreach (dir ${dirs})
    SUBDIRLIST(subdirs "${CMAKE_CURRENT_SOURCE_DIR}/${dir}")
    foreach (subdir ${subdirs})
        set( library_added "" )
        set(fullname "${dir}/${subdir}")
        DIRNAME2OPTION(option ${fullname})

        if (${${option}} MATCHES "ON")
            add_subdirectory(${fullname})
            set(rest_include_dirs ${rest_include_dirs} ${dirs_included})
            set(rest_libraries ${rest_libraries} ${library_added})
            set(rest_packages ${rest_packages} ${package_added})
            set(rest_features ${rest_features} ${feature_added})

            if ( NOT library_added STREQUAL "") 
                # define REST_*Lib e.g. REST_DetectorLib using library name: RestDetector -> REST_DetectorLib
                string(REGEX REPLACE "^Rest(.+)$" "REST_\\1Lib" DEFINE_VARIABLE_NAME ${library_added})
                message(STATUS "Adding compile definition: ${DEFINE_VARIABLE_NAME}")
                add_compile_definitions(${DEFINE_VARIABLE_NAME})
            endif ( NOT library_added STREQUAL "") 
        endif ()
    endforeach ()
endforeach ()

# remove duplicates
if (DEFINED rest_include_dirs)
    list(REMOVE_DUPLICATES rest_include_dirs)
endif ()
if (DEFINED rest_libraries)
    list(REMOVE_DUPLICATES rest_libraries)
endif ()
if (DEFINED rest_packages)
    list(REMOVE_DUPLICATES rest_packages)
endif ()
if (DEFINED rest_features)
    list(REMOVE_DUPLICATES rest_features)
endif ()

# set REST compilation flags to main cmake
set(rest_include_dirs ${rest_include_dirs} PARENT_SCOPE)
set(rest_libraries ${rest_libraries} PARENT_SCOPE)
set(rest_packages ${rest_packages} PARENT_SCOPE)
set(rest_features ${rest_features} PARENT_SCOPE)

set(rest_libraries_regular "-L${CMAKE_INSTALL_PREFIX}/lib ")
foreach (lib ${rest_libraries})
    set(rest_libraries_regular "${rest_libraries_regular} -l${lib} ")
endforeach (lib)
set(rest_libraries_regular ${rest_libraries_regular} PARENT_SCOPE)

set(rest_exes ${rest_exes} PARENT_SCOPE)
set(rest_macros ${rest_macros} PARENT_SCOPE)
# construct REST macros string(separate with blank)
set(rest_macros_str)
foreach (mac ${rest_macros})
    set(rest_macros_str "${rest_macros_str}${mac} ")
endforeach (mac ${rest_macros})
set(rest_macros_str ${rest_macros_str} PARENT_SCOPE)
