///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             myMetadata.h
///
///             Metadata class to be used to store basic info
///             inherited from TRestMetadata
///
///             jun 2016:   First concept. Javier Galan
//
///_______________________________________________________________________________

#ifndef _myMetadata
#define _myMetadata

#include <TRestMetadata.h>

class TRestDummyMetadata : public TRestMetadata {
   private:
    void Initialize();

    void InitFromConfigFile();

    // Define here your metadata members
    Int_t fDummy;

   public:
    // You can define methods to access your metadata members here
    Int_t GetDummyValue() { return fDummy; }

    void SetDummyValue(Int_t value) { fDummy = value; }

    void PrintMetadata();

    // Constructors
    TRestDummyMetadata();
    TRestDummyMetadata(const char* cfgFileName, std::string name = "");
    // Destructor
    ~TRestDummyMetadata();

    ClassDef(TRestDummyMetadata, 1);
};
#endif
