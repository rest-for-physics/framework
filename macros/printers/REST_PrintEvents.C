#ifdef REST_Geant4Lib
#include "TRestGeant4Event.h"
#endif

#include "TRestHitsEvent.h"
#include "TRestSignalEvent.h"
//#include "TRestLinearTrackEvent.h"

#ifdef REST_RawLib
#include "TRestRawSignalEvent.h"
#endif

#ifdef REST_TrackLib
#include "TRestTrackEvent.h"
#endif

#include "TRestTask.h"

#ifndef RESTTask_PrintEvents
#define RESTTask_PrintEvents

//*******************************************************************************************************
//***
//*** Your HELP is needed to verify, validate and document this macro
//*** This macro might need update/revision.
//***
//*******************************************************************************************************
Int_t REST_GenericEvents(TString fName, TString EventType = "", Int_t Entry = 0) {
    TRestRun* run = new TRestRun();
    TClass* cl = TClass::GetClass(EventType);
    if (cl == NULL || EventType == "") {
        run->OpenInputFile(fName);
        TRestEvent* eve = run->GetInputEvent();
        if (eve == NULL) {
            cout << "Failed to launch the printer!" << endl;
            return 0;
        }
    } else {
        TRestEvent* eve = (TRestEvent*)cl->New();
        run->SetInputEvent(eve);
        run->OpenInputFile(fName);
    }

    TRestEvent* evt = run->GetInputEvent();

    run->PrintMetadata();

    run->GetEntry(Entry);

    evt->PrintEvent();

    delete run;

    return 0;
}

#ifdef REST_Geant4Lib
Int_t REST_Geant4Event(TString fName, Int_t firstEvent = 0) {
    TRestRun* run = new TRestRun();

    run->OpenInputFile(fName);

    run->PrintMetadata();

    TRestGeant4Event* evt = new TRestGeant4Event();

    run->SetInputEvent(evt);

    run->GetEntry(firstEvent);

    evt->PrintEvent();

    delete run;

    return 0;
}
#endif

Int_t REST_HitsEvent(TString fName, Int_t firstEvent = 0) {
    TRestRun* run = new TRestRun();

    run->OpenInputFile(fName);

    run->PrintMetadata();

    TRestHitsEvent* evt = new TRestHitsEvent();

    run->SetInputEvent(evt);

    run->GetEntry(firstEvent);

    evt->PrintEvent();

    delete run;

    return 0;
}
/*
Int_t REST_LinearTrackEvent(TString fName, Int_t firstEvent = 0) {
    TRestRun* run = new TRestRun();

    run->OpenInputFile(fName);

    run->PrintMetadata();

    TRestLinearTrackEvent* evt = new TRestLinearTrackEvent();

    run->SetInputEvent(evt);

    run->GetEntry(firstEvent);

    evt->PrintEvent();

    delete run;

    return 0;
} */

#ifdef REST_RawLib
Int_t REST_RawSignalEvent(TString fName, Int_t firstEvent = 0) {
    TRestRun* run = new TRestRun();

    run->OpenInputFile(fName);

    run->PrintMetadata();

    TRestRawSignalEvent* evt = new TRestRawSignalEvent();

    run->SetInputEvent(evt);

    run->GetEntry(firstEvent);

    evt->PrintEvent();

    delete run;

    return 0;
}
#endif

Int_t REST_SignalEvent(TString fName, Int_t firstEvent = 0) {
    TRestRun* run = new TRestRun();

    run->OpenInputFile(fName);

    run->PrintMetadata();

    TRestSignalEvent* evt = new TRestSignalEvent();

    run->SetInputEvent(evt);

    run->GetEntry(firstEvent);

    evt->PrintEvent();

    delete run;

    return 0;
}

#ifdef REST_TrackLib
Int_t REST_TrackEvent(TString fName, Int_t firstEvent = 0) {
    TRestRun* run = new TRestRun();

    run->OpenInputFile(fName);

    run->PrintMetadata();

    TRestTrackEvent* evt = new TRestTrackEvent();

    run->SetInputEvent(evt);

    run->GetEntry(firstEvent);

    evt->PrintEvent();

    delete run;

    return 0;
}
#endif

#endif
