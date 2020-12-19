#include "TRestGeant4Event.h"
#include "TRestGeant4Metadata.h"
#include "TRestTask.h"

#ifndef RestTask_Geant4_ReadNEvents
#define RestTask_Geant4_ReadNEvents

//*******************************************************************************************************
//***
//*** Your HELP is needed to verify, validate and document this macro
//*** This macro might need update/revision.
//***
//*******************************************************************************************************
Int_t REST_Geant4_ReadNEvents(TString fName, int n1, int n2) {
    TRestRun* run = new TRestRun();

    string fname = fName.Data();
    if (!TRestTools::fileExists(fname)) {
        cout << "WARNING. Input file does not exist" << endl;
        exit(1);
    } else
        run->OpenInputFile(fName);

    run->OpenInputFile(fName);

    run->PrintMetadata();

    /////////////////////////////

    /////////////////////////////
    // Reading events

    TRestGeant4Event* ev = new TRestGeant4Event();

    run->SetInputEvent(ev);
    for (int i = n1; i < n2 + 1; i++) {
        run->GetEntry(i);

        // Do what ever you want with g4Event
        ev->PrintEvent();
    }

    delete ev;

    /////////////////////////////

    delete run;

    return 0;
}
#endif
