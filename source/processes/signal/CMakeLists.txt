#Generate CINT file
set(CLASS_DICT TRestSignalDeconvolutionProcess TRestAddSignalNoiseProcess TRestSignalGaussianConvolutionProcess TRestSignalShapingProcess TRestSignalToRawSignalProcess)
foreach (CLASS ${CLASS_DICT})
    message("Generate dict file for " ${CLASS})
    set(INC -I. -I./inc -I${RESTCORE_INC} -I${METADATA_GEANT4_INC} -I${EVENTS_GEANT4_INC} -I${EVENTS_GENERAL_INC} -I${EVENTS_ANALYSIS_INC} -I${METADATA_GENERAL_INC} -I${METADATA_READOUT_INC} -I${ROOT_INCLUDE_DIRS})
    execute_process(COMMAND rootcint -f ${CMAKE_CURRENT_SOURCE_DIR}/dict/${CLASS}.cxx -c ${INC} ${CMAKE_CURRENT_SOURCE_DIR}/inc/${CLASS}.h)
endforeach (CLASS)

file(GLOB ProcessSignal
        "src/*.cxx"
        "dict/*.cxx"
        )

set(ProcessSignal ${ProcessSignal} PARENT_SCOPE)

set(PROCESS_SIGNAL_INC ${CMAKE_CURRENT_SOURCE_DIR}/inc PARENT_SCOPE)