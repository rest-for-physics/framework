#Add General
set(PROCESS_GENERAL_INC ${CMAKE_CURRENT_SOURCE_DIR}/general/inc)
set(PROCESS_GENERAL_INC ${CMAKE_CURRENT_SOURCE_DIR}/general/inc PARENT_SCOPE)
file(GLOB ProcessesGeneral
        "general/src/*.cxx"
        )
set(CLASS_DICT  TRestElectronDiffusionProcess TRestHitsToSignalProcess TRestHitsToTrackProcess TRestFastHitsToTrackProcess TRestSignalToHitsProcess
        TRestTrackReductionProcess TRestTrackPathMinimizationProcess TRestTrackLinearizationProcess
        TRestAvalancheProcess TRestHitsRotateAndTraslateProcess TRestSmearingProcess TRestHitsNormalizationProcess)
foreach (CLASS ${CLASS_DICT})
    set(ROOT_DICT_INCLUDE_DIRS ${RESTCORE_INC} ${ROOT_INCLUDE_DIRS} ${METADATA_GENERAL_INC} ${METADATA_READOUT_INC} ${EVENTS_ANALYSIS_INC} ${EVENTS_GENERAL_INC} ${EVENTS_GEANT4_INC})
    set(ROOT_DICT_INPUT_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/general/inc/${CLASS}.h)
    GEN_ROOT_DICT_SOURCES(${CLASS}.cxx)
    LIST(APPEND ProcessesGeneral ${ROOT_DICT_OUTPUT_SOURCES})
endforeach (CLASS)


#Add analysis
set(PROCESS_ANALYSIS_INC ${CMAKE_CURRENT_SOURCE_DIR}/analysis/inc)
set(PROCESS_ANALYSIS_INC ${CMAKE_CURRENT_SOURCE_DIR}/analysis/inc PARENT_SCOPE)
file(GLOB ProcessesAnalysis
        "analysis/src/*.cxx"
        )
set(CLASS_DICT TRestGeant4AnalysisProcess TRestSignalAnalysisProcess TRestFindG4BlobAnalysisProcess TRestTrackAnalysisProcess TRestTriggerAnalysisProcess TRestHitsAnalysisProcess TRestPointLikeAnaProcess TRestPointLikeTrackAnaProcess)
foreach (CLASS ${CLASS_DICT})
    set(ROOT_DICT_INCLUDE_DIRS ${RESTCORE_INC} ${ROOT_INCLUDE_DIRS} ${METADATA_GENERAL_INC} ${METADATA_GEANT4_INC} ${METADATA_READOUT_INC} ${EVENTS_ANALYSIS_INC} ${EVENTS_GENERAL_INC} ${EVENTS_GEANT4_INC} ${PROCESS_GENERAL_INC})
    set(ROOT_DICT_INPUT_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/analysis/inc/${CLASS}.h)
    GEN_ROOT_DICT_SOURCES(${CLASS}.cxx)
    LIST(APPEND ProcessesAnalysis ${ROOT_DICT_OUTPUT_SOURCES})
endforeach (CLASS)


#Add raw
set(PROCESS_RAW_INC ${CMAKE_CURRENT_SOURCE_DIR}/raw/inc)
set(PROCESS_RAW_INC ${CMAKE_CURRENT_SOURCE_DIR}/raw/inc PARENT_SCOPE)
file(GLOB ProcessesRaw
        "raw/src/*.cxx"
        )
set(CLASS_DICT TRestRawToSignalProcess TRestFEMINOSToSignalProcess TRestAFTERToSignalProcess)
foreach (CLASS ${CLASS_DICT})
    set(ROOT_DICT_INCLUDE_DIRS ${RESTCORE_INC} ${ROOT_INCLUDE_DIRS} ${METADATA_GENERAL_INC} ${METADATA_READOUT_INC} ${EVENTS_ANALYSIS_INC} ${EVENTS_GENERAL_INC} ${EVENTS_GEANT4_INC} ${PROCESS_GENERAL_INC})
    set(ROOT_DICT_INPUT_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/raw/inc/${CLASS}.h)
    GEN_ROOT_DICT_SOURCES(${CLASS}.cxx)
    LIST(APPEND ProcessesRaw ${ROOT_DICT_OUTPUT_SOURCES})
endforeach (CLASS)


#Add signal
set(PROCESS_SIGNAL_INC ${CMAKE_CURRENT_SOURCE_DIR}/signal/inc)
set(PROCESS_SIGNAL_INC ${CMAKE_CURRENT_SOURCE_DIR}/signal/inc PARENT_SCOPE)
file(GLOB ProcessesSignal
        "signal/src/*.cxx"
        )
set(CLASS_DICT TRestSignalDeconvolutionProcess TRestAddSignalNoiseProcess TRestSignalGaussianConvolutionProcess TRestSignalShapingProcess TRestSignalToRawSignalProcess)
foreach (CLASS ${CLASS_DICT})
    set(ROOT_DICT_INCLUDE_DIRS ${RESTCORE_INC} ${ROOT_INCLUDE_DIRS} ${METADATA_GENERAL_INC} ${METADATA_READOUT_INC} ${EVENTS_ANALYSIS_INC} ${EVENTS_GENERAL_INC} ${EVENTS_GEANT4_INC} ${PROCESS_GENERAL_INC})
    set(ROOT_DICT_INPUT_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/signal/inc/${CLASS}.h)
    GEN_ROOT_DICT_SOURCES(${CLASS}.cxx)
    LIST(APPEND ProcessesSignal ${ROOT_DICT_OUTPUT_SOURCES})
endforeach (CLASS)


add_library(RestProcesses SHARED ${ProcessesGeneral} ${ProcessesAnalysis} ${ProcessesRaw} ${ProcessesSignal})
#add_library(RestProcessesStatic STATIC ${ProcessGeneral} ${ProcessAnalysis} ${ProcessRaw} ${ProcessSignal})

install(TARGETS RestProcesses
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib/static)
