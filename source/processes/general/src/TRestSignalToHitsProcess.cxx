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

#include <TRestDetectorSetup.h>

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
    fThreshold = 400;
    fElectricField = 1000;
    fGasPressure = 10;

}

void TRestSignalToHitsProcess::LoadConfig( std::string cfgFilename, std::string name )
{
    if( LoadConfigFromFile( cfgFilename, name ) ) LoadDefaultConfig( );

    // If the parameters have no value it tries to obtain it from detector setup

    if( fElectricField == PARAMETER_NOT_FOUND_DBL )
    {
	    TRestDetectorSetup *detSetup = (TRestDetectorSetup *) this->GetDetectorSetup();
	    if ( detSetup != NULL )
	    {
		    fElectricField = detSetup->GetFieldInVPerCm( );
		    cout << "SignalToHitsProcess : Obtainning electric field from detector setup : " << fElectricField << " V/cm" << endl;
	    }
    }

    if ( fSampling == PARAMETER_NOT_FOUND_DBL )
    {
	    TRestDetectorSetup *detSetup = (TRestDetectorSetup *) this->GetDetectorSetup();
	    if ( detSetup != NULL )
	    {
		    fSampling = detSetup->GetSamplingInMicroSeconds( );
		    cout << "SignalToHitsProcess : Obtainning sampling from detector setup : " << fSampling << " us" << endl;
	    }
    }

    if( fGasPressure == -1 )
    {
	    TRestDetectorSetup *detSetup = (TRestDetectorSetup *) this->GetDetectorSetup();
	    if ( detSetup != NULL )
	    {
		    fGasPressure = detSetup->GetPressureInBar( );
		    cout << "SignalToHitsProcess : Obtainning gas pressure from detector setup : " << fGasPressure << " bar" << endl;
	    }


    }

/* THIS IS OBSOLETE ( NOW WE SHOULD DEFINE TRestDetectorSetup inside TRestRun, TRestDetectorSetup defines field, pressure, sampling, etc )
    if ( fElectricField == PARAMETER_NOT_FOUND_DBL )
    {	
        fElectricField = this->GetDoubleParameterFromClassWithUnits( "TRestElectronDiffusionProcess", "electricField" );
        if( fElectricField != PARAMETER_NOT_FOUND_DBL )
            cout << "Getting electric field from electronDiffusionProcess : " << fElectricField << " V/cm" << endl;
    }

    GetChar();

    if ( fSampling == PARAMETER_NOT_FOUND_DBL )
    {
        fSampling = this->GetDoubleParameterFromClassWithUnits( "TRestHitsToSignalProcess", "sampling" );
        if( fSampling != PARAMETER_NOT_FOUND_DBL )
            cout << "Getting sampling rate from hitsToSignal process : " << fSampling << " um" << endl;
    }
*/
}

//______________________________________________________________________________
void TRestSignalToHitsProcess::Initialize()
{
    SetSectionName( this->ClassName() );

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

    fGas = (TRestGas *) this->GetGasMetadata( );
    if( fGas != NULL )
    {
        if( fGasPressure == -1 ) 
            fGasPressure = fGas->GetPressure();
        fGas->SetPressure( fGasPressure );

        if( fDriftVelocity == 0 )
            fDriftVelocity = fGas->GetDriftVelocity( fElectricField ) * cmTomm;
    }
    else
    {
        cout << "REST_WARNING. No TRestGas found in TRestRun." << endl;
    }

    fReadout = (TRestReadout *) this->GetReadoutMetadata( );

    if( fReadout == NULL )
    {
        cout << "REST ERRORRRR : Readout has not been initialized" << endl;
        exit(-1);
    }
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

    // TODO we must take this values from configuration
    fSignalEvent->SubstractBaselines( 5, 100 );

    fHitsEvent->SetID( fSignalEvent->GetID() );
    fHitsEvent->SetSubID( fSignalEvent->GetSubID() );
    fHitsEvent->SetTimeStamp( fSignalEvent->GetTimeStamp() );
    fHitsEvent->SetSubEventTag( fSignalEvent->GetSubEventTag() );

    Int_t numberOfSignals = fSignalEvent->GetNumberOfSignals();

    Double_t energy, x, y, z;
    Int_t planeID, readoutChannel = -1, readoutModule;
    for( int i = 0; i < numberOfSignals; i++ )
    {
        TRestSignal *sgnl = fSignalEvent->GetSignal( i );
        Int_t signalID = sgnl->GetSignalID();

        for( int p = 0; p < fReadout->GetNumberOfReadoutPlanes(); p++ )
        {
            TRestReadoutPlane *plane = fReadout->GetReadoutPlane( p );
            for( int m = 0; m < plane->GetNumberOfModules(); m++ )
            {
                TRestReadoutModule *mod = plane->GetModule( m );

                if( mod->isDaqIDInside( signalID ) )
                {
                    planeID = p;
                    readoutChannel = mod->DaqToReadoutChannel( signalID );
                    readoutModule = mod->GetModuleID();
                }
            }

        }


        if ( readoutChannel == -1 ) continue;
        /////////////////////////////////////////////////////////////////////////

        TRestReadoutPlane *plane = fReadout->GetReadoutPlane( planeID );
        
        // For the moment this will only be valid for a TPC with its axis (field direction) being in z
        Double_t fieldZDirection = plane->GetPlaneVector().Z();
        Double_t zPosition = plane->GetPosition().Z();

        Double_t thr = fThreshold * sgnl->GetBaseLineSigma( 5, 100 );
        for( int j = 0; j < sgnl->GetNumberOfPoints(); j++ )
        {
            energy = sgnl->GetData(j);
            if( energy < thr ) continue;

            Double_t distanceToPlane = ( sgnl->GetTime(j) * fSampling ) * fDriftVelocity;

            z = zPosition + fieldZDirection * distanceToPlane;

            x = plane->GetX( readoutModule, readoutChannel );
            y = plane->GetY( readoutModule, readoutChannel );
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
    fElectricField = GetDblParameterWithUnits( "electricField" );
    fSampling = GetDblParameterWithUnits( "sampling" );
    fThreshold = StringToDouble( GetParameter( "threshold" ) );
    fGasPressure = StringToDouble( GetParameter( "gasPressure", "-1" ) );
    fDriftVelocity = StringToDouble( GetParameter( "driftVelocity" , "0") ) * cmTomm;
}

