#Generate CINT file
set(CLASS_DICT TRestEvent TRestMetadata TRestEventProcess TRestRun TRestGeometry TRestRunMerger TRestEventViewer TRestBrowser TRestAnalysisTree TRestAnalysisPlot)

file(GLOB RestCoreSrc
        "src/*.cxx"
        )

foreach (CLASS ${CLASS_DICT})
    set(ROOT_DICT_INCLUDE_DIRS )
    set(ROOT_DICT_INPUT_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/inc/${CLASS}.h)
    GEN_ROOT_DICT_SOURCES(${CLASS}.cxx)
    LIST(APPEND RestCoreSrc ${ROOT_DICT_OUTPUT_SOURCES})
endforeach (CLASS)

add_library(RestCore SHARED ${RestCoreSrc})
target_link_libraries(RestCore Garfield)
#add_library(RestCoreStatic STATIC ${RestCoreSrc})

install(TARGETS RestCore
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib/static)

set(RESTCORE_INC ${CMAKE_CURRENT_SOURCE_DIR}/inc PARENT_SCOPE)