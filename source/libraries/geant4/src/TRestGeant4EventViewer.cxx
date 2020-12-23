///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestGeant4EventViewer.cxx
///
///             nov 2015:   First concept
///                 Viewer class for a TRestGeant4Event
///                 Javier Galan/JuanAn Garcia
///_______________________________________________________________________________

#include "TRestGeant4EventViewer.h"
#include "TRestStringOutput.h"
using namespace std;

ClassImp(TRestGeant4EventViewer);
//______________________________________________________________________________
TRestGeant4EventViewer::TRestGeant4EventViewer() { Initialize(); }

//______________________________________________________________________________
TRestGeant4EventViewer::~TRestGeant4EventViewer() {
    // TRestGeant4EventViewer destructor
}

//______________________________________________________________________________
void TRestGeant4EventViewer::Initialize() {
    fG4Event = new TRestGeant4Event();
    fEvent = fG4Event;

    fHitConnectors.clear();
    fHitConnectors.push_back(NULL);
}

void TRestGeant4EventViewer::DeleteCurrentEvent() {
    // cout<<"Removing event"<<endl;

    TRestEveEventViewer::DeleteCurrentEvent();

    fHitConnectors.clear();
    fHitConnectors.push_back(NULL);
}

void TRestGeant4EventViewer::AddEvent(TRestEvent* ev) {
    DeleteCurrentEvent();

    fG4Event = (TRestGeant4Event*)ev;

    TRestGeant4Track* g4Track;

    Double_t eDepMin = 1.e6;
    Double_t eDepMax = 0;
    Double_t totalEDep = 0;

    for (int i = 0; i < fG4Event->GetNumberOfTracks(); i++) {
        g4Track = fG4Event->GetTrack(i);
        TRestGeant4Hits* g4Hits = g4Track->GetHits();
        Int_t nHits = g4Track->GetNumberOfHits();
        for (int j = 0; j < nHits; j++) {
            Double_t eDep = g4Hits->GetEnergy(j);
            if (eDep > eDepMax) eDepMax = eDep;
            if (eDep < eDepMin) eDepMin = eDep;

            // cout<<" track "<<i<<" hit "<<j<<" eDep "<<eDep<<endl;

            totalEDep += eDep;
        }
    }

    cout << "TRestGeant4EventViewer::AddEvent. Total EDep " << totalEDep << endl;

    Double_t slope = (fMaxRadius - fMinRadius) / (eDepMax - eDepMin);
    Double_t bias = fMinRadius - slope * eDepMin;

    Int_t textAdded = 0;
    for (int trkID = 1; trkID < fG4Event->GetNumberOfTracks() + 1; trkID++) {
        g4Track = fG4Event->GetTrackByID(trkID);

        Int_t parentID = g4Track->GetParentID();
        TVector3 origin = g4Track->GetTrackOrigin();

        // Building track name
        Double_t eKin = g4Track->GetKineticEnergy();
        // cout << "eKin: " << eKin << endl;
        TString ptlName = g4Track->GetParticleName();
        // cout << "ptlName: " << ptlName << endl;
        char pcleStr[64];
        sprintf(pcleStr, "Track ID : %d %s (%6.2lf keV)", trkID, ptlName.Data(), eKin);

        char markerStr[256];
        sprintf(markerStr, " %s (%6.2lf keV). Position (%4.1lf,%4.1lf,%4.1lf)", ptlName.Data(), eKin,
                origin.X(), origin.Y(), origin.Z());

        if (parentID == 0) {
            // cout << " Parent ID : 0" << endl;
            char evInfoStr[256];
            sprintf(evInfoStr, "%s. EventID = %d at position (%4.2lf, %4.2lf, %4.2lf) mm", ptlName.Data(),
                    fG4Event->GetID(), origin.X(), origin.Y(), origin.Z());
            this->AddParentTrack(trkID, origin, pcleStr);
            if (fG4Event->GetSubID() == 0)
                this->AddText(evInfoStr, origin);
            else if (!textAdded) {
                textAdded = 1;
                sprintf(evInfoStr, "%s. EventID = %d at position (%4.2lf, %4.2lf, %4.2lf) mm",
                        fG4Event->GetSubEventTag().Data(), fG4Event->GetID(), origin.X(), origin.Y(),
                        origin.Z());
                this->AddText(evInfoStr, origin);
            }
        } else {
            // cout << "Adding track : " << trkID << " parent : " << parentID << " pt: " << pcleStr << endl;
            this->AddTrack(trkID, parentID, origin, pcleStr);
        }

        // cout << "Adding marker" << endl;
        this->AddMarker(trkID, origin, markerStr);

        TRestGeant4Hits* g4Hits = g4Track->GetHits();
        Int_t nHits = g4Track->GetNumberOfHits();

        // cout << "Adding hits" << endl;
        for (int i = 0; i < nHits; i++) {
            Double_t x = g4Hits->GetX(i);
            Double_t y = g4Hits->GetY(i);
            Double_t z = g4Hits->GetZ(i);

            this->NextTrackVertex(trkID, TVector3(x, y, z));

            Double_t eDep = g4Hits->GetEnergy(i);

            if (eDep > 0) {
                Double_t radius = slope * eDep + bias;
                AddSphericalHit(x, y, z, radius, eDep);
            }
        }
    }
    // cout << "Updating" << endl;

    Update();
}

void TRestGeant4EventViewer::AddText(TString text, TVector3 at) {
    TEveText* evText = new TEveText(text);
    evText->SetName("Event title");
    evText->SetFontSize(12);
    evText->RefMainTrans().SetPos((at.X() + 15) * GEOM_SCALE, (at.Y() + 15) * GEOM_SCALE,
                                  (at.Z() + 15) * GEOM_SCALE);

    gEve->AddElement(evText);
}

void TRestGeant4EventViewer::AddMarker(Int_t trkID, TVector3 at, TString name) {
    TEvePointSet* marker = new TEvePointSet(1);
    marker->SetName(name);
    marker->SetMarkerColor(kMagenta);
    marker->SetMarkerStyle(3);
    marker->SetPoint(0, at.X() * GEOM_SCALE, at.Y() * GEOM_SCALE, at.Z() * GEOM_SCALE);
    marker->SetMarkerSize(0.4);
    fHitConnectors[trkID]->AddElement(marker);
}

void TRestGeant4EventViewer::NextTrackVertex(Int_t trkID, TVector3 to) {
    fHitConnectors[trkID]->SetNextPoint(to.X() * GEOM_SCALE, to.Y() * GEOM_SCALE, to.Z() * GEOM_SCALE);
}

void TRestGeant4EventViewer::AddTrack(Int_t trkID, Int_t parentID, TVector3 from, TString name) {
    TEveLine* evLine = new TEveLine();
    evLine->SetName(name);
    fHitConnectors.push_back(evLine);

    fHitConnectors[trkID]->SetMainColor(kWhite);
    fHitConnectors[trkID]->SetLineWidth(4);

    if (name.Contains("gamma")) fHitConnectors[trkID]->SetMainColor(kGreen);
    if (name.Contains("e-")) fHitConnectors[trkID]->SetMainColor(kRed);
    if (name.Contains("mu-")) fHitConnectors[trkID]->SetMainColor(kGray);
    if (name.Contains("alpha")) fHitConnectors[trkID]->SetMainColor(kYellow);
    if (name.Contains("neutron")) fHitConnectors[trkID]->SetMainColor(kBlue);

    fHitConnectors[trkID]->SetNextPoint(from.X() * GEOM_SCALE, from.Y() * GEOM_SCALE, from.Z() * GEOM_SCALE);

    if (fHitConnectors.size() > parentID)
        fHitConnectors[parentID]->AddElement(fHitConnectors[trkID]);
    else {
        warning << "Parent ID: " << parentID << " of track " << trkID << " was not found!" << endl;
        warning << "This might be solved by enabling TRestGeant4Metadata::fRegisterEmptyTracks" << endl;
    }
}

void TRestGeant4EventViewer::AddParentTrack(Int_t trkID, TVector3 from, TString name) {
    TEveLine* evLine = new TEveLine();
    evLine->SetName(name);
    fHitConnectors.push_back(evLine);

    fHitConnectors[trkID]->SetMainColor(kWhite);
    fHitConnectors[trkID]->SetLineWidth(4);
    fHitConnectors[trkID]->SetNextPoint(from.X() * GEOM_SCALE, from.Y() * GEOM_SCALE, from.Z() * GEOM_SCALE);

    gEve->AddElement(fHitConnectors[trkID]);
}
