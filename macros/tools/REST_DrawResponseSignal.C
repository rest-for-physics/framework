#include "TRestSignal.h"
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

    TRestSignal* signal = new TRestSignal();
    signal = (TRestSignal*)f->Get("signal Response");

    TCanvas* c = new TCanvas();
    TGraph* gr = signal->GetGraph();

    gr->Draw();

    return 0;
}
#endif
