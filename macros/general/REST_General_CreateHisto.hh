
#include <TSystem.h>
#include <TMath.h>
#include <TF1.h>
#include <TH1D.h>
#include <TCanvas.h>
#include <TRestRun.h>
#include <TRestTask.h>

#ifndef RestTask_CreateHisto
#define RestTask_CreateHisto



Int_t REST_General_CreateHisto(char varName[],
	char rootFileName[],
	char histoName[],
	int startVal = 0,
	int endVal = 1000,
	int bins = 1000,
	Double_t normFactor = 1)
{
	std::vector <TString> inputFilesNew = GetFilesMatchingPattern(rootFileName);

	TH1D *h = new TH1D(histoName, histoName, bins, startVal, endVal);

	for (unsigned int n = 0; n < inputFilesNew.size(); n++)
	{
		TRestRun *run = new TRestRun();

		run->OpenInputFile(inputFilesNew[n]);

		run->SkipEventTree();
		run->PrintInfo();

		Int_t obsID = run->GetAnalysisTree()->GetObservableID(varName);

		for (int i = 0; i < run->GetEntries(); i++)
		{
			run->GetEntry(i);
			Double_t val = run->GetAnalysisTree()->GetObservableValue(obsID);
			if (val >= startVal && val <= endVal)
				h->Fill(val);
		}

		delete run;
	}

	h->Scale(normFactor);

	TFile *f = new TFile(rootFileName, "update");
	h->Write(histoName);
	f->Close();

	cout << "Written histogram " << histoName << " into " << rootFileName << endl;

	return 0;

};



class CreateHisto :public TRestTask {
public:
	ClassDef(CreateHisto, 1);
};

#endif




