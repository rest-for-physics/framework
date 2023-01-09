message(STATUS "Begin of external dependencies")

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
    set(gtest_force_shared_crt
        ON
        CACHE BOOL "" FORCE) # Windows only
    FetchContent_MakeAvailable(googletest)
endif ()

# # c++ modern string formatting (https://github.com/fmtlib/fmt) message(STATUS
# "- fmt") FetchContent_Declare( fmt GIT_REPOSITORY
# https://github.com/fmtlib/fmt.git GIT_TAG 9.0.0 )
# FetchContent_MakeAvailable(fmt) install(DIRECTORY ${fmt_SOURCE_DIR}/include
# DESTINATION .)
#
# set(local_include_dirs ${local_include_dirs} ${fmt_SOURCE_DIR}/include
# PARENT_SCOPE) set(external_libs "${external_libs};fmt" PARENT_SCOPE)
#
# message(STATUS "End of external dependencies")
