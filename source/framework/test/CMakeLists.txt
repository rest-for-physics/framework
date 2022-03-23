set(TESTING_EXECUTABLE testFramework)

enable_testing()

add_executable(${TESTING_EXECUTABLE})

FILE(GLOB SOURCES src/*.cxx)
target_sources(
        ${TESTING_EXECUTABLE} PUBLIC
        ${SOURCES}
)

target_link_libraries(
        ${TESTING_EXECUTABLE} PUBLIC
        gtest_main
        RestFramework
        stdc++fs # <filesystem>
)

include(GoogleTest)

gtest_discover_tests(${TESTING_EXECUTABLE})
