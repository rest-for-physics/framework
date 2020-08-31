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

#include "TRestStringOutput.h"
#include "TRestTools.h"

using namespace std;

class TRestDetector {
   protected:
    string fDetectorName;

   public:
    string GetDetectorName() { return fDetectorName; }

    //////// Field property ////////
    virtual TVector3 GetDriftField(TVector3 pos) { return TVector3(); }
    virtual TVector3 GetAmplificationField(TVector3 pos) { return TVector3(); }
    virtual Double_t GetDriftVoltage() { return 0; }
    virtual Double_t GetAmplificationVoltage() { return 0; }

    //////// Medium property ////////
    virtual string GetMediumName() { return ""; }
    virtual Double_t GetPressure() { return 0; }
    virtual Double_t GetTemperature() { return 0; }
    virtual Double_t GetWvalue() { return 0; }
    virtual Double_t GetDriftVelocity() { return 0; }
    virtual Double_t GetElectronLifeTime() { return 0; }
    virtual Double_t GetLongitudinalDiffusion() { return 0; }
    virtual Double_t GetTransversalDiffusion() { return 0; }

    //////// TPC geometry ////////
    virtual Double_t GetTargetMass() { return 0; }
    virtual Double_t GetTPCHeight() { return 0; }
    virtual Double_t GetTPCRadius() { return 0; }
    virtual Double_t GetTPCBottomZ() { return 0; };
    virtual Double_t GetTPCTopZ() { return 0; };
    virtual Double_t GetDriftDistance(TVector3 pos) { return 0; }
    virtual Double_t GetAmplificationDistance(TVector3 pos) { return 0; }

    //////// electronics ////////
    virtual Double_t GetDAQShapingTime() { return 0; }
    virtual Double_t GetDAQSamplingTime() { return 0; }
    virtual Double_t GetDAQDynamicRange() { return 0; }
    virtual Double_t GetDAQThreshold() { return 0; }

    //////// readout ////////
    virtual string GetReadoutName() { return ""; }
    virtual Int_t GetNReadoutModules() { return 0; }
    virtual Int_t GetNReadoutChannels() { return 0; }
    virtual Double_t GetReadoutVoltage(int id) { return 0; }
    virtual Double_t GetReadoutGain(int id) { return 0; }
    virtual TVector2 GetReadoutPosition(int id) { return 0; }
    virtual Int_t GetReadoutType(int id) { return 0; }

    /// instantiate specific detector: TRestDetectorXXX according to the name XXX
    static TRestDetector* instantiate(string name = "");

    virtual void RegisterMetadata(TObject* ptr) {}
    virtual void RegisterString(string str) {}

    virtual void Print() {}

    // Constructors
    TRestDetector() {}
    // Destructor
    ~TRestDetector() {}
};

extern TRestDetector* gDetector;

#endif
