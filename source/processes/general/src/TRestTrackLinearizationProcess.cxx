///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///             
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestTrackLinearizationProcess.cxx
///
///             Feb 2016:   First concept (Javier Galan)
//
///_______________________________________________________________________________

#include "TRestTrackLinearizationProcess.h"
using namespace std;

vector <Double_t> distanceToNode;

ClassImp(TRestTrackLinearizationProcess)
//______________________________________________________________________________
TRestTrackLinearizationProcess::TRestTrackLinearizationProcess( )
{
    Initialize();
}

//______________________________________________________________________________
TRestTrackLinearizationProcess::TRestTrackLinearizationProcess( char *cfgFileName )
{
    Initialize();

    if( LoadConfigFromFile( cfgFileName ) == -1 ) LoadDefaultConfig( );
    PrintMetadata();
}

//______________________________________________________________________________
TRestTrackLinearizationProcess::~TRestTrackLinearizationProcess( )
{
    delete fInputTrackEvent;
    delete fOutputLinearizedTrackEvent;
}

void TRestTrackLinearizationProcess::LoadDefaultConfig( )
{
    SetName( "trackLinearizationProcess" );
    SetTitle( "Default config" );

    fTransversalResolution = .1;
    fLengthResolution = 1;
}

//______________________________________________________________________________
void TRestTrackLinearizationProcess::Initialize( )
{
    SetName("trackLinearizationProcess");

    fInputTrackEvent = new TRestTrackEvent();
    fOutputLinearizedTrackEvent = new TRestLinearizedTrackEvent();

    fOutputEvent = fOutputLinearizedTrackEvent;
    fInputEvent  = fInputTrackEvent;

    fTransversalResolution = .1;
    fLengthResolution = 1;
}

void TRestTrackLinearizationProcess::LoadConfig( string cfgFilename )
{

    if( LoadConfigFromFile( cfgFilename ) == -1 ) LoadDefaultConfig( );

    PrintMetadata();

}

//______________________________________________________________________________
void TRestTrackLinearizationProcess::InitProcess()
{
    cout << __PRETTY_FUNCTION__ << endl;
}

//______________________________________________________________________________
void TRestTrackLinearizationProcess::BeginOfEventProcess() 
{
    fOutputLinearizedTrackEvent->Initialize(); 
}

//______________________________________________________________________________
TRestEvent* TRestTrackLinearizationProcess::ProcessEvent( TRestEvent *evInput )
{
    fInputTrackEvent = (TRestTrackEvent *) evInput;

    /* Debug output  */
    cout << "Event ID : " << fInputTrackEvent->GetEventID() << endl;
    cout << "Number of tracks : " << fInputTrackEvent->GetNumberOfTracks() << endl;
    cout << "*****************************" << endl;


    for( int tck = 0; tck < fInputTrackEvent->GetNumberOfTracks(); tck++ )
    {
        if( !fInputTrackEvent->isTopLevel( tck ) ) continue;

        TRestLinearTrack linTrack;

        TRestTrack *topTrack = fInputTrackEvent->GetTrack( tck );

        Int_t origin = fInputTrackEvent->GetOriginTrackID( tck );

        TRestTrack *parentTrack = fInputTrackEvent->GetTrackById( origin );

        /* {{{ Debug output 
        cout << "Top Track " << topTrack->GetTrackID() << endl;
        cout << "--------------------------------------" << endl;
        topTrack->PrintTrack();
        cout << "--------------------------------------" << endl;

        cout << "Parent Track " << parentTrack->GetTrackID() << endl;
        cout << "--------------------------------------" << endl;
        parentTrack->PrintTrack( false );
        cout << "--------------------------------------" << endl;
        getchar();

         }}} */

        TRestVolumeHits *nodeHits = topTrack->GetVolumeHits();
        TRestVolumeHits *originHits = parentTrack->GetVolumeHits();

        distanceToNode.clear();
        for( int h = 0; h < nodeHits->GetNumberOfHits(); h++ )
            distanceToNode.push_back( nodeHits->GetDistanceToNode( h ) );

        /* {{{  Debug output 
        for( int h = 0; h < nodeHits->GetNumberOfHits(); h++ )
            cout << "Distance : " <<  h << " = " << distanceToNode[h] << endl;
        getchar();
         }}} */

        for( int oHit = 0; oHit < originHits->GetNumberOfHits(); oHit++ )
        {
            TVector3 hitPosition = originHits->GetPosition( oHit );

            TVector2 projection = FindProjection( hitPosition, nodeHits );

            /*
            cout << "Hit position" << endl;
            cout << "------------" << endl;
            hitPosition.Print();
            cout << "------------" << endl;
            cout << "Projection" << endl;
            cout << "------------" << endl;
            projection.Print();
            cout << "------------" << endl;
            getchar();
            */

            Double_t energy = originHits->GetEnergy( oHit );

            Double_t longitudinal = fLengthResolution * ( (Int_t) (projection.X()/fLengthResolution) );
            Double_t transversal = fTransversalResolution * ( (Int_t) (projection.Y()/fTransversalResolution) );

            linTrack.AddChargeToLinearTrack( longitudinal, transversal, energy );
        }

        linTrack.SortTracks();
        linTrack.SetEnergy( parentTrack->GetEnergy() );
        linTrack.SetMeanPosition( parentTrack->GetMeanPosition() );

        fOutputLinearizedTrackEvent->AddLinearTrack( linTrack );
    }

    return fOutputLinearizedTrackEvent;
}

TVector2 TRestTrackLinearizationProcess::FindProjection( TVector3 position, TRestHits *nodes )
{
    Int_t closestNode = nodes->GetClosestHit( position );

    TVector2 projection1, projection2;


    if( closestNode > 0 )
        projection1 = nodes->GetProjection( closestNode, closestNode-1, position );
    else 
        projection1 = TVector2( 0, 0 );

    if( closestNode < nodes->GetNumberOfHits()-1 )
        projection2 = nodes->GetProjection( closestNode+1, closestNode, position );
    else 
        projection2 = TVector2( 0, 0 );

    /*
    cout << "Closest node : " << closestNode << endl;

    cout << endl;
    cout << "projection 1 " << endl;
    cout << "-------------" << endl;
    cout << "long : " << projection1.X() << " // tr : " << projection1.Y() << endl;
    cout << "-------------" << endl;
    cout << "projection 2 " << endl;
    cout << "-------------" << endl;
    cout << "long : " << projection2.X() << " // tr : " << projection2.Y() << endl;
    cout << "-------------" << endl;
    getchar();
    */

    if( projection2.Y() == 0 )
    {
        projection1 += TVector2( distanceToNode[closestNode-1], 0 );
        return projection1;
    }
    else if ( projection1.Y() == 0 )
    {
        projection2 +=  TVector2 ( distanceToNode[closestNode], 0 );
        return projection2;
    }
    else if ( projection1.Y() <= projection2.Y() )
    {
        projection1 += TVector2( distanceToNode[closestNode-1], 0 );
        return projection1;
    }
    else
    {
        projection2 +=  TVector2 ( distanceToNode[closestNode], 0 );
        return projection2;
    }
}

//______________________________________________________________________________
void TRestTrackLinearizationProcess::EndOfEventProcess() 
{

}

//______________________________________________________________________________
void TRestTrackLinearizationProcess::EndProcess()
{
}

//______________________________________________________________________________
void TRestTrackLinearizationProcess::InitFromConfigFile( )
{
    fLengthResolution = StringToDouble( GetParameter( "lengthResolution" ) );
    fTransversalResolution = StringToDouble( GetParameter( "transversalResolution" ) );
}

