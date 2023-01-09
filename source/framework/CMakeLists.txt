# '-fPIC' flag necessary on some systems to prevent error
add_compile_options("-fPIC")
link_libraries("-fPIC")

add_subdirectory(external)

set(contents external/tinyxml tools core analysis masks)

file(GLOB_RECURSE addon_src "tiny*cpp" "startup.cpp")

if (NOT ${REST_EVE} MATCHES "ON")
    set(excludes TRestEveEventViewer)
endif (NOT ${REST_EVE} MATCHES "ON")

if (CMAKE_SYSTEM_NAME MATCHES "Windows")
    set(excludes TRestBenchMarkProcess TRestRealTimeAddInputFileProcess
                 TRestRealTimeDrawingProcess TRestMessenger ${excludes})
endif (CMAKE_SYSTEM_NAME MATCHES "Windows")

# It is defined at the main CMakeLists.txt set(external_libs
# "${external_libs};-lstdc++fs") # sometimes needed for <filesystem>

compiledir(RestFramework)

add_library_test()
