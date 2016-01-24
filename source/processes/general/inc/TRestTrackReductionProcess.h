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

        TRestTrackEvent *fInputTrackEvent;
        TRestTrackEvent *fOutputTrackEvent;

        Double_t fMinimumDistance;
        Double_t fMaximumDistance;
        Double_t fMaxNodes;

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

        void LoadConfig( string cfgFilename );

        void PrintMetadata() { 

            BeginPrintProcess();

            cout << " Minimum distance : " << fMinimumDistance << endl;
            cout << " Maximum distance : " << fMaximumDistance << endl;
            cout << " Maximum number of nodes : " << fMaxNodes << endl;

            EndPrintProcess();

        }

        TString GetProcessName() { return (TString) "TrackReductionProcess"; }

        //Constructor
        TRestTrackReductionProcess();
        TRestTrackReductionProcess( char *cfgFileName );
        //Destructor
        ~TRestTrackReductionProcess();

        ClassDef(TRestTrackReductionProcess, 1);      // Template for a REST "event process" class inherited from TRestEventProcess
};
#endif

