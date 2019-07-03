#include "TRestHitsEvent.h"
#include "TRestSignalEvent.h"
#include "TRestTask.h"

#ifndef RESTTask_PrintEvents
#define RESTTask_PrintEvents

Int_t REST_PrintEvents(TString fName, TString EventType = "", Int_t Entry = 0) {
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

    run->PrintInfo();

    run->GetEntry(Entry);

    evt->PrintEvent();

    delete run;

    return 0;
}
#endif