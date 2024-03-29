
#include <TCanvas.h>
#include <TF1.h>
#include <TH1D.h>
#include <TMath.h>
#include <TRestRun.h>
#include <TRestTask.h>
#include <TSystem.h>

#ifndef RestTask_CreateHisto
#define RestTask_CreateHisto

//*******************************************************************************************************
//***
//*** Your HELP is needed to verify, validate and document this macro
//*** This macro might need update/revision.
//***
//*******************************************************************************************************
Int_t REST_CreateHisto(string varName, string rootFileName, TString histoName, int startVal = 0,
                       int endVal = 1000, int bins = 1000, Double_t normFactor = 1) {
    TRestStringOutput RESTLog;

    std::vector<string> inputFilesNew = TRestTools::GetFilesMatchingPattern(rootFileName);

    TH1D* h = new TH1D(histoName, histoName, bins, startVal, endVal);

    if (inputFilesNew.size() == 0) {
        RESTLog << "Files not found!" << RESTendl;
        return -1;
    }

    for (unsigned int n = 0; n < inputFilesNew.size(); n++) {
        TRestRun* run = new TRestRun();

        run->OpenInputFile(inputFilesNew[n]);

        run->PrintMetadata();

        Int_t obsID = run->GetAnalysisTree()->GetObservableID(varName);
        if (obsID == -1) {
            RESTLog << RESTendl;
            RESTLog.setcolor(COLOR_BOLDRED);
            RESTLog << "No observable \"" << varName << "\" in file " << inputFilesNew[n] << RESTendl;
            continue;
        }
        for (int i = 0; i < run->GetEntries(); i++) {
            run->GetAnalysisTree()->GetBranch((TString)varName)->GetEntry(i);
            Double_t val = run->GetAnalysisTree()->GetDblObservableValue(obsID);
            if (val >= startVal && val <= endVal) h->Fill(val);
        }

        delete run;
    }

    h->Scale(normFactor);

    TFile* f = new TFile((TString)rootFileName, "update");
    h->Write(histoName);
    f->Close();

    RESTLog << "Written histogram " << histoName << " into " << rootFileName << RESTendl;

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
