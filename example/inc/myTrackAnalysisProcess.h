///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///             
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             myTrackAnalysisProcess.h
///
///_______________________________________________________________________________


#ifndef RestCore_myTrackAnalysisProcess
#define RestCore_myTrackAnalysisProcess

#include <TRestTrackEvent.h>

#include "TRestEventProcess.h"

class myTrackAnalysisProcess:public TRestEventProcess {
    private:
#ifndef __CINT__
        TRestTrackEvent *fTrackEvent;
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

        void LoadConfig( std::string cfgFilename, std::string name = "" );

        void PrintMetadata() { 

            BeginPrintProcess();

            EndPrintProcess();
        }

        TString GetProcessName() { return (TString) "myTrackAnalysis"; }

        //Constructor
        myTrackAnalysisProcess();
        myTrackAnalysisProcess( char *cfgFileName );
        //Destructor
        ~myTrackAnalysisProcess();

        ClassDef(myTrackAnalysisProcess, 1);      // Template for a REST "event process" class inherited from TRestEventProcess
};
#endif

