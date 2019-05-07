////////////////////////////////////////////////////////////////////////////////////
///
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestDetectorSetup.h
///
///             Metadata class to be used to store basic detector setup info
///             inherited from TRestMetadata
///
///             jun 2016:   First concept. Javier Galan
///
////////////////////////////////////////////////////////////////////////////////////

#ifndef RestCore_TRestDetectorSetup
#define RestCore_TRestDetectorSetup

#include <stdio.h>
#include <stdlib.h>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <iostream>
#include <vector>

#include <TRestMetadata.h>

class TRestDetectorSetup : public TRestMetadata {
   private:
    void Initialize();

    void InitFromConfigFile();

    Int_t fRunNumber;
    Int_t fSubRunNumber;
    TString fRunTag;

    Double_t fMeshVoltage;
    Double_t fDriftField;        // Drift field in V/cm/bar
    Double_t fDetectorPressure;  // In bar

    TString fElectronicsGain;
    TString fShapingTime;
    TString fSamplingTime;

    Double_t fSamplingInMicroSec;

   public:
    Int_t GetRunNumber() { return fRunNumber; }
    Int_t GetSubRunNumber() { return fSubRunNumber; }
    TString GetRunTag() { return fRunTag; }

    void SetRunNumber(Int_t runNumber) { fRunNumber = runNumber; }
    void SetSubRunNumber(Int_t subRunNumber) { fSubRunNumber = subRunNumber; }

    Double_t GetSamplingInMicroSeconds() { return fSamplingInMicroSec; }

    Double_t GetFieldInVPerCm() { return fDriftField * fDetectorPressure; }

    Double_t GetPressureInBar() { return fDetectorPressure; }

    void InitFromFileName(TString fName);

    void PrintMetadata();

    // Constructors
    TRestDetectorSetup();
    TRestDetectorSetup(char* cfgFileName, std::string name = "");
    // Destructor
    ~TRestDetectorSetup();

    ClassDef(TRestDetectorSetup, 1);
};
#endif
