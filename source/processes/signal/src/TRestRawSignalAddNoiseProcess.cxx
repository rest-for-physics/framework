///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestRawSignalAddNoiseProcess.cxx
///
///             February 2016: Javier Gracia
///_______________________________________________________________________________

#include "TRestRawSignalAddNoiseProcess.h"
using namespace std;

#include <TRestFFT.h>

#include <TFile.h>

ClassImp(TRestRawSignalAddNoiseProcess);
//______________________________________________________________________________
TRestRawSignalAddNoiseProcess::TRestRawSignalAddNoiseProcess() { Initialize(); }

//______________________________________________________________________________
TRestRawSignalAddNoiseProcess::TRestRawSignalAddNoiseProcess(char* cfgFileName) {
    Initialize();

    if (LoadConfigFromFile(cfgFileName) == -1) LoadDefaultConfig();

    PrintMetadata();
    // TRestRawSignalAddNoiseProcess default constructor
}

//______________________________________________________________________________
TRestRawSignalAddNoiseProcess::~TRestRawSignalAddNoiseProcess() {
    delete fOutputSignalEvent;
    // TRestRawSignalAddNoiseProcess destructor
}

void TRestRawSignalAddNoiseProcess::LoadDefaultConfig() {
    SetName("addSignalNoiseProcess-Default");
    SetTitle("Default config");

    fNoiseLevel = 1;
}

//______________________________________________________________________________
void TRestRawSignalAddNoiseProcess::Initialize() {
    SetSectionName(this->ClassName());

    fNoiseLevel = 1;

    fInputSignalEvent = NULL;
    fOutputSignalEvent = new TRestRawSignalEvent();
}

void TRestRawSignalAddNoiseProcess::LoadConfig(string cfgFilename, string name) {
    if (LoadConfigFromFile(cfgFilename, name) == -1) LoadDefaultConfig();
}

//______________________________________________________________________________
void TRestRawSignalAddNoiseProcess::InitProcess() {
    // Function to be executed once at the beginning of process
    // (before starting the process of the events)

    // Start by calling the InitProcess function of the abstract class.
    // Comment this if you don't want it.
    // TRestEventProcess::InitProcess();
}

//______________________________________________________________________________
TRestEvent* TRestRawSignalAddNoiseProcess::ProcessEvent(TRestEvent* evInput) {
    fInputSignalEvent = (TRestRawSignalEvent*)evInput;

    // cout<<"Number of signals "<< fInputSignalEvent->GetNumberOfSignals()<<
    // endl;

    if (fInputSignalEvent->GetNumberOfSignals() <= 0) return NULL;

    for (int n = 0; n < fInputSignalEvent->GetNumberOfSignals(); n++) {
        TRestRawSignal noiseSignal;

        // Asign ID and add noise
        fInputSignalEvent->GetSignal(n)->GetWhiteNoiseSignal(&noiseSignal, fNoiseLevel);
        noiseSignal.SetSignalID(fInputSignalEvent->GetSignal(n)->GetSignalID());

        fOutputSignalEvent->AddSignal(noiseSignal);
    }

    return fOutputSignalEvent;
}

//______________________________________________________________________________
void TRestRawSignalAddNoiseProcess::EndProcess() {
    // Function to be executed once at the end of the process
    // (after all events have been processed)

    // Start by calling the EndProcess function of the abstract class.
    // Comment this if you don't want it.
    // TRestEventProcess::EndProcess();
}

//______________________________________________________________________________
void TRestRawSignalAddNoiseProcess::InitFromConfigFile() {
    fNoiseLevel = StringToDouble(GetParameter("noiseLevel"));
}
