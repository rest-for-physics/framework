#Generate CINT file
set(CLASS_DICT TRestG4Event TRestG4Hits TRestG4Track)
foreach (CLASS ${CLASS_DICT})
    message("Generate dict file for " ${CLASS})
    set(INC -I. -I./inc -I${RESTCORE_INC} -I${ROOT_INCLUDE_DIRS} -I${EVENTS_GENERAL_INC})
    execute_process(COMMAND rootcint -f ${CMAKE_CURRENT_SOURCE_DIR}/dict/${CLASS}.cxx -c ${INC} ${CMAKE_CURRENT_SOURCE_DIR}/inc/${CLASS}.h)
endforeach (CLASS)

file(GLOB EventsGeant4
        "src/*.cxx"
        "dict/*.cxx"
        )

set(EventsGeant4 ${EventsGeant4} PARENT_SCOPE)
set(EVENTS_GEANT4_INC ${CMAKE_CURRENT_SOURCE_DIR}/inc PARENT_SCOPE)