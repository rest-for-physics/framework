///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestReadoutAnalysisProcess.h
///
///_______________________________________________________________________________

#ifndef RestCore_TRestReadoutAnalysisProcess
#define RestCore_TRestReadoutAnalysisProcess

#include <TH1D.h>

//#include <TCanvas.h>

#include <TRestGas.h>
#include <TRestHitsEvent.h>
#include <TRestReadout.h>
#include <TRestSignalEvent.h>

#include "TRestEventProcess.h"

class TRestReadoutAnalysisProcess : public TRestEventProcess {
 private:
#ifndef __CINT__
  TRestSignalEvent* fSignalEvent;  //!

  TRestReadout* fReadout;  //!

#endif

  void InitFromConfigFile();

  void Initialize();

  // parameters
  map<int, double> fModuldeAmplification;  // [MM id, amplification]

  // analysis result(saved directly in root file)
  map<int, TH1D*> fChannelsHistos;  //! [MM id, channel activity]

  map<int, TH2D*> fChannelsHitMaps;  //! [MM id, channel activity]

 public:
  void InitProcess();
  void BeginOfEventProcess();
  TRestEvent* ProcessEvent(TRestEvent* eventInput);
  void EndOfEventProcess();
  void EndProcess();

  void PrintMetadata() {
    BeginPrintProcess();

    metadata << "module amplification:(module id -> amp value) " << endl;
    auto iter = fModuldeAmplification.begin();
    while (iter != fModuldeAmplification.end()) {
      metadata << iter->first << " -> " << iter->second << endl;
      iter++;
    }

    metadata << "channel activity histograms required for module: ";
    auto iter2 = fChannelsHistos.begin();
    while (iter2 != fChannelsHistos.end()) {
      metadata << iter2->first << ", ";
      iter2++;
    }
    metadata << endl;

    EndPrintProcess();
  }

  TString GetProcessName() { return (TString) "readoutAnalysis"; }

  // Constructor
  TRestReadoutAnalysisProcess();
  TRestReadoutAnalysisProcess(char* cfgFileName);
  // Destructor
  ~TRestReadoutAnalysisProcess();

  ClassDef(TRestReadoutAnalysisProcess,
           1);  // Template for a REST "event process" class inherited from
                // TRestEventProcess
};
#endif
