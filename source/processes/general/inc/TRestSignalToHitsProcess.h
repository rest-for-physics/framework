///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///             
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestSignalToHitsProcess.h
///
///_______________________________________________________________________________


#ifndef RestCore_TRestSignalToHitsProcess
#define RestCore_TRestSignalToHitsProcess

#include <TRestGas.h>
#include <TRestReadout.h>

#include <TRestSignalEvent.h>
#include <TRestHitsEvent.h>

#include "TRestEventProcess.h"

class TRestSignalToHitsProcess :public TRestEventProcess {
private:

#ifndef __CINT__
	TRestHitsEvent *fHitsEvent;//!
	TRestSignalEvent *fSignalEvent;//!

	TRestReadout *fReadout;//!
	TRestGas *fGas;//!
#endif

	void InitFromConfigFile();

	void Initialize();

	void LoadDefaultConfig();

protected:

	Double_t fElectricField; // V/cm
	Double_t fGasPressure; // atm
	Double_t fDriftVelocity; // mm/us

	TString fSignalToHitMethod;

public:

	void InitProcess();
	void BeginOfEventProcess();
	TRestEvent *ProcessEvent(TRestEvent *eventInput);
	void EndOfEventProcess();
	void EndProcess();

	void LoadConfig(std::string cfgFilename, std::string name = "");

	void PrintMetadata()
	{
		BeginPrintProcess();

		essential << "Electric field : " << fElectricField << " V/cm" << endl;
		essential << "Gas pressure : " << fGasPressure << " atm" << endl;
		essential << "Drift velocity : " << fDriftVelocity << " mm/us" << endl;

		essential << "Signal to hits method : " << fSignalToHitMethod << endl;

		EndPrintProcess();
	}

	TString GetProcessName() { return (TString) "signalToHits"; }

	//Constructor
	TRestSignalToHitsProcess();
	TRestSignalToHitsProcess(char *cfgFileName);
	//Destructor
	~TRestSignalToHitsProcess();

	ClassDef(TRestSignalToHitsProcess, 2);      // Template for a REST "event process" class inherited from TRestEventProcess
};
#endif

