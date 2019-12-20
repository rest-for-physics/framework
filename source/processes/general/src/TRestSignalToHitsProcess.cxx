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

ClassImp(TRestSignalToHitsProcess)
    //______________________________________________________________________________
    TRestSignalToHitsProcess::TRestSignalToHitsProcess() {
    Initialize();
}

//______________________________________________________________________________
TRestSignalToHitsProcess::TRestSignalToHitsProcess(char* cfgFileName) {
    Initialize();

    LoadConfig(cfgFileName);

    // TRestSignalToHitsProcess default constructor
}

//______________________________________________________________________________
TRestSignalToHitsProcess::~TRestSignalToHitsProcess() {
    delete fHitsEvent;
    delete fSignalEvent;
    // TRestSignalToHitsProcess destructor
}

void TRestSignalToHitsProcess::LoadDefaultConfig() {
    SetName("signalToHitsProcess-Default");
    SetTitle("Default config");

    cout << "Signal to hits metadata not found. Loading default values" << endl;

    fElectricField = 1000;
    fGasPressure = 10;
}

void TRestSignalToHitsProcess::LoadConfig(std::string cfgFilename, std::string name) {
    if (LoadConfigFromFile(cfgFilename, name)) LoadDefaultConfig();

    // If the parameters have no value it tries to obtain it from detector setup

    if (fElectricField == PARAMETER_NOT_FOUND_DBL) {
        TRestDetectorSetup* detSetup = (TRestDetectorSetup*)this->GetDetectorSetup();
        if (detSetup != NULL) {
            fElectricField = detSetup->GetFieldInVPerCm();
            cout << "SignalToHitsProcess : Obtainning electric field from detector "
                    "setup : "
                 << fElectricField << " V/cm" << endl;
        }
    }

    // if( fGasPressure <= 0 )
    //{
    // TRestDetectorSetup *detSetup = (TRestDetectorSetup *)
    // this->GetDetectorSetup(); if ( detSetup != NULL )
    // {
    //  fGasPressure = detSetup->GetPressureInBar( );
    //  cout << "SignalToHitsProcess : Obtainning gas pressure from detector setup
    //  : " << fGasPressure << " bar" << endl;
    // }

    //}

    /* THIS IS OBSOLETE ( NOW WE SHOULD DEFINE TRestDetectorSetup inside TRestRun,
       TRestDetectorSetup defines field, pressure, sampling, etc ) if (
       fElectricField == PARAMETER_NOT_FOUND_DBL )
            {
                    fElectricField = this->GetDoubleParameterFromClassWithUnits(
       "TRestElectronDiffusionProcess", "electricField" ); if( fElectricField !=
       PARAMETER_NOT_FOUND_DBL ) cout << "Getting electric field from
       electronDiffusionProcess : " << fElectricField << " V/cm" << endl;
            }

            GetChar();

            if ( fSampling == PARAMETER_NOT_FOUND_DBL )
            {
                    fSampling = this->GetDoubleParameterFromClassWithUnits(
       "TRestHitsToSignalProcess", "sampling" ); if( fSampling !=
       PARAMETER_NOT_FOUND_DBL ) cout << "Getting sampling rate from hitsToSignal
       process : " << fSampling << " um" << endl;
            }
    */
}

//______________________________________________________________________________
void TRestSignalToHitsProcess::Initialize() {
    SetSectionName(this->ClassName());

    fHitsEvent = new TRestHitsEvent();
    fSignalEvent = new TRestSignalEvent();

    fInputEvent = fSignalEvent;
    fOutputEvent = fHitsEvent;

    fGas = NULL;
    fReadout = NULL;
}

//______________________________________________________________________________
void TRestSignalToHitsProcess::InitProcess() {
    // Function to be executed once at the beginning of process
    // (before starting the process of the events)

    // Start by calling the InitProcess function of the abstract class.
    // Comment this if you don't want it.
    // TRestEventProcess::InitProcess();

    fGas = (TRestGas*)this->GetDriftMetadata();
    if (fGas != NULL) {
        if (fGasPressure <= 0) fGasPressure = fGas->GetPressure();
        if (fElectricField <= 0) fElectricField = fGas->GetElectricField();

        fGas->SetPressure(fGasPressure);
        fGas->SetElectricField(fElectricField);

        if (fDriftVelocity <= 0) fDriftVelocity = fGas->GetDriftVelocity();
    } else {
        warning << "No TRestGas found in TRestRun." << endl;
        if (fDriftVelocity == -1) {
            ferr << "TRestHitsToSignalProcess: drift velocity is undefined in the rml file!" << endl;
            exit(-1);
        }
    }

    fReadout = (TRestReadout*)this->GetReadoutMetadata();

    if (fReadout == NULL) {
        ferr << "Readout has not been initialized" << endl;
        exit(-1);
    }
}

//______________________________________________________________________________
void TRestSignalToHitsProcess::BeginOfEventProcess() { fHitsEvent->Initialize(); }

//______________________________________________________________________________
TRestEvent* TRestSignalToHitsProcess::ProcessEvent(TRestEvent* evInput) {
    fSignalEvent = (TRestSignalEvent*)evInput;

    if (GetVerboseLevel() >= REST_Debug) fSignalEvent->PrintEvent();

    fHitsEvent->SetID(fSignalEvent->GetID());
    fHitsEvent->SetSubID(fSignalEvent->GetSubID());
    fHitsEvent->SetTimeStamp(fSignalEvent->GetTimeStamp());
    fHitsEvent->SetSubEventTag(fSignalEvent->GetSubEventTag());

    Int_t numberOfSignals = fSignalEvent->GetNumberOfSignals();

    Int_t planeID, readoutChannel = -1, readoutModule;
    for (int i = 0; i < numberOfSignals; i++) {
        TRestSignal* sgnl = fSignalEvent->GetSignal(i);
        Int_t signalID = sgnl->GetSignalID();

        if (GetVerboseLevel() >= REST_Debug)
            cout << "Searching readout coordinates for signal ID : " << signalID << endl;
        // for( int p = 0; p < fReadout->GetNumberOfReadoutPlanes(); p++ )
        //{
        //    TRestReadoutPlane *plane = fReadout->GetReadoutPlane( p );
        //    for( int m = 0; m < plane->GetNumberOfModules(); m++ )
        //    {
        //        TRestReadoutModule *mod = plane->GetModule( m );

        //        if( mod->isDaqIDInside( signalID ) )
        //        {
        //            planeID = p;
        //            readoutChannel = mod->DaqToReadoutChannel( signalID );
        //            readoutModule = mod->GetModuleID();

        //            if( GetVerboseLevel() >= REST_Debug ) {
        //            cout <<
        //            "-------------------------------------------------------------------"
        //            << endl; cout << "signal Id : " << signalID << endl; cout <<
        //            "channel : " << readoutChannel << " module : " <<
        //            readoutModule << endl; cout <<
        //            "-------------------------------------------------------------------"
        //            << endl; }
        //        }
        //    }
        //}

        fReadout->GetPlaneModuleChannel(signalID, planeID, readoutModule, readoutChannel);

        if (readoutChannel == -1) {
            cout << "REST Warning : Readout channel not found for daq ID : " << signalID << endl;
            continue;
        }
        /////////////////////////////////////////////////////////////////////////

        TRestReadoutPlane* plane = fReadout->GetReadoutPlaneWithID(planeID);

        // For the moment this will only be valid for a TPC with its axis (field
        // direction) being in z
        Double_t fieldZDirection = plane->GetPlaneVector().Z();
        Double_t zPosition = plane->GetPosition().Z();

        Double_t x = plane->GetX(readoutModule, readoutChannel);
        Double_t y = plane->GetY(readoutModule, readoutChannel);

        REST_HitType type = XYZ;
        TRestReadoutModule* mod = plane->GetModuleByID(readoutModule);
        if (TMath::IsNaN(x)) {
            x = mod->GetPhysicalCoordinates(TVector2(mod->GetModuleSizeX(), mod->GetModuleSizeY())).X();
            type = YZ;
        } else if (TMath::IsNaN(y)) {
            y = mod->GetPhysicalCoordinates(TVector2(mod->GetModuleSizeX(), mod->GetModuleSizeY())).Y();
            type = XZ;
        }


        if (fSignalToHitMethod == "onlyMax") {
            Double_t time = sgnl->GetMaxPeakTime();
            Double_t distanceToPlane = time * fDriftVelocity;

            if (GetVerboseLevel() >= REST_Debug) cout << "Distance to plane : " << distanceToPlane << endl;

            Double_t z = zPosition + fieldZDirection * distanceToPlane;

            Double_t energy = sgnl->GetMaxPeakValue();

            if (GetVerboseLevel() >= REST_Debug)
                cout << "Adding hit. Time : " << time << " x : " << x << " y : " << y << " z : " << z
                     << " Energy : " << energy << endl;

            fHitsEvent->AddHit(x, y, z, energy, 0, type);
        } else if (fSignalToHitMethod == "tripleMax") {
            Int_t bin = sgnl->GetMaxIndex();
            int binprev = (bin - 1) < 0 ? bin : bin - 1;
            int binnext = (bin + 1) > sgnl->GetNumberOfPoints() - 1 ? bin : bin + 1;

            Double_t time = sgnl->GetTime(bin);
            Double_t energy = sgnl->GetData(bin);

            Double_t distanceToPlane = time * fDriftVelocity;
            Double_t z = zPosition + fieldZDirection * distanceToPlane;

            fHitsEvent->AddHit(x, y, z, energy, 0, type);

            time = sgnl->GetTime(binprev);
            energy = sgnl->GetData(binprev);

            distanceToPlane = time * fDriftVelocity;
            z = zPosition + fieldZDirection * distanceToPlane;

            fHitsEvent->AddHit(x, y, z, energy, 0, type);

            time = sgnl->GetTime(binnext);
            energy = sgnl->GetData(binnext);

            distanceToPlane = time * fDriftVelocity;
            z = zPosition + fieldZDirection * distanceToPlane;

            fHitsEvent->AddHit(x, y, z, energy, 0, type);

            if (GetVerboseLevel() >= REST_Debug) {
                cout << "Distance to plane : " << distanceToPlane << endl;
                cout << "Adding hit. Time : " << time << " x : " << x << " y : " << y << " z : " << z
                     << " Energy : " << energy << endl;
            }
        } else {
            for (int j = 0; j < sgnl->GetNumberOfPoints(); j++) {
                Double_t energy = sgnl->GetData(j);

                Double_t distanceToPlane = sgnl->GetTime(j) * fDriftVelocity;

                if (GetVerboseLevel() >= REST_Debug) {
                    cout << "Time : " << sgnl->GetTime(j) << " Drift velocity : " << fDriftVelocity << endl;
                    cout << "Distance to plane : " << distanceToPlane << endl;
                }

                Double_t z = zPosition + fieldZDirection * distanceToPlane;

                if (GetVerboseLevel() >= REST_Debug)
                    cout << "Adding hit. Time : " << sgnl->GetTime(j) << " x : " << x << " y : " << y
                         << " z : " << z << endl;

                fHitsEvent->AddHit(x, y, z, energy, 0, type);
            }
        }
    }

    if (this->GetVerboseLevel() >= REST_Info) {
        cout << "TRestSignalToHitsProcess. Hits added : " << fHitsEvent->GetNumberOfHits() << endl;
        cout << "TRestSignalToHitsProcess. Hits total energy : " << fHitsEvent->GetEnergy() << endl;
    }

    if (this->GetVerboseLevel() == REST_Debug) {
        fHitsEvent->PrintEvent(30);
    } else if (this->GetVerboseLevel() == REST_Extreme) {
        fHitsEvent->PrintEvent(-1);
    }

    if (fHitsEvent->GetNumberOfHits() <= 0) return NULL;

    return fHitsEvent;
}

//______________________________________________________________________________
void TRestSignalToHitsProcess::EndOfEventProcess() {}

//______________________________________________________________________________
void TRestSignalToHitsProcess::EndProcess() {
    // Function to be executed once at the end of the process
    // (after all events have been processed)

    // Start by calling the EndProcess function of the abstract class.
    // Comment this if you don't want it.
    // TRestEventProcess::EndProcess();
}

//______________________________________________________________________________
void TRestSignalToHitsProcess::InitFromConfigFile() {
    fElectricField = GetDblParameterWithUnits("electricField", 100.);
    fGasPressure = GetDblParameterWithUnits("gasPressure", -1.);
    fDriftVelocity = GetDblParameterWithUnits("driftVelocity", -1.);
    fSignalToHitMethod = GetParameter("method", "all");
}
