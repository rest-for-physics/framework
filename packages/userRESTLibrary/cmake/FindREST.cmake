set(REST_DIR ${REST_PATH})

find_path(REST_INCLUDE_DIRS TRestEvent.h
        HINTS ${REST_DIR}/include $ENV{REST_PATH}/include)


find_path(REST_LIBRARIES_DIRS NAMES libRestCore.so
        HINTS ${REST_DIR}/lib $ENV{REST_PATH}/lib)

if (REST_INCLUDE_DIRS AND REST_LIBRARIES_DIRS)
    set(REST_FOUND TRUE)
    set(REST_GARFIELD_LIB $ENV{REST_GARFIELD_LIB})
    set(REST_GARFIELD_INCLUDE $ENV{REST_GARFIELD_INCLUDE})
    set(REST_LIBRARIES -L${REST_LIBRARIES_DIRS} -lRestTools -lRestEvents -lRestMetadata -lRestProcesses -lRestCore ${REST_GARFIELD_LIB})
    set(REST_INSTLL_PATH ${REST_INCLUDE_DIRS}/../)

endif ()

if (REST_FOUND)
    message(STATUS "Found REST includes in ${REST_INCLUDE_DIRS}")
    message(STATUS "Found REST libraries in ${REST_LIBRARIES_DIRS}")
else (REST_FOUND)
    message(FATAL_ERROR "REST required, but not found")
endif (REST_FOUND)

# Make variables changeble to the advanced user
mark_as_advanced(REST_INCLUDE_DIRS)
mark_as_advanced(REST_LIBRARIES)
mark_as_advanced(REST_LIBRARIES_DIRS)
mark_as_advanced(REST_INSTLL_PATH)
mark_as_advanced(REST_GARFIELD_LIB)
mark_as_advanced(REST_GARFIELD_INCLUDE)