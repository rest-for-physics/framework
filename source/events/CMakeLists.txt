#add general
file(GLOB EventsGeneral
        "general/src/*.cxx"
        )
    set(CLASS_DICT TRestSignalEvent TRestRawSignalEvent TRestHitsEvent TRestSignal TRestRawSignal TRestHits TRestVolumeHits)
foreach (CLASS ${CLASS_DICT})
    set(ROOT_DICT_INCLUDE_DIRS ${REST_INCLUDE_DIR} ${ROOT_INCLUDE_DIRS})
    set(ROOT_DICT_INPUT_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/general/inc/${CLASS}.h)
    GEN_ROOT_DICT_SOURCES(${CLASS}.cxx)
    LIST(APPEND EventsGeneral ${ROOT_DICT_OUTPUT_SOURCES})
endforeach (CLASS)


#add geant4
file(GLOB EventsGeant4
        "geant4/src/*.cxx"
        )
set(CLASS_DICT TRestG4Event TRestG4Hits TRestG4Track)
foreach (CLASS ${CLASS_DICT})
    set(ROOT_DICT_INCLUDE_DIRS ${REST_INCLUDE_DIR} ${ROOT_INCLUDE_DIRS})
    set(ROOT_DICT_INPUT_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/geant4/inc/${CLASS}.h)
    GEN_ROOT_DICT_SOURCES(${CLASS}.cxx)
    LIST(APPEND EventsGeant4 ${ROOT_DICT_OUTPUT_SOURCES})
endforeach (CLASS)


#add analysis
file(GLOB EventsAnalysis
        "analysis/src/*.cxx"
        )
set(CLASS_DICT TRestTrackEvent TRestTrack TRestLinearTrackEvent TRestLinearTrack)
foreach (CLASS ${CLASS_DICT})
    set(ROOT_DICT_INCLUDE_DIRS ${REST_INCLUDE_DIR} ${ROOT_INCLUDE_DIRS})
    set(ROOT_DICT_INPUT_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/analysis/inc/${CLASS}.h)
    GEN_ROOT_DICT_SOURCES(${CLASS}.cxx)
    LIST(APPEND EventsAnalysis ${ROOT_DICT_OUTPUT_SOURCES})
endforeach (CLASS)


add_library(RestEvents SHARED ${EventsAnalysis} ${EventsGeant4} ${EventsGeneral})
target_link_libraries(RestEvents ${ROOT_LIBRARIES} -lGui -lEve -lGeom ${Garfield_LIBRARIES})
#add_library(RestEventsStatic STATIC ${EventsGeneral} ${EventsGeant4} ${EventsAnalysis})

install(TARGETS RestEvents
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib/static)
