

# ============================================================================
# macro for compileing the whole directories into a single library
#
# arguments:
#   contents - name of the sub directories
#
# requires following variables:
#       libname - the generated library name, in default it will use the name of CMAKE_CURRENT_SOURCE_DIR
#       rest_include_dirs - 
#		external_include_dirs - 
#
# returns:
#		included header directories in the specified sub directories
#		changed variable rest_libraries
#
# ----------------------------------------------------------------------------
MACRO( COMPILEDIR libname )

	message("making build files for ${CMAKE_CURRENT_SOURCE_DIR}")

	set(contentfiles)

	if(DEFINED contents)
		message("specified sub-dirs: ${contents}")
		foreach(content ${contents})
			set(rest_include_dirs ${rest_include_dirs} ${CMAKE_CURRENT_SOURCE_DIR}/${content} ${CMAKE_CURRENT_SOURCE_DIR}/${content}/inc)
		endforeach(content)
		set(rest_include_dirs ${rest_include_dirs} PARENT_SCOPE)

		file(GLOB_RECURSE files *.cxx)
		foreach (file ${files})

			string(REGEX MATCH "[^/\\]*cxx" temp ${file})
			string(REPLACE ".cxx" "" class ${temp})

			set(ROOT_DICT_INCLUDE_DIRS ${rest_include_dirs} ${external_include_dirs})
			file(GLOB_RECURSE header ${class}.h)
			set(ROOT_DICT_INPUT_HEADERS ${header})
			GEN_ROOT_DICT_SOURCES(CINT_${class}.cxx)

			set(contentfiles ${contentfiles} ${file} ${ROOT_DICT_OUTPUT_SOURCES})

		endforeach (file)
	else()
		message("using inc/src folders in root directory")
		set(rest_include_dirs ${rest_include_dirs} ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/inc)
		set(rest_include_dirs ${rest_include_dirs} PARENT_SCOPE)

		file(GLOB_RECURSE files *.cxx)
		foreach (file ${files})

			string(REGEX MATCH "[^/\\]*cxx" temp ${file})
			string(REPLACE ".cxx" "" class ${temp})

			set(ROOT_DICT_INCLUDE_DIRS ${rest_include_dirs} ${external_include_dirs})
			file(GLOB_RECURSE header ${class}.h)
			set(ROOT_DICT_INPUT_HEADERS ${header})
			GEN_ROOT_DICT_SOURCES(CINT_${class}.cxx)

			set(contentfiles ${contentfiles} ${file} ${ROOT_DICT_OUTPUT_SOURCES})

		endforeach (file)

	endif()


	include_directories(${rest_include_dirs})

	add_library(${libname} SHARED ${contentfiles})
	target_link_libraries(${libname} ${rest_libraries} ${external_libs})

	install(TARGETS ${libname}
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib/static)

	set(rest_libraries ${rest_libraries} ${libname})
	set(rest_libraries ${rest_libraries} PARENT_SCOPE)
ENDMACRO()

