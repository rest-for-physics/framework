#add tsp
add_subdirectory(tsp)

add_library(RestExternal SHARED ${tspSrc})
add_library(RestExternalStatic STATIC ${tspSrc})

install(TARGETS RestExternal RestExternalStatic
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib/static)