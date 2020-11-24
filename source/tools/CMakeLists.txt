set(contents external/tsp external/tinyxml resttools)

file(GLOB_RECURSE addon_src
    "trackMinimization.c"
    "allocrus.c"
    "edgelen.c"
    "edgeutil.c"
    "heldkarp.c"
	"tiny*.cpp"
    "startup.cpp")

COMPILEDIR(RestTools)