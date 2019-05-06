///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestDAQMetadata inherited from TRestMetadata
///
///             Load metadata of the DAQ
///
///             Jan 2016:   First concept
///                 Created as part of the conceptualization of existing REST
///                 software.
///                 JuanAn Garcia
///_______________________________________________________________________________

#ifndef RestCore_TRestDAQMetadata
#define RestCore_TRestDAQMetadata

#include <iostream>

#include "TRestMetadata.h"
#include "TString.h"

class TRestDAQMetadata : public TRestMetadata {
 private:
  void InitFromConfigFile();

  virtual void Initialize();

 protected:
  TString fOutBinFileName;
  TString fElectronicsType;
  std::vector<TString> fPedBuffer;  // Pedestal script
  std::vector<TString> fRunBuffer;  // Run script
  TString fNamePedScript;  // Name of the run script e.g. /home/user/scripts/run
  TString fNameRunScript;  // Name of the pedestal script e.g.
                           // /home/user/scripts/ped
  UInt_t fGain;  // Value of the gain in the script you have to convert it to fC
  UInt_t fShappingTime;  // Value of the shapping time in the script you have to
                         // convert it to nS

 public:
  void PrintMetadata();
  void PrintRunScript();
  void PrintPedScript();

  // Construtor
  TRestDAQMetadata();
  TRestDAQMetadata(char* cfgFileName);
  // Destructor
  virtual ~TRestDAQMetadata();

  void SetScriptsBuffer();
  void SetParFromPedBuffer();  // Set gain and shapping time from a given buffer
  void SetOutBinFileName(TString fName) { fOutBinFileName = fName; }

  UInt_t GetGain() { return fGain; }
  UInt_t GetShappingTime() { return fShappingTime; }
  UInt_t GetValFromString(TString var, TString line);

  ClassDef(TRestDAQMetadata, 1);  // REST run class
};
#endif
