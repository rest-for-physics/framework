///______________________________________________________________________________
///______________________________________________________________________________
///             
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestTrackAnalysisProcess.cxx
///
///
///             First implementation of Geant4 analysis process into REST_v2
///             Date : mar/2016
///             Author : J. Galan
///
///_______________________________________________________________________________


#include "TRestTrackAnalysisProcess.h"
using namespace std;


ClassImp(TRestTrackAnalysisProcess)
    //______________________________________________________________________________
TRestTrackAnalysisProcess::TRestTrackAnalysisProcess()
{
    Initialize();
}

//______________________________________________________________________________
TRestTrackAnalysisProcess::TRestTrackAnalysisProcess( char *cfgFileName )
{
    Initialize();

    if( LoadConfigFromFile( cfgFileName ) ) LoadDefaultConfig( );
}

//______________________________________________________________________________
TRestTrackAnalysisProcess::~TRestTrackAnalysisProcess()
{
    delete fInputTrackEvent;
    delete fOutputTrackEvent;
}

void TRestTrackAnalysisProcess::LoadDefaultConfig()
{
    SetTitle( "Default config" );
}

//______________________________________________________________________________
void TRestTrackAnalysisProcess::Initialize()
{
    SetSectionName( this->ClassName() );

    fInputTrackEvent = new TRestTrackEvent();
    fOutputTrackEvent = new TRestTrackEvent();

    fInputEvent = fInputTrackEvent;
    fOutputEvent = fOutputTrackEvent;

    fCutsEnabled = false;
}

void TRestTrackAnalysisProcess::LoadConfig( std::string cfgFilename, std::string name )
{
    if( LoadConfigFromFile( cfgFilename, name ) ) LoadDefaultConfig( );
}

//______________________________________________________________________________
void TRestTrackAnalysisProcess::InitProcess()
{
    std::vector <string> fObservables;
    fObservables = TRestEventProcess::ReadObservables();

    for( unsigned int i = 0; i < fObservables.size(); i++ )
        if( fObservables[i].find( "nTracks_LE_" ) != string::npos )
        {
            Double_t energy = StringToDouble ( fObservables[i].substr( 11, fObservables[i].length() ).c_str() );

            fTrack_LE_EnergyObservables.push_back( fObservables[i] );
            fTrack_LE_Threshold.push_back( energy );
            nTracks_LE.push_back(0);
        }

    for( unsigned int i = 0; i < fObservables.size(); i++ )
        if( fObservables[i].find( "nTracks_HE_" ) != string::npos )
        {
            Double_t energy = StringToDouble ( fObservables[i].substr( 11, fObservables[i].length() ).c_str() );

            fTrack_HE_EnergyObservables.push_back( fObservables[i] );
            fTrack_HE_Threshold.push_back( energy );
            nTracks_HE.push_back(0);
        }


       for( unsigned int i = 0; i < fObservables.size(); i++ )
        if( fObservables[i].find( "nTracks_En_" ) != string::npos )
        {
            Double_t energy = StringToDouble ( fObservables[i].substr( 11, fObservables[i].length() ).c_str() );

            fTrack_En_EnergyObservables.push_back( fObservables[i] );
            fTrack_En_Threshold.push_back( energy );
            nTracks_En.push_back(0);

        }
}

//______________________________________________________________________________
void TRestTrackAnalysisProcess::BeginOfEventProcess() 
{
    fOutputTrackEvent->Initialize();
}

//______________________________________________________________________________
TRestEvent* TRestTrackAnalysisProcess::ProcessEvent( TRestEvent *evInput )
{
    TRestTrackEvent *fInputTrackEvent =  (TRestTrackEvent *) evInput;

    // Copying the input tracks to the output track
    for( int tck = 0; tck < fInputTrackEvent->GetNumberOfTracks(); tck++ )
        fOutputTrackEvent->AddTrack( fInputTrackEvent->GetTrack(tck) ); 

    if( this->GetVerboseLevel() >= REST_Debug )
        fInputTrackEvent->PrintOnlyTracks();

    TString obsName;

    Double_t tckLenX = 0;
    Double_t tckLenY = 0;
    Double_t tckLenXYZ=0;
    Double_t tckMaxLenXYZ=0;
    Double_t tckMaxEnXYZ=0;
    Int_t nTracksX = 0;
    Int_t nTracksY = 0;
    Int_t nTracksXYZ = 0;
    Double_t maxX = 0, maxY = 0, maxZ = 0;;

    for( unsigned int n = 0; n < nTracks_HE.size(); n++ )
        nTracks_HE[n] = 0;
    for( unsigned int n = 0; n < nTracks_LE.size(); n++ )
        nTracks_LE[n] = 0;

    Double_t totalEnergy = 0;
    for( int tck = 0; tck < fInputTrackEvent->GetNumberOfTracks(); tck++ )
    {
        if( !fInputTrackEvent->isTopLevel( tck ) ) continue;

        TRestTrack *t = fInputTrackEvent->GetTrack( tck );
        Double_t en = t->GetEnergy( );
        totalEnergy += en;

        if( t->isXZ() )
        {
            if( t->GetTrackLength() >= 0 )
            {
                nTracksX++;
                tckLenX += t->GetTrackLength();
            }
        }

        if( t->isYZ() )
        {
            if( t->GetTrackLength() >= 0 )
            {
                nTracksY++;
                tckLenY += t->GetTrackLength();
            }
        }

        if( t->isXYZ() )
        {
            nTracksXYZ++;
            tckLenXYZ += t->GetTrackLength();
            if(en > tckMaxEnXYZ)  
            {
                tckMaxEnXYZ = en;
                tckMaxLenXYZ = t->GetTrackLength();
                maxX = t->GetMeanPosition().X();
                maxY = t->GetMeanPosition().Y();
                maxZ = t->GetMeanPosition().Z();
            }
        }


        for( unsigned int n = 0; n < fTrack_HE_EnergyObservables.size(); n++ )
            if( en > fTrack_HE_Threshold[n] )
                nTracks_HE[n]++;

        for( unsigned int n = 0; n < fTrack_LE_EnergyObservables.size(); n++ )
            if( en < fTrack_LE_Threshold[n] )
                nTracks_LE[n]++;

        for( unsigned int n = 0; n < fTrack_En_EnergyObservables.size(); n++ )
            if( en > fTrack_En_Threshold[n] - fDeltaEnergy && en < fTrack_En_Threshold[n] + fDeltaEnergy )
                nTracks_En[n]++;
    }

    Double_t trackEnergyRatioXYZ = 0;
    if( nTracksXYZ > 1 )
        trackEnergyRatioXYZ = (totalEnergy - tckMaxEnXYZ) / totalEnergy;

    obsName = this->GetName() + (TString) ".nTrackEnergyRatioXYZ";
    fAnalysisTree->SetObservableValue( obsName, trackEnergyRatioXYZ );

    Double_t evTimeDelay = 0;
    if( fPreviousEventTime.size() > 0 )
        evTimeDelay = fInputTrackEvent->GetTime() - fPreviousEventTime.back();
    obsName = this->GetName() + (TString) ".EventTimeDelay";
    fAnalysisTree->SetObservableValue( obsName, evTimeDelay );

    Double_t meanRate = 0;
    if( fPreviousEventTime.size() == 100 )
        meanRate = 100. / (fInputTrackEvent->GetTime()-fPreviousEventTime.front());

    obsName = this->GetName() + (TString) ".MeanRate_InHz";
    fAnalysisTree->SetObservableValue( obsName, meanRate );

    fInputTrackEvent->SetNumberOfXTracks( nTracksX );
    fInputTrackEvent->SetNumberOfYTracks( nTracksY );

    if( fCutsEnabled )
    {
        if( nTracksX < fNTracksXCut.X() || nTracksX > fNTracksXCut.Y() ) return NULL;
        if( nTracksY < fNTracksYCut.X() || nTracksY > fNTracksYCut.Y() ) return NULL;
    }

    Double_t x = 0, y = 0;

    TRestTrack *tX = fInputTrackEvent->GetMaxEnergyTrackInX();
    if( tX != NULL )
        x = tX->GetMeanPosition().X();

    TRestTrack *tY = fInputTrackEvent->GetMaxEnergyTrackInY();
    if( tY != NULL )
        y = tY->GetMeanPosition().Y();

  
   
    obsName = this->GetName() + (TString) ".xMean";
    fAnalysisTree->SetObservableValue( obsName, x );

    obsName = this->GetName() + (TString) ".yMean";
    fAnalysisTree->SetObservableValue( obsName, y );

    obsName = this->GetName() + (TString) ".LengthX";
    fAnalysisTree->SetObservableValue( obsName, tckLenX );

    obsName = this->GetName() + (TString) ".LengthY";
    fAnalysisTree->SetObservableValue( obsName, tckLenY );

    obsName = this->GetName() + (TString) ".nTracksX";
    fAnalysisTree->SetObservableValue( obsName, nTracksX );

    obsName = this->GetName() + (TString) ".nTracksY";
    fAnalysisTree->SetObservableValue( obsName, nTracksY );

    obsName = this->GetName() + (TString) ".nTracksXYZ";
    fAnalysisTree->SetObservableValue( obsName, nTracksXYZ );

    obsName = this->GetName() + (TString) ".MaxLengthXYZ";
    fAnalysisTree->SetObservableValue( obsName, tckMaxLenXYZ );

    obsName = this->GetName() + (TString) ".MaxEnXYZ";
    fAnalysisTree->SetObservableValue( obsName, tckMaxEnXYZ );

    obsName = this->GetName() + (TString) ".maxXMean";
    fAnalysisTree->SetObservableValue( obsName, maxX );
    obsName = this->GetName() + (TString) ".maxYMean";
    fAnalysisTree->SetObservableValue( obsName, maxY );
    obsName = this->GetName() + (TString) ".maxZMean";
    fAnalysisTree->SetObservableValue( obsName, maxZ );

    for( unsigned int n = 0; n < fTrack_LE_EnergyObservables.size(); n++ )
    {
        TString obsName = fTrack_LE_EnergyObservables[n];
        obsName = this->GetName( ) + (TString) "." + obsName;
        fAnalysisTree->SetObservableValue( obsName, nTracks_LE[n] );
    }

    for( unsigned int n = 0; n < fTrack_HE_EnergyObservables.size(); n++ )
    {
        TString obsName = fTrack_HE_EnergyObservables[n];
        obsName = this->GetName( ) + (TString) "." + obsName;
        fAnalysisTree->SetObservableValue( obsName, nTracks_HE[n] );
    }

     for( unsigned int n = 0; n < fTrack_En_EnergyObservables.size(); n++ )
    {
        TString obsName = fTrack_En_EnergyObservables[n];
        obsName = this->GetName( ) + (TString) "." + obsName;
        fAnalysisTree->SetObservableValue( obsName, nTracks_En[n] );
    }

    return fOutputTrackEvent;
}

//______________________________________________________________________________
void TRestTrackAnalysisProcess::EndOfEventProcess() 
{
    fPreviousEventTime.push_back( fInputTrackEvent->GetTimeStamp() );
    if( fPreviousEventTime.size() > 100 ) fPreviousEventTime.erase( fPreviousEventTime.begin() );
}

//______________________________________________________________________________
void TRestTrackAnalysisProcess::EndProcess()
{
    // Function to be executed once at the end of the process 
    // (after all events have been processed)

    //Start by calling the EndProcess function of the abstract class. 
    //Comment this if you don't want it.
    //TRestEventProcess::EndProcess();
}

//______________________________________________________________________________
void TRestTrackAnalysisProcess::InitFromConfigFile( )
{
    fNTracksXCut = StringTo2DVector( GetParameter( "nTracksXCut", "(1,10)") );
    fNTracksYCut = StringTo2DVector( GetParameter( "nTracksYCut", "(1,10)") );
    fDeltaEnergy = GetDblParameterWithUnits( "deltaEnergy", 1 );

    if( GetParameter( "cutsEnabled", "false" ) == "true" ) fCutsEnabled = true;
}

