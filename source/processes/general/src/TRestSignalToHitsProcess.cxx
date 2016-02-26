///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///             
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestSignalToHitsProcess.cxx
///
///             jan 2016:  Javier Galan
///_______________________________________________________________________________


#include "TRestSignalToHitsProcess.h"
using namespace std;

const double cmTomm = 10.;

ClassImp(TRestSignalToHitsProcess)
    //______________________________________________________________________________
TRestSignalToHitsProcess::TRestSignalToHitsProcess()
{
    Initialize();
}

//______________________________________________________________________________
TRestSignalToHitsProcess::TRestSignalToHitsProcess( char *cfgFileName )
{
    Initialize();

    LoadConfig( cfgFileName );

    // TRestSignalToHitsProcess default constructor
}

//______________________________________________________________________________
TRestSignalToHitsProcess::~TRestSignalToHitsProcess()
{
    if( fReadout != NULL ) delete fReadout;

    delete fHitsEvent;
    delete fSignalEvent;
    // TRestSignalToHitsProcess destructor
}

void TRestSignalToHitsProcess::LoadDefaultConfig( )
{
    SetName( "signalToHitsProcess-Default" );
    SetTitle( "Default config" );

    cout << "Signal to hits metadata not found. Loading default values" << endl;

    fSampling = 1;
    fCathodePosition = -1000;
    fAnodePosition = 0;
    fElectricField = 1000;

}

void TRestSignalToHitsProcess::LoadConfig( string cfgFilename )
{
    if( LoadConfigFromFile( cfgFilename ) ) LoadDefaultConfig( );

    this->PrintConfigBuffer();

    // The gas metadata will only be available after using AddProcess method of TRestRun
    fGas = (TRestGas *) this->GetGasMetadata( );
    if( fGas != NULL )
    {
        fGas->LoadGasFile( );
        cout << "Gas loaded from Run metadata" << endl;
    }
    else
    {
        cout << "I did not find the gas inside run. Loading gas from config file" << endl;
        fGas = new TRestGas( cfgFilename.c_str() );
    }

    if( fGas != NULL ) fGas->PrintMetadata( );


    // The readout metadata will only be available after using AddProcess method of TRestRun
    fReadout = (TRestReadout *) this->GetReadoutMetadata( );

    if( fReadout == NULL )
        fReadout = new TRestReadout( cfgFilename.c_str() );

    //if( fReadout != NULL ) fReadout->PrintMetadata();

    // If the parameters have no value it tries to obtain it from electronDiffusionProcess
    if ( fElectricField == PARAMETER_NOT_FOUND_DBL )
    {	
        fElectricField = this->GetDoubleParameterFromClassWithUnits( "TRestElectronDiffusionProcess", "electricField" );
        if( fElectricField != PARAMETER_NOT_FOUND_DBL )
            cout << "Getting electric field from electronDiffusionProcess : " << fElectricField << " V/cm" << endl;
    }

    if ( fCathodePosition == PARAMETER_NOT_FOUND_DBL )
    {
        fCathodePosition = this->GetDoubleParameterFromClassWithUnits( "TRestElectronDiffusionProcess", "cathodePosition" );
        if( fCathodePosition != PARAMETER_NOT_FOUND_DBL )
            cout << "Getting cathode position from electronDiffusionProcess : " << fCathodePosition << " mm" << endl;
    }

    if ( fAnodePosition == PARAMETER_NOT_FOUND_DBL )
    {
        fAnodePosition = this->GetDoubleParameterFromClassWithUnits( "TRestElectronDiffusionProcess", "anodePosition" );
        if( fAnodePosition != PARAMETER_NOT_FOUND_DBL )
            cout << "Getting cathode position from electronDiffusionProcess : " << fAnodePosition << " mm" << endl;
    }

    if ( fSampling == PARAMETER_NOT_FOUND_DBL )
    {
        fSampling = this->GetDoubleParameterFromClassWithUnits( "TRestHitsToSignalProcess", "sampling" );
        if( fCathodePosition != PARAMETER_NOT_FOUND_DBL )
            cout << "Getting sampling rate from hitsToSignal process : " << fSampling << " um" << endl;
    }
}

//______________________________________________________________________________
void TRestSignalToHitsProcess::Initialize()
{
    SetName( "signalToHitsProcess" );

    fHitsEvent = new TRestHitsEvent();

    fSignalEvent = new TRestSignalEvent();

    fInputEvent = fSignalEvent;
    fOutputEvent = fHitsEvent;

    fGas = NULL;
    fReadout = NULL;
}

//______________________________________________________________________________
void TRestSignalToHitsProcess::InitProcess()
{
    // Function to be executed once at the beginning of process
    // (before starting the process of the events)

    //Start by calling the InitProcess function of the abstract class. 
    //Comment this if you don't want it.
    //TRestEventProcess::InitProcess();

    if( fReadout == NULL ) cout << "REST ERRORRRR : Readout has not been initialized" << endl;
    if( fGas == NULL ) cout << "REST ERROR : Gas metadata is not initialized!" << endl;

}

//______________________________________________________________________________
void TRestSignalToHitsProcess::BeginOfEventProcess() 
{
    fHitsEvent->Initialize(); 
}


//______________________________________________________________________________
TRestEvent* TRestSignalToHitsProcess::ProcessEvent( TRestEvent *evInput )
{
    fSignalEvent = (TRestSignalEvent *) evInput;

    Int_t numberOfSignals = fSignalEvent->GetNumberOfSignals();

    Int_t numberOfHits = 0;
    Double_t energy, x, y, z;
    Int_t readoutChannel, readoutModule;
    for( int i = 0; i < numberOfSignals; i++ )
    {
        TRestSignal *sgnl = fSignalEvent->GetSignal( i );
        Int_t signalID = sgnl->GetSignalID();

         /////////////////////////////////////////////////////////////////////////
         // Here we still need to implement the decoding in the future
         // In simulations the channel numbering is just in order

         Int_t channels = 0;
         Int_t channelsBef;
         for( int md = 0; md < fReadout->GetNumberOfModules(); md++ )
         {
             channelsBef = channels;
             channels += fReadout->GetReadoutModule(md)->GetNumberOfChannels();
             if( signalID < channels )
             {
                 readoutModule = md+1;
                 readoutChannel = signalID - channelsBef;
                 break;
             }
         }

        /////////////////////////////////////////////////////////////////////////

        numberOfHits += fSignalEvent->GetSignal( i )->GetNumberOfPoints(); 
        for( int j = 0; j < sgnl->GetNumberOfPoints(); j++ )
        {
             energy = sgnl->GetData(j);

             z = ( sgnl->GetTime(j) * fSampling ) * (fGas->GetDriftVelocity( fElectricField ) * cmTomm );
             if( fCathodePosition - fAnodePosition < 0 ) z += fCathodePosition;
             else z -= fCathodePosition;

             x = fReadout->GetX( readoutModule, readoutChannel );
             y = fReadout->GetY( readoutModule, readoutChannel );
             fHitsEvent->AddHit( x, y, z, energy );
        }
    }

    return fHitsEvent;
}

//______________________________________________________________________________
void TRestSignalToHitsProcess::EndOfEventProcess() 
{

}

//______________________________________________________________________________
void TRestSignalToHitsProcess::EndProcess()
{
    // Function to be executed once at the end of the process 
    // (after all events have been processed)

    //Start by calling the EndProcess function of the abstract class. 
    //Comment this if you don't want it.
    //TRestEventProcess::EndProcess();
}

//______________________________________________________________________________
void TRestSignalToHitsProcess::InitFromConfigFile( )
{
    fCathodePosition = GetDblParameterWithUnits( "cathodePosition" );
    fAnodePosition = GetDblParameterWithUnits( "anodePosition" );
    fElectricField = GetDblParameterWithUnits( "electricField" );
    fSampling = GetDblParameterWithUnits( "sampling" );
}

