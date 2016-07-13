#Generate CINT file
set(CLASS_DICT TRestEvent TRestMetadata TRestEventProcess TRestRun TRestGeometry TRestRunMerger TRestEventViewer TRestBrowser TRestAnalysisTree TRestAnalysisPlot)
foreach (CLASS ${CLASS_DICT})
    message("Generate dict file for " ${CLASS})
    execute_process(COMMAND rootcint -f ${CMAKE_CURRENT_SOURCE_DIR}/dict/${CLASS}.cxx -c ${CMAKE_CURRENT_SOURCE_DIR}/inc/${CLASS}.h)
endforeach (CLASS)

file(GLOB RestCoreSrc
        "src/*.cxx"
        "dict/*.cxx"
        )

add_library(RestCore SHARED ${RestCoreSrc})
#add_library(RestCoreStatic STATIC ${RestCoreSrc})

install(TARGETS RestCore
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib/static)

set(RESTCORE_INC ${CMAKE_CURRENT_SOURCE_DIR}/inc PARENT_SCOPE)