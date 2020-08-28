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

#include "TRestDriftVolume.h"
#include "TRestReadout.h"
#include "TRestRun.h"
#include "TRestStringOutput.h"
#include "TRestTools.h"

using namespace std;

class TRestMetadata;

class TRestDetector {
   private:
    void ReadFileNameFEMINOS(string fName);
   protected:
    string fDetectorName;

    Double_t fDriftVoltage;
    Double_t fAmplificationVoltage;

    Double_t fMass;

    Double_t fTargetMass;
    Double_t fTPCHeight;
    Double_t fTPCRadius;
    Double_t fDriftDistance;
    Double_t fAmplificationDistance;

    Double_t fDAQShapingTime;
    Double_t fDAQSamplingTime;
    Double_t fDAQDynamicRange;
    Double_t fDAQThreshold;

    TRestDriftVolume* fDetectorMedium;
    TRestReadout* fReadout;

   public:
    string GetDetectorName() { return fDetectorName; }

    //////// Field property ////////
    virtual TVector3 GetDriftField(TVector3 pos) { return TVector3(); }
    virtual TVector3 GetAmplificationField(TVector3 pos) { return TVector3(); }
    virtual Double_t GetDriftVoltage() { return fDriftVoltage; }
    virtual Double_t GetAmplificationVoltage() { return fDriftVoltage; }

    //////// Medium property ////////
    virtual string GetMediumName();
    virtual Double_t GetPressure();
    virtual Double_t GetTemperature();
    virtual Double_t GetWvalue();
    virtual Double_t GetDriftVelocity();
    virtual Double_t GetElectronLifeTime();
    virtual Double_t GetLongitudinalDiffusion();
    virtual Double_t GetTransversalDiffusion();

    //////// TPC geometry ////////
    virtual Double_t GetTargetMass() { return fTargetMass; }
    virtual Double_t GetTPCHeight() { return fTPCHeight; }
    virtual Double_t GetTPCRadius() { return fTPCRadius; }
    virtual Double_t GetDriftDistance() { return fDriftDistance; }
    virtual Double_t GetAmplificationDistance() { return fAmplificationDistance; }

    //////// electronics ////////
    virtual Double_t GetDAQShapingTime() { return fDAQShapingTime; }
    virtual Double_t GetDAQSamplingTime() { return fDAQSamplingTime; }
    virtual Double_t GetDAQDynamicRange() { return fDAQDynamicRange; }
    virtual Double_t GetDAQThreshold() { return fDAQThreshold; }

    //////// readout ////////
    virtual string GetReadoutName();
    virtual Int_t GetNReadoutModules();
    virtual Int_t GetNReadoutChannels();
    virtual Double_t GetReadoutVoltage(int id);
    virtual Double_t GetReadoutGain(int id);
    virtual TVector2 GetReadoutPosition(int id);
    virtual Int_t GetReadoutType(int id);

    /// instantiate specific detector: TRestDetectorXXX according to the name XXX
    static TRestDetector* instantiate(string name = "");

    virtual void RegisterMetadata(TRestMetadata* ptr);
    virtual void RegisterString(string str);

    virtual void Print();

    // Constructors
    TRestDetector();
    // Destructor
    ~TRestDetector();
};

extern TRestDetector* gDetector;

#endif
