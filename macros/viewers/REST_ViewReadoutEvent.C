#include "TRestBrowser.h"
#include "TRestDetectorReadoutEventViewer.h"
#include "TRestTask.h"

#ifndef RestTask_ViewReadoutEvent
#define RestTask_ViewReadoutEvent

// - Argument "readoutFilename" can be an rml file or a root file.
// - ReadoutEvent viewer differs fomr the rest of REST event-viewers in that its
//  associated class "TRestDetectorReadoutEventViewer", in short "TREV", needs to access
//  the readout file, in addition to the event file.
//   Firstly, the TREV is instantiated by the TRestBrowser ctor.
//   Then, the initialization is finalised by retrieving the just instantiated
//  TREV object, and handing it over the TRestDetectorReadout object, via TREV's
//  "SetReadout" member method.

//*******************************************************************************************************
//***
//*** Your HELP is needed to verify, validate and document this macro
//*** This macro might need update/revision.
//***
//*******************************************************************************************************
Int_t REST_ViewReadoutEvent(TString fName, TString readoutFilename = "definitions/readouts.root") {
    // Instantiate browser (and hence TREV object)
    TRestBrowser* browser = new TRestBrowser("TRestDetectorReadoutEventViewer");

    // Retrieve readout based on arg. <readoutFilename>
    TRestDetectorReadout* readout = 0;
    int length = readoutFilename.Length();
    if (((string)readoutFilename).find(".root") == length - 5) {
        // ***** <readoutFilename> IS A ROOT FILE
        // - Check that it's indeed the case.
        TFile* fFile = new TFile(readoutFilename.Data());
        if (fFile->IsZombie()) {
            printf("\n** REST_ViewReadoutEvent: Bad <readoutFilename> argument (=\"%s\") => Exit...",
                   readoutFilename.Data());
            return 1;
        }
        // - Search KEY of TRestDetectorReadout class.
        TIter nextkey(fFile->GetListOfKeys());
        TKey* key;
        while ((key = (TKey*)nextkey()) != NULL) {
            if (key->GetClassName() == (TString) "TRestDetectorReadout") {
                if (readout == NULL) readout = (TRestDetectorReadout*)fFile->Get(key->GetName());
            }
        }
        delete key;
        if (readout == NULL) {
            printf(
                "\n** REST_ViewReadoutEvent: <readoutFilename> argument (=\"%s\") contains no "
                "TRestDetectorReadout\n",
                readoutFilename.Data());
            return 1;
        }
    } else if (((string)readoutFilename).find(".rml") == length - 4) {
        // ***** <readoutFilename> IS AN RML FILE
        readout = new TRestDetectorReadout(readoutFilename);
    } else {
        printf("\n** REST_ViewReadoutEvent: Argument <readoutFilename> must be a ROOT file or an RML file\n");
        return 1;
    }

    // Finalise the initialization of TREV
    TRestDetectorReadoutEventViewer* viewer = (TRestDetectorReadoutEventViewer*)browser->GetViewer();
    viewer->SetReadout(readout);

    browser->OpenFile(fName);

// When we run this macro from restManager from bash, we need to call
// TRestMetadata::GetChar() to prevent returning, while keeping GUI alive.
#ifdef REST_MANAGER
    GetChar("Running...\nPress a key to exit");
#endif

    return 0;
}

#endif
