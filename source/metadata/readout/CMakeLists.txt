#Generate CINT file
set(CLASS_DICT TRestReadout TRestReadoutPlane TRestReadoutModule TRestReadoutChannel TRestReadoutPixel TRestReadoutMapping)
foreach (CLASS ${CLASS_DICT})
    message("Generate dict file for " ${CLASS})
    set(INC -I. -I./inc -I${RESTCORE_INC} -I${ROOT_INCLUDE_DIRS})
    execute_process(COMMAND rootcint -f ${CMAKE_CURRENT_SOURCE_DIR}/dict/${CLASS}.cxx -c ${INC} ${CMAKE_CURRENT_SOURCE_DIR}/inc/${CLASS}.h)
endforeach (CLASS)

file(GLOB MetadataReadout
        "src/*.cxx"
        "dict/*.cxx"
        )

set(MetadataReadout ${MetadataReadout} PARENT_SCOPE)

set(METADATA_READOUT_INC ${CMAKE_CURRENT_SOURCE_DIR}/inc PARENT_SCOPE)