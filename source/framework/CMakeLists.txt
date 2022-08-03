
# '-fPIC' flag necessary on some systems to prevent error
add_compile_options("-fPIC")
link_libraries("-fPIC")

add_subdirectory(external)

set(contents external/tinyxml tools core analysis masks)

file(GLOB_RECURSE addon_src
        "tiny*cpp"
        "startup.cpp")

if (NOT ${REST_EVE} MATCHES "ON")
    set(excludes TRestEveEventViewer)
endif (NOT ${REST_EVE} MATCHES "ON")

if (CMAKE_SYSTEM_NAME MATCHES "Windows")
    set(excludes TRestBenchMarkProcess TRestRealTimeAddInputFileProcess TRestRealTimeDrawingProcess TRestMessenger ${excludes})
endif (CMAKE_SYSTEM_NAME MATCHES "Windows")

set(external_libs "${external_libs};-lstdc++fs") # sometimes needed for <filesystem>

find_library(CURL_LIB curl)
if (NOT ${CURL_LIB} STREQUAL "CURL_LIB-NOTFOUND")
    add_compile_definitions(USE_Curl)
    message(STATUS "Found curl library: ${CURL_LIB}")
    set(external_libs "${external_libs};-lcurl")
endif ()

# This might be needed for external deps
# set(rest_include_dirs ${rest_include_dirs} ${CMAKE_INSTALL_PREFIX}/include)

COMPILEDIR(RestFramework)

ADD_LIBRARY_TEST()
