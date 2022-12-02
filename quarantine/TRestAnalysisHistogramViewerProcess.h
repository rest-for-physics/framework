///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestAnalysisHistogramViewerProcess.h
///
///_______________________________________________________________________________

#ifndef RestCore_TRestAnalysisHistogramViewerProcess
#define RestCore_TRestAnalysisHistogramViewerProcess

#include <TRestTrackEvent.h>

#include "TCanvas.h"
#include "TH1D.h"
#include "TRestEventProcess.h"

class TRestAnalysisHistogramViewerProcess : public TRestEventProcess {
   private:
#ifndef __CINT__
    TRestTrackEvent* fInputTrackEvent;   //!
    TRestTrackEvent* fOutputTrackEvent;  //!

    TCanvas* fAnaHistoCanvas;   //!
    TVector2 fHistoCanvasSize;  //!

    TString fPlotString;  //!
#endif

    void InitFromConfigFile();

    void Initialize();

    void LoadDefaultConfig();

   protected:
    // add here the members of your event process

   public:
    any GetInputEvent() { return fInputTrackEvent; }
    any GetOutputEvent() { return fOutputTrackEvent; }

    void InitProcess();
    TRestEvent* ProcessEvent(TRestEvent* eventInput);
    void EndProcess();

    void LoadConfig(std::string configFilename, std::string name = "");

    void PrintMetadata() {
        BeginPrintProcess();

        EndPrintProcess();
    }

    TString GetProcessName() { return (TString) "histogramAnalysisViewer"; }

    // Constructor
    TRestAnalysisHistogramViewerProcess();
    TRestAnalysisHistogramViewerProcess(const char* configFilename);
    // Destructor
    ~TRestAnalysisHistogramViewerProcess();

    ClassDef(TRestAnalysisHistogramViewerProcess,
             1);  // Template for a REST "event process" class inherited from
                  // TRestEventProcess
};
#endif
