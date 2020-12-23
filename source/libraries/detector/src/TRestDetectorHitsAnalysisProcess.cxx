/*************************************************************************
 * This file is part of the REST software framework.                     *
 *                                                                       *
 * Copyright (C) 2016 GIFNA/TREX (University of Zaragoza)                *
 * For more information see http://gifna.unizar.es/trex                  *
 *                                                                       *
 * REST is free software: you can redistribute it and/or modify          *
 * it under the terms of the GNU General Public License as published by  *
 * the Free Software Foundation, either version 3 of the License, or     *
 * (at your option) any later version.                                   *
 *                                                                       *
 * REST is distributed in the hope that it will be useful,               *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the          *
 * GNU General Public License for more details.                          *
 *                                                                       *
 * You should have a copy of the GNU General Public License along with   *
 * REST in $REST_PATH/LICENSE.                                           *
 * If not, see http://www.gnu.org/licenses/.                             *
 * For the list of contributors see $REST_PATH/CREDITS.                  *
 *************************************************************************/

//////////////////////////////////////////////////////////////////////////
///
/// ### Observables
///
/// Related to the number of hits:
///
/// * **nHits**: Number of punctual energy depositions (fNHits).
/// * **nHitsX**: Number of punctual depositions detected in X channels.
/// * **nHitsY**: Number of punctual depositions detected in Y channels.
/// * **balanceXYnHits**: (nHitsX - nHitsY) / (nHitsX + nHitsY).
/// * **nHitsSizeXY**: If (nHits == nHitsX) or (nHits == nHitsY) => nHitsSizeXY==nHits
///	                   Else: nHitsSizeXY == (nHitsX^2+ nHitsY^2)^1/2.
///
/// Cylindrical fiducial volume:
///
/// * **isInsideCylindricalVolume**: 1 if some hits of the event are inside the cylindrical
/// fiducial volume, 0 if none is inside.
/// * **nInsideCylindricalVolume**: Number of hits inside the cylindrical fiducial volume.
/// * **energyInsideCylindricalVolume**: energy of the hits inside the cylindrical fiducial
/// volume.
/// * **xMeanInCylinder**: For all energy depositions inside the cylindrical fiducial volume
/// it takes the mean position in X coordinate using the energies as weights.
/// * **yMeanInCylinder**: For all energy depositions inside the cylindrical fiducial volume
/// it takes the mean position in Y coordinate using the energies as weights.
/// * **zMeanInCylinder**: For all energy depositions inside the cylindrical fiducial volume
/// it takes the mean position in Z coordinate using the energies as weights.
///
/// Prism fiducial volume:
///
/// * **isInsidePrismVolume**: It is a binary observable. It checks if the event has deposited
/// energy inside the volume. To do so, it checks if GetNumberOfHitsInsidePrism > 0.
/// GetNumberOfHitsInsidePrism counts the number of hits inside the volume.
/// * **nInsidePrismVolume**: Number of hits of the event registered inside the volume.
/// * **energyInsidePrismVolume**: Total energy of the hits deposited inside the volume.
/// * **xMeanInPrism**: For all energy depositions inside the prism fiducial volume it takes
/// the mean position in X coordinate using the energies as weights.
/// * **yMeanInPrism**: For all energy depositions inside the prism fiducial volume it takes
/// the mean position in Y coordinate using the energies as weights.
/// * **zMeanInPrism**: For all energy depositions inside the prism fiducial volume it takes
/// the mean position in Z coordinate using the energies as weights.
///
/// Distance to the borders of the volume (cylinder or prism):
///
/// * **distanceToCylinderWall**: Distance from the closest hit of the event to the cylinder
/// wall. Only hits inside the cylinder.
/// * **distanceToCylinderTop**: Distance from the closest hit of the event to the top of
/// the cylinder. Only hits inside the cylinder.
/// * **distanceToCylinderBottom**: Distance from the closest hit of the event to the bottom
/// of the cylinder. Only hits inside the cylinder.
/// * **distanceToPrismWall**: Distance from the closest hit of the event to the wall of the
/// prism. Only hits inside the prism.
/// * **distanceToPrismTop**: Distance from the closest hit of the event to the top of the
/// prism. Only hits inside the prism.
/// * **distanceToPrismBottom**: Distance from the closest hit of the event to the bottom
/// of the prism. Only hits inside the prism.
///
/// Energy properties:
///
/// * **energy**: Total energy deposited in all the hits of the event (fTotEnergy).
/// * **energyX**: Total energy deposited in hits with X coordinate well defined (!IsNaN(fX[n])).
/// * **energyY**: Total energy deposited in hits with Y coordinate well defined (!IsNaN(fY[n])).
/// * **balanceXYenergy**: (energyX - energyY) / (energyX + energyY).
/// * **maxHitEnergy**: Maximum energy of a hit in the event.
/// * **minHitEnergy**: Minimum energy of a hit in the event.
/// * **meanHitEnergy**: Total energy / Number of hits.
/// * **meanHitEnergyBalance**: meanHitEnergy / energy.
///
/// Position statistics (mean, variance, skewness):
///
/// * **xMean**: Coordinate X of the mean position of the hits of the event weighted with
/// their energies. SUM(E_n x_n)/E_T .
/// * **yMean**: Coordinate Y of the mean position of the hits of the event weighted with
/// their energies. SUM(E_n y_n)/E_T .
/// * **zMean**: Coordinate Z of the mean position of the hits of the event weighted with
/// their energies.  SUM(E_n z_n)/E_T .
///
/// * **xy2Sigma**: Sum of the variance in coordinates X and Y in the hits of an event.
/// s_xy^2=SUM(E_n[(m_x-x_n)^2+(m_y-y_n)^2])/E_T =s_x^2+s_y^2.
/// * **xySigmaBalance**: (s_x-s_y)/(s_x+s_y).
/// * **z2Sigma**: Variance in Z in the hits of an event,  s_z^2=SUM(E_n (m_z-z_n )^2)/E_T .
/// * **xySkew**: s_xy=SUM(E_n [(m_x-x_n)^3+(m_y-y_n)^3])/(E_T x s_xy^3).
/// * **zSkew**: skew=SUM(E_n (m_z-z_n)^3)/(E_T x s_z^3) Fisher-Pearson coefficient of skewness,
/// third central moment divided by the standard deviation to the power three.
/// It measures the asymmetry of the distribution.
///
///______________________________________________________________________________
///
/// RESTsoft - Software for Rare Event Searches with TPCs
///
/// History of developments:
///
/// 2016-May: First implementation of hits analysis process into REST_v2.
///
/// \class      TRestDetectorHitsAnalysisProcess
/// \author     Javier Galan
///
///______________________________________________________________________________
///
//////////////////////////////////////////////////////////////////////////

#include "TRestDetectorHitsAnalysisProcess.h"
using namespace std;

ClassImp(TRestDetectorHitsAnalysisProcess);
//______________________________________________________________________________
TRestDetectorHitsAnalysisProcess::TRestDetectorHitsAnalysisProcess() { Initialize(); }

//______________________________________________________________________________
TRestDetectorHitsAnalysisProcess::TRestDetectorHitsAnalysisProcess(char* cfgFileName) {
    Initialize();

    if (LoadConfigFromFile(cfgFileName) == -1) LoadDefaultConfig();
}

//______________________________________________________________________________
TRestDetectorHitsAnalysisProcess::~TRestDetectorHitsAnalysisProcess() { delete fOutputHitsEvent; }

void TRestDetectorHitsAnalysisProcess::LoadDefaultConfig() { SetTitle("Default config"); }

//______________________________________________________________________________
void TRestDetectorHitsAnalysisProcess::Initialize() {
    SetSectionName(this->ClassName());

    fInputHitsEvent = NULL;
    fOutputHitsEvent = new TRestDetectorHitsEvent();

    fPrismFiducial = false;
    fCylinderFiducial = false;
}

void TRestDetectorHitsAnalysisProcess::LoadConfig(std::string cfgFilename, std::string name) {
    if (LoadConfigFromFile(cfgFilename, name) == -1) LoadDefaultConfig();
}

//______________________________________________________________________________
void TRestDetectorHitsAnalysisProcess::InitProcess() { TRestEventProcess::ReadObservables(); }

//______________________________________________________________________________
TRestEvent* TRestDetectorHitsAnalysisProcess::ProcessEvent(TRestEvent* evInput) {
    fInputHitsEvent = (TRestDetectorHitsEvent*)evInput;

    TString obsName;

    TRestHits* hits = fInputHitsEvent->GetHits();
    for (int n = 0; n < hits->GetNumberOfHits(); n++) {
        Double_t eDep = hits->GetEnergy(n);

        Double_t x = hits->GetX(n);
        Double_t y = hits->GetY(n);
        Double_t z = hits->GetZ(n);

        auto time = hits->GetTime(n);
        auto type = hits->GetType(n);

        fOutputHitsEvent->AddHit(x, y, z, eDep, time, type);
    }

    Double_t energy = fOutputHitsEvent->GetEnergy();
    TVector3 meanPosition = fOutputHitsEvent->GetMeanPosition();
    Double_t sigmaX = fOutputHitsEvent->GetSigmaX();
    Double_t sigmaY = fOutputHitsEvent->GetSigmaY();
    Double_t sigmaXY2 = fOutputHitsEvent->GetSigmaXY2();
    Double_t sigmaZ2 = fOutputHitsEvent->GetSigmaZ2();
    Double_t skewXY = fOutputHitsEvent->GetSkewXY();
    Double_t skewZ = fOutputHitsEvent->GetSkewZ();
    Double_t energyX = fOutputHitsEvent->GetEnergyX();
    Double_t energyY = fOutputHitsEvent->GetEnergyY();
    Double_t maxEnergy = fOutputHitsEvent->GetMaximumHitEnergy();
    Double_t minEnergy = fOutputHitsEvent->GetMinimumHitEnergy();
    Double_t meanEnergy = fOutputHitsEvent->GetMeanHitEnergy();
    Int_t nHits = fOutputHitsEvent->GetNumberOfHits();
    Int_t nHitsX = fOutputHitsEvent->GetNumberOfHitsX();
    Int_t nHitsY = fOutputHitsEvent->GetNumberOfHitsY();

    SetObservableValue("nHits", nHits);

    SetObservableValue("nHitsX", nHitsX);

    SetObservableValue("nHitsY", nHitsY);

    SetObservableValue("balanceXYnHits", (nHitsX - nHitsY) / double(nHitsX + nHitsY));

    if ((nHits == nHitsX) || (nHits == nHitsY))
        SetObservableValue("nHitsSizeXY", (double)nHits);
    else
        SetObservableValue("nHitsSizeXY", TMath::Sqrt(nHitsX * nHitsX + nHitsY * nHitsY));

    // Checking hits inside fiducial cylinder
    if (fCylinderFiducial) {
        TVector3 meanPositionInCylinder =
            fOutputHitsEvent->GetMeanPositionInCylinder(fFid_x0, fFid_x1, fFid_R);

        Int_t isInsideCylinder = 0;
        if (fOutputHitsEvent->anyHitInsideCylinder(fFid_x0, fFid_x1, fFid_R)) isInsideCylinder = 1;

        Int_t nCylVol = fOutputHitsEvent->GetNumberOfHitsInsideCylinder(fFid_x0, fFid_x1, fFid_R);

        Double_t enCylVol = fOutputHitsEvent->GetEnergyInCylinder(fFid_x0, fFid_x1, fFid_R);

        SetObservableValue("isInsideCylindricalVolume", isInsideCylinder);

        SetObservableValue("nInsideCylindricalVolume", nCylVol);

        SetObservableValue("energyInsideCylindricalVolume", enCylVol);

        // mean positions
        SetObservableValue("xMeanInCylinder", meanPositionInCylinder.X());

        SetObservableValue("yMeanInCylinder", meanPositionInCylinder.Y());

        SetObservableValue("zMeanInCylinder", meanPositionInCylinder.Z());
    }

    // Checking hits inside fiducial prism
    if (fPrismFiducial) {
        TVector3 meanPositionInPrism =
            fOutputHitsEvent->GetMeanPositionInPrism(fFid_x0, fFid_x1, fFid_sX, fFid_sY, fFid_theta);

        Int_t isInsidePrism = 0;
        if (fOutputHitsEvent->anyHitInsidePrism(fFid_x0, fFid_x1, fFid_sX, fFid_sY, fFid_theta))
            isInsidePrism = 1;

        Int_t nPrismVol =
            fOutputHitsEvent->GetNumberOfHitsInsidePrism(fFid_x0, fFid_x1, fFid_sX, fFid_sY, fFid_theta);

        Double_t enPrismVol =
            fOutputHitsEvent->GetEnergyInPrism(fFid_x0, fFid_x1, fFid_sX, fFid_sY, fFid_theta);

        SetObservableValue("isInsidePrismVolume", isInsidePrism);

        SetObservableValue("nInsidePrismVolume", nPrismVol);

        SetObservableValue("energyInsidePrismVolume", enPrismVol);

        // Mean Positions

        SetObservableValue("xMeanInPrism", meanPositionInPrism.X());

        SetObservableValue("yMeanInPrism", meanPositionInPrism.Y());

        SetObservableValue("zMeanInPrism", meanPositionInPrism.Z());
    }

    ///////////////////////////////////////

    if (fCylinderFiducial) {
        // Adding distances to cylinder wall
        Double_t dToCylWall =
            fOutputHitsEvent->GetClosestHitInsideDistanceToCylinderWall(fFid_x0, fFid_x1, fFid_R);
        Double_t dToCylTop =
            fOutputHitsEvent->GetClosestHitInsideDistanceToCylinderTop(fFid_x0, fFid_x1, fFid_R);
        Double_t dToCylBottom =
            fOutputHitsEvent->GetClosestHitInsideDistanceToCylinderBottom(fFid_x0, fFid_x1, fFid_R);

        SetObservableValue("distanceToCylinderWall", dToCylWall);
        SetObservableValue("distanceToCylinderTop", dToCylTop);
        SetObservableValue("distanceToCylinderBottom", dToCylBottom);
    }

    if (fPrismFiducial) {
        // Adding distances to prism wall
        Double_t dToPrismWall = fOutputHitsEvent->GetClosestHitInsideDistanceToPrismWall(
            fFid_x0, fFid_x1, fFid_sX, fFid_sY, fFid_theta);
        Double_t dToPrismTop = fOutputHitsEvent->GetClosestHitInsideDistanceToPrismTop(
            fFid_x0, fFid_x1, fFid_sX, fFid_sY, fFid_theta);
        Double_t dToPrismBottom = fOutputHitsEvent->GetClosestHitInsideDistanceToPrismBottom(
            fFid_x0, fFid_x1, fFid_sX, fFid_sY, fFid_theta);

        SetObservableValue("distanceToPrismWall", dToPrismWall);

        SetObservableValue("distanceToPrismTop", dToPrismTop);

        SetObservableValue("distanceToPrismBottom", dToPrismBottom);
    }

    ///////////////////////////////////////

    SetObservableValue("energy", energy);
    SetObservableValue("energyX", energyX);
    SetObservableValue("energyY", energyY);
    SetObservableValue("balanceXYenergy", (energyX - energyY) / (energyX + energyY));

    SetObservableValue("maxHitEnergy", maxEnergy);
    SetObservableValue("minHitEnergy", minEnergy);
    SetObservableValue("meanHitEnergy", meanEnergy);
    SetObservableValue("meanHitEnergyBalance", meanEnergy / energy);

    SetObservableValue("xMean", meanPosition.X());

    SetObservableValue("yMean", meanPosition.Y());

    SetObservableValue("zMean", meanPosition.Z());
    SetObservableValue("xy2Sigma", sigmaXY2);
    SetObservableValue("xSigma", sigmaX);
    SetObservableValue("ySigma", sigmaY);
    SetObservableValue("xySigmaBalance", (sigmaX - sigmaY) / (sigmaX + sigmaY));

    SetObservableValue("z2Sigma", sigmaZ2);

    SetObservableValue("xySkew", skewXY);
    SetObservableValue("zSkew", skewZ);

    if (GetVerboseLevel() >= REST_Extreme) {
        fOutputHitsEvent->PrintEvent(1000);
        GetChar();
    }

    if (fOutputHitsEvent->GetNumberOfHits() == 0) return NULL;

    return fOutputHitsEvent;
}

//______________________________________________________________________________
void TRestDetectorHitsAnalysisProcess::EndProcess() {
    // Function to be executed once at the end of the process
    // (after all events have been processed)

    // Start by calling the EndProcess function of the abstract class.
    // Comment this if you don't want it.
    // TRestEventProcess::EndProcess();
}

//______________________________________________________________________________
void TRestDetectorHitsAnalysisProcess::InitFromConfigFile() {
    fFid_x0 = Get3DVectorParameterWithUnits("fiducial_x0", TVector3(0, 0, 0));
    fFid_x1 = Get3DVectorParameterWithUnits("fiducial_x1", TVector3(0, 0, 0));

    fFid_R = GetDblParameterWithUnits("fiducial_R", 1);
    fFid_sX = GetDblParameterWithUnits("fiducial_sX", 1);
    fFid_sY = GetDblParameterWithUnits("fiducial_sY", 1);
    // read angle in degrees
    fFid_theta = StringToDouble(GetParameter("fiducial_theta", "0"));
    // convert it into radians for internal usage
    fFid_theta = std::fmod(fFid_theta, 360) * TMath::DegToRad();

    if (GetParameter("cylinderFiducialization", "false") == "true") fCylinderFiducial = true;

    if (GetParameter("prismFiducialization", "false") == "true") fPrismFiducial = true;
}
