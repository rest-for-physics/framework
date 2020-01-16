///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestG4Hits.h
///
///             Base class from which to inherit all other event classes in REST
///
///             jul 2015:   First concept
///                 Created as part of the conceptualization of existing REST
///                 software.
///                 J. Galan
///_______________________________________________________________________________

#ifndef RestCore_TRestG4Hits
#define RestCore_TRestG4Hits

#include <TArrayI.h>
#include <TRestHits.h>

#include <iostream>

#include "TObject.h"

class TRestG4Hits : public TRestHits {
   protected:
    TArrayI fVolumeID;
    TArrayI fProcessID;  // [fNHits]

   public:
    Int_t GetProcess(int n) { return fProcessID[n]; }

    void AddG4Hit(TVector3 pos, Double_t en, Double_t hit_global_time, Int_t process, Int_t volume);
    void AddG4Hit(Double_t X, Double_t Y, Double_t Z, Double_t en, Int_t process, Int_t volume);
    void RemoveG4Hits();

    Int_t GetHitProcess(int n) { return fProcessID[n]; }
    Int_t GetHitVolume(int n) { return fVolumeID[n]; }
    Int_t GetVolumeId(int n) { return fVolumeID[n]; }

    Double_t GetEnergyInVolume(Int_t volID);

    TVector3 GetMeanPositionInVolume(Int_t volID);
    TVector3 GetFirstPositionInVolume(Int_t volID);
    TVector3 GetLastPositionInVolume(Int_t volID);

    // Constructor
    TRestG4Hits();
    // Destructor
    virtual ~TRestG4Hits();

    ClassDef(TRestG4Hits, 3);  // REST event superclass
};
#endif
