///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestDetectorSignalToHitsProcess.cxx
///
///             jan 2016:  Javier Galan
///_______________________________________________________________________________

#include "TRestDetectorSignalToHitsProcess.h"

#include <TRestDetectorSetup.h>
using namespace std;

ClassImp(TRestDetectorSignalToHitsProcess);
//______________________________________________________________________________
TRestDetectorSignalToHitsProcess::TRestDetectorSignalToHitsProcess() { Initialize(); }

//______________________________________________________________________________
TRestDetectorSignalToHitsProcess::TRestDetectorSignalToHitsProcess(char* cfgFileName) {
    Initialize();

    LoadConfig(cfgFileName);

    // TRestDetectorSignalToHitsProcess default constructor
}

//______________________________________________________________________________
TRestDetectorSignalToHitsProcess::~TRestDetectorSignalToHitsProcess() {
    delete fHitsEvent;
    // TRestDetectorSignalToHitsProcess destructor
}

void TRestDetectorSignalToHitsProcess::LoadDefaultConfig() {
    SetName("signalToHitsProcess-Default");
    SetTitle("Default config");

    cout << "Signal to hits metadata not found. Loading default values" << endl;

    fElectricField = 1000;
    fGasPressure = 10;
}

void TRestDetectorSignalToHitsProcess::LoadConfig(std::string cfgFilename, std::string name) {
    if (LoadConfigFromFile(cfgFilename, name)) LoadDefaultConfig();

    // If the parameters have no value it tries to obtain it from detector setup

    // if (fElectricField == PARAMETER_NOT_FOUND_DBL) {
    //    TRestDetectorSetup* detSetup = GetMetadata<TRestDetectorSetup>();
    //    if (detSetup != NULL) {
    // fElectricField = gDetector->GetDriftField();
    // cout << "SignalToHitsProcess : Obtainning electric field from detector "
    //        "setup : "
    //     << fElectricField << " V/cm" << endl;
    //    }
    //}

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
       "TRestDetectorElectronDiffusionProcess", "electricField" ); if( fElectricField !=
       PARAMETER_NOT_FOUND_DBL ) cout << "Getting electric field from
       electronDiffusionProcess : " << fElectricField << " V/cm" << endl;
            }

            GetChar();

            if ( fSampling == PARAMETER_NOT_FOUND_DBL )
            {
                    fSampling = this->GetDoubleParameterFromClassWithUnits(
       "TRestDetectorHitsToSignalProcess", "sampling" ); if( fSampling !=
       PARAMETER_NOT_FOUND_DBL ) cout << "Getting sampling rate from hitsToSignal
       process : " << fSampling << " um" << endl;
            }
    */
}

//______________________________________________________________________________
void TRestDetectorSignalToHitsProcess::Initialize() {
    SetSectionName(this->ClassName());

    fHitsEvent = new TRestDetectorHitsEvent();
    fSignalEvent = 0;

    fGas = NULL;
    fReadout = NULL;
}

//______________________________________________________________________________
void TRestDetectorSignalToHitsProcess::InitProcess() {
    // Function to be executed once at the beginning of process
    // (before starting the process of the events)

    // Start by calling the InitProcess function of the abstract class.
    // Comment this if you don't want it.
    // TRestEventProcess::InitProcess();

    fGas = GetMetadata<TRestDetectorGas>();
    if (fGas != NULL) {
#ifndef USE_Garfield
        ferr << "A TRestDetectorGas definition was found but REST was not linked to Garfield libraries." << endl;
        ferr << "Please, remove the TRestDetectorGas definition, and add gas parameters inside the process "
                "TRestDetectorSignalToHitsProcess"
             << endl;
        exit(-1);
#endif
        if (fGasPressure <= 0) fGasPressure = fGas->GetPressure();
        if (fElectricField <= 0) fElectricField = fGas->GetElectricField();

        fGas->SetPressure(fGasPressure);
        fGas->SetElectricField(fElectricField);

        if (fDriftVelocity <= 0) fDriftVelocity = fGas->GetDriftVelocity();
    } else {
        warning << "No TRestDetectorGas found in TRestRun." << endl;
        if (fDriftVelocity == -1) {
            ferr << "TRestDetectorHitsToSignalProcess: drift velocity is undefined in the rml file!" << endl;
            exit(-1);
        }
    }

    fReadout = GetMetadata<TRestDetectorReadout>();

    if (fReadout == NULL) {
        ferr << "Readout has not been initialized" << endl;
        exit(-1);
    }
}

//______________________________________________________________________________
TRestEvent* TRestDetectorSignalToHitsProcess::ProcessEvent(TRestEvent* evInput) {
    fSignalEvent = (TRestDetectorSignalEvent*)evInput;

    if (GetVerboseLevel() >= REST_Debug) fSignalEvent->PrintEvent();

    fHitsEvent->SetID(fSignalEvent->GetID());
    fHitsEvent->SetSubID(fSignalEvent->GetSubID());
    fHitsEvent->SetTimeStamp(fSignalEvent->GetTimeStamp());
    fHitsEvent->SetSubEventTag(fSignalEvent->GetSubEventTag());

    Int_t numberOfSignals = fSignalEvent->GetNumberOfSignals();

    Int_t planeID, readoutChannel = -1, readoutModule;
    for (int i = 0; i < numberOfSignals; i++) {
        TRestDetectorSignal* sgnl = fSignalEvent->GetSignal(i);
        Int_t signalID = sgnl->GetSignalID();

        if (GetVerboseLevel() >= REST_Debug)
            cout << "Searching readout coordinates for signal ID : " << signalID << endl;
        // for( int p = 0; p < fReadout->GetNumberOfReadoutPlanes(); p++ )
        //{
        //    TRestDetectorReadoutPlane *plane = fReadout->GetReadoutPlane( p );
        //    for( int m = 0; m < plane->GetNumberOfModules(); m++ )
        //    {
        //        TRestDetectorReadoutModule *mod = plane->GetModule( m );

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
            // cout << "REST Warning : Readout channel not found for daq ID : " << signalID << endl;
            continue;
        }
        /////////////////////////////////////////////////////////////////////////

        TRestDetectorReadoutPlane* plane = fReadout->GetReadoutPlaneWithID(planeID);

        // For the moment this will only be valid for a TPC with its axis (field
        // direction) being in z
        Double_t fieldZDirection = plane->GetPlaneVector().Z();
        Double_t zPosition = plane->GetPosition().Z();

        Double_t x = plane->GetX(readoutModule, readoutChannel);
        Double_t y = plane->GetY(readoutModule, readoutChannel);

        REST_HitType type = XYZ;
        TRestDetectorReadoutModule* mod = plane->GetModuleByID(readoutModule);
        if (TMath::IsNaN(x)) {
            x = mod->GetPhysicalCoordinates(TVector2(mod->GetModuleSizeX() / 2, mod->GetModuleSizeY() / 2))
                    .X();
            type = YZ;
        } else if (TMath::IsNaN(y)) {
            y = mod->GetPhysicalCoordinates(TVector2(mod->GetModuleSizeX() / 2, mod->GetModuleSizeY() / 2))
                    .Y();
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

    if (this->GetVerboseLevel() >= REST_Debug) {
        debug << "TRestDetectorSignalToHitsProcess. Hits added : " << fHitsEvent->GetNumberOfHits() << endl;
        debug << "TRestDetectorSignalToHitsProcess. Hits total energy : " << fHitsEvent->GetEnergy() << endl;
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
void TRestDetectorSignalToHitsProcess::EndProcess() {
    // Function to be executed once at the end of the process
    // (after all events have been processed)

    // Start by calling the EndProcess function of the abstract class.
    // Comment this if you don't want it.
    // TRestEventProcess::EndProcess();
}

//______________________________________________________________________________
void TRestDetectorSignalToHitsProcess::InitFromConfigFile() {
    fElectricField = GetDblParameterWithUnits("electricField", 100.);
    fGasPressure = GetDblParameterWithUnits("gasPressure", -1.);
    fDriftVelocity = GetDblParameterWithUnits("driftVelocity", -1.);
    fSignalToHitMethod = GetParameter("method", "all");
}
