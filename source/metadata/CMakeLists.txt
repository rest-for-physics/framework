include_directories(${RESTCORE_INC})

add_subdirectory(general)
set(METADATA_GENERAL_INC ${METADATA_GENERAL_INC} PARENT_SCOPE)

add_subdirectory(daq)
set(METADATA_DAQ_INC ${METADATA_DAQ_INC} PARENT_SCOPE)

add_subdirectory(geant4)
set(METADATA_GEANT4_INC ${METADATA_GEANT4_INC} PARENT_SCOPE)

add_subdirectory(readout)
set(METADATA_READOUT_INC ${METADATA_READOUT_INC} PARENT_SCOPE)

add_library(RestMetadata ${MetadataGeneral} ${MetadataGeant4} ${Metadatadaq} ${MetadataReadout})
