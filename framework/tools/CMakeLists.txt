set(contents external/tsp external/tinyxml resttools)

if(CMAKE_SYSTEM_NAME MATCHES "Windows")
  file(GLOB_RECURSE addon_src
		"tiny*.cpp")
  set(addon_inc ${CMAKE_CURRENT_SOURCE_DIR}/external/win)
else()
  file(GLOB_RECURSE addon_src
        "trackMinimization.c"
        "allocrus.c"
        "edgelen.c"
        "edgeutil.c"
        "heldkarp.c"
		"tiny*.cpp")
  endif()
COMPILEDIR(RestTools)