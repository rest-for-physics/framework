///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///             
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestSignalAnalysisProcess.h
///
///_______________________________________________________________________________


#ifndef RestCore_TRestSignalAnalysisProcess
#define RestCore_TRestSignalAnalysisProcess

#include <TRestGas.h>
#include <TRestSignalEvent.h>
#include <TRestHitsEvent.h>

#include "TRestEventProcess.h"

class TRestSignalAnalysisProcess:public TRestEventProcess {
    private:
#ifndef __CINT__
        TRestSignalEvent *fSignalEvent;
        // TODO We must get here a pointer to TRestDaqMetadata
        // In order to convert the parameters to time using the sampling time
#endif

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

        void LoadConfig( std::string cfgFilename );

        void PrintMetadata() { 

            BeginPrintProcess();

            EndPrintProcess();

        }

        TString GetProcessName() { return (TString) "signalAnalysis"; }

        //Constructor
        TRestSignalAnalysisProcess();
        TRestSignalAnalysisProcess( char *cfgFileName );
        //Destructor
        ~TRestSignalAnalysisProcess();

        ClassDef(TRestSignalAnalysisProcess, 1);      // Template for a REST "event process" class inherited from TRestEventProcess
};
#endif

