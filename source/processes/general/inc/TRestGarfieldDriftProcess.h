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

#ifndef __CINT__
#include "Sensor.hh"
#include "ComponentBase.hh"
#include "AvalancheMC.hh"
// TODO Could we have this as an option given through metadata?
#define DRIFT_METHOD AvalancheMC
#endif

#include <TRandom3.h>

#include <TRestGas.h>
#include <TRestReadout.h>
#include <TRestGeometry.h>
#include <TRestSignalEvent.h>
#include <TRestHitsEvent.h>

#include "TRestEventProcess.h"

class TRestGarfieldDriftProcess : public TRestEventProcess {
    private:

        TRandom3 *fRandom;

#ifndef __CINT__
        TRestHitsEvent *fInputHitsEvent;
        TRestHitsEvent *fOutputHitsEvent;

        TRestReadout *fReadout;
        TRestGas *fGas;
        TRestGeometry *fGeometry;

        Garfield::Sensor *fGfSensor;
        Garfield::DRIFT_METHOD *fGfDriftMethod;
#endif

        void InitFromConfigFile();

        void Initialize();

        void LoadDefaultConfig();

        Int_t FindModule( Int_t readoutPlane, Double_t x, Double_t y );
        Int_t FindChannel( Int_t module, Double_t x, Double_t y );

    protected:

        Double_t fGasPressure; // atm
//         Double_t fElectricField; // V/cm
        Double_t fDriftPotential; // V
        Double_t fPEReduction; // reduction factor of primary electrons to track
        double   fStopDistance; ///< Distance from readout to stop electron drift, in mm

        TString fGDML_Filename;


    public:
        void InitProcess();
        void BeginOfEventProcess(); 
        TRestEvent *ProcessEvent( TRestEvent *eventInput );
        void EndOfEventProcess(); 
        void EndProcess();

        void LoadConfig( std::string cfgFilename, std::string name = "" );

        void PrintMetadata() {
            BeginPrintProcess();

//             std::cout << "Electric field : " << fElectricField << " V/cm" << std::endl;
            std::cout << "Drift electrode potential : " << fDriftPotential << " V" << std::endl;
            std::cout << "Gas pressure : " << fGasPressure << " atm" << std::endl;
            std::cout << "Electron reduction factor : " << fPEReduction << std::endl;
            std::cout << "Drift stop distance : " << fStopDistance << " mm" << std::endl;

            EndPrintProcess();
        }

        TRestMetadata *GetProcessMetadata( ) { return fReadout; }

        TString GetProcessName() { return (TString) "garfieldDrift"; }

#ifndef __CINT__
        Garfield::Sensor* GetGfSensor() { return fGfSensor; }
#endif

        //Constructor
        TRestGarfieldDriftProcess();
        TRestGarfieldDriftProcess( char *cfgFileName );
        //Destructor
        ~TRestGarfieldDriftProcess();

        ClassDef(TRestGarfieldDriftProcess, 1);      // Template for a REST "event process" class inherited from TRestEventProcess
};
#endif

