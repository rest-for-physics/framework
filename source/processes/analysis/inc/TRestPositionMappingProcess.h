///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestPositionMappingProcess.h
///
///_______________________________________________________________________________

#ifndef RestCore_TRestPositionMappingProcess
#define RestCore_TRestPositionMappingProcess

#include <TH1D.h>

//#include <TCanvas.h>

#include <TRestGas.h>
#include <TRestHitsEvent.h>
#include <TRestReadout.h>
#include <TRestSignalEvent.h>

#include "TRestEventProcess.h"
#include "TRestGainMap.h"

class TRestPositionMappingProcess : public TRestEventProcess {
   private:
#ifndef __CINT__
    TRestHitsEvent* fHitsEvent;  //!
    TRestReadout* fReadout;      //!
    TRestGainMap* fCalib;        //!
    TRestGas* fGas;              //!
#endif

    void InitFromConfigFile();

    void Initialize();
    // parameters
    bool fApplyGainCorrection;
    bool fCreateGainMap;
    TVector2 fEnergyCutRange;
    TVector2 fNHitsCutRange;
    string fMappingSave;

    double fNBinsX;
    double fNBinsY;
    double fNBinsZ;

    // temp data
    TH2D* fAreaThrIntegralSum;  //!
    TH2D* fAreaCounts;          //!

    // analysis result
    TH2F* fAreaGainMap;  //!

   public:
    any GetInputEvent() { return fHitsEvent; }
    any GetOutputEvent() { return fHitsEvent; }

    void InitProcess();
    TRestEvent* ProcessEvent(TRestEvent* eventInput);
    void EndProcess();

    double GetCorrection2(double x, double y);
    double GetCorrection3(double x, double y, double z);

    void PrintMetadata() {
        BeginPrintProcess();

        metadata << "the mode is:" << endl;
        metadata << (fApplyGainCorrection ? ">   " : "    ") << "Apply position correction map for spectrum "
                 << endl;
        metadata << (fCreateGainMap ? ">   " : "    ") << "Create new correction map for each position"
                 << endl;
        metadata << "output mapping file: " << fMappingSave << endl;
        metadata << "Energy cut for Threshold integral: " << any(fEnergyCutRange).ToString() << endl;
        metadata << "Energy cut for NGoodSignals: " << any(fNHitsCutRange).ToString() << endl;
        metadata << "Binning: " << fNBinsX << ", " << fNBinsY << ", " << fNBinsZ << endl;

        EndPrintProcess();
    }

    TString GetProcessName() { return (TString) "readoutAnalysis"; }

    // Constructor
    TRestPositionMappingProcess();
    TRestPositionMappingProcess(char* cfgFileName);
    // Destructor
    ~TRestPositionMappingProcess();

    ClassDef(TRestPositionMappingProcess, 1);  // Template for a REST "event process" class inherited from
                                               // TRestEventProcess
};
#endif
