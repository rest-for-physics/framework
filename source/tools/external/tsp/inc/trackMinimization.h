#include "bigguy.h"
#include "edgegen.h"
#include "heldkarp.h"
#include "linkern.h"
#include "machdefs.h"
#include "macrorus.h"
#include "tsp.h"
#include "util.h"

#ifdef __cplusplus
extern "C"
#endif
    int
    TrackMinimization_2D(int* xIn, int* yIn, int ncount, int* mytour);

#ifdef __cplusplus
extern "C"
#endif
    int
    TrackMinimization_segment(int ncount, int* elen, int* mytour);

#ifdef __cplusplus
extern "C"
#endif
    int
    TrackMinimization_3D(int* xIn, int* yIn, int* zIn, int ncount, int* mytour);
