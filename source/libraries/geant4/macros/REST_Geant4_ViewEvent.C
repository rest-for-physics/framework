#include "TRestBrowser.h"
#include "TRestTask.h"

#ifndef RestTask_ViewG4Events
#define RestTask_ViewG4Events

Int_t REST_Geant4_ViewEvent(TString fName) {
    TRestBrowser* browser = new TRestBrowser("TRestGeant4EventViewer");

    TRestEvent* eve = new TRestGeant4Event();
    browser->SetInputEvent(eve);

    browser->OpenFile(fName);

// when we run this macro from restManager from bash,
// we need to call TRestMetadata::GetChar() to prevent returning,
// while keeping GUI alive.
#ifdef REST_MANAGER
    browser->GetChar("Running...\nPress a key to exit");
#endif

    return 0;
}
#endif
