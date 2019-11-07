
#include <TCanvas.h>
#include <TF1.h>
#include <TH1D.h>
#include <TMath.h>
#include <TRestRun.h>
#include <TRestTask.h>
#include <TSystem.h>

#ifndef RestTask_IntegrateSmearing
#define RestTask_IntegrateSmearing

Int_t REST_IntegrateSmearing(TString varName, TString rootFileName, double Middle) {
    TRestStringOutput cout;

    std::vector<string> inputFilesNew = TRestTools::GetFilesMatchingPattern((string)rootFileName);

    if (inputFilesNew.size() == 0) {
        cout << "Files not found!" << endl;
        return -1;
    }

    Double_t Qbb = 2457.83;
    Double_t mean = Qbb;
    Double_t sigma_1 = mean * 0.005 / (2 * TMath::Sqrt(2 * TMath::Log(2.)));
    Double_t sigma_2 = mean * 0.01 / (2 * TMath::Sqrt(2 * TMath::Log(2.)));
    Double_t sigma_3 = mean * 0.03 / (2 * TMath::Sqrt(2 * TMath::Log(2.)));

    for (unsigned int n = 0; n < inputFilesNew.size(); n++) {
        TRestRun* run = new TRestRun();

        run->OpenInputFile(inputFilesNew[n]);

        run->PrintInfo();

        Int_t obsID = run->GetAnalysisTree()->GetObservableID(varName);
        if (obsID == -1) {
            cout << endl;
            cout.setcolor(COLOR_BOLDRED);
            cout << "No observable \"" << varName << "\" in file " << inputFilesNew[n] << endl;
            continue;
        }
        cout << "Entries : " << run->GetEntries() << endl;
        TH1D* h = new TH1D("histo", "histo", 180, Qbb - 90, Qbb + 90);
        Int_t peak = 0;
        for (int i = 0; i < run->GetEntries(); i++) {
            run->GetAnalysisTree()->GetBranch(varName)->GetEntry(i);
            Double_t en = run->GetAnalysisTree()->GetDblObservableValue(obsID);
            if (en > Qbb - 5 && en < Qbb + 5) peak++;

            h->Fill(en);
        }

        h->Draw();

        TF1* gausFunc = new TF1("g", "[0]*exp(-0.5*((x-[1])/[2])**2)", 0, 10000);

        Double_t contribution_1 = 0;
        Double_t contribution_2 = 0;
        Double_t contribution_3 = 0;
        for (int i = 1; i <= h->GetNbinsX(); i++) {
            Double_t en = h->GetXaxis()->GetBinCenter(i);
            Double_t counts = h->GetBinContent(i);

            gausFunc->SetParameters(counts / TMath::Sqrt(2 * TMath::Pi()) / sigma_1, en, sigma_1);
            contribution_1 += gausFunc->Integral(mean - 2 * sigma_1, mean + 2 * sigma_1);

            gausFunc->SetParameters(counts / TMath::Sqrt(2 * TMath::Pi()) / sigma_2, en, sigma_2);
            contribution_2 += gausFunc->Integral(mean - 2 * sigma_2, mean + 2 * sigma_2);

            gausFunc->SetParameters(counts / TMath::Sqrt(2 * TMath::Pi()) / sigma_3, en, sigma_3);
            contribution_3 += gausFunc->Integral(mean - 2 * sigma_3, mean + 2 * sigma_3);
        }

        cout.setcolor(COLOR_BOLDBLUE);
        cout.setborder("*");
        cout << endl;
        cout << "=" << endl;
        cout << "Total events : " << run->GetEntries() << endl;
        cout << " " << endl;
        cout << "FWHM = 0.5% -> " << contribution_1 << endl;
        cout << "FWHM = 1.% -> " << contribution_2 << endl;
        cout << "FWHM = 3.% -> " << contribution_3 << endl;
        cout << "peak : " << peak << endl;
        cout << "=" << endl;
        cout << endl;

        delete run;
    }

    return 0;
};

#endif