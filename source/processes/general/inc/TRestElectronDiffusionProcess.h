///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestElectronDiffusionProcess.h
///
///_______________________________________________________________________________

#ifndef RestCore_TRestElectronDiffusionProcess
#define RestCore_TRestElectronDiffusionProcess

#include <TRestGas.h>
#include <TRestHitsEvent.h>
#include <TRestReadout.h>

#include <TRandom3.h>

#include "TRestEventProcess.h"

class TRestElectronDiffusionProcess : public TRestEventProcess {
   private:
    TRestHitsEvent* fInputHitsEvent;   //!
    TRestHitsEvent* fOutputHitsEvent;  //!

    TRandom3* fRandom;  //!

    void InitFromConfigFile();

    void Initialize();

    void LoadDefaultConfig();

   protected:
    Double_t fAttachment;
    Double_t fWvalue;
    Double_t fLonglDiffCoeff;
    Double_t fTransDiffCoeff;

    Int_t fMaxHits;

    Double_t fSeed = 0;

   public:
    any GetInputEvent() { return fInputHitsEvent; }
    any GetOutputEvent() { return fOutputHitsEvent; }

    void InitProcess();
    TRestEvent* ProcessEvent(TRestEvent* eventInput);
    void EndProcess();

    void LoadConfig(std::string cfgFilename, std::string name = "");

    void PrintMetadata() {
        BeginPrintProcess();

        metadata << " Attachment coeficient : " << fAttachment << " cm-1" << endl;
        metadata << " Longitudinal diffusion coefficient : " << fLonglDiffCoeff << " cm^1/2" << endl;
        metadata << " Transversal diffusion coefficient : " << fTransDiffCoeff << " cm^1/2" << endl;
        metadata << " W value : " << fWvalue << " eV" << endl;

        metadata << " Maximum number of hits : " << fMaxHits << endl;

        metadata << " Seed : " << fSeed << endl;

        EndPrintProcess();
    }

    TString GetProcessName() { return (TString) "electronDiffusion"; }

    Double_t GetAttachmentCoefficient() { return fAttachment; }

    // Constructor
    TRestElectronDiffusionProcess();
    TRestElectronDiffusionProcess(char* cfgFileName);
    // Destructor
    ~TRestElectronDiffusionProcess();

    ClassDef(TRestElectronDiffusionProcess, 3);  // Template for a REST "event process" class inherited from
                                                 // TRestEventProcess
};
#endif
