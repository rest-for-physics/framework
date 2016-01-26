///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///             
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestSignalToHitsProcess.h
///
///_______________________________________________________________________________


#ifndef RestCore_TRestSignalToHitsProcess
#define RestCore_TRestSignalToHitsProcess

#include <TRestGas.h>
#include <TRestReadout.h>
#include <TRestSignalEvent.h>
#include <TRestHitsEvent.h>

#include "TRestEventProcess.h"

class TRestSignalToHitsProcess:public TRestEventProcess {
    private:
        Double_t fSampling; // us
        Double_t fAnodePosition; //mm
        Double_t fCathodePosition; //mm
        Double_t fElectricField; // V/cm

#ifndef __CINT__
        TRestHitsEvent *fHitsEvent;
        TRestSignalEvent *fSignalEvent;

        TRestReadout *fReadout;
        TRestGas *fGas;
#endif

        void InitFromConfigFile();

        void Initialize();

        void LoadDefaultConfig();

    public:
        void InitProcess();
        void BeginOfEventProcess(); 
        TRestEvent *ProcessEvent( TRestEvent *eventInput );
        void EndOfEventProcess(); 
        void EndProcess();

        void LoadConfig( std::string cfgFilename );

        void PrintMetadata() 
        {
            BeginPrintProcess();

            std::cout << "Sampling rate : " << fSampling << " mm" << std::endl;
            std::cout << "Anode position : " << fAnodePosition << " mm" << std::endl;
            std::cout << "Cathode position : " << fCathodePosition << " mm" << std::endl;
            std::cout << "Electric field : " << fElectricField << " V/cm" << std::endl;

            EndPrintProcess();
        }

        TRestMetadata *GetMetadata( ) { return fReadout; }

        TString GetProcessName() { return (TString) "signalToHits"; }

        //Constructor
        TRestSignalToHitsProcess();
        TRestSignalToHitsProcess( char *cfgFileName );
        //Destructor
        ~TRestSignalToHitsProcess();

        ClassDef(TRestSignalToHitsProcess, 1);      // Template for a REST "event process" class inherited from TRestEventProcess
};
#endif

