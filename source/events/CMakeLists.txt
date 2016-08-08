#add general
set(EVENTS_GENERAL_INC ${CMAKE_CURRENT_SOURCE_DIR}/general/inc)
set(EVENTS_GENERAL_INC ${CMAKE_CURRENT_SOURCE_DIR}/general/inc PARENT_SCOPE)
file(GLOB EventsGeneral
        "general/src/*.cxx"
        )
set(CLASS_DICT TRestSignalEvent TRestHitsEvent TRestSignal TRestHits TRestVolumeHits)
foreach (CLASS ${CLASS_DICT})
    set(ROOT_DICT_INCLUDE_DIRS ${RESTCORE_INC} ${ROOT_INCLUDE_DIRS} ${EVENTS_GENERAL_INC})
    set(ROOT_DICT_INPUT_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/general/inc/${CLASS}.h)
    GEN_ROOT_DICT_SOURCES(${CLASS}.cxx)
    LIST(APPEND EventsGeneral ${ROOT_DICT_OUTPUT_SOURCES})
endforeach (CLASS)


#add geant4
set(EVENTS_GEANT4_INC ${CMAKE_CURRENT_SOURCE_DIR}/geant4/inc)
set(EVENTS_GEANT4_INC ${CMAKE_CURRENT_SOURCE_DIR}/geant4/inc PARENT_SCOPE)
file(GLOB EventsGeant4
        "geant4/src/*.cxx"
        )
set(CLASS_DICT TRestG4Event TRestG4Hits TRestG4Track)
foreach (CLASS ${CLASS_DICT})
    set(ROOT_DICT_INCLUDE_DIRS ${RESTCORE_INC} ${ROOT_INCLUDE_DIRS} ${EVENTS_GENERAL_INC})
    set(ROOT_DICT_INPUT_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/geant4/inc/${CLASS}.h)
    GEN_ROOT_DICT_SOURCES(${CLASS}.cxx)
    LIST(APPEND EventsGeant4 ${ROOT_DICT_OUTPUT_SOURCES})
endforeach (CLASS)


#add analysis
set(EVENTS_ANALYSIS_INC ${CMAKE_CURRENT_SOURCE_DIR}/analysis/inc)
set(EVENTS_ANALYSIS_INC ${CMAKE_CURRENT_SOURCE_DIR}/analysis/inc PARENT_SCOPE)
file(GLOB EventsAnalysis
        "analysis/src/*.cxx"
        )
set(CLASS_DICT TRestTrackEvent TRestTrack TRestLinearizedTrackEvent TRestLinearTrack)
foreach (CLASS ${CLASS_DICT})
    set(ROOT_DICT_INCLUDE_DIRS ${RESTCORE_INC} ${ROOT_INCLUDE_DIRS} ${EVENTS_GENERAL_INC})
    set(ROOT_DICT_INPUT_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/analysis/inc/${CLASS}.h)
    GEN_ROOT_DICT_SOURCES(${CLASS}.cxx)
    LIST(APPEND EventsAnalysis ${ROOT_DICT_OUTPUT_SOURCES})
endforeach (CLASS)


add_library(RestEvents SHARED ${EventsAnalysis} ${EventsGeant4} ${EventsGeneral})
#add_library(RestEventsStatic STATIC ${EventsGeneral} ${EventsGeant4} ${EventsAnalysis})

install(TARGETS RestEvents
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib/static)