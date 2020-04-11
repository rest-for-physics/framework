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
#ifndef __CINT__
    TRestHitsEvent* fInputHitsEvent;   //!
    TRestHitsEvent* fOutputHitsEvent;  //!

    TH2D* htemp = NULL;                      //!
    TRestEventProcess* fCompareProc = NULL;  //!
#endif

    Double_t fZRange;
    bool fDraw;
    double fDrawThres;
    bool fDoEnergyScaling;

    void InitFromConfigFile();

    void Initialize();

   protected:
   public:
    void InitProcess();
    TRestEvent* ProcessEvent(TRestEvent* eventInput);
    void EndProcess();

    void PrintMetadata() {
        BeginPrintProcess();

        EndPrintProcess();
    }

    double LogAmbiguity(const int& n, const int& m) { return log(Ambiguity(n, m)); }
    int Ambiguity(const int& n, const int& m);
    int Factorial(const int& n);

    // Constructor
    TRestHits3DReconstructionProcess();
    // Destructor
    ~TRestHits3DReconstructionProcess();

    ClassDef(TRestHits3DReconstructionProcess, 1);
};
#endif
