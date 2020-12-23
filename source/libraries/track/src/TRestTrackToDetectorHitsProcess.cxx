//////////////////////////////////////////////////////////////////////////
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestTrackToDetectorHitsProcess.cxx
///
///             Apr 2017:   First concept (Javier Galan)
///
//////////////////////////////////////////////////////////////////////////

#include "TRestTrackToDetectorHitsProcess.h"
using namespace std;

ClassImp(TRestTrackToDetectorHitsProcess);
//______________________________________________________________________________
TRestTrackToDetectorHitsProcess::TRestTrackToDetectorHitsProcess() { Initialize(); }

//______________________________________________________________________________
TRestTrackToDetectorHitsProcess::TRestTrackToDetectorHitsProcess(char* cfgFileName) {
    Initialize();

    if (LoadConfigFromFile(cfgFileName) == -1) LoadDefaultConfig();
}

//______________________________________________________________________________
TRestTrackToDetectorHitsProcess::~TRestTrackToDetectorHitsProcess() { delete fOutputHitsEvent; }

void TRestTrackToDetectorHitsProcess::LoadDefaultConfig() {
    SetName("trackToDetectorHitsProcess");
    SetTitle("Default config");

    fTrackLevel = 0;
}

//______________________________________________________________________________
void TRestTrackToDetectorHitsProcess::Initialize() {
    SetSectionName(this->ClassName());

    fInputTrackEvent = NULL;
    fOutputHitsEvent = new TRestDetectorHitsEvent();
}

void TRestTrackToDetectorHitsProcess::LoadConfig(std::string cfgFilename, std::string name) {
    if (LoadConfigFromFile(cfgFilename, name) == -1) LoadDefaultConfig();
}

//______________________________________________________________________________
void TRestTrackToDetectorHitsProcess::InitProcess() {}

//______________________________________________________________________________
TRestEvent* TRestTrackToDetectorHitsProcess::ProcessEvent(TRestEvent* evInput) {
    fInputTrackEvent = (TRestTrackEvent*)evInput;

    if (this->GetVerboseLevel() >= REST_Debug) fInputTrackEvent->PrintOnlyTracks();

    for (int n = 0; n < fInputTrackEvent->GetNumberOfTracks(); n++)
        if (fInputTrackEvent->GetLevel(n) == fTrackLevel) {
            TRestHits* hits = fInputTrackEvent->GetTrack(n)->GetHits();

            for (int h = 0; h < hits->GetNumberOfHits(); h++)
                fOutputHitsEvent->AddHit(hits->GetX(h), hits->GetY(h), hits->GetZ(h), hits->GetEnergy(h),
                                         hits->GetTime(h), hits->GetType(h));
        }

    return fOutputHitsEvent;
}

//______________________________________________________________________________
void TRestTrackToDetectorHitsProcess::EndProcess() {}

//______________________________________________________________________________
void TRestTrackToDetectorHitsProcess::InitFromConfigFile() {
    fTrackLevel = StringToInteger(GetParameter("trackLevel", "1"));
}
