#include "TRestG4Event.h"
#include "TRestG4Metadata.h"
#include "TRestHitsEvent.h"
#include "TRestLinearTrackEvent.h"
#include "TRestRawSignalEvent.h"
#include "TRestSignalEvent.h"
#include "TRestTask.h"
#include "TRestTrackEvent.h"

#ifndef RESTTask_PrintEvents
#define RESTTask_PrintEvents

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

Int_t REST_G4Event(TString fName, Int_t firstEvent = 0) {
    TRestRun* run = new TRestRun();

    run->OpenInputFile(fName);

    run->PrintMetadata();

    TRestG4Event* evt = new TRestG4Event();

    run->SetInputEvent(evt);

    run->GetEntry(firstEvent);

    evt->PrintEvent();

    delete run;

    return 0;
}

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
}

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