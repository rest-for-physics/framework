///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestVolumeHits.h
///
///             Event class to store signals form simulation and acquisition
///             events
///
///             oct 2015:   First concept
///                 Created as part of the conceptualization of existing REST
///                 software.
///                 Javier Gracia
///_______________________________________________________________________________

#include "TRestVolumeHits.h"

using namespace std;

ClassImp(TRestVolumeHits);

TRestVolumeHits::TRestVolumeHits() {
    // TRestVolumeHits default constructor
}

TRestVolumeHits::~TRestVolumeHits() {
    // TRestVolumeHits destructor
}

void TRestVolumeHits::AddHit(Double_t x, Double_t y, Double_t z, Double_t en, Double_t time,
                             REST_HitType type, Double_t sigmaX, Double_t sigmaY, Double_t sigmaZ) {
    if (fType.size() > 0 && type != fType[0]) {
        cout << "Error! Cannot add different typed hits into TRestVolumeHits!" << endl;
        return;
    }

    TRestHits::AddHit(x, y, z, en, time, type);
    fSigmaX.push_back((Float_t)sigmaX);
    fSigmaY.push_back((Float_t)sigmaY);
    fSigmaZ.push_back((Float_t)sigmaZ);
}

void TRestVolumeHits::AddHit(const TVector3& pos, Double_t en, Double_t time, REST_HitType type,
                             const TVector3& sigma) {
    if (fType.size() > 0 && type != fType[0]) {
        cout << "Error! Cannot add different typed hits into TRestVolumeHits!" << endl;
        return;
    }

    TRestHits::AddHit(pos, en, time, type);
    fSigmaX.push_back((Float_t)sigma.X());
    fSigmaY.push_back((Float_t)sigma.Y());
    fSigmaZ.push_back((Float_t)sigma.Z());
}

void TRestVolumeHits::AddHit(const TRestVolumeHits& hits, Int_t n) {
    Double_t x = hits.GetX(n);
    Double_t y = hits.GetY(n);
    Double_t z = hits.GetZ(n);
    Double_t t = hits.GetTime(n);
    Double_t en = hits.GetEnergy(n);
    REST_HitType type = hits.GetType(n);

    Double_t sx = hits.GetSigmaX(n);
    Double_t sy = hits.GetSigmaY(n);
    Double_t sz = hits.GetSigmaZ(n);

    if (fType.size() > 0 && type != fType[0]) {
        cout << "Error! Cannot add different typed hits into TRestVolumeHits!" << endl;
        return;
    }

    AddHit(x, y, z, en, t, type, sx, sy, sz);
}

void TRestVolumeHits::RemoveHits() {
    // cout << "Removing hits" << endl;
    TRestHits::RemoveHits();
    fSigmaX.clear();
    fSigmaY.clear();
    fSigmaZ.clear();
}

Bool_t TRestVolumeHits::areXY() const {
    if (fType.size() == 0)
        return TMath::IsNaN(fZ[0]) && !TMath::IsNaN(fX[0]) && !TMath::IsNaN(fY[0]);
    else
        return fType[0] == XY;
}
Bool_t TRestVolumeHits::areXZ() const {
    if (fType.size() == 0)
        return !TMath::IsNaN(fZ[0]) && !TMath::IsNaN(fX[0]) && TMath::IsNaN(fY[0]);
    else
        return fType[0] == XZ;
}
Bool_t TRestVolumeHits::areYZ() const {
    if (fType.size() == 0)
        return !TMath::IsNaN(fZ[0]) && TMath::IsNaN(fX[0]) && !TMath::IsNaN(fY[0]);
    else
        return fType[0] == YZ;
}
Bool_t TRestVolumeHits::areXYZ() const {
    if (fType.size() == 0)
        return !TMath::IsNaN(fZ[0]) && !TMath::IsNaN(fX[0]) && !TMath::IsNaN(fY[0]);
    else
        return fType[0] == XYZ;
}

void TRestVolumeHits::MergeHits(Int_t n, Int_t m) {
    Double_t totalEnergy = fEnergy[n] + fEnergy[m];

    // TODO : This is wrong but not very important for the moment
    fSigmaX[n] = (fSigmaX[n] * fEnergy[n] + fSigmaX[m] * fEnergy[m]) / totalEnergy;
    fSigmaY[n] = (fSigmaY[n] * fEnergy[n] + fSigmaY[m] * fEnergy[m]) / totalEnergy;
    fSigmaZ[n] = (fSigmaZ[n] * fEnergy[n] + fSigmaZ[m] * fEnergy[m]) / totalEnergy;

    fSigmaX.erase(fSigmaX.begin() + m);
    fSigmaY.erase(fSigmaY.begin() + m);
    fSigmaZ.erase(fSigmaZ.begin() + m);

    TRestHits::MergeHits(n, m);
}

void TRestVolumeHits::RemoveHit(int n) {
    TRestHits::RemoveHit(n);

    fSigmaX.erase(fSigmaX.begin() + n);
    fSigmaY.erase(fSigmaY.begin() + n);
    fSigmaZ.erase(fSigmaZ.begin() + n);
}

void TRestVolumeHits::SortByEnergy() {
    while (!isSortedByEnergy()) {
        for (int i = 0; i < GetNumberOfHits(); i++) {
            for (int j = i + 1; j < GetNumberOfHits(); j++) {
                if (GetEnergy(i) < GetEnergy(j)) SwapHits(i, j);
            }
        }
    }
}

void TRestVolumeHits::SwapHits(Int_t i, Int_t j) {
    iter_swap(fSigmaX.begin() + i, fSigmaX.begin() + j);
    iter_swap(fSigmaY.begin() + i, fSigmaY.begin() + j);
    iter_swap(fSigmaZ.begin() + i, fSigmaZ.begin() + j);

    TRestHits::SwapHits(i, j);
}

TVector3 TRestVolumeHits::GetSigma(int n) const {
    return TVector3(((Double_t)fSigmaX[n]), ((Double_t)fSigmaY[n]), ((Double_t)fSigmaZ[n]));
}

void TRestVolumeHits::PrintHits() const {
    for (int n = 0; n < GetNumberOfHits(); n++) {
        cout << "Hit " << n << " X: " << GetX(n) << " Y: " << GetY(n) << " Z: " << GetZ(n)
             << " sX: " << GetSigmaX(n) << " sY: " << GetSigmaY(n) << " sZ: " << GetSigmaZ(n)
             << " Energy: " << GetEnergy(n) << endl;
    }
}

void TRestVolumeHits::kMeansClustering(TRestVolumeHits* hits, TRestVolumeHits& vHits, int maxIt) {
    const int nodes = vHits.GetNumberOfHits();
    vector<TRestVolumeHits> volHits(nodes);
    // std::cout<<"Nhits "<<hits->GetNumberOfHits()<<" Nodes "<<nodes<<std::endl;
    TVector3 nullVector = TVector3(0, 0, 0);
    std::vector<TVector3> centroid(nodes);
    std::vector<TVector3> centroidOld(nodes, nullVector);

    for (int h = 0; h < nodes; h++) centroid[h] = vHits.GetPosition(h);

    for (int it = 0; it < maxIt; it++) {
        for (int n = 0; n < nodes; n++) volHits[n].RemoveHits();
        for (int i = 0; i < hits->GetNumberOfHits(); i++) {
            double minDist = 1E9;
            int clIndex = -1;
            for (int n = 0; n < nodes; n++) {
                TVector3 hitPos = hits->GetPosition(i);
                double dist = (centroid[n] - hitPos).Mag();
                if (dist < minDist) {
                    minDist = dist;
                    clIndex = n;
                }
            }
            // cout<<minDist<<" "<<clIndex<<endl;
            volHits[clIndex].AddHit(*hits, i);
        }
        bool converge = true;
        for (int n = 0; n < nodes; n++) {
            centroid[n] = volHits[n].GetMeanPosition();
            converge &= (centroid[n] == centroidOld[n]);
            centroidOld[n] = centroid[n];
        }
        if (converge) {
            break;
        }
    }

    vHits.RemoveHits();
    const TVector3 sigma(0., 0., 0.);
    for (int n = 0; n < nodes; n++) {
        if (volHits[n].GetNumberOfHits() > 0)
            vHits.AddHit(volHits[n].GetMeanPosition(), volHits[n].GetTotalEnergy(), 0, volHits[n].GetType(0),
                         sigma);
    }
}
