#include "machdefs.h"
#include "util.h"
#include "edgegen.h"
#include "tsp.h"
#include "linkern.h"
#include "heldkarp.h"
#include "bigguy.h"
#include "macrorus.h"

#ifdef __cplusplus
extern "C" 
#endif
int TrackMinimization_2D ( int *xIn, int *yIn, int ncount );

#ifdef __cplusplus
extern "C" 
#endif
int TrackMinimization_3D ( int *xIn, int *yIn, int *zIn, int ncount );
