///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///             
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             myProcess.h
///
///_______________________________________________________________________________


#ifndef _myProcess
#define _myProcess

#include <TRestSignalEvent.h>

#include "TRestEventProcess.h"

class myProcess:public TRestEventProcess {

    private:
        TRestSignalEvent *fInputSignalEvent;
        TRestSignalEvent *fOutputSignalEvent;

        void InitFromConfigFile();

        void Initialize();

        void LoadDefaultConfig();

        //add here the metadata members of your event process
        Int_t fMyProcessParameter;

    protected:

    public:
 //       void InitProcess() { cout << "myProcess::InitProcess. Implement this if you need to do something before start processing any event." << endl; }
 //       void BeginOfEventProcess() { cout << "myProcess::BeginOfEventProcess. Implement this if you need to do something at the beginning of each event process." << endl; }
        TRestEvent *ProcessEvent( TRestEvent *eventInput );
 //       void EndOfEventProcess() { cout << "myProcess::EndOfEventProcess. Implement this if you need to do something at the end of each event process." << endl; }
 //       void EndProcess() { cout << "myProcess::EndProcess. Implement this if you need to do something after processing all events." << endl; }

        void LoadConfig( std::string cfgFilename, string name = "" );

        void PrintMetadata() 
        { 
            BeginPrintProcess();

            std::cout << "Show every : " << fMyProcessParameter << std::endl;

            EndPrintProcess();
        }

        // Use this only if your process generates metadata. If it is returned the metadata will be stored in TRestRun.
        TRestMetadata *GetProcessMetadata( ) { return NULL; }

        TString GetProcessName() { return (TString) "myProcess"; }

        //Constructor
        myProcess();
        //Destructor
        ~myProcess();

        ClassDef(myProcess, 1);      // Template for a REST "event process" class inherited from TRestEventProcess
};
#endif

