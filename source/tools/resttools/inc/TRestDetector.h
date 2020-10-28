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
#include "TRestStringOutput.h"
#include "TRestTools.h"

using namespace std;

class TRestDetector : public map<string, string> {
   protected:
    string fDetectorName;
    Int_t fRunNumber;

   public:
    // Constructors
    TRestDetector() {}
    // Destructor
    ~TRestDetector() {}

    Int_t GetRunNumber() const { return fRunNumber; }
    void SetRunNumber(const Int_t& run) { fRunNumber = run; }
    string GetDetectorName() const { return fDetectorName; }

    // virtual methods
    virtual string GetParameter(const string& paraname) const {
        if (this->count(paraname) > 0) return this->at(paraname);
        return PARAMETER_NOT_FOUND_STR;
    }
    virtual void SetParameter(const string& paraname, const string& paraval);
    virtual void Print() const;
    virtual void WriteFile(TFile* f) const;
    virtual void ReadFile(TFile* f);
};

extern TRestDetector* gDetector;

#endif
