
#include <TSystem.h>
#include <TMath.h>
#include <TF1.h>
#include <TH1D.h>
#include <TCanvas.h>
#include <TRestRun.h>
#include <TRestTask.h>

#ifndef RestTask_Fit
#define RestTask_Fit


Int_t REST_General_Fit(TString varName,
	TString rootFileName,
	double startVal,
	double endVal,
	int nBins = 100)
{
	TRestStringOutput cout;
	cout << "Variable name : " << varName << endl;
	cout << "Fit range : ( " << startVal << " , " << endVal << " ) " << endl;

	if (startVal >= endVal)
	{
		cout << "Start or End integration values not properly defined!!!" << endl;
		return -1;
	}

	std::vector <TString> inputFilesNew = GetFilesMatchingPattern(rootFileName);

	TH1D *h = new TH1D("FitHisto", "FitHisto", nBins, startVal, endVal);

	TRestRun *run = new TRestRun();

	if (inputFilesNew.size() == 0) {
		cout << "Files not found!" << endl;
		return -1;
	}

	for (unsigned int n = 0; n < inputFilesNew.size(); n++)
	{
		run->OpenInputFile(inputFilesNew[n]);

		run->SkipEventTree();
		run->PrintInfo();

		Int_t obsID = run->GetAnalysisTree()->GetObservableID(varName);
		if (obsID == -1) {
			cout << "No observable \"" << varName << "\" in file " << inputFilesNew[n] << endl;
			continue;
		}
		for (int i = 0; i < run->GetEntries(); i++)
		{
			run->GetEntry(i);
			Double_t value = run->GetAnalysisTree()->GetObservableValue(obsID);
			if (value >= startVal && value < endVal)
			{
				h->Fill(value);
			}
		}

	}

	h->Fit("gaus");
	h->Draw();

	run->GetChar();

	delete run;

	return 0;

};


#endif




