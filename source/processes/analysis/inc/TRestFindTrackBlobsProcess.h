///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///             
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestFindTrackBlobsProcess.h
///
///_______________________________________________________________________________


#ifndef RestCore_TRestFindTrackBlobsProcess
#define RestCore_TRestFindTrackBlobsProcess

#include <TRestTrackEvent.h>

#include "TRestEventProcess.h"

class TRestFindTrackBlobsProcess:public TRestEventProcess {
    private:
#ifndef __CINT__
        TRestTrackEvent *fTrackEvent;

        std::vector <std::string> fQ1_Observables;
        std::vector <double> fQ1_Radius;

        std::vector <std::string> fQ2_Observables;
        std::vector <double> fQ2_Radius;
#endif

        Double_t fHitsToCheckFraction;

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

            std::cout << " Hits to check factor : " << fHitsToCheckFraction << std::endl;

            EndPrintProcess();
        }

        TString GetProcessName() { return (TString) "findTrackBlobs"; }

        //Constructor
        TRestFindTrackBlobsProcess();
        TRestFindTrackBlobsProcess( char *cfgFileName );
        //Destructor
        ~TRestFindTrackBlobsProcess();

        ClassDef(TRestFindTrackBlobsProcess, 1);      // Template for a REST "event process" class inherited from TRestEventProcess
};
#endif

