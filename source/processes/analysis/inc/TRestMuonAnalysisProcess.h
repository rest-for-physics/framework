///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///             
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestMuonAnalysisProcess.h
///
///_______________________________________________________________________________


#ifndef _TRestMuonAnalysisProcess
#define _TRestMuonAnalysisProcess

#include "TRestEventProcess.h"
#include "TRest2DHitsEvent.h"

#define stripWidth 3

class TRestMuonAnalysisProcess:public TRestEventProcess {

    private:

        // We define specific input/output event data holders
        TRest2DHitsEvent *fAnaEvent;//!

		TH1D*hxzt;//!
		TH1D*hyzt;//!
		TH1D*hxzr;//!
		TH1D*hyzr;//!

		int X1;//!
		int X2;//!
		int Y1;//!
		int Y2;//!

		int nummudeposxz;//!
		int nummudeposyz;//!
		int numsmear;//!
		TH1D* mudeposxz;//!
		TH1D* mudeposyz;//!

		TH1D* mudepos;//!
		TH1D* musmear;//!
		TH1D* mutanthe;//!


        void InitFromConfigFile();

        void Initialize();

        // add here the metadata members of your event process
        // You can just remove fMyProcessParameter
        Int_t fMyDummyParameter;

    protected:

    public:
		void InitProcess();

        TRestEvent *ProcessEvent( TRestEvent *eventInput );

		void EndProcess();

        void PrintMetadata() 
        { 
            BeginPrintProcess();

            std::cout << "A dummy Process parameter : " << fMyDummyParameter << std::endl;

            EndPrintProcess();
        }

        //Constructor
        TRestMuonAnalysisProcess();
        //Destructor
        ~TRestMuonAnalysisProcess();

        ClassDef(TRestMuonAnalysisProcess, 1);      // Template for a REST "event process" class inherited from TRestEventProcess
};
#endif

