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

static int runHeldKarp (int ncount, CCdatagroup *dat, int *hk_tour)
{
    double hk_val;
    int hk_found, hk_yesno;
    int *hk_tlist = (int *) NULL;
    int rval = 0;
    int m = 0;
    int i = 0;
    int silent = 2;
    int j = 0;
    int k = 0;
    int ecount;
    int *elist = (int *) NULL;
    int *elen  = (int *) NULL;
    int *besttour  = (int *) NULL;



    ncount++; // add a new pseudo city

    hk_tlist = CC_SAFE_MALLOC (2*ncount, int);
    CCcheck_NULL (hk_tlist, "out of memory for hk_tlist");

    ecount = ncount * (ncount-1) / 2;
    elist = CC_SAFE_MALLOC (ecount*2, int);
    elen  = CC_SAFE_MALLOC (ecount, int);
    if (elist == (int *) NULL || elen == (int *) NULL) {
        fprintf (stderr, "out of memory in CCheldkarp_small\n");
        rval = HELDKARP_ERROR; goto CLEANUP;
    }
    
    for (i = 0, k = 0; i < ncount; i++) {
        for (j = 0; j < i; j++) {
            elist[2*k] = i;
            elist[2*k+1] = j;
            // the distance to our pseudo city is 0
            if( i == ncount - 1 ) { elen[k] = 0; }
            else elen[k] = CCutil_dat_edgelen (i, j, dat);
            k++;
        }
    }

    rval = CCheldkarp_small_elist (ncount, ecount, elist, elen, (double *)NULL,
                                   &hk_val, &hk_found, 0, hk_tlist,
                                   1000000, silent);
    CCcheck_rval (rval, "CCheldkarp_small failed");
    //printf ("Optimal Solution: %.2f\n", hk_val); fflush (stdout);

    besttour = CC_SAFE_MALLOC (ncount, int);
    CCcheck_NULL (besttour, "out of memory for besttour");

    rval = CCutil_edge_to_cycle (ncount, hk_tlist, &hk_yesno, besttour);
    
    // shift our pseudo to the end
    for(;besttour[ncount-1] != ncount-1;) {
        int t = besttour[0];
        for(j = 1; j < ncount; ++j) {
            besttour[j-1] = besttour[j]; 
        }
        besttour[ncount-1] = t;        
    }
    memcpy(hk_tour, besttour, (ncount - 1) * sizeof(int));

CLEANUP:
    CC_IFFREE (besttour, int);
    CC_IFFREE (elen, int);
    CC_IFFREE (elist, int);
    CC_IFFREE (hk_tlist, int);

    return rval;
}

