#Add general
file(GLOB ViewersGeneral
        "general/src/*.cxx"
        )
set(CLASS_DICT TRestEveEventViewer TRestHitsEventViewer TRestGenericEventViewer TRestReadoutEventViewer)
foreach (CLASS ${CLASS_DICT})
    set(ROOT_DICT_INCLUDE_DIRS ${REST_INCLUDE_DIR} ${ROOT_INCLUDE_DIRS})
    set(ROOT_DICT_INPUT_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/general/inc/${CLASS}.h)
    GEN_ROOT_DICT_SOURCES(${CLASS}.cxx)
    LIST(APPEND ViewersGeneral ${ROOT_DICT_OUTPUT_SOURCES})
endforeach (CLASS)


#Add geant4
file(GLOB ViewersGeant4
        "geant4/src/*.cxx"
        )
set(CLASS_DICT TRestG4EventViewer)
foreach (CLASS ${CLASS_DICT})
    set(ROOT_DICT_INCLUDE_DIRS ${REST_INCLUDE_DIR} ${ROOT_INCLUDE_DIRS})
    set(ROOT_DICT_INPUT_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/geant4/inc/${CLASS}.h)
    GEN_ROOT_DICT_SOURCES(${CLASS}.cxx)
    LIST(APPEND ViewersGeant4 ${ROOT_DICT_OUTPUT_SOURCES})
endforeach (CLASS)


add_library(RestViewers SHARED ${ViewersGeneral} ${ViewersGeant4} )
target_link_libraries(RestViewers ${ROOT_LIBRARIES} -lGui -lEve -lGeom ${Garfield_LIBRARIES})
#add_library(RestViewersStatic STATIC ${ViewersGeneral} ${ViewersGeant4} )

target_link_libraries(RestViewers ${ROOT_LIBRARIES} -lGui -lEve -lGeom ${Garfield_LIBRARIES})

install(TARGETS RestViewers
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib/static)
