#Generate CINT file
set(CLASS_DICT TRestMesh TRestFFT TRestManager)

file(GLOB RestToolsSrc
        "src/*.cxx"
        )


foreach (CLASS ${CLASS_DICT})
    set(ROOT_DICT_INCLUDE_DIRS ${REST_INCLUDE_DIR} ${ROOT_INCLUDE_DIRS})
    set(ROOT_DICT_INPUT_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/inc/${CLASS}.h)
    GEN_ROOT_DICT_SOURCES(${CLASS}.cxx)
    LIST(APPEND RestToolsSrc ${ROOT_DICT_OUTPUT_SOURCES})
endforeach (CLASS)


add_library(RestTools SHARED ${RestToolsSrc})
#add_library(RestToolsStatic STATIC ${RestToolsSrc})

install(TARGETS RestTools
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib/static)

set(RESTTOOLS_INC SHARED ${CMAKE_CURRENT_SOURCE_DIR}/inc PARENT_SCOPE)