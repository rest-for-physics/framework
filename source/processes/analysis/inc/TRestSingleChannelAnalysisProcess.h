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
    TVector2 fSpecFitRange;
    string fCalibSave;

    // analysis result
    map<int, TH1D*> fChannelThrIntegral;  //-> [channel id, sum]
    map<int, double> fChannelGain;        // [MM id, channel gain]
    map<int, double> fChannelGainError;   // [MM id, channel gain]

   public:
    void FitChannelGain();
    void SaveGainMetadata(string filename);
    void InitProcess();
    TRestEvent* ProcessEvent(TRestEvent* eventInput);
    void EndProcess();
    TH1D* GetChannelSpectrum(int id);
    void PrintChannelSpectrums(string filename);

    void PrintMetadata() {
        BeginPrintProcess();
        
        metadata << "the mode is:" << endl;
        metadata << (fApplyGainCorrection ? ">   " : "    ") << "Apply channel correction map for spectrum " << endl;
        metadata << (fCreateGainMap       ? ">   " : "    ") << "Create new correction map for each channel" << endl;
        metadata << "output mapping file: " << fCalibSave << endl;
        metadata << "Energy cut for Threshold integral: " << any(fThrIntegralCutRange).ToString() << endl;
        metadata << "Energy cut for NGoodSignals: " << any(fNGoodSignalsCutRange).ToString() << endl;
        metadata << "Fit range for the spectrums: " << any(fSpecFitRange).ToString() << endl;

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
