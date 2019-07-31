///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestTrack.h
///
///             Event class to store signals form simulation and acquisition
///             events
///
///             oct 2015:   First concept
///                 Created as part of the conceptualization of existing REST
///                 software.
///                 Javier Gracia
///_______________________________________________________________________________

#ifndef RestCore_TRestTrack
#define RestCore_TRestTrack

#include <iostream>

#include <TObject.h>
#include <TRestVolumeHits.h>
#include <TVector3.h>

class TRestTrack : public TObject {
   protected:
    Int_t fTrackID;         ///< Track ID
    Int_t fParentID;        ///< Parent ID
    Double_t fTrackEnergy;  ///< Total energy of the track
    Double_t fTrackLength;  ///< Total length of the track

    // Hit volumes
    TRestVolumeHits fVolumeHits;  ///< Hit volumes that define a track

   public:
    void Initialize();

    // Setters
    void SetTrackID(Int_t sID) { fTrackID = sID; }
    void SetParentID(Int_t pID) { fParentID = pID; }

    void SetVolumeHits(TRestVolumeHits hits);
    void RemoveVolumeHits();

    Bool_t isXY() { return (&fVolumeHits)->areXY(); }
    Bool_t isXZ() { return (&fVolumeHits)->areXZ(); }
    Bool_t isYZ() { return (&fVolumeHits)->areYZ(); }
    Bool_t isXYZ() { return (&fVolumeHits)->areXYZ(); }

    // Getters
    Int_t GetTrackID() { return fTrackID; }
    Int_t GetParentID() { return fParentID; }
    Double_t GetEnergy() { return fTrackEnergy; }
    Double_t GetTrackEnergy() { return fTrackEnergy; }

    Double_t GetLength(Bool_t update = true) { return GetTrackLength(update); }
    Double_t GetTrackLength(Bool_t update = true) {
        if (update) fTrackLength = fVolumeHits.GetTotalDistance();
        return fTrackLength;
    }

    Double_t GetMaximumDistance() { return fVolumeHits.GetMaximumHitDistance(); }
    Double_t GetMaximumDistance2() { return fVolumeHits.GetMaximumHitDistance2(); }
    Double_t GetVolume() { return fVolumeHits.GetMaximumHitDistance2(); }

    TVector3 GetMeanPosition() { return fVolumeHits.GetMeanPosition(); }

    TRestVolumeHits* GetVolumeHits() { return &fVolumeHits; }
    TRestHits* GetHits() { return (TRestHits*)&fVolumeHits; }
    Int_t GetNumberOfHits() { return GetVolumeHits()->GetNumberOfHits(); }

    void PrintTrack(Bool_t fullInfo = true);

    // Construtor
    TRestTrack();
    // Destructor
    ~TRestTrack();

    ClassDef(TRestTrack, 2);
};
#endif
