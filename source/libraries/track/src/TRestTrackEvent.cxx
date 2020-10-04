///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestTrackEvent.h
///
///             Event class to store DAQ events either from simulation and
///             acquisition
///
///             oct 2015:   First concept
///                 Created as part of the conceptualization of existing REST
///                 software.
///                 Javier Gracia
///
///
///_______________________________________________________________________________

#include "TRestTrackEvent.h"

#include "TRestDetectorReadout.h"
#include "TRestRun.h"
#include "TRestTools.h"

using namespace std;

ClassImp(TRestTrackEvent);
//______________________________________________________________________________
TRestTrackEvent::TRestTrackEvent() {
    // TRestTrackEvent default constructor
    TRestEvent::Initialize();
    fTrack.clear();
    fXYHit = NULL;
    fXZHit = NULL;
    fYZHit = NULL;
    fXYZHit = NULL;
    fXYTrack = NULL;
    fXZTrack = NULL;
    fYZTrack = NULL;
    fXYZTrack = NULL;
    fPad = NULL;
    fLevels = -1;

    fPrintHitsWarning = true;
}

//______________________________________________________________________________
TRestTrackEvent::~TRestTrackEvent() {
    // TRestTrackEvent destructor
}

void TRestTrackEvent::Initialize() {
    fNtracks = 0;
    fNtracksX = 0;
    fNtracksY = 0;
    fTrack.clear();
    TRestEvent::Initialize();
}

void TRestTrackEvent::AddTrack(TRestTrack* c) {
    if (c->GetParentID() > 0) {
        TRestTrack* pTrack = GetTrackById(c->GetParentID());

        // if (pTrack->isXZ()) {
        //    TRestVolumeHits* vHits = c->GetVolumeHits();

        //    Float_t NaN = std::numeric_limits<Float_t>::quiet_NaN();

        //    vHits->InitializeYArray(NaN);
        //}

        // if (pTrack->isYZ()) {
        //    TRestVolumeHits* vHits = c->GetVolumeHits();

        //    Float_t NaN = std::numeric_limits<Float_t>::quiet_NaN();

        //    vHits->InitializeXArray(NaN);
        //}
    }

    if (c->isXZ()) fNtracksX++;
    if (c->isYZ()) fNtracksY++;
    fNtracks++;

    fTrack.push_back(*c);

    SetLevels();
}

void TRestTrackEvent::RemoveTrack(int n) {
    if (fTrack[n].isXZ()) fNtracksX--;
    if (fTrack[n].isYZ()) fNtracksY--;
    fNtracks--;

    fTrack.erase(fTrack.begin() + n);

    SetLevels();
}

Int_t TRestTrackEvent::GetNumberOfTracks(TString option) {
    if (option == "")
        return fNtracks;
    else {
        Int_t nT = 0;
        for (int n = 0; n < GetNumberOfTracks(); n++) {
            if (!this->isTopLevel(n)) continue;

            if (option == "X" && GetTrack(n)->isXZ())
                nT++;
            else if (option == "Y" && GetTrack(n)->isYZ())
                nT++;
            else if (option == "XYZ" && GetTrack(n)->isXYZ())
                nT++;
        }

        return nT;
    }

    return -1;
}

TRestTrack* TRestTrackEvent::GetTrackById(Int_t id) {
    for (int i = 0; i < GetNumberOfTracks(); i++)
        if (GetTrack(i)->GetTrackID() == id) return GetTrack(i);
    return NULL;
}

TRestTrack* TRestTrackEvent::GetMaxEnergyTrackInX() {
    Int_t track = -1;
    Double_t maxEnergy = 0;
    for (int tck = 0; tck < GetNumberOfTracks(); tck++) {
        if (!this->isTopLevel(tck)) continue;

        TRestTrack* t = GetTrack(tck);
        if (t->isXZ()) {
            if (t->GetEnergy() > maxEnergy) {
                maxEnergy = t->GetEnergy();
                track = tck;
            }
        }
    }

    if (track == -1) return NULL;

    return GetTrack(track);
}

TRestTrack* TRestTrackEvent::GetMaxEnergyTrackInY() {
    Int_t track = -1;
    Double_t maxEnergy = 0;
    for (int tck = 0; tck < GetNumberOfTracks(); tck++) {
        if (!this->isTopLevel(tck)) continue;

        TRestTrack* t = GetTrack(tck);
        if (t->isYZ()) {
            if (t->GetEnergy() > maxEnergy) {
                maxEnergy = t->GetEnergy();
                track = tck;
            }
        }
    }

    if (track == -1) return NULL;

    return GetTrack(track);
}

TRestTrack* TRestTrackEvent::GetMaxEnergyTrack(TString option) {
    if (option == "X") return GetMaxEnergyTrackInX();
    if (option == "Y") return GetMaxEnergyTrackInY();

    Int_t track = -1;
    Double_t maxEnergy = 0;
    for (int tck = 0; tck < GetNumberOfTracks(); tck++) {
        if (!this->isTopLevel(tck)) continue;

        TRestTrack* t = GetTrack(tck);
        if (t->isXYZ()) {
            if (t->GetEnergy() > maxEnergy) {
                maxEnergy = t->GetEnergy();
                track = tck;
            }
        }
    }

    if (track == -1) return NULL;

    return GetTrack(track);
}

TRestTrack* TRestTrackEvent::GetSecondMaxEnergyTrack(TString option) {
    if (GetMaxEnergyTrack(option) == NULL) return NULL;

    Int_t id = GetMaxEnergyTrack(option)->GetTrackID();

    Int_t track = -1;
    Double_t maxEnergy = 0;
    for (int tck = 0; tck < GetNumberOfTracks(); tck++) {
        if (!this->isTopLevel(tck)) continue;

        TRestTrack* t = GetTrack(tck);
        if (t->GetTrackID() == id) continue;

        Double_t en = t->GetEnergy();

        if (option == "X" && t->isXZ()) {
            if (en > maxEnergy) {
                maxEnergy = t->GetEnergy();
                track = tck;
            }
        } else if (option == "Y" && t->isYZ()) {
            if (t->GetEnergy() > maxEnergy) {
                maxEnergy = t->GetEnergy();
                track = tck;
            }
        } else if (t->isXYZ()) {
            if (t->GetEnergy() > maxEnergy) {
                maxEnergy = t->GetEnergy();
                track = tck;
            }
        }
    }

    if (track == -1) return NULL;

    return GetTrack(track);
}

Double_t TRestTrackEvent::GetMaxEnergyTrackVolume(TString option) {
    if (this->GetMaxEnergyTrack(option)) return this->GetMaxEnergyTrack(option)->GetVolume();
    return 0;
}

Double_t TRestTrackEvent::GetMaxEnergyTrackLength(TString option) {
    if (this->GetMaxEnergyTrack(option)) return this->GetMaxEnergyTrack(option)->GetLength();
    return 0;
}

Double_t TRestTrackEvent::GetEnergy(TString option) {
    Double_t en = 0;
    for (int tck = 0; tck < this->GetNumberOfTracks(); tck++) {
        if (!this->isTopLevel(tck)) continue;

        TRestTrack* t = GetTrack(tck);

        if (option == "")
            en += t->GetEnergy();

        else if (option == "X" && t->isXZ())
            en += t->GetEnergy();

        else if (option == "Y" && t->isYZ())
            en += t->GetEnergy();

        else if (option == "XYZ" && t->isXYZ())
            en += t->GetEnergy();
    }

    return en;
}

Bool_t TRestTrackEvent::isXYZ() {
    for (int tck = 0; tck < GetNumberOfTracks(); tck++)
        if (!fTrack[tck].isXYZ()) return false;
    return true;
}

Int_t TRestTrackEvent::GetTotalHits() {
    Int_t totHits = 0;
    for (int tck = 0; tck < GetNumberOfTracks(); tck++) totHits += GetTrack(tck)->GetNumberOfHits();
    return totHits;
}

Int_t TRestTrackEvent::GetLevel(Int_t tck) {
    Int_t lvl = 1;
    Int_t parentTrackId = GetTrack(tck)->GetParentID();

    while (parentTrackId > 0) {
        lvl++;
        parentTrackId = GetTrackById(parentTrackId)->GetParentID();
    }
    return lvl;
}

Bool_t TRestTrackEvent::isTopLevel(Int_t tck) {
    if (GetLevels() == GetLevel(tck)) return true;
    return false;
}

Int_t TRestTrackEvent::GetOriginTrackID(Int_t tck) {
    Int_t originTrackID = tck;
    Int_t pID = GetTrack(tck)->GetParentID();

    while (pID != 0) {
        originTrackID = pID;
        pID = GetTrackById(originTrackID)->GetParentID();
    }

    return originTrackID;
}

TRestTrack* TRestTrackEvent::GetOriginTrack(Int_t tck) {
    Int_t originTrackID = GetTrack(tck)->GetTrackID();
    Int_t pID = GetTrackById(originTrackID)->GetParentID();

    while (pID != 0) {
        originTrackID = pID;
        pID = GetTrackById(originTrackID)->GetParentID();
    }

    return GetTrackById(originTrackID);
}

TRestTrack* TRestTrackEvent::GetOriginTrackById(Int_t tckId) {
    Int_t originTrackID = tckId;
    Int_t pID = GetTrackById(tckId)->GetParentID();

    while (pID != 0) {
        originTrackID = pID;
        pID = GetTrackById(originTrackID)->GetParentID();
    }

    return GetTrackById(originTrackID);
}

void TRestTrackEvent::SetLevels() {
    Int_t maxLevel = 0;

    for (int tck = 0; tck < GetNumberOfTracks(); tck++) {
        Int_t lvl = GetLevel(tck);
        if (maxLevel < lvl) maxLevel = lvl;
    }
    fLevels = maxLevel;
}

void TRestTrackEvent::PrintOnlyTracks() {
    cout << "TrackEvent " << GetID() << endl;
    cout << "-----------------------" << endl;
    for (int i = 0; i < GetNumberOfTracks(); i++) {
        cout << "Track " << i << " id : " << GetTrack(i)->GetTrackID()
             << " parent : " << GetTrack(i)->GetParentID() << endl;
    }
    cout << "-----------------------" << endl;
    cout << "Track levels : " << GetLevels() << endl;
}

void TRestTrackEvent::PrintEvent(Bool_t fullInfo) {
    TRestEvent::PrintEvent();

    cout << "Number of tracks : "
         << GetNumberOfTracks("XYZ") + GetNumberOfTracks("X") + GetNumberOfTracks("Y") << endl;
    cout << "Number of tracks XZ " << GetNumberOfTracks("X") << endl;
    cout << "Number of tracks YZ " << GetNumberOfTracks("Y") << endl;
    cout << "Track levels : " << GetLevels() << endl;
    cout << "+++++++++++++++++++++++++++++++++++" << endl;
    for (int i = 0; i < GetNumberOfTracks(); i++) this->GetTrack(i)->PrintTrack(fullInfo);
}

// Draw current event in a Tpad
TPad* TRestTrackEvent::DrawEvent(TString option) {
    /* Not used for the moment
    Bool_t drawXZ = false;
    Bool_t drawYZ = false;
    Bool_t drawXY = false;
    Bool_t drawXYZ = false;
    Bool_t drawLines = false;
    */

    Int_t maxLevel = 0;
    Int_t minLevel = 0;

    vector<string> optList = TRestTools::GetOptions((string)option);

    for (unsigned int n = 0; n < optList.size(); n++) {
        if (optList[n] == "print") this->PrintEvent();
        if (optList[n] == "noWarning") fPrintHitsWarning = false;
    }

    optList.erase(std::remove(optList.begin(), optList.end(), "print"), optList.end());
    optList.erase(std::remove(optList.begin(), optList.end(), "noWarning"), optList.end());

    for (unsigned int n = 0; n < optList.size(); n++) {
        /* Not used for the moment
        if( optList[n] == "XZ" ) drawXZ = true;
        if( optList[n] == "YZ" ) drawYZ = true;
        if( optList[n] == "XY" ) drawXY = true;
        if( optList[n] == "XYZ" ) drawXYZ = true;
        if( optList[n] == "L" || optList[n] == "lines"  ) drawLines = true;
        */
        string opt = optList[n];

        if (opt.find("maxLevel=") != string::npos) maxLevel = stoi(opt.substr(9, opt.length()).c_str());

        if (opt.find("minLevel=") != string::npos) minLevel = stoi(opt.substr(9, opt.length()).c_str());
    }

    if (fXYHit != NULL) {
        delete[] fXYHit;
        fXYHit = NULL;
    }
    if (fXZHit != NULL) {
        delete[] fXZHit;
        fXZHit = NULL;
    }
    if (fYZHit != NULL) {
        delete[] fYZHit;
        fYZHit = NULL;
    }
    if (fXYZHit != NULL) {
        delete[] fXYZHit;
        fXYZHit = NULL;
    }
    if (fXYTrack != NULL) {
        delete[] fXYTrack;
        fXYTrack = NULL;
    }
    if (fXZTrack != NULL) {
        delete[] fXZTrack;
        fXZTrack = NULL;
    }
    if (fYZTrack != NULL) {
        delete[] fYZTrack;
        fYZTrack = NULL;
    }
    if (fXYZTrack != NULL) {
        delete[] fXYZTrack;
        fXYZTrack = NULL;
    }
    if (fPad != NULL) {
        delete fPad;
        fPad = NULL;
    }

    int nTracks = this->GetNumberOfTracks();

    if (nTracks == 0) {
        cout << "Empty event " << endl;
        return NULL;
    }

    this->PrintEvent(false);

    double maxX = -1e10, minX = 1e10, maxZ = -1e10, minZ = 1e10, maxY = -1e10, minY = 1e10;

    Int_t nTotHits = GetTotalHits();

    if (fPrintHitsWarning && nTotHits > 5000) {
        cout << endl;
        cout << " REST WARNING. TRestTrackEvent::DrawEvent. Number of hits is too "
                "high."
             << endl;
        cout << " This drawing method is not properly optimized to draw events "
                "with a high number of hits."
             << endl;
        cout << " To remove this warning you may use the DrawEvent method option : "
                "noWarning "
             << endl;
        cout << endl;

        fPrintHitsWarning = false;
    }

    fXYHit = new TGraph[nTotHits];
    fXZHit = new TGraph[nTotHits];
    fYZHit = new TGraph[nTotHits];
    fXYZHit = new TGraph2D[nTotHits];
    fXYTrack = new TGraph[nTracks];
    fXZTrack = new TGraph[nTracks];
    fYZTrack = new TGraph[nTracks];
    fXYZTrack = new TGraph2D[nTracks];

    vector<Int_t> drawLinesXY(nTracks);
    vector<Int_t> drawLinesXZ(nTracks);
    vector<Int_t> drawLinesYZ(nTracks);
    vector<Int_t> drawLinesXYZ(nTracks);

    for (int i = 0; i < nTracks; i++) {
        drawLinesXY[i] = 0;
        drawLinesXZ[i] = 0;
        drawLinesYZ[i] = 0;
    }

    int countXY = 0, countYZ = 0, countXZ = 0, countXYZ = 0;
    int nTckXY = 0, nTckXZ = 0, nTckYZ = 0, nTckXYZ = 0;

    Double_t minRadiusSize = 0.4;
    Double_t maxRadiusSize = 2.;

    Int_t maxTrackHits = 0;

    Int_t tckColor = 1;

    for (int tck = 0; tck < nTracks; tck++) {
        TRestVolumeHits* hits = fTrack[tck].GetVolumeHits();

        Double_t maxHitEnergy = hits->GetMaximumHitEnergy();
        Double_t meanHitEnergy = hits->GetMeanHitEnergy();

        /*
        cout << "Max hit energy : " << maxHitEnergy << endl;
        cout << "Mean hit energy : " << meanHitEnergy << endl;
        cout << "Number of hits " << hits->GetNumberOfHits( ) <<endl;
        */

        Bool_t isTopLevel = this->isTopLevel(tck);
        if (isTopLevel) tckColor++;
        Int_t level = this->GetLevel(tck);

        if (!isTopLevel && maxLevel > 0 && level > maxLevel) continue;

        if (!isTopLevel && minLevel > 0 && level < minLevel) continue;

        int tckXY = 0, tckYZ = 0, tckXZ = 0, tckXYZ = 0;
        Double_t radius;

        for (int nhit = 0; nhit < hits->GetNumberOfHits(); nhit++) {
            if (hits->GetNumberOfHits() > maxTrackHits) maxTrackHits = hits->GetNumberOfHits();

            if (hits->isNaN(nhit)) {
                cout << "REST Warning. TRestTrackEvent::Draw. Hit is not defined!!" << endl;
                getchar();
                continue;
            }

            Double_t x = hits->GetX(nhit);
            Double_t y = hits->GetY(nhit);
            Double_t z = hits->GetZ(nhit);
            Double_t en = hits->GetEnergy(nhit);
            auto type = hits->GetType(nhit);

            // cout << x << " " << y << " " << z << " " << type << endl;

            /* {{{ Hit size definition (radius) */
            Double_t m = (maxRadiusSize) / (maxHitEnergy - meanHitEnergy);
            Double_t n = (maxRadiusSize - minRadiusSize) - m * meanHitEnergy;

            if (isTopLevel) {
                radius = m * en + n;
                if (radius < 0.1) radius = 0.1;
                if (hits->GetNumberOfHits() == 1) radius = 2;
                if (hits->GetNumberOfHits() > 100) radius = 0.8;

            } else {
                radius = 1.5 * minRadiusSize * level;
            }
            /* }}} */

            if (this->isXYZ() && nhit > 1) {
                if (tckXYZ == 0) nTckXYZ++;
                fXYZTrack[nTckXYZ - 1].SetPoint(tckXYZ, x, y, z);

                if (isTopLevel) drawLinesXYZ[nTckXYZ - 1] = 1;

                if (isTopLevel) {
                    fXYZHit[countXYZ].SetPoint(0, x, y, z);
                    // If there is only one-point the TGraph2D does NOT draw the point!
                    fXYZHit[countXYZ].SetPoint(1, x + 0.001, y + 0.001, z + 0.001);

                    fXYZHit[countXYZ].SetMarkerColor(level + 11);

                    fXYZHit[countXYZ].SetMarkerSize(radius);
                    fXYZHit[countXYZ].SetMarkerStyle(20);
                    countXYZ++;
                }
                tckXYZ++;
            }

            if (type == XY) {
                if (tckXY == 0) nTckXY++;
                fXYTrack[nTckXY - 1].SetPoint(tckXY, x, y);
                if (isTopLevel) drawLinesXY[nTckXY - 1] = 1;
                fXYHit[countXY].SetPoint(0, x, y);

                if (!isTopLevel)
                    fXYHit[countXY].SetMarkerColor(level + 11);
                else
                    fXYHit[countXY].SetMarkerColor(tckColor);

                fXYHit[countXY].SetMarkerSize(radius);
                fXYHit[countXY].SetMarkerStyle(20);
                tckXY++;
                countXY++;
            }

            if (type == XZ) {
                if (tckXZ == 0) nTckXZ++;
                fXZTrack[nTckXZ - 1].SetPoint(tckXZ, x, z);
                if (isTopLevel) drawLinesXZ[nTckXZ - 1] = 1;
                fXZHit[countXZ].SetPoint(0, x, z);

                if (!isTopLevel)
                    fXZHit[countXZ].SetMarkerColor(level + 11);
                else
                    fXZHit[countXZ].SetMarkerColor(tckColor);

                fXZHit[countXZ].SetMarkerSize(radius);
                fXZHit[countXZ].SetMarkerStyle(20);
                tckXZ++;
                countXZ++;
            }

            if (type == YZ) {
                if (tckYZ == 0) nTckYZ++;
                fYZTrack[nTckYZ - 1].SetPoint(tckYZ, y, z);
                if (isTopLevel) drawLinesYZ[nTckYZ - 1] = 1;
                fYZHit[countYZ].SetPoint(0, y, z);

                if (!isTopLevel)
                    fYZHit[countYZ].SetMarkerColor(level + 11);
                else
                    fYZHit[countYZ].SetMarkerColor(tckColor);

                fYZHit[countYZ].SetMarkerSize(radius);
                fYZHit[countYZ].SetMarkerStyle(20);
                tckYZ++;
                countYZ++;
            }

            if (x > maxX) maxX = x;
            if (x < minX) minX = x;
            if (y > maxY) maxY = y;
            if (y < minY) minY = y;
            if (z > maxZ) maxZ = z;
            if (z < minZ) minZ = z;
        }
    }

    fPad = new TPad(this->GetName(), " ", 0, 0, 1, 1);
    if (this->isXYZ())
        fPad->Divide(2, 2);
    else
        fPad->Divide(2, 1);
    fPad->Draw();

    char title[256];
    sprintf(title, "Event ID %d", this->GetID());

    TMultiGraph* mgXY = new TMultiGraph();
    TMultiGraph* mgXZ = new TMultiGraph();
    TMultiGraph* mgYZ = new TMultiGraph();

    fPad->cd(1)->DrawFrame(minX - 10, minZ - 10, maxX + 10, maxZ + 10, title);
    fPad->cd(2)->DrawFrame(minY - 10, minZ - 10, maxY + 10, maxZ + 10, title);

    if (this->isXYZ()) fPad->cd(3)->DrawFrame(minX - 10, minY - 10, maxX + 10, maxY + 10, title);

    for (int i = 0; i < countXZ; i++) mgXZ->Add(&fXZHit[i]);

    fPad->cd(1);
    mgXZ->GetXaxis()->SetTitle("X-axis (mm)");
    mgXZ->GetYaxis()->SetTitle("Z-axis (mm)");
    mgXZ->GetYaxis()->SetTitleOffset(1.75);
    mgXZ->GetYaxis()->SetTitleSize(1.4 * mgXZ->GetYaxis()->GetTitleSize());
    mgXZ->GetXaxis()->SetTitleSize(1.4 * mgXZ->GetXaxis()->GetTitleSize());
    mgXZ->GetYaxis()->SetLabelSize(1.25 * mgXZ->GetYaxis()->GetLabelSize());
    mgXZ->GetXaxis()->SetLabelSize(1.25 * mgXZ->GetXaxis()->GetLabelSize());
    mgXZ->Draw("P");

    for (int i = 0; i < countYZ; i++) mgYZ->Add(&fYZHit[i]);

    fPad->cd(2);
    mgYZ->GetXaxis()->SetTitle("Y-axis (mm)");
    mgYZ->GetYaxis()->SetTitle("Z-axis (mm)");
    mgYZ->GetYaxis()->SetTitleOffset(1.75);
    mgYZ->GetYaxis()->SetTitleSize(1.4 * mgYZ->GetYaxis()->GetTitleSize());
    mgYZ->GetXaxis()->SetTitleSize(1.4 * mgYZ->GetXaxis()->GetTitleSize());
    mgYZ->GetYaxis()->SetLabelSize(1.25 * mgYZ->GetYaxis()->GetLabelSize());
    mgYZ->GetXaxis()->SetLabelSize(1.25 * mgYZ->GetXaxis()->GetLabelSize());
    mgYZ->Draw("P");

    if (this->isXYZ()) {
        for (int i = 0; i < countXY; i++) mgXY->Add(&fXYHit[i]);

        fPad->cd(3);
        mgXY->GetXaxis()->SetTitle("X-axis (mm)");
        mgXY->GetYaxis()->SetTitle("Y-axis (mm)");
        mgXY->GetYaxis()->SetTitleOffset(1.75);
        mgXY->Draw("P");
        mgXY->GetYaxis()->SetTitleSize(1.4 * mgXY->GetYaxis()->GetTitleSize());
        mgXY->GetXaxis()->SetTitleSize(1.4 * mgXY->GetXaxis()->GetTitleSize());
        mgXY->GetYaxis()->SetLabelSize(1.25 * mgXY->GetYaxis()->GetLabelSize());
        mgXY->GetXaxis()->SetLabelSize(1.25 * mgXY->GetXaxis()->GetLabelSize());
    }

    for (int tck = 0; tck < nTckXZ; tck++) {
        fPad->cd(1);
        fXZTrack[tck].SetLineWidth(2.);
        if (fXZTrack[tck].GetN() < 100 && drawLinesXZ[tck] == 1) fXZTrack[tck].Draw("L");
    }

    for (int tck = 0; tck < nTckYZ; tck++) {
        fPad->cd(2);
        fYZTrack[tck].SetLineWidth(2.);
        if (fYZTrack[tck].GetN() < 100 && drawLinesYZ[tck] == 1) fYZTrack[tck].Draw("L");
    }

    if (this->isXYZ()) {
        for (int tck = 0; tck < nTckXY; tck++) {
            fPad->cd(3);
            fXYTrack[tck].SetLineWidth(2.);
            if (fXYTrack[tck].GetN() < 100 && drawLinesXY[tck] == 1) fXYTrack[tck].Draw("L");
        }

        fPad->cd(4);

        TString option = "P";
        for (int tck = 0; tck < nTckXYZ; tck++) {
            fXYZTrack[tck].SetLineWidth(2.);
            if (fXZTrack[tck].GetN() < 100 && drawLinesXYZ[tck] == 1) {
                fXYZTrack[tck].Draw("LINE");
                option = "same P";
            }
        }

        for (int i = 0; i < countXYZ; i++) {
            if (i > 0) option = "same P";
            fXYZHit[i].Draw(option);
        }
    }

    return fPad;
}
