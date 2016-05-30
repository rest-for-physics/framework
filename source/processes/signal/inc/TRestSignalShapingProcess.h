///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///             
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestSignalShapingProcess.h
///
///_______________________________________________________________________________


#ifndef RestCore_TRestSignalShapingProcess
#define RestCore_TRestSignalShapingProcess

#include <TRestReadout.h>
#include <TRestSignalEvent.h>

#include "TRestEventProcess.h"

class TRestSignalShapingProcess:public TRestEventProcess {

    private:
        TRestSignalEvent *fInputSignalEvent;
        TRestSignalEvent *fOutputSignalEvent;
        TRestSignal *responseSignal;

        void InitFromConfigFile();

        void Initialize();

        void LoadDefaultConfig();

    protected:
        //add here the members of your event process
        TString fResponseFilename;

    public:
        void InitProcess();
        void BeginOfEventProcess(); 
        TRestEvent *ProcessEvent( TRestEvent *eventInput );
        void EndOfEventProcess(); 
        void EndProcess();

        void LoadConfig( std::string cfgFilename, std::string name = "" );

        void PrintMetadata() 
        { 
            BeginPrintProcess();

            EndPrintProcess();
        }

        TRestMetadata *GetProcessMetadata( ) { return NULL; }

        TString GetProcessName() { return (TString) "signalShaping"; }

        //Constructor
        TRestSignalShapingProcess();
        TRestSignalShapingProcess( char *cfgFileName );
        //Destructor
        ~TRestSignalShapingProcess();

        ClassDef(TRestSignalShapingProcess, 1);      // Template for a REST "event process" class inherited from TRestEventProcess
};
#endif

