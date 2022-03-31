
message(STATUS "External dependencies:")

include(FetchContent)
set(FETCHCONTENT_QUIET ON)

# Google Test
if (TEST)
    # https://google.github.io/googletest/quickstart-cmake.html
    message(STATUS "- Google Test")
    FetchContent_Declare(
            googletest
            URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
    )
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE) # Windows only
    FetchContent_MakeAvailable(googletest)
endif ()

include(FetchContent)
FetchContent_Declare(cpr GIT_REPOSITORY https://github.com/libcpr/cpr.git
        GIT_TAG 21f42cf882d0b7e5ae9e3434574fc47e187728de)
# The commit hash for 1.8.0. Replace with the latest from: https://github.com/libcpr/cpr/releases
FetchContent_MakeAvailable(cpr)
