///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///             
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestSignalToRawSignalProcess.h
///
///_______________________________________________________________________________


#ifndef RestCore_TRestSignalToRawSignalProcess
#define RestCore_TRestSignalToRawSignalProcess

#include <TRestRawSignalEvent.h>
#include <TRestSignalEvent.h>

#include "TRestEventProcess.h"

class TRestSignalToRawSignalProcess:public TRestEventProcess {

    private:
        TRestSignalEvent *fInputSignalEvent;
        TRestRawSignalEvent *fOutputSignalEvent;

        void InitFromConfigFile();

        void Initialize();

        void LoadDefaultConfig();

    protected:
        //add here the members of your event process

	// TODO Add trigger time
    public:
        void InitProcess();
        void BeginOfEventProcess(); 
        TRestEvent *ProcessEvent( TRestEvent *eventInput );
        void EndOfEventProcess(); 
        void EndProcess();

        void LoadConfig( std::string cfgFilename, string name = "" );

        void PrintMetadata() 
        { 
            BeginPrintProcess();

            EndPrintProcess();
        }

        TRestMetadata *GetProcessMetadata( ) { return NULL; }

        TString GetProcessName() { return (TString) "signalToRawSignal"; }

        //Constructor
        TRestSignalToRawSignalProcess();
        TRestSignalToRawSignalProcess( char *cfgFileName );
        //Destructor
        ~TRestSignalToRawSignalProcess();

        ClassDef(TRestSignalToRawSignalProcess, 1);      // Template for a REST "event process" class inherited from TRestEventProcess
};
#endif

