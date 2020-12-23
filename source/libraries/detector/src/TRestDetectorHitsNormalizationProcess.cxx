///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestDetectorHitsNormalizationProcess.cxx
///
///             Template to use to design "event process" classes inherited from
///             TRestDetectorHitsNormalizationProcess
///             How to use: replace TRestDetectorHitsNormalizationProcess by your name,
///             fill the required functions following instructions and add all
///             needed additional members and funcionality
///
///             sep 2016:   First concept
///                 Created as part of the conceptualization of existing REST
///                 software.
///                 Javier Galan
///_______________________________________________________________________________

#include "TRestDetectorHitsNormalizationProcess.h"
using namespace std;

ClassImp(TRestDetectorHitsNormalizationProcess)
    //______________________________________________________________________________
    TRestDetectorHitsNormalizationProcess::TRestDetectorHitsNormalizationProcess() {
    Initialize();
}

//______________________________________________________________________________
TRestDetectorHitsNormalizationProcess::TRestDetectorHitsNormalizationProcess(char* cfgFileName) {
    Initialize();

    if (LoadConfigFromFile(cfgFileName)) LoadDefaultConfig();

    PrintMetadata();

    // TRestDetectorHitsNormalizationProcess default constructor
}

//______________________________________________________________________________
TRestDetectorHitsNormalizationProcess::~TRestDetectorHitsNormalizationProcess() {
    delete fHitsOutputEvent;
    // TRestDetectorHitsNormalizationProcess destructor
}

void TRestDetectorHitsNormalizationProcess::LoadDefaultConfig() {
    SetTitle("Default config");

    fFactor = 5.9;
}

//______________________________________________________________________________
void TRestDetectorHitsNormalizationProcess::Initialize() {
    SetSectionName(this->ClassName());

    fFactor = 1.;

    fHitsInputEvent = NULL;
    fHitsOutputEvent = new TRestDetectorHitsEvent();
}

void TRestDetectorHitsNormalizationProcess::LoadConfig(string cfgFilename, string name) {
    if (LoadConfigFromFile(cfgFilename, name)) LoadDefaultConfig();

    PrintMetadata();
}

//______________________________________________________________________________
void TRestDetectorHitsNormalizationProcess::InitProcess() {
    // Function to be executed once at the beginning of process
    // (before starting the process of the events)

    // Start by calling the InitProcess function of the abstract class.
    // Comment this if you don't want it.
    // TRestEventProcess::InitProcess();
}

//______________________________________________________________________________
TRestEvent* TRestDetectorHitsNormalizationProcess::ProcessEvent(TRestEvent* evInput) {
    fHitsInputEvent = (TRestDetectorHitsEvent*)evInput;
    fHitsOutputEvent->SetEventInfo(fHitsInputEvent);

    for (int hit = 0; hit < fHitsInputEvent->GetNumberOfHits(); hit++)
        fHitsOutputEvent->AddHit(fHitsInputEvent->GetX(hit), fHitsInputEvent->GetY(hit),
                                 fHitsInputEvent->GetZ(hit), fHitsInputEvent->GetEnergy(hit) * fFactor,
                                 fHitsInputEvent->GetTime(hit), fHitsInputEvent->GetType(hit));

    if (this->GetVerboseLevel() >= REST_Debug) {
        cout << "TRestDetectorHitsNormalizationProcess. Hits added : " << fHitsOutputEvent->GetNumberOfHits() << endl;
        cout << "TRestDetectorHitsNormalizationProcess. Hits total energy : " << fHitsOutputEvent->GetEnergy()
             << endl;
    }

    return fHitsOutputEvent;
}

//______________________________________________________________________________
void TRestDetectorHitsNormalizationProcess::EndProcess() {
    // Function to be executed once at the end of the process
    // (after all events have been processed)

    // Start by calling the EndProcess function of the abstract class.
    // Comment this if you don't want it.
    // TRestEventProcess::EndProcess();
}

//______________________________________________________________________________
void TRestDetectorHitsNormalizationProcess::InitFromConfigFile() {
    fFactor = StringToDouble(GetParameter("normFactor", "1"));
}
