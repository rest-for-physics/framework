///______________________________________________________________________________
///______________________________________________________________________________
///
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestHitsAnalysisProcess.cxx
///
///
///             First implementation of hits analysis process into REST_v2
///             Date : may/2016
///             Author : J. Galan
///
///_______________________________________________________________________________

#include "TRestHitsAnalysisProcess.h"
using namespace std;

ClassImp(TRestHitsAnalysisProcess)
    //______________________________________________________________________________
    TRestHitsAnalysisProcess::TRestHitsAnalysisProcess() {
    Initialize();
}

//______________________________________________________________________________
TRestHitsAnalysisProcess::TRestHitsAnalysisProcess(char* cfgFileName) {
    Initialize();

    if (LoadConfigFromFile(cfgFileName) == -1) LoadDefaultConfig();
}

//______________________________________________________________________________
TRestHitsAnalysisProcess::~TRestHitsAnalysisProcess() {
    delete fInputHitsEvent;
    delete fOutputHitsEvent;
}

void TRestHitsAnalysisProcess::LoadDefaultConfig() { SetTitle("Default config"); }

//______________________________________________________________________________
void TRestHitsAnalysisProcess::Initialize() {
    SetSectionName(this->ClassName());

    fInputHitsEvent = new TRestHitsEvent();
    fOutputHitsEvent = new TRestHitsEvent();

    fOutputEvent = fOutputHitsEvent;
    fInputEvent = fInputHitsEvent;

    fPrismFiducial = false;
    fCylinderFiducial = false;
}

void TRestHitsAnalysisProcess::LoadConfig(std::string cfgFilename, std::string name) {
    if (LoadConfigFromFile(cfgFilename, name) == -1) LoadDefaultConfig();
}

//______________________________________________________________________________
void TRestHitsAnalysisProcess::InitProcess() { TRestEventProcess::ReadObservables(); }

//______________________________________________________________________________
void TRestHitsAnalysisProcess::BeginOfEventProcess() { fOutputHitsEvent->Initialize(); }

//______________________________________________________________________________
TRestEvent* TRestHitsAnalysisProcess::ProcessEvent(TRestEvent* evInput) {
    fInputHitsEvent = (TRestHitsEvent*)evInput;

    TString obsName;

    TRestHits* hits = fInputHitsEvent->GetHits();
    for (int n = 0; n < hits->GetNumberOfHits(); n++) {
        Double_t eDep = hits->GetEnergy(n);

        Double_t x = hits->GetX(n);
        Double_t y = hits->GetY(n);
        Double_t z = hits->GetZ(n);

        fOutputHitsEvent->AddHit(x, y, z, eDep);
    }

    if (fOutputHitsEvent->GetNumberOfHits() == 0) return NULL;

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

    SetObservableValue("balanceXYnHits", (nHitsX - nHitsY) / (nHitsX + nHitsY));

    if ((nHits == nHitsX) || (nHits == nHitsY))
        SetObservableValue("nHitsSizeXY", nHits);
    else
        SetObservableValue("nHitsSizeXY", TMath::Sqrt(nHitsX * nHitsX + nHitsY * nHitsY));

    // Checking hits inside fiducial cylinder
    if (fCylinderFiducial) {
        TVector3 meanPositionInCylinder =
            fOutputHitsEvent->GetMeanPositionInCylinder(fFid_x0, fFid_x1, fFid_R);

        Int_t isInsideCylinder = 0;
        if (fOutputHitsEvent->isHitsEventInsideCylinder(fFid_x0, fFid_x1, fFid_R)) isInsideCylinder = 1;

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
        if (fOutputHitsEvent->isHitsEventInsidePrism(fFid_x0, fFid_x1, fFid_sX, fFid_sY, fFid_theta))
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
    SetObservableValue("xySigmaBalance", (sigmaX - sigmaY) / (sigmaX + sigmaY));

    SetObservableValue("z2Sigma", sigmaZ2);

    SetObservableValue("xySkew", skewXY);
    SetObservableValue("zSkew", skewZ);

    if (GetVerboseLevel() >= REST_Extreme) {
        fOutputHitsEvent->PrintEvent(1000);
        GetChar();
    }

    return fOutputHitsEvent;
}

//______________________________________________________________________________
void TRestHitsAnalysisProcess::EndOfEventProcess() {}

//______________________________________________________________________________
void TRestHitsAnalysisProcess::EndProcess() {
    // Function to be executed once at the end of the process
    // (after all events have been processed)

    // Start by calling the EndProcess function of the abstract class.
    // Comment this if you don't want it.
    // TRestEventProcess::EndProcess();
}

//______________________________________________________________________________
void TRestHitsAnalysisProcess::InitFromConfigFile() {
    fFid_x0 = Get3DVectorParameterWithUnits("fiducial_x0", TVector3(0, 0, 0));
    fFid_x1 = Get3DVectorParameterWithUnits("fiducial_x1", TVector3(0, 0, 0));

    fFid_R = GetDblParameterWithUnits("fiducial_R", 1);
    fFid_sX = GetDblParameterWithUnits("fiducial_sX", 1);
    fFid_sY = GetDblParameterWithUnits("fiducial_sY", 1);
    fFid_theta = StringToDouble(GetParameter("fiducial_theta", "0"));

    if (GetParameter("cylinderFiducialization", "false") == "true") fCylinderFiducial = true;

    if (GetParameter("prismFiducialization", "false") == "true") fPrismFiducial = true;

    if (fCylinderFiducial) cout << "Cylinder fiducial active" << endl;
    if (fPrismFiducial) cout << "Prism fiducial active" << endl;
}
