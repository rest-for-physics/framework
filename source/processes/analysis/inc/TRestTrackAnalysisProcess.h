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
        TRestTrackEvent *fTrackEvent;//!

        vector <Double_t> fPreviousEventTime;//!

        Bool_t fCutsEnabled;//!

        std::vector <std::string> fTrack_LE_EnergyObservables;//!
        std::vector <Double_t> fTrack_LE_Threshold;//!
        std::vector <Int_t> nTracks_LE;//!

        std::vector <std::string> fTrack_HE_EnergyObservables;//!
        std::vector <Double_t> fTrack_HE_Threshold;//!
        std::vector <Int_t> nTracks_HE;//!
        
        std::vector <std::string> fTrack_LE_Y_EnergyObservables;//!
        std::vector <Double_t> fTrack_LE_Y_Threshold;//!
        std::vector <Int_t> nTracks_LE_Y;//!

        std::vector <std::string> fTrack_HE_Y_EnergyObservables;//!
        std::vector <Double_t> fTrack_HE_Y_Threshold;//!
        std::vector <Int_t> nTracks_HE_Y;//!

		std::vector <std::string> fTrack_LE_X_EnergyObservables;//!
		std::vector <Double_t> fTrack_LE_X_Threshold;//!
		std::vector <Int_t> nTracks_LE_X;//!

		std::vector <std::string> fTrack_HE_X_EnergyObservables;//!
		std::vector <Double_t> fTrack_HE_X_Threshold;//!
		std::vector <Int_t> nTracks_HE_X;//!
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

		if( fCutsEnabled )
		{
			cout << "Number of tracks in X cut : ( " << fNTracksXCut.X() << " , " << fNTracksXCut.Y() << " ) " << endl;
			cout << "Number of tracks in Y cut : ( " << fNTracksYCut.X() << " , " << fNTracksYCut.Y() << " ) " << endl;
		}
		else
		{
			cout << endl;
			cout << "No cuts have been enabled" << endl;

		}

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

