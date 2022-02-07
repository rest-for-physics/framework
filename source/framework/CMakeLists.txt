
set(contents external/tinyxml tools core analysis)
message(STATUS "CONTENTS: ${contents}")

file(GLOB_RECURSE addon_src
        "tiny*cpp"
        "startup.cpp")

if (NOT ${REST_EVE} MATCHES "ON")
    set(excludes TRestEveEventViewer)
endif (NOT ${REST_EVE} MATCHES "ON")

# COMPILEDIR(RestFramework)

set(LIBRARY RestFramework)

set(SOURCES)
set(HEADERS)

add_subdirectory(core)
# add_subdirectory(analysis)

message(STATUS "HEADERS: ${HEADERS}")
message(STATUS "SOURCES: ${SOURCES}")

add_library(${LIBRARY} SHARED)
install(TARGETS ${LIBRARY} DESTINATION lib)

string(STRIP "${SOURCES}" SOURCES)
message(STATUS "SOURCES: ${SOURCES}")

target_sources(
        ${LIBRARY} PUBLIC
        ${SOURCES}
)

target_include_directories(
        ${LIBRARY} SYSTEM PUBLIC
        ${ROOT_INCLUDE_DIRS}
        ${HEADERS}
)

target_link_libraries(
        ${LIBRARY} PUBLIC
        ${ROOT_LIBRARIES}
)

return()

THIS_GENERATE_ROOT_DICTIONARIES(RestFrameWork)
