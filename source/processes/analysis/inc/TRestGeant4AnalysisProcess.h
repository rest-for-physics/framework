///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///             
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestGeant4AnalysisProcess.h
///
///_______________________________________________________________________________


#ifndef RestCore_TRestGeant4AnalysisProcess
#define RestCore_TRestGeant4AnalysisProcess

#include <TRestGas.h>
#include <TRestG4Event.h>
#include <TRestG4Metadata.h>
#include <TRestHitsEvent.h>

#include "TRestEventProcess.h"

class TRestGeant4AnalysisProcess:public TRestEventProcess {
    private:
#ifndef __CINT__
        TRestG4Event *fG4Event;
        TRestG4Metadata *fG4Metadata;
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

        TString GetProcessName() { return (TString) "geant4Analysis"; }

        //Constructor
        TRestGeant4AnalysisProcess();
        TRestGeant4AnalysisProcess( char *cfgFileName );
        //Destructor
        ~TRestGeant4AnalysisProcess();

        ClassDef(TRestGeant4AnalysisProcess, 1);      // Template for a REST "event process" class inherited from TRestEventProcess
};
#endif

