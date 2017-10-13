///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///             
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestHitsAnalysisProcess.h
///
///_______________________________________________________________________________


#ifndef RestCore_TRestHitsAnalysisProcess
#define RestCore_TRestHitsAnalysisProcess

#include <TH1D.h>

#include <TCanvas.h>

#include <TRestGas.h>
#include <TRestReadout.h>
#include <TRestSignalEvent.h>
#include <TRestHitsEvent.h>

#include "TRestEventProcess.h"

class TRestHitsAnalysisProcess:public TRestEventProcess {
    private:
#ifndef __CINT__
        TRestHitsEvent *fInputHitsEvent;
        TRestHitsEvent *fOutputHitsEvent;
#endif

        void InitFromConfigFile();

        void Initialize();

        void LoadDefaultConfig();

    protected:

        //add here the members of your event process
        //
        TVector3 fCyl_x0;
        TVector3 fCyl_x1;
        Double_t fCyl_R;

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

        TString GetProcessName() { return (TString) "hitsAnalysis"; }

        //Constructor
        TRestHitsAnalysisProcess();
        TRestHitsAnalysisProcess( char *cfgFileName );
        //Destructor
        ~TRestHitsAnalysisProcess();

        ClassDef(TRestHitsAnalysisProcess, 1);      // Template for a REST "event process" class inherited from TRestEventProcess
};
#endif

