///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///             
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestRawSignalTo2DHitsProcess.h
///
///_______________________________________________________________________________


#ifndef RESTCore_TRestRawSignalTo2DHitsProcess
#define RESTCore_TRestRawSignalTo2DHitsProcess

#include "TRestEventProcess.h"
#include "TRestRawSignalEvent.h"
#include "TRest2DHitsEvent.h"
#include "TRestSignalEvent.h"

class TRestRawSignalTo2DHitsProcess:public TRestEventProcess {

    private:

        // We define specific input/output event data holders
        TRestRawSignalEvent *fInputSignalEvent;//!
		TRest2DHitsEvent *fOutput2DHitsEvent;//!

		TRestReadout* fReadout;//!

        void InitFromConfigFile();

        void Initialize();

        // add here the metadata members of your event process
        // You can just remove fMyProcessParameter
		int fNoiseReductionLevel;//0: no reduction, 1: subtract baseline, 2: subtract baseline plus threshold

		string fSelection;//0: uses all, 1: muon, 2: electron, 3: weak source, 4: firing, 5: other

		TVector2 fBaseLineRange;//!
		TVector2 fIntegralRange;//!
		Double_t fPointThreshold;//!
		Double_t fSignalThreshold;//!
		Int_t fNPointsOverThreshold;//!

		vector<TVector3> fHough_XZ; //y=ax+b, vertical line angle 牟, length 老, [id][老,牟,weight]
		vector<TVector3> fHough_YZ; //y=ax+b, vertical line angle 牟, length 老, [id][老,牟,weight]

    protected:

    public:
		void InitProcess();

        TRestEvent *ProcessEvent( TRestEvent *eventInput );

        void PrintMetadata() 
        { 
            BeginPrintProcess();

            std::cout << "Noise Reduction Level : " << fNoiseReductionLevel << std::endl;
			std::cout << "Event selection: " << fSelection << std::endl;

            EndPrintProcess();
        }

        //Constructor
        TRestRawSignalTo2DHitsProcess();
        //Destructor
        ~TRestRawSignalTo2DHitsProcess();

        ClassDef(TRestRawSignalTo2DHitsProcess, 1);      // Template for a REST "event process" class inherited from TRestEventProcess
};
#endif

