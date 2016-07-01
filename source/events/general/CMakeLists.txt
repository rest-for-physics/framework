#Generate CINT file
set(CLASS_DICT TRestSignalEvent TRestHitsEvent TRestSignal TRestHits TRestVolumeHits)
foreach (CLASS ${CLASS_DICT})
    message("Generate dict file for " ${CLASS})
    set(INC -I. -I./inc -I${RESTCORE_INC} -I${ROOT_INCLUDE_DIRS})
    execute_process(COMMAND rootcint -f ${CMAKE_CURRENT_SOURCE_DIR}/dict/${CLASS}.cxx -c ${INC} ${CMAKE_CURRENT_SOURCE_DIR}/inc/${CLASS}.h)
endforeach (CLASS)

file(GLOB EventsGeneral
        "src/*.cxx"
        "dict/*.cxx"
        )

set(EventsGeneral ${EventsGeneral} PARENT_SCOPE)

set(EVENTS_GENERAL_INC ${CMAKE_CURRENT_SOURCE_DIR}/inc PARENT_SCOPE)