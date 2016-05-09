///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///             
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestTrackAnalysisProcess.h
///
///_______________________________________________________________________________


#ifndef RestCore_TRestTrackAnalysisProcess
#define RestCore_TRestTrackAnalysisProcess

#include <TRestTrackEvent.h>

#include "TRestEventProcess.h"

class TRestTrackAnalysisProcess:public TRestEventProcess {
    private:
#ifndef __CINT__
        TRestTrackEvent *fTrackEvent;

        vector <Double_t> fPreviousEventTime;
#endif

        void InitFromConfigFile();

        void Initialize();

        void LoadDefaultConfig();

    protected:
        
        //add here the members of your event process

        TVector2 fNTracksXCut;
        TVector2 fNTracksYCut;


    public:
        void InitProcess();
        void BeginOfEventProcess(); 
        TRestEvent *ProcessEvent( TRestEvent *eventInput );
        void EndOfEventProcess(); 
        void EndProcess();

        void LoadConfig( std::string cfgFilename, std::string name = "" );

        void PrintMetadata() { 

            BeginPrintProcess();

            cout << "Number of tracks in X cut : ( " << fNTracksXCut.X() << " , " << fNTracksXCut.Y() << " ) " << endl;
            cout << "Number of tracks in Y cut : ( " << fNTracksYCut.X() << " , " << fNTracksYCut.Y() << " ) " << endl;

            EndPrintProcess();
        }

        TString GetProcessName() { return (TString) "trackAnalysis"; }

        //Constructor
        TRestTrackAnalysisProcess();
        TRestTrackAnalysisProcess( char *cfgFileName );
        //Destructor
        ~TRestTrackAnalysisProcess();

        ClassDef(TRestTrackAnalysisProcess, 1);      // Template for a REST "event process" class inherited from TRestEventProcess
};
#endif

