include_directories(inc)

#Generate CINT file
set(CLASS_DICT TRestEvent TRestMetadata TRestEventProcess TRestRun TRestGeometry TRestRunMerger TRestEventViewer TRestBrowser)
foreach (CLASS ${CLASS_DICT})
    message("Generate dict file for " ${CLASS})
    execute_process(COMMAND rootcint -f ${CMAKE_CURRENT_SOURCE_DIR}/dict/${CLASS}.cxx -c ${CMAKE_CURRENT_SOURCE_DIR}/inc/${CLASS}.h)
endforeach (CLASS)

file(GLOB RestCoreSrc
        "src/*.cxx"
        "dict/*.cxx"
        )

add_library(RestCore ${RestCoreSrc})

set(RESTCORE_INC ${CMAKE_CURRENT_SOURCE_DIR}/inc PARENT_SCOPE)