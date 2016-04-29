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
#include <TRestReadout.h>
#include <TRestSignalEvent.h>
#include <TRestHitsEvent.h>

#include "TRestEventProcess.h"

class TRestHitsToSignalProcess:public TRestEventProcess {
    private:

#ifndef __CINT__
        TRestHitsEvent *fHitsEvent;
        TRestSignalEvent *fSignalEvent;

        TRestReadout *fReadout;
        TRestGas *fGas;
#endif

        void InitFromConfigFile();

        void Initialize();

        void LoadDefaultConfig();

        Int_t FindModule( Int_t readoutPlane, Double_t x, Double_t y );
        Int_t FindChannel( Int_t module, Double_t x, Double_t y );

    protected:

        Double_t fSampling; // us
        Double_t fGasPressure;
        Double_t fElectricField;


    public:
        void InitProcess();
        void BeginOfEventProcess(); 
        TRestEvent *ProcessEvent( TRestEvent *eventInput );
        void EndOfEventProcess(); 
        void EndProcess();

        void LoadConfig( std::string cfgFilename, std::string name = "" );

        void PrintMetadata() 
        {
            BeginPrintProcess();

            std::cout << "Sampling : " << fSampling << " us" << std::endl;
            std::cout << "Gas pressure : " << fGasPressure << " atm" << std::endl;
            std::cout << "Electric field : " << fElectricField << " V/cm" << std::endl;

            EndPrintProcess();
        }

        TRestMetadata *GetProcessMetadata( ) { return fReadout; }

        TString GetProcessName() { return (TString) "hitsToSignal"; }

        //Constructor
        TRestHitsToSignalProcess();
        TRestHitsToSignalProcess( char *cfgFileName );
        //Destructor
        ~TRestHitsToSignalProcess();

        ClassDef(TRestHitsToSignalProcess, 1);      // Template for a REST "event process" class inherited from TRestEventProcess
};
#endif

