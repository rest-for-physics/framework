#include "TRestGeant4Event.h"
#include "TRestGeant4Metadata.h"
#include "TRestTask.h"

#ifndef RestTask_ListIsotopes
#define RestTask_ListIsotopes

//*******************************************************************************************************
//***
//*** Your HELP is needed to verify, validate and document this macro
//*** This macro might need update/revision.
//***
//*******************************************************************************************************
Int_t REST_Geant4_ListIsotopes(TString fName, TString fOutName) {
    cout << "Filename : " << fName << ", storing info in " << fOutName << endl;

    ofstream fOut;
    fOut.open(fOutName);

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

    cout << "Total number of entries : " << run->GetEntries() << endl;

    int k = 0, kaux = 0;
    const Int_t nx = 2000;
    TString isotope[nx];
    int ni[nx];

    if (run->GetEntries() > 0) {
        // isotope[0]="e-";

        for (int n = 0; n < run->GetEntries(); n++) {
            run->GetEntry(n);

            for (int i = 0; i < ev->GetNumberOfTracks(); i++) {
                kaux = 0;

                if ((ev->GetTrack(i)->GetParticleName() != "e+") &&
                    (ev->GetTrack(i)->GetParticleName() != "e-")) {
                    if (ev->GetTrack(i)->GetParticleName() != "gamma") {
                        for (int j = 0; j < k + 1; j++) {
                            // cout<<"i "<<i<<" j "<<j<<" "<<isotope[j]<<"
                            // "<<ev->GetTrack(i)->GetParticleName()<<" k"<<k<<endl;

                            if (ev->GetTrack(i)->GetParticleName() == isotope[j]) {
                                ni[j]++;

                                kaux = 1;
                            }
                        }
                        if (kaux != 1) {
                            // cout<<"new isotope "<<ev->GetTrack(i)->GetParticleName()<<" in track "<<i<<"and
                            // entry"<<n<<endl;
                            isotope[k] = ev->GetTrack(i)->GetParticleName();
                            ni[k] = 1;
                            // cout<<"k "<<k<<" isotope "<<isotope[k]<<endl;
                            k++;
                            kaux = 0;
                        }
                    }
                }
            }
        }
    }

    for (int m = 0; m < k; m++) fOut << isotope[m] << " " << ni[m] << endl;

    /////////////////////////////
    fOut.close();
    cout << "closing file" << endl;

    delete run;
    delete ev;

    return 0;
}
#endif
