///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestDetectorHitsToTrackProcess.h
///
///_______________________________________________________________________________

#ifndef RestCore_TRestDetectorHitsToTrackProcess
#define RestCore_TRestDetectorHitsToTrackProcess

#include <TRestDetectorHitsEvent.h>
#include <TRestTrackEvent.h>
#include "TMatrixD.h"
#include "TRestEventProcess.h"

class TRestDetectorHitsToTrackProcess : public TRestEventProcess {
   private:
#ifndef __CINT__
    TRestDetectorHitsEvent* fHitsEvent;    //!
    TRestTrackEvent* fTrackEvent;  //!
#endif

    void InitFromConfigFile();

    void Initialize();
    Int_t FindTracks(TRestHits* hits);

   protected:
    // add here the members of your event process
    Double_t fClusterDistance;

   public:
    any GetInputEvent() { return fHitsEvent; }
    any GetOutputEvent() { return fTrackEvent; }

    void InitProcess();
    TRestEvent* ProcessEvent(TRestEvent* eventInput);
    void EndProcess();
    void LoadDefaultConfig();

    void LoadConfig(std::string cfgFilename, std::string name = "");

    void PrintMetadata() {
        BeginPrintProcess();

        metadata << " cluster-distance : " << fClusterDistance << " mm " << endl;

        EndPrintProcess();
    }

    TString GetProcessName() { return (TString) "hitsToTrack"; }

    // Constructor
    TRestDetectorHitsToTrackProcess();
    TRestDetectorHitsToTrackProcess(char* cfgFileName);
    // Destructor
    ~TRestDetectorHitsToTrackProcess();

    ClassDef(TRestDetectorHitsToTrackProcess, 1);  // Template for a REST "event process" class inherited from
                                                   // TRestEventProcess
};
#endif
