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

#include <TRestHitsEvent.h>
#include <TRestReadout.h>
#include <TRestSignalEvent.h>

#include "TRestEventProcess.h"

class TRestSignalDeconvolutionProcess : public TRestEventProcess {
   private:
#ifndef __CINT__
    TRestSignalEvent* fInputSignalEvent;   //!
    TRestSignalEvent* fOutputSignalEvent;  //!

    TCanvas* canvas;  //!
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
    TRestEvent* ProcessEvent(TRestEvent* eventInput);
    void EndOfEventProcess();
    void EndProcess();

    void LoadConfig(std::string cfgFilename);

    void PrintMetadata() {
        BeginPrintProcess();

        metadata << "Response filename : " << fResponseFilename << endl;
        metadata << "Frequency 1 : " << fFreq1 << endl;
        metadata << "Frequency 2 : " << fFreq2 << endl;
        metadata << "Cut frequency : " << fCutFrequency << endl;

        metadata << "Smoothing points : " << fSmoothingPoints << endl;
        metadata << "Smearing points : " << fSmearingPoints << endl;

        metadata << "Baseline range : ( " << fBaseLineStart << " , " << fBaseLineEnd << " ) " << endl;
        metadata << "FFT remove beginning points : " << fFFTStart << endl;
        metadata << "FFT remove end points : " << fFFTEnd << endl;

        EndPrintProcess();
    }

    TRestMetadata* GetProcessMetadata() { return NULL; }

    TString GetProcessName() { return (TString) "signalDeconvolution"; }

    // Constructor
    TRestSignalDeconvolutionProcess();
    TRestSignalDeconvolutionProcess(char* cfgFileName);
    // Destructor
    ~TRestSignalDeconvolutionProcess();

    ClassDef(TRestSignalDeconvolutionProcess, 1);  // Template for a REST "event process" class inherited from
                                                   // TRestEventProcess
};
#endif
