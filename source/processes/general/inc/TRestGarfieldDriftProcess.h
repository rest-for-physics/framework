///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             Process which create ionization electrons and drift them toward
///             the readout plane (stop 1mm above)
///
///             TRestGarfieldDriftProcess.h
///
///_______________________________________________________________________________

#ifndef RestCore_TRestGarfieldDriftProcess
#define RestCore_TRestGarfieldDriftProcess

#include <TRestGas.h>

#if defined USE_Garfield
#include "AvalancheMC.hh"
#include "ComponentBase.hh"
#include "Sensor.hh"
// TODO Could we have this as an option given through metadata?
#define DRIFT_METHOD AvalancheMC
#endif

#include <TRandom3.h>

#include <TRestGeometry.h>
#include <TRestHitsEvent.h>
#include <TRestReadout.h>
#include <TRestSignalEvent.h>

#include "TRestEventProcess.h"

class TRestGarfieldDriftProcess : public TRestEventProcess {
   private:
    void Initialize();

    TRandom3* fRandom;

    TRestHitsEvent* fInputHitsEvent;   //!
    TRestHitsEvent* fOutputHitsEvent;  //!

#if defined USE_Garfield
    TRestReadout* fReadout;    //!
    TRestGas* fGas;            //!
    TRestGeometry* fGeometry;  //!

    Garfield::Sensor* fGfSensor;             //!
    Garfield::DRIFT_METHOD* fGfDriftMethod;  //!

    void InitFromConfigFile();

    void LoadDefaultConfig();

    Int_t FindModule(Int_t readoutPlane, Double_t x, Double_t y);
    Int_t FindChannel(Int_t module, Double_t x, Double_t y);
#endif
   protected:
#if defined USE_Garfield
    Double_t fGasPressure;     // atm
                               //         Double_t fElectricField; // V/cm
    Double_t fDriftPotential;  // V
    Double_t fPEReduction;     // reduction factor of primary electrons to track
    double fStopDistance;      ///< Distance from readout to stop electron drift, in mm

    TString fGDML_Filename;

#endif
   public:
    TRestEvent* ProcessEvent(TRestEvent* eventInput);

#if defined USE_Garfield
    void InitProcess();
    void BeginOfEventProcess();

    void EndOfEventProcess();
    void EndProcess();

    void LoadConfig(std::string cfgFilename, std::string name = "");

    void PrintMetadata() {
        BeginPrintProcess();

        //             std::cout << "Electric field : " << fElectricField << " V/cm"
        //             << std::endl;
        std::cout << "Drift electrode potential : " << fDriftPotential << " V" << std::endl;
        std::cout << "Gas pressure : " << fGasPressure << " atm" << std::endl;
        std::cout << "Electron reduction factor : " << fPEReduction << std::endl;
        std::cout << "Drift stop distance : " << fStopDistance << " mm" << std::endl;

        EndPrintProcess();
    }

    TRestMetadata* GetProcessMetadata() { return fReadout; }

    TString GetProcessName() { return (TString) "garfieldDrift"; }

    Garfield::Sensor* GetGfSensor() { return fGfSensor; }

    // Constructor
    TRestGarfieldDriftProcess();
    TRestGarfieldDriftProcess(char* cfgFileName);
    // Destructor
    ~TRestGarfieldDriftProcess();
#endif
    ClassDef(TRestGarfieldDriftProcess, 1);  // Template for a REST "event process" class inherited from
                                             // TRestEventProcess
};
#endif
