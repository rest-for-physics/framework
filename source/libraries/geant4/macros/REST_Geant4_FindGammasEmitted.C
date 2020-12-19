#include "TRestGeant4Event.h"
#include "TRestGeant4Metadata.h"
#include "TRestTask.h"

#ifndef RestTask_Geant4_FindGammasEmitted
#define RestTask_Geant4_FindGammasEmitted

//*******************************************************************************************************
//***
//*** Your HELP is needed to verify, validate and document this macro
//*** This macro might need update/revision.
//***
//*******************************************************************************************************
Int_t REST_Geant4_FindGammasEmitted(TString fName) {
    cout << "Filename : " << fName << endl;

    TRestRun* run = new TRestRun();
    string fname = fName.Data();

    if (!TRestTools::fileExists(fname)) {
        cout << "WARNING. Input file does not exist" << endl;
        exit(1);
    }

    TFile* f = new TFile(fName);

    TRestGeant4Metadata* metadata = new TRestGeant4Metadata();

    // Getting metadata
    TIter nextkey(f->GetListOfKeys());
    TKey* key;
    while ((key = (TKey*)nextkey())) {
        string className = key->GetClassName();
        if (className == "TRestGeant4Metadata") {
            metadata = (TRestGeant4Metadata*)f->Get(key->GetName());
        }
        if (className == "TRestRun") {
            run = (TRestRun*)f->Get(key->GetName());
        }
    }

    if (metadata == NULL) {
        cout << "WARNING no TRestGeant4Metadata class was found" << endl;
        exit(1);
    }
    if (run == NULL) {
        cout << "WARNING no TRestRun class was found" << endl;
        exit(1);
    }

    run->PrintMetadata();

    metadata->PrintMetadata();

    TRestGeant4Event* ev = new TRestGeant4Event();

    TTree* tr = (TTree*)f->Get("TRestGeant4Event Tree");

    TBranch* br = tr->GetBranch("eventBranch");

    br->SetAddress(&ev);

    TH1D* h = new TH1D("Gammas", "Gammas emitted", 500, 3000, 3500);
    Double_t Ek = 0;
    TString pName;
    Int_t tracks = 0;
    for (int evID = 0; evID < tr->GetEntries(); evID++) {
        tr->GetEntry(evID);

        if (evID % 50000 == 0) cout << "Event : " << evID << endl;

        for (int i = 0; i < ev->GetNumberOfTracks(); i++) {
            tracks++;
            pName = ev->GetTrack(i)->GetParticleName();
            Ek = ev->GetTrack(i)->GetKineticEnergy();
            if (pName == "gamma" && Ek > 3000 && Ek < 3500) {
                h->Fill(Ek);
                //               cout << "Is a gamma. Energy : " << Ek << " keV" << endl;
            }
        }
    }

    TCanvas* c = new TCanvas("c", " ");
    h->Draw("same");
    c->Update();

    return 0;
}
#endif
