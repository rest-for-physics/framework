message(STATUS "LOADING GENERATE ROOT DICTIONARIES MACRO")
if (NOT ${FIND_ROOT})
    message(ERROR "ROOT NOT FOUND")
endif ()

set(MODULEMAP_FILENAME ${CMAKE_BINARY_DIR}/module.modulemap)
file(WRITE ${MODULEMAP_FILENAME})
install(FILES ${MODULEMAP_FILENAME} DESTINATION lib)

macro(THIS_GENERATE_ROOT_DICTIONARIES HEADERS)
    # Create LinkDef.h file
    set(LINKDEF_FILE ${CMAKE_CURRENT_BINARY_DIR}/LinkDef.h)
    file(WRITE ${LINKDEF_FILE})
    file(APPEND ${LINKDEF_FILE} "#ifdef __CLING__\n")
    file(APPEND ${LINKDEF_FILE} "#pragma link off all globals;\n")
    file(APPEND ${LINKDEF_FILE} "#pragma link off all classes;\n")
    file(APPEND ${LINKDEF_FILE} "#pragma link off all functions;\n")
    file(APPEND ${LINKDEF_FILE} "#pragma link C++ nestedclasses;\n")

    foreach (header ${HEADERS})
        get_filename_component(class ${header} NAME_WE)
        file(APPEND ${LINKDEF_FILE} "#pragma link C++ class ${class}+;\n")
    endforeach ()

    file(APPEND ${LINKDEF_FILE} "#endif\n")

    ROOT_GENERATE_DICTIONARY(${LIBRARY}RootDict ${HEADERS} LINKDEF ${LINKDEF_FILE} MODULE ${LIBRARY})

    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/lib${LIBRARY}_rdict.pcm DESTINATION lib)
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/lib${LIBRARY}.rootmap DESTINATION lib)

    file(READ ${CMAKE_CURRENT_BINARY_DIR}/module.modulemap MODULEMAP_CONTENTS)

    string(REGEX REPLACE "${CMAKE_CURRENT_SOURCE_DIR}/include" "${CMAKE_INSTALL_PREFIX}/include" MODULEMAP_CONTENTS ${MODULEMAP_CONTENTS})

    file(APPEND ${MODULEMAP_FILENAME} "${MODULEMAP_CONTENTS}\n")

endmacro()