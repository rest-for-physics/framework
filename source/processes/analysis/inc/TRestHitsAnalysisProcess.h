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
        TRestHitsEvent *fInputHitsEvent;//!
        TRestHitsEvent *fOutputHitsEvent;//!

        Bool_t fCylinderFiducial;//!
        Bool_t fPrismFiducial;//!
#endif

        void InitFromConfigFile();

        void Initialize();

        void LoadDefaultConfig();

    protected:

        //add here the members of your event process
        //
        TVector3 fFid_x0;
        TVector3 fFid_x1;
        Double_t fFid_R;
        Double_t fFid_sX;
        Double_t fFid_sY;


    public:
        void InitProcess();
        void BeginOfEventProcess(); 
        TRestEvent *ProcessEvent( TRestEvent *eventInput );
        void EndOfEventProcess(); 
        void EndProcess();

        void LoadConfig( std::string cfgFilename, std::string name = "" );

        void PrintMetadata() { 

            BeginPrintProcess();

            essential << " Fiducial parameters" << endl;
            essential << " ------------------- " << endl;
            essential << " x0 : (" << fFid_x0.X() << " , " << fFid_x0.Y() << " , " << fFid_x0.Z() << ")" << endl;
            essential << " x1 : (" << fFid_x1.X() << " , " << fFid_x1.Y() << " , " << fFid_x1.Z() << ")" << endl;
            essential << " R : " << fFid_R << endl;
            essential << " sX : " << fFid_sX << endl;
            essential << " sY : " << fFid_sY << endl;
            essential << " ------------------- " << endl;

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

