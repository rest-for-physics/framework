
if (NOT DEFINED TEST OR NOT TEST)
    set(TEST OFF)
    message(STATUS "Testing disabled (Disabled by default, enabled via -DTEST=ON flag)")
endif ()
if (TEST)
    message(STATUS "Testing enabled (Disabled by default, enabled via -DTEST=ON flag)")
    enable_testing()
    add_compile_definitions(REST_TESTING_ENABLED)
endif ()

macro(ADD_TEST)
    if (TEST)
        message(STATUS "Adding tests at ${CMAKE_CURRENT_SOURCE_DIR}")
        add_subdirectory(test)
    endif ()
endmacro()

macro(ADD_LIBRARY_TEST)
    if (TEST)
        message(STATUS "Adding tests at ${CMAKE_CURRENT_SOURCE_DIR}")
        set(TESTING_EXECUTABLE testRestGeant4)

        get_filename_component(DIR_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)

        string(SUBSTRING ${DIR_NAME} 0 1 FIRST_LETTER)
        string(TOUPPER ${FIRST_LETTER} FIRST_LETTER)
        string(REGEX REPLACE "^.(.*)" "${FIRST_LETTER}\\1" DIR_NAME_CAPITALIZED "${DIR_NAME}")

        set(LIBRARY_NAME "Rest${DIR_NAME_CAPITALIZED}")

        set(TESTING_EXECUTABLE "test${LIBRARY_NAME}")

        enable_testing()

        add_executable(${TESTING_EXECUTABLE})

        FILE(GLOB SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/test/src/*.cxx)
        target_sources(
                ${TESTING_EXECUTABLE} PUBLIC
                ${SOURCES}
        )

        target_link_libraries(
                ${TESTING_EXECUTABLE} PUBLIC
                ${LIBRARY_NAME}
                gtest_main
                RestFramework
                stdc++fs # <filesystem>
        )

        include(GoogleTest)

        gtest_discover_tests(${TESTING_EXECUTABLE})

    endif ()
endmacro()
