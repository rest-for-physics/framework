///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///             
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestSignalAnalysisProcess.h
///
///_______________________________________________________________________________


#ifndef RestCore_TRestSignalAnalysisProcess
#define RestCore_TRestSignalAnalysisProcess

#include <TH1D.h>

#include <TRestGas.h>
#include <TRestReadout.h>
#include <TRestSignalEvent.h>
#include <TRestHitsEvent.h>

#include "TRestEventProcess.h"

class TRestSignalAnalysisProcess:public TRestEventProcess {
    private:
#ifndef __CINT__
        TRestSignalEvent *fSignalEvent;
        // TODO We must get here a pointer to TRestDaqMetadata
        // In order to convert the parameters to time using the sampling time
        TRestReadout *fReadout;

        Double_t fFirstEventTime;
        vector <Double_t> fPreviousEventTime;
#endif

        void InitFromConfigFile();

        void Initialize();

        void LoadDefaultConfig();

    protected:
        
        //add here the members of your event process
        //
        TVector2 fBaseLineRange;
        TVector2 fIntegralRange;
        Double_t fThreshold;
        Double_t fMinPeakAmplitude;
        Int_t fNPointsOverThreshold;

        Bool_t fCutsEnabled;
        // cut parameters
        TVector2 fMeanBaseLineCutRange;
        TVector2 fMeanBaseLineSigmaCutRange;
        TVector2 fMaxNumberOfSignalsCut;
        TVector2 fMaxNumberOfGoodSignalsCut;
        TVector2 fFullIntegralCut;
        TVector2 fThresholdIntegralCut;
        TVector2 fPeakTimeDelayCut;

        TH1D *fChannelsHisto;


    public:
        void InitProcess();
        void BeginOfEventProcess(); 
        TRestEvent *ProcessEvent( TRestEvent *eventInput );
        void EndOfEventProcess(); 
        void EndProcess();

        void LoadConfig( std::string cfgFilename, std::string name = "" );

        void PrintMetadata() { 

            BeginPrintProcess();

            cout << "Baseline range : ( " << fBaseLineRange.X() << " , " << fBaseLineRange.Y() << " ) " << endl;
            cout << "Integral range : ( " << fIntegralRange.X() << " , " << fIntegralRange.Y() << " ) " << endl;
            cout << "Threshold : " << fThreshold << " sigmas" << endl;
            cout << "Number of points over threshold : " << fNPointsOverThreshold << endl;
            cout << "Mean base line cut range : ( " << fMeanBaseLineCutRange.X() << " , " << fMeanBaseLineCutRange.Y() << " ) " << endl;
            cout << "Mean base line sigma cut range : ( " << fMeanBaseLineSigmaCutRange.X() << " , " << fMeanBaseLineSigmaCutRange.Y() << " ) " << endl;
            cout << "Max number of signals cut range : ( " << fMaxNumberOfSignalsCut.X() << " , " << fMaxNumberOfSignalsCut.Y() << " ) " << endl;
            cout << "Max number of good signals cut range : ( " << fMaxNumberOfGoodSignalsCut.X() << " , " << fMaxNumberOfGoodSignalsCut.Y() << " ) " << endl;
            cout << "Full integral cut range : ( " << fFullIntegralCut.X() << " , " << fFullIntegralCut.Y() << " ) " << endl;
            cout << "Threshold integral cut range : ( " << fThresholdIntegralCut.X() << " , " << fThresholdIntegralCut.Y() << " ) " << endl;
            cout << "Peak time delay cut range : ( " << fPeakTimeDelayCut.X() << " , " << fPeakTimeDelayCut.Y() << " )" << endl;

            EndPrintProcess();
        }

        TString GetProcessName() { return (TString) "signalAnalysis"; }

        //Constructor
        TRestSignalAnalysisProcess();
        TRestSignalAnalysisProcess( char *cfgFileName );
        //Destructor
        ~TRestSignalAnalysisProcess();

        ClassDef(TRestSignalAnalysisProcess, 1);      // Template for a REST "event process" class inherited from TRestEventProcess
};
#endif

