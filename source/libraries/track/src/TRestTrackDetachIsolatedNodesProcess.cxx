//////////////////////////////////////////////////////////////////////////
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestTrackDetachIsolatedNodesProcess.cxx
///
///             May 2018:   First concept (Javier Galan)
///
//////////////////////////////////////////////////////////////////////////

#include "TRestTrackDetachIsolatedNodesProcess.h"
using namespace std;

const int Nmax = 30;

ClassImp(TRestTrackDetachIsolatedNodesProcess)
    //______________________________________________________________________________
    TRestTrackDetachIsolatedNodesProcess::TRestTrackDetachIsolatedNodesProcess() {
    Initialize();
}

//______________________________________________________________________________
TRestTrackDetachIsolatedNodesProcess::TRestTrackDetachIsolatedNodesProcess(char* cfgFileName) {
    Initialize();

    if (LoadConfigFromFile(cfgFileName) == -1) LoadDefaultConfig();
    PrintMetadata();
}

//______________________________________________________________________________
TRestTrackDetachIsolatedNodesProcess::~TRestTrackDetachIsolatedNodesProcess() { delete fOutputTrackEvent; }

void TRestTrackDetachIsolatedNodesProcess::LoadDefaultConfig() {
    SetName("trackDetachIsolatedNodesProcess");
    SetTitle("Default config");
}

//______________________________________________________________________________
void TRestTrackDetachIsolatedNodesProcess::Initialize() {
    SetSectionName(this->ClassName());

    fInputTrackEvent = NULL;
    fOutputTrackEvent = new TRestTrackEvent();
}

void TRestTrackDetachIsolatedNodesProcess::LoadConfig(std::string cfgFilename, std::string name) {
    if (LoadConfigFromFile(cfgFilename, name) == -1) LoadDefaultConfig();

    PrintMetadata();
}

//______________________________________________________________________________
void TRestTrackDetachIsolatedNodesProcess::InitProcess() {}

//______________________________________________________________________________
TRestEvent* TRestTrackDetachIsolatedNodesProcess::ProcessEvent(TRestEvent* evInput) {
    fInputTrackEvent = (TRestTrackEvent*)evInput;

    if (this->GetVerboseLevel() >= REST_Debug)
        cout << "TRestTrackDetachIsolatedNodesProcess. Number of tracks : "
             << fInputTrackEvent->GetNumberOfTracks() << endl;

    if (GetVerboseLevel() >= REST_Debug) fInputTrackEvent->PrintEvent();

    // Copying the input tracks to the output track
    for (int tck = 0; tck < fInputTrackEvent->GetNumberOfTracks(); tck++)
        fOutputTrackEvent->AddTrack(fInputTrackEvent->GetTrack(tck));

    for (int tck = 0; tck < fInputTrackEvent->GetNumberOfTracks(); tck++) {
        if (!fInputTrackEvent->isTopLevel(tck)) continue;
        Int_t tckId = fInputTrackEvent->GetTrack(tck)->GetTrackID();

        TRestVolumeHits* hits = fInputTrackEvent->GetTrack(tck)->GetVolumeHits();
        TRestVolumeHits* originHits = fInputTrackEvent->GetOriginTrackById(tckId)->GetVolumeHits();

        Int_t nHits = hits->GetNumberOfHits();

        /* {{{ Debug output */

        if (this->GetVerboseLevel() >= REST_Debug) {
            Int_t pId = fInputTrackEvent->GetTrack(tck)->GetParentID();
            cout << "Track : " << tck << " TrackID : " << tckId << " ParentID : " << pId << endl;
            cout << "-----------------" << endl;
            hits->PrintHits();
            cout << "-----------------" << endl;
            GetChar();
        }
        /* }}} */

        TRestVolumeHits connectedHits;
        TRestVolumeHits isolatedHit;

        connectedHits.AddHit(hits->GetPosition(0), hits->GetEnergy(0), 0, hits->GetType(0),
                             hits->GetSigma(0));

        for (int n = 1; n < nHits - 1; n++) {
            TVector3 x0, x1, pos0, pos1;

            Double_t hitConnectivity = 0;

            x0 = hits->GetPosition(n);

            Double_t distance = 0;
            for (int m = n - 1; m < n + 1; m++) {
                if (n == m) m++;

                x1 = hits->GetPosition(m);

                pos0 = fTubeLengthReduction * (x1 - x0) + x0;
                pos1 = (1 - fTubeLengthReduction) * (x1 - x0) + x0;

                distance += (x0 - x1).Mag();
                hitConnectivity += originHits->GetEnergyInCylinder(pos0, pos1, fTubeRadius);
            }

            if (GetVerboseLevel() >= REST_Debug)
                cout << "Hit : " << n << " Connectivity : " << hitConnectivity
                     << " distance : " << distance / 2. << endl;

            if (hitConnectivity <= fConnectivityThreshold && distance / 2 > fThresholdDistance) {
                isolatedHit.AddHit(hits->GetPosition(n), hits->GetEnergy(n), 0, hits->GetType(n),
                                   hits->GetSigma(n));

                TRestTrack isoTrack;
                isoTrack.SetTrackID(fOutputTrackEvent->GetNumberOfTracks() + 1);

                isoTrack.SetParentID(tckId);

                isoTrack.SetVolumeHits(isolatedHit);

                fOutputTrackEvent->AddTrack(&isoTrack);

                isolatedHit.RemoveHits();
            } else {
                connectedHits.AddHit(hits->GetPosition(n), hits->GetEnergy(n), 0, hits->GetType(n),
                                     hits->GetSigma(n));
            }
        }

        connectedHits.AddHit(hits->GetPosition(nHits - 1), hits->GetEnergy(nHits - 1), 0,
                             hits->GetType(nHits - 1), hits->GetSigma(nHits - 1));

        TRestTrack connectedTrack;
        connectedTrack.SetTrackID(fOutputTrackEvent->GetNumberOfTracks() + 1);

        connectedTrack.SetParentID(tckId);

        connectedTrack.SetVolumeHits(connectedHits);

        fOutputTrackEvent->AddTrack(&connectedTrack);
    }

    if (GetVerboseLevel() >= REST_Debug) {
        cout << "xxxx DetachIsolatedNodes trackEvent output xxxxx" << endl;
        fOutputTrackEvent->PrintEvent();
        cout << "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx" << endl;
        GetChar();
    }

    return fOutputTrackEvent;
}

//______________________________________________________________________________
void TRestTrackDetachIsolatedNodesProcess::EndProcess() {}

//______________________________________________________________________________
void TRestTrackDetachIsolatedNodesProcess::InitFromConfigFile() {
    fThresholdDistance = StringToDouble(GetParameter("thDistance", "8"));
    fConnectivityThreshold = StringToDouble(GetParameter("connectivityThreshold", "0"));

    fTubeLengthReduction = StringToDouble(GetParameter("tubeLength", "0.2"));
    fTubeRadius = StringToDouble(GetParameter("tubeRadius", "0.2"));
}
