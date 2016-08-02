#Add general
set(VIEWERS_GENERAL_INC ${CMAKE_CURRENT_SOURCE_DIR}/general/inc)
set(VIEWERS_GENERAL_INC ${CMAKE_CURRENT_SOURCE_DIR}/general/inc PARENT_SCOPE)
file(GLOB ViewersGeneral
        "general/src/*.cxx"
        )
set(CLASS_DICT TRestEveEventViewer TRestHitsEventViewer TRestGenericEventViewer TRestReadoutEventViewer)
foreach (CLASS ${CLASS_DICT})
    set(ROOT_DICT_INCLUDE_DIRS ${RESTCORE_INC} ${ROOT_INCLUDE_DIRS} ${METADATA_GENERAL_INC} ${EVENTS_GEANT4_INC} ${EVENTS_GENERAL_INC} ${METADATA_READOUT_INC})
    set(ROOT_DICT_INPUT_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/general/inc/${CLASS}.h)
    GEN_ROOT_DICT_SOURCES(${CLASS}.cxx)
    LIST(APPEND ViewersGeneral ${ROOT_DICT_OUTPUT_SOURCES})
endforeach (CLASS)


#Add geant4
set(VIEWERS_GEANT4_INC ${CMAKE_CURRENT_SOURCE_DIR}/geant4/inc)
set(VIEWERS_GEANT4_INC ${CMAKE_CURRENT_SOURCE_DIR}/geant4/inc PARENT_SCOPE)
file(GLOB ViewersGeant4
        "geant4/src/*.cxx"
        )
set(CLASS_DICT TRestG4EventViewer)
foreach (CLASS ${CLASS_DICT})
    set(ROOT_DICT_INCLUDE_DIRS ${RESTCORE_INC} ${ROOT_INCLUDE_DIRS} ${METADATA_GENERAL_INC} ${VIEWERS_GENERAL_INC} ${EVENTS_GEANT4_INC} ${EVENTS_GENERAL_INC})
    set(ROOT_DICT_INPUT_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/geant4/inc/${CLASS}.h)
    GEN_ROOT_DICT_SOURCES(${CLASS}.cxx)
    LIST(APPEND ViewersGeant4 ${ROOT_DICT_OUTPUT_SOURCES})
endforeach (CLASS)


add_library(RestViewers SHARED ${ViewersGeneral} ${ViewersGeant4} )
#add_library(RestViewersStatic STATIC ${ViewersGeneral} ${ViewersGeant4} )

target_link_libraries(RestViewers ${ROOT_LIBRARIES} -lGui -lEve -lGeom ${Garfield_LIBRARIES})

install(TARGETS RestViewers
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib/static)
