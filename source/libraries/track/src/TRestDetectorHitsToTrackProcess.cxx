///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestDetectorHitsToTrackProcess.cxx
///
///             Dec 2015:   First concept (Javier Gracia Garza)
//
//              History :
//              Jan 2016: Readapted to obtain tracks in bi-dimensional hits
//              (Javier Galan)
///_______________________________________________________________________________

#include "TRestDetectorHitsToTrackProcess.h"
using namespace std;

ClassImp(TRestDetectorHitsToTrackProcess);
//______________________________________________________________________________
TRestDetectorHitsToTrackProcess::TRestDetectorHitsToTrackProcess() { Initialize(); }

//______________________________________________________________________________
TRestDetectorHitsToTrackProcess::TRestDetectorHitsToTrackProcess(char* cfgFileName) {
    Initialize();

    if (LoadConfigFromFile(cfgFileName) == -1) LoadDefaultConfig();

    // TRestDetectorHitsToTrackProcess default constructor
}

//______________________________________________________________________________
TRestDetectorHitsToTrackProcess::~TRestDetectorHitsToTrackProcess() {
    delete fTrackEvent;
    // TRestDetectorHitsToTrackProcess destructor
}

void TRestDetectorHitsToTrackProcess::LoadDefaultConfig() {
    SetName("hitsToTrackProcess");
    SetTitle("Default config");

    fClusterDistance = 1.0;
}

//______________________________________________________________________________
void TRestDetectorHitsToTrackProcess::Initialize() {
    SetSectionName(this->ClassName());

    fClusterDistance = 1.;

    fHitsEvent = NULL;
    fTrackEvent = new TRestTrackEvent();
}

//______________________________________________________________________________
void TRestDetectorHitsToTrackProcess::LoadConfig(string cfgFilename, std::string name) {
    if (LoadConfigFromFile(cfgFilename, name) == -1) LoadDefaultConfig();
}

//______________________________________________________________________________
void TRestDetectorHitsToTrackProcess::InitProcess() {
    // Function to be executed once at the beginning of process
    // (before starting the process of the events)

    // Start by calling the InitProcess function of the abstract class.
    // Comment this if you don't want it.
    // TRestEventProcess::InitProcess();
}

//______________________________________________________________________________
TRestEvent* TRestDetectorHitsToTrackProcess::ProcessEvent(TRestEvent* evInput) {
    /* Time measurement
    high_resolution_clock::time_point t1 = high_resolution_clock::now();
    */

    fHitsEvent = (TRestDetectorHitsEvent*)evInput;
    fTrackEvent->SetEventInfo(fHitsEvent);

    if (GetVerboseLevel() >= REST_Debug)
        cout << "TResDetectorHitsToTrackProcess : nHits " << fHitsEvent->GetNumberOfHits() << endl;

    TRestHits* xzHits = fHitsEvent->GetXZHits();

    if (GetVerboseLevel() >= REST_Debug)
        cout << "TRestDetectorHitsToTrackProcess : Number of xzHits : " << xzHits->GetNumberOfHits() << endl;
    Int_t xTracks = FindTracks(xzHits);

    fTrackEvent->SetNumberOfXTracks(xTracks);

    TRestHits* yzHits = fHitsEvent->GetYZHits();
    if (GetVerboseLevel() >= REST_Debug)
        cout << "TRestDetectorHitsToTrackProcess : Number of yzHits : " << yzHits->GetNumberOfHits() << endl;
    Int_t yTracks = FindTracks(yzHits);

    fTrackEvent->SetNumberOfYTracks(yTracks);

    TRestHits* xyzHits = fHitsEvent->GetXYZHits();
    if (GetVerboseLevel() >= REST_Debug)
        cout << "TRestDetectorHitsToTrackProcess : Number of xyzHits : " << xyzHits->GetNumberOfHits()
             << endl;

    FindTracks(xyzHits);

    if (GetVerboseLevel() >= REST_Debug) {
        cout << "TRestDetectorHitsToTrackProcess. X tracks : " << xTracks << "  Y tracks : " << yTracks
             << endl;
        cout << "TRestDetectorHitsToTrackProcess. Total number of tracks : "
             << fTrackEvent->GetNumberOfTracks() << endl;
    }

    if (fTrackEvent->GetNumberOfTracks() == 0) return NULL;

    if (GetVerboseLevel() >= REST_Debug) fTrackEvent->PrintOnlyTracks();

    fTrackEvent->SetLevels();

    return fTrackEvent;
}

Int_t TRestDetectorHitsToTrackProcess::FindTracks(TRestHits* hits) {
    if (GetVerboseLevel() >= REST_Extreme) hits->PrintHits();
    Int_t nTracksFound = 0;
    vector<Int_t> Q;  // list of points (hits) that need to be checked
    vector<Int_t> P;  // list of neighbours within a radious fClusterDistance

    bool isProcessed = false;
    Int_t qsize = 0;
    TRestTrack* track = new TRestTrack();

    TRestVolumeHits volHit;

    Float_t fClusterDistance2 = (Float_t)(fClusterDistance * fClusterDistance);

    // for every event in the point cloud
    while (hits->GetNumberOfHits() > 0) {
        Q.push_back(0);

        // for every point in Q
        for (unsigned int q = 0; q < Q.size(); q++) {
            // we look for the neighbours
            for (int j = 0; j < hits->GetNumberOfHits(); j++) {
                if (j != Q[q]) {
                    if (hits->GetDistance2(Q[q], j) < fClusterDistance2) P.push_back(j);
                }
            }
            qsize = Q.size();

            // For all the neighbours found P.size()
            // Check if the points have already been processed
            for (unsigned int i = 0; i < P.size(); i++) {
                isProcessed = false;

                for (int j = 0; j < qsize; j++) {
                    // if yes, we do not consider it again
                    if (P[i] == Q[j]) {
                        isProcessed = true;
                        break;
                    }
                }

                // If not, we add the point P[i] to the list of Q
                if (isProcessed == false) {
                    Q.push_back(P[i]);
                }
            }

            P.clear();
        }

        // We order the Q vector
        std::sort(Q.begin(), Q.end());
        // Then we swap to decresing order
        std::reverse(Q.begin(), Q.end());

        // When the list of all points in Q has been processed, we add the clusters
        // to the TrackEvent and reset Q
        for (unsigned int nhit = 0; nhit < Q.size(); nhit++) {
            const Double_t x = hits->GetX(Q[nhit]);
            const Double_t y = hits->GetY(Q[nhit]);
            const Double_t z = hits->GetZ(Q[nhit]);
            const Double_t en = hits->GetEnergy(Q[nhit]);

            TVector3 pos(x, y, z);
            TVector3 sigma(0., 0., 0.);

            volHit.AddHit(pos, en, 0, hits->GetType(Q[nhit]), sigma);

            hits->RemoveHit(Q[nhit]);
        }

        track->SetParentID(0);
        track->SetTrackID(fTrackEvent->GetNumberOfTracks() + 1);
        track->SetVolumeHits(volHit);
        volHit.RemoveHits();

        //      cout << "Adding track : id=" << track->GetTrackID() << " parent : "
        //      << track->GetParentID() << endl;
        fTrackEvent->AddTrack(track);
        nTracksFound++;

        Q.clear();
    }

    delete track;

    return nTracksFound;
}

//______________________________________________________________________________
void TRestDetectorHitsToTrackProcess::EndProcess() {
    // Function to be executed once at the end of the process
    // (after all events have been processed)

    // Start by calling the EndProcess function of the abstract class.
    // Comment this if you don't want it.
    // TRestEventProcess::EndProcess();
}

//______________________________________________________________________________
void TRestDetectorHitsToTrackProcess::InitFromConfigFile() {
    fClusterDistance = GetDblParameterWithUnits("clusterDistance");
}
