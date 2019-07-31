///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestHitsRotateAndTraslateProcess.cxx
///
///             Template to use to design "event process" classes inherited from
///             TRestHitsRotateAndTraslateProcess
///             How to use: replace TRestHitsRotateAndTraslateProcess by your
///             name, fill the required functions following instructions and add
///             all needed additional members and funcionality
///
///             march 2016:   First concept
///                 Created as part of the conceptualization of existing REST
///                 software.
///                 Javier G. Garza
///_______________________________________________________________________________

#include "TRestHitsRotateAndTraslateProcess.h"
using namespace std;

#include <TRandom3.h>

ClassImp(TRestHitsRotateAndTraslateProcess)
    //______________________________________________________________________________
    TRestHitsRotateAndTraslateProcess::TRestHitsRotateAndTraslateProcess() {
    Initialize();
}

//______________________________________________________________________________
TRestHitsRotateAndTraslateProcess::TRestHitsRotateAndTraslateProcess(char* cfgFileName) {
    Initialize();

    if (LoadConfigFromFile(cfgFileName)) LoadDefaultConfig();

    PrintMetadata();

    // TRestHitsRotateAndTraslateProcess default constructor
}

//______________________________________________________________________________
TRestHitsRotateAndTraslateProcess::~TRestHitsRotateAndTraslateProcess() {
    delete fHitsInputEvent;
    delete fHitsOutputEvent;
    // TRestHitsRotateAndTraslateProcess destructor
}

void TRestHitsRotateAndTraslateProcess::LoadDefaultConfig() {
    SetTitle("Default config");

    fDeltaX = 1.0;
    fDeltaY = 1.0;
    fDeltaZ = 1.0;
    fAlpha = 0.;
    fBeta = 0.;
    fGamma = 0.;
}

//______________________________________________________________________________
void TRestHitsRotateAndTraslateProcess::Initialize() {
    SetSectionName(this->ClassName());

    fDeltaX = 1.0;
    fDeltaY = 1.0;
    fDeltaZ = 1.0;
    fAlpha = 0.;
    fBeta = 0.;
    fGamma = 0.;

    fHitsInputEvent = new TRestHitsEvent();
    fHitsOutputEvent = new TRestHitsEvent();

    fOutputEvent = fHitsOutputEvent;
    fInputEvent = fHitsInputEvent;
}

void TRestHitsRotateAndTraslateProcess::LoadConfig(string cfgFilename) {
    if (LoadConfigFromFile(cfgFilename)) LoadDefaultConfig();

    PrintMetadata();
}

//______________________________________________________________________________
void TRestHitsRotateAndTraslateProcess::InitProcess() {
    // Function to be executed once at the beginning of process
    // (before starting the process of the events)

    // Start by calling the InitProcess function of the abstract class.
    // Comment this if you don't want it.
    // TRestEventProcess::InitProcess();

    cout << __PRETTY_FUNCTION__ << endl;
}

//______________________________________________________________________________
void TRestHitsRotateAndTraslateProcess::BeginOfEventProcess() {
    cout << "Begin of event process" << endl;
    fHitsOutputEvent->Initialize();
}

//______________________________________________________________________________
TRestEvent* TRestHitsRotateAndTraslateProcess::ProcessEvent(TRestEvent* evInput) {
    fHitsInputEvent = (TRestHitsEvent*)evInput;
    TRestHitsEvent* proEvent = new TRestHitsEvent();

    proEvent = fHitsInputEvent;

    TVector3 meanPosition = proEvent->fHits->GetMeanPosition();

    for (int hit = 0; hit < fHitsInputEvent->GetNumberOfHits(); hit++) {
        proEvent->fHits->RotateIn3D(hit, fAlpha, fBeta, fGamma, meanPosition);
        proEvent->fHits->Translate(hit, fDeltaX, fDeltaY, fDeltaZ);

        fHitsOutputEvent->AddHit(proEvent->GetX(hit), proEvent->GetY(hit), proEvent->GetZ(hit),
                                 proEvent->GetEnergy(hit));
    }

    if (fHitsOutputEvent->GetNumberOfHits() == 0) return NULL;

    cout << "Electrons rotated: " << fHitsInputEvent->GetNumberOfHits() << "e-s" << endl;
    return fHitsOutputEvent;
}

//______________________________________________________________________________
void TRestHitsRotateAndTraslateProcess::EndOfEventProcess() {}

//______________________________________________________________________________
void TRestHitsRotateAndTraslateProcess::EndProcess() {
    // Function to be executed once at the end of the process
    // (after all events have been processed)

    // Start by calling the EndProcess function of the abstract class.
    // Comment this if you don't want it.
    // TRestEventProcess::EndProcess();
}

//______________________________________________________________________________
void TRestHitsRotateAndTraslateProcess::InitFromConfigFile() {
    fDeltaX = GetDblParameterWithUnits("deltaX");
    fDeltaY = GetDblParameterWithUnits("deltaY");
    fDeltaZ = GetDblParameterWithUnits("deltaZ");

    fAlpha = StringToDouble(GetParameter("alpha"));  // rotation angle around Z
    fBeta = StringToDouble(GetParameter("beta"));    // rotation angle around Y
    fGamma = StringToDouble(GetParameter("gamma"));  // rotation angle around X

    // Conversion to radians
    fAlpha = fAlpha * TMath::Pi() / 180.;
    fBeta = fBeta * TMath::Pi() / 180.;
    fGamma = fGamma * TMath::Pi() / 180.;
}
