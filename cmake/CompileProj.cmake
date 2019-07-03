

MACRO( COMPILEPROJ projname )

	# find ROOT 
	include(FindROOT)

	# find REST 
	include(FindREST)

	if(REST_INCONFIGURATION)
	message( "\n++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n
			  After \"make install\" ${projname} will be installed in ${REST_PATH}/lib\n\n
			  It will take effect in the next launch of restManager/restRoot
			  \n++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n" )
	endif()
	set(CMAKE_INSTALL_PREFIX ${REST_PATH})

	#----------------------------------------------------------------------------
	# Set include and lib
	set(incdir ${REST_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS} ${Depend_Inc})
	set(lnklib ${REST_LIBRARIES} ${ROOT_LIBRARIES} -lGui -lEve -lRGL -lGeom -lGdml -lMinuit ${Depend_Lib})

	foreach (file ${Lib_Source})
		get_filename_component(dir ${file} DIRECTORY)
		list(APPEND incdir ${dir})
		list(APPEND incdir ${dir}/../inc)
	endforeach()
	list(REMOVE_DUPLICATES incdir)
	include_directories(${incdir})

	if(Lib_Source)
		# Generate ROOT dictionary for the class
		if(GenDict)
			include(MacroRootDict)
			foreach (file ${Lib_Source})
				get_filename_component(class ${file} NAME_WE)
				string(FIND ${class} "TRest" pos)
				if(pos GREATER -1)
					file(GLOB_RECURSE header ${class}.h*)
					if(header)
						#message("hhhhh ${class} ${header}")

						set(ROOT_DICT_INCLUDE_DIRS ${incdir})
						set(ROOT_DICT_INPUT_HEADERS ${header} ${ROOT_DICT_OUTPUT_DIR}/${class}_LinkDef.h)
						GEN_ROOT_DICT_LINKDEF_HEADER( ${class} ${header})
						GEN_ROOT_DICT_SOURCES(CINT_${class}.cxx ${ROOT_DICT_OUTPUT_DIR}/${class}_LinkDef.h)
						set(newsources ${newsources} ${ROOT_DICT_OUTPUT_SOURCES})

						# Install PCM files
						install(CODE
						"
						file(GLOB PCMFiles \"${ROOT_DICT_OUTPUT_DIR}/*${class}*.pcm\")
						file(COPY \${PCMFiles} DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
						"
						)

					endif()
				endif()

			endforeach()
		endif()
		# Add the library, and link it
		add_library(${projname} SHARED ${Lib_Source} ${newsources})
		target_link_libraries(${projname} ${lnklib})
		# Install library
		install(TARGETS ${projname} DESTINATION lib)
	endif()


	if(Exe_Source)
		foreach (file ${Exe_Source})
			get_filename_component(exe ${file} NAME_WE)
			add_executable(${exe} ${file})
			target_link_libraries(${exe} ${lnklib})
			if(Lib_Source)
				target_link_libraries(${exe} ${projname})
			endif()
			# Install executable
			install(TARGETS ${exe} DESTINATION bin)
		endforeach()
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
