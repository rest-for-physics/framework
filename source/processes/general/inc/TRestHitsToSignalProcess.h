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
        Double_t fSampling; // us
        Double_t fCathodePosition;
        Double_t fElectricField;

#ifndef __CINT__
        TRestHitsEvent *fHitsEvent;
        TRestSignalEvent *fSignalEvent;

        TRestReadout *fReadout;

	TRestGas *fGas;
#endif

        void InitFromConfigFile();

        void Initialize();

        void LoadDefaultConfig();

        Int_t FindModule( Double_t x, Double_t y );
        Int_t FindChannel( Int_t module, Double_t x, Double_t y );

        TRestGas *GetGasFromRunMetadata( );
        Double_t GetCathodePositionFromElectronDiffusionProcess( );
        Double_t GetElectricFieldFromElectronDiffusionProcess( );

    public:
        void InitProcess();
        void BeginOfEventProcess(); 
        TRestEvent *ProcessEvent( TRestEvent *eventInput );
        void EndOfEventProcess(); 
        void EndProcess();

        void PrintMetadata() 
        {
            BeginPrintProcess();

            cout << "Sampling : " << fSampling << " us" << endl;
            cout << "Cathode position : " << fCathodePosition << " mm" << endl;
            cout << "Electric field : " << fElectricField << " V/cm" << endl;

            EndPrintProcess();
        }

        TRestMetadata *GetMetadata( ) { return fReadout; }

        TString GetProcessName() { return (TString) "chargeSegmentation"; }

        //Constructor
        TRestHitsToSignalProcess();
        TRestHitsToSignalProcess( char *cfgFileName );
        //Destructor
        ~TRestHitsToSignalProcess();

        ClassDef(TRestHitsToSignalProcess, 1);      // Template for a REST "event process" class inherited from TRestEventProcess
};
#endif

