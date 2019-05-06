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
#ifndef __CINT__
  TRestHitsEvent* fInputHitsEvent;   //!
  TRestHitsEvent* fOutputHitsEvent;  //!

  TRestGas* fGas;          //!
  TRestReadout* fReadout;  //!

  TRandom3* fRandom;  //!
#endif

  void InitFromConfigFile();

  void Initialize();

  void LoadDefaultConfig();

 protected:
  Double_t fElectricField;
  Double_t fAttachment;
  Double_t fGasPressure;
  Double_t fWvalue;
  Double_t fLonglDiffCoeff;
  Double_t fTransDiffCoeff;

  Int_t fMaxHits;

 public:
  void InitProcess();
  void BeginOfEventProcess();
  TRestEvent* ProcessEvent(TRestEvent* eventInput);
  void EndOfEventProcess();
  void EndProcess();

  void LoadConfig(std::string cfgFilename, std::string name = "");

  void PrintMetadata() {
    BeginPrintProcess();

    std::cout << " eField : " << fElectricField << " V/cm" << std::endl;
    std::cout << " attachment coeficient : " << fAttachment << " V/cm"
              << std::endl;
    std::cout << " gas pressure : " << fGasPressure << " atm" << std::endl;
    std::cout << " longitudinal diffusion coefficient : " << fLonglDiffCoeff
              << " cm^1/2" << std::endl;
    std::cout << " transversal diffusion coefficient : " << fTransDiffCoeff
              << " cm^1/2" << std::endl;
    std::cout << " W value : " << fWvalue << " eV" << std::endl;

    std::cout << " Maximum number of hits : " << fMaxHits << endl;

    EndPrintProcess();
  }

  TRestMetadata* GetProcessMetadata() { return fGas; }

  TString GetProcessName() { return (TString) "electronDiffusion"; }

  Double_t GetElectricField() { return fElectricField; }
  Double_t GetAttachmentCoefficient() { return fAttachment; }
  Double_t GetGasPressure() { return fGasPressure; }

  // Constructor
  TRestElectronDiffusionProcess();
  TRestElectronDiffusionProcess(char* cfgFileName);
  // Destructor
  ~TRestElectronDiffusionProcess();

  ClassDef(TRestElectronDiffusionProcess,
           1);  // Template for a REST "event process" class inherited from
                // TRestEventProcess
};
#endif
