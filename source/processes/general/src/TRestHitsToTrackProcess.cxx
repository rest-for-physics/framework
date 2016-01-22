///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///             
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestHitsToTrackProcess.cxx
///
///             Template to use to design "event process" classes inherited from 
///             TRestHitsToTrackProcess
///             How to use: replace TRestHitsToTrackProcess by your name, 
///             fill the required functions following instructions and add all
///             needed additional members and funcionality
///
///             jun 2014:   First concept
///                 Created as part of the conceptualization of existing REST 
///                 software.
///                 Igor G. Irastorza
///_______________________________________________________________________________


#include "TRestHitsToTrackProcess.h"

#include <TRandom.h>

ClassImp(TRestHitsToTrackProcess)
    //______________________________________________________________________________
TRestHitsToTrackProcess::TRestHitsToTrackProcess()
{
    Initialize();
}

//______________________________________________________________________________
TRestHitsToTrackProcess::TRestHitsToTrackProcess( char *cfgFileName )
{
    Initialize();

    if( LoadConfigFromFile( cfgFileName ) == -1 ) LoadDefaultConfig( );
    PrintMetadata();

    // TRestHitsToTrackProcess default constructor
}

//______________________________________________________________________________
TRestHitsToTrackProcess::~TRestHitsToTrackProcess()
{
    delete fHitsEvent;
    delete fTrackEvent;
    // TRestHitsToTrackProcess destructor
}

void TRestHitsToTrackProcess::LoadDefaultConfig( )
{
    SetName( "hitsToTrackProcess" );
    SetTitle( "Default config" );

    fClusterDistance = 1.0;


}

//______________________________________________________________________________
void TRestHitsToTrackProcess::Initialize()
{
    SetName("hitsToTrackProcess");

    fClusterDistance = 1.;

    fHitsEvent = new TRestHitsEvent();
    fTrackEvent = new TRestTrackEvent();

    fOutputEvent = fTrackEvent;
    fInputEvent  = fHitsEvent;
}

void TRestHitsToTrackProcess::LoadConfig( string cfgFilename )
{

    if( LoadConfigFromFile( cfgFilename ) == -1 ) LoadDefaultConfig( );
    PrintMetadata();

}

//______________________________________________________________________________
void TRestHitsToTrackProcess::InitProcess()
{
    // Function to be executed once at the beginning of process
    // (before starting the process of the events)

    //Start by calling the InitProcess function of the abstract class. 
    //Comment this if you don't want it.
    //TRestEventProcess::InitProcess();

    cout << __PRETTY_FUNCTION__ << endl;

}

//______________________________________________________________________________
void TRestHitsToTrackProcess::BeginOfEventProcess() 
{
    cout << "Begin of event process" << endl;
    fTrackEvent->Initialize(); 
}

//______________________________________________________________________________
TRestEvent* TRestHitsToTrackProcess::ProcessEvent( TRestEvent *evInput )
{

    fHitsEvent = (TRestHitsEvent *) evInput;

    cout << "Event ID : " << fHitsEvent->GetEventID() << endl;
    cout << "Number of hits : " << fHitsEvent->GetNumberOfHits() << endl;

    vector <Int_t> Q; //list of points (hits) that need to be checked
    vector <Int_t> P; //list of neighbours within a radious fClusterDistance

    bool isProcessed = false;
    Int_t qsize = 0;
    TRestTrack *track = new TRestTrack();

    //bool process = true;
    Double_t trackEnergy = 0.;
    TRestVolumeHits volHit;


    //creating the matrix of distances between hits
    distMatrix = new TMatrixD(fHitsEvent->GetNumberOfHits(), fHitsEvent->GetNumberOfHits());

    //Filling the symmetric matrix
    /*
       for( int i = 0; i < fHitsEvent->GetNumberOfHits(); i++ )
       for( int j = i+1; j < fHitsEvent->GetNumberOfHits(); j++ )
       {
       (*distMatrix)[i][j]  = fHitsEvent->GetDistance2( i , j );
       }
       */

    //bool event = true;
    //Int_t nHits = 0;
    //for every event in the point cloud
    while (fHitsEvent->GetNumberOfHits()>0)
    {
        Q.push_back( 0 );

        //for every point in Q
        for ( unsigned int q = 0; q < Q.size(); q++)
        {		  
            //we look for the neighbours
            for ( int j=0; j < fHitsEvent->GetNumberOfHits(); j++ )
            {
                if (j != Q[q])
                {
                    if(fHitsEvent->GetDistance2( Q[q] , j ) < fClusterDistance*fClusterDistance)
                        P.push_back( j );

                    /*				   if(Q[q]<j)
                                       if( (*distMatrix)[Q[q]][j] < fClusterDistance*fClusterDistance)
                                       P.push_back( j );
                                       else if(Q[q]>j)
                                       if( (*distMatrix)[j][Q[q]] < fClusterDistance*fClusterDistance)	
                                       P.push_back( j );	  
                                       */
                }
            }

            qsize  = Q.size();

            //For all the neighbours found P.size()
            //Check if the points have already been processed
            for (unsigned int i=0; i < P.size(); i++)
            {		
                isProcessed = false;

                for ( int j = 0; j < qsize; j++)
                {
                    // if yes, we do not consider it again
                    if (P[i] == Q[j])
                    {
                        isProcessed = true;
                        break;	
                    }
                }

                //If not, we add the point P[i] to the list of Q
                if (isProcessed == false)
                {
                    Q.push_back ( P[i] );
                }
            }

            P.clear();	
        }

        //We order the Q vector
        std::sort (Q.begin(), Q.end());
        //Then we swap to decresing order
        std::reverse(Q.begin(),Q.end());

        //When the list of all points in Q has been processed, we add the clusters to the TrackEvent and reset Q
        for (unsigned int nhit = 0; nhit < Q.size() ; nhit++)
        {
            const Double_t x =  fHitsEvent->GetX( Q[nhit] );
            const Double_t y =  fHitsEvent->GetY( Q[nhit] );
            const Double_t z =  fHitsEvent->GetZ( Q[nhit] );
            const Double_t en = fHitsEvent->GetEnergy( Q[nhit] );

            trackEnergy += en;
            TVector3 pos ( x, y, z );
            TVector3 sigma ( 0., 0., 0. );		


            volHit.AddHit(pos, en, sigma);

            fHitsEvent->RemoveHit(Q[nhit]);
        }

        track->SetTrackEnergy(trackEnergy);
        track->SetVolumeHit(volHit);
        trackEnergy = 0.0;
        volHit.RemoveHits();

        fTrackEvent->AddTrack(*track);

        Q.clear();
    }


    if( fTrackEvent->GetNumberOfTracks() == 0 ) return NULL;

    cout <<  " Tracks : " << fTrackEvent->GetNumberOfTracks() << endl;
    cout<<"***********************"<<endl;

    return fTrackEvent;
}

//______________________________________________________________________________
void TRestHitsToTrackProcess::EndOfEventProcess() 
{

}

//______________________________________________________________________________
void TRestHitsToTrackProcess::EndProcess()
{
    // Function to be executed once at the end of the process 
    // (after all events have been processed)

    //Start by calling the EndProcess function of the abstract class. 
    //Comment this if you don't want it.
    //TRestEventProcess::EndProcess();
}

//______________________________________________________________________________
void TRestHitsToTrackProcess::InitFromConfigFile( )
{
    fClusterDistance = StringToDouble( GetParameter( "clusterDistance" ) );

    cout << " cluster-distance : " << fClusterDistance << endl;
}
