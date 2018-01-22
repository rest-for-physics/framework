///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///             
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestRawSignalAnalysisProcess.h
///
///_______________________________________________________________________________


#ifndef RestCore_TRestRawSignalAnalysisProcess
#define RestCore_TRestRawSignalAnalysisProcess

#include <TH1D.h>

//#include <TCanvas.h>

#include <TRestGas.h>
#include <TRestReadout.h>
#include <TRestRawSignalEvent.h>
#include <TRestHitsEvent.h>

#include "TRestEventProcess.h"

class TRestRawSignalAnalysisProcess :public TRestEventProcess {
private:
#ifndef __CINT__
	TRestRawSignalEvent *fSignalEvent;//!
									  // TODO We must get here a pointer to TRestDaqMetadata
									  // In order to convert the parameters to time using the sampling time
	TRestReadout *fReadout;//!

	Double_t fFirstEventTime;//!
	vector <Double_t> fPreviousEventTime;//!

										 //TCanvas *fCanvas;
	vector <TObject *> fDrawingObjects;//!
	Double_t fDrawRefresh;//!

	time_t timeStored;//!

	std::vector <std::string> fSignalAnalysisObservables;//!
#endif

	TPad *DrawSignal(Int_t signal);
	TPad *DrawObservables();

	void InitFromConfigFile();

	void Initialize();

	void LoadDefaultConfig();


	// parameters
	TVector2 fBaseLineRange;//!
	TVector2 fIntegralRange;//!
	Double_t fPointThreshold;//!
	Double_t fSignalThreshold;//!
	Int_t fNPointsOverThreshold;//!
	Bool_t fCutsEnabled;//!
	TVector2 fMeanBaseLineCutRange;//!
	TVector2 fMeanBaseLineSigmaCutRange;//!
	TVector2 fMaxNumberOfSignalsCut;//!
	TVector2 fMaxNumberOfGoodSignalsCut;//!
	TVector2 fFullIntegralCut;//!
	TVector2 fThresholdIntegralCut;//!
	TVector2 fPeakTimeDelayCut;//!


    // analysis result(saved directly in root file)
	TH1D *fChannelsHisto;//!

	// analysis result(saved in its branch in the analysis tree, in sequence)
	vector<Double_t> baseline;
	Double_t baselinemean;
	vector<Double_t> baselinesigma;
	Double_t baselinesigmamean;
	vector<Double_t> ampsgn_maxmethod;
	Double_t ampeve_maxmethod;
	vector<Double_t> ampsgn_intmethod;
	Double_t ampeve_intmethod;
	vector<Double_t> risetime;
	Double_t risetimemean;



public:
	void InitProcess();
	void BeginOfEventProcess();
	TRestEvent *ProcessEvent(TRestEvent *eventInput);
	void EndOfEventProcess();
	void EndProcess();

	void LoadConfig(std::string cfgFilename, std::string name = "");

	void PrintMetadata() {

		BeginPrintProcess();

		cout << "Baseline range : ( " << fBaseLineRange.X() << " , " << fBaseLineRange.Y() << " ) " << endl;
		cout << "Integral range : ( " << fIntegralRange.X() << " , " << fIntegralRange.Y() << " ) " << endl;
		cout << "Point Threshold : " << fPointThreshold << " sigmas" << endl;
		cout << "Signal threshold : " << fSignalThreshold << " sigmas" << endl;
		cout << "Number of points over threshold : " << fNPointsOverThreshold << endl;
		cout << endl;
		if (fCutsEnabled)
		{
			cout << "Cuts enabled" << endl;
			cout << "------------" << endl;
			cout << "Mean base line cut range : ( " << fMeanBaseLineCutRange.X() << " , " << fMeanBaseLineCutRange.Y() << " ) " << endl;
			cout << "Mean base line sigma cut range : ( " << fMeanBaseLineSigmaCutRange.X() << " , " << fMeanBaseLineSigmaCutRange.Y() << " ) " << endl;
			cout << "Max number of signals cut range : ( " << fMaxNumberOfSignalsCut.X() << " , " << fMaxNumberOfSignalsCut.Y() << " ) " << endl;
			cout << "Max number of good signals cut range : ( " << fMaxNumberOfGoodSignalsCut.X() << " , " << fMaxNumberOfGoodSignalsCut.Y() << " ) " << endl;
			cout << "Full integral cut range : ( " << fFullIntegralCut.X() << " , " << fFullIntegralCut.Y() << " ) " << endl;
			cout << "Threshold integral cut range : ( " << fThresholdIntegralCut.X() << " , " << fThresholdIntegralCut.Y() << " ) " << endl;
			cout << "Peak time delay cut range : ( " << fPeakTimeDelayCut.X() << " , " << fPeakTimeDelayCut.Y() << " )" << endl;
		}
		else
		{
			cout << "No cuts have been enabled" << endl;

		}

		EndPrintProcess();
	}

	TString GetProcessName() { return (TString) "rawSignalAnalysis"; }

	//Constructor
	TRestRawSignalAnalysisProcess();
	TRestRawSignalAnalysisProcess(char *cfgFileName);
	//Destructor
	~TRestRawSignalAnalysisProcess();

	ClassDef(TRestRawSignalAnalysisProcess, 1);      // Template for a REST "event process" class inherited from TRestEventProcess
};
#endif

