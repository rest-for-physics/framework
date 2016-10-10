#Generate CINT file
set(CLASS_DICT myManager myProcess myMetadata)

file(GLOB MyRESTLibrarySrc
        "src/*.cxx"
        )

foreach (CLASS ${CLASS_DICT})
    set(ROOT_DICT_INCLUDE_DIRS ${REST_INCLUDE_DIR} ${ROOT_INCLUDE_DIRS})
    set(ROOT_DICT_INPUT_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/inc/${CLASS}.h)
    GEN_ROOT_DICT_SOURCES(${CLASS}.cxx)
    LIST(APPEND MyRESTLibrarySrc ${ROOT_DICT_OUTPUT_SOURCES})
endforeach (CLASS)

include_directories(inc)
add_library(MyRESTLibrary SHARED ${MyRESTLibrarySrc})
set(target_libraries RestViewers RestTools RestEvents RestMetadata RestProcesses RestExternal RestCore ${target_libraries} -lGui ${ROOT_LIBRARIES} -lEve -lGeom)
#target_link_libraries(MyRESTLibrary ${ROOT_LIBRARIES} -lGui -lEve -lGeom ${Garfield_LIBRARIES})
#add_library(RestCoreStatic STATIC ${RestCoreSrc})
target_link_libraries(MyRESTLibrary ${target_libraries})

add_executable(restLauncher restLauncher.cc)
target_link_libraries(restLauncher MyRESTLibrary)
install(TARGETS MyRESTLibrary
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib/static)

