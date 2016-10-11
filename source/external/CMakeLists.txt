#add tsp
add_subdirectory(tsp)

add_library(RestExternal SHARED ${tspSrc})
target_link_libraries(RestExternal ${ROOT_LIBRARIES} -lGui -lEve -lGeom ${Garfield_LIBRARIES})
#add_library(RestExternalStatic STATIC ${tspSrc})

install(TARGETS RestExternal
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib/static)