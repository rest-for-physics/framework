include_directories(${rest_include_dirs})
include_directories(${external_include_dirs})

set(rest_exes)

set(files restManager.cxx restRoot.cxx)

foreach (file ${files})
    # Get file name without extension
    string(REGEX MATCH "[^/\\]*cxx" temp ${file})
    string(REPLACE ".cxx" "" name ${temp})

    if (name STREQUAL "restRoot")
        add_executable(${name} ${file} LegacySignalRecoveryCLI.cxx)
        target_compile_definitions(
            ${name} PRIVATE REST_ROOT_EXECUTABLE="${ROOT_root_CMD}")
    else ()
        add_executable(${name} ${file})
    endif ()
    target_link_libraries(${name} ${rest_libraries} ${external_libs})
    install(TARGETS ${name} RUNTIME DESTINATION bin)
    set(rest_exes ${rest_exes} ${name})

endforeach (file)

if (TEST)
    set(LEGACY_SIGNAL_RECOVERY_CLI_TEST_SOURCE
        test/LegacySignalRecoveryCLI.cxx)
    add_executable(
        testRestRootLegacySignalRecoveryProcessProbe
        test/LegacySignalRecoveryProcessProbe.cxx)
    add_executable(
        testRestRootLegacySignalRecovery
        ${LEGACY_SIGNAL_RECOVERY_CLI_TEST_SOURCE}
        LegacySignalRecoveryCLI.cxx)
    target_include_directories(
        testRestRootLegacySignalRecovery PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
    target_link_libraries(
        testRestRootLegacySignalRecovery PRIVATE gtest_main)
    target_compile_definitions(
        testRestRootLegacySignalRecovery
        PRIVATE
            REST_LEGACY_RECOVERY_PROCESS_PROBE="$<TARGET_FILE:testRestRootLegacySignalRecoveryProcessProbe>"
    )
    add_dependencies(
        testRestRootLegacySignalRecovery
        testRestRootLegacySignalRecoveryProcessProbe)
    include(GoogleTest)
    gtest_add_tests(
        TARGET testRestRootLegacySignalRecovery
        SOURCES ${LEGACY_SIGNAL_RECOVERY_CLI_TEST_SOURCE})
endif ()

set(rest_exes
    ${rest_exes}
    PARENT_SCOPE)
