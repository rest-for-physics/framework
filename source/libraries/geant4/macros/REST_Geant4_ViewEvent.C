#include "TRestBrowser.h"
#include "TRestTask.h"

#ifndef RestTask_ViewG4Events
#define RestTask_ViewG4Events

//*******************************************************************************************************
//***
//*** Your HELP is needed to verify, validate and document this macro
//*** This macro might need update/revision.
//***
//*******************************************************************************************************
Int_t REST_Geant_ViewG4Event(TString fName) {
    TRestBrowser* browser = new TRestBrowser("TRestG4EventViewer");

    TRestEvent* eve = new TRestGeant4Event();
    browser->SetInputEvent(eve);

    browser->OpenFile(fName);

// when we run this macro from restManager from bash,
// we need to call TRestMetadata::GetChar() to prevent returning,
// while keeping GUI alive.
#ifdef REST_MANAGER
    GetChar("Running...\nPress a key to exit");
#endif

    return 0;
}
#endif
