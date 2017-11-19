///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///             
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestFindResponseSignalProcess.h
///
///_______________________________________________________________________________


#ifndef RestCore_TRestFindResponseSignalProcess
#define RestCore_TRestFindResponseSignalProcess

#include <TRestRawSignalEvent.h>

#include "TRestEventProcess.h"

class TRestFindResponseSignalProcess:public TRestEventProcess {

    private:
        TRestRawSignalEvent *fInputSignalEvent;//!
        TRestRawSignalEvent *fOutputSignalEvent;//!

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

        TString GetProcessName() { return (TString) "findResponseSignal"; }

        //Constructor
        TRestFindResponseSignalProcess();
        TRestFindResponseSignalProcess( char *cfgFileName );
        //Destructor
        ~TRestFindResponseSignalProcess();

        ClassDef(TRestFindResponseSignalProcess, 1);      // Template for a REST "event process" class inherited from TRestEventProcess
};
#endif

