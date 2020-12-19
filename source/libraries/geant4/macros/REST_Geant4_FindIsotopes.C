#include "TRestGeant4Event.h"
#include "TRestGeant4Metadata.h"
#include "TRestTask.h"

#ifndef RestTask_Geant4_FindIsotopes
#define RestTask_Geant4_FindIsotopes

//*******************************************************************************************************
//***
//*** Your HELP is needed to verify, validate and document this macro
//*** This macro might need update/revision.
//***
//*******************************************************************************************************
Int_t REST_Geant4_FindIsotopes(TString fName, TString fIsotope) {
    cout << "Filename : " << fName << " looking for " << fIsotope << endl;

    // gSystem->Load("librestcore.so");

    TRestRun* run = new TRestRun();

    string fname = fName.Data();
    if (!TRestTools::fileExists(fname)) {
        cout << "WARNING. Input file does not exist" << endl;
        exit(1);
    } else
        run->OpenInputFile(fName);

    if (run == NULL) {
        cout << "WARNING no TRestGeant4Run class was found" << endl;
        exit(1);
    }

    run->OpenInputFile(fName);

    run->PrintMetadata();

    /////////////////////////////

    /////////////////////////////

    /////////////////////////////
    // Reading event
    TRestGeant4Event* ev = new TRestGeant4Event();

    run->SetInputEvent(ev);

    int j = 0;

    cout << "Total number of entries : " << run->GetEntries() << endl;

    // Printing the first event
    if (run->GetEntries() > 0) {
        for (int n = 0; n < run->GetEntries(); n++) {
            run->GetEntry(n);
            // cout<<" Event "<<n<<" has "<<ev->GetNumberOfTracks()<<" tracks"<<endl;
            for (int i = 0; i < ev->GetNumberOfTracks(); i++) {
                TString pName = ev->GetTrack(i)->GetParticleName();

                if ((ev->GetTrack(i)->GetParticleName() != "gamma") &&
                    (ev->GetTrack(i)->GetParticleName() != "e+") &&
                    (ev->GetTrack(i)->GetParticleName() != "e-")) {
                    if (ev->GetTrack(i)->GetParticleName() != "neutron")
                        cout << "Track : " << i << " --> " << ev->GetTrack(i)->GetParticleName() << "  "
                             << ev->GetTrack(i)->GetHits()->GetNumberOfHits() + 1 << " hits in volume "
                             << ev->GetTrack(i)->GetHits()->GetHitVolume(0) << endl;

                    if (pName.Contains(fIsotope)) j++;
                }
            }
        }

        // ev->PrintEvent();

        cout << "Number of " << fIsotope << " nuclei activated = " << j << endl;
    }
    /////////////////////////////

    delete run;
    delete ev;

    return 0;
}
#endif
