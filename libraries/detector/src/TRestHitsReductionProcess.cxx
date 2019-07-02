///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestHitsReductionProcess.cxx
///
///             Jan 2016:   First concept (Javier Galan)
//
///_______________________________________________________________________________

#include "TRestHitsReductionProcess.h"
using namespace std;

ClassImp(TRestHitsReductionProcess)
    //______________________________________________________________________________
    TRestHitsReductionProcess::TRestHitsReductionProcess() {
    Initialize();
}

//______________________________________________________________________________
TRestHitsReductionProcess::TRestHitsReductionProcess(char* cfgFileName) {
    Initialize();

    if (LoadConfigFromFile(cfgFileName) == -1) LoadDefaultConfig();
}

//______________________________________________________________________________
TRestHitsReductionProcess::~TRestHitsReductionProcess() {
    delete fInputHitsEvent;
    delete fOutputHitsEvent;
}

void TRestHitsReductionProcess::LoadDefaultConfig() {
    SetName("hitsReductionProcess");
    SetTitle("Default config");

    fStartingDistance = 0.5;
    fMinimumDistance = 3;
    fDistanceFactor = 1.5;
    fMaxNodes = 30;
}

//______________________________________________________________________________
void TRestHitsReductionProcess::Initialize() {
    SetSectionName(this->ClassName());

    fInputHitsEvent = new TRestHitsEvent();
    fOutputHitsEvent = new TRestHitsEvent();

    fOutputEvent = fOutputHitsEvent;
    fInputEvent = fInputHitsEvent;
}

void TRestHitsReductionProcess::LoadConfig(std::string cfgFilename, std::string name) {
    if (LoadConfigFromFile(cfgFilename, name) == -1) LoadDefaultConfig();
}

//______________________________________________________________________________
void TRestHitsReductionProcess::InitProcess() { cout << __PRETTY_FUNCTION__ << endl; }

//______________________________________________________________________________
void TRestHitsReductionProcess::BeginOfEventProcess() { fOutputHitsEvent->Initialize(); }

//______________________________________________________________________________
TRestEvent* TRestHitsReductionProcess::ProcessEvent(TRestEvent* evInput) {
    fInputHitsEvent = (TRestHitsEvent*)evInput;

    // Copying the input hits event to the output hits event
    for (int h = 0; h < fInputHitsEvent->GetNumberOfHits(); h++) {
        Double_t x = fInputHitsEvent->GetX(h);
        Double_t y = fInputHitsEvent->GetY(h);
        Double_t z = fInputHitsEvent->GetZ(h);
        Double_t en = fInputHitsEvent->GetEnergy(h);
        fOutputHitsEvent->AddHit(x, y, z, en);
    }

    // Reducing the hits
    TRestHits* hits = fOutputHitsEvent->GetHits();

    Double_t distance = fStartingDistance;
    while (distance < fMinimumDistance || hits->GetNumberOfHits() > fMaxNodes) {
        Bool_t merged = true;
        while (merged) {
            merged = false;
            for (int i = 0; i < hits->GetNumberOfHits(); i++) {
                for (int j = i + 1; j < hits->GetNumberOfHits(); j++) {
                    if (hits->GetDistance2(i, j) < distance * distance) {
                        hits->MergeHits(i, j);
                        merged = true;
                    }
                }
            }
        }
        distance *= fDistanceFactor;
    }

    Int_t initialHits = fInputHitsEvent->GetNumberOfHits();
    Int_t finalHits = fOutputHitsEvent->GetNumberOfHits();

    if (this->GetVerboseLevel() == REST_Debug) {
        cout << "TRestHitsReductionProcess : Initial number of hits : " << initialHits << endl;
        cout << "TRestHitsReductionProcess : Final number of hits : " << finalHits << endl;
    }

    /*
       cout << "output event" << endl;
       cout << "+++++++++++++++++" << endl;
       fOutputHitsEvent->PrintEvent();
       cout << "+++++++++++++++++" << endl;
       getchar();
       */

    // cout << "Number output of tracks : " <<
    // fOutputTrackEvent->GetNumberOfTracks() << endl;

    return fOutputHitsEvent;
}

//______________________________________________________________________________
void TRestHitsReductionProcess::EndOfEventProcess() {}

//______________________________________________________________________________
void TRestHitsReductionProcess::EndProcess() {}

//______________________________________________________________________________
void TRestHitsReductionProcess::InitFromConfigFile() {
    fStartingDistance = GetDblParameterWithUnits("startingDistance");
    fMinimumDistance = GetDblParameterWithUnits("minimumDistance");
    fDistanceFactor = StringToDouble(GetParameter("distanceStepFactor"));
    fMaxNodes = StringToDouble(GetParameter("maxNodes"));
}
