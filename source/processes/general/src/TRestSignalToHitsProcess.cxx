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

    if( GetVerboseLevel() >= REST_Debug )
        fSignalEvent->PrintEvent();

    fHitsEvent->SetID( fSignalEvent->GetID() );
    fHitsEvent->SetSubID( fSignalEvent->GetSubID() );
    fHitsEvent->SetTimeStamp( fSignalEvent->GetTimeStamp() );
    fHitsEvent->SetSubEventTag( fSignalEvent->GetSubEventTag() );

    Int_t numberOfSignals = fSignalEvent->GetNumberOfSignals();

    Int_t planeID, readoutChannel = -1, readoutModule;
    for( int i = 0; i < numberOfSignals; i++ )
    {
        TRestSignal *sgnl = fSignalEvent->GetSignal( i );
        Int_t signalID = sgnl->GetSignalID();

        if( GetVerboseLevel() >= REST_Debug )
            cout << "Searching readout coordinates for signal ID : " << signalID << endl;
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

                    if( GetVerboseLevel() >= REST_Debug ) {
                        cout << "-------------------------------------------------------------------" << endl;
                        cout << "signal Id : " << signalID << endl;
                        cout << "channel : " << readoutChannel << " module : " << readoutModule << endl;
                        cout << "-------------------------------------------------------------------" << endl; }
                }
            }
        }

        if ( readoutChannel == -1 )
        {
			cout << "REST Warning : Readout channel not found for daq ID : " << signalID << endl;
			continue;
		}
		/////////////////////////////////////////////////////////////////////////

		TRestReadoutPlane *plane = fReadout->GetReadoutPlane( planeID );

		// For the moment this will only be valid for a TPC with its axis (field direction) being in z
		Double_t fieldZDirection = plane->GetPlaneVector().Z();
		Double_t zPosition = plane->GetPosition().Z();

		Double_t x = plane->GetX( readoutModule, readoutChannel );
		Double_t y = plane->GetY( readoutModule, readoutChannel );

		if( fSignalToHitMethod == "onlyMax" )
		{
			Double_t time = sgnl->GetMaxPeakTime();
			Double_t distanceToPlane = time * fDriftVelocity;

			if( GetVerboseLevel() >= REST_Debug )
				cout << "Distance to plane : " << distanceToPlane << endl;

			Double_t z = zPosition + fieldZDirection * distanceToPlane;

			Double_t energy = sgnl->GetMaxPeakValue();

			if( GetVerboseLevel() >= REST_Debug )
				cout << "Adding hit. Time : " << time << " x : " << x << " y : " << y << " z : " << z << " Energy : " << energy << endl;

			fHitsEvent->AddHit( x, y, z, energy, 0, (Short_t) readoutModule, (Short_t) readoutChannel );
		}
		else if( fSignalToHitMethod == "tripleMax" )
		{
			Int_t bin = sgnl->GetMaxIndex();

			Double_t time = sgnl->GetTime( bin );
			Double_t energy = sgnl->GetData(bin);

			Double_t distanceToPlane = time * fDriftVelocity;
			Double_t z = zPosition + fieldZDirection * distanceToPlane;

			fHitsEvent->AddHit( x, y, z, energy, 0, (Short_t) readoutModule, (Short_t) readoutChannel );

			time = sgnl->GetTime( bin-1 );
			energy = sgnl->GetData( bin-1 );

			distanceToPlane = time * fDriftVelocity;
			z = zPosition + fieldZDirection * distanceToPlane;

			fHitsEvent->AddHit( x, y, z, energy, 0, (Short_t) readoutModule, (Short_t) readoutChannel );

			time = sgnl->GetTime( bin+1 );
			energy = sgnl->GetData( bin+1 );

			distanceToPlane = time * fDriftVelocity;
			z = zPosition + fieldZDirection * distanceToPlane;

			fHitsEvent->AddHit( x, y, z, energy, 0, (Short_t) readoutModule, (Short_t) readoutChannel );

			if( GetVerboseLevel() >= REST_Debug )
			{
				cout << "Distance to plane : " << distanceToPlane << endl;
				cout << "Adding hit. Time : " << time << " x : " << x << " y : " << y << " z : " << z << " Energy : " << energy << endl;
			}
		}
		else
		{
			for( int j = 0; j < sgnl->GetNumberOfPoints(); j++ )
			{
				Double_t energy = sgnl->GetData(j);

				if( energy <= 0 ) continue;

				Double_t distanceToPlane = sgnl->GetTime(j) * fDriftVelocity;

				if( GetVerboseLevel() >= REST_Debug )
				{
					cout << "Time : " << sgnl->GetTime(j) << " Drift velocity : " << fDriftVelocity << endl;
					cout << "Distance to plane : " << distanceToPlane << endl;
				}

				Double_t z = zPosition + fieldZDirection * distanceToPlane;

				if( GetVerboseLevel() >= REST_Debug )
					cout << "Adding hit. Time : " << sgnl->GetTime(j) << " x : " << x << " y : " << y << " z : " << z << endl;

				fHitsEvent->AddHit( x, y, z, energy, 0, (Short_t) readoutModule, (Short_t) readoutChannel );
			}
		}
    }

    if( this->GetVerboseLevel() >= REST_Info ) 
    {
        cout << "TRestSignalToHitsProcess. Hits added : " << fHitsEvent->GetNumberOfHits() << endl;
        cout << "TRestSignalToHitsProcess. Hits total energy : " << fHitsEvent->GetEnergy() << endl;
    }

    if( this->GetVerboseLevel() >= REST_Debug )
    {
        fHitsEvent->PrintEvent(300);
        GetChar();
    }

    if( fHitsEvent->GetNumberOfHits() <= 0 ) return NULL;

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
    fGasPressure = StringToDouble( GetParameter( "gasPressure", "-1" ) );
    fDriftVelocity = StringToDouble( GetParameter( "driftVelocity" , "0") ) * cmTomm;
    fSignalToHitMethod =  GetParameter( "method", "all" );

}

