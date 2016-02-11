///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///             
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestTrackReductionProcess.h
//
//              Jan 2016 : Javier Galan
///
///_______________________________________________________________________________


#ifndef RestCore_TRestTrackReductionProcess
#define RestCore_TRestTrackReductionProcess

#include <TRestTrackEvent.h>
#include "TRestEventProcess.h"

class TRestTrackReductionProcess:public TRestEventProcess {

    private:

#ifndef __CINT__
        TRestTrackEvent *fInputTrackEvent;
        TRestTrackEvent *fOutputTrackEvent;
#endif

        void InitFromConfigFile();

        void Initialize();

    protected:

        Double_t fMinimumDistance;
        Double_t fMaximumDistance;
        Double_t fMaxNodes;
	

    public:
        void InitProcess();
        void BeginOfEventProcess(); 
        TRestEvent *ProcessEvent( TRestEvent *eventInput );
        void EndOfEventProcess(); 
        void EndProcess();
        void LoadDefaultConfig( );

        void LoadConfig( std::string cfgFilename );

        void PrintMetadata() { 

            BeginPrintProcess();

            std::cout << " Minimum distance : " << fMinimumDistance << std::endl;
            std::cout << " Maximum distance : " << fMaximumDistance << std::endl;
            std::cout << " Maximum number of nodes : " << fMaxNodes << std::endl;

            EndPrintProcess();

        }

        TString GetProcessName() { return (TString) "trackReduction"; }

        //Constructor
        TRestTrackReductionProcess();
        TRestTrackReductionProcess( char *cfgFileName );
        //Destructor
        ~TRestTrackReductionProcess();

        ClassDef(TRestTrackReductionProcess, 1);      // Template for a REST "event process" class inherited from TRestEventProcess
};
#endif

