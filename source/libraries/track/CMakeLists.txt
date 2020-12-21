set(contents . tsp)

file(GLOB_RECURSE addon_src
    "trackMinimization.c"
    "allocrus.c"
    "edgelen.c"
    "edgeutil.c"
    "heldkarp.c" )

COMPILEDIR(RestTrack)
