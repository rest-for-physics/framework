#include "TRestDriftVolume.h"
using namespace REST_Units;

ClassImp(TRestDriftVolume)

TRestDriftVolume::TRestDriftVolume() { Initialize(); }
TRestDriftVolume::TRestDriftVolume(const char* cfgFileName, string name) : TRestMetadata(cfgFileName) {
    LoadConfigFromFile(cfgFileName, name);
}

void TRestDriftVolume::Initialize() {
    string fMaterial;
    Double_t fW;

    Double_t fElectricField;
    Double_t fDriftVelocity;
    Double_t fElectronLifeTime;
    Double_t fLongitudinalDiffusion;
    Double_t fTransversalDiffusion;
    Double_t fTownsendCoefficient;
    Double_t fAttachmentCoefficient;
}

void TRestDriftVolume::InitFromConfigFile() {
    fMaterial = GetParameter("material", "");
    fW = GetDblParameterWithUnits("wValue", 0.);
    fElectricField = GetDblParameterWithUnits("electricField", 0.);
    fDriftVelocity = GetDblParameterWithUnits("driftVelocity", 0.);
    fElectronLifeTime = GetDblParameterWithUnits("electronLifeTime", 0.);
    fLongitudinalDiffusion = GetDblParameterWithUnits("longitudinalDiffusion", 0.);
    fTransversalDiffusion = GetDblParameterWithUnits("transversalDiffusion", 0.);
    fTownsendCoefficient = GetDblParameterWithUnits("townsendCoefficient", 0.);
    fAttachmentCoefficient = GetDblParameterWithUnits("attachmentCoefficient", 0.);
}

void TRestDriftVolume::PrintMetadata() {
    TRestMetadata::PrintMetadata();

    metadata << "Drift Volume Material : " << fMaterial << ", W-value : " << fW * eV << " eV" << endl;
    metadata << "Electric Field : " << fElectricField * V / cm << " V/cm " << endl;
    metadata << "Drift Speed : " << fDriftVelocity * mm / us << " mm/us" << endl;
    metadata << "Electron LifeTime : " << fElectronLifeTime * ns << " ns" << endl;
    metadata << "Longitudinal Diffusion : " << fLongitudinalDiffusion << " (cm)^1/2" << endl;
    metadata << "Transversal Diffusion : " << fTransversalDiffusion << " (cm)^1/2" << endl;
    metadata << "******************************************" << endl;
    metadata << endl;
    metadata << endl;
}