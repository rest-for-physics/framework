add_subdirectory(external)

set(contents external/tinyxml tools core analysis)

file(GLOB_RECURSE addon_src
        "tiny*cpp"
        "startup.cpp")

if (NOT ${REST_EVE} MATCHES "ON")
    set(excludes TRestEveEventViewer)
endif (NOT ${REST_EVE} MATCHES "ON")

if (CMAKE_SYSTEM_NAME MATCHES "Windows")
    set(excludes TRestBenchMarkProcess TRestRealTimeAddInputFileProcess TRestRealTimeDrawingProcess TRestMessenger ${excludes})
endif (CMAKE_SYSTEM_NAME MATCHES "Windows")

find_library(CURL_LIB curl)
if (NOT ${CURL_LIB} STREQUAL "CURL_LIB-NOTFOUND")
    add_compile_definitions(USE_Curl)
    message(STATUS "Found curl library: ${CURL_LIB}")
    set(external_libs "${external_libs};-lcurl;-lstdc++fs")
endif ()

COMPILEDIR(RestFramework)

ADD_LIBRARY_TEST()
