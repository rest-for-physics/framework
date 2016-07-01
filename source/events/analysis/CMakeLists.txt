#Generate CINT file
set(CLASS_DICT TRestTrackEvent TRestTrack TRestLinearizedTrackEvent TRestLinearTrack)
foreach (CLASS ${CLASS_DICT})
    message("Generate dict file for " ${CLASS})
    set(INC -I. -I./inc -I${RESTCORE_INC} -I${ROOT_INCLUDE_DIRS} -I${EVENTS_GENERAL_INC})
    execute_process(COMMAND rootcint -f ${CMAKE_CURRENT_SOURCE_DIR}/dict/${CLASS}.cxx -c ${INC} ${CMAKE_CURRENT_SOURCE_DIR}/inc/${CLASS}.h)
endforeach (CLASS)

file(GLOB EventsAnalysis
        "src/*.cxx"
        "dict/*.cxx"
        )

set(EventsAnalysis ${EventsAnalysis} PARENT_SCOPE)
set(EVENTS_ANALYSIS_INC ${CMAKE_CURRENT_SOURCE_DIR}/inc PARENT_SCOPE)