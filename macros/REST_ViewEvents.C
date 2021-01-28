#include "TRestBrowser.h"
#include "TRestEventViewer.h"
#include "TRestTask.h"

#ifndef RestTask_ViewEvents
#define RestTask_ViewEvents

//*******************************************************************************************************
//***
//*** Your HELP is needed to verify, validate and document this macro
//*** This macro might need update/revision.
//***
//*******************************************************************************************************
Int_t REST_ViewEvents(TString fName, TString EventType = "") {
    TRestBrowser* browser = new TRestBrowser("TRestEventViewer");

    TClass* cl = TClass::GetClass(EventType);
    if (cl == NULL) {
        if (EventType != "") {
            cout << "REST WARNING : unknown event type \"" << EventType << "\"" << endl;
        }
        cout << "Using default event in file..." << endl;
        cout << endl;

        browser->OpenFile(fName);
    } else {
        TRestEvent* eve = (TRestEvent*)cl->New();
        browser->SetInputEvent(eve);
        // viewer->SetEvent(eve);
        browser->OpenFile(fName);
    }

#ifdef REST_MANAGER
    GetChar("Running...\nPress a key to exit");
#endif

    return 0;
}

/*
Int_t REST_ViewLinearTrackEvent(TString fName) {
    REST_ViewEvents(fName, "TRestLinearTrackEvent");
    return 0;
} */

Int_t REST_ViewSignalEvent(TString fName) {
    REST_ViewEvents(fName, "TRestDetectorSignalEvent");
    return 0;
}

#endif  // !REST_ViewEvents
