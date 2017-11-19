///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///             
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestSignalGaussianConvolutionProcess.h
///
///_______________________________________________________________________________


#ifndef RestCore_TRestSignalGaussianConvolutionProcess
#define RestCore_TRestSignalGaussianConvolutionProcess

#include <TRestReadout.h>
#include <TRestSignalEvent.h>

#include "TRestEventProcess.h"

class TRestSignalGaussianConvolutionProcess:public TRestEventProcess {

    private:
        TRestSignalEvent *fInputSignalEvent;//!
        TRestSignalEvent *fOutputSignalEvent;//!

        void InitFromConfigFile();

        void Initialize();

        void LoadDefaultConfig();

        Double_t fSigma;
        Int_t fNSigmas;

    protected:
        //add here the members of your event process

    public:
        void InitProcess();
        void BeginOfEventProcess(); 
        TRestEvent *ProcessEvent( TRestEvent *eventInput );
        void EndOfEventProcess(); 
        void EndProcess();

        void LoadConfig( std::string cfgFilename, string name = "" );

        void PrintMetadata() 
        { 
            BeginPrintProcess();

            std::cout << "Gaussian sigma : " << fSigma << std::endl;
            std::cout << "Number of sigmas : " << fNSigmas << std::endl;

            EndPrintProcess();
        }

        TRestMetadata *GetProcessMetadata( ) { return NULL; }

        TString GetProcessName() { return (TString) "gaussianConvolutionProcess"; }

        //Constructor
        TRestSignalGaussianConvolutionProcess();
        TRestSignalGaussianConvolutionProcess( char *cfgFileName );
        //Destructor
        ~TRestSignalGaussianConvolutionProcess();

        ClassDef(TRestSignalGaussianConvolutionProcess, 1);      // Template for a REST "event process" class inherited from TRestEventProcess
};
#endif

