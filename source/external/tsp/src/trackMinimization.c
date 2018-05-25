/****************************************************************************/
/*                                                                          */
/*  This file is part of CONCORDE                                           */
/*                                                                          */
/*  (c) Copyright 1995--1999 by David Applegate, Robert Bixby,              */
/*  Vasek Chvatal, and William Cook                                         */
/*                                                                          */
/*  Permission is granted for academic research use.  For other uses,       */
/*  contact the authors for licensing options.                              */
/*                                                                          */
/*  Use at your own risk.  We make no guarantees about the                  */
/*  correctness or usefulness of this code.                                 */
/*                                                                          */
/****************************************************************************/

/****************************************************************************/
/*                                                                          */
/*                  THE MAIN PROGRAM FOR CONCORDE                           */
/*                                                                          */
/*                           TSP CODE                                       */
/*                                                                          */
/*                                                                          */
/*  Written by:  Applegate, Bixby, Chvatal, and Cook                        */
/*  Date: September 25, 1995                                                */
/*        November 28, 2003 (bico)                                          */
/*  SEE short decsribtion in usage ().                                      */
/*                                                                          */
/*  NOTE:  When using CC_SPARSE edge sets, it is important to specify a     */
/*   a tour or an upperbound if you have one, since our heuristics are      */
/*   not designed for finding Hamilton circuits in sparse graphs.           */
/*                                                                          */
/****************************************************************************/

#include "trackMinimization.h"


static int runHeldKarp (int ncount, CCdatagroup *dat, int *hk_tour);
static int runHeldKarp_segment (int ncount, int *elen, int *hk_tour );

int TrackMinimization_3D ( int *xIn, int *yIn, int *zIn, int ncount, int *mytour )
{
    int rval = 0;
    int i;
    CCdatagroup dat;
    int *besttour = (int *) NULL;

    if( ncount <= 3 ) return rval;

    /////////////////////////////////////////////
    // Initializing dat structure
    CCutil_init_datagroup (&dat);
    CCutil_dat_setnorm ( &dat, CC_EUCLIDEAN_3D );

    dat.x = CC_SAFE_MALLOC (ncount, double);
    dat.y = CC_SAFE_MALLOC (ncount, double);
    dat.z = CC_SAFE_MALLOC (ncount, double);
    for (i = 0; i < ncount; i++) {
        dat.x[i] = (double) xIn[i];
        dat.y[i] = (double) yIn[i];
        dat.z[i] = (double) zIn[i];
    }
    /////////////////////////////////////////////



    /////////////////////////////////////////////
    // Solving using Held-Karp
    besttour = CC_SAFE_MALLOC (ncount, int);
    CCcheck_NULL (besttour, "out of memory for besttour");
    rval = runHeldKarp (ncount, &dat, besttour);
    CCcheck_rval (rval, "runHeldKarp failed");
    /////////////////////////////////////////////
    //

    for( i = 0; i < ncount; i++ )
    {
        xIn[i] = dat.x[besttour[i]];
        yIn[i] = dat.y[besttour[i]];
        zIn[i] = dat.z[besttour[i]];
        mytour[i] = besttour[i];
    }


    goto CLEANUP;


CLEANUP:

    CC_IFFREE (besttour, int);
    CCutil_freedatagroup (&dat);

    return rval;
}

int TrackMinimization_segment ( int ncount, int *elen, int *mytour )
{
    int rval = 0;
    int i;
    int *besttour = (int *) NULL;

    if( ncount <= 3 ) return rval;

    /////////////////////////////////////////////
    // Solving using Held-Karp
    besttour = CC_SAFE_MALLOC (ncount, int);
    CCcheck_NULL (besttour, "out of memory for besttour");
    rval = runHeldKarp_segment (ncount, elen, besttour);
    CCcheck_rval (rval, "runHeldKarp failed");
    /////////////////////////////////////////////

    for( i = 0; i < ncount; i++ )
        mytour[i] = besttour[i];

    goto CLEANUP;


CLEANUP:

    CC_IFFREE (besttour, int);

    return rval;
}

int TrackMinimization_2D ( int *xIn, int *yIn, int ncount, int *mytour )
{
    int rval = 0;
    int i;
    CCdatagroup dat;
    int *besttour = (int *) NULL;

    /////////////////////////////////////////////
    // Initializing dat structure
    CCutil_init_datagroup (&dat);
    CCutil_dat_setnorm ( &dat, CC_EUCLIDEAN);

    dat.x = CC_SAFE_MALLOC (ncount, double);
    dat.y = CC_SAFE_MALLOC (ncount, double);
    for (i = 0; i < ncount; i++) {
        dat.x[i] = (double) xIn[i];
        dat.y[i] = (double) yIn[i];
    }
    /////////////////////////////////////////////



    /////////////////////////////////////////////
    // Solving using Held-Karp
    besttour = CC_SAFE_MALLOC (ncount, int);
    CCcheck_NULL (besttour, "out of memory for besttour");
    if( ncount > 3 )
    {
        rval = runHeldKarp (ncount, &dat, besttour);
        CCcheck_rval (rval, "runHeldKarp failed");
    }
    /////////////////////////////////////////////
    //

    for( i = 0; i < ncount; i++ )
    {
        xIn[i] = dat.x[besttour[i]];
        yIn[i] = dat.y[besttour[i]];
        mytour[i] = besttour[i];
    }


    goto CLEANUP;


CLEANUP:

    CC_IFFREE (besttour, int);
    CCutil_freedatagroup (&dat);

    return rval;
}

static int runHeldKarp (int ncount, CCdatagroup *dat, int *hk_tour )
{
    double hk_val;
    int hk_found, hk_yesno;
    int *hk_tlist = (int *) NULL;
    int rval = 0;
    int m = 0;
    int i = 0;
    int silent = 2;

    hk_tlist = CC_SAFE_MALLOC (2*ncount, int);
    CCcheck_NULL (hk_tlist, "out of memory for hk_tlist");

    rval = CCheldkarp_small (ncount, dat, (double *) NULL, &hk_val,
                             &hk_found, 0, hk_tlist, 1000000, silent);
    CCcheck_rval (rval, "CCheldkarp_small failed");
    //printf ("Optimal Solution: %.2f\n", hk_val); fflush (stdout);

    rval = CCutil_edge_to_cycle (ncount, hk_tlist, &hk_yesno, hk_tour);

    int contiguos = 1;
    while( contiguos )
    {
        contiguos = 0;
        for( i = 1; i < ncount; i++ )
            if( hk_tour[i] - hk_tour[i-1] == ncount-1 ) contiguos = 1;
            if( hk_tour[i] - hk_tour[i-1] == 1-ncount ) contiguos = 1;

        if( contiguos )
        {
            int tmp = hk_tour[0];
            for( i = 1; i < ncount; i++ )
                hk_tour[i-1] = hk_tour[i];
            hk_tour[ncount-1] = tmp;
        }
    }

CLEANUP:

     CC_IFFREE (hk_tlist, int);
     return rval;
}

static int runHeldKarp_segment (int ncount, int *elen, int *hk_tour )
{
    double hk_val;
    int hk_found, hk_yesno;
    int *hk_tlist = (int *) NULL;
    int rval = 0;
    int m = 0;
    int i = 0;
    int silent = 2;

    hk_tlist = CC_SAFE_MALLOC (2*ncount, int);
    CCcheck_NULL (hk_tlist, "out of memory for hk_tlist");

    rval = CCheldkarp_small_segment (ncount, elen, (double *) NULL, &hk_val,
                             &hk_found, 0, hk_tlist, 1000000, silent);
    CCcheck_rval (rval, "CCheldkarp_small failed");
    //printf ("Optimal Solution: %.2f\n", hk_val); fflush (stdout);

    rval = CCutil_edge_to_cycle (ncount, hk_tlist, &hk_yesno, hk_tour);

    int contiguos = 1;
    while( contiguos )
    {
        contiguos = 0;
        for( i = 1; i < ncount; i++ )
            if( hk_tour[i] - hk_tour[i-1] == ncount-1 ) contiguos = 1;
            if( hk_tour[i] - hk_tour[i-1] == 1-ncount ) contiguos = 1;

        if( contiguos )
        {
            int tmp = hk_tour[0];
            for( i = 1; i < ncount; i++ )
                hk_tour[i-1] = hk_tour[i];
            hk_tour[ncount-1] = tmp;
        }
    }

CLEANUP:

     CC_IFFREE (hk_tlist, int);
     return rval;
}

