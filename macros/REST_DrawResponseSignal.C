#include "TRestDetectorSignal.h"
#include "TRestTask.h"

#ifndef RESTTask_DrawResponseSignal
#define RESTTask_DrawResponseSignal

//*******************************************************************************************************
//***
//*** Your HELP is needed to verify, validate and document this macro
//*** This macro might need update/revision.
//***
//*******************************************************************************************************
Int_t REST_DrawResponseSignal(TString fName) {
    TFile* f = new TFile(fName);

    TRestDetectorSignal* signal = new TRestDetectorSignal();
    signal = (TRestDetectorSignal*)f->Get("signal Response");

    TCanvas* c = new TCanvas();
    TGraph* gr = signal->GetGraph();

    gr->Draw();

    return 0;
}
#endif
