#Generate CINT file
set(CLASS_DICT TRestG4EventViewer)
foreach (CLASS ${CLASS_DICT})
    message("Generate dict file for " ${CLASS})
    set(INC -I. -I./inc -I${RESTCORE_INC} -I${EVENTS_GEANT4_INC} -I${EVENTS_GENERAL_INC} -I${VIEWERS_GENERAL_INC} -I${ROOT_INCLUDE_DIRS})
    execute_process(COMMAND rootcint -f ${CMAKE_CURRENT_SOURCE_DIR}/dict/${CLASS}.cxx -c ${INC} ${CMAKE_CURRENT_SOURCE_DIR}/inc/${CLASS}.h)
endforeach (CLASS)

file(GLOB ViewersGeant4
        "src/*.cxx"
        "dict/*.cxx"
        )

set(ViewersGeant4 ${ViewersGeant4} PARENT_SCOPE)

set(VIEWERS_GEANT4_INC ${CMAKE_CURRENT_SOURCE_DIR}/inc PARENT_SCOPE)