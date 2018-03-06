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
int TrackMinimization_2D ( int *xIn, int *yIn, int ncount, int *mytour );

#ifdef __cplusplus
extern "C"
#endif
int TrackMinimization_3D_segment ( int ncount, int *elen, int *mytour );

#ifdef __cplusplus
extern "C" 
#endif
int TrackMinimization_3D ( int *xIn, int *yIn, int *zIn, int ncount, int *mytour );
