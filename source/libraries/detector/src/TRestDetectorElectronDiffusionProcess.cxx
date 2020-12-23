///______________________________________________________________________________
///______________________________________________________________________________
///
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestDetectorElectronDiffusionProcess.cxx
///
///
///             First implementation of electron diffusion process into REST_v2
///             Date : Sep/2015
///             Author : J. Galan
///
///_______________________________________________________________________________

#include "TRestDetectorElectronDiffusionProcess.h"
using namespace std;

ClassImp(TRestDetectorElectronDiffusionProcess);

//______________________________________________________________________________
TRestDetectorElectronDiffusionProcess::TRestDetectorElectronDiffusionProcess() { Initialize(); }

//______________________________________________________________________________
TRestDetectorElectronDiffusionProcess::TRestDetectorElectronDiffusionProcess(char* cfgFileName) {
    Initialize();

    if (LoadConfigFromFile(cfgFileName)) LoadDefaultConfig();
}

//______________________________________________________________________________
TRestDetectorElectronDiffusionProcess::~TRestDetectorElectronDiffusionProcess() { delete fOutputHitsEvent; }

void TRestDetectorElectronDiffusionProcess::LoadDefaultConfig() {
    SetTitle("Default config");

    fElectricField = 2000;
    fAttachment = 0;
    fGasPressure = 1;
}

//______________________________________________________________________________
void TRestDetectorElectronDiffusionProcess::Initialize() {
    SetSectionName(this->ClassName());

    fElectricField = 0;
    fAttachment = 0;
    fGasPressure = 1;

    fTransDiffCoeff = 0;
    fLonglDiffCoeff = 0;
    fWvalue = 0;

    fOutputHitsEvent = new TRestDetectorHitsEvent();
    fInputHitsEvent = NULL;

    fGas = NULL;
    fReadout = NULL;

    fRandom = NULL;
}

void TRestDetectorElectronDiffusionProcess::LoadConfig(string cfgFilename, string name) {
    if (LoadConfigFromFile(cfgFilename, name)) LoadDefaultConfig();
}

//______________________________________________________________________________
void TRestDetectorElectronDiffusionProcess::InitProcess() {
    fRandom = new TRandom3(fSeed);

    fGas = GetMetadata<TRestDetectorGas>();
    if (fGas == NULL) {
        if (fLonglDiffCoeff == -1 || fTransDiffCoeff == -1) {
            warning << "Gas has not been initialized" << endl;
            ferr << "TRestDetectorElectronDiffusionProcess: diffusion parameters are not defined in the rml file!"
                 << endl;
            exit(-1);
        }
        if (fWvalue == -1) {
            warning << "Gas has not been initialized" << endl;
            ferr << "TRestDetectorElectronDiffusionProcess: gas work function has not been defined in the rml file!"
                 << endl;
            exit(-1);
        }
    } else {
#ifndef USE_Garfield
        ferr << "A TRestDetectorGas definition was found but REST was not linked to Garfield libraries." << endl;
        ferr << "Please, remove the TRestDetectorGas definition, and add gas parameters inside the process "
                "TRestDetectorElectronDiffusionProcess"
             << endl;
        exit(1);
#endif
        if (fGasPressure <= 0) fGasPressure = fGas->GetPressure();
        if (fElectricField <= 0) fElectricField = fGas->GetElectricField();
        if (fWvalue <= 0) fWvalue = fGas->GetWvalue();

        fGas->SetPressure(fGasPressure);
        fGas->SetElectricField(fElectricField);
        fGas->SetW(fWvalue);

        if (fLonglDiffCoeff <= 0) fLonglDiffCoeff = fGas->GetLongitudinalDiffusion();  // (cm)^1/2

        if (fTransDiffCoeff <= 0) fTransDiffCoeff = fGas->GetTransversalDiffusion();  // (cm)^1/2
    }

    fReadout = GetMetadata<TRestDetectorReadout>();
    if (fReadout == NULL) {
        cout << "REST ERRORRRR : Readout has not been initialized" << endl;
        exit(-1);
    }
}

//______________________________________________________________________________
TRestEvent* TRestDetectorElectronDiffusionProcess::ProcessEvent(TRestEvent* evInput) {
    fInputHitsEvent = (TRestDetectorHitsEvent*)evInput;
    fOutputHitsEvent->SetEventInfo(fInputHitsEvent);

    Int_t nHits = fInputHitsEvent->GetNumberOfHits();
    if (nHits <= 0) return NULL;

    Int_t isAttached;

    Int_t totalElectrons = fInputHitsEvent->GetEnergy() * REST_Units::eV / fWvalue;

    Double_t wValue = fWvalue;
    if (fMaxHits > 0 && totalElectrons > fMaxHits)
        wValue = fInputHitsEvent->GetEnergy() * REST_Units::eV / fMaxHits;

    for (int n = 0; n < nHits; n++) {
        TRestHits* hits = fInputHitsEvent->GetHits();

        Double_t eDep = hits->GetEnergy(n);

        if (eDep > 0) {
            const Double_t x = hits->GetX(n);
            const Double_t y = hits->GetY(n);
            const Double_t z = hits->GetZ(n);

            for (int p = 0; p < fReadout->GetNumberOfReadoutPlanes(); p++) {
                TRestDetectorReadoutPlane* plane = &(*fReadout)[p];

                if (plane->isZInsideDriftVolume(z)) {
                    Double_t xDiff, yDiff, zDiff;

                    Double_t driftDistance = plane->GetDistanceTo(x, y, z);

                    Int_t numberOfElectrons = (Int_t)(eDep * REST_Units::eV / wValue);

                    if (numberOfElectrons == 0 && eDep > 0) numberOfElectrons = 1;

                    Double_t localWValue = eDep * REST_Units::eV / numberOfElectrons;
                    Double_t localEnergy = 0;

                    while (numberOfElectrons > 0) {
                        numberOfElectrons--;

                        Double_t longHitDiffusion =
                            10. * TMath::Sqrt(driftDistance / 10.) * fLonglDiffCoeff;  // mm

                        Double_t transHitDiffusion =
                            10. * TMath::Sqrt(driftDistance / 10.) * fTransDiffCoeff;  // mm

                        if (fAttachment)
                            isAttached = (fRandom->Uniform(0, 1) > pow(1 - fAttachment, driftDistance / 10.));
                        else
                            isAttached = 0;

                        if (isAttached == 0) {
                            xDiff = x + fRandom->Gaus(0, transHitDiffusion);

                            yDiff = y + fRandom->Gaus(0, transHitDiffusion);

                            zDiff = z + fRandom->Gaus(0, longHitDiffusion);

                            localEnergy += localWValue * REST_Units::keV / REST_Units::eV;
                            if (GetVerboseLevel() >= REST_Extreme)
                                cout << "Adding hit. x : " << xDiff << " y : " << yDiff << " z : " << zDiff
                                     << " en : " << localWValue * REST_Units::keV / REST_Units::eV << " keV"
                                     << endl;
                            fOutputHitsEvent->AddHit(xDiff, yDiff, zDiff,
                                                     localWValue * REST_Units::keV / REST_Units::eV,
                                                     hits->GetTime(n), hits->GetType(n));
                        }
                    }
                }
            }
        }
    }

    if (this->GetVerboseLevel() >= REST_Debug) {
        cout << "TRestDetectorElectronDiffusionProcess. Input hits energy : " << fInputHitsEvent->GetEnergy() << endl;
        cout << "TRestDetectorElectronDiffusionProcess. Hits added : " << fOutputHitsEvent->GetNumberOfHits() << endl;
        cout << "TRestDetectorElectronDiffusionProcess. Hits total energy : " << fOutputHitsEvent->GetEnergy()
             << endl;
        if (GetVerboseLevel() >= REST_Extreme) GetChar();
    }

    return fOutputHitsEvent;
}

//______________________________________________________________________________
void TRestDetectorElectronDiffusionProcess::EndProcess() {
    // Function to be executed once at the end of the process
    // (after all events have been processed)

    // Start by calling the EndProcess function of the abstract class.
    // Comment this if you don't want it.
    // TRestEventProcess::EndProcess();
}

//______________________________________________________________________________
void TRestDetectorElectronDiffusionProcess::InitFromConfigFile() {
    // TODO add pressure units
    fGasPressure = GetDblParameterWithUnits("gasPressure", -1.);
    fElectricField = GetDblParameterWithUnits("electricField", -1.);
    fWvalue = GetDblParameterWithUnits("Wvalue", (double)0) * REST_Units::eV;
    fAttachment = StringToDouble(GetParameter("attachment", "0"));
    fLonglDiffCoeff = StringToDouble(GetParameter("longitudinalDiffusionCoefficient", "-1"));
    if (fLonglDiffCoeff == -1)
        fLonglDiffCoeff = StringToDouble(GetParameter("longDiff", "-1"));
    else {
        warning << "longitudinalDiffusionCoeffient is now OBSOLETE! It will soon dissapear." << endl;
        warning << " Please use the shorter form of this parameter : longDiff" << endl;
    }

    fTransDiffCoeff = StringToDouble(GetParameter("transversalDiffusionCoefficient", "-1"));
    if (fTransDiffCoeff == -1)
        fTransDiffCoeff = StringToDouble(GetParameter("transDiff", "-1"));
    else {
        warning << "transversalDiffusionCoeffient is now OBSOLETE! It will soon dissapear." << endl;
        warning << " Please use the shorter form of this parameter : transDiff" << endl;
    }
    fMaxHits = StringToInteger(GetParameter("maxHits", "1000"));
    fSeed = StringToDouble(GetParameter("seed", "0"));
}
