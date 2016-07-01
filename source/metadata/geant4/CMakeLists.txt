#Generate CINT file
set(CLASS_DICT TRestG4Metadata TRestBiasingVolume TRestG4PrimaryGenerator TRestParticleCollection TRestParticle TRestParticleSource)
foreach (CLASS ${CLASS_DICT})
    message("Generate dict file for " ${CLASS})
    set(INC -I. -I./inc -I${RESTCORE_INC} -I${ROOT_INCLUDE_DIRS})
    set(INC -I. -I./inc -I${RESTCORE_INC} -I${ROOT_INCLUDE_DIRS})
    execute_process(COMMAND rootcint -f ${CMAKE_CURRENT_SOURCE_DIR}/dict/${CLASS}.cxx -c ${INC} ${CMAKE_CURRENT_SOURCE_DIR}/inc/${CLASS}.h)
endforeach (CLASS)

file(GLOB MetadataGeant4
        "src/*.cxx"
        "dict/*.cxx"
        )

set(MetadataGeant4 ${MetadataGeant4} PARENT_SCOPE)

set(METADATA_GEANT4_INC ${CMAKE_CURRENT_SOURCE_DIR}/inc PARENT_SCOPE)