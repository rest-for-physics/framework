/*************************************************************************
 * This file is part of the REST software framework.                     *
 *                                                                       *
 * Copyright (C) 2016 GIFNA/TREX (University of Zaragoza)                *
 * For more information see http://gifna.unizar.es/trex                  *
 *                                                                       *
 * REST is free software: you can redistribute it and/or modify          *
 * it under the terms of the GNU General Public License as published by  *
 * the Free Software Foundation, either version 3 of the License, or     *
 * (at your option) any later version.                                   *
 *                                                                       *
 * REST is distributed in the hope that it will be useful,               *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the          *
 * GNU General Public License for more details.                          *
 *                                                                       *
 * You should have a copy of the GNU General Public License along with   *
 * REST in $REST_PATH/LICENSE.                                           *
 * If not, see http://www.gnu.org/licenses/.                             *
 * For the list of contributors see $REST_PATH/CREDITS.                  *
 *************************************************************************/

#ifndef RestCore_TRestG4Event
#define RestCore_TRestG4Event

#include <iostream>

#include <TGraph.h>
#include <TGraph2D.h>
#include <TH1D.h>
#include <TH2F.h>
#include <TLegend.h>
#include <TMultiGraph.h>
#include <TObject.h>
#include <TRestEvent.h>
#include <TRestG4Track.h>
#include <TVector3.h>

/// An event class to store geant4 generated event information
class TRestG4Event : public TRestEvent {
   private:
#ifndef __CINT__
    Double_t fMinX, fMaxX;            //!
    Double_t fMinY, fMaxY;            //!
    Double_t fMinZ, fMaxZ;            //!
    Double_t fMinEnergy, fMaxEnergy;  //!
#endif

    void SetBoundaries();

    void AddEnergyDepositToVolume(Int_t volID, Double_t eDep);

   protected:
#ifndef __CINT__

    // TODO These graphs should be placed in TRestTrack?
    // (following GetGraph implementation in TRestSignal)
    TGraph* fXZHitGraph;  //!
    TGraph* fYZHitGraph;  //!
    TGraph* fXYHitGraph;  //!
    // TGraph2D *fXYZHitGraph; //! (TODO to implement XYZ visualization)

    std::vector<TGraph*> fXYPcsMarker;  //!
    std::vector<TGraph*> fYZPcsMarker;  //!
    std::vector<TGraph*> fXZPcsMarker;  //!

    TMultiGraph* fXZMultiGraph;  //!
    TMultiGraph* fYZMultiGraph;  //!
    TMultiGraph* fXYMultiGraph;  //!
    // TMultiGraph *fXYZMultiGraph; //! (TODO to implement XYZ visualization)

    TH2F* fXYHisto;  //!
    TH2F* fXZHisto;  //!
    TH2F* fYZHisto;  //!

    TH1D* fXHisto;  //!
    TH1D* fYHisto;  //!
    TH1D* fZHisto;  //!

    TLegend* fLegend_XY;  //!
    TLegend* fLegend_XZ;  //!
    TLegend* fLegend_YZ;  //!

    std::vector<Int_t> legendAdded;  //!

    Int_t fTotalHits;  //!

    TMultiGraph* GetXZMultiGraph(Int_t gridElement, std::vector<TString> pcsList, Double_t minPointSize = 0.4,
                                 Double_t maxPointSize = 4);
    TMultiGraph* GetYZMultiGraph(Int_t gridElement, std::vector<TString> pcsList, Double_t minPointSize = 0.4,
                                 Double_t maxPointSize = 4);
    TMultiGraph* GetXYMultiGraph(Int_t gridElement, std::vector<TString> pcsList, Double_t minPointSize = 0.4,
                                 Double_t maxPointSize = 4);

    TH2F* GetXYHistogram(Int_t gridElement, std::vector<TString> optList);
    TH2F* GetXZHistogram(Int_t gridElement, std::vector<TString> optList);
    TH2F* GetYZHistogram(Int_t gridElement, std::vector<TString> optList);

    TH1D* GetXHistogram(Int_t gridElement, std::vector<TString> optList);
    TH1D* GetYHistogram(Int_t gridElement, std::vector<TString> optList);
    TH1D* GetZHistogram(Int_t gridElement, std::vector<TString> optList);
#endif

    TVector3 fPrimaryEventOrigin;

    std::vector<TString> fPrimaryParticleName;
    std::vector<TVector3> fPrimaryEventDirection;
    std::vector<Double_t> fPrimaryEventEnergy;

    Double_t fTotalDepositedEnergy;
    Double_t fSensitiveVolumeEnergy;

    Int_t fNVolumes;
    std::vector<Int_t> fVolumeStored;
    std::vector<Double_t> fVolumeDepositedEnergy;

    Int_t fNTracks;
    std::vector<TRestG4Track> fTrack;

    Int_t fMaxSubEventID;

   public:
    TString GetPrimaryEventParticleName(int n) {
        if (fPrimaryParticleName.size() > n) return fPrimaryParticleName[n];
        return "Not defined";
    }

    TVector3 GetPrimaryEventDirection(int n) { return fPrimaryEventDirection[n]; }
    TVector3 GetPrimaryEventOrigin() { return fPrimaryEventOrigin; }
    Double_t GetPrimaryEventEnergy(int n) { return fPrimaryEventEnergy[n]; }

    Int_t GetNumberOfHits();
    Int_t GetNumberOfTracks() { return fNTracks; }
    Int_t GetNumberOfPrimaries() { return fPrimaryEventDirection.size(); }
    Int_t GetNumberOfActiveVolumes() { return fNVolumes; }

    Int_t isVolumeStored(int n) { return fVolumeStored[n]; }
    TRestG4Track* GetTrack(int n) { return &fTrack[n]; }
    TRestG4Track* GetTrackByID(int id);
    Int_t GetNumberOfSubEventIDTracks() { return fMaxSubEventID + 1; }

    Double_t GetTotalDepositedEnergy() { return fTotalDepositedEnergy; }
    Double_t GetTotalDepositedEnergyFromTracks();
    Double_t GetEnergyDepositedInVolume(Int_t volID) { return fVolumeDepositedEnergy[volID]; }
    Double_t GetSensitiveVolumeEnergy() { return fSensitiveVolumeEnergy; }
    TVector3 GetMeanPositionInVolume(Int_t volID);
    TVector3 GetFirstPositionInVolume(Int_t volID);
    TVector3 GetLastPositionInVolume(Int_t volID);

    TRestHits GetHits();

    Int_t GetNumberOfTracksForParticle(TString parName);
    Int_t GetEnergyDepositedByParticle(TString parName);

    void SetPrimaryEventOrigin(TVector3 pos) { fPrimaryEventOrigin = pos; }
    void SetPrimaryEventDirection(TVector3 dir) { fPrimaryEventDirection.push_back(dir); }
    void SetPrimaryEventParticleName(TString pName) { fPrimaryParticleName.push_back(pName); }
    void SetPrimaryEventEnergy(Double_t en) { fPrimaryEventEnergy.push_back(en); }
    void ActivateVolumeForStorage(Int_t n) { fVolumeStored[n] = 1; }
    void DisableVolumeForStorage(Int_t n) { fVolumeStored[n] = 0; }

    void AddActiveVolume();
    void ClearVolumes();
    void AddEnergyToSensitiveVolume(Double_t en) { fSensitiveVolumeEnergy += en; }

    void SetEnergyDepositedInVolume(Int_t volID, Double_t eDep) { fVolumeDepositedEnergy[volID] = eDep; }
    void SetSensitiveVolumeEnergy(Double_t en) { fSensitiveVolumeEnergy = en; }

    Int_t GetLowestTrackID() {
        Int_t lowestID = 0;
        if (fNTracks > 0) lowestID = GetTrack(0)->GetTrackID();

        for (int i = 0; i < fNTracks; i++) {
            TRestG4Track* tr = GetTrack(i);
            if (tr->GetTrackID() < lowestID) lowestID = tr->GetTrackID();
        }

        return lowestID;
    }

    void SetTrackSubEventID(Int_t n, Int_t id);
    void AddTrack(TRestG4Track trk);

    Bool_t isRadiactiveDecay() {
        for (int n = 0; n < GetNumberOfTracks(); n++)
            if (GetTrack(n)->isRadiactiveDecay()) return true;
        return false;
    }

    Bool_t isPhotoElectric() {
        for (int n = 0; n < GetNumberOfTracks(); n++)
            if (GetTrack(n)->isPhotoElectric()) return true;
        return false;
    }
    Bool_t isCompton() {
        for (int n = 0; n < GetNumberOfTracks(); n++)
            if (GetTrack(n)->isCompton()) return true;
        return false;
    }
    Bool_t isBremstralung() {
        for (int n = 0; n < GetNumberOfTracks(); n++)
            if (GetTrack(n)->isBremstralung()) return true;
        return false;
    }

    Bool_t ishadElastic() {
        for (int n = 0; n < GetNumberOfTracks(); n++)
            if (GetTrack(n)->ishadElastic()) return true;
        return false;
    }
    Bool_t isneutronInelastic() {
        for (int n = 0; n < GetNumberOfTracks(); n++)
            if (GetTrack(n)->isneutronInelastic()) return true;
        return false;
    }

    Bool_t isnCapture() {
        for (int n = 0; n < GetNumberOfTracks(); n++)
            if (GetTrack(n)->isnCapture()) return true;
        return false;
    }

    Bool_t ishIoni() {
        for (int n = 0; n < GetNumberOfTracks(); n++)
            if (GetTrack(n)->ishIoni()) return true;
        return false;
    }

    Bool_t isAlpha() {
        for (int n = 0; n < GetNumberOfTracks(); n++)
            if (GetTrack(n)->GetParticleName() == "alpha") return true;
        return false;
    }

    Bool_t isNeutron() {
        for (int n = 0; n < GetNumberOfTracks(); n++)
            if (GetTrack(n)->GetParticleName() == "neutron") return true;
        return false;
    }

    Bool_t isArgon() {
        for (int n = 0; n < GetNumberOfTracks(); n++)
            if ((GetTrack(n)->GetParticleName()).Contains("Ar")) return true;
        return false;
    }

    Bool_t isXenon() {
        for (int n = 0; n < GetNumberOfTracks(); n++)
            if ((GetTrack(n)->GetParticleName()).Contains("Xe")) return true;
        return false;
    }

    Bool_t isNeon() {
        for (int n = 0; n < GetNumberOfTracks(); n++)
            if ((GetTrack(n)->GetParticleName()).Contains("Ne")) return true;
        return false;
    }
    /// Processes and particles in a given volume

    Bool_t isRadiactiveDecayInVolume(Int_t volID) {
        for (int n = 0; n < GetNumberOfTracks(); n++)
            if (GetTrack(n)->isRadiactiveDecayInVolume(volID)) return true;
        return false;
    }

    Bool_t isPhotoElectricInVolume(Int_t volID) {
        for (int n = 0; n < GetNumberOfTracks(); n++)
            if (GetTrack(n)->isPhotoElectricInVolume(volID)) return true;
        return false;
    }
    Bool_t isPhotonNuclearInVolume(Int_t volID) {
        for (int n = 0; n < GetNumberOfTracks(); n++)
            if (GetTrack(n)->isPhotonNuclearInVolume(volID)) return true;
        return false;
    }
    Bool_t isComptonInVolume(Int_t volID) {
        for (int n = 0; n < GetNumberOfTracks(); n++)
            if (GetTrack(n)->isComptonInVolume(volID)) return true;
        return false;
    }
    Bool_t isBremstralungInVolume(Int_t volID) {
        for (int n = 0; n < GetNumberOfTracks(); n++)
            if (GetTrack(n)->isBremstralungInVolume(volID)) return true;
        return false;
    }

    Bool_t isHadElasticInVolume(Int_t volID) {
        for (int n = 0; n < GetNumberOfTracks(); n++)
            if (GetTrack(n)->isHadElasticInVolume(volID)) return true;
        return false;
    }
    Bool_t isNeutronInelasticInVolume(Int_t volID) {
        for (int n = 0; n < GetNumberOfTracks(); n++)
            if (GetTrack(n)->isNeutronInelasticInVolume(volID)) return true;
        return false;
    }

    Bool_t isNCaptureInVolume(Int_t volID) {
        for (int n = 0; n < GetNumberOfTracks(); n++)
            if (GetTrack(n)->isNCaptureInVolume(volID)) return true;
        return false;
    }

    Bool_t ishIoniInVolume(Int_t volID) {
        for (int n = 0; n < GetNumberOfTracks(); n++)
            if (GetTrack(n)->isHIoniInVolume(volID)) return true;
        return false;
    }

    Bool_t isAlphaInVolume(Int_t volID) {
        for (int n = 0; n < GetNumberOfTracks(); n++)
            if (GetTrack(n)->isAlphaInVolume(volID)) return true;
        return false;
    }

    Bool_t isNeutronInVolume(Int_t volID) {
        for (int n = 0; n < GetNumberOfTracks(); n++)
            if (GetTrack(n)->isNeutronInVolume(volID)) return true;
        return false;
    }

    Bool_t isArgonInVolume(Int_t volID) {
        for (int n = 0; n < GetNumberOfTracks(); n++)
            if (GetTrack(n)->isArgonInVolume(volID)) return true;
        return false;
    }

    Bool_t isXenonInVolume(Int_t volID) {
        for (int n = 0; n < GetNumberOfTracks(); n++)
            if (GetTrack(n)->isXenonInVolume(volID)) return true;
        return false;
    }

    Bool_t isNeonInVolume(Int_t volID) {
        for (int n = 0; n < GetNumberOfTracks(); n++)
            if (GetTrack(n)->isNeonInVolume(volID)) return true;
        return false;
    }

    void Initialize();

    /// maxTracks : number of tracks to print, 0 = all
    void PrintEvent(int maxTracks = 0, int maxHits = 0);

    TPad* DrawEvent(TString option = "");

    // Construtor
    TRestG4Event();
    // Destructor
    virtual ~TRestG4Event();

    ClassDef(TRestG4Event, 2);  // REST event superclass
};
#endif
