//////////////////////////////////////////////////////////////////////////
///
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestTrackReconnectionProcess.h
///
///              Nov 2016 : Javier Galan
///
//////////////////////////////////////////////////////////////////////////

#ifndef RestCore_TRestTrackReconnectionProcess
#define RestCore_TRestTrackReconnectionProcess

#include <TRestTrackEvent.h>
#include "TRestEventProcess.h"

class TRestTrackReconnectionProcess : public TRestEventProcess {
   private:
#ifndef __CINT__
    TRestTrackEvent* fInputTrackEvent;   //!
    TRestTrackEvent* fOutputTrackEvent;  //!

    Double_t fMeanDistance;  //!
    Double_t fSigma;         //!
#endif

    Bool_t fSplitTrack;
    Double_t fNSigmas;

    void InitFromConfigFile();

    void Initialize();

    void SetDistanceMeanAndSigma(TRestHits* h);

   protected:
   public:
    any GetInputEvent() { return fInputTrackEvent; }
    any GetOutputEvent() { return fOutputTrackEvent; }

    void InitProcess();
    TRestEvent* ProcessEvent(TRestEvent* eventInput);
    void EndProcess();
    void LoadDefaultConfig();

    void LoadConfig(std::string cfgFilename, std::string name = "");

    void PrintMetadata() {
        BeginPrintProcess();

        metadata << "Split track : ";
        if (fSplitTrack)
            metadata << " enabled" << endl;
        else
            metadata << " disabled" << endl;

        metadata << "Number of sigmas to defined a branch : " << fNSigmas << endl;

        EndPrintProcess();
    }

    TString GetProcessName() { return (TString) "trackReconnection"; }

    void BreakTracks(TRestVolumeHits* hits, vector<TRestVolumeHits>& hitSets, Double_t nSigma = 2.);
    void ReconnectTracks(vector<TRestVolumeHits>& hitSets);
    Int_t GetTrackBranches(TRestHits& h, Double_t nSigma);

    // Constructor
    TRestTrackReconnectionProcess();
    TRestTrackReconnectionProcess(char* cfgFileName);
    // Destructor
    ~TRestTrackReconnectionProcess();

    ClassDef(TRestTrackReconnectionProcess, 1);
};
#endif
