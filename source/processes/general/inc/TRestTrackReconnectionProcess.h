//////////////////////////////////////////////////////////////////////////
///             
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestTrackReconnectionProcess.h
///
///              Nov 2016 : Javier Galan
///
//////////////////////////////////////////////////////////////////////////


#ifndef RestCore_TRestTrackReconnectionProcess
#define RestCore_TRestTrackReconnectionProcess

#include <TRestTrackEvent.h>
#include "TRestEventProcess.h"

class TRestTrackReconnectionProcess:public TRestEventProcess {

    private:

#ifndef __CINT__
        TRestTrackEvent *fInputTrackEvent;//!
        TRestTrackEvent *fOutputTrackEvent;//!

        Double_t meanDistance;//!
        Double_t sigma;//!
#endif

        void InitFromConfigFile();

        void Initialize();


    protected:

    public:

        void InitProcess();
        void BeginOfEventProcess(); 
        TRestEvent *ProcessEvent( TRestEvent *eventInput );
        void EndOfEventProcess(); 
        void EndProcess();
        void LoadDefaultConfig( );

        void LoadConfig( std::string cfgFilename, std::string name = "" );

        void PrintMetadata() 
        { 
            BeginPrintProcess();
            EndPrintProcess();
        }

        TString GetProcessName() { return (TString) "trackReconnection"; }

        void BreakTracks( TRestVolumeHits *hits, vector <TRestVolumeHits>& hitSets );
        void ReconnectTracks( vector <TRestVolumeHits>& hitSets );
        Int_t GetTrackBranches( TRestHits &h, Double_t mean, Double_t sigma );

        //Constructor
        TRestTrackReconnectionProcess();
        TRestTrackReconnectionProcess( char *cfgFileName );
        //Destructor
        ~TRestTrackReconnectionProcess();

        ClassDef( TRestTrackReconnectionProcess, 1);
};
#endif

