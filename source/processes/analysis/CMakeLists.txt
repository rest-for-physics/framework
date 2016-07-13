#Generate CINT file
set(CLASS_DICT TRestGeant4AnalysisProcess TRestSignalAnalysisProcess TRestFindG4BlobAnalysisProcess TRestTrackAnalysisProcess TRestTriggerAnalysisProcess TRestHitsAnalysisProcess)
        foreach (CLASS ${CLASS_DICT})
    message("Generate dict file for " ${CLASS})
    set(INC -I. -I./inc -I${RESTCORE_INC} -I${METADATA_GEANT4_INC} -I${EVENTS_GEANT4_INC} -I${EVENTS_GENERAL_INC} -I${EVENTS_ANALYSIS_INC} -I${METADATA_GENERAL_INC} -I${METADATA_READOUT_INC} -I${ROOT_INCLUDE_DIRS})
    execute_process(COMMAND rootcint -f ${CMAKE_CURRENT_SOURCE_DIR}/dict/${CLASS}.cxx -c ${INC} ${CMAKE_CURRENT_SOURCE_DIR}/inc/${CLASS}.h)
endforeach (CLASS)

file(GLOB ProcessAnalysis
        "src/*.cxx"
        "dict/*.cxx"
        )

set(ProcessAnalysis ${ProcessAnalysis} PARENT_SCOPE)

set(PROCESS_ANALYSIS_INC ${CMAKE_CURRENT_SOURCE_DIR}/inc PARENT_SCOPE)