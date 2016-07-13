add_subdirectory(general)
set(PROCESS_GENERAL_INC ${PROCESS_GENERAL_INC} PARENT_SCOPE)

add_subdirectory(analysis)
set(PROCESS_ANALYSIS_INC ${PROCESS_ANALYSIS_INC} PARENT_SCOPE)

add_subdirectory(raw)
set(PROCESS_RAW_INC ${PROCESS_RAW_INC} PARENT_SCOPE)

add_subdirectory(signal)
set(PROCESS_SIGNAL_INC ${PROCESS_SIGNAL_INC} PARENT_SCOPE)

add_library(RestProcesses SHARED ${ProcessGeneral} ${ProcessAnalysis} ${ProcessRaw} ${ProcessSignal})
#add_library(RestProcessesStatic STATIC ${ProcessGeneral} ${ProcessAnalysis} ${ProcessRaw} ${ProcessSignal})

install(TARGETS RestProcesses
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib/static)