///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestDetectorHitsEventViewer.cxx
///
///             nov 2015:   First concept
///                 Viewer class for a TRestHitEvent
///                 JuanAn Garcia
///_______________________________________________________________________________

#include "TRestDetectorHitsEventViewer.h"
using namespace std;
using namespace TMath;

ClassImp(TRestDetectorHitsEventViewer)
    //______________________________________________________________________________
    TRestDetectorHitsEventViewer::TRestDetectorHitsEventViewer() {
    Initialize();
}

//______________________________________________________________________________
TRestDetectorHitsEventViewer::~TRestDetectorHitsEventViewer() {
    // TRestDetectorHitsEventViewer destructor
}

//______________________________________________________________________________
void TRestDetectorHitsEventViewer::Initialize() {
    fHitsEvent = new TRestDetectorHitsEvent();
    fEvent = fHitsEvent;
}

void TRestDetectorHitsEventViewer::DeleteCurrentEvent() {
    TRestEveEventViewer::DeleteCurrentEvent();
    cout << "Removing event" << endl;
}

void TRestDetectorHitsEventViewer::AddEvent(TRestEvent* ev) {
    DeleteCurrentEvent();

    fHitsEvent = (TRestDetectorHitsEvent*)ev;

    Double_t eDepMin = 1.e6;
    Double_t eDepMax = 0;
    Double_t totalEDep = 0;

    for (int hit = 0; hit < fHitsEvent->GetNumberOfHits(); hit++) {
        Double_t eDep = fHitsEvent->GetEnergy(hit);
        if (eDep > eDepMax) eDepMax = eDep;
        if (eDep < eDepMin) eDepMin = eDep;
        totalEDep += eDep;
    }

    Double_t slope;
    if (eDepMin == eDepMin)
        slope = 0;
    else
        slope = (fMaxRadius - fMinRadius) / (eDepMax - eDepMin);

    Double_t bias = fMinRadius - slope * eDepMin;

    for (int hit = 0; hit < fHitsEvent->GetNumberOfHits(); hit++) {
        Float_t x = fHitsEvent->GetX(hit);
        Float_t y = fHitsEvent->GetY(hit);
        Float_t energy = fHitsEvent->GetEnergy(hit);
        Float_t z = fHitsEvent->GetZ(hit);

        Float_t radius = slope * energy + bias;

        if (IsNaN(x)) x = 0;
        if (IsNaN(y)) y = 0;
        if (IsNaN(z)) z = 0;

        AddSphericalHit(x, y, z, radius, energy);
    }

    Update();
}
