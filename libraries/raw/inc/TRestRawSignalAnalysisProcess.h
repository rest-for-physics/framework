///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestRawSignalAnalysisProcess.h
///
///
///_______________________________________________________________________________

#ifndef RestCore_TRestRawSignalAnalysisProcess
#define RestCore_TRestRawSignalAnalysisProcess

#include <TH1D.h>

//#include <TCanvas.h>

#include <TRestGas.h>
#include <TRestHitsEvent.h>
#include <TRestRawSignalEvent.h>
#include <TRestReadout.h>

#include "TRestEventProcess.h"

class TRestRawSignalAnalysisProcess : public TRestEventProcess {
   private:
#ifndef __CINT__
    TRestRawSignalEvent* fSignalEvent;  //!

    TRestReadout* fReadout;  //!

    Double_t fFirstEventTime;             //!
    vector<Double_t> fPreviousEventTime;  //!

    vector<TObject*> fDrawingObjects;  //!
    Double_t fDrawRefresh;             //!

    time_t timeStored;  //!

    std::vector<std::string> fSignalAnalysisObservables;  //!

    // parameters
    TVector2 fBaseLineRange;
    TVector2 fIntegralRange;
    Double_t fPointThreshold;
    Double_t fSignalThreshold;
    Int_t fNPointsOverThreshold;
    // Bool_t fCutsEnabled;//!
    // TVector2 fMeanBaseLineCutRange;//!
    // TVector2 fMeanBaseLineSigmaCutRange;//!
    // TVector2 fMaxNumberOfSignalsCut;//!
    // TVector2 fMaxNumberOfGoodSignalsCut;//!
    // TVector2 fFullIntegralCut;//!
    // TVector2 fThresholdIntegralCut;//!
    // TVector2 fPeakTimeDelayCut;//!

    // analysis result(saved directly in root file)
    TH1D* fChannelsHisto;  //!
#endif

    TPad* DrawSignal(Int_t signal);
    TPad* DrawObservables();

    void InitFromConfigFile();

    void Initialize();

    void LoadDefaultConfig();

   public:
    void InitProcess();
    void BeginOfEventProcess();
    TRestEvent* ProcessEvent(TRestEvent* eventInput);
    void EndOfEventProcess();
    void EndProcess();

    void LoadConfig(std::string cfgFilename, std::string name = "");

    void PrintMetadata() {
        BeginPrintProcess();

        metadata << "Baseline range : ( " << fBaseLineRange.X() << " , " << fBaseLineRange.Y() << " ) "
                 << endl;
        metadata << "Integral range : ( " << fIntegralRange.X() << " , " << fIntegralRange.Y() << " ) "
                 << endl;
        metadata << "Point Threshold : " << fPointThreshold << " sigmas" << endl;
        metadata << "Signal threshold : " << fSignalThreshold << " sigmas" << endl;
        metadata << "Number of points over threshold : " << fNPointsOverThreshold << endl;
        metadata << " " << endl;

        EndPrintProcess();
    }

    TString GetProcessName() { return (TString) "rawSignalAnalysis"; }

    // Constructor
    TRestRawSignalAnalysisProcess();
    TRestRawSignalAnalysisProcess(char* cfgFileName);
    // Destructor
    ~TRestRawSignalAnalysisProcess();

    ClassDef(TRestRawSignalAnalysisProcess,
             1);  // Template for a REST "event process" class inherited from
                  // TRestEventProcess
};
#endif
