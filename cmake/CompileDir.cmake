

# ============================================================================
# Macro for compileing the whole directories into a single library
#
# The working directory of this macro should have regular form like:
#	DIR
#    ├── CMakeLists.txt
#    ├── SUB-DIR-1
#    │   ├── inc
#    │   │   └── CLASS_A.h
#    │   └── src
#    │        └── CLASS_A.cxx
#    └── SUB-DIR-2
#         ├── inc
#         │   ├── CLASS_B.h
#         │   └── CLASS_C.h
#         └── src
#              ├── CLASS_B.cxx
#              └── CLASS_C.cxx
# Or:
#   DIR                     
#    ├── CMakeLists.txt              
#    ├── inc                         
#    │   ├── TRestAnalysisTree.h     
#    │   └── TRestBrowser.h             
#    └── src                         
#         ├── TRestAnalysisTree.cxx      
#         └── TRestSystemOfUnits.cxx  
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
#       libname               - the generated library name
#
## Optional global variables(PARENT_SCOPE):
#       rest_include_dirs     - the previous inc directories of REST. After this macro, 
#								additional inc dirs from the current directory will be 
#								attatched at the end of this variable.
#
#		external_include_dirs - the external inc dirs, for example from ROOT.
#
## Optional local variables:
#		contents              - this variable defines needed sub-directories of current directory
#
#		addon_src             - if some of the scripts do not follow regular directory form,
#								set them in this argument to compile them. CINT will not be 
#								called for them.
#
#		addon_inc			  - if some of the header directories do not follow regular directory 
#								form, set them in this argument to include them. 
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


	include_directories(${rest_include_dirs} ${addon_inc})
	#include_directories(${rest_include_dirs})

	add_library(${libname} SHARED ${contentfiles} ${addon_src})
	#add_library(${libname} SHARED ${contentfiles})
	target_link_libraries(${libname} ${rest_libraries} ${external_libs})

	install(TARGETS ${libname}
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib/static)

	set(rest_libraries ${rest_libraries} ${libname})
	set(rest_libraries ${rest_libraries} PARENT_SCOPE)
ENDMACRO()

