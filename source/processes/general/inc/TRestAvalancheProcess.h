///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///             
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestAvalancheProcess.h
///
///             feb 2016:   First concept
///                 Created as part of the conceptualization of existing REST 
///                 software.
///                 Javier G. Garza
///_______________________________________________________________________________


#ifndef RestCore_TRestAvalancheProcess
#define RestCore_TRestAvalancheProcess

#include <TRestGas.h>
#include <TRestHitsEvent.h>

#include "TRestEventProcess.h"

class TRestAvalancheProcess:public TRestEventProcess {
    private:
        TRestHitsEvent *fHitsInputEvent;//!
        TRestHitsEvent *fHitsOutputEvent;//!

        Double_t fEnergyRef;	///< reference energy for the FWHM
        Double_t fResolutionAtEref; ///< FWHM at Energy of reference
        Double_t fDetectorGain;	///< Detector's gain.

        void InitFromConfigFile();
        void Initialize();
        void LoadDefaultConfig();

    protected:
        //add here the members of your event process
        TRestGas *fGas;//!


    public:
        void InitProcess();
        void BeginOfEventProcess(); 
        TRestEvent *ProcessEvent( TRestEvent *eventInput );
        void EndOfEventProcess(); 
        void EndProcess();

        void LoadConfig( std::string cfgFilename );

        void PrintMetadata() { 

            BeginPrintProcess();

            std::cout << " reference energy (Eref): " << fEnergyRef << std::endl;
            std::cout << " resolution at Eref : " << fResolutionAtEref << std::endl;
            std::cout << " detector gain : " << fDetectorGain << std::endl;

            EndPrintProcess();

        }

        TRestMetadata *GetProcessMetadata( ) { return fGas; }

        TString GetProcessName() { return (TString) "avalancheProcess"; }

        Double_t GetEnergyReference() { return fEnergyRef; }
        Double_t GetResolutionReference() { return fResolutionAtEref; }
        Double_t GetDetectorGain() { return fDetectorGain; }

        //Constructor
        TRestAvalancheProcess();
        TRestAvalancheProcess( char *cfgFileName );
        //Destructor
        ~TRestAvalancheProcess();

        ClassDef(TRestAvalancheProcess, 1);      // Template for a REST "event process" class inherited from TRestEventProcess
};
#endif

