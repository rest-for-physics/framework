

MACRO( COMPILEPROJ projname )

	# find ROOT 
	include(FindROOT)

	# find REST 
	include(FindREST)
	message(${REST_LIBRARIES})

	if(REST_INCONFIGURATION)
	message( "\n++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n
			  After \"make install\" ${projname} will be installed in ${REST_PATH}/lib\n\n
			  It will take effect in the next launch of restManager/restRoot
			  \n++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n" )
	endif()
	set(CMAKE_INSTALL_PREFIX ${REST_PATH})

	#----------------------------------------------------------------------------
	# Set include and lib
	set(incdir ${PROJECT_SOURCE_DIR}/inc ${ROOT_INCLUDE_DIRS} ${REST_INCLUDE_DIRS} ${Depend_Inc})
	set(lnklib ${REST_LIBRARIES} ${ROOT_LIBRARIES} -lGui -lEve -lRGL -lGeom -lGdml -lMinuit ${Depend_Lib})

	include_directories(${incdir})

	if(Lib_Source)
		# Generate ROOT dictionary for the class
		if(GenDict)
			include(MacroRootDict)
			foreach (file ${Lib_Source})
				get_filename_component(class ${file} NAME_WE)
				file(GLOB_RECURSE header ${class}.h*)

				message("hhhhh ${class} ${header}")

				set(ROOT_DICT_INCLUDE_DIRS ${incdir})
				set(ROOT_DICT_INPUT_HEADERS ${header} ${ROOT_DICT_OUTPUT_DIR}/${class}_LinkDef.h)
				GEN_ROOT_DICT_LINKDEF_HEADER( ${class} ${header})
				GEN_ROOT_DICT_SOURCES(CINT_${class}.cxx ${ROOT_DICT_OUTPUT_DIR}/${class}_LinkDef.h)
				set(newsources ${newsources} ${ROOT_DICT_OUTPUT_SOURCES})

				# Install PCM files
				install(CODE
				"
				file(GLOB PCMFiles \"\${CMAKE_CURRENT_SOURCE_DIR}/rootdict/${class}.pcm\")
				file(COPY \${PCMFiles} DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
				"
				)

			endforeach()
		endif()
		# Add the library, and link it
		add_library(${projname} SHARED ${Lib_Source} ${newsources})
		target_link_libraries(${projname} ${lnklib})
		# Install library
		install(TARGETS ${projname} DESTINATION lib)
	endif()


	if(Exe_Source)
		add_executable(${projname} ${Exe_Source})
		target_link_libraries(${projname} ${lnklib})
		# Install executable
		install(TARGETS ${projname} DESTINATION bin)
	endif()


	if(Copy_Source)
		# Install additional directories
		foreach (dir ${Copy_Source})
			install( DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/${dir}
			 DESTINATION .
			 COMPONENT install
					 )
		endforeach()
	endif()

ENDMACRO()
