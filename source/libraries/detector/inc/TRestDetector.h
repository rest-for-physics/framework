////////////////////////////////////////////////////////////////////////////////////
///
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestDetector.h
///
///             Metadata class to be used to store basic detector setup info
///             inherited from TRestMetadata
///
///             jun 2016:   First concept. Javier Galan
///
////////////////////////////////////////////////////////////////////////////////////

#ifndef RestCore_TRestDetector
#define RestCore_TRestDetector

#include <Rtypes.h>
#include <TVector3.h>
#include <stdio.h>
#include <stdlib.h>

#include <cstdio>
#include <cstring>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "TFile.h"
#include "TRestMetadata.h"
#include "TRestStringOutput.h"
#include "TRestTools.h"

using namespace std;

class TRestDetector : public TRestMetadata {
   public:
    string fDetectorName = "REST Detector";
    Int_t fRunNumber = 0;

    Double_t fDriftVoltage;   // In V
    Double_t fDriftDistance;  // in mm
    Double_t fDriftGain;      // in mm

    Double_t fAmplificationVoltage;   // In V
    Double_t fAmplificationDistance;  // in mm
    Double_t fAmplificationGain;      //

    Double_t fMass;    // in kg
    Double_t fHeight;  // in mm
    Double_t fRadius;  // in mm
    Double_t fLength;  // in mm
    Double_t fWidth;   // in mm

    Double_t fDAQShapingTime;   // in us
    Double_t fDAQSamplingTime;  // in us
    Double_t fDAQDynamicRange;
    Double_t fDAQThreshold;

    void InitFromConfigFile() { ReadAllParameters(); }
    void PrintMetadata();

    // Constructors
    TRestDetector() {}
    // Destructor
    ~TRestDetector() {}

    ClassDef(TRestDetector, 1);
};

#endif
