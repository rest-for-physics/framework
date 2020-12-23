///______________________________________________________________________________
///______________________________________________________________________________
///
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestDetectorFiducializationProcess.cxx
///
///
///             First implementation of electron diffusion process into REST_v2
///             Date : Mar/2017
///             Author : J. Galan
///
///_______________________________________________________________________________

#include "TRestDetectorFiducializationProcess.h"
using namespace std;

ClassImp(TRestDetectorFiducializationProcess)
    //______________________________________________________________________________
    TRestDetectorFiducializationProcess::TRestDetectorFiducializationProcess() {
    Initialize();
}

//______________________________________________________________________________
TRestDetectorFiducializationProcess::TRestDetectorFiducializationProcess(char* cfgFileName) {
    Initialize();

    if (LoadConfigFromFile(cfgFileName)) LoadDefaultConfig();
}

//______________________________________________________________________________
TRestDetectorFiducializationProcess::~TRestDetectorFiducializationProcess() { delete fOutputHitsEvent; }

void TRestDetectorFiducializationProcess::LoadDefaultConfig() { SetTitle("Default config"); }

//______________________________________________________________________________
void TRestDetectorFiducializationProcess::Initialize() {
    SetSectionName(this->ClassName());

    fOutputHitsEvent = new TRestDetectorHitsEvent();
    fInputHitsEvent = NULL;

    fReadout = NULL;
}

void TRestDetectorFiducializationProcess::LoadConfig(string cfgFilename, string name) {
    if (LoadConfigFromFile(cfgFilename, name)) LoadDefaultConfig();
}

//______________________________________________________________________________
void TRestDetectorFiducializationProcess::InitProcess() {
    fReadout = GetMetadata<TRestDetectorReadout>();
    if (fReadout == NULL) {
        cout << "REST ERRORRRR : Readout has not been initialized" << endl;
        exit(-1);
    }
}

//______________________________________________________________________________
TRestEvent* TRestDetectorFiducializationProcess::ProcessEvent(TRestEvent* evInput) {
    fInputHitsEvent = (TRestDetectorHitsEvent*)evInput;

    Int_t nHits = fInputHitsEvent->GetNumberOfHits();
    if (nHits <= 0) return NULL;

    TRestHits* hits = fInputHitsEvent->GetHits();
    for (int n = 0; n < nHits; n++) {
        Double_t eDep = hits->GetEnergy(n);

        const Double_t x = hits->GetX(n);
        const Double_t y = hits->GetY(n);
        const Double_t z = hits->GetZ(n);

        const Double_t t = hits->GetTime(n);
        auto type = hits->GetType(n);

        for (int p = 0; p < fReadout->GetNumberOfReadoutPlanes(); p++) {
            TRestDetectorReadoutPlane* plane = fReadout->GetReadoutPlane(p);

            if (plane->GetModuleIDFromPosition(TVector3(x, y, z)) >= 0)
                fOutputHitsEvent->AddHit(x, y, z, eDep, t, type);
        }
    }

    if (fOutputHitsEvent->GetNumberOfHits() == 0) return NULL;

    if (this->GetVerboseLevel() >= REST_Debug) {
        cout << "TRestDetectorFiducializationProcess. Hits added : " << fOutputHitsEvent->GetNumberOfHits() << endl;
        cout << "TRestDetectorFiducializationProcess. Hits total energy : " << fOutputHitsEvent->GetEnergy() << endl;
    }

    return fOutputHitsEvent;
}

//______________________________________________________________________________
void TRestDetectorFiducializationProcess::EndProcess() {
    // Function to be executed once at the end of the process
    // (after all events have been processed)

    // Start by calling the EndProcess function of the abstract class.
    // Comment this if you don't want it.
    // TRestEventProcess::EndProcess();
}

//______________________________________________________________________________
void TRestDetectorFiducializationProcess::InitFromConfigFile() {}
