//////////////////////////////////////////////////////////////////////////
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestDetectorHitsShuffleProcess.cxx
///
///             Jan 2016:   First concept (Javier Galan)
///
//////////////////////////////////////////////////////////////////////////

#include "TRestDetectorHitsShuffleProcess.h"
using namespace std;

ClassImp(TRestDetectorHitsShuffleProcess)
    //______________________________________________________________________________
    TRestDetectorHitsShuffleProcess::TRestDetectorHitsShuffleProcess() {
    Initialize();
}

//______________________________________________________________________________
TRestDetectorHitsShuffleProcess::TRestDetectorHitsShuffleProcess(char* cfgFileName) {
    Initialize();

    if (LoadConfigFromFile(cfgFileName) == -1) LoadDefaultConfig();
}

//______________________________________________________________________________
TRestDetectorHitsShuffleProcess::~TRestDetectorHitsShuffleProcess() {
    // delete fHitsEvent;
}

void TRestDetectorHitsShuffleProcess::LoadDefaultConfig() {
    SetName("hitsShuffleProcess");
    SetTitle("Default config");

    fIterations = 100;
}

//______________________________________________________________________________
void TRestDetectorHitsShuffleProcess::Initialize() {
    SetSectionName(this->ClassName());

    fHitsEvent = NULL;

    fRandom = NULL;
}

void TRestDetectorHitsShuffleProcess::LoadConfig(std::string cfgFilename, std::string name) {
    if (LoadConfigFromFile(cfgFilename, name) == -1) LoadDefaultConfig();
}

//______________________________________________________________________________
void TRestDetectorHitsShuffleProcess::InitProcess() {}

//______________________________________________________________________________
TRestEvent* TRestDetectorHitsShuffleProcess::ProcessEvent(TRestEvent* evInput) {
    fHitsEvent = (TRestDetectorHitsEvent*)evInput;

    TRestHits* hits = fHitsEvent->GetHits();

    Int_t nHits = hits->GetNumberOfHits();
    if (nHits >= 2) {
        for (int n = 0; n < fIterations; n++) {
            Int_t hit1 = (Int_t)(nHits * fRandom->Uniform(0, 1));
            Int_t hit2 = (Int_t)(nHits * fRandom->Uniform(0, 1));

            hits->SwapHits(hit1, hit2);
        }
    }
    return fHitsEvent;
}

//______________________________________________________________________________
void TRestDetectorHitsShuffleProcess::EndProcess() {}

//______________________________________________________________________________
void TRestDetectorHitsShuffleProcess::InitFromConfigFile() {
    fIterations = StringToInteger(GetParameter("iterations"));
    fRandom = new TRandom3(StringToDouble(GetParameter("seed", "0")));
}
