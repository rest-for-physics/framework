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
    SetSectionName( this->ClassName() );

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
    std::vector <string> fObservables;
    fObservables = TRestEventProcess::ReadObservables();

    for( unsigned int i = 0; i < fObservables.size(); i++ )
    {
        if( fObservables[i].find( "Q1_R") != string::npos )
            fQ1_Observables.push_back( fObservables[i] );
        if( fObservables[i].find( "Q2_R") != string::npos )
            fQ2_Observables.push_back( fObservables[i] );
    }

    for( unsigned int i = 0; i < fQ1_Observables.size(); i++ )
    {
        Double_t r1 = atof ( fQ1_Observables[i].substr( 4, fQ1_Observables[i].length() - 4 ).c_str() );
        fQ1_Radius.push_back( r1 );
    }

    for( unsigned int i = 0; i < fQ2_Observables.size(); i++ )
    {
        Double_t r2 = atof ( fQ2_Observables[i].substr( 4, fQ2_Observables[i].length() - 4 ).c_str() );
        fQ2_Radius.push_back( r2 );
    }

    fG4Metadata = (TRestG4Metadata *) GetGeant4Metadata( );
}

//______________________________________________________________________________
void TRestFindG4BlobAnalysisProcess::BeginOfEventProcess() 
{
}

//______________________________________________________________________________
TRestEvent* TRestFindG4BlobAnalysisProcess::ProcessEvent( TRestEvent *evInput )
{
    *fG4Event = *((TRestG4Event *) evInput);

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
            }
            else
            {
                xBlob2 = track->GetHits()->GetX( nHits-1); 
                yBlob2 = track->GetHits()->GetY( nHits-1); 
                zBlob2 = track->GetHits()->GetZ( nHits-1); 
            }


            nBlobs++;
        }
    }
    
    // The blob with z-coordinate closer to z=0 is stored in x1,y1,z1 
    Double_t x1,y1,z1;
    Double_t x2,y2,z2;

    if( fabs( zBlob1 ) < fabs( zBlob2 ) )
    {
        x1 = xBlob1;
        y1 = yBlob1;
        z1 = zBlob1;

        x2 = xBlob2;
        y2 = yBlob2;
        z2 = zBlob2;
    }
    else
    {
        x1 = xBlob2;
        y1 = yBlob2;
        z1 = zBlob2;

        x2 = xBlob1;
        y2 = yBlob1;
        z2 = zBlob1;

    }


    obsName = this->GetName() + (TString) ".x1";
    fAnalysisTree->SetObservableValue( obsName, x1 );

    obsName = this->GetName() + (TString) ".y1";
    fAnalysisTree->SetObservableValue( obsName, y1 );

    obsName = this->GetName() + (TString) ".z1";
    fAnalysisTree->SetObservableValue( obsName, z1 );
    
    obsName = this->GetName() + (TString) ".x2";
    fAnalysisTree->SetObservableValue( obsName, x2 );

    obsName = this->GetName() + (TString) ".y2";
    fAnalysisTree->SetObservableValue( obsName, y2 );

    obsName = this->GetName() + (TString) ".z2";
    fAnalysisTree->SetObservableValue( obsName, z2 );

    Double_t deltaX = xBlob1 - xBlob2;
    Double_t deltaY = yBlob1 - yBlob2;
    Double_t deltaZ = zBlob1 - zBlob2;

    Double_t blobDistance = deltaX*deltaX + deltaY*deltaY + deltaZ*deltaZ;
    blobDistance = TMath::Sqrt( blobDistance );

    obsName = this->GetName() + (TString) ".distance";
    fAnalysisTree->SetObservableValue( obsName, blobDistance );

    /// We get the energy of the blobs
    TRestHits hits = fG4Event->GetHits();
    
    for( unsigned int n = 0; n < fQ1_Observables.size(); n++ )
    {
        Double_t q = hits.GetEnergyInSphere( x1, y1, z1, fQ1_Radius[n] );

        obsName = this->GetName() + (TString) "." + (TString) fQ1_Observables[n];
        fAnalysisTree->SetObservableValue( obsName, q );
    }

    for( unsigned int n = 0; n < fQ2_Observables.size(); n++ )
    {
        Double_t q = hits.GetEnergyInSphere( x2, y2, z2, fQ2_Radius[n] );

        obsName = this->GetName() + (TString) "." + (TString) fQ2_Observables[n];
        fAnalysisTree->SetObservableValue( obsName, q );
    }

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

