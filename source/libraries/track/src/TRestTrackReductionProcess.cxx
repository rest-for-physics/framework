//////////////////////////////////////////////////////////////////////////
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestTrackReductionProcess.cxx
///
///             Jan 2016:   First concept (Javier Galan)
///
//////////////////////////////////////////////////////////////////////////

#include "TRestTrackReductionProcess.h"
using namespace std;

ClassImp(TRestTrackReductionProcess)
    //______________________________________________________________________________
    TRestTrackReductionProcess::TRestTrackReductionProcess() {
    Initialize();
}

//______________________________________________________________________________
TRestTrackReductionProcess::TRestTrackReductionProcess(char* cfgFileName) {
    Initialize();

    if (LoadConfigFromFile(cfgFileName) == -1) LoadDefaultConfig();
}

//______________________________________________________________________________
TRestTrackReductionProcess::~TRestTrackReductionProcess() { delete fOutputTrackEvent; }

void TRestTrackReductionProcess::LoadDefaultConfig() {
    SetName("trackReductionProcess");
    SetTitle("Default config");

    fStartingDistance = 0.5;
    fMinimumDistance = 3;
    fDistanceFactor = 1.5;
    fMaxNodes = 30;
}

//______________________________________________________________________________
void TRestTrackReductionProcess::Initialize() {
    SetSectionName(this->ClassName());

    fInputTrackEvent = NULL;
    fOutputTrackEvent = new TRestTrackEvent();
}

void TRestTrackReductionProcess::LoadConfig(std::string cfgFilename, std::string name) {
    if (LoadConfigFromFile(cfgFilename, name) == -1) LoadDefaultConfig();
}

//______________________________________________________________________________
void TRestTrackReductionProcess::InitProcess() {}

//______________________________________________________________________________
TRestEvent* TRestTrackReductionProcess::ProcessEvent(TRestEvent* evInput) {
    fInputTrackEvent = (TRestTrackEvent*)evInput;
    fOutputTrackEvent->SetEventInfo(fInputTrackEvent);
    // Copying the input tracks to the output track
    for (int tck = 0; tck < fInputTrackEvent->GetNumberOfTracks(); tck++)
        fOutputTrackEvent->AddTrack(fInputTrackEvent->GetTrack(tck));

    if (this->GetVerboseLevel() >= REST_Debug) fInputTrackEvent->PrintOnlyTracks();

    // Reducing the hits inside each track
    for (int tck = 0; tck < fInputTrackEvent->GetNumberOfTracks(); tck++) {
        if (!fInputTrackEvent->isTopLevel(tck)) continue;

        TRestTrack* track = fInputTrackEvent->GetTrack(tck);
        TRestVolumeHits* hits = track->GetVolumeHits();

        if (this->GetVerboseLevel() >= REST_Debug)
            cout << "TRestTrackReductionProcess. Reducing hits in track id : " << track->GetTrackID() << endl;

        Double_t distance = fStartingDistance;
        while (distance < fMinimumDistance || hits->GetNumberOfHits() > fMaxNodes) {
            if (this->GetVerboseLevel() >= REST_Debug) {
                cout << "TRestTrackReductionProcess. Merging track hits within a "
                        "distance : "
                     << distance << " mm" << endl;
                cout << "TRestTrackReductionProcess. Hits now : " << hits->GetNumberOfHits() << endl;
            }

            Int_t mergedHits = 0;

            Bool_t merged = true;
            while (merged) {
                merged = false;
                for (int i = 0; i < hits->GetNumberOfHits(); i++) {
                    for (int j = i + 1; j < hits->GetNumberOfHits(); j++) {
                        if (hits->GetDistance2(i, j) < distance * distance) {
                            mergedHits++;
                            hits->MergeHits(i, j);
                            merged = true;
                        }
                    }
                }
            }

            if (this->GetVerboseLevel() >= REST_Debug) {
                cout << "TRestTrackReductionProcess. Number of hits merged : " << mergedHits << endl;
            }

            mergedHits = 0;

            distance *= fDistanceFactor;
        }

        track->SetParentID(track->GetTrackID());
        track->SetTrackID(fOutputTrackEvent->GetNumberOfTracks() + 1);

        fOutputTrackEvent->AddTrack(track);
    }

    /*
    cout << "output event" << endl;
    cout << "+++++++++++++++++" << endl;
    fOutputTrackEvent->PrintOnlyTracks();
    cout << "+++++++++++++++++" << endl;
    getchar();

    cout << "Checking levels " << endl;
    cout << "---------------------------------------------_" << endl;
    cout << "TrackEvent pointer : " << fOutputTrackEvent << endl;
    cout << fOutputTrackEvent->GetNumberOfTracks() << endl;
    cout << "TrackEvent pointer : " << fOutputTrackEvent << endl;

    fOutputTrackEvent->GetLevels();
    cout << "---------------" << endl;
    */

    // cout << "Number output of tracks : " <<
    // fOutputTrackEvent->GetNumberOfTracks() << endl;

    fOutputTrackEvent->SetLevels();
    return fOutputTrackEvent;
}

//______________________________________________________________________________
void TRestTrackReductionProcess::EndProcess() {}

//______________________________________________________________________________
void TRestTrackReductionProcess::InitFromConfigFile() {
    fStartingDistance = GetDblParameterWithUnits("startingDistance");
    fMinimumDistance = GetDblParameterWithUnits("minimumDistance");
    fDistanceFactor = StringToDouble(GetParameter("distanceStepFactor"));
    fMaxNodes = StringToDouble(GetParameter("maxNodes"));
}
