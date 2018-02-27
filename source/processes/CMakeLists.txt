#Add General
file(GLOB ProcessesGeneral
        "general/src/*.cxx"
        )
    set(CLASS_DICT  TRestElectronDiffusionProcess TRestGarfieldDriftProcess TRestFiducializationProcess TRestHitsReductionProcess TRestHitsShuffleProcess TRestHitsToSignalProcess TRestHitsToTrackProcess TRestTrackToHitsProcess TRestFastHitsToTrackProcess TRestSignalToRawSignalProcess TRestSignalToHitsProcess TRestSignalZeroSuppresionProcess TRestTrackReductionProcess TRestTrackPathMinimizationProcess TRestTrackReconnectionProcess TRestTrackLinearizationProcess TRestAvalancheProcess TRestHitsRotateAndTraslateProcess TRestSmearingProcess TRestHitsNormalizationProcess TRestG4toHitsProcess TRestRawSignalToSignalProcess )
foreach (CLASS ${CLASS_DICT})
    set(ROOT_DICT_INCLUDE_DIRS ${REST_INCLUDE_DIR} ${ROOT_INCLUDE_DIRS} ${Garfield_INCLUDE_DIRS})
    set(ROOT_DICT_INPUT_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/general/inc/${CLASS}.h)
    GEN_ROOT_DICT_SOURCES(${CLASS}.cxx)
    LIST(APPEND ProcessesGeneral ${ROOT_DICT_OUTPUT_SOURCES})
endforeach (CLASS)


#Add analysis
file(GLOB ProcessesAnalysis
        "analysis/src/*.cxx"
        )
    set(CLASS_DICT TRestGeant4AnalysisProcess TRestRawSignalAnalysisProcess TRestSignalViewerProcess TRestRawSignalViewerProcess TRestFindG4BlobAnalysisProcess TRestTrackAnalysisProcess TRestTriggerAnalysisProcess TRestHitsAnalysisProcess TRestPointLikeAnaProcess TRestPointLikeTrackAnaProcess TRestFindTrackBlobsProcess)
foreach (CLASS ${CLASS_DICT})
    set(ROOT_DICT_INCLUDE_DIRS ${REST_INCLUDE_DIR} ${ROOT_INCLUDE_DIRS} ${Garfield_INCLUDE_DIRS})
    set(ROOT_DICT_INPUT_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/analysis/inc/${CLASS}.h)
    GEN_ROOT_DICT_SOURCES(${CLASS}.cxx)
    LIST(APPEND ProcessesAnalysis ${ROOT_DICT_OUTPUT_SOURCES})
endforeach (CLASS)


#Add raw
file(GLOB ProcessesRaw
        "raw/src/*.cxx"
        )
    set(CLASS_DICT TRestRawToSignalProcess TRestFEMINOSToSignalProcess TRestMultiFEMINOSToSignalProcess TRestMultiCoBoAsAdToSignalProcess TRestCoBoAsAdToSignalProcess TRestAFTERToSignalProcess)
foreach (CLASS ${CLASS_DICT})
    set(ROOT_DICT_INCLUDE_DIRS ${REST_INCLUDE_DIR} ${ROOT_INCLUDE_DIRS})
    set(ROOT_DICT_INPUT_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/raw/inc/${CLASS}.h)
    GEN_ROOT_DICT_SOURCES(${CLASS}.cxx)
    LIST(APPEND ProcessesRaw ${ROOT_DICT_OUTPUT_SOURCES})
endforeach (CLASS)


#Add signal
file(GLOB ProcessesSignal
        "signal/src/*.cxx"
        )
    set(CLASS_DICT TRestSignalDeconvolutionProcess TRestAddSignalNoiseProcess TRestSignalGaussianConvolutionProcess TRestSignalShapingProcess TRestFindResponseSignalProcess TRestRawSignalRemoveChannelsProcess TRestRawSignalRecoverChannelsProcess)
foreach (CLASS ${CLASS_DICT})
    set(ROOT_DICT_INCLUDE_DIRS ${REST_INCLUDE_DIR} ${ROOT_INCLUDE_DIRS})
    set(ROOT_DICT_INPUT_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/signal/inc/${CLASS}.h)
    GEN_ROOT_DICT_SOURCES(${CLASS}.cxx)
    LIST(APPEND ProcessesSignal ${ROOT_DICT_OUTPUT_SOURCES})
endforeach (CLASS)


add_library(RestProcesses SHARED ${ProcessesGeneral} ${ProcessesAnalysis} ${ProcessesRaw} ${ProcessesSignal})
target_link_libraries(RestProcesses ${ROOT_LIBRARIES} -lGui -lEve -lGeom ${Garfield_LIBRARIES})
#add_library(RestProcessesStatic STATIC ${ProcessGeneral} ${ProcessAnalysis} ${ProcessRaw} ${ProcessSignal})

install(TARGETS RestProcesses
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib/static)
