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
        TRestSignalEvent *fInputSignalEvent;
        TRestSignalEvent *fOutputSignalEvent;

        void InitFromConfigFile();

        void Initialize();

        void LoadDefaultConfig();

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


    protected:
        //add here the members of your event process

    public:
        void InitProcess();
        void BeginOfEventProcess(); 
        TRestEvent *ProcessEvent( TRestEvent *eventInput );
        void EndOfEventProcess(); 
        void EndProcess();

        void LoadConfig( string cfgFilename );

        void PrintMetadata() 
        { 
            BeginPrintProcess();

            cout << "Response filename : " << fResponseFilename << endl;
            cout << "Frequency 1 : " << fFreq1 << endl;
            cout << "Frequency 2 : " << fFreq2 << endl;
            cout << "Cut frequency : " << fCutFrequency << endl;

            cout << "Smoothing points : " << fSmoothingPoints << endl;
            cout << "Smearing points : " << fSmearingPoints << endl;

            cout << "Baseline range : ( " << fBaseLineStart << " , " << fBaseLineEnd << " ) " << endl;
            cout << "FFT remove beginning points : " << fFFTStart << endl;
            cout << "FFT remove end points : " << fFFTEnd << endl;

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

