IF (APPLE)
    SET(LD_LIBRARY_PATH_VAR DYLD_LIBRARY_PATH)
ELSE ()
    SET(LD_LIBRARY_PATH_VAR LD_LIBRARY_PATH)
ENDIF ()
SET(LD_LIBRARY_PATH_CONTENTS $ENV{${LD_LIBRARY_PATH_VAR}})
#MESSAGE( STATUS "LD_LIBRARY_PATH_CONTENTS: ${LD_LIBRARY_PATH_CONTENTS}" )

SET(ROOT_CINT_WRAPPER ${LD_LIBRARY_PATH_VAR}=${ROOT_LIBRARY_DIR}:${LD_LIBRARY_PATH_CONTENTS} ${ROOTCINT_EXECUTABLE})

if (CMAKE_SYSTEM_NAME MATCHES "Windows")
    SET(ROOT_CINT_WRAPPER ${ROOTCINT_EXECUTABLE} -D_HAS_STD_BYTE=0)
endif ()

IF (NOT DEFINED ROOT_DICT_OUTPUT_DIR)
    SET(ROOT_DICT_OUTPUT_DIR "${PROJECT_BINARY_DIR}/rootdict")
ENDIF ()

# clean generated header files with 'make clean'
SET_DIRECTORY_PROPERTIES(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES "${ROOT_DICT_OUTPUT_DIR}")

IF (NOT ROOT_FIND_QUIETLY)
    MESSAGE(STATUS "Check for ROOT_DICT_OUTPUT_DIR: ${PROJECT_BINARY_DIR}/rootdict")
    MESSAGE(STATUS "Check for ROOT_DICT_CINT_DEFINITIONS: ${ROOT_DICT_CINT_DEFINITIONS}")
ENDIF ()


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
MACRO(PREPARE_ROOT_DICT_HEADERS _input_dir)

    FILE(GLOB ROOT_DICT_INPUT_HEADERS "${_input_dir}/*.h")
    FILE(GLOB _linkdef_hdr "${_input_dir}/LinkDef.h")

    IF (_linkdef_hdr)
        LIST(REMOVE_ITEM ROOT_DICT_INPUT_HEADERS "${_linkdef_hdr}")
        LIST(APPEND ROOT_DICT_INPUT_HEADERS "${_linkdef_hdr}")
    ENDIF ()

    #MESSAGE( STATUS "ROOT_DICT_INPUT_HEADERS: ${ROOT_DICT_INPUT_HEADERS}" )

ENDMACRO(PREPARE_ROOT_DICT_HEADERS)


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
MACRO(GEN_ROOT_DICT_LINKDEF_HEADER _namespace)

    SET(_input_headers ${ARGN})
    SET(_linkdef_header "${ROOT_DICT_OUTPUT_DIR}/${_namespace}_Linkdef.h")

	#message ( STATUS "${_input_headers}" )

	######### This code is used to identify and add std:: lists that use a struct to LinkDef
	file (STRINGS "${_input_headers}" lines)
	string(FIND "${lines}" "struct" FOUND_STRUCT )
	set ( _structName "NONE" )
	if ( FOUND_STRUCT GREATER 0 )
		string(SUBSTRING "${lines}" ${FOUND_STRUCT} 50 subline)
		string(FIND "${subline}" "{" FOUND_OPEN_BRACE )
		# It helps to filter some undesired occurences of struct. But it limits the size of the struct name
		if ( FOUND_OPEN_BRACE GREATER 0 AND FOUND_OPEN_BRACE LESS 30 )
			string( SUBSTRING "${subline}" 7 ${FOUND_OPEN_BRACE} _structName)
			string( FIND "${_structName}" " {" FOUND_OPEN_BRACE )
			string( SUBSTRING "${_structName}" 0 ${FOUND_OPEN_BRACE} _structName)
		endif ( FOUND_OPEN_BRACE GREATER 0 AND FOUND_OPEN_BRACE LESS 30 )
	endif ( FOUND_STRUCT GREATER 0 )

	set ( FOUND_STD_STRUCT "NO" )
	if( NOT _structName MATCHES "NONE" )
		#message( STATUS "Found::${_structName}::" )
		while(lines)
			list(POP_FRONT lines LINE)
			#	message( STATUS "This is line::${LINE}::" )
			string( FIND "${LINE}" "${_structName}" FOUND_STRUCT_NAME )
			string( FIND "${LINE}" "std::" FOUND_STD )
			if ( FOUND_STRUCT_NAME GREATER 0 AND FOUND_STD GREATER 0 )
				string( SUBSTRING "${LINE}" ${FOUND_STD} -1 LINE)
				string( FIND "${LINE}" ">" FOUND_END REVERSE)
				if ( FOUND_END GREATER 0 AND FOUND_END LESS 80 )
					MATH(EXPR FOUND_END "${FOUND_END}+1")
					string( SUBSTRING "${LINE}" 0 ${FOUND_END} LINE)
					message( STATUS "Adding ${LINE} to the dictionary!" )
					set ( FOUND_STD_STRUCT "${LINE}" )
				endif ( FOUND_END GREATER 0 AND FOUND_END LESS 80 )
			endif ( FOUND_STRUCT_NAME GREATER 0 AND FOUND_STD GREATER 0 )

			string(FIND LINE "${_structName}" FOUND_STRUCT_NAME )
		endwhile()
	endif( NOT _structName MATCHES "NONE" )
	######### This code is used to identify and add std:: lists that use a struct to LinkDef

    FOREACH (_header ${_input_headers})
        SET(${_namespace}_file_contents "${${_namespace}_file_contents}#ifdef __CINT__" \n)
        SET(${_namespace}_file_contents "${${_namespace}_file_contents}#pragma link off all globals\;" \n)
        SET(${_namespace}_file_contents "${${_namespace}_file_contents}#pragma link off all classes\;" \n)
        SET(${_namespace}_file_contents "${${_namespace}_file_contents}#pragma link off all functions\;" \n)
        SET(${_namespace}_file_contents "${${_namespace}_file_contents}#pragma link C++ nestedclasses\;" \n)
        SET(${_namespace}_file_contents "${${_namespace}_file_contents}#pragma link C++ nestedtypedef\;" \n)
		if( NOT FOUND_STD_STRUCT MATCHES "NO" )
            SET(${_namespace}_file_contents "${${_namespace}_file_contents}#pragma link C++ class ${_structName}+\;" \n)
			SET(${_namespace}_file_contents "${${_namespace}_file_contents}#pragma link C++ class ${FOUND_STD_STRUCT}+\;" \n)
		endif( NOT FOUND_STD_STRUCT MATCHES "NO" )
        SET(${_namespace}_file_contents "${${_namespace}_file_contents}#pragma link C++ class ${_namespace}+\;" \n)
        SET(${_namespace}_file_contents "${${_namespace}_file_contents}#endif" \n)
    ENDFOREACH ()

    file(MAKE_DIRECTORY ${ROOT_DICT_OUTPUT_DIR})
    file(WRITE ${_linkdef_header} ${${_namespace}_file_contents})

    SET(ROOT_DICT_INPUT_HEADERS ${_input_headers} ${_linkdef_header})

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
MACRO(GEN_ROOT_DICT_SOURCE _dict_src_filename)

    SET(_input_depend ${ARGN})
    # TODO check for ROOT_CINT_EXECUTABLE
    file(MAKE_DIRECTORY ${ROOT_DICT_OUTPUT_DIR})
    # need to prefix all include dirs with -I
    set(_dict_includes "-I../include") 
    FOREACH (_inc ${ROOT_DICT_INCLUDE_DIRS})
        SET(_dict_includes "${_dict_includes}\t-I${_inc}")  #fg: the \t fixes a wired string expansion
    ENDFOREACH ()

    # We modify the list of headers to be given to ROOTCINT command.
    # We must remove/clean the full path from the main header
    list(GET ROOT_DICT_INPUT_HEADERS 0 MAIN_HEADER)
    get_filename_component(MAIN_HEADER_CLEAN ${MAIN_HEADER} NAME)
    list(GET ROOT_DICT_INPUT_HEADERS 1 LINKDEF_HEADER)
    set(ROOT_DICT_INPUT_HEADERS_CLEAN ${MAIN_HEADER_CLEAN} ${LINKDEF_HEADER})

    STRING(REPLACE "/" "_" _dict_src_filename_nosc ${_dict_src_filename})
    SET(_dict_src_file ${ROOT_DICT_OUTPUT_DIR}/${_dict_src_filename_nosc})
    STRING(REGEX REPLACE "^(.*)\\.(.*)$" "\\1.h" _dict_hdr_file "${_dict_src_file}")
    ADD_CUSTOM_COMMAND(
            OUTPUT ${_dict_src_file}
            COMMAND ${ROOT_CINT_WRAPPER} -f "${_dict_src_file}" ${_dict_includes} ${ROOT_DICT_INPUT_HEADERS_CLEAN}
            WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
            DEPENDS ${ROOT_DICT_INPUT_HEADERS} ${_input_depend}
            COMMENT "generating: ${_dict_src_file} with ${ROOT_DICT_INPUT_HEADERS}"
    )
    LIST(APPEND ROOT_DICT_OUTPUT_SOURCES ${_dict_src_file})

ENDMACRO()

# for backwards compatibility
MACRO(GEN_ROOT_DICT_SOURCES _dict_src_filename)
    #MESSAGE( "USING DEPRECATED GEN_ROOT_DICT_SOURCES. PLEASE USE GEN_ROOT_DICT_SOURCE instead." )
    SET(ROOT_DICT_OUTPUT_SOURCES)
    GEN_ROOT_DICT_SOURCE(${_dict_src_filename})
ENDMACRO()
# ============================================================================


# ============================================================================
# Macro to compile the whole directories into a single library
#
# The working directory of this macro should have regular form like:
#   DIR
#    ������ CMakeLists.txt
#    ������ SUB-DIR-1
#    ��    ������ inc
#    ��    ��    ������ CLASS_A.h
#    ��    ������ src
#    ��          ������ CLASS_A.cxx
#    ������ SUB-DIR-2
#          ������ inc
#          ��    ������ CLASS_B.h
#          ��    ������ CLASS_C.h
#          ������ src
#                ������ CLASS_B.cxx
#                ������ CLASS_C.cxx
# Or:
#   DIR                     
#    ������ CMakeLists.txt              
#    ������ inc                         
#    ��    ������ CLASS_A.h     
#    ��    ������ CLASS_B.h             
#    ������ src                         
#          ������ CLASS_A.cxx      
#          ������ CLASS_B.cxx  
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
MACRO(COMPILEDIR libname)

    message(STATUS "making build files for ${CMAKE_CURRENT_SOURCE_DIR}, schema evolution: ${local_SE}")

    # We need to define the include paths relative to the compilation directory
    set(REAL_SOURCE_DIR "${CMAKE_SOURCE_DIR}/source")
    string(REPLACE ${REAL_SOURCE_DIR} "" RELATIVE ${CMAKE_CURRENT_SOURCE_DIR})
    set(RELATIVE_PATH "..${RELATIVE}")

    set(contentfiles)

    if (DEFINED contents)
        message("specified sub-dirs: ${contents}")
        foreach (content ${contents})
            set(local_include_dirs ${local_include_dirs} ${addon_inc} ${CMAKE_CURRENT_SOURCE_DIR}/${content} ${CMAKE_CURRENT_SOURCE_DIR}/${content}/inc)
        endforeach (content)
        set(local_include_dirs ${local_include_dirs} PARENT_SCOPE)

        foreach (content ${contents})
            file(GLOB_RECURSE files ${content}/*.cxx)
            foreach (file ${files})

                string(REGEX MATCH "[^/\\]*cxx" temp ${file})
                string(REPLACE ".cxx" "" class ${temp})

                set(SKIP "FALSE")
                if (DEFINED excludes)
                    foreach (exclude ${excludes})
                        if ("${exclude}" STREQUAL "${class}")
                            set(SKIP "TRUE")
                            #	message( STATUS "Skipping ${class}" )
                        endif ()
                    endforeach (exclude)
                endif ()

                if (${SKIP} STREQUAL "FALSE")
                    set(ROOT_DICT_INCLUDE_DIRS ${local_include_dirs} ${external_include_dirs})
                    file(GLOB_RECURSE header ${class}.h)
                    if (NOT header)
                        message(WARNING "header file: " ${class}.h " does not exist for source file: " ${file} ". If you really want to build it, add it to \"addon_src\" variable before calling COMPILEDIR()")
                    else ()
                        set(ROOT_DICT_INPUT_HEADERS ${header} ${ROOT_DICT_OUTPUT_DIR}/${class}_LinkDef.h)
                        if (${SCHEMA_EVOLUTION} MATCHES "ON")
                            GEN_ROOT_DICT_LINKDEF_HEADER(${class} ${header})
                            GEN_ROOT_DICT_SOURCES(CINT_${class}.cxx ${ROOT_DICT_OUTPUT_DIR}/${class}_LinkDef.h)
                        else ()
                            GEN_ROOT_DICT_SOURCES(CINT_${class}.cxx)
                        endif ()

                        set(contentfiles ${contentfiles} ${file} ${ROOT_DICT_OUTPUT_SOURCES})
                    endif ()
                endif (${SKIP} STREQUAL "FALSE")

            endforeach (file)

            #	message( STATUS "contentfiles: ${contentfiles}" )
        endforeach (content)
    else ()
        message("using inc/src folders in root directory")
        set(local_include_dirs ${local_include_dirs} ${addon_inc} ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/inc)
        set(local_include_dirs ${local_include_dirs} PARENT_SCOPE)

        file(GLOB_RECURSE files src/*.cxx)
        foreach (file ${files})
            string(REGEX MATCH "[^/\\]*cxx" temp ${file})
            string(REPLACE ".cxx" "" class ${temp})

            set(SKIP "FALSE")
            if (DEFINED excludes)
                #			message ( STATUS "EXCLUDES: ${excludes}" )
                foreach (exclude ${excludes})
                    if ("${exclude}" STREQUAL "${class}")
                        set(SKIP "TRUE")
                        #					message( STATUS "Skipping ${class}" )
                    endif ()
                endforeach (exclude)
            endif ()

            if (${SKIP} STREQUAL "FALSE")
                set(ROOT_DICT_INCLUDE_DIRS ${local_include_dirs} ${external_include_dirs})
                file(GLOB_RECURSE header ${class}.h)
                if (NOT header)
                    message(WARNING "header file: " ${class}.h " does not exist for source file: " ${file} ". If you really want to build it, add it to \"addon_src\" variable before calling COMPILEDIR()")
                else ()
                    set(ROOT_DICT_INPUT_HEADERS ${header} ${ROOT_DICT_OUTPUT_DIR}/${class}_LinkDef.h)
                    if (${SCHEMA_EVOLUTION} MATCHES "ON")
                        GEN_ROOT_DICT_LINKDEF_HEADER(${class} ${header})
                        GEN_ROOT_DICT_SOURCES(CINT_${class}.cxx ${ROOT_DICT_OUTPUT_DIR}/${class}_LinkDef.h)
                    else ()
                        GEN_ROOT_DICT_SOURCES(CINT_${class}.cxx)
                    endif ()

                    set(contentfiles ${contentfiles} ${file} ${ROOT_DICT_OUTPUT_SOURCES})
                endif ()
            endif (${SKIP} STREQUAL "FALSE")
        endforeach (file)
    endif ()


    foreach (src ${addon_CINT})
        string(REGEX MATCH "[^/\\]+$" filename ${src})
        set(ROOT_DICT_INCLUDE_DIRS ${local_include_dirs} ${external_include_dirs})
        set(ROOT_DICT_INPUT_HEADERS ${src})
        GEN_ROOT_DICT_SOURCES(CINT_${filename}.cxx)
        set(contentfiles ${contentfiles} ${src} ${ROOT_DICT_OUTPUT_SOURCES})
    endforeach (src)

    include_directories(${local_include_dirs})
    add_library(${libname} SHARED ${contentfiles} ${addon_src})
    target_link_libraries(${libname} ${local_libraries} ${external_libs})
    
    if (CMAKE_SYSTEM_NAME MATCHES "Windows")
        set_target_properties(${libname} PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS TRUE)
    else ()
        install(TARGETS ${libname}
                RUNTIME DESTINATION bin
                LIBRARY DESTINATION lib
                ARCHIVE DESTINATION lib/static)
    endif ()
    set(local_libraries ${local_libraries} ${libname})
    set(local_libraries ${local_libraries} PARENT_SCOPE)
ENDMACRO()

MACRO(SUBDIRLIST result curdir)
    FILE(GLOB children RELATIVE ${curdir} ${curdir}/*)
    SET(dirlist "")
    FOREACH (child ${children})
        IF (IS_DIRECTORY ${curdir}/${child})
            LIST(APPEND dirlist ${child})
        ENDIF ()
    ENDFOREACH ()
    SET(${result} ${dirlist})
ENDMACRO()

# ============================================================================
# Macro to convert directory name to REST library name
# e.g. axion --> RestAxion
# e.g. detector --> RestDetector
MACRO(DIRNAME2LIBNAME result dirname)
    string(SUBSTRING ${dirname} 0 1 firstletter)
    string(TOUPPER ${firstletter} firstlettercapital)
    string(SUBSTRING ${dirname} 1 -1 remainingletter)
    set(${result} "Rest${firstlettercapital}${remainingletter}")
ENDMACRO()

# ============================================================================
# Macro to convert directory name to REST CMake Option name
# e.g. source/libraries/axion --> RESTLIB_AXION
# e.g. source/packages/restG4 --> REST_G4
#
# result: the converted option string. Will be empty if error
# dirname: the relative path from CMAKE_CURRENT_SOURCE_DIR to the target dir
macro(DIRNAME2OPTION result dirname)
    set(${result} "")
    if (${dirname} MATCHES "libraries/")
        string(REPLACE "libraries/" "" purename ${dirname})
        string(TOUPPER ${purename} uppername)
        set(${result} "RESTLIB_${uppername}")
    endif ()

    if (${dirname} MATCHES "packages/")
        string(REPLACE "packages/rest" "" purename ${dirname})
        set(${result} "REST_${purename}")
    endif ()

ENDMACRO()


# ============================================================================
# Macro to compile REST libraries sub-dir into a single library
#
# The working directory of this macro should have regular form like:
#   DIR                     
#    ������ CMakeLists.txt              
#    ������ inc                         
#    ��    ������ CLASS_A.h     
#    ��    ������ CLASS_B.h             
#    ������ src                         
#          ������ CLASS_A.cxx      
#          ������ CLASS_B.cxx  
#    --- xxx.cc
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
#       dependency            - the libraries which this one depends on. Should be the name of a list. 
#                               Could be -L/usr/local/lib/xxx.so, or be other libname(e.g. RestDetector)
#                               If RestDetector_COMPULED=FALSE, then it will execute this macro
#                               firstly on that library.
#
## OUTPUT variables (will be changed after this macro):
#		dirs_included         - After this macro, additional inc dirs from the current 
#		                        directory will be attatched at the end of this variable.
#
#		library_added       - After this macro, the generated library name will be attatched 
#                               at the end of this variable.
#
## Optional variables (to be set before this macro):
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
MACRO(COMPILELIB dependency)

    string(REGEX MATCH "[^/\\]*$" puredirname ${CMAKE_CURRENT_SOURCE_DIR})
    DIRNAME2LIBNAME(libname ${puredirname})

    message(STATUS "making build files in ${CMAKE_CURRENT_SOURCE_DIR}, dependency: ${${dependency}}")

    set(files_cxx)
    set(files_to_compile_cxx)
    set(files_to_compile_cint)
    set(dirs_to_include ${rest_framework_include_dirs})
    set(libs_to_link ${rest_framework_libraries})

    # check dependency
    foreach (dep ${${dependency}})
        set(dirs_to_include ${dirs_to_include} "${CMAKE_CURRENT_SOURCE_DIR}/../${dep}/inc")
        DIRNAME2LIBNAME(deplibname ${dep})
        set(libs_to_link ${libs_to_link} ${deplibname})
    endforeach ()

    # include dir
    set(dirs_to_include ${dirs_to_include} ${addon_inc} ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/inc)

    # generate CINT files
    file(GLOB_RECURSE files_cxx ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cxx)
    foreach (file ${files_cxx})
        string(REGEX MATCH "[^/\\]*cxx$" temp ${file}) # get pure file name
        string(REPLACE ".cxx" "" class ${temp}) # get file name without extension, i.e. class name

        set(SKIP "FALSE")
        if (DEFINED excludes)
            foreach (exclude ${excludes})
                if ("${exclude}" STREQUAL "${class}")
                    set(SKIP "TRUE")
                    #	message( STATUS "Skipping ${class}" )
                endif ()
            endforeach (exclude)
        endif ()

        if (${SKIP} STREQUAL "FALSE")
            set(ROOT_DICT_INCLUDE_DIRS ${dirs_to_include} ${external_include_dirs})
            file(GLOB_RECURSE header ${class}.h)
            if (NOT header)
                message(WARNING "header file: " ${class}.h " does not exist for source file: " ${file} ". If you really want to build it, add it to \"addon_src\" variable before calling COMPILEDIR()")
            else ()
                set(ROOT_DICT_INPUT_HEADERS ${header} ${ROOT_DICT_OUTPUT_DIR}/${class}_LinkDef.h)
                if (${SCHEMA_EVOLUTION} MATCHES "ON")
                    GEN_ROOT_DICT_LINKDEF_HEADER(${class} ${header})
                    GEN_ROOT_DICT_SOURCES(CINT_${class}.cxx ${ROOT_DICT_OUTPUT_DIR}/${class}_LinkDef.h)
                else ()
                    GEN_ROOT_DICT_SOURCES(CINT_${class}.cxx)
                endif ()

                set(files_to_compile_cxx ${files_to_compile_cxx} ${file})
                set(files_to_compile_cint ${files_to_compile_cint} ${ROOT_DICT_OUTPUT_SOURCES})
            endif ()
        endif (${SKIP} STREQUAL "FALSE")
    endforeach (file)

    foreach (src ${addon_CINT})
        string(REGEX MATCH "[^/\\]+$" filename ${src})
        set(ROOT_DICT_INCLUDE_DIRS ${dirs_to_include} ${external_include_dirs})
        set(ROOT_DICT_INPUT_HEADERS ${src})
        GEN_ROOT_DICT_SOURCES(CINT_${filename}.cxx)
        set(files_to_compile ${files_to_compile} ${src} ${ROOT_DICT_OUTPUT_SOURCES})
    endforeach (src)

    list(LENGTH files_cxx Nfiles_cxx)
    list(LENGTH files_to_compile_cint Nfiles_cint)
    list(LENGTH addon_src Nfiles_add)

    message(STATUS "${Nfiles_cxx} source files in total, ${Nfiles_cint} classes to generete, ${Nfiles_add} additional source files")

    # start compile
    include_directories(${dirs_to_include} ${external_include_dirs})
    add_library(${libname} SHARED ${files_to_compile_cxx} ${files_to_compile_cint} ${addon_src})
    target_link_libraries(${libname} ${libs_to_link} ${external_libs})

    # install
        if (CMAKE_SYSTEM_NAME MATCHES "Windows")
        set_target_properties(${libname} PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS TRUE)
    else ()
        install(TARGETS ${libname}
                RUNTIME DESTINATION bin
                LIBRARY DESTINATION lib
                ARCHIVE DESTINATION lib/static)
    endif ()

    file(GLOB_RECURSE Headers "${CMAKE_CURRENT_SOURCE_DIR}/inc/*.h")
    INSTALL(FILES ${Headers} DESTINATION include)

    set(dirs_included ${dirs_to_include} PARENT_SCOPE)
    set(library_added ${libname})
    set(library_added ${library_added} PARENT_SCOPE)
ENDMACRO()
