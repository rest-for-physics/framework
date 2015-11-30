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

//#include <TCanvas.h>

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

        //TCanvas *fCanvas;

        Double_t fFreq1;
        Double_t fFreq2;
        Int_t fSmoothingPoints;
        Int_t fSmearingPoints;

    protected:
        //add here the members of your event process

    public:
        void InitProcess();
        void BeginOfEventProcess(); 
        TRestEvent *ProcessEvent( TRestEvent *eventInput );
        void EndOfEventProcess(); 
        void EndProcess();

        void PrintMetadata() { cout << "TODO : Needs to be implemented" << endl; }

        TRestMetadata *GetMetadata( ) { return NULL; }

        TString GetProcessName() { return (TString) "signalDeconvolution"; }

        //Constructor
        TRestSignalDeconvolutionProcess();
        TRestSignalDeconvolutionProcess( char *cfgFileName );
        //Destructor
        ~TRestSignalDeconvolutionProcess();

        ClassDef(TRestSignalDeconvolutionProcess, 1);      // Template for a REST "event process" class inherited from TRestEventProcess
};
#endif

