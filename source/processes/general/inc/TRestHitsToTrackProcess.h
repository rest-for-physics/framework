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
#include "TMatrixD.h"

class TRestHitsToTrackProcess:public TRestEventProcess {
    private:

#ifndef __CINT__
        TRestHitsEvent *fHitsEvent;//!
        TRestTrackEvent *fTrackEvent;//!
#endif

        void InitFromConfigFile();

        void Initialize();
        Int_t FindTracks( TRestHits *hits );

    protected:
        //add here the members of your event process
        Double_t fClusterDistance;
	

    public:
        void InitProcess();
        void BeginOfEventProcess(); 
        TRestEvent *ProcessEvent( TRestEvent *eventInput );
        void EndOfEventProcess(); 
        void EndProcess();
        void LoadDefaultConfig( );

        void LoadConfig( std::string cfgFilename, std::string name = "" );

        void PrintMetadata() { 

            BeginPrintProcess();

            std::cout << " cluster-distance : " << fClusterDistance << " mm " << std::endl;

            EndPrintProcess();

        }

        TString GetProcessName() { return (TString) "hitsToTrack"; }

        //Constructor
        TRestHitsToTrackProcess();
        TRestHitsToTrackProcess( char *cfgFileName );
        //Destructor
        ~TRestHitsToTrackProcess();

        ClassDef(TRestHitsToTrackProcess, 1);      // Template for a REST "event process" class inherited from TRestEventProcess
};
#endif

