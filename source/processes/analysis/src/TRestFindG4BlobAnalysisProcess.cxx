///______________________________________________________________________________
///______________________________________________________________________________
///             
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestFindG4BlobAnalysisProcess.cxx
///
///
///             First implementation of Geant4 analysis process into REST_v2
///             Date : mar/2016
///             Author : J. Galan
///
///_______________________________________________________________________________


#include "TRestFindG4BlobAnalysisProcess.h"
using namespace std;


ClassImp(TRestFindG4BlobAnalysisProcess)
    //______________________________________________________________________________
TRestFindG4BlobAnalysisProcess::TRestFindG4BlobAnalysisProcess()
{
    Initialize();
}

//______________________________________________________________________________
TRestFindG4BlobAnalysisProcess::TRestFindG4BlobAnalysisProcess( char *cfgFileName )
{
    Initialize();

    if( LoadConfigFromFile( cfgFileName ) ) LoadDefaultConfig( );
}

//______________________________________________________________________________
TRestFindG4BlobAnalysisProcess::~TRestFindG4BlobAnalysisProcess()
{
    delete fG4Event;
}

void TRestFindG4BlobAnalysisProcess::LoadDefaultConfig()
{
    SetTitle( "Default config" );
}

//______________________________________________________________________________
void TRestFindG4BlobAnalysisProcess::Initialize()
{
    SetName( "findG4BlobAnalysisProcess" );

    fG4Event = new TRestG4Event();
    ///fOutputG4Event = new TRestG4Event();

    fOutputEvent = fG4Event;
    fInputEvent = fG4Event;
}

void TRestFindG4BlobAnalysisProcess::LoadConfig( std::string cfgFilename, std::string name )
{
    if( LoadConfigFromFile( cfgFilename, name ) ) LoadDefaultConfig( );
}

//______________________________________________________________________________
void TRestFindG4BlobAnalysisProcess::InitProcess()
{
    TRestEventProcess::ReadObservables();

    fG4Metadata = (TRestG4Metadata *) GetGeant4Metadata( );
}

//______________________________________________________________________________
void TRestFindG4BlobAnalysisProcess::BeginOfEventProcess() 
{
}

//______________________________________________________________________________
TRestEvent* TRestFindG4BlobAnalysisProcess::ProcessEvent( TRestEvent *evInput )
{
    fG4Event = (TRestG4Event *) evInput;

    TString obsName;

    Int_t nBlobs = 0;

    Double_t xBlob1 = 0, yBlob1 = 0, zBlob1 = 0;
    Double_t xBlob2 = 0, yBlob2 = 0, zBlob2 = 0;

    for( int tck = 0; tck < fG4Event->GetNumberOfTracks(); tck++ )
    {
        TRestG4Track *track = fG4Event->GetTrack( tck );
        if(  track->GetParentID() == 0 )
        {
            if( track->GetParticleName() != "e-" )
            {
                cout << "TRestFindG4BlobAnalysis Warning. Primary particle is not an electron" << endl;
                cout << "Skipping." << endl;
                continue;
            }

            if( nBlobs >= 2 ) 
            {
                cout << "TRestFindG4BlobAnalysis Warning. More than 2 e- primaries found!" << endl;
                continue;
            }

            Int_t nHits = track->GetNumberOfHits();

            if( nBlobs == 0 )
            {
                xBlob1 = track->GetHits()->GetX( nHits-1); 
                yBlob1 = track->GetHits()->GetY( nHits-1); 
                zBlob1 = track->GetHits()->GetZ( nHits-1); 

                obsName = this->GetName() + (TString) ".xBlob1";
                fAnalysisTree->SetObservableValue( obsName, xBlob1 );

                obsName = this->GetName() + (TString) ".yBlob1";
                fAnalysisTree->SetObservableValue( obsName, yBlob1 );

                obsName = this->GetName() + (TString) ".zBlob1";
                fAnalysisTree->SetObservableValue( obsName, zBlob1 );
            }
            else
            {
                xBlob2 = track->GetHits()->GetX( nHits-1); 
                yBlob2 = track->GetHits()->GetY( nHits-1); 
                zBlob2 = track->GetHits()->GetZ( nHits-1); 
                
                obsName = this->GetName() + (TString) ".xBlob2";
                fAnalysisTree->SetObservableValue( obsName, xBlob2 );

                obsName = this->GetName() + (TString) ".yBlob2";
                fAnalysisTree->SetObservableValue( obsName, yBlob2 );

                obsName = this->GetName() + (TString) ".zBlob2";
                fAnalysisTree->SetObservableValue( obsName, zBlob2 );
            }


            nBlobs++;
        }
    }

    Double_t deltaX = xBlob1 - xBlob2;
    Double_t deltaY = yBlob1 - yBlob2;
    Double_t deltaZ = zBlob1 - zBlob2;

    Double_t blobDistance = deltaX*deltaX + deltaY*deltaY + deltaZ*deltaZ;
    blobDistance = TMath::Sqrt( blobDistance );

    obsName = this->GetName() + (TString) ".blobDistance";
    fAnalysisTree->SetObservableValue( obsName, blobDistance );


    cout << "Event : " << fG4Event->GetID() << " Tracks : " << fG4Event->GetNumberOfTracks() << endl;

    return fG4Event;
}

//______________________________________________________________________________
void TRestFindG4BlobAnalysisProcess::EndOfEventProcess() 
{

}

//______________________________________________________________________________
void TRestFindG4BlobAnalysisProcess::EndProcess()
{
    // Function to be executed once at the end of the process 
    // (after all events have been processed)

    //Start by calling the EndProcess function of the abstract class. 
    //Comment this if you don't want it.
    //TRestEventProcess::EndProcess();
}

//______________________________________________________________________________
void TRestFindG4BlobAnalysisProcess::InitFromConfigFile( )
{

}

