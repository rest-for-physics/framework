///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///             
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestPointLikeAnaProcess.h
///
///_______________________________________________________________________________


#ifndef RestCore_TRestPointLikeAnaProcess
#define RestCore_TRestPointLikeAnaProcess


#include <TRestSignalEvent.h>
#include "TRestReadout.h"

#include "TRestEventProcess.h"

class TRestPointLikeAnaProcess:public TRestEventProcess {
    private:
#ifndef __CINT__
        TRestSignalEvent *fSignalEvent;
        // TODO We must get here a pointer to TRestDaqMetadata
        // In order to convert the parameters to time using the sampling time
#endif

        void InitFromConfigFile();

        void Initialize();

        void LoadDefaultConfig();

        Int_t fThreshold;
        Int_t fPedLevel;
        Int_t fNChannels;
        Double_t fPitch;
        TString fGainFilename;
     
        Double_t fCalGain;

    protected:
        
        //add here the members of your event process
        TRestReadout *fReadout;

    public:
        void InitProcess();
        void BeginOfEventProcess(); 
        TRestEvent *ProcessEvent( TRestEvent *eventInput );
        void EndOfEventProcess(); 
        void EndProcess();

        void LoadConfig( std::string cfgFilename );

        void PrintMetadata() { 

            BeginPrintProcess();

            std::cout << "Calibration File Name : " << fGainFilename << std::endl;

            std::cout << "Selection threshold : " << fThreshold << " ADC units" << std::endl;
            std::cout << "Pedestal level : " << fPedLevel << " ADC units" << std::endl;
            std::cout << "Number of channels: " << fNChannels << " ADC units" << std::endl;
            std::cout << "Readout pitch: " << fPitch << " mm" << std::endl;

            EndPrintProcess();

        }

        TString GetProcessName() { return (TString) "pointLikeAna"; }
        Double_t GetCalibFactor();           ///< Calibration factor is found.

        //Constructor
        TRestPointLikeAnaProcess();
        TRestPointLikeAnaProcess( char *cfgFileName );
        //Destructor
        ~TRestPointLikeAnaProcess();

        ClassDef(TRestPointLikeAnaProcess, 1);      // Template for a REST "event process" class inherited from TRestEventProcess
};
#endif

