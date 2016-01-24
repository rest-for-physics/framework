///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///             
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestHitsToTrackProcess.cxx
///
///             Dec 2015:   First concept (Javier Gracia Garza)
//
//              History : 
//              Jan 2016: Readapted to obtain tracks in bi-dimensional hits (Javier Galan)
///_______________________________________________________________________________

#include "TRestHitsToTrackProcess.h"

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
    fTrackEvent->Initialize(); 
}

//______________________________________________________________________________
TRestEvent* TRestHitsToTrackProcess::ProcessEvent( TRestEvent *evInput )
{
    /* Time measurement
    high_resolution_clock::time_point t1 = high_resolution_clock::now();
    */

    fHitsEvent = (TRestHitsEvent *) evInput;

    /* Debugging output
    cout << "Event ID : " << fHitsEvent->GetEventID() << endl;
    cout << "Number of hits : " << fHitsEvent->GetNumberOfHits() << endl;
    */

    TRestHits *xzHits = fHitsEvent->GetXZHits();
    Int_t xTracks = FindTracks( xzHits );
    fTrackEvent->SetNumberOfXTracks( xTracks );

    TRestHits *yzHits = fHitsEvent->GetYZHits();
    Int_t yTracks = FindTracks( yzHits );
    fTrackEvent->SetNumberOfYTracks( yTracks );

    TRestHits *xyzHits = fHitsEvent->GetXYZHits();
    FindTracks( xyzHits );

    /* Debugging output
    cout << "Tracks in X : " << fTrackEvent->GetNumberOfXTracks() << endl;
    cout << "Tracks in Y : " << fTrackEvent->GetNumberOfYTracks() << endl;
    cout <<  " Tracks : " << fTrackEvent->GetNumberOfTracks() << endl;

    cout<<"***********************"<<endl;
    */

    /* Time measurement
    high_resolution_clock::time_point t2 = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>( t2 - t1 ).count();
    cout << duration << " us" << endl;
    getchar();
    */

    if( fTrackEvent->GetNumberOfTracks() == 0 ) return NULL;

    return fTrackEvent;
}

Int_t TRestHitsToTrackProcess::FindTracks( TRestHits *hits )
{
    Int_t nTracksFound = 0;
    vector <Int_t> Q; //list of points (hits) that need to be checked
    vector <Int_t> P; //list of neighbours within a radious fClusterDistance

    bool isProcessed = false;
    Int_t qsize = 0;
    TRestTrack *track = new TRestTrack();

    Double_t trackEnergy = 0.;
    TRestVolumeHits volHit;

    Float_t fClusterDistance2 = (Float_t) (fClusterDistance * fClusterDistance);

    //for every event in the point cloud
    while (hits->GetNumberOfHits()>0)
    {
        Q.push_back( 0 );

        //for every point in Q
        for ( unsigned int q = 0; q < Q.size(); q++)
        {		  
            //we look for the neighbours
            for ( int j=0; j < hits->GetNumberOfHits(); j++ )
            {
                if (j != Q[q])
                {
                    if(hits->GetDistance2( Q[q] , j ) < fClusterDistance2 )
                        P.push_back( j );
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
            const Double_t x =  hits->GetX( Q[nhit] );
            const Double_t y =  hits->GetY( Q[nhit] );
            const Double_t z =  hits->GetZ( Q[nhit] );
            const Double_t en = hits->GetEnergy( Q[nhit] );

            trackEnergy += en;
            TVector3 pos ( x, y, z );
            TVector3 sigma ( 0., 0., 0. );		

            volHit.AddHit(pos, en, sigma);

            hits->RemoveHit(Q[nhit]);
        }

        track->SetTrackEnergy(trackEnergy);
        track->SetVolumeHit(volHit);
        trackEnergy = 0.0;
        volHit.RemoveHits();

        fTrackEvent->AddTrack(track);
        nTracksFound++;

        Q.clear();
    }

    return nTracksFound;
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
