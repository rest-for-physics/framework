set(rest_include_dirs)
set(rest_libraries)
set(rest_macros)

add_subdirectory(tools)
add_subdirectory(restcore)
add_subdirectory(metadata)
add_subdirectory(events)
add_subdirectory(processes)
add_subdirectory(../macros tasks.out)

set(rest_include_dirs ${local_include_dirs})
set(rest_libraries ${local_libraries})

SUBDIRLIST(SUBDIRS ${PROJECT_SOURCE_DIR}/libraries)

message( "${PROJECT_SOURCE_DIR}")
FOREACH(subdir ${SUBDIRS})
	set(rest_include_dirs ${rest_include_dirs} ${PROJECT_SOURCE_DIR}/libraries/${subdir}/inc)
ENDFOREACH()

set(rest_include_dirs ${rest_include_dirs} PARENT_SCOPE)
set(rest_libraries ${rest_libraries} PARENT_SCOPE)

set(rest_libraries_regular "-L${CMAKE_INSTALL_PREFIX}/lib ")
foreach(lib ${rest_libraries})
  set(rest_libraries_regular "${rest_libraries_regular} -l${lib} ")
endforeach(lib)
set(rest_libraries_regular ${rest_libraries_regular} PARENT_SCOPE)

add_subdirectory(bin)

set(rest_exes ${rest_exes} PARENT_SCOPE)
set(rest_macros ${rest_macros} PARENT_SCOPE)
set(rest_macros_str ${rest_macros_str} PARENT_SCOPE)
