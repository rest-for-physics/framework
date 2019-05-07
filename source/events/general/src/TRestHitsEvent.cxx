#include "TRestHitsEvent.h"
#include "TRestTools.h"

#include "TStyle.h"

using namespace std;
using namespace TMath;

ClassImp(TRestHitsEvent)

    TRestHitsEvent::TRestHitsEvent() {
    fHits = new TRestHits();

    fPad = NULL;

    fXYHitGraph = NULL;
    fXZHitGraph = NULL;
    fYZHitGraph = NULL;

    fXYHisto = NULL;
    fXZHisto = NULL;
    fYZHisto = NULL;

    fXHisto = NULL;
    fYHisto = NULL;
    fZHisto = NULL;

    fMinX = -10;
    fMaxX = 10;

    fMinY = -10;
    fMaxY = 10;

    fMinZ = -10;
    fMaxZ = 10;
}

TRestHitsEvent::~TRestHitsEvent() { delete fHits; }

void TRestHitsEvent::AddHit(Double_t x, Double_t y, Double_t z, Double_t en, Double_t t, Short_t mod,
                            Short_t ch) {
    fHits->AddHit(x, y, z, en, t, mod, ch);
}

void TRestHitsEvent::AddHit(TVector3 pos, Double_t en, Double_t t, Short_t mod, Short_t ch) {
    fHits->AddHit(pos, en, t, mod, ch);
}

void TRestHitsEvent::Initialize() {
    TRestEvent::Initialize();

    RemoveHits();

    fXZHits = new TRestHits();
    fYZHits = new TRestHits();
    fXYZHits = new TRestHits();
}

void TRestHitsEvent::MergeHits(int n, int m) { fHits->MergeHits(n, m); }

void TRestHitsEvent::RemoveHit(int n) { fHits->RemoveHit(n); }

void TRestHitsEvent::RemoveHits() { fHits->RemoveHits(); }

TRestHits* TRestHitsEvent::GetXZHits() {
    fXZHits->RemoveHits();

    for (int i = 0; i < this->GetNumberOfHits(); i++)
        if (IsNaN(this->GetY(i)))
            fXZHits->AddHit(this->GetX(i), this->GetY(i), this->GetZ(i), this->GetEnergy(i), 0,
                            this->GetModule(i), this->GetChannel(i));

    return fXZHits;
}

TRestHits* TRestHitsEvent::GetYZHits() {
    fYZHits->RemoveHits();

    for (int i = 0; i < this->GetNumberOfHits(); i++)
        if (IsNaN(this->GetX(i)))
            fYZHits->AddHit(this->GetX(i), this->GetY(i), this->GetZ(i), this->GetEnergy(i), 0,
                            this->GetModule(i), this->GetChannel(i));

    return fYZHits;
}

TRestHits* TRestHitsEvent::GetXYZHits() {
    fXYZHits->RemoveHits();

    for (int i = 0; i < this->GetNumberOfHits(); i++)
        if (!IsNaN(this->GetX(i)) && !IsNaN(this->GetY(i)) && !IsNaN(this->GetZ(i)))
            fXYZHits->AddHit(this->GetX(i), this->GetY(i), this->GetZ(i), this->GetEnergy(i), 0,
                             this->GetModule(i), this->GetChannel(i));

    return fXYZHits;
}

void TRestHitsEvent::ChangeOrigin(double origx, double origy, double origz) {
    /*
        for(int i = 0; i < fHits; i++)
        {
                fX.fArray[i] += origx;
                fY.fArray[i] += origy;
                fZ.fArray[i] += origz;
        }
    */
}

Bool_t TRestHitsEvent::isHitsEventInsideCylinder(TVector3 x0, TVector3 x1, Double_t radius) {
    if (fHits->GetNumberOfHitsInsideCylinder(x0, x1, radius) > 0) return true;

    return false;
}

Bool_t TRestHitsEvent::isHitsEventInsidePrism(TVector3 x0, TVector3 x1, Double_t sizeX, Double_t sizeY,
                                              Double_t theta) {
    if (fHits->GetNumberOfHitsInsidePrism(x0, x1, sizeX, sizeY, theta) > 0) return true;

    return false;
}

Bool_t TRestHitsEvent::areHitsFullyContainnedInsideCylinder(TVector3 x0, TVector3 x1, Double_t radius) {
    if (fHits->GetNumberOfHitsInsideCylinder(x0, x1, radius) == GetNumberOfHits()) return true;

    return false;
}

Bool_t TRestHitsEvent::areHitsFullyContainnedInsidePrism(TVector3 x0, TVector3 x1, Double_t sizeX,
                                                         Double_t sizeY, Double_t theta) {
    if (fHits->GetNumberOfHitsInsidePrism(x0, x1, sizeX, sizeY, theta) == GetNumberOfHits()) return true;

    return false;
}

Double_t TRestHitsEvent::GetClosestHitInsideDistanceToCylinderWall(TVector3 x0, TVector3 x1,
                                                                   Double_t radius) {
    Double_t rad2 = radius * radius;
    Double_t hitDistance = rad2;

    Double_t d2, l;

    TVector3 axis = x1 - x0;
    Double_t cylLength = axis.Mag();

    Int_t nhits = 0;
    for (int n = 0; n < GetNumberOfHits(); n++) {
        if (fHits->isHitNInsideCylinder(n, x0, x1, radius)) {
            l = axis.Dot(this->GetPosition(n) - x0) / cylLength;

            d2 = rad2 - (this->GetPosition(n) - x0).Mag2() - l * l;

            if (d2 < hitDistance) hitDistance = d2;

            nhits++;
        }
    }

    if (nhits == 0) return -1;

    return TMath::Sqrt(hitDistance);
}

Double_t TRestHitsEvent::GetClosestHitInsideDistanceToCylinderTop(TVector3 x0, TVector3 x1, Double_t radius) {
    TVector3 axis = x1 - x0;
    Double_t cylLength = axis.Mag();

    Double_t hitDistance = cylLength;
    Double_t d = cylLength;

    Int_t nhits = 0;
    for (int n = 0; n < GetNumberOfHits(); n++) {
        if (fHits->isHitNInsideCylinder(n, x0, x1, radius)) {
            d = cylLength - axis.Dot(this->GetPosition(n) - x0) / cylLength;

            if (d < hitDistance) hitDistance = d;

            nhits++;
        }
    }

    if (nhits == 0) return -1;

    return hitDistance;
}

Double_t TRestHitsEvent::GetClosestHitInsideDistanceToCylinderBottom(TVector3 x0, TVector3 x1,
                                                                     Double_t radius) {
    TVector3 axis = x1 - x0;
    Double_t cylLength = axis.Mag();

    Double_t hitDistance = cylLength;
    Double_t d = cylLength;

    Int_t nhits = 0;
    for (int n = 0; n < GetNumberOfHits(); n++) {
        if (fHits->isHitNInsideCylinder(n, x0, x1, radius)) {
            d = cylLength - axis.Dot(this->GetPosition(n) - x0) / cylLength;

            if (d < hitDistance) hitDistance = d;

            nhits++;
        }
    }

    if (nhits == 0) return -1;

    return hitDistance;
}

Double_t TRestHitsEvent::GetClosestHitInsideDistanceToPrismWall(TVector3 x0, TVector3 x1, Double_t sizeX,
                                                                Double_t sizeY, Double_t theta) {
    Double_t dX = sizeX / 2;
    Double_t dY = sizeY / 2;

    Double_t hitDistance = max(dX, dY);

    Double_t d;
    Int_t nhits = 0;
    for (int n = 0; n < GetNumberOfHits(); n++) {
        if (fHits->isHitNInsidePrism(n, x0, x1, sizeX, sizeY, theta)) {
            dX = sizeX / 2 - TMath::Abs((this->GetPosition(n) - x0).X());
            dY = sizeY / 2 - TMath::Abs((this->GetPosition(n) - x0).Y());

            d = min(dX, dY);

            if (d < hitDistance) hitDistance = d;

            nhits++;
        }
    }

    if (nhits == 0) return -1;

    return hitDistance;
}

Double_t TRestHitsEvent::GetClosestHitInsideDistanceToPrismTop(TVector3 x0, TVector3 x1, Double_t sizeX,
                                                               Double_t sizeY, Double_t theta) {
    TVector3 axis = x1 - x0;
    Double_t prismLength = axis.Mag();

    Double_t hitDistance = prismLength;

    Double_t d;
    Int_t nhits = 0;
    for (int n = 0; n < GetNumberOfHits(); n++) {
        if (fHits->isHitNInsidePrism(n, x0, x1, sizeX, sizeY, theta)) {
            d = prismLength - axis.Dot(this->GetPosition(n) - x0) / prismLength;

            if (d < hitDistance) hitDistance = d;

            nhits++;
        }
    }

    if (nhits == 0) return -1;

    return hitDistance;
}

Double_t TRestHitsEvent::GetClosestHitInsideDistanceToPrismBottom(TVector3 x0, TVector3 x1, Double_t sizeX,
                                                                  Double_t sizeY, Double_t theta) {
    TVector3 axis = x1 - x0;
    Double_t prismLength = axis.Mag();

    Double_t hitDistance = prismLength;

    Double_t d;
    Int_t nhits = 0;
    for (int n = 0; n < GetNumberOfHits(); n++) {
        if (fHits->isHitNInsidePrism(n, x0, x1, sizeX, sizeY, theta)) {
            d = axis.Dot(this->GetPosition(n) - x0) / prismLength;

            if (d < hitDistance) hitDistance = d;

            nhits++;
        }
    }

    if (nhits == 0) return -1;

    return hitDistance;
}

TPad* TRestHitsEvent::DrawEvent(TString option) {
    vector<TString> optList = TRestTools::GetOptions(option);

    SetBoundaries();

    for (unsigned int n = 0; n < optList.size(); n++) {
        if (optList[n] == "print") this->PrintEvent();
    }

    optList.erase(std::remove(optList.begin(), optList.end(), "print"), optList.end());

    if (optList.size() == 0) optList.push_back("hist(Cont1,col)");

    if (fPad != NULL) {
        delete fPad;
        fPad = NULL;
    }

    fPad = new TPad(this->GetName(), " ", 0, 0, 1, 1);
    fPad->Divide(3, 2 * optList.size());
    fPad->Draw();

    Int_t column = 0;
    for (unsigned int n = 0; n < optList.size(); n++) {
        string optionStr = (string)optList[n];
        TString drawEventOption = optList[n];

        // Generating drawOption argument
        size_t startPos = optionStr.find("(");
        if (startPos != string::npos) drawEventOption = optList[n](0, startPos);

        // Generating histogram option argument
        string histOption = "";
        size_t endPos = optionStr.find(")");
        if (endPos != string::npos) {
            TString histOptionTmp = optList[n](startPos + 1, endPos - startPos - 1);

            histOption = (string)histOptionTmp;
            size_t pos = 0;
            while ((pos = histOption.find(",", pos)) != string::npos) {
                histOption.replace(pos, 1, ":");
                pos = pos + 1;
            }
        }

        // Generating histogram pitch argument
        string pitchOption = "";

        startPos = optionStr.find("[");
        endPos = optionStr.find("]");
        Double_t pitch = 3;
        if (endPos != string::npos) {
            TString pitchOption = optList[n](startPos + 1, endPos - startPos - 1);
            pitch = stod((string)pitchOption);
        }

        if (drawEventOption == "graph") this->DrawGraphs(column);

        if (drawEventOption == "hist") this->DrawHistograms(column, pitch, histOption);
    }

    return fPad;
}

void TRestHitsEvent::SetBoundaries() {
    Double_t maxX = -1e10, minX = 1e10, maxZ = -1e10, minZ = 1e10, maxY = -1e10, minY = 1e10;

    for (int nhit = 0; nhit < this->GetNumberOfHits(); nhit++) {
        Double_t x = fHits->GetX(nhit);
        Double_t y = fHits->GetY(nhit);
        Double_t z = fHits->GetZ(nhit);

        if (x > maxX) maxX = x;
        if (x < minX) minX = x;
        if (y > maxY) maxY = y;
        if (y < minY) minY = y;
        if (z > maxZ) maxZ = z;
        if (z < minZ) minZ = z;
    }

    fMinX = minX;
    fMaxX = maxX;

    fMinY = minY;
    fMaxY = maxY;

    fMinZ = minZ;
    fMaxZ = maxZ;
}

void TRestHitsEvent::DrawGraphs(Int_t& column) {
    if (fXYHitGraph != NULL) {
        delete fXYHitGraph;
        fXYHitGraph = NULL;
    }
    if (fXZHitGraph != NULL) {
        delete fXZHitGraph;
        fXZHitGraph = NULL;
    }
    if (fYZHitGraph != NULL) {
        delete fYZHitGraph;
        fYZHitGraph = NULL;
    }

    Double_t xz[2][this->GetNumberOfHits()];
    Double_t yz[2][this->GetNumberOfHits()];
    Double_t xy[2][this->GetNumberOfHits()];

    /* {{{ Creating xz, yz, and xy arrays and initializing graphs */
    Int_t nXZ = 0;
    Int_t nYZ = 0;
    Int_t nXY = 0;

    for (int nhit = 0; nhit < this->GetNumberOfHits(); nhit++) {
        Double_t x = fHits->GetX(nhit);
        Double_t y = fHits->GetY(nhit);
        Double_t z = fHits->GetZ(nhit);

        if (!IsNaN(x) && !IsNaN(z)) {
            xz[0][nXZ] = x;
            xz[1][nXZ] = z;
            nXZ++;
        }

        if (!IsNaN(y) && !IsNaN(z)) {
            yz[0][nYZ] = y;
            yz[1][nYZ] = z;
            nYZ++;
        }

        if (!IsNaN(x) && !IsNaN(y)) {
            xy[0][nXY] = x;
            xy[1][nXY] = y;
            nXY++;
        }
    }

    fXZHitGraph = new TGraph(nXZ, xz[0], xz[1]);
    fXZHitGraph->SetMarkerColor(kBlue);
    fXZHitGraph->SetMarkerSize(0.3);
    fXZHitGraph->SetMarkerStyle(20);

    fYZHitGraph = new TGraph(nYZ, yz[0], yz[1]);
    fYZHitGraph->SetMarkerColor(kRed);
    fYZHitGraph->SetMarkerSize(0.3);
    fYZHitGraph->SetMarkerStyle(20);

    fXYHitGraph = new TGraph(nXY, xy[0], xy[1]);
    fXYHitGraph->SetMarkerColor(kBlack);
    fXYHitGraph->SetMarkerSize(0.3);
    fXYHitGraph->SetMarkerStyle(20);
    /* }}} */

    char title[256];
    sprintf(title, "Event ID %d", this->GetID());

    if (nXZ > 0) {
        fPad->cd(1 + 3 * column);
        fXZHitGraph->SetTitle(title);
        fXZHitGraph->Draw("AP*");

        fXZHitGraph->GetXaxis()->SetTitle("X-axis (mm)");
        fXZHitGraph->GetYaxis()->SetTitle("Z-axis (mm)");
    }

    if (nYZ > 0) {
        fPad->cd(2 + 3 * column);
        fYZHitGraph->SetTitle(title);
        fYZHitGraph->Draw("AP");

        fYZHitGraph->GetXaxis()->SetTitle("Y-axis (mm)");
        fYZHitGraph->GetYaxis()->SetTitle("Z-axis (mm)");
    }

    if (nXY > 0) {
        fPad->cd(3 + 3 * column);
        fXYHitGraph->SetTitle(title);
        fXYHitGraph->Draw("AP");

        fXYHitGraph->GetXaxis()->SetTitle("X-axis (mm)");
        fXYHitGraph->GetYaxis()->SetTitle("Y-axis (mm)");
    }

    column++;
}

void TRestHitsEvent::DrawHistograms(Int_t& column, Double_t pitch, TString histOption) {
    if (fXYHisto != NULL) {
        delete fXYHisto;
        fXYHisto = NULL;
    }
    if (fXZHisto != NULL) {
        delete fXZHisto;
        fXZHisto = NULL;
    }
    if (fYZHisto != NULL) {
        delete fYZHisto;
        fYZHisto = NULL;
    }

    if (fXHisto != NULL) {
        delete fXHisto;
        fXHisto = NULL;
    }
    if (fYHisto != NULL) {
        delete fYHisto;
        fYHisto = NULL;
    }
    if (fZHisto != NULL) {
        delete fZHisto;
        fZHisto = NULL;
    }

    Int_t nBinsX = (fMaxX - fMinX + 20) / pitch;
    Int_t nBinsY = (fMaxY - fMinY + 20) / pitch;
    Int_t nBinsZ = (fMaxZ - fMinZ + 20) * 3 / pitch;

    fXYHisto = new TH2F("XY", "", nBinsX, fMinX - 10, fMinX + pitch * nBinsX, nBinsY, fMinY - 10,
                        fMinY + pitch * nBinsY);
    fXZHisto = new TH2F("XZ", "", nBinsX, fMinX - 10, fMinX + pitch * nBinsX, nBinsZ, fMinZ - 10,
                        fMinZ + (pitch / 3) * nBinsZ);
    fYZHisto = new TH2F("YZ", "", nBinsY, fMinY - 10, fMinY + pitch * nBinsY, nBinsZ, fMinZ - 10,
                        fMinZ + (pitch / 3) * nBinsZ);

    fXHisto = new TH1F("X", "", nBinsX, fMinX - 10, fMinX + pitch * nBinsX);
    fYHisto = new TH1F("Y", "", nBinsY, fMinY - 10, fMinY + pitch * nBinsY);
    fZHisto = new TH1F("Z", "", nBinsZ, fMinZ - 10, fMinZ + pitch * nBinsZ);

    Int_t nYZ = 0, nXZ = 0, nXY = 0;
    Int_t nX = 0, nY = 0, nZ = 0;

    for (int nhit = 0; nhit < this->GetNumberOfHits(); nhit++) {
        Double_t x = fHits->GetX(nhit);
        Double_t y = fHits->GetY(nhit);
        Double_t z = fHits->GetZ(nhit);

        if (!IsNaN(x) && !IsNaN(z)) {
            fXZHisto->Fill(x, z);
            nXZ++;
        }

        if (!IsNaN(y) && !IsNaN(z)) {
            fYZHisto->Fill(y, z);
            nYZ++;
        }

        if (!IsNaN(x) && !IsNaN(y)) {
            fXYHisto->Fill(x, y);
            nXY++;
        }

        if (!IsNaN(x)) {
            fXHisto->Fill(x);
            nX++;
        }

        if (!IsNaN(y)) {
            fYHisto->Fill(y);
            nY++;
        }

        if (!IsNaN(z)) {
            fZHisto->Fill(z);
            nZ++;
        }
    }

    TStyle style;
    style.SetPalette(1);

    if (nXZ > 0) {
        fPad->cd(1 + 3 * column);
        fXZHisto->Draw(histOption);
        fXZHisto->GetXaxis()->SetTitle("X-axis (mm)");
        fXZHisto->GetYaxis()->SetTitle("Z-axis (mm)");
        fXZHisto->GetYaxis()->SetTitleSize(1.4 * fXZHisto->GetYaxis()->GetTitleSize());
        fXZHisto->GetXaxis()->SetTitleSize(1.4 * fXZHisto->GetXaxis()->GetTitleSize());
        fXZHisto->GetYaxis()->SetLabelSize(1.25 * fXZHisto->GetYaxis()->GetLabelSize());
        fXZHisto->GetXaxis()->SetLabelSize(1.25 * fXZHisto->GetXaxis()->GetLabelSize());
        fXZHisto->GetYaxis()->SetTitleOffset(1.75);
    }

    if (nYZ > 0) {
        fPad->cd(2 + 3 * column);
        fYZHisto->Draw(histOption);
        fYZHisto->GetXaxis()->SetTitle("Y-axis (mm)");
        fYZHisto->GetYaxis()->SetTitle("Z-axis (mm)");
        fYZHisto->GetYaxis()->SetTitleSize(1.4 * fYZHisto->GetYaxis()->GetTitleSize());
        fYZHisto->GetXaxis()->SetTitleSize(1.4 * fYZHisto->GetXaxis()->GetTitleSize());
        fYZHisto->GetYaxis()->SetLabelSize(1.25 * fYZHisto->GetYaxis()->GetLabelSize());
        fYZHisto->GetXaxis()->SetLabelSize(1.25 * fYZHisto->GetXaxis()->GetLabelSize());
        fYZHisto->GetYaxis()->SetTitleOffset(1.75);
    }

    if (nXY > 0) {
        fPad->cd(3 + 3 * column);
        fXYHisto->Draw(histOption);
        fXYHisto->GetXaxis()->SetTitle("X-axis (mm)");
        fXYHisto->GetYaxis()->SetTitle("Y-axis (mm)");
    }

    column++;

    if (nX > 0) {
        fPad->cd(1 + 3 * column);
        fXHisto->Draw(histOption);
        fXHisto->GetXaxis()->SetTitle("X-axis (mm)");
        fXHisto->GetYaxis()->SetTitle("Number of events");
    }

    if (nY > 0) {
        fPad->cd(2 + 3 * column);
        fYHisto->Draw(histOption);
        fYHisto->GetYaxis()->SetTitle("Y-axis (mm)");
        fYHisto->GetYaxis()->SetTitle("Number of events");
    }

    if (nZ > 0) {
        fPad->cd(3 + 3 * column);
        fZHisto->Draw(histOption);
        fZHisto->GetZaxis()->SetTitle("Z-axis (mm)");
        fZHisto->GetYaxis()->SetTitle("Number of events");
    }

    column++;
}

void TRestHitsEvent::PrintEvent(Int_t nHits) {
    TRestEvent::PrintEvent();

    cout << "Total energy : " << GetEnergy() << endl;
    cout << "Mean position : ( " << GetMeanPositionX() << " , " << GetMeanPositionY() << " , "
         << GetMeanPositionZ() << " ) " << endl;
    cout << "Number of hits : " << fHits->GetNumberOfHits() << endl;
    if (nHits != -1) {
        cout << "+++++++++++++++++++++++" << endl;
        cout << "Printing only the first " << nHits << " hits" << endl;
    }

    fHits->PrintHits(nHits);
}
