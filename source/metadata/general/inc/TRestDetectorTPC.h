////////////////////////////////////////////////////////////////////////////////////
///
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestDetectorTPC.h
///
///             Metadata class to be used to store basic detector setup info
///             inherited from TRestMetadata
///
///             jun 2016:   First concept. Javier Galan
///
////////////////////////////////////////////////////////////////////////////////////

#ifndef RestCore_TRestDetectorTPC
#define RestCore_TRestDetectorTPC

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
class TRestDetectorReadout;
class TRestGainMap;
class TRestRun;

class TRestDetectorTPC : public TRestDetector {
   private:

   protected:
    Double_t fDriftVoltage;  // In V
    Double_t fAmplificationVoltage;// In V

    Double_t fTargetMass;// in kg
    Double_t fTPCHeight;// in mm
    Double_t fTPCRadius;// in mm
    Double_t fDriftDistance; // in mm
    Double_t fAmplificationDistance;  // in mm

    Double_t fDAQShapingTime; // in us
    Double_t fDAQSamplingTime;// in us
    Double_t fDAQDynamicRange;
    Double_t fDAQThreshold;

    TRestDriftVolume* fDetectorMedium;
    TRestDetectorReadout* fReadout;
    TRestGainMap* fGain;
    TRestRun* fRun;

   public:
    //////// Field property ////////
    virtual TVector3 GetDriftField(TVector3 pos) { return TVector3(); }
    virtual TVector3 GetAmplificationField(TVector3 pos) { return TVector3(); }
    virtual Double_t GetDriftField() { return fDriftVoltage / fDriftDistance; }
    virtual Double_t GetAmplificationField() { return fAmplificationVoltage / fAmplificationDistance; }

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
    virtual Double_t GetTPCBottomZ();
    virtual Double_t GetTPCTopZ();
    virtual Double_t GetDriftDistance(TVector3 pos);
    virtual Double_t GetAmplificationDistance(TVector3 pos);

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
    virtual TVector3 GetReadoutPosition(int id);
    virtual TVector3 GetReadoutDirection(int id);
    virtual Int_t GetReadoutType(int id);

    //////// setters for some frequent-changed parameters ////////
    virtual void SetDriftMedium(string mediumname);
    virtual void SetDriftField(Double_t df);
    virtual void SetPressure(Double_t p);
    virtual void SetDAQSamplingTime(Double_t st);
    virtual void SetElectronLifeTime(Double_t elt);

    virtual void RegisterMetadata(TObject* ptr) override;
    virtual string GetParameter(string paraname) override;
    virtual void SetParameter(string paraname, string paraval) override;

    virtual void Print() override;

    // Constructors
    TRestDetectorTPC();
    // Destructor
    ~TRestDetectorTPC();
};

#endif
