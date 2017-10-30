set(contents external/tsp resttools)
set(libname RestTools)

file(GLOB_RECURSE addon_src
        "trackMinimization.c"
        "allocrus.c"
        "edgelen.c"
        "edgeutil.c"
        "heldkarp.c")

COMPILEDIR(${libname})