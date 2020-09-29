IF(APPLE)
    SET( LD_LIBRARY_PATH_VAR DYLD_LIBRARY_PATH )
ELSE()
    SET( LD_LIBRARY_PATH_VAR LD_LIBRARY_PATH )
ENDIF()
SET( LD_LIBRARY_PATH_CONTENTS $ENV{${LD_LIBRARY_PATH_VAR}} )
#MESSAGE( STATUS "LD_LIBRARY_PATH_CONTENTS: ${LD_LIBRARY_PATH_CONTENTS}" )

SET( ROOT_CINT_WRAPPER ${LD_LIBRARY_PATH_VAR}=${ROOT_LIBRARY_DIR}:${LD_LIBRARY_PATH_CONTENTS} ${ROOTCINT_EXECUTABLE} )

if(CMAKE_SYSTEM_NAME MATCHES "Windows")
    SET( ROOT_CINT_WRAPPER ${ROOTCINT_EXECUTABLE} )
endif()

IF( NOT DEFINED ROOT_DICT_OUTPUT_DIR )
    SET( ROOT_DICT_OUTPUT_DIR "${PROJECT_BINARY_DIR}/rootdict" )
ENDIF()

# clean generated header files with 'make clean'
SET_DIRECTORY_PROPERTIES( PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES "${ROOT_DICT_OUTPUT_DIR}" )

IF( NOT ROOT_FIND_QUIETLY )
    MESSAGE( STATUS "Check for ROOT_DICT_OUTPUT_DIR: ${PROJECT_BINARY_DIR}/rootdict" )
    MESSAGE( STATUS "Check for ROOT_DICT_CINT_DEFINITIONS: ${ROOT_DICT_CINT_DEFINITIONS}" )
ENDIF()


# ============================================================================
# helper macro to prepare input headers for GEN_ROOT_DICT_SOURCES
#   sorts LinkDef.h to be the last header (required by rootcint)
#
# arguments:
#   input_dir - directory to search for headers matching *.hh
#
# returns:
#   ROOT_DICT_INPUT_HEADERS - all header files found in input_dir with
#       ${input_dir}_LinkDef.h as the last header (if found)
#
# ----------------------------------------------------------------------------
MACRO( PREPARE_ROOT_DICT_HEADERS _input_dir )

    FILE( GLOB ROOT_DICT_INPUT_HEADERS "${_input_dir}/*.h" )
    FILE( GLOB _linkdef_hdr "${_input_dir}/LinkDef.h" )

    IF( _linkdef_hdr )
        LIST( REMOVE_ITEM ROOT_DICT_INPUT_HEADERS "${_linkdef_hdr}" )
        LIST( APPEND ROOT_DICT_INPUT_HEADERS "${_linkdef_hdr}")
    ENDIF()

    #MESSAGE( STATUS "ROOT_DICT_INPUT_HEADERS: ${ROOT_DICT_INPUT_HEADERS}" )

ENDMACRO( PREPARE_ROOT_DICT_HEADERS )



# ============================================================================
# helper macro to generate Linkdef.h files for rootcint
#
# arguments:
#   namespace - prefix used for creating header <namespace>_Linkdef.h
#   ARGN      - list of sources to be used for generating Linkdef.h
#
# returns:
#   ROOT_DICT_INPUT_HEADERS - all header files + <namespace>_LinkDef.h in the
#       correct order to be used by macro GEN_ROOT_DICT_SOURCES
#
# ----------------------------------------------------------------------------
MACRO( GEN_ROOT_DICT_LINKDEF_HEADER _namespace )

    SET( _input_headers ${ARGN} )
    SET( _linkdef_header "${ROOT_DICT_OUTPUT_DIR}/${_namespace}_Linkdef.h" )

    FOREACH( _header ${_input_headers} )
        SET( ${_namespace}_file_contents "${${_namespace}_file_contents}#ifdef __CINT__" \n )
        SET( ${_namespace}_file_contents "${${_namespace}_file_contents}#pragma link off all globals\;" \n )
        SET( ${_namespace}_file_contents "${${_namespace}_file_contents}#pragma link off all classes\;" \n )
        SET( ${_namespace}_file_contents "${${_namespace}_file_contents}#pragma link off all functions\;" \n )
        SET( ${_namespace}_file_contents "${${_namespace}_file_contents}#pragma link C++ nestedclasses\;" \n )
        SET( ${_namespace}_file_contents "${${_namespace}_file_contents}#pragma link C++ nestedclasses\;" \n )
        SET( ${_namespace}_file_contents "${${_namespace}_file_contents}#pragma link C++ class ${_namespace}+\;" \n )
        SET( ${_namespace}_file_contents "${${_namespace}_file_contents}#endif" \n )
    ENDFOREACH()

	file(MAKE_DIRECTORY ${ROOT_DICT_OUTPUT_DIR})
	file(WRITE ${_linkdef_header} ${${_namespace}_file_contents})

    SET( ROOT_DICT_INPUT_HEADERS ${_input_headers} ${_linkdef_header} )

ENDMACRO()


# ============================================================================
# macro for generating root dict sources with rootcint
#
# arguments:
#   dict_src_filename - filename of the dictionary source (to be generated)
#
# requires following variables:
#       ROOT_DICT_INPUT_HEADERS - list of headers needed to generate dict source
#           * if $LinkDef.h is in the list it must be at the end !!
#       ROOT_DICT_INCLUDE_DIRS - list of include dirs to pass to rootcint -I..
#       ROOT_DICT_CINT_DEFINITIONS - extra definitions to pass to rootcint
#       ROOT_DICT_OUTPUT_DIR - where dictionary source should be generated
#
# returns:
#       ROOT_DICT_OUTPUT_SOURCES - list containing generated source and other
#           previously generated sources
                                    
# ----------------------------------------------------------------------------
MACRO( GEN_ROOT_DICT_SOURCE _dict_src_filename )

    SET( _input_depend ${ARGN} )
    # TODO check for ROOT_CINT_EXECUTABLE
	file(MAKE_DIRECTORY ${ROOT_DICT_OUTPUT_DIR})
    # need to prefix all include dirs with -I
    set( _dict_includes )
    FOREACH( _inc ${ROOT_DICT_INCLUDE_DIRS} )
        SET( _dict_includes "${_dict_includes}\t-I${_inc}")  #fg: the \t fixes a wired string expansion 
    ENDFOREACH()

    # We modify the list of headers to be given to ROOTCINT command.
    # We must remove/clean the full path from the main header
    list ( GET ROOT_DICT_INPUT_HEADERS 0 MAIN_HEADER)
	get_filename_component( MAIN_HEADER_CLEAN ${MAIN_HEADER} NAME)
    list ( GET ROOT_DICT_INPUT_HEADERS 1 LINKDEF_HEADER )
    set( ROOT_DICT_INPUT_HEADERS_CLEAN ${MAIN_HEADER_CLEAN} ${LINKDEF_HEADER} )

    STRING( REPLACE "/" "_" _dict_src_filename_nosc ${_dict_src_filename} )
    SET( _dict_src_file ${ROOT_DICT_OUTPUT_DIR}/${_dict_src_filename_nosc} )
    STRING( REGEX REPLACE "^(.*)\\.(.*)$" "\\1.h" _dict_hdr_file "${_dict_src_file}" )
    ADD_CUSTOM_COMMAND(
        OUTPUT  ${_dict_src_file}
        COMMAND ${ROOT_CINT_WRAPPER} -f "${_dict_src_file}" ${_dict_includes} ${ROOT_DICT_INPUT_HEADERS_CLEAN}
        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
        DEPENDS ${ROOT_DICT_INPUT_HEADERS} ${_input_depend}
        COMMENT "generating: ${_dict_src_file} with ${ROOT_DICT_INPUT_HEADERS}"
    )
    LIST( APPEND ROOT_DICT_OUTPUT_SOURCES ${_dict_src_file} )

ENDMACRO()

# for backwards compatibility
MACRO( GEN_ROOT_DICT_SOURCES _dict_src_filename )
    #MESSAGE( "USING DEPRECATED GEN_ROOT_DICT_SOURCES. PLEASE USE GEN_ROOT_DICT_SOURCE instead." )
    SET( ROOT_DICT_OUTPUT_SOURCES )
    GEN_ROOT_DICT_SOURCE( ${_dict_src_filename} )
ENDMACRO()
# ============================================================================



# ============================================================================
# Macro to compile the whole directories into a single library
#
# The working directory of this macro should have regular form like:
#   DIR
#    ©À©¤©¤ CMakeLists.txt
#    ©À©¤©¤ SUB-DIR-1
#    ©¦    ©À©¤©¤ inc
#    ©¦    ©¦    ©¸©¤©¤ CLASS_A.h
#    ©¦    ©¸©¤©¤ src
#    ©¦          ©¸©¤©¤ CLASS_A.cxx
#    ©¸©¤©¤ SUB-DIR-2
#          ©À©¤©¤ inc
#          ©¦    ©À©¤©¤ CLASS_B.h
#          ©¦    ©¸©¤©¤ CLASS_C.h
#          ©¸©¤©¤ src
#                ©À©¤©¤ CLASS_B.cxx
#                ©¸©¤©¤ CLASS_C.cxx
# Or:
#   DIR                     
#    ©À©¤©¤ CMakeLists.txt              
#    ©À©¤©¤ inc                         
#    ©¦    ©À©¤©¤ CLASS_A.h     
#    ©¦    ©¸©¤©¤ CLASS_B.h             
#    ©¸©¤©¤ src                         
#          ©À©¤©¤ CLASS_A.cxx      
#          ©¸©¤©¤ CLASS_B.cxx  
#
# This macro will first set include directories of cmake to ${CMAKE_CURRENT_SOURCE_DIR},
# ${CMAKE_CURRENT_SOURCE_DIR}/inc, sub-directories, and sub-directories/inc.
#
# Then it will find out all the cxx files and call CINT.
#
# Finally it will call cmake to add a library, using the found cxx files, CINT-wrappered 
# cxx files, and other defined c++ scripts.
#
## Arguments:
#		libname               - the generated library name
#
## OUTPUT variables (global):
#		local_include_dirs    - After this macro, additional inc dirs from the current 
#		                        directory will be attatched at the end of this variable.
#
#		external_include_dirs - the external inc dirs, for example from ROOT.
#
## Optional local variables:
#		contents              - this variable defines needed sub-directories of current directory
#
#		addon_src             - if some of the scripts do not follow regular directory form,
#		                        set them in this argument to compile them. CINT will not be 
#		                        called for them.
#
#		addon_CINT            - if some of the scripts do not follow regular directory form,
#		                        set them in this argument to compile them with CINT
#
#		addon_inc             - if some of the header directories do not follow regular directory 
#		                        form, set them in this argument to include them. 
#
# ----------------------------------------------------------------------------
MACRO( COMPILEDIR libname )

	message(STATUS "making build files for ${CMAKE_CURRENT_SOURCE_DIR}, schema evolution: ${local_SE}")

    # We need to define the include paths relative to the compilation directory
    set ( REAL_SOURCE_DIR "${CMAKE_SOURCE_DIR}/source" )
    string( REPLACE ${REAL_SOURCE_DIR} "" RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} )
    set(RELATIVE_PATH "..${RELATIVE}")

	set(contentfiles)

	if(DEFINED contents)
		message("specified sub-dirs: ${contents}")
		foreach(content ${contents})
			set(local_include_dirs ${local_include_dirs} ${addon_inc} ${CMAKE_CURRENT_SOURCE_DIR}/${content} ${CMAKE_CURRENT_SOURCE_DIR}/${content}/inc)
		endforeach(content)
		set(local_include_dirs ${local_include_dirs} PARENT_SCOPE)

		foreach(content ${contents})
		file(GLOB_RECURSE files ${content}/*.cxx)
		foreach (file ${files})
			string(REGEX MATCH "[^/\\]*cxx" temp ${file})
			string(REPLACE ".cxx" "" class ${temp})

			set(ROOT_DICT_INCLUDE_DIRS ${local_include_dirs} ${external_include_dirs})
			file(GLOB_RECURSE header ${class}.h)
			if(NOT header)
				message(WARNING "header file: " ${class}.h " does not exist for source file: " ${file} ". If you really want to build it, add it to \"addon_src\" variable before calling COMPILEDIR()")
			else()
				set(ROOT_DICT_INPUT_HEADERS ${header} ${ROOT_DICT_OUTPUT_DIR}/${class}_LinkDef.h)
				if(${SCHEMA_EVOLUTION} MATCHES "ON")
					GEN_ROOT_DICT_LINKDEF_HEADER( ${class} ${header})
					GEN_ROOT_DICT_SOURCES(CINT_${class}.cxx ${ROOT_DICT_OUTPUT_DIR}/${class}_LinkDef.h)
				else()
					GEN_ROOT_DICT_SOURCES(CINT_${class}.cxx)
				endif()

				set(contentfiles ${contentfiles} ${file} ${ROOT_DICT_OUTPUT_SOURCES})
			endif()

		endforeach (file)
		
		endforeach(content)
	else()
		message("using inc/src folders in root directory")
		set(local_include_dirs ${local_include_dirs} ${addon_inc} ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/inc)
		set(local_include_dirs ${local_include_dirs} PARENT_SCOPE)

		file(GLOB_RECURSE files src/*.cxx)
		foreach (file ${files})
			string(REGEX MATCH "[^/\\]*cxx" temp ${file})
			string(REPLACE ".cxx" "" class ${temp})

			set(ROOT_DICT_INCLUDE_DIRS ${local_include_dirs} ${external_include_dirs})
			file(GLOB_RECURSE header ${class}.h)
			if(NOT header)
				message(WARNING "header file: " ${class}.h " does not exist for source file: " ${file} ". If you really want to build it, add it to \"addon_src\" variable before calling COMPILEDIR()")
			else()
				set(ROOT_DICT_INPUT_HEADERS ${header} ${ROOT_DICT_OUTPUT_DIR}/${class}_LinkDef.h)
				if(${SCHEMA_EVOLUTION} MATCHES "ON")
					GEN_ROOT_DICT_LINKDEF_HEADER( ${class} ${header})
					GEN_ROOT_DICT_SOURCES(CINT_${class}.cxx ${ROOT_DICT_OUTPUT_DIR}/${class}_LinkDef.h)
				else()
					GEN_ROOT_DICT_SOURCES(CINT_${class}.cxx)
				endif()

				set(contentfiles ${contentfiles} ${file} ${ROOT_DICT_OUTPUT_SOURCES})
			endif()
		endforeach (file)
	endif()


	foreach(src ${addon_CINT})
		string(REGEX MATCH "[^/\\]+$" filename ${src})
		set(ROOT_DICT_INCLUDE_DIRS ${local_include_dirs} ${external_include_dirs})
		set(ROOT_DICT_INPUT_HEADERS ${src})
		GEN_ROOT_DICT_SOURCES(CINT_${filename}.cxx)
		set(contentfiles ${contentfiles} ${src} ${ROOT_DICT_OUTPUT_SOURCES})
	endforeach(src)

	include_directories(${local_include_dirs})
	add_library(${libname} SHARED ${contentfiles} ${addon_src})


	if(CMAKE_SYSTEM_NAME MATCHES "Windows")
		set_target_properties(${libname} PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS TRUE)
		target_link_libraries(${libname} ${local_libraries} ${external_libs})
		install(TARGETS ${libname}
			RUNTIME DESTINATION bin
			LIBRARY DESTINATION bin
			ARCHIVE DESTINATION lib)
	else()
		target_link_libraries(${libname} ${local_libraries} ${external_libs})
		install(TARGETS ${libname}
			RUNTIME DESTINATION bin
			LIBRARY DESTINATION lib
			ARCHIVE DESTINATION lib/static)
	endif()
	set(local_libraries ${local_libraries} ${libname})
	set(local_libraries ${local_libraries} PARENT_SCOPE)
ENDMACRO()

MACRO(SUBDIRLIST result curdir)
	FILE(GLOB children RELATIVE ${curdir} ${curdir}/*)
	SET(dirlist "")
	FOREACH(child ${children})
		IF(IS_DIRECTORY ${curdir}/${child})
			LIST(APPEND dirlist ${child})
		ENDIF()
	ENDFOREACH()
	SET(${result} ${dirlist})
ENDMACRO()
