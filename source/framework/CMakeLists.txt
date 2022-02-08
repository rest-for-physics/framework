# External dependencies

add_subdirectory(external)

set(contents external/tinyxml tools core analysis)

file(GLOB_RECURSE addon_src
        "tiny*cpp"
        "startup.cpp")

if (NOT ${REST_EVE} MATCHES "ON")
    set(excludes TRestEveEventViewer)
endif (NOT ${REST_EVE} MATCHES "ON")

COMPILEDIR(RestFramework)

if (NOT DEFINED TEST OR NOT TEST)
    set(TEST OFF)
    message(STATUS "Testing disabled (Disabled by default, enabled via -DTEST=ON flag)")
endif ()
if (TEST)
    message(STATUS "Testing enabled (Disabled by default, enabled via -DTEST=ON flag)")
    enable_testing()
endif ()

ADD_TEST()
