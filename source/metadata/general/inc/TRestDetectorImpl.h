////////////////////////////////////////////////////////////////////////////////////
///
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestDetectorImpl.h
///
///             Metadata class to be used to store basic detector setup info
///             inherited from TRestMetadata
///
///             jun 2016:   First concept. Javier Galan
///
////////////////////////////////////////////////////////////////////////////////////

#ifndef RestCore_TRestDetectorImpl
#define RestCore_TRestDetectorImpl

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

#include "TRestDetector.h"
#include "TRestStringOutput.h"
#include "TRestTools.h"

using namespace std;

class TRestMetadata;
class TRestDriftVolume;
class TRestReadout;
class TRestGainMap;

class TRestDetectorImpl : public TRestDetector {
   private:
    void ReadFileNameFEMINOS(string fName);

   protected:
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
    TRestGainMap* fGain;

   public:
    //////// Field property ////////
    virtual TVector3 GetDriftField(TVector3 pos) override { return TVector3(); }
    virtual TVector3 GetAmplificationField(TVector3 pos) override { return TVector3(); }
    virtual Double_t GetDriftVoltage() override { return fDriftVoltage; }
    virtual Double_t GetAmplificationVoltage() override { return fDriftVoltage; }

    //////// Medium property ////////
    virtual string GetMediumName() override;
    virtual Double_t GetPressure() override;
    virtual Double_t GetTemperature() override;
    virtual Double_t GetWvalue() override;
    virtual Double_t GetDriftVelocity() override;
    virtual Double_t GetElectronLifeTime() override;
    virtual Double_t GetLongitudinalDiffusion() override;
    virtual Double_t GetTransversalDiffusion() override;

    //////// TPC geometry ////////
    virtual Double_t GetTargetMass() override { return fTargetMass; }
    virtual Double_t GetTPCHeight() override { return fTPCHeight; }
    virtual Double_t GetTPCRadius() override { return fTPCRadius; }
    virtual Double_t GetTPCBottomZ() override;
    virtual Double_t GetTPCTopZ() override;
    virtual Double_t GetDriftDistance(TVector3 pos) override;
    virtual Double_t GetAmplificationDistance(TVector3 pos) override;

    //////// electronics ////////
    virtual Double_t GetDAQShapingTime() override { return fDAQShapingTime; }
    virtual Double_t GetDAQSamplingTime() override { return fDAQSamplingTime; }
    virtual Double_t GetDAQDynamicRange() override { return fDAQDynamicRange; }
    virtual Double_t GetDAQThreshold() override { return fDAQThreshold; }

    //////// readout ////////
    virtual string GetReadoutName() override;
    virtual Int_t GetNReadoutModules() override;
    virtual Int_t GetNReadoutChannels() override;
    virtual Double_t GetReadoutVoltage(int id) override;
    virtual Double_t GetReadoutGain(int id) override;
    virtual TVector3 GetReadoutPosition(int id) override;
    virtual TVector3 GetReadoutDirection(int id) override;
    virtual Int_t GetReadoutType(int id) override;

    virtual void RegisterMetadata(TObject* ptr) override;
    virtual void RegisterString(string str) override;

    virtual void Print() override;

    // Constructors
    TRestDetectorImpl();
    // Destructor
    ~TRestDetectorImpl();
};

#endif
