///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///             
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestMultiCoBoAsAdToSignalProcess.h
///
///             Template to use to design "event process" classes inherited from 
///             TRestProcess
///             How to use: replace TRestMultiCoBoAsAdToSignalProcess by your name, 
///             fill the required functions following instructions and add all
///             needed additional members and funcionality
///
///             May 2017:   First concept
///                 Created as part of the conceptualization of existing REST software.
///                 Javier Galan
///_______________________________________________________________________________


#ifndef RestCore_TRestMultiCoBoAsAdToSignalProcess
#define RestCore_TRestMultiCoBoAsAdToSignalProcess

#include "TRestRawToSignalProcess.h"
#include "TRestSignalEvent.h"

struct CoBoDataFrame {
  TTimeStamp timeStamp;
  Bool_t chHit[272];
  Int_t data[272][512];
  Int_t evId;
  Int_t asadId;
};

class TRestMultiCoBoAsAdToSignalProcess:public TRestRawToSignalProcess {

	private:

		TTimeStamp fStartTimeStamp;

		Int_t nFiles;

#ifndef __CINT__

		std::vector <FILE *> fInputMultiBinFiles;

		std::vector <CoBoDataFrame> fDataFrame;

		TRestRawSignal sgnl;
#endif

	public:
		void InitProcess();
		void Initialize();

		Bool_t OpenInputMultiCoBoAsAdBinFile( std::vector <TString> fNames );

		Bool_t InitializeStartTimeStampFromFilename( TString fName );

		TRestEvent *ProcessEvent( TRestEvent *evInput );

		Bool_t FillBuffer( Int_t n );
		void ClearBuffer( Int_t n );

		Int_t GetLowestEventId(  );

		Bool_t EndReading( );

		TString GetProcessName(){ return (TString) "MultiCoBoAsAdToSignal"; }

		//Constructor
		TRestMultiCoBoAsAdToSignalProcess();
		TRestMultiCoBoAsAdToSignalProcess(char *cfgFileName);
		//Destructor
		~TRestMultiCoBoAsAdToSignalProcess();

		ClassDef(TRestMultiCoBoAsAdToSignalProcess, 1);      // Template for a REST "event process" class inherited from TRestEventProcess
};
#endif
