#Generate CINT file
set(CLASS_DICT TRestMesh TRestFFT TRestManager)
foreach (CLASS ${CLASS_DICT})
    message("Generate dict file for " ${CLASS})
    set(INC -I. -I./inc -I${RESTCORE_INC} -I${ROOT_INCLUDE_DIRS} -I${EVENTS_GENERAL_INC} )
    execute_process(COMMAND rootcint -f ${CMAKE_CURRENT_SOURCE_DIR}/dict/${CLASS}.cxx -c ${INC} ${CMAKE_CURRENT_SOURCE_DIR}/inc/${CLASS}.h)
endforeach (CLASS)

file(GLOB RestToolsSrc
        "src/*.cxx"
        "dict/*.cxx"
        )

add_library(RestTools ${RestToolsSrc})

set(RESTTOOLS_INC ${CMAKE_CURRENT_SOURCE_DIR}/inc PARENT_SCOPE)