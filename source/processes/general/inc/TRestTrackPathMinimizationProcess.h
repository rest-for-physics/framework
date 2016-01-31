///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///             
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestTrackPathMinimizationProcess.h
//
//              Jan 2016 : Javier Galan
///
///_______________________________________________________________________________


#ifndef RestCore_TRestTrackPathMinimizationProcess
#define RestCore_TRestTrackPathMinimizationProcess

#include <TRestTrackEvent.h>
#include "TRestEventProcess.h"

#ifndef __CINT__
#include "trackMinimization.h"
#endif

class TRestTrackPathMinimizationProcess:public TRestEventProcess {
    private:

        TRestTrackEvent *fInputTrackEvent;
        TRestTrackEvent *fOutputTrackEvent;

        Double_t fEnergyThreshold;
        Int_t fMaxNodes;

        void InitFromConfigFile();

        void Initialize();

    protected:
        //add here the members of your event process


    public:
        void InitProcess();
        void BeginOfEventProcess(); 
        TRestEvent *ProcessEvent( TRestEvent *eventInput );
        void EndOfEventProcess(); 
        void EndProcess();
        void LoadDefaultConfig( );

        void LoadConfig( std::string cfgFilename );

        void PrintMetadata() 
        { 
            BeginPrintProcess();
            std::cout << " Energy threshold : " << fEnergyThreshold << std::endl;
            EndPrintProcess();
        }

        TString GetProcessName() { return (TString) "trackPathMinimization"; }

        //Constructor
        TRestTrackPathMinimizationProcess();
        TRestTrackPathMinimizationProcess( char *cfgFileName );
        //Destructor
        ~TRestTrackPathMinimizationProcess();

        ClassDef( TRestTrackPathMinimizationProcess, 1);
};
#endif

