set(FIXTURE_OUTPUT_DIR "${CMAKE_CURRENT_BINARY_DIR}/artifacts")

function (add_rest_io_fixture version)
    set(target "RestIOFixtureV${version}")
    set(source_dir "${CMAKE_CURRENT_SOURCE_DIR}/v${version}")
    set(dictionary "G__${target}")
    root_generate_dictionary(
        ${dictionary} "${source_dir}/TRestIOFixturePayload.h" LINKDEF
        "${source_dir}/LinkDef.h")
    add_library(
        ${target} SHARED "${source_dir}/TRestIOFixturePayload.cxx"
                         "${CMAKE_CURRENT_BINARY_DIR}/${dictionary}.cxx")
    target_include_directories(${target} PUBLIC "${source_dir}")
    target_link_libraries(${target} PUBLIC ${ROOT_LIBRARIES})
    set_target_properties(
        ${target} PROPERTIES LIBRARY_OUTPUT_DIRECTORY
                             "${FIXTURE_OUTPUT_DIR}/v${version}")
    add_custom_command(
        TARGET ${target}
        POST_BUILD
        COMMAND
            ${CMAKE_COMMAND} -E copy_if_different
            "${CMAKE_CURRENT_BINARY_DIR}/lib${target}_rdict.pcm"
            "$<TARGET_FILE_DIR:${target}>"
        COMMAND
            ${CMAKE_COMMAND} -E copy_if_different
            "${CMAKE_CURRENT_BINARY_DIR}/lib${target}.rootmap"
            "$<TARGET_FILE_DIR:${target}>")

    add_executable("RestIOWriterV${version}" WriteFixture.cxx)
    target_link_libraries("RestIOWriterV${version}" PRIVATE ${target}
                                                            ${ROOT_LIBRARIES})
    set_target_properties(
        "RestIOWriterV${version}" PROPERTIES RUNTIME_OUTPUT_DIRECTORY
                                             "${FIXTURE_OUTPUT_DIR}/bin")
endfunction ()

add_rest_io_fixture(1)
add_rest_io_fixture(2)

add_executable(RestIOUpdater UpdateFixture.cxx)
target_link_libraries(RestIOUpdater PRIVATE RestFramework ${ROOT_LIBRARIES})
set_target_properties(RestIOUpdater PROPERTIES RUNTIME_OUTPUT_DIRECTORY
                                               "${FIXTURE_OUTPUT_DIR}/bin")

add_executable(RestIOLoadedUpdater UpdateFixtureLoaded.cxx)
target_link_libraries(RestIOLoadedUpdater PRIVATE RestIOFixtureV2 RestFramework
                                                  ${ROOT_LIBRARIES})
set_target_properties(
    RestIOLoadedUpdater PROPERTIES RUNTIME_OUTPUT_DIRECTORY
                                   "${FIXTURE_OUTPUT_DIR}/bin")

add_executable(RestIOVerifier VerifyFixture.cxx)
target_link_libraries(RestIOVerifier PRIVATE RestIOFixtureV2 ${ROOT_LIBRARIES})
set_target_properties(RestIOVerifier PROPERTIES RUNTIME_OUTPUT_DIRECTORY
                                                "${FIXTURE_OUTPUT_DIR}/bin")

add_executable(RestIOVerifierV1 VerifyFixtureV1.cxx)
target_link_libraries(RestIOVerifierV1 PRIVATE RestIOFixtureV1
                                               ${ROOT_LIBRARIES})
set_target_properties(RestIOVerifierV1 PROPERTIES RUNTIME_OUTPUT_DIRECTORY
                                                  "${FIXTURE_OUTPUT_DIR}/bin")

add_dependencies(
    testRestFramework
    RestIOWriterV1
    RestIOWriterV2
    RestIOUpdater
    RestIOLoadedUpdater
    RestIOVerifier
    RestIOVerifierV1)
target_compile_definitions(
    testRestFramework
    PRIVATE REST_IO_WRITER_V1="$<TARGET_FILE:RestIOWriterV1>"
            REST_IO_WRITER_V2="$<TARGET_FILE:RestIOWriterV2>"
            REST_IO_UPDATER="$<TARGET_FILE:RestIOUpdater>"
            REST_IO_LOADED_UPDATER="$<TARGET_FILE:RestIOLoadedUpdater>"
            REST_IO_VERIFIER="$<TARGET_FILE:RestIOVerifier>"
            REST_IO_VERIFIER_V1="$<TARGET_FILE:RestIOVerifierV1>")
