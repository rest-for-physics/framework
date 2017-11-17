





#Add General

set(rest_include_dirs)
set(rest_libraries)


add_subdirectory(tools)
add_subdirectory(restcore)
add_subdirectory(events)
add_subdirectory(metadata)
add_subdirectory(viewers)
add_subdirectory(processes)
add_subdirectory(../macros tasks.out)
add_subdirectory(bin)

set(rest_libraries_regular ${rest_libraries_regular} PARENT_SCOPE)
set(rest_exes ${rest_exes} PARENT_SCOPE)