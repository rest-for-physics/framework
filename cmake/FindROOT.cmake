# - Finds ROOT instalation
# This module sets up ROOT information 
# It defines:
# ROOT_FOUND          If the ROOT is found
# ROOT_INCLUDE_DIR    PATH to the include directory (deprecated)
# ROOT_INCLUDE_DIRS   PATH to the include directory
# ROOT_LIBRARIES      Most common libraries
# ROOT_LIBRARY_DIR    PATH to the library directory 
# ROOT_BIN_DIR	      PATH to the excutables directory
# ROOT_PYTHONVER      Compatible python version string

# First search for ROOTConfig.cmake on the path defined via user setting 
# ROOT_DIR




if (CMAKE_SYSTEM_NAME MATCHES "Windows")

  if(DEFINED ENV{ROOTSYS})
   set(ROOTSYS $ENV{ROOTSYS})
  endif()

  if(NOT ROOTSYS)
	set(ROOT_FOUND FALSE)
  else()
    set(ROOT_FOUND TRUE)

    set(ROOT_LIBRARY_DIR ${ROOTSYS}/lib)
    set(ROOT_INCLUDE_DIRS ${ROOTSYS}/include)
    set(ROOT_BIN_DIR ${ROOTSYS}/bin)
    set(ROOTCINT_EXECUTABLE ${ROOTSYS}/bin/rootcint.exe)
	set(GENREFLEX_EXECUTABLE ${ROOTSYS}/bin/genreflex.exe)

    set(CMAKE_FIND_LIBRARY_PREFIXES "lib")
    set(CMAKE_FIND_LIBRARY_SUFFIXES ".lib" ".dll")
    set(ROOT_LIBRARIES)
    foreach(_cpt Core Imt RIO Net Hist Gui Eve RGL Gdml Geom Graf Graf3d Gpad Tree TreePlayer Rint Postscript Matrix Physics MathCore Thread MultiProc Minuit)
      find_library(ROOT_${_cpt}_LIBRARY ${_cpt} HINTS ${ROOT_LIBRARY_DIR})
        if(ROOT_${_cpt}_LIBRARY)
          mark_as_advanced(ROOT_${_cpt}_LIBRARY)
		    list(APPEND ROOT_LIBRARIES ${ROOT_${_cpt}_LIBRARY})
		  endif()
    endforeach()
    if(ROOT_LIBRARIES)
      list(REMOVE_DUPLICATES ROOT_LIBRARIES)
    endif()

  endif()
else()

  find_program(ROOT_CONFIG_EXECUTABLE root-config
    PATHS ${ROOTSYS}/bin $ENV{ROOTSYS}/bin)

  if(NOT ROOT_CONFIG_EXECUTABLE)
    set(ROOT_FOUND FALSE)
  else()    
    set(ROOT_FOUND TRUE)


	execute_process(
      COMMAND ${ROOT_CONFIG_EXECUTABLE} --incdir 
      OUTPUT_VARIABLE ROOT_INCLUDE_DIRS
      OUTPUT_STRIP_TRAILING_WHITESPACE)

	execute_process(
      COMMAND ${ROOT_CONFIG_EXECUTABLE} --libdir 
      OUTPUT_VARIABLE ROOT_LIBRARY_DIR
      OUTPUT_STRIP_TRAILING_WHITESPACE)


    set(ROOTCINT_EXECUTABLE $ENV{ROOTSYS}/bin/rootcint)
	set(GENREFLEX_EXECUTABLE $ENV{ROOTSYS}/bin/genreflex)

    execute_process(
      COMMAND ${ROOT_CONFIG_EXECUTABLE} --version 
      OUTPUT_VARIABLE ROOT_VERSION
      OUTPUT_STRIP_TRAILING_WHITESPACE)

    execute_process(
      COMMAND ${ROOT_CONFIG_EXECUTABLE} --libs
      OUTPUT_VARIABLE ROOT_LIBRARIES
      OUTPUT_STRIP_TRAILING_WHITESPACE)

    execute_process(
      COMMAND ${ROOT_CONFIG_EXECUTABLE} --bindir
      OUTPUT_VARIABLE ROOT_BIN_DIR
      OUTPUT_STRIP_TRAILING_WHITESPACE)

	# Make variables changeble to the advanced user
    mark_as_advanced(ROOT_CONFIG_EXECUTABLE)
  endif()
endif()


if (NOT ROOT_FOUND)
    message(FATAL_ERROR "ROOT required, but not found")
else()
	message(STATUS "Find ROOT ${ROOT_VERSION} in ${ROOT_BIN_DIR}")
endif()

