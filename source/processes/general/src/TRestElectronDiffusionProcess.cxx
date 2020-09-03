///______________________________________________________________________________
///______________________________________________________________________________
///
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestElectronDiffusionProcess.cxx
///
///
///             First implementation of electron diffusion process into REST_v2
///             Date : Sep/2015
///             Author : J. Galan
///
///_______________________________________________________________________________

#include "TRestElectronDiffusionProcess.h"
using namespace std;

ClassImp(TRestElectronDiffusionProcess);

//______________________________________________________________________________
TRestElectronDiffusionProcess::TRestElectronDiffusionProcess() { Initialize(); }

//______________________________________________________________________________
TRestElectronDiffusionProcess::TRestElectronDiffusionProcess(char* cfgFileName) {
    Initialize();

    if (LoadConfigFromFile(cfgFileName)) LoadDefaultConfig();
}

//______________________________________________________________________________
TRestElectronDiffusionProcess::~TRestElectronDiffusionProcess() { delete fOutputHitsEvent; }

void TRestElectronDiffusionProcess::LoadDefaultConfig() {
    SetTitle("Default config");

    fAttachment = 0;
}

//______________________________________________________________________________
void TRestElectronDiffusionProcess::Initialize() {
    SetSectionName(this->ClassName());

    fAttachment = 0;

    fTransDiffCoeff = 0;
    fLonglDiffCoeff = 0;
    fWvalue = 0;

    fOutputHitsEvent = new TRestHitsEvent();
    fInputHitsEvent = NULL;

    fRandom = NULL;
}

void TRestElectronDiffusionProcess::LoadConfig(string cfgFilename, string name) {
    if (LoadConfigFromFile(cfgFilename, name)) LoadDefaultConfig();
}

//______________________________________________________________________________
void TRestElectronDiffusionProcess::InitProcess() {
    fRandom = new TRandom3(fSeed);

    // calculate attachment from life time:
    //(1-A) = e^(-t*v)
    if (fAttachment <= 0)
        fAttachment = 1 - exp(-gDetector->GetElectronLifeTime() * gDetector->GetDriftVelocity() *
                              REST_Units::cm);  // attatched ratio per cm

    debug << "Attachment : " << fAttachment << endl;

    if (fWvalue <= 0) fWvalue = gDetector->GetWvalue();
    if (fLonglDiffCoeff <= 0) fLonglDiffCoeff = gDetector->GetLongitudinalDiffusion();  // (cm)^1/2
    if (fTransDiffCoeff <= 0) fTransDiffCoeff = gDetector->GetTransversalDiffusion();   // (cm)^1/2
    if (fLonglDiffCoeff <= 0 || fTransDiffCoeff <= 0) {
        warning << "Gas has not been initialized" << endl;
        ferr << "TRestElectronDiffusionProcess: diffusion parameters are not defined in the rml file!"
             << endl;
        exit(-1);
    }
    if (fWvalue <= 0) {
        warning << "Gas has not been initialized" << endl;
        ferr << "TRestElectronDiffusionProcess: gas work function has not been defined in the rml file!"
             << endl;
        exit(-1);
    }
}

//______________________________________________________________________________
TRestEvent* TRestElectronDiffusionProcess::ProcessEvent(TRestEvent* evInput) {
    fInputHitsEvent = (TRestHitsEvent*)evInput;
    fOutputHitsEvent->SetEventInfo(fInputHitsEvent);

    Int_t nHits = fInputHitsEvent->GetNumberOfHits();
    if (nHits <= 0) return NULL;

    Int_t isAttached;

    Int_t totalElectrons = fInputHitsEvent->GetEnergy() * REST_Units::eV / fWvalue;

    Double_t wValue = fWvalue;
    if (fMaxHits > 0 && totalElectrons > fMaxHits)
        wValue = fInputHitsEvent->GetEnergy() * REST_Units::eV / fMaxHits;

    double top = gDetector->GetTPCTopZ();
    double bottom = gDetector->GetTPCBottomZ();

    for (int n = 0; n < nHits; n++) {
        TRestHits* hits = fInputHitsEvent->GetHits();

        Double_t eDep = hits->GetEnergy(n);

        if (eDep > 0) {
            const Double_t x = hits->GetX(n);
            const Double_t y = hits->GetY(n);
            const Double_t z = hits->GetZ(n);

            if (z > bottom && z < top) {
                Double_t xDiff, yDiff, zDiff;

                Double_t driftDistance = gDetector->GetDriftDistance(TVector3(x, y, z));

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

    if (this->GetVerboseLevel() >= REST_Debug) {
        cout << "TRestElectronDiffusionProcess. Input hits energy : " << fInputHitsEvent->GetEnergy() << endl;
        cout << "TRestElectronDiffusionProcess. Hits added : " << fOutputHitsEvent->GetNumberOfHits() << endl;
        cout << "TRestElectronDiffusionProcess. Hits total energy : " << fOutputHitsEvent->GetEnergy()
             << endl;
        if (GetVerboseLevel() >= REST_Extreme) GetChar();
    }

    return fOutputHitsEvent;
}

//______________________________________________________________________________
void TRestElectronDiffusionProcess::EndProcess() {
    // Function to be executed once at the end of the process
    // (after all events have been processed)

    // Start by calling the EndProcess function of the abstract class.
    // Comment this if you don't want it.
    // TRestEventProcess::EndProcess();
}

//______________________________________________________________________________
void TRestElectronDiffusionProcess::InitFromConfigFile() {
    double field = GetDblParameterWithUnits("electricField", -1);
    if (field != -1) gDetector->SetDriftField(field);

    double pressure = GetDblParameterWithUnits("gasPressure", -1);
    if (pressure != -1) gDetector->SetPressure(pressure);

    fWvalue = GetDblParameterWithUnits("Wvalue", (double)0) * REST_Units::eV;

    fAttachment = StringToDouble(GetParameter("attachment", "0"));

    fLonglDiffCoeff = StringToDouble(GetParameter("longDiff", "-1"));

    fTransDiffCoeff = StringToDouble(GetParameter("transDiff", "-1"));

    fMaxHits = StringToInteger(GetParameter("maxHits", "1000"));

    fSeed = StringToDouble(GetParameter("seed", "0"));
}
