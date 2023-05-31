# * Try to find Garfield++ installation This module sets up Garfield information
#   It defines: Garfield_FOUND          If Garfield++ is found
#   Garfield_INCLUDE_DIRS   PATH to the include directories Garfield_LIBRARIES
#   the libraries needed to use Garfield++

message(STATUS "Looking for Garfield using 'FindGarfieldOld.cmake'")

set(Garfield_DIR $ENV{GARFIELD_HOME})

find_path(Garfield_INCLUDE_DIRS Sensor.hh
          HINTS ${Garfield_DIR}/Include ${Garfield_DIR}/Include/Garfield
                ${Garfield_DIR}/include ${Garfield_DIR}/include/Garfield)

find_path(Garfield_INCLUDE_Heed_DIRS HeedChamber.hh HINTS ${Garfield_DIR}/Heed)

find_library(
    Garfield_LIBRARIES
    NAMES libGarfield.so Garfield
    HINTS ${Garfield_DIR}/lib ${Garfield_LIB_DIR})

if (Garfield_INCLUDE_DIRS AND Garfield_LIBRARIES)
    set(Garfield_FOUND TRUE)
endif ()

if (NOT DEFINED ENV{GARFIELD_HOME})
    message(
        "\n!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n\nError : GARFIELD HOME is not defined!
		\nCheck Garfield is installed and GARFIELD_HOME is pointing to install directory
		\nHINT : GARFIELD_HOME/lib/libGarfield.so should exist.
        \nIf you do not need Garfield++ interface in your REST installation, then,
        \nplease, run cmake disabling REST_GARFIELD variable
        \n\ni.e. : cmake -DREST_GARFIELD=OFF ../
		\n!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n")
endif ()

if (Garfield_FOUND)
    if (NOT Garfield_FIND_QUIETLY)
        message(STATUS "Found Garfield includes in ${Garfield_INCLUDE_DIRS}")
        message(STATUS "Found Garfield libraries ${Garfield_LIBRARIES}")
    endif (NOT Garfield_FIND_QUIETLY)
else (Garfield_FOUND)
    if (Garfield_FIND_REQUIRED)
        message(
            FATAL_ERROR
                "Garfield required, but not found!\ntry to run \"./scripts/installation/v2.2/installGarfield.sh\" to install it.
		")
    endif (Garfield_FIND_REQUIRED)
endif (Garfield_FOUND)

# Make variables changeable to the advanced user
mark_as_advanced(Garfield_INCLUDE_DIRS)
mark_as_advanced(Garfield_LIBRARIES)
mark_as_advanced(Garfield_LIBRARY_DIR)
