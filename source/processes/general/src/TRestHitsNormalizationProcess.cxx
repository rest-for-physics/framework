///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestHitsNormalizationProcess.cxx
///
///             Template to use to design "event process" classes inherited from
///             TRestHitsNormalizationProcess
///             How to use: replace TRestHitsNormalizationProcess by your name,
///             fill the required functions following instructions and add all
///             needed additional members and funcionality
///
///             sep 2016:   First concept
///                 Created as part of the conceptualization of existing REST
///                 software.
///                 Javier Galan
///_______________________________________________________________________________

#include "TRestHitsNormalizationProcess.h"
using namespace std;

ClassImp(TRestHitsNormalizationProcess)
    //______________________________________________________________________________
    TRestHitsNormalizationProcess::TRestHitsNormalizationProcess() {
    Initialize();
}

//______________________________________________________________________________
TRestHitsNormalizationProcess::TRestHitsNormalizationProcess(char* cfgFileName) {
    Initialize();

    if (LoadConfigFromFile(cfgFileName)) LoadDefaultConfig();

    PrintMetadata();

    // TRestHitsNormalizationProcess default constructor
}

//______________________________________________________________________________
TRestHitsNormalizationProcess::~TRestHitsNormalizationProcess() {
    delete fHitsInputEvent;
    delete fHitsOutputEvent;
    // TRestHitsNormalizationProcess destructor
}

void TRestHitsNormalizationProcess::LoadDefaultConfig() {
    SetTitle("Default config");

    fFactor = 5.9;
}

//______________________________________________________________________________
void TRestHitsNormalizationProcess::Initialize() {
    SetSectionName(this->ClassName());

    fFactor = 1.;

    fHitsInputEvent = new TRestHitsEvent();
    fHitsOutputEvent = new TRestHitsEvent();

    fOutputEvent = fHitsOutputEvent;
    fInputEvent = fHitsInputEvent;
}

void TRestHitsNormalizationProcess::LoadConfig(string cfgFilename, string name) {
    if (LoadConfigFromFile(cfgFilename, name)) LoadDefaultConfig();

    PrintMetadata();
}

//______________________________________________________________________________
void TRestHitsNormalizationProcess::InitProcess() {
    // Function to be executed once at the beginning of process
    // (before starting the process of the events)

    // Start by calling the InitProcess function of the abstract class.
    // Comment this if you don't want it.
    // TRestEventProcess::InitProcess();
}

//______________________________________________________________________________
void TRestHitsNormalizationProcess::BeginOfEventProcess() { fHitsOutputEvent->Initialize(); }

//______________________________________________________________________________
TRestEvent* TRestHitsNormalizationProcess::ProcessEvent(TRestEvent* evInput) {
    fHitsInputEvent = (TRestHitsEvent*)evInput;
    fHitsOutputEvent->SetEventInfo(fHitsInputEvent);

    for (int hit = 0; hit < fHitsInputEvent->GetNumberOfHits(); hit++)
        fHitsOutputEvent->AddHit(fHitsInputEvent->GetX(hit), fHitsInputEvent->GetY(hit),
                                 fHitsInputEvent->GetZ(hit), fHitsInputEvent->GetEnergy(hit) * fFactor,
                                 fHitsInputEvent->GetTime(hit), fHitsInputEvent->GetType(hit));

    if (this->GetVerboseLevel() >= REST_Debug) {
        cout << "TRestHitsNormalizationProcess. Hits added : " << fHitsOutputEvent->GetNumberOfHits() << endl;
        cout << "TRestHitsNormalizationProcess. Hits total energy : " << fHitsOutputEvent->GetEnergy()
             << endl;
    }

    return fHitsOutputEvent;
}

//______________________________________________________________________________
void TRestHitsNormalizationProcess::EndOfEventProcess() {}

//______________________________________________________________________________
void TRestHitsNormalizationProcess::EndProcess() {
    // Function to be executed once at the end of the process
    // (after all events have been processed)

    // Start by calling the EndProcess function of the abstract class.
    // Comment this if you don't want it.
    // TRestEventProcess::EndProcess();
}

//______________________________________________________________________________
void TRestHitsNormalizationProcess::InitFromConfigFile() {
    fFactor = StringToDouble(GetParameter("normFactor", "1"));
}
