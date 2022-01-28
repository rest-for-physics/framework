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
#include "TF1.h"
#include "TH1.h"
#include "TGraphErrors.h"
#include "TCanvas.h"

using namespace std;
using namespace TMath;

ClassImp(TRestHits);

TRestHits::TRestHits() {
    fTotEnergy = 0;
}

TRestHits::~TRestHits() {}

Bool_t TRestHits::areXY() {
    for (const auto &h : fHits) {
        if (h.fType != "XY") {
            // all hits should fit this condition to be considered XY
            return false;
        }
    }
    return true;
}

Bool_t TRestHits::areXZ() {
    for (const auto &h : fHits) {
        if (h.fType != "XZ") {
            // all hits should fit this condition to be considered XY
            return false;
        }
    }
    return true;
}

Bool_t TRestHits::areYZ() {
     for (const auto &h : fHits) {
        if (h.fType != "YZ") {
            // all hits should fit this condition to be considered XY
            return false;
        }
    }
    return true;
}

Bool_t TRestHits::areXYZ() {
    for (const auto &h : fHits) {
        if (h.fType != "XYZ") {
            // all hits should fit this condition to be considered XY
            return false;
        }
    }
    return true;
}

Bool_t TRestHits::isNaN(Int_t n) {
    if (IsNaN(GetX(n)) && IsNaN(GetY(n)) && IsNaN(GetZ(n))) return true;
    return false;
}

Double_t TRestHits::GetEnergyIntegral() {
    Double_t sum = 0;
    for (auto &h : fHits) sum += h.fEnergy;
    return sum;
}

Bool_t TRestHits::isHitNInsidePrism(Int_t n, TVector3 x0, TVector3 x1, Double_t sizeX, Double_t sizeY, Double_t theta) {
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

void TRestHits::AddHit(const Double_t &x, const Double_t &y, const Double_t &z, const Double_t &en, Double_t t, TString type) {
    TRestHit hit (x,y,z,en,t,type);
    AddHit(hit);
}

void TRestHits::AddHit(const TVector3 &pos, const Double_t &en, Double_t t, TString type) {
    AddHit(pos.X(),pos.Y(),pos.Z(),en,t,type);
}

void TRestHits::AddHit (const TRestHit &hit){
  fHits.emplace_back(hit);
  fTotEnergy += hit.fEnergy;

}

void TRestHits::AddHit(TRestHits& hits, Int_t n) {
    TRestHit hit =hits.GetHit(n); 
    AddHit(hit);
}

void TRestHits::RemoveHits() {
    fHits.clear();
    fTotEnergy = 0;
}

void TRestHits::Translate(Int_t n, double x, double y, double z) {

    fHits.at(n).fX += x;
    fHits.at(n).fY += y;
    fHits.at(n).fZ += z;
}

void TRestHits::RotateIn3D(Int_t n, Double_t alpha, Double_t beta, Double_t gamma, TVector3 vMean) {
    TVector3 position = GetPosition(n);
    TVector3 vHit = position - vMean;

    vHit.RotateZ(gamma);
    vHit.RotateY(beta);
    vHit.RotateX(alpha);

    fHits.at(n).fX = vHit[0] + vMean[0];
    fHits.at(n).fY = vHit[1] + vMean[1];
    fHits.at(n).fZ = vHit[2] + vMean[2];
}

void TRestHits::Rotate(Int_t n, Double_t alpha, TVector3 vAxis, TVector3 vMean) {
    TVector3 vHit;

    vHit[0] = fHits.at(n).fX - vMean[0];
    vHit[1] = fHits.at(n).fY - vMean[1];
    vHit[2] = fHits.at(n).fZ - vMean[2];

    vHit.Rotate(alpha, vAxis);

    fHits.at(n).fX = vHit[0] + vMean[0];
    fHits.at(n).fY = vHit[1] + vMean[1];
    fHits.at(n).fZ = vHit[2] + vMean[2];
}

Double_t TRestHits::GetMaximumHitEnergy() {
    Double_t energy = 0;
    for (const auto &h : fHits)
        if (h.fEnergy > energy) energy = h.fEnergy;
    return energy;
}

Double_t TRestHits::GetMinimumHitEnergy() {
    Double_t energy = GetMaximumHitEnergy();
    for (const auto &h : fHits)
        if (h.fEnergy < energy) energy = h.fEnergy;
    return energy;
}

Double_t TRestHits::GetMeanHitEnergy() { return GetTotalEnergy() / GetNumberOfHits(); }

void TRestHits::MergeHits(int n, int m) {
    Double_t totalEnergy = fHits.at(n).fEnergy + fHits.at(m).fEnergy;
    fHits.at(n).fX = (fHits.at(n).fX * fHits.at(n).fEnergy + fHits.at(m).fX * fHits.at(m).fEnergy) / totalEnergy;
    fHits.at(n).fY = (fHits.at(n).fY * fHits.at(n).fEnergy + fHits.at(m).fY * fHits.at(m).fEnergy) / totalEnergy;
    fHits.at(n).fZ = (fHits.at(n).fZ * fHits.at(n).fEnergy + fHits.at(m).fZ * fHits.at(m).fEnergy) / totalEnergy;
    fHits.at(n).fT = (fHits.at(n).fT * fHits.at(n).fEnergy + fHits.at(m).fT * fHits.at(m).fEnergy) / totalEnergy;
    fHits.at(n).fEnergy += fHits.at(m).fEnergy;

    fHits.erase(fHits.begin() + m);
}

void TRestHits::SwapHits(Int_t i, Int_t j) {
    swap(fHits[i],fHits[j]);
}

Bool_t TRestHits::isSortedByEnergy() {
    for (int i = 0; i < GetNumberOfHits() - 1; i++)
        if (GetEnergy(i + 1) > GetEnergy(i)) return false;

    return true;
}

void TRestHits::RemoveHit(int n) {
    fTotEnergy -= GetEnergy(n);
    fHits.erase(fHits.begin() + n);
}

TVector3 TRestHits::GetPosition(int n) {
    if ( fHits.at(n).fType == "XY")
        return TVector3(fHits.at(n).fX, fHits.at(n).fY, 0);
    else if (fHits.at(n).fType == "XZ")
        return TVector3(fHits.at(n).fX, 0, fHits.at(n).fZ);
    else if (fHits.at(n).fType == "YZ")
        return TVector3(0, fHits.at(n).fY, fHits.at(n).fZ);

    return TVector3(fHits.at(n).fX, fHits.at(n).fY, fHits.at(n).fZ);
}

TVector3 TRestHits::GetVector(int i, int j) {
    TVector3 vector = GetPosition(i) - GetPosition(j);
    return vector;
}

Int_t TRestHits::GetNumberOfHitsX() {
    Int_t nHitsX = 0;

    for (const auto &h : fHits)
        if (h.fType.Contains("X")) nHitsX++;

    return nHitsX;
}

Int_t TRestHits::GetNumberOfHitsY() {
    Int_t nHitsY = 0;

    for (const auto &h : fHits)
        if (h.fType.Contains("Y")) nHitsY++;

    return nHitsY;
}

Double_t TRestHits::GetEnergyX() {
    Double_t totalEnergy = 0;
    for (const auto &h : fHits)
        if (h.fType.Contains("X")) totalEnergy += h.fEnergy;

    return totalEnergy;
}

Double_t TRestHits::GetEnergyY() {
    Double_t totalEnergy = 0;
    for (const auto &h : fHits)
        if (h.fType.Contains("Y")) totalEnergy += h.fEnergy;

    return totalEnergy;
}
Double_t TRestHits::GetMeanPositionX() {
    Double_t meanX = 0;
    Double_t totalEnergy = 0;
    for (const auto &h : fHits){
        if (h.fType.Contains("X")) {
            meanX += h.fX * h.fEnergy;
            totalEnergy += h.fEnergy;
        }
    }

    if (totalEnergy == 0) return 0;
    meanX /= totalEnergy;

    return meanX;
}

Double_t TRestHits::GetMeanPositionY() {
    Double_t meanY = 0;
    Double_t totalEnergy = 0;
    for (const auto &h : fHits){
        if (h.fType.Contains("Y") == 0) {
            meanY += h.fY * h.fEnergy;
            totalEnergy += h.fEnergy;
        }
    }

    if (totalEnergy == 0) return 0;
    meanY /= totalEnergy;

    return meanY;
}

Double_t TRestHits::GetMeanPositionZ() {
    Double_t meanZ = 0;
    Double_t totalEnergy = 0;
    for (const auto &h : fHits){
        if (h.fType.Contains("Z")) {
            meanZ += h.fZ * h.fEnergy;
            totalEnergy += h.fEnergy;
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
    Double_t totalEnergy = GetTotalEnergy();
    Double_t meanX = GetMeanPositionX();
    Double_t meanY = GetMeanPositionY();
    for (const auto &h : fHits){
        if (h.fType.Contains("X"))
            sigmaXY2 += h.fEnergy * (meanY - h.fY) * (meanY - h.fY);
        if (h.fType.Contains("Y"))
            sigmaXY2 += h.fEnergy * (meanX - h.fX) * (meanX - h.fX);
    }
    return sigmaXY2 /= totalEnergy;
}

Double_t TRestHits::GetSigmaX() {
    Double_t sigmaX2 = 0;
    Double_t sigmaX = 0;
    Double_t totalEnergy = 0;
    Double_t meanX = GetMeanPositionX();

    for (const auto &h : fHits){
        if (h.fType.Contains("X")) {
            sigmaX2 += h.fEnergy * (meanX - h.fX) * (meanX - h.fX);
            totalEnergy += h.fEnergy;
        }
    }

    if(totalEnergy>0)
      sigmaX2 /= totalEnergy;

    return sigmaX = TMath::Sqrt(sigmaX2);
}

Double_t TRestHits::GetSigmaY() {
    Double_t sigmaY2 = 0;
    Double_t sigmaY = 0;
    Double_t totalEnergy = 0;
    Double_t meanY = GetMeanPositionY();

    for (const auto &h : fHits){
        if (h.fType.Contains("Y")) {
            sigmaY2 += h.fEnergy * (meanY - h.fY) * (meanY - h.fY);
            totalEnergy += h.fEnergy;
        }
    }

    if(totalEnergy>0)
      sigmaY2 /= totalEnergy;

    return sigmaY = TMath::Sqrt(sigmaY2);
}

void TRestHits::WriteHitsToTextFile(TString filename) {
    FILE* fff = fopen(filename.Data(), "w");
    int i=0;
    for (const auto &h : fHits){
        if (h.fType.Contains("X"))
            fprintf(fff, "%d\t%e\t%s\t%e\t%e\n", i, h.fX, "NaN", h.fZ, h.fEnergy);
        if (h.fType.Contains("Y"))		
            fprintf(fff, "%d\t%s\t%e\t%e\t%e\n", i, "NaN", h.fY, h.fZ, h.fEnergy);
      i++;
    }
    fclose(fff);
}

Double_t TRestHits::GetGaussSigmaX() {
	Double_t gausSigmaX = 0;
	Int_t nHits = GetNumberOfHits();
	Double_t x[nHits], y[nHits], ex[nHits], ey[nHits];
	if (nHits <= 3) {	
		gausSigmaX = 0;
	} else {
	      int n=0;
		for (const auto &h : fHits){
		  x[n] = h.fX;
		  y[n] = h.fEnergy;
		  ex[n] = 0;
		    if (y[n] != 0) {
		      ey[n] = 10*sqrt(y[n]);		
		    } else {
		      ey[n] = 0; 
		    }
		  n++;
		}
		TGraphErrors *grX = new TGraphErrors(nHits,x,y,ex,ey);
		Double_t maxY =  MaxElement(nHits,grX->GetY());
		Double_t maxX = grX->GetX()[LocMax(nHits,grX->GetY())];

		TF1 *fit = new TF1("","gaus");
		fit->SetParameter(0,maxY);
		fit->SetParameter(1,maxX);
		fit->SetParameter(2, 2.0);
		grX->Fit(fit, "QNB");  // Q = quiet, no info in screen; N = no plot; B = no automatic start parmaeters; R = Use the Range specified in the function range

		gausSigmaX = fit->GetParameter(2);

	}

	return gausSigmaX;
}

Double_t TRestHits::GetGaussSigmaY() {
	Double_t gausSigmaY = 0;
	Int_t nHits = GetNumberOfHits();

	Double_t x[nHits], y[nHits], ex[nHits], ey[nHits];
	if (nHits <= 3) {	
		gausSigmaY = 0;
	} else {
	      int n=0;
		for (const auto &h : fHits){
		  x[n] = h.fY;
		  y[n] = h.fEnergy;
		  ex[n] = 0;
		    if (y[n] != 0) {
		      ey[n] = 10*sqrt(y[n]);		
		    } else {
		      ey[n] = 0; 
		    }
		  n++;
		}	
		TGraphErrors *grY = new TGraphErrors(nHits,x,y,ex,ey);
		Double_t maxY =  MaxElement(nHits,grY->GetY());
		Double_t maxX = grY->GetX()[LocMax(nHits,grY->GetY())];
		   
		TF1 *fit = new TF1("","gaus");
		fit->SetParameter(0,maxY);
		fit->SetParameter(1,maxX);
		fit->SetParameter(2, 2.0);
		grY->Fit(fit, "QNB");  // Q = quiet, no info in screen; N = no plot; B = no automatic start parmaeters; R = Use the Range specified in the function range

		gausSigmaY = fit->GetParameter(2);
	}
	return gausSigmaY;
}

Double_t TRestHits::GetGaussSigmaZ() {
	Double_t gausSigmaZ = 0;
	Int_t nHits = GetNumberOfHits();

	Double_t x[nHits], y[nHits], ex[nHits], ey[nHits];
	if (nHits <= 3) {	
		gausSigmaZ = 0;
	} else {
	      int n=0;
		for (const auto &h : fHits){
		  x[n] = h.fZ;
		  y[n] = h.fEnergy;
		  ex[n] = 0;
		    if (y[n] != 0) {
		      ey[n] = 10*sqrt(y[n]);		
		    } else {
		      ey[n] = 0; 
		    }
		  n++;
		}
		TGraphErrors *grZ = new TGraphErrors(nHits,x,y,ex,ey);
		Double_t maxY =  MaxElement(nHits,grZ->GetY());
		Double_t maxX = grZ->GetX()[LocMax(nHits,grZ->GetY())];

		TF1 *fit = new TF1("","gaus",maxX-5,maxX+5);
		fit->SetParameter(0,maxY);
		fit->SetParameter(1,maxX);
		fit->SetParameter(2, 2.0);
		grZ->Fit(fit, "QNB");  // Q = quiet, no info in screen; N = no plot; B = no automatic start parmaeters; R = Use the Range specified in the function range

		gausSigmaZ = fit->GetParameter(2);
	}
	return gausSigmaZ;
}

Double_t TRestHits::GetSkewXY() {
    Double_t skewXY = 0;
    Double_t totalEnergy = GetTotalEnergy();
    Double_t sigmaXY = TMath::Sqrt(GetSigmaXY2());
    Double_t meanX = GetMeanPositionX();
    Double_t meanY = GetMeanPositionY();
    for (const auto &h : fHits){
        if (h.fType.Contains("Y"))
            skewXY += h.fEnergy * (meanY - h.fY) * (meanY - h.fY) * (meanY - h.fY);
        if (h.fType.Contains("X"))
            skewXY += h.fEnergy * (meanX - h.fX) * (meanX - h.fX) * (meanX - h.fX);
    }
    return skewXY /= (totalEnergy * sigmaXY * sigmaXY * sigmaXY);
}

Double_t TRestHits::GetSigmaZ2() {
    Double_t sigmaZ2 = 0;
    Double_t totalEnergy = GetTotalEnergy();
    Double_t meanZ = GetMeanPositionZ();

    for (const auto &h : fHits){
        if (h.fType.Contains("Z"))sigmaZ2 += h.fEnergy * (meanZ - h.fZ) * (meanZ - h.fZ);
    }
    return sigmaZ2 /= totalEnergy;
}

Double_t TRestHits::GetSkewZ() {
    Double_t skewZ = 0;
    Double_t totalEnergy = this->GetTotalEnergy();
    Double_t sigmaZ = TMath::Sqrt(this->GetSigmaZ2());
    Double_t meanZ = this->GetMeanPositionZ();

    for (const auto &h : fHits){
        if (h.fType.Contains("Z")) skewZ += h.fEnergy * (meanZ - h.fZ) * (meanZ - h.fZ) * (meanZ - h.fZ);
    }

    return skewZ /= (totalEnergy * sigmaZ * sigmaZ * sigmaZ);
}

Double_t TRestHits::GetMeanPositionXInPrism(TVector3 x0, TVector3 x1, Double_t sizeX, Double_t sizeY,
                                            Double_t theta) {
    Double_t meanX = 0;
    Double_t totalEnergy = 0;
    int n=0;
      for (const auto &h : fHits){
        if (h.fType.Contains("X") && (isHitNInsidePrism(n, x0, x1, sizeX, sizeY, theta))) {
            meanX += h.fX * h.fEnergy;
            totalEnergy += h.fEnergy;
        }
        n++;
      }

    meanX /= totalEnergy;

    return meanX;
}

Double_t TRestHits::GetMeanPositionYInPrism(TVector3 x0, TVector3 x1, Double_t sizeX, Double_t sizeY, Double_t theta) {
    Double_t meanY = 0;
    Double_t totalEnergy = 0;
    int n=0;
    for (const auto &h : fHits){
        if (h.fType.Contains("Y") && (isHitNInsidePrism(n, x0, x1, sizeX, sizeY, theta))) {
            meanY += h.fY * h.fEnergy;
            totalEnergy += h.fEnergy;
        }
      n++;
    }

    meanY /= totalEnergy;

    return meanY;
}

Double_t TRestHits::GetMeanPositionZInPrism(TVector3 x0, TVector3 x1, Double_t sizeX, Double_t sizeY, Double_t theta) {
    Double_t meanZ = 0;
    Double_t totalEnergy = 0;
    int n=0;
   for (const auto &h : fHits) {
        if (h.fType.Contains("Z") && (isHitNInsidePrism(n, x0, x1, sizeX, sizeY, theta))) {
            meanZ += h.fZ * h.fEnergy;
            totalEnergy += h.fEnergy;
        }
      n++;
    }

    meanZ /= totalEnergy;

    return meanZ;
}

TVector3 TRestHits::GetMeanPositionInPrism(TVector3 x0, TVector3 x1, Double_t sizeX, Double_t sizeY, Double_t theta) {
    TVector3 mean(GetMeanPositionXInPrism(x0, x1, sizeX, sizeY, theta),
                  GetMeanPositionYInPrism(x0, x1, sizeX, sizeY, theta),
                  GetMeanPositionZInPrism(x0, x1, sizeX, sizeY, theta));
    return mean;
}

Double_t TRestHits::GetMeanPositionXInCylinder(TVector3 x0, TVector3 x1, Double_t radius) {
    Double_t meanX = 0;
    Double_t totalEnergy = 0;
    int n=0;
    for (const auto &h : fHits) {
        if (h.fType.Contains("X") && (isHitNInsideCylinder(n, x0, x1, radius))) {
            meanX += h.fX * h.fEnergy;
            totalEnergy += h.fEnergy;
        }
      n++;
    }

    meanX /= totalEnergy;

    return meanX;
}

Double_t TRestHits::GetMeanPositionYInCylinder(TVector3 x0, TVector3 x1, Double_t radius) {
    Double_t meanY = 0;
    Double_t totalEnergy = 0;
    int n=0;
    for (const auto &h : fHits) {
        if (h.fType.Contains("Y") && (isHitNInsideCylinder(n, x0, x1, radius))) {
            meanY += h.fY * h.fEnergy;
            totalEnergy += h.fEnergy;
        }
      n++;
    }

    meanY /= totalEnergy;

    return meanY;
}

Double_t TRestHits::GetMeanPositionZInCylinder(TVector3 x0, TVector3 x1, Double_t radius) {
    Double_t meanZ = 0;
    Double_t totalEnergy = 0;
    int n=0;
    for (const auto &h : fHits) {
        if (h.fType.Contains("Z") && (isHitNInsideCylinder(n, x0, x1, radius))) {
            meanZ += h.fZ * h.fEnergy;
            totalEnergy += h.fEnergy;
        }
      n++;
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

