///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestDetectorHitsReductionProcess.cxx
///
///             Jan 2016:   First concept (Javier Galan)
//
///_______________________________________________________________________________

#include "TRestDetectorHitsReductionProcess.h"
using namespace std;

ClassImp(TRestDetectorHitsReductionProcess)
    //______________________________________________________________________________
    TRestDetectorHitsReductionProcess::TRestDetectorHitsReductionProcess() {
    Initialize();
}

//______________________________________________________________________________
TRestDetectorHitsReductionProcess::TRestDetectorHitsReductionProcess(char* cfgFileName) {
    Initialize();

    if (LoadConfigFromFile(cfgFileName) == -1) LoadDefaultConfig();
}

//______________________________________________________________________________
TRestDetectorHitsReductionProcess::~TRestDetectorHitsReductionProcess() {}

void TRestDetectorHitsReductionProcess::LoadDefaultConfig() {
    SetName("hitsReductionProcess");
    SetTitle("Default config");

    fStartingDistance = 0.5;
    fMinimumDistance = 3;
    fDistanceFactor = 1.5;
    fMaxNodes = 30;
}

//______________________________________________________________________________
void TRestDetectorHitsReductionProcess::Initialize() {
    SetSectionName(this->ClassName());

    fInputHitsEvent = NULL;
    fOutputHitsEvent = NULL;
}

void TRestDetectorHitsReductionProcess::LoadConfig(std::string cfgFilename, std::string name) {
    if (LoadConfigFromFile(cfgFilename, name) == -1) LoadDefaultConfig();
}

//______________________________________________________________________________
void TRestDetectorHitsReductionProcess::InitProcess() {}

//______________________________________________________________________________
TRestEvent* TRestDetectorHitsReductionProcess::ProcessEvent(TRestEvent* evInput) {
    fInputHitsEvent = (TRestDetectorHitsEvent*)evInput;
    fOutputHitsEvent = fInputHitsEvent;

    Int_t initialHits = fOutputHitsEvent->GetNumberOfHits();

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

    Int_t finalHits = fOutputHitsEvent->GetNumberOfHits();

    if (this->GetVerboseLevel() == REST_Debug) {
        cout << "TRestDetectorHitsReductionProcess : Initial number of hits : " << initialHits << endl;
        cout << "TRestDetectorHitsReductionProcess : Final number of hits : " << finalHits << endl;
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
void TRestDetectorHitsReductionProcess::EndProcess() {}

//______________________________________________________________________________
void TRestDetectorHitsReductionProcess::InitFromConfigFile() {
    fStartingDistance = GetDblParameterWithUnits("startingDistance");
    fMinimumDistance = GetDblParameterWithUnits("minimumDistance");
    fDistanceFactor = StringToDouble(GetParameter("distanceStepFactor"));
    fMaxNodes = StringToDouble(GetParameter("maxNodes"));
}
