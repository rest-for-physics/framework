///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///             
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestHitsToSignalProcess.cxx
///
///             oct 2015:  Javier Galan
///_______________________________________________________________________________


#include "TRestHitsToSignalProcess.h"
using namespace std;


/* Chrono can be used for measuring time with better precision and test the time spent for different parts of the code
   using high_resolution_clock 
#include <iostream>
#include <chrono>

using namespace std;
using namespace std::chrono;
 */

const double cmTomm = 10.;

ClassImp(TRestHitsToSignalProcess)
    //______________________________________________________________________________
TRestHitsToSignalProcess::TRestHitsToSignalProcess()
{
    Initialize();

}

// __________________________________________________________
//     TODO : Perhaps this constructor should be removed
//            since we will allway load the config from TRestRun
//            when we use AddProcess. It would be necessary only if we use the process stand alone
//            but even then we could just call LoadConfig
//            __________________________________________________________
TRestHitsToSignalProcess::TRestHitsToSignalProcess( char *cfgFileName )
{
    Initialize();

    if( LoadConfigFromFile( cfgFileName ) == -1 ) LoadDefaultConfig( );

    PrintMetadata();

    if( fReadout == NULL ) fReadout = new TRestReadout( cfgFileName );

    // TRestHitsToSignalProcess default constructor
}


//______________________________________________________________________________
TRestHitsToSignalProcess::~TRestHitsToSignalProcess()
{
    if( fReadout != NULL ) delete fReadout;

    delete fHitsEvent;
    delete fSignalEvent;
    // TRestHitsToSignalProcess destructor
}

void TRestHitsToSignalProcess::LoadDefaultConfig( )
{
    SetName( "hitsToSignalProcess-Default" );
    SetTitle( "Default config" );

    cout << "Hits to signal metadata not found. Loading default values" << endl;

    fSampling = 1;
    fCathodePosition = 0;
    fElectricField = 1000;

}

void TRestHitsToSignalProcess::LoadConfig( string cfgFilename )
{
    if( LoadConfigFromFile( cfgFilename ) ) LoadDefaultConfig( );

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

    if( fReadout != NULL )
        cout << "Readout imported from Run metadata" << endl;
    else
        fReadout = new TRestReadout( cfgFilename.c_str() );

    if( fReadout != NULL ) fReadout->PrintMetadata();

    // If the parameters have no value it tries to obtain it from electronDiffusionProcess
    if ( fElectricField == PARAMETER_NOT_FOUND_DBL )
    {
        fElectricField = this->GetDoubleParameterFromClassWithUnits( "TRestElectronDiffusionProcess", "electricField" );
        if( fElectricField != PARAMETER_NOT_FOUND_DBL )
        {
            cout << "Getting electric field from electronDiffusionProcess : " << fElectricField << " V/cm" << endl;
        }

    }

    if ( fCathodePosition == PARAMETER_NOT_FOUND_DBL )
    {
        fCathodePosition = this->GetDoubleParameterFromClassWithUnits( "TRestElectronDiffusionProcess", "cathodePosition" );
        if( fCathodePosition != PARAMETER_NOT_FOUND_DBL )
        {
            cout << "Getting cathode position from electronDiffusionProcess : " << fCathodePosition << " mm" << endl;
        }
    }
}

//______________________________________________________________________________
void TRestHitsToSignalProcess::Initialize()
{
    SetName("hitsToSignalProcess");

    fReadout = NULL;
    fGas = NULL;

    fHitsEvent = new TRestHitsEvent();

    fSignalEvent = new TRestSignalEvent();

    fInputEvent = fHitsEvent;
    fOutputEvent = fSignalEvent;
}

//______________________________________________________________________________
void TRestHitsToSignalProcess::InitProcess()
{
    // Function to be executed once at the beginning of process
    // (before starting the process of the events)

    //Start by calling the InitProcess function of the abstract class. 
    //Comment this if you don't want it.
    //TRestEventProcess::InitProcess();

    if( fReadout == NULL ) cout << "REST ERRORRRR : Readout has not been initialized" << endl;
    if( fGas == NULL ) cout << "REST ERROR: Gas has not been initialized" << endl;
}

//______________________________________________________________________________
void TRestHitsToSignalProcess::BeginOfEventProcess() 
{
    //cout << "Begin of event process" << endl;
    fSignalEvent->Initialize(); 
}

Int_t TRestHitsToSignalProcess::FindModule( Int_t readoutPlane, Double_t x, Double_t y )
{
    // TODO verify this
        TRestReadoutPlane *plane = fReadout->GetReadoutPlane( readoutPlane );
        for ( int md = 0; md < plane->GetNumberOfModules(); md++ )
            if( plane->GetReadoutModule( md )->isInside( x, y ) ) return md;

    return -1;
}


//______________________________________________________________________________
TRestEvent* TRestHitsToSignalProcess::ProcessEvent( TRestEvent *evInput )
{
    fHitsEvent = (TRestHitsEvent *) evInput;

       cout << "Event ID : " << fHitsEvent->GetID() << endl;
       cout << "Number of hits : " << fHitsEvent->GetNumberOfHits() << endl;
       cout << "--------------------------" << endl;

    for( int hit = 0; hit < fHitsEvent->GetNumberOfHits(); hit++ )
    {
        Double_t x = fHitsEvent->GetX( hit );
        Double_t y = fHitsEvent->GetY( hit );

        // TODO : Here we must check to which readout plane it is related the event. For the moment we write the first plane for testing
        Int_t mod = this->FindModule( 0, x, y );
        TRestReadoutPlane *plane = fReadout->GetReadoutPlane( 0 );
	
	//cout<<"module "<<mod<<endl;

        if( mod >= 0 )
        {
        //    high_resolution_clock::time_point t1 = high_resolution_clock::now();
            Int_t chnl = plane->GetReadoutModule( mod )->FindChannel( x, y );
        //    high_resolution_clock::time_point t2 = high_resolution_clock::now();

            /*
            auto duration = duration_cast<microseconds>( t2 - t1 ).count();
            cout << duration << " us" << endl;
            getchar();
            */
            if( chnl >= 0 )
            {
                Int_t channelID = 0;
                for( int n = 0; n < mod-1; n++ )
                    channelID += plane->GetReadoutModule(n)->GetNumberOfChannels();
                channelID += chnl;

                //cout << "Hit found in channel : " << chnl << " chID : " << channelID << endl;

                /* This is now done internally in TRestSignalEvent
                   if ( !fSignalEvent->signalIDExists( channelID ) )
                   {
                   TRestSignal sgnl;
                   sgnl.SetSignalID( channelID );
                   fSignalEvent->AddSignal( sgnl );
                   }
                   */

                Double_t energy = fHitsEvent->GetEnergy( hit );
                Double_t time = fHitsEvent->GetZ( hit ) - fCathodePosition;
                time = time /(fGas->GetDriftVelocity( fElectricField ) * cmTomm );
                time = ( (Int_t) (time/fSampling) );

                fSignalEvent->AddChargeToSignal( channelID, time, energy );
            }
        }
    }

    fSignalEvent->SortSignals();

    //fSignalEvent->PrintEvent();

    cout << "Number of signals inside event : " << fSignalEvent->GetNumberOfSignals() << endl;

    return fSignalEvent;
}

//______________________________________________________________________________
void TRestHitsToSignalProcess::EndOfEventProcess() 
{

}

//______________________________________________________________________________
void TRestHitsToSignalProcess::EndProcess()
{
    // Function to be executed once at the end of the process 
    // (after all events have been processed)

    //Start by calling the EndProcess function of the abstract class. 
    //Comment this if you don't want it.
    //TRestEventProcess::EndProcess();
}

//______________________________________________________________________________
void TRestHitsToSignalProcess::InitFromConfigFile( )
{
    fSampling = GetDblParameterWithUnits( "sampling" );
    fCathodePosition = GetDblParameterWithUnits( "cathodePosition" );
    fElectricField = GetDblParameterWithUnits( "electricField" );
}

