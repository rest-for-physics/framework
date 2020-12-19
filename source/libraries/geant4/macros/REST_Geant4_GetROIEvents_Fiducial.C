#include "TRestGeant4Event.h"
#include "TRestGeant4Metadata.h"
#include "TRestTask.h"
// Double_t Qbb = 2457.83;

#ifndef RestTask_GetROIEventsFiducial
#define RestTask_GetROIEventsFiducial

//*******************************************************************************************************
//***
//*** Your HELP is needed to verify, validate and document this macro
//*** This macro might need update/revision.
//***
//*******************************************************************************************************
Double_t REST_Geant4_GetROIEventsFiducial(TString fName, Double_t zMin, Double_t zMax, Double_t radius,
                                          Double_t mean = 2457.83, Double_t fwhm = 25) {
    cout << "Filename : " << fName << endl;

    TRestRun* run = new TRestRun();
    string fname = fName.Data();
    if (!TRestTools::fileExists(fname)) {
        cout << "WARNING. Input file does not exist" << endl;
        exit(1);
    } else
        run->OpenInputFile(fName);

    TH1D* h = new TH1D("Spectrum", "Spectrum", 5000, 0, 5000);

    TF1* gausFunc = new TF1("g", "[0]*exp(-0.5*((x-[1])/[2])**2)", 0, 10000);

    double sigma = 0.425 * fwhm;

    TRestGeant4Event* ev = new TRestGeant4Event();

    run->SetInputEvent(ev);

    cout << "Total number of entries : " << run->GetEntries() << endl;

    // Printing the first event

    for (int i = 0; i < run->GetEntries(); i++) {
        Bool_t veto = false;

        if (i % 10000 == 0) cout << "Event " << i << endl;

        run->GetEntry(i);

        Double_t eDep = 0;
        for (int j = 0; j < ev->GetNumberOfTracks(); j++) {
            for (int k = 0; k < ev->GetTrack(j)->GetNumberOfHits(); k++) {
                if (ev->GetTrack(j)->GetHits()->GetEnergy(k) > 0) {
                    Double_t z = ev->GetTrack(j)->GetHits()->GetZ(k);
                    if (z > zMin && z < zMax) {
                        Double_t x = ev->GetTrack(j)->GetHits()->GetX(k);
                        Double_t y = ev->GetTrack(j)->GetHits()->GetY(k);

                        Double_t r = TMath::Sqrt(x * x + y * y);

                        if (r < radius) eDep += ev->GetTrack(j)->GetHits()->GetEnergy(k);
                        //           if( r > radius - 50 ) veto = true;
                    }
                }
            }
        }

        if (eDep > 0 && !veto) h->Fill(eDep);
    }

    TCanvas* c1 = new TCanvas();
    h->Draw("");

    Double_t totalContribution = 0;
    for (int i = 1; i <= h->GetNbinsX(); i++) {
        Double_t en = h->GetXaxis()->GetBinCenter(i);
        Double_t counts = h->GetBinContent(i);

        gausFunc->SetParameters(counts / TMath::Sqrt(2 * TMath::Pi()) / sigma, en, sigma);
        totalContribution += gausFunc->Integral(mean - fwhm, mean + fwhm);
    }

    cout << "Total contribution in the ROI : " << totalContribution << endl;

    /*
    cout << "ROI("<<fwhm<<"keV) : " <<smeared->Integral(  ) << endl;

    TCanvas *c1 = new TCanvas();
    smeared->Draw("");
    */
    return 0;
}
#endif
