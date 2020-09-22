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
    Int_t fRunNumber;

   public:
    // Constructors
    TRestDetector() {}
    // Destructor
    ~TRestDetector() {}

    Int_t GetRunNumber() { return fRunNumber; }
    void SetRunNumber(Int_t run) { fRunNumber = run; }
    string GetDetectorName() { return fDetectorName; }

    // 
    virtual string GetParameter(string paraname) { return PARAMETER_NOT_FOUND_STR; }
    virtual void SetParameter(string paraname, string paraval) { }
    virtual void RegisterMetadata(TObject* ptr) {}
    virtual void RegisterString(string str) {}
    virtual void Print() {}
};

extern TRestDetector* gDetector;

#endif
