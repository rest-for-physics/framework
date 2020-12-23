///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestDetectorHitsToTrackFastProcess.cxx
///
///             Feb 2016:   First concept (Javier Galan)
//
///_______________________________________________________________________________

#include "TRestDetectorHitsToTrackFastProcess.h"
using namespace std;

#include <TRestMesh.h>

ClassImp(TRestDetectorHitsToTrackFastProcess);
//______________________________________________________________________________
TRestDetectorHitsToTrackFastProcess::TRestDetectorHitsToTrackFastProcess() { Initialize(); }

//______________________________________________________________________________
TRestDetectorHitsToTrackFastProcess::TRestDetectorHitsToTrackFastProcess(char* cfgFileName) {
    Initialize();

    if (LoadConfigFromFile(cfgFileName) == -1) LoadDefaultConfig();

    // TRestDetectorHitsToTrackFastProcess default constructor
}

//______________________________________________________________________________
TRestDetectorHitsToTrackFastProcess::~TRestDetectorHitsToTrackFastProcess() {
    delete fTrackEvent;
    // TRestDetectorHitsToTrackFastProcess destructor
}

void TRestDetectorHitsToTrackFastProcess::LoadDefaultConfig() {
    SetName("fastHitsToTrackProcess");
    SetTitle("Default config");

    fCellResolution = 10.;
    fNetSize = 1000.;
    fNetOrigin = TVector3(-500, -500, -500);
    fNodes = (Int_t)(fNetSize / fCellResolution);
}

//______________________________________________________________________________
void TRestDetectorHitsToTrackFastProcess::Initialize() {
    SetSectionName(this->ClassName());

    fCellResolution = 10.;
    fNetSize = 1000.;
    fNetOrigin = TVector3(-500, -500, -500);
    fNodes = (Int_t)(fNetSize / fCellResolution);

    fHitsEvent = NULL;
    fTrackEvent = new TRestTrackEvent();
}

void TRestDetectorHitsToTrackFastProcess::LoadConfig(std::string cfgFilename, std::string name) {
    if (LoadConfigFromFile(cfgFilename, name) == -1) LoadDefaultConfig();
}

//______________________________________________________________________________
void TRestDetectorHitsToTrackFastProcess::InitProcess() {
    // Function to be executed once at the beginning of process
    // (before starting the process of the events)

    // Start by calling the InitProcess function of the abstract class.
    // Comment this if you don't want it.
    // TRestEventProcess::InitProcess();
}

//______________________________________________________________________________
TRestEvent* TRestDetectorHitsToTrackFastProcess::ProcessEvent(TRestEvent* evInput) {
    /* Time measurement
    high_resolution_clock::time_point t1 = high_resolution_clock::now();
    */
    fHitsEvent = (TRestDetectorHitsEvent*)evInput;

    fTrackEvent->SetID(fHitsEvent->GetID());
    fTrackEvent->SetSubID(fHitsEvent->GetSubID());
    fTrackEvent->SetTimeStamp(fHitsEvent->GetTimeStamp());
    fTrackEvent->SetSubEventTag(fHitsEvent->GetSubEventTag());

    /* Debug output
    cout << "----------------------" << endl;
    cout << "Event ID : " << fHitsEvent->GetID() << endl;
    cout << "Number of hits : " << fHitsEvent->GetNumberOfHits() << endl;
    */

    /* Debugging output
    fHitsEvent->PrintEvent();
    getchar();
    */

    TRestHits* xzHits = fHitsEvent->GetXZHits();
    // cout << "Number of xzHits : " <<  xzHits->GetNumberOfHits() << endl;
    Int_t xTracks = FindTracks(xzHits);

    fTrackEvent->SetNumberOfXTracks(xTracks);

    TRestHits* yzHits = fHitsEvent->GetYZHits();
    // cout << "Number of yzHits : " <<  yzHits->GetNumberOfHits() << endl;
    Int_t yTracks = FindTracks(yzHits);

    fTrackEvent->SetNumberOfYTracks(yTracks);

    TRestHits* xyzHits = fHitsEvent->GetXYZHits();
    // cout << "Number of xyzHits : " <<  xyzHits->GetNumberOfHits() << endl;

    FindTracks(xyzHits);

    /*
    cout << "X tracks : " << xTracks << "  Y tracks : " << yTracks << endl;
    cout << "Total number of tracks : " << fTrackEvent->GetNumberOfTracks() <<
    endl;
    */

    /* Time measurement
    high_resolution_clock::time_point t2 = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>( t2 - t1 ).count();
    cout << duration << " us" << endl;
    getchar();
    */

    if (fTrackEvent->GetNumberOfTracks() == 0) return NULL;

    fTrackEvent->SetLevels();

    return fTrackEvent;
}

Int_t TRestDetectorHitsToTrackFastProcess::FindTracks(TRestHits* hits) {
    TRestMesh* mesh = new TRestMesh(fNetSize, fNodes);
    mesh->SetOrigin(fNetOrigin);

    mesh->SetNodesFromHits(hits);

    Int_t nTracksFound = mesh->GetNumberOfGroups();

    TRestTrack track[nTracksFound];
    TRestVolumeHits volHit[nTracksFound];

    double nan = numeric_limits<double>::quiet_NaN();
    for (int h = 0; h < hits->GetNumberOfHits(); h++) {
        Double_t x = hits->GetX(h);
        Double_t y = hits->GetY(h);
        Double_t z = hits->GetZ(h);
        Double_t time = hits->GetTime(h);
        REST_HitType type = hits->GetType(h);
        Double_t en = hits->GetEnergy(h);

        TVector3 pos(x, y, z);
        TVector3 sigma(0, 0, 0);

        Int_t gId = mesh->GetGroupId(type == YZ ? nan : hits->GetX(h), type == XZ ? nan : hits->GetY(h),
                                     hits->GetZ(h));
        volHit[gId].AddHit(pos, en, time, type, sigma);
    }

    for (int tckID = 0; tckID < nTracksFound; tckID++) {
        track[tckID].SetParentID(0);
        track[tckID].SetTrackID(fTrackEvent->GetNumberOfTracks() + 1);
        track[tckID].SetVolumeHits(volHit[tckID]);
        fTrackEvent->AddTrack(&track[tckID]);
    }

    delete mesh;

    return nTracksFound;
}

//______________________________________________________________________________
void TRestDetectorHitsToTrackFastProcess::EndProcess() {
    // Function to be executed once at the end of the process
    // (after all events have been processed)

    // Start by calling the EndProcess function of the abstract class.
    // Comment this if you don't want it.
    // TRestEventProcess::EndProcess();
}

//______________________________________________________________________________
void TRestDetectorHitsToTrackFastProcess::InitFromConfigFile() {
    fCellResolution = GetDblParameterWithUnits("cellResolution");
    fNetSize = GetDblParameterWithUnits("netSize");
    fNetOrigin = Get3DVectorParameterWithUnits("netOrigin");
    fNodes = (Int_t)(fNetSize / fCellResolution);
}
