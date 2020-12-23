///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestDetectorHitsToSignalProcess.h
///
///_______________________________________________________________________________

#ifndef RestCore_TRestDetectorHitsToSignalProcess
#define RestCore_TRestDetectorHitsToSignalProcess

#include <TRestDetectorReadout.h>
#include <TRestDetectorGas.h>
#include <TRestDetectorHitsEvent.h>
#include <TRestDetectorSignalEvent.h>

#include "TRestEventProcess.h"

class TRestDetectorHitsToSignalProcess : public TRestEventProcess {
   private:
#ifndef __CINT__
    TRestDetectorHitsEvent* fHitsEvent;      //!
    TRestDetectorSignalEvent* fSignalEvent;  //!

    TRestDetectorReadout* fReadout;  //!
    TRestDetectorGas* fGas;                  //!
#endif

    void InitFromConfigFile();

    void Initialize();

    void LoadDefaultConfig();

    Int_t FindModule(Int_t readoutPlane, Double_t x, Double_t y);
    Int_t FindChannel(Int_t module, Double_t x, Double_t y);

   protected:
    Double_t fSampling;       // us
    Double_t fGasPressure;    // atm
    Double_t fElectricField;  // V/mm
    Double_t fDriftVelocity;  // mm/us

   public:
    any GetInputEvent() { return fHitsEvent; }
    any GetOutputEvent() { return fSignalEvent; }

    void InitProcess();
    TRestEvent* ProcessEvent(TRestEvent* eventInput);
    void EndProcess();

    void LoadConfig(std::string cfgFilename, std::string name = "");

    void PrintMetadata() {
        BeginPrintProcess();

        metadata << "Sampling : " << fSampling << " us" << endl;
        metadata << "Electric field : " << fElectricField * units("V/cm") << " V/cm" << endl;
        metadata << "Gas pressure : " << fGasPressure << " atm" << endl;
        metadata << "Drift velocity : " << fDriftVelocity << " mm/us" << endl;

        EndPrintProcess();
    }

    TRestMetadata* GetProcessMetadata() { return fReadout; }

    TString GetProcessName() { return (TString) "hitsToSignal"; }

    // Constructor
    TRestDetectorHitsToSignalProcess();
    TRestDetectorHitsToSignalProcess(char* cfgFileName);
    // Destructor
    ~TRestDetectorHitsToSignalProcess();

    ClassDef(TRestDetectorHitsToSignalProcess, 1);  // Template for a REST "event process" class inherited from
                                            // TRestEventProcess
};
#endif
