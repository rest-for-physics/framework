





#Add General

set(rest_include_dirs)
set(rest_libraries)
set(rest_macros)


add_subdirectory(tools)
add_subdirectory(restcore)
add_subdirectory(metadata)
add_subdirectory(events)
add_subdirectory(processes)
add_subdirectory(../macros tasks.out)
add_subdirectory(bin)

set(rest_include_dirs ${rest_include_dirs} PARENT_SCOPE)
set(rest_libraries ${rest_libraries} PARENT_SCOPE)
set(rest_libraries_regular ${rest_libraries_regular} PARENT_SCOPE)
set(rest_exes ${rest_exes} PARENT_SCOPE)
set(rest_macros ${rest_macros} PARENT_SCOPE)
set(rest_macros_str ${rest_macros_str} PARENT_SCOPE)