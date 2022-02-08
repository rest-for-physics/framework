
message(STATUS "External dependencies:")

set(FETCHCONTENT_QUIET ON)

include(FetchContent)

# Google Test
if (TEST)
    message(STATUS "- Google Test")
    FetchContent_Declare(
            googletest
            URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
    )
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE) # Windows only
    FetchContent_MakeAvailable(googletest)
endif ()

# YAML parser
message(STATUS "- yaml-cpp")
FetchContent_Declare(
        yaml-cpp
        GIT_REPOSITORY https://github.com/jbeder/yaml-cpp.git
        GIT_TAG yaml-cpp-0.7.0
)
FetchContent_MakeAvailable(yaml-cpp)
install(DIRECTORY ${yaml-cpp_SOURCE_DIR}/include DESTINATION .)
install(TARGETS yaml-cpp DESTINATION lib)

# Logging library
message(STATUS "- spdlog")
FetchContent_Declare(
        spdlog
        GIT_REPOSITORY https://github.com/gabime/spdlog.git
        GIT_TAG v1.9.2
)
FetchContent_MakeAvailable(spdlog)
install(DIRECTORY ${spdlog_SOURCE_DIR}/include DESTINATION .)
install(TARGETS spdlog DESTINATION lib)
