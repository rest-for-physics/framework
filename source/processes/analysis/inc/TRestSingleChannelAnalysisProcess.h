///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestSingleChannelAnalysisProcess.h
///
///_______________________________________________________________________________

#ifndef RestCore_TRestSingleChannelAnalysisProcess
#define RestCore_TRestSingleChannelAnalysisProcess

#include <TH1D.h>

//#include <TCanvas.h>

#include <TRestGas.h>
#include <TRestHitsEvent.h>
#include <TRestReadout.h>
#include <TRestSignalEvent.h>

#include "TRestGainMap.h"
#include "TRestEventProcess.h"

class TRestSingleChannelAnalysisProcess : public TRestEventProcess {
   private:
#ifndef __CINT__
    TRestSignalEvent* fSignalEvent;  //!

    TRestReadout* fReadout;  //!

    TRestGainMap* fCalib;  //!
#endif

    void InitFromConfigFile();

    void Initialize();
    // parameters
    bool fApplyGainCorrection;
    bool fCreateGainMap;
    TVector2 fThrIntegralCutRange;
    TVector2 fNGoodSignalsCutRange;
    string fOutputCalibrationFileName;
    string fMethod;

    // temp data
    map<int, double> fChannelThrIntegralSum;  //! [channel id, sum]
    map<int, int> fChannelCounts;                         //! [channel id, trigger counts]          
    TH2D* fAreaThrIntegralSum;               //!
    TH2D* fAreaCounts;               //!

    // analysis result(saved directly in root file)
    map<int, double> fChannelGain;  //! [MM id, channel gain]
    TH2D* fAreaGainMap;             //!

   public:
    void InitProcess();
    TRestEvent* ProcessEvent(TRestEvent* eventInput);
    void EndProcess();

    void PrintMetadata() {
        BeginPrintProcess();
        
        metadata << "the mode is:" << endl;
        metadata << (fApplyGainCorrection ? ">   " : "    ") << "Apply channel correction map for spectrum " << endl;
        metadata << (fCreateGainMap       ? ">   " : "    ") << "Create new correction map for each channel" << endl;
        metadata << "method: " << fMethod << endl;
        metadata << "output mapping file: " << fOutputCalibrationFileName << endl;
        metadata << "Energy cut for Threshold integral: " << any(fThrIntegralCutRange).ToString() << endl;
        metadata << "Energy cut for NGoodSignals: " << any(fNGoodSignalsCutRange).ToString() << endl;

        EndPrintProcess();
    }

    TString GetProcessName() { return (TString) "readoutAnalysis"; }

    // Constructor
    TRestSingleChannelAnalysisProcess();
    TRestSingleChannelAnalysisProcess(char* cfgFileName);
    // Destructor
    ~TRestSingleChannelAnalysisProcess();

    ClassDef(TRestSingleChannelAnalysisProcess,
             1);  // Template for a REST "event process" class inherited from
                  // TRestEventProcess
};
#endif
