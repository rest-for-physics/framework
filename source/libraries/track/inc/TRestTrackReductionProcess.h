//////////////////////////////////////////////////////////////////////////
///
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestTrackReductionProcess.h
///
///              Jan 2016 : Javier Galan
///
//////////////////////////////////////////////////////////////////////////

#ifndef RestCore_TRestTrackReductionProcess
#define RestCore_TRestTrackReductionProcess

#include <TRestTrackEvent.h>
#include "TRestEventProcess.h"

class TRestTrackReductionProcess : public TRestEventProcess {
   private:
#ifndef __CINT__
    TRestTrackEvent* fInputTrackEvent;   //!
    TRestTrackEvent* fOutputTrackEvent;  //!
#endif

    void InitFromConfigFile();

    void Initialize();

   protected:
    Double_t fStartingDistance;
    Double_t fMinimumDistance;
    Double_t fDistanceFactor;
    Double_t fMaxNodes;

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

        metadata << " Starting distance : " << fStartingDistance << endl;
        metadata << " Minimum distance : " << fMinimumDistance << endl;
        metadata << " Distance step factor : " << fDistanceFactor << endl;
        metadata << " Maximum number of nodes : " << fMaxNodes << endl;

        EndPrintProcess();
    }

    TString GetProcessName() { return (TString) "trackReduction"; }

    // Constructor
    TRestTrackReductionProcess();
    TRestTrackReductionProcess(char* cfgFileName);
    // Destructor
    ~TRestTrackReductionProcess();

    ClassDef(TRestTrackReductionProcess, 1);  // Template for a REST "event process" class inherited from
                                              // TRestEventProcess
};
#endif
