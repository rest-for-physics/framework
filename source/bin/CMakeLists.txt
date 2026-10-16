include_directories(${rest_include_dirs})
include_directories(${external_include_dirs})

set(rest_exes)

set(files restManager.cxx restRoot.cxx)

foreach (file ${files})
    # Get file name without extension
    string(REGEX MATCH "[^/\\]*cxx" temp ${file})
    string(REPLACE ".cxx" "" name ${temp})

    add_executable(${name} ${file})
    target_link_libraries(${name} ${rest_libraries} ${external_libs})
    install(TARGETS ${name} RUNTIME DESTINATION bin)
    set(rest_exes ${rest_exes} ${name})

endforeach (file)

if (TEST)
    find_package(Python3 REQUIRED COMPONENTS Interpreter)
    add_test(
        NAME RestRoot.ExpressionArguments
        COMMAND
            ${Python3_EXECUTABLE}
            ${CMAKE_CURRENT_SOURCE_DIR}/../../pipeline/restRoot/test_expression_arguments.py
            $<TARGET_FILE:restRoot>)
endif ()

set(rest_exes
    ${rest_exes}
    PARENT_SCOPE)
