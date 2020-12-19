#include "TRestGeant4Event.h"
#include "TRestGeant4Metadata.h"
#include "TRestTask.h"
// Double_t Qbb = 2457.83;

Double_t zMin = 0;
Double_t zMax = -1000;

Double_t radius = 63.0;

#ifndef RestTask_GetROIEvents
#define RestTask_GetROIEvents

//*******************************************************************************************************
//***
//*** Your HELP is needed to verify, validate and document this macro
//*** This macro might need update/revision.
//***
//*******************************************************************************************************
Double_t REST_Geant4_GetROIEvents(TString fName, Double_t mean = 2457.83, Double_t fwhm = 0.03) {
    Double_t sigma = mean * fwhm / (2 * TMath::Sqrt(2 * TMath::Log(2.)));

    cout << "Filename : " << fName << endl;

    TRestRun* run = new TRestRun();
    string fname = fName.Data();
    if (!TRestTools::fileExists(fname)) {
        cout << "WARNING. Input file does not exist" << endl;
        exit(1);
    }

    TFile* f = new TFile(fName);

    TH1D* h = (TH1D*)f->Get("Total energy deposited");

    TF1* gausFunc = new TF1("g", "[0]*exp(-0.5*((x-[1])/[2])**2)", 0, 10000);

    Double_t totalContribution = 0;
    for (int i = 1; i <= h->GetNbinsX(); i++) {
        Double_t en = h->GetXaxis()->GetBinCenter(i);
        Double_t counts = h->GetBinContent(i);

        gausFunc->SetParameters(counts / TMath::Sqrt(2 * TMath::Pi()) / sigma, en, sigma);
        totalContribution += gausFunc->Integral(mean - 2 * sigma, mean + 2 * sigma);
        //       cout << "Energy : " << en << " keV counts : " << counts <<  " contribution : " <<
        //       totalContribution  << endl;
    }

    cout << "Total contribution in the ROI : " << totalContribution << endl;

    /*
    cout << "ROI("<<fwhm<<"keV) : " <<smeared->Integral(  ) << endl;

    TCanvas *c1 = new TCanvas();
    smeared->Draw("");
    */
    return totalContribution;
}
#endif
