//////////////////////////////////////////////////////////////////////////
///
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestTrackLinearizationProcess.h
///
///              Jan 2016 : Javier Galan
///
//////////////////////////////////////////////////////////////////////////

#ifndef RestCore_TRestTrackLinearizationProcess
#define RestCore_TRestTrackLinearizationProcess

#include <TRestLinearTrackEvent.h>
#include <TRestTrackEvent.h>
#include "TRestEventProcess.h"

class TRestTrackLinearizationProcess : public TRestEventProcess {
   private:
    Double_t fLengthResolution;
    Double_t fTransversalResolution;

    TString fMethod;
    Double_t fRadius;

    Int_t fDivisions;

#ifndef __CINT__
    TRestTrackEvent* fInputTrackEvent;               //!
    TRestLinearTrackEvent* fOutputLinearTrackEvent;  //!
#endif

    void InitFromConfigFile();

    void Initialize();

    TVector2 FindProjection(TVector3 position, TRestHits* nodes);
    Double_t GetReferenceNode(TRestHits* nHits, Double_t dist, Int_t& ref);

   protected:
   public:
    any GetInputEvent() { return fInputTrackEvent; }
    any GetOutputEvent() { return fOutputLinearTrackEvent; }

    void InitProcess();
    TRestEvent* ProcessEvent(TRestEvent* eventInput);
    void LoadDefaultConfig();

    void LoadConfig(std::string cfgFilename);

    void PrintMetadata() {
        BeginPrintProcess();
        metadata << "Lenght resolution : " << fLengthResolution << " mm" << endl;
        metadata << "Transversal resolution : " << fTransversalResolution << " mm" << endl;
        EndPrintProcess();
    }

    TString GetProcessName() { return (TString) "trackLinearization"; }

    // Constructor
    TRestTrackLinearizationProcess();
    TRestTrackLinearizationProcess(char* cfgFileName);
    // Destructor
    ~TRestTrackLinearizationProcess();

    ClassDef(TRestTrackLinearizationProcess, 1);
};
#endif
