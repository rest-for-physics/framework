include_directories(${RESTCORE_INC_DIR})

add_subdirectory(general)
include_directories(general/inc)

add_subdirectory(daq)
include_directories(daq/inc)

add_subdirectory(geant4)
include_directories(geant4/inc)

add_subdirectory(readout)
include_directories(readout/inc)

add_library(RestMetadata ${MetadataGeneral} ${MetadataGeant4} ${Metadatadaq} ${MetadataReadout})
