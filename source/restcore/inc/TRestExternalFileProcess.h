///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///             
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestExternalFileProcess.h
///
///             Template to use to design "event process" classes inherited from 
///             TRestProcess
///             How to use: replace TRestExternalFileProcess by your name, 
///             fill the required functions following instructions and add all
///             needed additional members and funcionality
///
///             oct 2015:   First concept
///                 Created as part of the conceptualization of existing REST 
///                 software.
///                 JuanAn
///_______________________________________________________________________________


#ifndef RestCore_TRestExternalFileProcess
#define RestCore_TRestExternalFileProcess

#include "TRestEventProcess.h"

class TRestExternalFileProcess :public TRestEventProcess {

protected:


	#ifndef __CINT__
	std::vector<FILE *> fInputExtFiles;//!
	std::vector<TString> fInputFileNames;//!

	Int_t fRunOrigin;//!
	Int_t fSubRunOrigin;//!

	Int_t nFiles;//!

	#endif

	void ReadFormat();


public:
	virtual void Initialize();

	virtual Int_t OpenInputExtFiles(vector<TString> files);

	void SetRunOrigin(Int_t runOrigin) { fRunOrigin = runOrigin; }
	void SetSubRunOrigin(Int_t subOrigin) { fSubRunOrigin = subOrigin; }

	//  Int_t GetRunNumber(){return fRunNumber;}
	//  Int_t GetRunIndex(){return fRunIndex;}


	  //Constructor
	TRestExternalFileProcess();
	TRestExternalFileProcess(char *cfgFileName);
	//Destructor
	~TRestExternalFileProcess();

	ClassDef(TRestExternalFileProcess, 1);      // Template for a REST "event process" class inherited from TRestEventProcess
};
#endif
