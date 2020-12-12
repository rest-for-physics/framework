///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestHits.cxx
///
///             Event class to store hits
///
///             sept 2015:   First concept
///                 Created as part of the conceptualization of existing REST
///                 software.
///                 Javier Galan
///		nov 2015:
///		    Changed vectors fX fY fZ and fEnergy from <Int_t> to
///< Float_t> 	            JuanAn Garcia
///_______________________________________________________________________________

#include "TRestHits.h"
#include "TROOT.h"
using namespace std;
using namespace TMath;

ClassImp(TRestHits);

TRestHits::TRestHits() {
    fNHits = 0;
    fTotEnergy = 0;
}

TRestHits::~TRestHits() {}

Bool_t TRestHits::areXY() {
    for (int i = 0; i < GetNumberOfHits(); i++) {
        if (fType[i] != XY) {
            // all hits should fit this condition to be considered XY
            return false;
        } else if (i == GetNumberOfHits() - 1) {
            return true;
        }
    }
    return false;
}

Bool_t TRestHits::areXZ() {
    for (int i = 0; i < GetNumberOfHits(); i++) {
        if (fType[i] != XZ) {
            // all hits should fit this condition to be considered XY
            return false;
        } else if (i == GetNumberOfHits() - 1) {
            return true;
        }
    }
    return false;
}

Bool_t TRestHits::areYZ() {
    for (int i = 0; i < GetNumberOfHits(); i++) {
        if (fType[i] != YZ) {
            // all hits should fit this condition to be considered XY
            return false;
        } else if (i == GetNumberOfHits() - 1) {
            return true;
        }
    }
    return false;
}

Bool_t TRestHits::areXYZ() {
    for (int i = 0; i < GetNumberOfHits(); i++) {
        if (fType[i] != XYZ) {
            // all hits should fit this condition to be considered XY
            return false;
        } else if (i == GetNumberOfHits() - 1) {
            return true;
        }
    }
    return false;
}

Bool_t TRestHits::isNaN(Int_t n) {
    if (IsNaN(GetX(n)) && IsNaN(GetY(n)) && IsNaN(GetZ(n))) return true;
    return false;
}

void TRestHits::GetXArray(Float_t* x) {
    if (areYZ()) {
        for (int i = 0; i < GetNumberOfHits(); i++) x[i] = 0;
    } else {
        for (int i = 0; i < GetNumberOfHits(); i++) x[i] = GetX(i);
    }
}

void TRestHits::InitializeXArray(Float_t x) {
    for (int i = 0; i < GetNumberOfHits(); i++) fX[i] = x;
}

void TRestHits::InitializeYArray(Float_t y) {
    for (int i = 0; i < GetNumberOfHits(); i++) fY[i] = y;
}

void TRestHits::InitializeZArray(Float_t z) {
    for (int i = 0; i < GetNumberOfHits(); i++) fZ[i] = z;
}

void TRestHits::GetYArray(Float_t* y) {
    if (areXZ()) {
        for (int i = 0; i < GetNumberOfHits(); i++) y[i] = 0;
    } else {
        for (int i = 0; i < GetNumberOfHits(); i++) y[i] = GetY(i);
    }
}

void TRestHits::GetZArray(Float_t* z) {
    if (areXY()) {
        for (int i = 0; i < GetNumberOfHits(); i++) z[i] = 0;
    } else {
        for (int i = 0; i < GetNumberOfHits(); i++) z[i] = GetZ(i);
    }
}

Double_t TRestHits::GetEnergyIntegral() {
    Double_t sum = 0;
    for (int i = 0; i < GetNumberOfHits(); i++) sum += GetEnergy(i);
    return sum;
}

Bool_t TRestHits::isHitNInsidePrism(Int_t n, TVector3 x0, TVector3 x1, Double_t sizeX, Double_t sizeY,
                                    Double_t theta) {
    TVector3 axis = x1 - x0;

    Double_t prismLength = axis.Mag();

    TVector3 hitPos = this->GetPosition(n) - x0;
    hitPos.RotateZ(theta);
    Double_t l = axis.Dot(hitPos) / prismLength;

    if ((l > 0) && (l < prismLength))
        if ((TMath::Abs(hitPos.X()) < sizeX / 2) && (TMath::Abs(hitPos.Y()) < sizeY / 2)) return true;

    return false;
}

Double_t TRestHits::GetEnergyInPrism(TVector3 x0, TVector3 x1, Double_t sizeX, Double_t sizeY,
                                     Double_t theta) {
    Double_t energy = 0.;

    for (int n = 0; n < GetNumberOfHits(); n++)
        if (isHitNInsidePrism(n, x0, x1, sizeX, sizeY, theta)) energy += this->GetEnergy(n);

    return energy;
}

Int_t TRestHits::GetNumberOfHitsInsidePrism(TVector3 x0, TVector3 x1, Double_t sizeX, Double_t sizeY,
                                            Double_t theta) {
    Int_t hits = 0;

    for (int n = 0; n < GetNumberOfHits(); n++)
        if (isHitNInsidePrism(n, x0, x1, sizeX, sizeY, theta)) hits++;

    return hits;
}

Bool_t TRestHits::isHitNInsideCylinder(Int_t n, TVector3 x0, TVector3 x1, Double_t radius) {
    /* cout << "TRestHits::isHitNInsideCylinder has not been validated." << endl;
     cout << "After validation this output should be removed" << endl;*/

    TVector3 axis = x1 - x0;

    Double_t cylLength = axis.Mag();

    /* cout << "X0 : " << endl;
     x0.Print();
     cout << "Y0 : " << endl;
     x1.Print();
     cout << "Radius : " << radius << endl;

     cout << "Absolute position" << endl;
     this->GetPosition( n ).Print();*/

    TVector3 hitPos = this->GetPosition(n) - x0;
    // cout << "HitPos" << endl;
    //  hitPos.Print();

    Double_t l = axis.Dot(hitPos) / cylLength;

    if (l > 0 && l < cylLength) {
        // cout << "Is inside length" << endl;
        Double_t hitPosNorm2 = hitPos.Mag2();
        Double_t r = TMath::Sqrt(hitPosNorm2 - l * l);

        if (r < radius) return true;
    }

    return false;
}

Double_t TRestHits::GetEnergyInCylinder(Int_t i, Int_t j, Double_t radius) {
    return GetEnergyInCylinder(this->GetPosition(i), this->GetPosition(j), radius);
}

Double_t TRestHits::GetEnergyInCylinder(TVector3 x0, TVector3 x1, Double_t radius) {
    Double_t energy = 0.;
    for (int n = 0; n < GetNumberOfHits(); n++) {
        if (isHitNInsideCylinder(n, x0, x1, radius)) energy += this->GetEnergy(n);
    }

    return energy;
}

Int_t TRestHits::GetNumberOfHitsInsideCylinder(Int_t i, Int_t j, Double_t radius) {
    return GetNumberOfHitsInsideCylinder(this->GetPosition(i), this->GetPosition(j), radius);
}

Int_t TRestHits::GetNumberOfHitsInsideCylinder(TVector3 x0, TVector3 x1, Double_t radius) {
    Int_t hits = 0;
    for (int n = 0; n < GetNumberOfHits(); n++)
        if (isHitNInsideCylinder(n, x0, x1, radius)) hits++;

    return hits;
}

Double_t TRestHits::GetEnergyInSphere(TVector3 pos0, Double_t radius) {
    return GetEnergyInSphere(pos0.X(), pos0.Y(), pos0.Z(), radius);
}

Double_t TRestHits::GetEnergyInSphere(Double_t x0, Double_t y0, Double_t z0, Double_t radius) {
    Double_t sum = 0;
    for (int i = 0; i < GetNumberOfHits(); i++) {
        Double_t x = this->GetPosition(i).X();
        Double_t y = this->GetPosition(i).Y();
        Double_t z = this->GetPosition(i).Z();

        Double_t dist = (x - x0) * (x - x0) + (y - y0) * (y - y0) + (z - z0) * (z - z0);

        if (dist < radius * radius) sum += GetEnergy(i);
    }
    return sum;
}

Bool_t TRestHits::isHitNInsideSphere(Int_t n, TVector3 pos0, Double_t radius) {
    return isHitNInsideSphere(n, pos0.X(), pos0.Y(), pos0.Z(), radius);
}

Bool_t TRestHits::isHitNInsideSphere(Int_t n, Double_t x0, Double_t y0, Double_t z0, Double_t radius) {
    Double_t x = this->GetPosition(n).X();
    Double_t y = this->GetPosition(n).Y();
    Double_t z = this->GetPosition(n).Z();

    Double_t dist = (x - x0) * (x - x0) + (y - y0) * (y - y0) + (z - z0) * (z - z0);

    if (dist < radius * radius) return kTRUE;

    return kFALSE;
}

void TRestHits::AddHit(Double_t x, Double_t y, Double_t z, Double_t en, Double_t t, REST_HitType type) {
    fNHits++;
    fX.push_back((Float_t)(x));
    fY.push_back((Float_t)(y));
    fZ.push_back((Float_t)(z));
    fT.push_back((Float_t)t);
    fEnergy.push_back((Float_t)(en));
    fType.push_back(type);

    fTotEnergy += en;
}

void TRestHits::AddHit(TVector3 pos, Double_t en, Double_t t, REST_HitType type) {
    fNHits++;

    fX.push_back((Float_t)(pos.X()));
    fY.push_back((Float_t)(pos.Y()));
    fZ.push_back((Float_t)(pos.Z()));
    fT.push_back((Float_t)t);
    fEnergy.push_back((Float_t)(en));
    fType.push_back(type);

    fTotEnergy += en;
}

void TRestHits::AddHit(TRestHits& hits, Int_t n) {
    Double_t x = hits.GetX(n);
    Double_t y = hits.GetY(n);
    Double_t z = hits.GetZ(n);
    Double_t en = hits.GetEnergy(n);
    Double_t t = hits.GetTime(n);
    REST_HitType type = hits.GetType(n);

    AddHit(x, y, z, en, t, type);
}

void TRestHits::RemoveHits() {
    fNHits = 0;
    fX.clear();
    fY.clear();
    fZ.clear();
    fT.clear();
    fEnergy.clear();
    fType.clear();
    fTotEnergy = 0;
}

void TRestHits::Translate(Int_t n, double x, double y, double z) {
    fX[n] += x;
    fY[n] += y;
    fZ[n] += z;
}

void TRestHits::RotateIn3D(Int_t n, Double_t alpha, Double_t beta, Double_t gamma, TVector3 vMean) {
    TVector3 position = GetPosition(n);
    TVector3 vHit = position - vMean;

    vHit.RotateZ(gamma);
    vHit.RotateY(beta);
    vHit.RotateX(alpha);

    fX[n] = vHit[0] + vMean[0];
    fY[n] = vHit[1] + vMean[1];
    fZ[n] = vHit[2] + vMean[2];
}

void TRestHits::Rotate(Int_t n, Double_t alpha, TVector3 vAxis, TVector3 vMean) {
    TVector3 vHit;

    vHit[0] = fX[n] - vMean[0];
    vHit[1] = fY[n] - vMean[1];
    vHit[2] = fZ[n] - vMean[2];

    vHit.Rotate(alpha, vAxis);

    fX[n] = vHit[0] + vMean[0];
    fY[n] = vHit[1] + vMean[1];
    fZ[n] = vHit[2] + vMean[2];
}

Double_t TRestHits::GetMaximumHitEnergy() {
    Double_t energy = 0;
    for (int i = 0; i < GetNumberOfHits(); i++)
        if (GetEnergy(i) > energy) energy = GetEnergy(i);
    return energy;
}

Double_t TRestHits::GetMinimumHitEnergy() {
    Double_t energy = GetMaximumHitEnergy();
    for (int i = 0; i < GetNumberOfHits(); i++)
        if (GetEnergy(i) < energy) energy = GetEnergy(i);
    return energy;
}

Double_t TRestHits::GetMeanHitEnergy() { return GetTotalEnergy() / GetNumberOfHits(); }

void TRestHits::MergeHits(int n, int m) {
    Double_t totalEnergy = fEnergy[n] + fEnergy[m];
    fX[n] = (fX[n] * fEnergy[n] + fX[m] * fEnergy[m]) / totalEnergy;
    fY[n] = (fY[n] * fEnergy[n] + fY[m] * fEnergy[m]) / totalEnergy;
    fZ[n] = (fZ[n] * fEnergy[n] + fZ[m] * fEnergy[m]) / totalEnergy;
    fT[n] = (fT[n] * fEnergy[n] + fT[m] * fEnergy[m]) / totalEnergy;
    fEnergy[n] += fEnergy[m];

    fX.erase(fX.begin() + m);
    fY.erase(fY.begin() + m);
    fZ.erase(fZ.begin() + m);
    fT.erase(fT.begin() + m);
    fEnergy.erase(fEnergy.begin() + m);
    fType.erase(fType.begin() + m);
    fNHits--;
}

void TRestHits::SwapHits(Int_t i, Int_t j) {
    iter_swap(fX.begin() + i, fX.begin() + j);
    iter_swap(fY.begin() + i, fY.begin() + j);
    iter_swap(fZ.begin() + i, fZ.begin() + j);
    iter_swap(fEnergy.begin() + i, fEnergy.begin() + j);
    iter_swap(fType.begin() + i, fType.begin() + j);
    iter_swap(fT.begin() + i, fT.begin() + j);
}

Bool_t TRestHits::isSortedByEnergy() {
    for (int i = 0; i < GetNumberOfHits() - 1; i++)
        if (GetEnergy(i + 1) > GetEnergy(i)) return false;

    return true;
}

void TRestHits::RemoveHit(int n) {
    fTotEnergy -= GetEnergy(n);
    fX.erase(fX.begin() + n);
    fY.erase(fY.begin() + n);
    fZ.erase(fZ.begin() + n);
    fT.erase(fT.begin() + n);
    fEnergy.erase(fEnergy.begin() + n);
    fType.erase(fType.begin() + n);
    fNHits--;
}

TVector3 TRestHits::GetPosition(int n) {
    if ((fType.size() == 0 ? !IsNaN(fX[n]) : fType[n] == XY))
        return TVector3(((Double_t)fX[n]), ((Double_t)fY[n]), 0);
    if ((fType.size() == 0 ? !IsNaN(fX[n]) : fType[n] == XZ))
        return TVector3(((Double_t)fX[n]), 0, ((Double_t)fZ[n]));
    if ((fType.size() == 0 ? !IsNaN(fX[n]) : fType[n] == YZ))
        return TVector3(0, ((Double_t)fY[n]), ((Double_t)fZ[n]));
    return TVector3(((Double_t)fX[n]), ((Double_t)fY[n]), ((Double_t)fZ[n]));
}

TVector3 TRestHits::GetVector(int i, int j) {
    TVector3 vector = GetPosition(i) - GetPosition(j);
    return vector;
}

Int_t TRestHits::GetNumberOfHitsX() {
    Int_t nHitsX = 0;

    for (int n = 0; n < GetNumberOfHits(); n++)
        if ((fType.size() == 0 ? !IsNaN(fX[n]) : fType[n] % X == 0)) nHitsX++;

    return nHitsX;
}

Int_t TRestHits::GetNumberOfHitsY() {
    Int_t nHitsY = 0;

    for (int n = 0; n < GetNumberOfHits(); n++)
        if ((fType.size() == 0 ? !IsNaN(fY[n]) : fType[n] % Y == 0)) nHitsY++;

    return nHitsY;
}

Double_t TRestHits::GetEnergyX() {
    Double_t totalEnergy = 0;
    for (int n = 0; n < GetNumberOfHits(); n++) {
        if ((fType.size() == 0 ? !IsNaN(fX[n]) : fType[n] % X == 0)) {
            totalEnergy += fEnergy[n];
        }
    }

    return totalEnergy;
}

Double_t TRestHits::GetEnergyY() {
    Double_t totalEnergy = 0;
    for (int n = 0; n < GetNumberOfHits(); n++) {
        if ((fType.size() == 0 ? !IsNaN(fY[n]) : fType[n] % Y == 0)) {
            totalEnergy += fEnergy[n];
        }
    }

    return totalEnergy;
}
Double_t TRestHits::GetMeanPositionX() {
    Double_t meanX = 0;
    Double_t totalEnergy = 0;
    for (int n = 0; n < GetNumberOfHits(); n++) {
        if ((fType.size() == 0 ? !IsNaN(fX[n]) : fType[n] % X == 0)) {
            meanX += fX[n] * fEnergy[n];
            totalEnergy += fEnergy[n];
        }
    }

    if (totalEnergy == 0) return 0;
    meanX /= totalEnergy;

    return meanX;
}

Double_t TRestHits::GetMeanPositionY() {
    Double_t meanY = 0;
    Double_t totalEnergy = 0;
    for (int n = 0; n < GetNumberOfHits(); n++) {
        if ((fType.size() == 0 ? !IsNaN(fY[n]) : fType[n] % Y == 0)) {
            meanY += fY[n] * fEnergy[n];
            totalEnergy += fEnergy[n];
        }
    }

    if (totalEnergy == 0) return 0;
    meanY /= totalEnergy;

    return meanY;
}

Double_t TRestHits::GetMeanPositionZ() {
    Double_t meanZ = 0;
    Double_t totalEnergy = 0;
    for (int n = 0; n < GetNumberOfHits(); n++) {
        if (!IsNaN(fZ[n])) {
            meanZ += fZ[n] * fEnergy[n];
            totalEnergy += fEnergy[n];
        }
    }

    if (totalEnergy == 0) return 0;
    meanZ /= totalEnergy;

    return meanZ;
}

TVector3 TRestHits::GetMeanPosition() {
    TVector3 mean(GetMeanPositionX(), GetMeanPositionY(), GetMeanPositionZ());
    return mean;
}

Double_t TRestHits::GetSigmaXY2() {
    Double_t sigmaXY2 = 0;
    Double_t totalEnergy = this->GetTotalEnergy();
    Double_t meanX = this->GetMeanPositionX();
    Double_t meanY = this->GetMeanPositionY();
    for (int n = 0; n < GetNumberOfHits(); n++) {
        if ((fType.size() == 0 ? !IsNaN(fY[n]) : fType[n] % Y == 0))
            sigmaXY2 += fEnergy[n] * (meanY - fY[n]) * (meanY - fY[n]);
        if ((fType.size() == 0 ? !IsNaN(fX[n]) : fType[n] % X == 0))
            sigmaXY2 += fEnergy[n] * (meanX - fX[n]) * (meanX - fX[n]);
    }
    return sigmaXY2 /= totalEnergy;
}

Double_t TRestHits::GetSigmaX() {
    Double_t sigmaX2 = 0;
    Double_t sigmaX = 0;
    Double_t totalEnergy = this->GetTotalEnergy();
    Double_t meanX = this->GetMeanPositionX();

    for (int n = 0; n < GetNumberOfHits(); n++) {
        if ((fType.size() == 0 ? !IsNaN(fX[n]) : fType[n] % X == 0))
            sigmaX2 += fEnergy[n] * (meanX - fX[n]) * (meanX - fX[n]);
    }
    sigmaX2 /= totalEnergy;

    return sigmaX = TMath::Sqrt(sigmaX2);
}

Double_t TRestHits::GetSigmaY() {
    Double_t sigmaY2 = 0;
    Double_t sigmaY = 0;
    Double_t totalEnergy = this->GetTotalEnergy();
    Double_t meanY = this->GetMeanPositionY();

    for (int n = 0; n < GetNumberOfHits(); n++) {
        if ((fType.size() == 0 ? !IsNaN(fY[n]) : fType[n] % Y == 0))
            sigmaY2 += fEnergy[n] * (meanY - fY[n]) * (meanY - fY[n]);
    }
    sigmaY2 /= totalEnergy;

    return sigmaY = TMath::Sqrt(sigmaY2);
}

Double_t TRestHits::GetGaussSigmaX(Int_t readoutChannels, Int_t startChannel, Int_t endChannel, Double_t pitch) {
	TH1::AddDirectory(kFALSE);
    Double_t gausSigmaX = 0;

	TH1D* hX = new TH1D("hitsGaussHistoX", "hitsGaussHistoX", readoutChannels, startChannel, endChannel);

    for (int n = 0; n < GetNumberOfHits(); n++) hX->Fill(fX[n], fEnergy[n]);

    TF1* fit = new TF1("", "gaus", hX->GetMaximumBin() / (1/pitch) - 32, hX->GetMaximumBin() / (1/pitch) - 26);

    hX->Fit(fit, "QNRL");  // Q = quiet, no info in screen; N = no plot; R = fit in the function range; L = log likelihood fit

    gausSigmaX = fit->GetParameter(2);

    delete fit;
	delete hX;

	TH1::AddDirectory(kTRUE);
    return gausSigmaX;
}

Double_t TRestHits::GetGaussSigmaY(Int_t readoutChannels, Int_t startChannel, Int_t endChannel, Double_t pitch) {
	TH1::AddDirectory(kFALSE);
    Double_t gausSigmaY = 0;

	TH1D* hY = new TH1D("hitsGaussHistoY", "hitsGaussHistoY", readoutChannels, startChannel, endChannel);

    for (int n = 0; n < GetNumberOfHits(); n++) hY->Fill(fY[n], fEnergy[n]);

    TF1* fit = new TF1("", "gaus", hY->GetMaximumBin() / (1/pitch) - 32, hY->GetMaximumBin() / (1/pitch) - 26);

    hY->Fit(fit, "QNRL");  // Q = quiet, no info in screen; N = no plot; R = fit in the function range; L = log likelihood fit

    gausSigmaY = fit->GetParameter(2);

    delete fit;
	delete hY;

	TH1::AddDirectory(kTRUE);
    return gausSigmaY;
}

Double_t TRestHits::GetSkewXY() {
    Double_t skewXY = 0;
    Double_t totalEnergy = this->GetTotalEnergy();
    Double_t sigmaXY = TMath::Sqrt(this->GetSigmaXY2());
    Double_t meanX = this->GetMeanPositionX();
    Double_t meanY = this->GetMeanPositionY();
    for (int n = 0; n < GetNumberOfHits(); n++) {
        if ((fType.size() == 0 ? !IsNaN(fY[n]) : fType[n] % Y == 0))
            skewXY += fEnergy[n] * (meanY - fY[n]) * (meanY - fY[n]) * (meanY - fY[n]);
        if ((fType.size() == 0 ? !IsNaN(fX[n]) : fType[n] % X == 0))
            skewXY += fEnergy[n] * (meanX - fX[n]) * (meanX - fX[n]) * (meanX - fX[n]);
    }
    return skewXY /= (totalEnergy * sigmaXY * sigmaXY * sigmaXY);
}

Double_t TRestHits::GetSigmaZ2() {
    Double_t sigmaZ2 = 0;
    Double_t totalEnergy = this->GetTotalEnergy();
    Double_t meanZ = this->GetMeanPositionZ();

    for (int n = 0; n < GetNumberOfHits(); n++) {
        if (!IsNaN(fZ[n])) sigmaZ2 += fEnergy[n] * (meanZ - fZ[n]) * (meanZ - fZ[n]);
    }
    return sigmaZ2 /= totalEnergy;
}

Double_t TRestHits::GetSkewZ() {
    Double_t skewZ = 0;
    Double_t totalEnergy = this->GetTotalEnergy();
    Double_t sigmaZ = TMath::Sqrt(this->GetSigmaZ2());
    Double_t meanZ = this->GetMeanPositionZ();

    for (int n = 0; n < GetNumberOfHits(); n++) {
        if (!IsNaN(fZ[n])) skewZ += fEnergy[n] * (meanZ - fZ[n]) * (meanZ - fZ[n]) * (meanZ - fZ[n]);
    }
    return skewZ /= (totalEnergy * sigmaZ * sigmaZ * sigmaZ);
}

Double_t TRestHits::GetMeanPositionXInPrism(TVector3 x0, TVector3 x1, Double_t sizeX, Double_t sizeY,
                                            Double_t theta) {
    Double_t meanX = 0;
    Double_t totalEnergy = 0;
    for (int n = 0; n < GetNumberOfHits(); n++) {
        if (((fType.size() == 0 ? !IsNaN(fX[n]) : fType[n] % X == 0) &&
             (isHitNInsidePrism(n, x0, x1, sizeX, sizeY, theta)))) {
            meanX += fX[n] * fEnergy[n];
            totalEnergy += fEnergy[n];
        }
    }

    meanX /= totalEnergy;

    return meanX;
}

Double_t TRestHits::GetMeanPositionYInPrism(TVector3 x0, TVector3 x1, Double_t sizeX, Double_t sizeY,
                                            Double_t theta) {
    Double_t meanY = 0;
    Double_t totalEnergy = 0;
    for (int n = 0; n < GetNumberOfHits(); n++) {
        if (((fType.size() == 0 ? !IsNaN(fY[n]) : fType[n] % Y == 0) &&
             (isHitNInsidePrism(n, x0, x1, sizeX, sizeY, theta)))) {
            meanY += fY[n] * fEnergy[n];
            totalEnergy += fEnergy[n];
        }
    }

    meanY /= totalEnergy;

    return meanY;
}
Double_t TRestHits::GetMeanPositionZInPrism(TVector3 x0, TVector3 x1, Double_t sizeX, Double_t sizeY,
                                            Double_t theta) {
    Double_t meanZ = 0;
    Double_t totalEnergy = 0;
    for (int n = 0; n < GetNumberOfHits(); n++) {
        if ((!IsNaN(fZ[n]) && (isHitNInsidePrism(n, x0, x1, sizeX, sizeY, theta)))) {
            meanZ += fZ[n] * fEnergy[n];
            totalEnergy += fEnergy[n];
        }
    }

    meanZ /= totalEnergy;

    return meanZ;
}

TVector3 TRestHits::GetMeanPositionInPrism(TVector3 x0, TVector3 x1, Double_t sizeX, Double_t sizeY,
                                           Double_t theta) {
    TVector3 mean(GetMeanPositionXInPrism(x0, x1, sizeX, sizeY, theta),
                  GetMeanPositionYInPrism(x0, x1, sizeX, sizeY, theta),
                  GetMeanPositionZInPrism(x0, x1, sizeX, sizeY, theta));
    return mean;
}

Double_t TRestHits::GetMeanPositionXInCylinder(TVector3 x0, TVector3 x1, Double_t radius) {
    Double_t meanX = 0;
    Double_t totalEnergy = 0;
    for (int n = 0; n < GetNumberOfHits(); n++) {
        if (((fType.size() == 0 ? !IsNaN(fX[n]) : fType[n] % X == 0) &&
             (isHitNInsideCylinder(n, x0, x1, radius)))) {
            meanX += fX[n] * fEnergy[n];
            totalEnergy += fEnergy[n];
        }
    }

    meanX /= totalEnergy;

    return meanX;
}

Double_t TRestHits::GetMeanPositionYInCylinder(TVector3 x0, TVector3 x1, Double_t radius) {
    Double_t meanY = 0;
    Double_t totalEnergy = 0;
    for (int n = 0; n < GetNumberOfHits(); n++) {
        if (((fType.size() == 0 ? !IsNaN(fY[n]) : fType[n] % Y == 0) &&
             (isHitNInsideCylinder(n, x0, x1, radius)))) {
            meanY += fY[n] * fEnergy[n];
            totalEnergy += fEnergy[n];
        }
    }

    meanY /= totalEnergy;

    return meanY;
}

Double_t TRestHits::GetMeanPositionZInCylinder(TVector3 x0, TVector3 x1, Double_t radius) {
    Double_t meanZ = 0;
    Double_t totalEnergy = 0;
    for (int n = 0; n < GetNumberOfHits(); n++) {
        if ((!IsNaN(fZ[n]) && (isHitNInsideCylinder(n, x0, x1, radius)))) {
            meanZ += fZ[n] * fEnergy[n];
            totalEnergy += fEnergy[n];
        }
    }

    meanZ /= totalEnergy;

    return meanZ;
}

TVector3 TRestHits::GetMeanPositionInCylinder(TVector3 x0, TVector3 x1, Double_t radius) {
    TVector3 mean(GetMeanPositionXInCylinder(x0, x1, radius), GetMeanPositionYInCylinder(x0, x1, radius),
                  GetMeanPositionZInCylinder(x0, x1, radius));
    return mean;
}

Double_t TRestHits::GetHitsPathLength(Int_t n, Int_t m) {
    if (n < 0) n = 0;
    if (m > GetNumberOfHits() - 1) m = GetNumberOfHits() - 1;

    Double_t distance = 0;
    for (int i = n; i < m; i++) distance += TMath::Sqrt(GetDistance2(i, i + 1));
    return distance;
}

Double_t TRestHits::GetTotalDistance() {
    Double_t distance = 0;
    for (int i = 0; i < GetNumberOfHits() - 1; i++) distance += TMath::Sqrt(GetDistance2(i, i + 1));
    return distance;
}

Double_t TRestHits::GetDistance2(int n, int m) {
    Double_t dx = GetX(n) - GetX(m);
    Double_t dy = GetY(n) - GetY(m);
    Double_t dz = GetZ(n) - GetZ(m);

    if (areXY()) return dx * dx + dy * dy;
    if (areXZ()) return dx * dx + dz * dz;
    if (areYZ()) return dy * dy + dz * dz;

    return dx * dx + dy * dy + dz * dz;
}

Double_t TRestHits::GetDistanceToNode(Int_t n) {
    Double_t distance = 0;
    if (n > GetNumberOfHits() - 1) n = GetNumberOfHits() - 1;

    for (int hit = 0; hit < n; hit++) distance += GetVector(hit + 1, hit).Mag();

    return distance;
}

Int_t TRestHits::GetMostEnergeticHitInRange(Int_t n, Int_t m) {
    Int_t maxEn = 0;
    Int_t hit = -1;
    for (int i = n; i < m; i++) {
        if (this->GetEnergy(i) > maxEn) {
            maxEn = this->GetEnergy(i);
            hit = i;
        }
    }
    // if (hit == -1) cout << "REST warning : No largest hit found! No hits?" << endl;
    return hit;
}

Int_t TRestHits::GetClosestHit(TVector3 position) {
    Int_t closestHit = 0;

    Double_t minDistance = 1.e30;
    for (int nHit = 0; nHit < GetNumberOfHits(); nHit++) {
        TVector3 vector = position - GetPosition(nHit);

        Double_t distance = vector.Mag2();
        if (distance < minDistance) {
            closestHit = nHit;
            minDistance = distance;
        }
    }

    return closestHit;
}

TVector2 TRestHits::GetProjection(Int_t n, Int_t m, TVector3 position) {
    TVector3 nodesSegment = this->GetVector(n, m);

    TVector3 origin = position - this->GetPosition(m);

    if (origin == TVector3(0, 0, 0)) return TVector2(0, 0);

    Double_t longitudinal = nodesSegment.Unit().Dot(origin);
    if (origin == nodesSegment) return TVector2(longitudinal, 0);

    Double_t transversal = TMath::Sqrt(origin.Mag2() - longitudinal * longitudinal);

    return TVector2(longitudinal, transversal);
}

Double_t TRestHits::GetTransversalProjection(TVector3 p0, TVector3 direction, TVector3 position) {
    TVector3 oX = position - p0;

    if (oX == TVector3(0, 0, 0)) return 0;

    Double_t longitudinal = direction.Unit().Dot(oX);

    return TMath::Sqrt(oX.Mag2() - longitudinal * longitudinal);
}

Double_t TRestHits::GetHitsTwist(Int_t n, Int_t m) {
    if (n < 0) n = 0;
    if (m == 0) m = this->GetNumberOfHits();

    if (m - n < 2) return 0;

    Double_t sum = 0;
    Int_t cont = 0;
    for (int N = n + 1; N < m - 1; N++) {
        TVector3 a = (this->GetPosition(N - 1) - this->GetPosition(N + 1)).Unit();
        TVector3 b = (this->GetPosition(N) - this->GetPosition(N + 1)).Unit();

        sum += (1 - a.Dot(b)) / 2.;
        cont++;
    }

    if (cont == 0) return -1;

    return sum / cont;
}

Double_t TRestHits::GetHitsTwistWeighted(Int_t n, Int_t m) {
    if (n < 0) n = 0;
    if (m == 0) m = this->GetNumberOfHits();

    if (m - n < 2) return 0;

    Double_t sum = 0;
    Int_t cont = 0;
    for (int N = n + 1; N < m - 1; N++) {
        TVector3 a = (this->GetPosition(N - 1) - this->GetPosition(N)).Unit();
        TVector3 b = (this->GetPosition(N) - this->GetPosition(N + 1)).Unit();

        Double_t weight = TMath::Abs(this->GetEnergy(N - 1) - this->GetEnergy(N));
        weight += TMath::Abs(this->GetEnergy(N) - this->GetEnergy(N + 1));

        sum += weight * (1 - a.Dot(b)) / 2.;
        cont++;
    }

    if (cont == 0) return -1;

    return sum / cont;
}

Double_t TRestHits::GetMaximumHitDistance() {
    Double_t maxDistance = 0;
    for (int n = 0; n < this->GetNumberOfHits(); n++)
        for (int m = n + 1; m < this->GetNumberOfHits(); m++) {
            Double_t d = this->GetDistance(n, m);
            if (d > maxDistance) maxDistance = d;
        }

    return maxDistance;
}

Double_t TRestHits::GetMaximumHitDistance2() {
    Double_t maxDistance = 0;
    for (int n = 0; n < this->GetNumberOfHits(); n++)
        for (int m = n + 1; m < this->GetNumberOfHits(); m++) {
            Double_t d = this->GetDistance2(n, m);
            if (d > maxDistance) maxDistance = d;
        }

    return maxDistance;
}

void TRestHits::PrintHits(Int_t nHits) {
    Int_t N = nHits;

    if (N == -1) N = GetNumberOfHits();
    if (N > GetNumberOfHits()) N = GetNumberOfHits();

    for (int n = 0; n < N; n++) {
        cout << "Hit " << n << " X: " << GetX(n) << " Y: " << GetY(n) << " Z: " << GetZ(n)
             << " Energy: " << GetEnergy(n) << " T: " << GetTime(n);
        cout << endl;
    }
}

///////////////////////
// Iterator methods

TRestHits::TRestHits_Iterator::TRestHits_Iterator(TRestHits* h, int _index) {
    fHits = h;
    index = _index;
    maxindex = fHits->GetNumberOfHits();
    if (index < 0) index = 0;
    if (index >= maxindex) index = maxindex;
}

void TRestHits::TRestHits_Iterator::toaccessor() {
    _x = x();
    _y = y();
    _z = z();
    _t = t();
    _e = e();
    _type = type();
    isaccessor = true;
}

TRestHits::TRestHits_Iterator TRestHits::TRestHits_Iterator::operator*() {
    TRestHits_Iterator i(*this);
    i.toaccessor();
    return i;
}

TRestHits::TRestHits_Iterator& TRestHits::TRestHits_Iterator::operator++() {
    index++;
    if (index >= maxindex) index = maxindex;
    return *this;
}

TRestHits::TRestHits_Iterator& TRestHits::TRestHits_Iterator::operator+=(const int& n) {
    if (index + n >= maxindex) {
        index = maxindex;
    } else {
        index += n;
    }
    return *this;
}

TRestHits::TRestHits_Iterator TRestHits::TRestHits_Iterator::operator+(const int& n) {
    if (index + n >= maxindex) {
        return TRestHits_Iterator(fHits, maxindex);
    } else {
        return TRestHits_Iterator(fHits, index + n);
    }
}

TRestHits::TRestHits_Iterator& TRestHits::TRestHits_Iterator::operator--() {
    index--;
    if (index <= 0) index = 0;
    return *this;
}

TRestHits::TRestHits_Iterator& TRestHits::TRestHits_Iterator::operator-=(const int& n) {
    if (index - n <= 0) {
        index = 0;
    } else {
        index -= n;
    }
    return *this;
}

TRestHits::TRestHits_Iterator TRestHits::TRestHits_Iterator::operator-(const int& n) {
    if (index - n <= 0) {
        return TRestHits_Iterator(fHits, 0);
    } else {
        return TRestHits_Iterator(fHits, index - n);
    }
}

TRestHits::TRestHits_Iterator& TRestHits::TRestHits_Iterator::operator=(const TRestHits_Iterator& iter) {
    if (isaccessor) {
        (fHits ? fHits->fX[index] : x()) = iter.x();
        (fHits ? fHits->fY[index] : y()) = iter.y();
        (fHits ? fHits->fZ[index] : z()) = iter.z();
        (fHits ? fHits->fEnergy[index] : e()) = iter.e();
        (fHits ? fHits->fT[index] : t()) = iter.t();
        (fHits ? fHits->fType[index] : type()) = iter.type();
    } else {
        fHits = iter.fHits;
        index = iter.index;
    }
    return *this;
}
