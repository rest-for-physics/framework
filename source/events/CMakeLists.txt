
#add analysis
add_subdirectory(general)
set(EVENTS_GENERAL_INC ${EVENTS_GENERAL_INC} PARENT_SCOPE)
#add geant4
add_subdirectory(geant4)
set(EVENTS_GEANT4_INC ${EVENTS_GEANT4_INC} PARENT_SCOPE)
#add analysis
add_subdirectory(analysis)
set(EVENTS_ANALYSIS_INC ${EVENTS_ANALYSIS_INC} PARENT_SCOPE)

add_library(RestEvents ${EventsGeneral} ${EventsGeant4} ${EventsAnalysis})
