#include "TRestGeant4Event.h"
#include "TRestGeant4Metadata.h"
#include "TRestTask.h"

Int_t REST_MeanTrackLength(TString fName) {
    const double eMin_ROI = 2458 - 25;
    const double eMax_ROI = 2458 + 25;

    const Double_t xMax = 4000;
    const Double_t yMax = 4000;
    const Double_t zMax = 4000;

    Int_t nbinsROI = (Int_t)(eMax_ROI - eMin_ROI);

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

    Double_t meanLength = 0;
    Int_t tracks = 0;
    for (int evID = 0; evID < tr->GetEntries(); evID++) {
        tr->GetEntry(evID);
        if (evID % 100000 == 0) cout << "Event : " << evID << endl;

        for (int i = 0; i < ev->GetNumberOfTracks(); i++) {
            tracks++;
            meanLength += ev->GetTrack(i)->GetTrackLength();
        }
    }

    cout << "Mean track length : " << meanLength / tracks << endl;

    delete ev;

    f->Close();

    return 0;
}
