///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///             
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestHitsToTrackProcess.h
///
///_______________________________________________________________________________


#ifndef RestCore_TRestHitsToTrackProcess
#define RestCore_TRestHitsToTrackProcess

#include <TRestHitsEvent.h>
#include <TRestTrackEvent.h>
#include "TRestEventProcess.h"

class TRestHitsToTrackProcess:public TRestEventProcess {
    private:

        TRestHitsEvent *fHitsEvent;
        TRestTrackEvent *fTrackEvent;

        Double_t fClusterDistance;

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

        void PrintMetadata() { 

            BeginPrintProcess();

            cout << " cluster-distance : " << fClusterDistance << endl;

            EndPrintProcess();

        }

        TString GetProcessName() { return (TString) "HitsToTrackProcess"; }

        //Constructor
        TRestHitsToTrackProcess();
        TRestHitsToTrackProcess( char *cfgFileName );
        //Destructor
        ~TRestHitsToTrackProcess();

        ClassDef(TRestHitsToTrackProcess, 1);      // Template for a REST "event process" class inherited from TRestEventProcess
};
#endif

