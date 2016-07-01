include_directories(${RESTCORE_INC_DIR})
#add analysis
add_subdirectory(general)
include_directories(general/inc)
#add geant4
add_subdirectory(geant4)
include_directories(geant4/inc)
#add analysis
add_subdirectory(analysis)
include_directories(analysis/inc)

add_library(RestEvents ${EventGeneral} ${EventGeant4} ${EventAnalysis})
