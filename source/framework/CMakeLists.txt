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

find_library(CURL_LIB curl)
if (NOT ${CURL_LIB} STREQUAL "CURL_LIB-NOTFOUND")
    add_compile_definitions(USE_Curl)
    message(STATUS "Found curl library: ${CURL_LIB}")
    set(external_libs "${external_libs};-lcurl;-lstdc++fs")
endif ()

find_path(MPFR_INCLUDE mpfr.h HINTS ${MPFR_PATH}) # path to include directory
find_library(MPFR_LIB mpfr HINTS ${MPFR_PATH}) # path to .so file

if (NOT MPFR_INCLUDE MATCHES "MPFR_INCLUDE-NOTFOUND" AND NOT MPFR_LIB MATCHES "MPFR_LIB-NOTFOUND")
    # MPFR is found
    add_compile_definitions(USE_MPFR)
    set(external_include_dirs ${external_include_dirs} ${MPFR_INCLUDE})
    set(external_libs "${external_libs};-lmpfr")
    link_directories(${MPFR_LIB})
else ()
    message(WARNING "MPFR library was not found. Adding the location to the installation path may solve this problem.\n Use -DMPFR_PATH=/path/to/mpfr/")
endif ()

COMPILEDIR(RestFramework)

ADD_LIBRARY_TEST()
