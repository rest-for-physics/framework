#Generate CINT file
set(CLASS_DICT TRestElectronDiffusionProcess TRestHitsToSignalProcess TRestHitsToTrackProcess TRestFastHitsToTrackProcess TRestSignalToHitsProcess TRestTrackReductionProcess TRestTrackPathMinimizationProcess TRestAvalancheProcess TRestTrackLinearizationProcess TRestHitsRotateAndTraslateProcess)
foreach (CLASS ${CLASS_DICT})
    message("Generate dict file for " ${CLASS})
    set(INC -I. -I./inc -I${RESTCORE_INC} -I${EVENTS_GEANT4_INC} -I${EVENTS_GENERAL_INC} -I${EVENTS_ANALYSIS_INC} -I${METADATA_GENERAL_INC} -I${METADATA_READOUT_INC} -I${ROOT_INCLUDE_DIRS})
    execute_process(COMMAND rootcint -f ${CMAKE_CURRENT_SOURCE_DIR}/dict/${CLASS}.cxx -c ${INC} ${CMAKE_CURRENT_SOURCE_DIR}/inc/${CLASS}.h)
endforeach (CLASS)

file(GLOB ProcessGeneral
        "src/*.cxx"
        "dict/*.cxx"
        )

set(ProcessGeneral ${ProcessGeneral} PARENT_SCOPE)

set(PROCESS_GENERAL_INC ${CMAKE_CURRENT_SOURCE_DIR}/inc PARENT_SCOPE)