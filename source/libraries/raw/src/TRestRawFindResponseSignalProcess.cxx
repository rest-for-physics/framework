///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestRawFindResponseSignalProcess.cxx
///
///             February 2016: Javier Galan
///_______________________________________________________________________________

#include "TRestRawFindResponseSignalProcess.h"
using namespace std;

#include <TFile.h>

ClassImp(TRestRawFindResponseSignalProcess)
    //______________________________________________________________________________
    TRestRawFindResponseSignalProcess::TRestRawFindResponseSignalProcess() {
    Initialize();
}

//______________________________________________________________________________
TRestRawFindResponseSignalProcess::TRestRawFindResponseSignalProcess(char* cfgFileName) {
    Initialize();

    if (LoadConfigFromFile(cfgFileName) == -1) LoadDefaultConfig();

    PrintMetadata();
    // TRestRawFindResponseSignalProcess default constructor
}

//______________________________________________________________________________
TRestRawFindResponseSignalProcess::~TRestRawFindResponseSignalProcess() {
    delete fOutputSignalEvent;
    // TRestRawFindResponseSignalProcess destructor
}

void TRestRawFindResponseSignalProcess::LoadDefaultConfig() {
    SetName("findResponseSignal-Default");
    SetTitle("Default config");
}

//______________________________________________________________________________
void TRestRawFindResponseSignalProcess::Initialize() {
    SetSectionName(this->ClassName());

    fInputSignalEvent = NULL;
    fOutputSignalEvent = new TRestRawSignalEvent();
}

void TRestRawFindResponseSignalProcess::LoadConfig(string cfgFilename, string name) {
    if (LoadConfigFromFile(cfgFilename, name) == -1) LoadDefaultConfig();
}

//______________________________________________________________________________
void TRestRawFindResponseSignalProcess::InitProcess() {
    // Function to be executed once at the beginning of process
    // (before starting the process of the events)

    // Start by calling the InitProcess function of the abstract class.
    // Comment this if you don't want it.
    // TRestEventProcess::InitProcess();
}

//______________________________________________________________________________
TRestEvent* TRestRawFindResponseSignalProcess::ProcessEvent(TRestEvent* evInput) {
    fInputSignalEvent = (TRestRawSignalEvent*)evInput;

    if (fInputSignalEvent->GetNumberOfSignals() <= 0) return NULL;
    if (fInputSignalEvent->GetNumberOfSignals() > 8) return NULL;

    Int_t dominantSignal = -1;
    Double_t maxPeak = 0;
    //   Double_t maxTime = 0;
    for (int n = 0; n < fInputSignalEvent->GetNumberOfSignals(); n++) {
        if (fInputSignalEvent->GetSignal(n)->GetMaxPeakValue() > maxPeak) {
            maxPeak = fInputSignalEvent->GetSignal(n)->GetMaxPeakValue();
            // maxTime = fInputSignalEvent->GetSignal(n)->GetMaxPeakBin();
            dominantSignal = n;
        }
    }

    if (maxPeak < 400 || maxPeak > 600) return NULL;

    TRestRawSignal* sgnl = fInputSignalEvent->GetSignal(dominantSignal);
    sgnl->Scale(1000. / maxPeak);

    fOutputSignalEvent->AddSignal(*sgnl);

    /// Copying the signal event to the output event
    /*

    fOutputSignalEvent->SetID( fInputSignalEvent->GetID() );
    fOutputSignalEvent->SetSubID( fInputSignalEvent->GetSubID() );
    fOutputSignalEvent->SetTimeStamp( fInputSignalEvent->GetTimeStamp() );
    fOutputSignalEvent->SetSubEventTag( fInputSignalEvent->GetSubEventTag() );

    fOutputSignalEvent->AddSignal( *fInputSignalEvent->GetSignal( dominantSignal )
    );
    /////////////////////////////////////////////////
    */

    return fOutputSignalEvent;
}

//______________________________________________________________________________
void TRestRawFindResponseSignalProcess::EndProcess() {
    // Function to be executed once at the end of the process
    // (after all events have been processed)

    // Start by calling the EndProcess function of the abstract class.
    // Comment this if you don't want it.
    // TRestEventProcess::EndProcess();
}

//______________________________________________________________________________
void TRestRawFindResponseSignalProcess::InitFromConfigFile() {
    /*
        fTriggerTime = StringToInteger( GetParameter( "triggerTime" ) );
        fNBins = StringToInteger( GetParameter( "nBins" ) );
    */
}
