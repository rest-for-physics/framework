///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///             
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestAnalysisPlot.cxx
///
///             june 2016    Gloria Luzón/Javier Galan
///_______________________________________________________________________________


#include "TRestAnalysisPlot.h"
using namespace std;

#include <TStyle.h>



ClassImp(TRestAnalysisPlot)
//______________________________________________________________________________
TRestAnalysisPlot::TRestAnalysisPlot()
{
	Initialize();
}


void TRestAnalysisPlot::Initialize()
{
	SetSectionName(this->ClassName());

	fRun = NULL;

	fNFiles = 0;

	fCombinedCanvas = NULL;

	fStartTime = 0;
	fEndTime = 0;
}


//______________________________________________________________________________
TRestAnalysisPlot::~TRestAnalysisPlot()
{
	if (fRun != NULL) delete fRun;
}

//______________________________________________________________________________
void TRestAnalysisPlot::EndOfInit()
{

}

Int_t TRestAnalysisPlot::GetPlotIndex(TString plotName)
{

	for (unsigned int n = 0; n < fPlotNames.size(); n++)
		if (fPlotNames[n] == plotName) return n;

	cout << "REST WARNING : GetPlotIndex. Plot name " << plotName << " not found" << endl;
	return -1;
}

void TRestAnalysisPlot::PlotCombinedCanvas()
{
	// We should check that all the variables defined in the plots exist in our files
	// TOBE implemented and return a REST error in case
	if (fPlotMode == "compare")
	{
		PlotCombinedCanvasCompare();
		return;
	}

	if (fPlotMode == "add")
	{
		PlotCombinedCanvasAdd();
		return;
	}

	cout << "REST WARNING: TRestAnalysisPlot::PlotCombinedCanvas(). Plot mode (" << fPlotMode << ") not found" << endl;

}

void TRestAnalysisPlot::PlotCombinedCanvasAdd()
{

	vector <TRestRun *> runs;
	vector <TRestAnalysisTree *> trees;

	TRestRun *r;
	TRestAnalysisTree *anT;
	for (int n = 0; n < fNFiles; n++)
	{
		r = new TRestRun();
		runs.push_back(r);
		r->OpenInputFile(fFileNames[n]);
		anT = r->GetAnalysisTree();
		trees.push_back(anT);

		r->GetEntry(0);
		if (fStartTime == 0 || anT->GetTimeStamp() < fStartTime) fStartTime = anT->GetTimeStamp();

		r->GetEntry(r->GetTotalEntries() - 1);
		if (fEndTime == 0 || anT->GetTimeStamp() > fEndTime) fEndTime = anT->GetTimeStamp();
	}

	fCanvasSave = ReplaceFilenameTags(fCanvasSave, runs[0]);

	fHistoOutputFile = ReplaceFilenameTags(fHistoOutputFile, runs[0]);
	TFile *f = new TFile(fHistoOutputFile, "RECREATE");

	cout << "Saving histograms to ROOT file : " << fHistoOutputFile << endl;

	if (fCombinedCanvas != NULL)
	{
		delete fCombinedCanvas;
		fCombinedCanvas = NULL;
	}

	fCombinedCanvas = new TCanvas("combined", "combined", 0, 0, fCanvasSize.X(), fCanvasSize.Y());

	fCombinedCanvas->Divide((Int_t)fCanvasDivisions.X(), (Int_t)fCanvasDivisions.Y());

	TStyle *st = new TStyle();
	st->SetPalette(1);

	for (unsigned int n = 0; n < fPlotString.size(); n++)
	{
		fCombinedCanvas->cd(n + 1);
		if (fLogScale[n])
			fCombinedCanvas->cd(n + 1)->SetLogy();

		for (int m = 0; m < fNFiles; m++)
		{

			TString plotString = fPlotString[n];

			if (m > 0)
			{
				plotString = Replace((string)fPlotString[n], ">>", ">>+", 0);
				plotString = plotString(0, fPlotString[n].First(">>+") + 3) + fPlotNames[n];
			}

			if (GetVerboseLevel() >= REST_Debug)
			{
				cout << endl;
				cout << "--------------------------------------" << endl;
				cout << "Plot name : " << fPlotNames[n] << endl;
				cout << "Plot string : " << plotString << endl;
				cout << "Cut string : " << fCutString[n] << endl;
				cout << "Plot option : " << fPlotOption[n] << endl;
				cout << "--------------------------------------" << endl;

			}
			trees[m]->Draw(plotString, fCutString[n], fPlotOption[n]);
		}

		TH3F *htemp = (TH3F*)gPad->GetPrimitive(fPlotNames[n]);
		htemp->SetTitle(fPlotTitle[n]);
		htemp->GetXaxis()->SetTitle(fPlotXLabel[n]);
		htemp->GetYaxis()->SetTitle(fPlotYLabel[n]);

		/*
		if( fPlotXLabel[n].Contains("Time") ||  fPlotXLabel[n].Contains("time") )
		{
		cout.precision(12);
		cout << "Start : " << fStartTime << endl;
		cout << "End : " << fEndTime << endl;
		htemp->GetXaxis()->SetRangeUser( fStartTime, fEndTime );
		htemp->GetXaxis()->SetLimits( fStartTime, fEndTime );
		htemp->SetAxisRange( fStartTime, fEndTime,"X");
		GetChar();
		}
		*/


		f->cd();
		htemp->Write(fPlotNames[n]);

		if (fPlotSaveToFile[n] != "Notdefined" && fPlotSaveToFile[n] != "")
			SavePlotToPDF(fPlotNames[n], fPlotSaveToFile[n]);
		fCombinedCanvas->Update();
	}

	if (fCanvasSave != "")
		fCombinedCanvas->Print(fCanvasSave);

	f->Close();
}

void TRestAnalysisPlot::PlotCombinedCanvasCompare()
{
	vector <TRestRun *> runs;
	vector <TRestAnalysisTree *> trees;

	TRestRun *r;
	TRestAnalysisTree *anT;
	for (int n = 0; n < fNFiles; n++)
	{
		r = new TRestRun();
		runs.push_back(r);
		r->OpenInputFile(fFileNames[n]);
		anT = r->GetAnalysisTree();
		trees.push_back(anT);
	}

	fCanvasSave = ReplaceFilenameTags(fCanvasSave, runs[0]);

	if (fCombinedCanvas != NULL)
	{
		delete fCombinedCanvas;
		fCombinedCanvas = NULL;
	}

	fCombinedCanvas = new TCanvas("combined", "combined", 0, 0, fCanvasSize.X(), fCanvasSize.Y());

	fCombinedCanvas->Divide((Int_t)fCanvasDivisions.X(), (Int_t)fCanvasDivisions.Y());

	for (unsigned int n = 0; n < fPlotString.size(); n++)
	{
		fCombinedCanvas->cd(n + 1);
		if (fLogScale[n])
			fCombinedCanvas->cd(n + 1)->SetLogy();

		for (int m = 0; m < fNFiles; m++)
		{

			TString plotString = fPlotString[n];

			if (m > 0)
				plotString = fPlotString[n](0, fPlotString[n].First(">>"));

			trees[m]->SetLineColor(1 + m);
			if (m == 0)
				trees[m]->Draw(plotString, fCutString[n], "");
			else
				trees[m]->Draw(plotString, fCutString[n], "same");
		}

		TH3F *htemp = (TH3F*)gPad->GetPrimitive(fPlotNames[n]);
		htemp->SetTitle(fPlotTitle[n]);
		htemp->GetXaxis()->SetTitle(fPlotXLabel[n]);
		htemp->GetYaxis()->SetTitle(fPlotYLabel[n]);

		if (fPlotSaveToFile[n] != "Notdefined" && fPlotSaveToFile[n] != "")
			SavePlotToPDF(fPlotNames[n], fPlotSaveToFile[n]);
		fCombinedCanvas->Update();
	}

	if (fCanvasSave != "")
		fCombinedCanvas->Print(fCanvasSave);
}

void TRestAnalysisPlot::SavePlotToPDF(TString plotName, TString fileName)
{
	Int_t index = GetPlotIndex(plotName);
	if (index >= 0)
		SavePlotToPDF(index, fileName);
	else
		cout << "Save to plot failed. Plot name " << plotName << " not found" << endl;

}

void TRestAnalysisPlot::SavePlotToPDF(Int_t n, TString fileName)
{
	gErrorIgnoreLevel = 10;

	TCanvas *c = new TCanvas(fPlotNames[n], fPlotNames[n], 800, 600);

	TRestRun *run = new TRestRun();
	run->OpenInputFile(fFileNames[0]);

	TRestAnalysisTree *anTree = run->GetAnalysisTree();
	anTree->Draw(fPlotString[n], fCutString[n], "", anTree->GetEntries(), 0);

	TH3F *htemp = (TH3F*)gPad->GetPrimitive(fPlotNames[n]);
	htemp->SetTitle(fPlotTitle[n]);
	htemp->GetXaxis()->SetTitle(fPlotXLabel[n]);
	htemp->GetYaxis()->SetTitle(fPlotYLabel[n]);

	c->Print(fileName);

	delete c;
}

TString TRestAnalysisPlot::ReplaceFilenameTags(TString filename, TRestRun *run)
{


}

