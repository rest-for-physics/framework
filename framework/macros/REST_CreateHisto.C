
#include <TCanvas.h>
#include <TF1.h>
#include <TH1D.h>
#include <TMath.h>
#include <TRestRun.h>
#include <TRestTask.h>
#include <TSystem.h>

#ifndef RestTask_CreateHisto
#define RestTask_CreateHisto

Int_t REST_CreateHisto(TString varName, TString rootFileName, TString histoName, int startVal = 0,
                       int endVal = 1000, int bins = 1000, Double_t normFactor = 1) {
    TRestStringOutput cout;

    std::vector<string> inputFilesNew = TRestTools::GetFilesMatchingPattern((string)rootFileName);

    TH1D* h = new TH1D(histoName, histoName, bins, startVal, endVal);

    if (inputFilesNew.size() == 0) {
        cout << "Files not found!" << endl;
        return -1;
    }

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
        for (int i = 0; i < run->GetEntries(); i++) {
            run->GetAnalysisTree()->GetBranch(varName)->GetEntry(i);
            Double_t val = run->GetAnalysisTree()->GetObservableValue(obsID);
            if (val >= startVal && val <= endVal) h->Fill(val);
        }

        delete run;
    }

    h->Scale(normFactor);

    TFile* f = new TFile(rootFileName, "update");
    h->Write(histoName);
    f->Close();

    cout << "Written histogram " << histoName << " into " << rootFileName << endl;

    return 0;
};

    //
    // class REST_CreateHisto :public TRestTask {
    // public:
    //	ClassDef(REST_CreateHisto, 1);
    //
    //	REST_CreateHisto() { fNRequiredArgument = 3; }
    //	~REST_CreateHisto() {}
    //
    //	TString varName="";
    //	TString rootFileName="";
    //	TString histoName="";
    //	int startVal = 0;
    //	int endVal = 1000;
    //	int bins = 1000;
    //	Double_t normFactor = 1;
    //
    //	void RunTask(TRestManager*mgr)
    //	{
    //		REST_CreateHisto(varName,
    //				rootFileName,
    //				histoName,
    //				startVal,
    //				endVal,
    //				bins,
    //				normFactor);
    //	}
    //
    //};

#endif
