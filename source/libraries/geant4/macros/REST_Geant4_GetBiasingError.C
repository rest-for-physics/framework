#include "TRestGeant4Event.h"
#include "TRestGeant4Metadata.h"
#include "TRestTask.h"

#ifndef RestTask_GetBiasingError
#define RestTask_GetBiasingError

//*******************************************************************************************************
//***
//*** Your HELP is needed to verify, validate and document this macro
//*** This macro might need update/revision.
//***
//*******************************************************************************************************
Double_t REST_Geant4_GetBiasingError(TString fName, Int_t finalEvents = 0) {
    cout << "Filename : " << fName << endl;

    TRestRun* run = new TRestRun();
    TRestGeant4Metadata* metadata = new TRestGeant4Metadata();

    string fname = fName.Data();
    if (!TRestTools::fileExists(fname)) {
        cout << "WARNING. Input file does not exist" << endl;
        exit(1);
    }

    TFile* f = new TFile(fName);

    /////////////////////////////
    // Reading metadata classes

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

    /////////////////////////////
    run->PrintMetadata();
    metadata->PrintMetadata();
    /////////////////////////////

    // Reading event
    TRestGeant4Event* ev = new TRestGeant4Event();
    TTree* tr = (TTree*)f->Get("TRestGeant4Event Tree");
    TBranch* br = tr->GetBranch("eventBranch");

    br->SetAddress(&ev);

    cout << "Number of biasing volumes : " << metadata->GetNumberOfBiasingVolumes() << endl;

    Double_t sum = 0;
    TH1D* h;
    for (int i = 0; i < metadata->GetNumberOfBiasingVolumes(); i++) {
        TString index;
        index.Form("%d", i);

        TString biasSpctName = "Bias_Spectrum_" + index;
        h = (TH1D*)f->Get(biasSpctName);
        cout << "Number of counts in bias " << i << " " << h->Integral() << endl;
        sum += 1. / h->Integral();
    }

    h = (TH1D*)f->Get("Total energy deposited (ROI)");
    sum += 1. / h->Integral();

    Double_t roiEvents = 0;
    if (finalEvents > 0)
        roiEvents = finalEvents;
    else
        roiEvents = h->Integral();

    sum += 1. / roiEvents;

    cout << "Number of events in ROI : " << roiEvents << endl;

    Double_t error = roiEvents * TMath::Sqrt(sum);

    cout << "The error is : " << error << endl;

    delete run;
    delete metadata;

    delete ev;

    f->Close();

    return error;
}
#endif
