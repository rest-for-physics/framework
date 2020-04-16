///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestHitsToSignalProcess.h
///
///_______________________________________________________________________________

#ifndef RestCore_TRestHitsToSignalProcess
#define RestCore_TRestHitsToSignalProcess

#include <TRestGas.h>
#include <TRestHitsEvent.h>
#include <TRestReadout.h>
#include <TRestSignalEvent.h>

#include "TRestEventProcess.h"

class TRestHitsToSignalProcess : public TRestEventProcess {
   private:
#ifndef __CINT__
    TRestHitsEvent* fHitsEvent;      //!
    TRestSignalEvent* fSignalEvent;  //!

    TRestReadout* fReadout;  //!
    TRestGas* fGas;          //!
#endif

    void InitFromConfigFile();

    void Initialize();

    void LoadDefaultConfig();

    Int_t FindModule(Int_t readoutPlane, Double_t x, Double_t y);
    Int_t FindChannel(Int_t module, Double_t x, Double_t y);

   protected:
    Double_t fSampling;       // us
    Double_t fGasPressure;    // atm
    Double_t fElectricField;  // V/cm
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
    TRestHitsToSignalProcess();
    TRestHitsToSignalProcess(char* cfgFileName);
    // Destructor
    ~TRestHitsToSignalProcess();

    ClassDef(TRestHitsToSignalProcess, 1);  // Template for a REST "event process" class inherited from
                                            // TRestEventProcess
};
#endif
