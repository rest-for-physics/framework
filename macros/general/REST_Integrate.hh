
#include <TSystem.h>
#include <TMath.h>
#include <TF1.h>
#include <TH1D.h>
#include <TCanvas.h>
#include <TRestRun.h>
#include <TRestTask.h>

#ifndef RestTask_Integrate
#define RestTask_Integrate


Int_t REST_General_Integrate(TString varName,
	TString rootFileName,
	double startVal,
	double endVal)
{
	TRestStringOutput cout;
	cout << "Variable name : " << varName << endl;
	cout << "Integration range : ( " << startVal << " , " << endVal << " ) " << endl;

	if (startVal >= endVal)
	{
		cout << "Start or End integration values not properly defined!!!" << endl;
		return -1;
	}

	std::vector <TString> inputFilesNew = GetFilesMatchingPattern(rootFileName);

	if (inputFilesNew.size() == 0) {
		cout << "Files not found!" << endl;
		return -1;
	}



	TRestRun *run = new TRestRun();
	Int_t integral = 0;

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
			if (value >= startVal && value < endVal) integral++;
		}

	}

	cout.setcolor(COLOR_BOLDBLUE);
	cout.setborder("*");
	cout << endl;
	cout << "=" << endl;
	cout << "Integral : " << integral << endl;
	cout << "=" << endl;
	cout << endl;

	delete run;

	return 0;

};


#endif




