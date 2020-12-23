//////////////////////////////////////////////////////////////////////////
///
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestDetectorHits3DReconstructionProcess.h
///
///              Nov 2016 : Javier Galan
///
//////////////////////////////////////////////////////////////////////////

#ifndef RestCore_TRestDetectorHits3DReconstructionProcess
#define RestCore_TRestDetectorHits3DReconstructionProcess

#include <TRestDetectorHitsEvent.h>

#include "TRestEventProcess.h"

class TRestDetectorHits3DReconstructionProcess : public TRestEventProcess {
   private:
    // input/output event with concrete type
    TRestDetectorHitsEvent* fInputHitsEvent;   //!
    TRestDetectorHitsEvent* fOutputHitsEvent;  //!

    // unsaved parameters, temporary data members
    TH2D* htemp = NULL;                      //!
    TRestEventProcess* fCompareProc = NULL;  //!
    int* testmember;                         //!

    // process parameters
    Double_t fZRange;
    bool fDraw;
    double fDrawThres;
    bool fDoEnergyScaling;

   protected:
    void InitFromConfigFile();
    void Initialize();

   public:
    any GetInputEvent() { return fInputHitsEvent; }
    any GetOutputEvent() { return fOutputHitsEvent; }

    void InitProcess();
    TRestEvent* ProcessEvent(TRestEvent* eventInput);
    void EndProcess();

    double LogAmbiguity(const int& n, const int& m) { return log(Ambiguity(n, m)); }
    int Ambiguity(const int& n, const int& m);
    int Factorial(const int& n);

    // Process Information Printer
    void PrintMetadata();
    // Constructor
    TRestDetectorHits3DReconstructionProcess();
    // Destructor
    ~TRestDetectorHits3DReconstructionProcess();

    ClassDef(TRestDetectorHits3DReconstructionProcess, 1);
};
#endif
