file(GLOB tspSrc
        "src/trackMinimization.c"
        "src/allocrus.c"
        "src/edgelen.c"
        "src/edgeutil.c"
        "src/heldkarp.c")

set(tspSrc ${tspSrc} PARENT_SCOPE)