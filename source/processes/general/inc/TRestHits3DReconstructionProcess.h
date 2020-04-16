//////////////////////////////////////////////////////////////////////////
///
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestHits3DReconstructionProcess.h
///
///              Nov 2016 : Javier Galan
///
//////////////////////////////////////////////////////////////////////////

#ifndef RestCore_TRestHits3DReconstructionProcess
#define RestCore_TRestHits3DReconstructionProcess

#include <TRestHitsEvent.h>

#include "TRestEventProcess.h"

class TRestHits3DReconstructionProcess : public TRestEventProcess {
   private:
    // input/output event with concrete type
    TRestHitsEvent* fInputHitsEvent;   //!
    TRestHitsEvent* fOutputHitsEvent;  //!

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
    TRestHits3DReconstructionProcess();
    // Destructor
    ~TRestHits3DReconstructionProcess();

    ClassDef(TRestHits3DReconstructionProcess, 1);
};
#endif
