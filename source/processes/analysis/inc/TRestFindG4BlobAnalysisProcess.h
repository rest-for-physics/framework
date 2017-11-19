///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///             
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestFindG4BlobAnalysisProcess.h
///
///_______________________________________________________________________________


#ifndef RestCore_TRestFindG4BlobAnalysisProcess
#define RestCore_TRestFindG4BlobAnalysisProcess

#include <TRestG4Event.h>
#include <TRestG4Metadata.h>

#include "TRestEventProcess.h"

class TRestFindG4BlobAnalysisProcess:public TRestEventProcess {
    private:
#ifndef __CINT__
        TRestG4Event *fG4Event;//!
        TRestG4Metadata *fG4Metadata;//!

        std::vector <std::string> fQ1_Observables;//!
        std::vector <double> fQ1_Radius;//!

        std::vector <std::string> fQ2_Observables;//!
        std::vector <double> fQ2_Radius;//!
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

        TString GetProcessName() { return (TString) "findG4BlobAnalysis"; }

        //Constructor
        TRestFindG4BlobAnalysisProcess();
        TRestFindG4BlobAnalysisProcess( char *cfgFileName );
        //Destructor
        ~TRestFindG4BlobAnalysisProcess();

        ClassDef(TRestFindG4BlobAnalysisProcess, 1);      // Template for a REST "event process" class inherited from TRestEventProcess
};
#endif

