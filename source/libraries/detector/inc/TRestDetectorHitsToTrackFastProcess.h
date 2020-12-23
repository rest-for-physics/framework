///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestDetectorHitsToTrackFastProcess.h
///
///_______________________________________________________________________________

#ifndef RestCore_TRestDetectorHitsToTrackFastProcess
#define RestCore_TRestDetectorHitsToTrackFastProcess

#include <TRestEventProcess.h>
#include <TRestDetectorHitsEvent.h>
#include <TRestTrackEvent.h>

#include <TVector3.h>

class TRestDetectorHitsToTrackFastProcess : public TRestEventProcess {
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

    Double_t fCellResolution;
    Double_t fNetSize;
    TVector3 fNetOrigin;
    Int_t fNodes;

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

        metadata << " Cell resolution : " << fCellResolution << " mm " << endl;
        metadata << " Net size : " << fNetSize << " mm " << endl;
        metadata << " Net origin : ( " << fNetOrigin.X() << " , " << fNetOrigin.Y() << " , " << fNetOrigin.Z()
                 << " ) mm " << endl;
        metadata << " Number of nodes (per axis) : " << fNodes << endl;

        EndPrintProcess();
    }

    TString GetProcessName() { return (TString) "fastHitsToTrack"; }

    // Constructor
    TRestDetectorHitsToTrackFastProcess();
    TRestDetectorHitsToTrackFastProcess(char* cfgFileName);
    // Destructor
    ~TRestDetectorHitsToTrackFastProcess();

    ClassDef(TRestDetectorHitsToTrackFastProcess,
             1);  // Template for a REST "event process" class inherited from
                  // TRestEventProcess
};
#endif
