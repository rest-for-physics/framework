///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///             
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestRawSignalToSignalProcess.h
///
///_______________________________________________________________________________


#ifndef RestCore_TRestRawSignalToSignalProcess
#define RestCore_TRestRawSignalToSignalProcess

#include <TRestRawSignalEvent.h>
#include <TRestSignalEvent.h>

#include "TRestEventProcess.h"

class TRestRawSignalToSignalProcess:public TRestEventProcess {

    private:
        TRestRawSignalEvent *fInputSignalEvent;
        TRestSignalEvent *fOutputSignalEvent;

        void InitFromConfigFile();

        void Initialize();

        void LoadDefaultConfig();

    protected:
        //add here the members of your event process

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

        TString GetProcessName() { return (TString) "rawSignalToSignal"; }

        //Constructor
        TRestRawSignalToSignalProcess();
        TRestRawSignalToSignalProcess( char *cfgFileName );
        //Destructor
        ~TRestRawSignalToSignalProcess();

        ClassDef(TRestRawSignalToSignalProcess, 1);      // Template for a REST "event process" class inherited from TRestEventProcess
};
#endif

