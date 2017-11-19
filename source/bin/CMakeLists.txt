#set(target_libraries ${rest_libraries} -lGui ${ROOT_LIBRARIES} -lEve -lGeom)
include_directories(${rest_include_dirs})
include_directories(${external_include_dirs})

set(rest_libraries_regular "-L${CMAKE_INSTALL_PREFIX}/lib ")
foreach(lib ${rest_libraries})
set(rest_libraries_regular "${rest_libraries_regular}-l${lib} ")
endforeach(lib)

set(rest_exes)

set(files restManager.cxx restRoot.cxx)
foreach(file ${files})
#get file name without extension
string(REGEX MATCH "[^/\\]*cxx" temp ${file})
string(REPLACE ".cxx" "" name ${temp})

ADD_EXECUTABLE(${name} ${file} )
TARGET_LINK_LIBRARIES(${name} ${rest_libraries} ${external_libs} )
install(TARGETS ${name} RUNTIME DESTINATION bin)
set(rest_exes "${rest_exes}${name} ")

endforeach(file)

set(rest_libraries_regular ${rest_libraries_regular} PARENT_SCOPE)
set(rest_exes ${rest_exes} PARENT_SCOPE)
