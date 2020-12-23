///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestDetectorSignalToHitsProcess.h
///
///_______________________________________________________________________________

#ifndef RestCore_TRestDetectorSignalToHitsProcess
#define RestCore_TRestDetectorSignalToHitsProcess

#include <TRestDetectorReadout.h>
#include <TRestDetectorGas.h>

#include <TRestDetectorHitsEvent.h>
#include <TRestDetectorSignalEvent.h>

#include "TRestEventProcess.h"

class TRestDetectorSignalToHitsProcess : public TRestEventProcess {
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

   protected:
    Double_t fElectricField;  // V/cm
    Double_t fGasPressure;    // atm
    Double_t fDriftVelocity;  // mm/us

    TString fSignalToHitMethod;

   public:
    any GetInputEvent() { return fSignalEvent; }
    any GetOutputEvent() { return fHitsEvent; }

    void InitProcess();
    TRestEvent* ProcessEvent(TRestEvent* eventInput);
    void EndProcess();

    void LoadConfig(std::string cfgFilename, std::string name = "");

    void PrintMetadata() {
        BeginPrintProcess();

        metadata << "Electric field : " << fElectricField * units("V/cm") << " V/cm" << endl;
        metadata << "Gas pressure : " << fGasPressure << " atm" << endl;
        metadata << "Drift velocity : " << fDriftVelocity << " mm/us" << endl;

        metadata << "Signal to hits method : " << fSignalToHitMethod << endl;

        EndPrintProcess();
    }

    TString GetProcessName() { return (TString) "signalToHits"; }

    // Constructor
    TRestDetectorSignalToHitsProcess();
    TRestDetectorSignalToHitsProcess(char* cfgFileName);
    // Destructor
    ~TRestDetectorSignalToHitsProcess();

    ClassDef(TRestDetectorSignalToHitsProcess, 2);  // Template for a REST "event process" class inherited from
                                            // TRestEventProcess
};
#endif
