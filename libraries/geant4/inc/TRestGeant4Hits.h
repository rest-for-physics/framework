///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestGeant4Hits.h
///
///             Base class from which to inherit all other event classes in REST
///
///             jul 2015:   First concept
///                 Created as part of the conceptualization of existing REST
///                 software.
///                 J. Galan
///_______________________________________________________________________________

#ifndef RestCore_TRestGeant4Hits
#define RestCore_TRestGeant4Hits

#include <TArrayF.h>
#include <TArrayI.h>
#include <TRestHits.h>

#include <iostream>

#include "TObject.h"

class TRestGeant4Hits : public TRestHits {
   protected:
    TArrayI fVolumeID;
    TArrayI fProcessID;      // [fNHits]
    TArrayF fKineticEnergy;  // [fNHits]

   public:
    TArrayF fMomentumDirectionX;
    TArrayF fMomentumDirectionY;
    TArrayF fMomentumDirectionZ;

    TVector3 GetMomentumDirection(int n) {
        return TVector3(fMomentumDirectionX[n], fMomentumDirectionY[n], fMomentumDirectionZ[n]);
    }

    Int_t GetProcess(int n) { return fProcessID[n]; }

    void AddG4Hit(TVector3 pos, Double_t en, Double_t hit_global_time, Int_t process, Int_t volume,
                  Double_t eKin, TVector3 momentumDirection);
    void RemoveG4Hits();

    Int_t GetHitProcess(int n) { return fProcessID[n]; }
    Int_t GetHitVolume(int n) { return fVolumeID[n]; }
    Int_t GetVolumeId(int n) { return fVolumeID[n]; }
    Double_t GetKineticEnergy(int n) { return fKineticEnergy[n]; }

    Double_t GetEnergyInVolume(Int_t volID);

    TVector3 GetMeanPositionInVolume(Int_t volID);
    TVector3 GetFirstPositionInVolume(Int_t volID);
    TVector3 GetLastPositionInVolume(Int_t volID);

    // Constructor
    TRestGeant4Hits();
    // Destructor
    virtual ~TRestGeant4Hits();

    ClassDef(TRestGeant4Hits, 6);  // REST event superclass
};
#endif
