#include "TRestDetectorDriftVolume.h"
using namespace REST_Units;

ClassImp(TRestDetectorDriftVolume)

    TRestDetectorDriftVolume::TRestDetectorDriftVolume() {
    Initialize();
}
TRestDetectorDriftVolume::TRestDetectorDriftVolume(const char* cfgFileName, string name) : TRestMetadata(cfgFileName) {
    LoadConfigFromFile(cfgFileName, name);
}

void TRestDetectorDriftVolume::Initialize() {
    fMaterial = "";
    fW = -1;

    fElectricField = 0;
    fDriftVelocity = 0;
    fElectronLifeTime = 0;
    fLongitudinalDiffusion = 0;
    fTransversalDiffusion = 0;
    fTownsendCoefficient = 0;
    fAttachmentCoefficient = 0;

    fPressureInAtm = 1;
    fTemperatureInK = 300;
}

void TRestDetectorDriftVolume::InitFromConfigFile() {
    fMaterial = GetParameter("material", "");
    fW = StringToDouble(GetParameter("W_value", "-1"));

    fElectricField = GetDblParameterWithUnits("electricField", 0.);
    fDriftVelocity = GetDblParameterWithUnits("driftVelocity", 0.);
    fElectronLifeTime = GetDblParameterWithUnits("electronLifeTime", 0.);
    fLongitudinalDiffusion = GetDblParameterWithUnits("longitudinalDiffusion", 0.);
    fTransversalDiffusion = GetDblParameterWithUnits("transversalDiffusion", 0.);
    fTownsendCoefficient = GetDblParameterWithUnits("townsendCoefficient", 0.);
    fAttachmentCoefficient = GetDblParameterWithUnits("attachmentCoefficient", 0.);

    fPressureInAtm = GetDblParameterWithUnits("pressure");
    fTemperatureInK = GetDblParameterWithUnits("temperature");
}

void TRestDetectorDriftVolume::PrintMetadata() {
    TRestMetadata::PrintMetadata();

    metadata << "Drift Volume Material : " << fMaterial << ", W-value : " << fW << " eV" << endl;
    metadata << "Electric Field : " << fElectricField * V / cm << " V/cm " << endl;
    metadata << "Drift Speed : " << fDriftVelocity * mm / us << " mm/us" << endl;
    metadata << "Electron LifeTime : " << fElectronLifeTime * ns << " ns" << endl;
    metadata << "Longitudinal Diffusion : " << fLongitudinalDiffusion << " (cm)^1/2" << endl;
    metadata << "Transversal Diffusion : " << fTransversalDiffusion << " (cm)^1/2" << endl;
    metadata << "******************************************" << endl;
    metadata << endl;
    metadata << endl;
}