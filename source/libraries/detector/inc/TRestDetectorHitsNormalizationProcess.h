///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestDetectorHitsNormalizationProcess.h
///
///             feb 2016:   First concept
///                 Created as part of the conceptualization of existing REST
///                 software.
///                 Javier G. Garza
///_______________________________________________________________________________

#ifndef RestCore_TRestDetectorHitsNormalizationProcess
#define RestCore_TRestDetectorHitsNormalizationProcess

#include <TRestDetectorHitsEvent.h>

#include "TRestEventProcess.h"

class TRestDetectorHitsNormalizationProcess : public TRestEventProcess {
   private:
#ifndef __CINT__

    TRestDetectorHitsEvent* fHitsInputEvent;   //!
    TRestDetectorHitsEvent* fHitsOutputEvent;  //!

#endif

    void InitFromConfigFile();
    void Initialize();
    void LoadDefaultConfig();

   protected:
    // add here the members of your event process

    Double_t fFactor;

   public:
    any GetInputEvent() { return fHitsInputEvent; }
    any GetOutputEvent() { return fHitsOutputEvent; }

    void InitProcess();
    TRestEvent* ProcessEvent(TRestEvent* eventInput);
    void EndProcess();

    void LoadConfig(std::string cfgFilename, std::string name = "");

    void PrintMetadata() {
        BeginPrintProcess();

        metadata << " Renormalization factor : " << fFactor << endl;

        EndPrintProcess();
    }

    TRestMetadata* GetProcessMetadata() { return NULL; }

    TString GetProcessName() { return (TString) "hitsNormalizationProcess"; }

    // Constructor
    TRestDetectorHitsNormalizationProcess();
    TRestDetectorHitsNormalizationProcess(char* cfgFileName);
    // Destructor
    ~TRestDetectorHitsNormalizationProcess();

    ClassDef(TRestDetectorHitsNormalizationProcess, 1);  // Template for a REST "event process" class inherited from
                                                 // TRestEventProcess
};
#endif
