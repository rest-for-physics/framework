///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             mySignalProcess.cxx
///
///_______________________________________________________________________________

#include "mySignalProcess.h"
using namespace std;

ClassImp(mySignalProcess)
    //______________________________________________________________________________
    mySignalProcess::mySignalProcess() {
  Initialize();
}

//______________________________________________________________________________
mySignalProcess::~mySignalProcess() {
  delete fOutputSignalEvent;
  delete fInputSignalEvent;
}

//______________________________________________________________________________
void mySignalProcess::Initialize() {
  // We define the section name (by default we use the name of the class)
  SetSectionName(this->ClassName());

  // We create the input/output specific event data
  fInputSignalEvent = new TRestSignalEvent();
  fOutputSignalEvent = new TRestSignalEvent();

  // We connect the TRestEventProcess input/output event pointers
  fInputEvent = fInputSignalEvent;
  fOutputEvent = fOutputSignalEvent;
}

//______________________________________________________________________________
TRestEvent* mySignalProcess::ProcessEvent(TRestEvent* evInput) {
  fInputSignalEvent = (TRestSignalEvent*)evInput;

  if (fInputSignalEvent->GetNumberOfSignals() <= 0) return NULL;

  if (GetVerboseLevel() >= REST_Debug) {
    cout << "This a debug message" << endl;
    cout << "I am in event id : " << fInputSignalEvent->GetID() << endl;
    cout << "I am a new signal process" << endl;
  }

  /* If you are not modifying the input signal event
     You may just skip this loop and use

     TransferEvent( fOutputSignalEvent, fInputSignalEvent ); */

  for (int n = 0; n < fInputSignalEvent->GetNumberOfSignals(); n++) {
    TRestSignal* outSignal = fInputSignalEvent->GetSignal(n);

    // Do something with output signal

    fOutputSignalEvent->AddSignal(*outSignal);
  }

  return fOutputSignalEvent;
}

void mySignalProcess::InitFromConfigFile() {
  fMyDummyParameter = StringToInteger(GetParameter("aDummyParameter"));
}
