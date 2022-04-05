add_subdirectory(external)

set(contents external/tinyxml tools core analysis)

file(GLOB_RECURSE addon_src
        "tiny*cpp"
        "startup.cpp")

if (NOT ${REST_EVE} MATCHES "ON")
    set(excludes TRestEveEventViewer)
endif (NOT ${REST_EVE} MATCHES "ON")

find_library(CURL_LIB curl)
if (NOT "${CURL_LIB}" MATCHES "NOTFOUND")
    add_definitions(-DUSE_Curl)
    message(STATUS "Found CURL: ${CURL_LIB}")
    set(external_libs "${external_libs};-lcurl")
endif (NOT "${CURL_LIB}" MATCHES "NOTFOUND")

COMPILEDIR(RestFramework)

ADD_LIBRARY_TEST()
