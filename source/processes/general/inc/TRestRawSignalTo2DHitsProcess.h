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
		TRest2DHitsEvent *fOutputSignalEvent;//!

		TRestReadout* fReadout;

        void InitFromConfigFile();

        void Initialize();

        // add here the metadata members of your event process
        // You can just remove fMyProcessParameter
        Int_t fMyDummyParameter;

    protected:

    public:
		void InitProcess();

        TRestEvent *ProcessEvent( TRestEvent *eventInput );

        void PrintMetadata() 
        { 
            BeginPrintProcess();

            std::cout << "A dummy Process parameter : " << fMyDummyParameter << std::endl;

            EndPrintProcess();
        }

        //Constructor
        TRestRawSignalTo2DHitsProcess();
        //Destructor
        ~TRestRawSignalTo2DHitsProcess();

        ClassDef(TRestRawSignalTo2DHitsProcess, 1);      // Template for a REST "event process" class inherited from TRestEventProcess
};
#endif

