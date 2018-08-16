set(contents external/tsp external/tinyxml resttools)
set(libname RestTools)

if(CMAKE_SYSTEM_NAME MATCHES "Windows")
file(GLOB_RECURSE addon_src
		"tiny*.cpp")
else()
file(GLOB_RECURSE addon_src
        "trackMinimization.c"
        "allocrus.c"
        "edgelen.c"
        "edgeutil.c"
        "heldkarp.c"
		"tiny*.cpp")
endif()
COMPILEDIR(${libname})