
# - Try to find Garfield++ instalation
# This module sets up Garfield information
# It defines:
# Garfield_FOUND          If Garfiled++ is found
# Garfield_INCLUDE_DIRS   PATH to the include directories
# Garfield_LIBRARIES      the libraries needed to use Garfield++

#message(STATUS "Looking for Garfield ...")

# Alternative paths which can be defined by user
#set(Garfield_DIR "" CACHE PATH "Directory where Garfield is installed")
#set(Garfield_INC_DIR "" CACHE PATH "Alternative directory for Garfield includes")
#set(Garfield_LIB_DIR "" CACHE PATH "Alternative directory for Garfield libraries")
set(Garfield_DIR ${GARFIELD_HOME})

find_path(Garfield_INCLUDE_DIRS Sensor.hh
    HINTS ${Garfield_DIR}/include ${Garfield_INC_DIR}
    $ENV{GARFIELD_HOME}/Include)
#message(STATUS Garfield_INCLUDE_DIRS ${Garfield_INCLUDE_DIRS})

find_library(Garfield_LIBRARIES NAMES libGarfield.so Garfield
    HINTS ${Garfield_DIR}/lib ${Garfield_LIB_DIR}
    $ENV{GARFIELD_HOME}/lib)
#message(STATUS Garfield_LIBRARIES ${Garfield_LIBRARIES})

#if (${Garfield_LIBRARY_DIR})
#  set (Garfield_LIBRARIES -L${Garfield_LIBRARY_DIR} -lGarfield)
#endif()

if (Garfield_INCLUDE_DIRS AND Garfield_LIBRARIES)
    set (Garfield_FOUND TRUE)
endif()

if (NOT DEFINED ENV{GARFIELD_HOME} )
	message("\n!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n\nGARFIELD HOME has not been defined!
		\nCheck Garfield is installed and GARFIELD_HOME is pointing to install directory
		\nHINT : GARFIELD_HOME/lib/libGarfield.so should exist.
		\n!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n" )
endif()

if (Garfield_FOUND)
    if (NOT Garfield_FIND_QUIETLY)
        message(STATUS "Found Garfield includes in ${Garfield_INCLUDE_DIRS}")
        message(STATUS "Found Garfield libraries ${Garfield_LIBRARIES}")
    endif (NOT Garfield_FIND_QUIETLY)
else(Garfield_FOUND)
    if (Garfield_FIND_REQUIRED)
        message(FATAL_ERROR "Garfield required, but not found!\ntry to run \"./scripts/installation/v2.2/installGarfield.sh\" to install it.
		")
    endif (Garfield_FIND_REQUIRED)
endif(Garfield_FOUND)

# Make variables changeble to the advanced user
mark_as_advanced(Garfield_INCLUDE_DIRS)
mark_as_advanced(Garfield_LIBRARIES)
mark_as_advanced(Garfield_LIBRARY_DIR)
