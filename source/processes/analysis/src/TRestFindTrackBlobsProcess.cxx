///______________________________________________________________________________
///______________________________________________________________________________
///             
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestFindTrackBlobsProcess.cxx
///
///
///             First implementation of Geant4 analysis process into REST_v2
///             Date : mar/2016
///             Author : J. Galan
///
///_______________________________________________________________________________


#include "TRestFindTrackBlobsProcess.h"
using namespace std;


ClassImp(TRestFindTrackBlobsProcess)
    //______________________________________________________________________________
TRestFindTrackBlobsProcess::TRestFindTrackBlobsProcess()
{
    Initialize();
}

//______________________________________________________________________________
TRestFindTrackBlobsProcess::TRestFindTrackBlobsProcess( char *cfgFileName )
{
    Initialize();

    if( LoadConfigFromFile( cfgFileName ) ) LoadDefaultConfig( );
}

//______________________________________________________________________________
TRestFindTrackBlobsProcess::~TRestFindTrackBlobsProcess()
{
    delete fInputTrackEvent;
    delete fOutputTrackEvent;
}

void TRestFindTrackBlobsProcess::LoadDefaultConfig()
{
    SetTitle( "Default config" );
}

//______________________________________________________________________________
void TRestFindTrackBlobsProcess::Initialize()
{
    SetSectionName( this->ClassName() );

    fInputTrackEvent = new TRestTrackEvent();
    fOutputTrackEvent = new TRestTrackEvent();

    fOutputEvent = fOutputTrackEvent;
    fInputEvent  = fInputTrackEvent;

    fHitsToCheckFraction = 0.2;
}

void TRestFindTrackBlobsProcess::LoadConfig( std::string cfgFilename, std::string name )
{
    if( LoadConfigFromFile( cfgFilename, name ) ) LoadDefaultConfig( );
}

//______________________________________________________________________________
void TRestFindTrackBlobsProcess::InitProcess()
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
}

//______________________________________________________________________________
void TRestFindTrackBlobsProcess::BeginOfEventProcess() 
{
    fOutputTrackEvent->Initialize();
}

//______________________________________________________________________________
TRestEvent* TRestFindTrackBlobsProcess::ProcessEvent( TRestEvent *evInput )
{
    fInputTrackEvent = (TRestTrackEvent *) evInput;

    // Copying the input tracks to the output track
    for( int tck = 0; tck < fInputTrackEvent->GetNumberOfTracks(); tck++ )
        fOutputTrackEvent->AddTrack( fInputTrackEvent->GetTrack(tck) ); 

    TRestTrack *longTrack = fInputTrackEvent->GetLongestTopLevelTrack();
    if( longTrack == NULL ) 
    { 
        cout << "REST Warning. TRestFindTrackBlobsProcess. Long track not found." << endl;
        fInputTrackEvent->PrintEvent();
        return NULL;
    }

    TRestHits *hits = (TRestHits *) longTrack->GetVolumeHits();

    Int_t nHits = hits->GetNumberOfHits();

    Int_t nCheck = (Int_t) (longTrack->GetNumberOfHits() * fHitsToCheckFraction);

    Int_t hit1 = hits->GetMostEnergeticHitInRange( 0, nCheck );
    Int_t hit2 = hits->GetMostEnergeticHitInRange( nHits - nCheck, nHits );

    // The blob with z-coordinate closer to z=0 is stored in x1,y1,z1 
    Double_t x1,y1,z1;
    Double_t x2,y2,z2;
    if( fabs( hits->GetZ( hit1 ) ) < fabs( hits->GetZ( hit2 ) ) ) 
    {
        x1 = hits->GetX( hit1 );
        y1 = hits->GetY( hit1 );
        z1 = hits->GetZ( hit1 );

        x2 = hits->GetX( hit2 );
        y2 = hits->GetY( hit2 );
        z2 = hits->GetZ( hit2 );
    }
    else
    {
        x2 = hits->GetX( hit1 );
        y2 = hits->GetY( hit1 );
        z2 = hits->GetZ( hit1 );

        x1 = hits->GetX( hit2 );
        y1 = hits->GetY( hit2 );
        z1 = hits->GetZ( hit2 );
    }

    TString obsName;

    obsName = this->GetName() + (TString) "_x1";
    fAnalysisTree->SetObservableValue( obsName, x1 );

    obsName = this->GetName() + (TString) "_y1";
    fAnalysisTree->SetObservableValue( obsName, y1 );

    obsName = this->GetName() + (TString) "_z1";
    fAnalysisTree->SetObservableValue( obsName, z1 );

    /////

    obsName = this->GetName() + (TString) "_x2";
    fAnalysisTree->SetObservableValue( obsName, x2 );

    obsName = this->GetName() + (TString) "_y2";
    fAnalysisTree->SetObservableValue( obsName, y2 );

    obsName = this->GetName() + (TString) "_z2";
    fAnalysisTree->SetObservableValue( obsName, z2 );

    Double_t dist = (x1-x2) * (x1-x2) + (y1-y2) * (y1-y2) + (z1-z2) * (z1-z2);
    dist = TMath::Sqrt( dist );

    obsName = this->GetName() + (TString) "_distance";
    fAnalysisTree->SetObservableValue( obsName, dist );

    // We get the hit blob energy from the origin track (not from the reduced track)
    Int_t longTrackId = longTrack->GetTrackID();
    TRestTrack *originTrack = fInputTrackEvent->GetOriginTrackById( longTrackId );
    TRestHits *originHits = (TRestHits *) ( originTrack->GetVolumeHits() );
    
    for( unsigned int n = 0; n < fQ1_Observables.size(); n++ )
    {
        Double_t q = originHits->GetEnergyInSphere( x1, y1, z1, fQ1_Radius[n] );

        obsName = this->GetName() + (TString) "_" + (TString) fQ1_Observables[n];
        fAnalysisTree->SetObservableValue( obsName, q );
    }

    for( unsigned int n = 0; n < fQ2_Observables.size(); n++ )
    {
        Double_t q = originHits->GetEnergyInSphere( x2, y2, z2, fQ2_Radius[n] );

        obsName = this->GetName() + (TString) "_" + (TString) fQ2_Observables[n];
        fAnalysisTree->SetObservableValue( obsName, q );
    }

    return fOutputTrackEvent;
}

//______________________________________________________________________________
void TRestFindTrackBlobsProcess::EndOfEventProcess() 
{
}

//______________________________________________________________________________
void TRestFindTrackBlobsProcess::EndProcess()
{
    // Function to be executed once at the end of the process 
    // (after all events have been processed)

    //Start by calling the EndProcess function of the abstract class. 
    //Comment this if you don't want it.
    //TRestEventProcess::EndProcess();
}

//______________________________________________________________________________
void TRestFindTrackBlobsProcess::InitFromConfigFile( )
{
    fHitsToCheckFraction = StringToDouble( GetParameter( "hitsToCheckFraction", "0.2" ) );
}

