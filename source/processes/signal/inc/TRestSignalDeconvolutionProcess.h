///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///             
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestSignalDeconvolutionProcess.h
///
///_______________________________________________________________________________


#ifndef RestCore_TRestSignalDeconvolutionProcess
#define RestCore_TRestSignalDeconvolutionProcess

#include <TRestReadout.h>
#include <TRestSignalEvent.h>
#include <TRestHitsEvent.h>

#include "TRestEventProcess.h"

class TRestSignalDeconvolutionProcess:public TRestEventProcess {

    private:

#ifndef __CINT__
        TRestSignalEvent *fInputSignalEvent;
        TRestSignalEvent *fOutputSignalEvent;

	TCanvas *canvas;
#endif

        void InitFromConfigFile();

        void Initialize();

        void LoadDefaultConfig();


    protected:

        TString fResponseFilename;
        Double_t fFreq1;
        Double_t fFreq2;

        Int_t fCutFrequency;
        Int_t fSmoothingPoints;
        Int_t fSmearingPoints;

        Int_t fBaseLineStart;
        Int_t fBaseLineEnd;

        Int_t fFFTStart;
        Int_t fFFTEnd;


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

            std::cout << "Response filename : " << fResponseFilename << std::endl;
            std::cout << "Frequency 1 : " << fFreq1 << std::endl;
            std::cout << "Frequency 2 : " << fFreq2 << std::endl;
            std::cout << "Cut frequency : " << fCutFrequency << std::endl;

            std::cout << "Smoothing points : " << fSmoothingPoints << std::endl;
            std::cout << "Smearing points : " << fSmearingPoints << std::endl;

            std::cout << "Baseline range : ( " << fBaseLineStart << " , " << fBaseLineEnd << " ) " << std::endl;
            std::cout << "FFT remove beginning points : " << fFFTStart << std::endl;
            std::cout << "FFT remove end points : " << fFFTEnd << std::endl;

            EndPrintProcess();
        }

        TRestMetadata *GetProcessMetadata( ) { return NULL; }

        TString GetProcessName() { return (TString) "signalDeconvolution"; }

        //Constructor
        TRestSignalDeconvolutionProcess();
        TRestSignalDeconvolutionProcess( char *cfgFileName );
        //Destructor
        ~TRestSignalDeconvolutionProcess();

        ClassDef(TRestSignalDeconvolutionProcess, 1);      // Template for a REST "event process" class inherited from TRestEventProcess
};
#endif

