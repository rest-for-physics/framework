#include "TRestSignal.h"
#include "TRestTask.h"
Int_t REST_DrawResponseSignal(TString fName) {
    TFile* f = new TFile(fName);

    TRestSignal* signal = new TRestSignal();
    signal = (TRestSignal*)f->Get("signal Response");

    TCanvas* c = new TCanvas();
    TGraph* gr = signal->GetGraph();

    gr->Draw();

    return 0;
}
