include_directories(${RESTCORE_INC})

#add general
set(METADATA_GENERAL_INC ${CMAKE_CURRENT_SOURCE_DIR}/general/inc)
set(METADATA_GENERAL_INC ${CMAKE_CURRENT_SOURCE_DIR}/general/inc PARENT_SCOPE)
file(GLOB MetadataGeneral
        "general/src/*.cxx"
        )
set(CLASS_DICT TRestGas TRestDetectorSetup)
foreach (CLASS ${CLASS_DICT})
    set(ROOT_DICT_INCLUDE_DIRS ${RESTCORE_INC} ${ROOT_INCLUDE_DIRS} ${METADATA_GENERAL_INC})
    set(ROOT_DICT_INPUT_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/general/inc/${CLASS}.h)
    GEN_ROOT_DICT_SOURCES(${CLASS}.cxx)
    LIST(APPEND MetadataGeneral ${ROOT_DICT_OUTPUT_SOURCES})
endforeach (CLASS)

#add daq
set(METADATA_DAQ_INC ${CMAKE_CURRENT_SOURCE_DIR}/daq/inc)
set(METADATA_DAQ_INC ${CMAKE_CURRENT_SOURCE_DIR}/daq/inc PARENT_SCOPE)
file(GLOB MetadataDaq
        "daq/src/*.cxx"
        )
set(CLASS_DICT TRestDAQMetadata)
foreach (CLASS ${CLASS_DICT})
    set(ROOT_DICT_INCLUDE_DIRS ${RESTCORE_INC} ${ROOT_INCLUDE_DIRS} ${METADATA_GENERAL_INC})
    set(ROOT_DICT_INPUT_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/daq/inc/${CLASS}.h)
    GEN_ROOT_DICT_SOURCES(${CLASS}.cxx)
    LIST(APPEND MetadataDaq ${ROOT_DICT_OUTPUT_SOURCES})
endforeach (CLASS)

#add geant4
set(METADATA_GEANT4_INC ${CMAKE_CURRENT_SOURCE_DIR}/geant4/inc)
set(METADATA_GEANT4_INC ${CMAKE_CURRENT_SOURCE_DIR}/geant4/inc PARENT_SCOPE)
file(GLOB MetadataGeant4
        "geant4/src/*.cxx"
        )
set(CLASS_DICT TRestG4Metadata TRestBiasingVolume TRestG4PrimaryGenerator TRestParticleCollection TRestParticle TRestParticleSource)
foreach (CLASS ${CLASS_DICT})
    set(ROOT_DICT_INCLUDE_DIRS ${RESTCORE_INC} ${ROOT_INCLUDE_DIRS} ${METADATA_GENERAL_INC})
    set(ROOT_DICT_INPUT_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/geant4/inc/${CLASS}.h)
    GEN_ROOT_DICT_SOURCES(${CLASS}.cxx)
    LIST(APPEND MetadataGeant4 ${ROOT_DICT_OUTPUT_SOURCES})
endforeach (CLASS)

#add readout
set(METADATA_READOUT_INC ${CMAKE_CURRENT_SOURCE_DIR}/readout/inc )
set(METADATA_READOUT_INC ${CMAKE_CURRENT_SOURCE_DIR}/readout/inc PARENT_SCOPE)
file(GLOB MetadataReadout
        "readout/src/*.cxx"
        )
set(CLASS_DICT TRestReadout TRestReadoutPlane TRestReadoutModule TRestReadoutChannel TRestReadoutPixel TRestReadoutMapping)
foreach (CLASS ${CLASS_DICT})
    set(ROOT_DICT_INCLUDE_DIRS ${RESTCORE_INC} ${ROOT_INCLUDE_DIRS} ${METADATA_GENERAL_INC})
    set(ROOT_DICT_INPUT_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/readout/inc/${CLASS}.h)
    GEN_ROOT_DICT_SOURCES(${CLASS}.cxx)
    LIST(APPEND MetadataReadout ${ROOT_DICT_OUTPUT_SOURCES})
endforeach (CLASS)

add_library(RestMetadata SHARED ${MetadataGeneral} ${MetadataGeant4} ${MetadataDaq} ${MetadataReadout})
#add_library(RestMetadataStatic STATIC ${MetadataGeneral} ${MetadataGeant4} ${Metadatadaq} ${MetadataReadout})

install(TARGETS RestMetadata
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib/static)