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

#ifndef RestCore_TRestGeant4Event
#define RestCore_TRestGeant4Event

#include <TGraph.h>
#include <TGraph2D.h>
#include <TH1D.h>
#include <TH2F.h>
#include <TLegend.h>
#include <TMultiGraph.h>
#include <TObject.h>
#include <TRestEvent.h>
#include <TRestGeant4Track.h>
#include <TVector3.h>

#include <iostream>
#include <map>

/// An event class to store geant4 generated event information
class TRestGeant4Event : public TRestEvent {
   private:
#ifndef __CINT__
    Double_t fMinX, fMaxX;            //!
    Double_t fMinY, fMaxY;            //!
    Double_t fMinZ, fMaxZ;            //!
    Double_t fMinEnergy, fMaxEnergy;  //!
#endif

    void AddEnergyDepositToVolume(Int_t volID, Double_t eDep);

    Bool_t PerProcessEnergyInitFlag = false;
    std::map<string, Double_t> PerProcessEnergyInSensitive;

    void inline InitializePerProcessEnergyInSensitive() {
        PerProcessEnergyInitFlag = true;
        PerProcessEnergyInSensitive["photoelectric"] = 0;
        PerProcessEnergyInSensitive["compton"] = 0;
        PerProcessEnergyInSensitive["electron_ionization"] = 0;
        PerProcessEnergyInSensitive["ion_ionization"] = 0;
        PerProcessEnergyInSensitive["alpha_ionization"] = 0;
        PerProcessEnergyInSensitive["msc"] = 0;
        PerProcessEnergyInSensitive["hadronic_ionization"] = 0;
        PerProcessEnergyInSensitive["proton_ionization"] = 0;
        PerProcessEnergyInSensitive["hadronic_elastic"] = 0;
        PerProcessEnergyInSensitive["neutron_elastic"] = 0;

        string volume_name;
        string process_name;
        TRestGeant4Track* track;
        TRestGeant4Hits* hits;
        Double_t energy;

        for (Int_t track_id = 0; track_id < GetNumberOfTracks(); track_id++) {
            track = GetTrack(track_id);

            if (track->GetEnergyInVolume(0) == 0) {
                continue;
            }

            hits = track->GetHits();

            for (Int_t hit_id = 0; hit_id < hits->GetNumberOfHits(); hit_id++) {
                if (hits->GetVolumeId(hit_id) != 0) {
                    continue;
                }

                process_name = (string)track->GetProcessName(hits->GetHitProcess(hit_id));
                energy = hits->GetEnergy(hit_id);
                if (process_name == "phot") {
                    PerProcessEnergyInSensitive["photoelectric"] += energy;
                } else if (process_name == "compt") {
                    PerProcessEnergyInSensitive["compton"] += energy;
                } else if (process_name == "eIoni" || process_name == "e-Step" || process_name == "e+Step") {
                    PerProcessEnergyInSensitive["electron_ionization"] += energy;
                } else if (process_name == "ionIoni") {
                    PerProcessEnergyInSensitive["ion_ionization"] += energy;
                    if (track->GetParticleName() == "alpha") {
                        PerProcessEnergyInSensitive["alpha_ionization"] += energy;
                    }
                } else if (process_name == "msc") {
                    PerProcessEnergyInSensitive["msc"] += energy;
                } else if (process_name == "hIoni") {
                    PerProcessEnergyInSensitive["hadronic_ionization"] += energy;
                    if (track->GetParticleName() == "proton") {
                        PerProcessEnergyInSensitive["proton_ionization"] += energy;
                    }
                } else if (process_name == "hadElastic") {
                    PerProcessEnergyInSensitive["hadronic_elastic"] += energy;
                    if (track->GetParticleName() == "neutron") {
                        PerProcessEnergyInSensitive["neutron_elastic"] += energy;
                    }
                } else if (process_name == "Transportation") {
                    if (track->GetParticleName() == "proton") {
                        PerProcessEnergyInSensitive["hadronic_ionization"] += energy;
                        PerProcessEnergyInSensitive["proton_ionization"] += energy;
                    } else if (track->GetParticleName() == "e-" || track->GetParticleName() == "e+") {
                        PerProcessEnergyInSensitive["electron_ionization"] += energy;
                    }
                }
            }
        }
    }

   protected:
#ifndef __CINT__

    // TODO These graphs should be placed in TRestTrack?
    // (following GetGraph implementation in TRestDetectorSignal)
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
    std::vector<string> fVolumeStoredNames;
    std::vector<Double_t> fVolumeDepositedEnergy;

    Int_t fNTracks;
    std::vector<TRestGeant4Track> fTrack;

    Int_t fMaxSubEventID;

   public:
    void SetBoundaries();
    void SetBoundaries(Double_t xMin, Double_t xMax, Double_t yMin, Double_t yMax, Double_t zMin,
                       Double_t zMax);

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
    TRestGeant4Track* GetTrack(int n) { return &fTrack[n]; }
    TRestGeant4Track* GetTrackByID(int id);
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

    Double_t GetEnergyInSensitiveFromProcessPhoto() {
        if (!PerProcessEnergyInitFlag) {
            InitializePerProcessEnergyInSensitive();
        }
        return PerProcessEnergyInSensitive["photoelectric"];
    }
    Double_t GetEnergyInSensitiveFromProcessCompton() {
        if (!PerProcessEnergyInitFlag) {
            InitializePerProcessEnergyInSensitive();
        }
        return PerProcessEnergyInSensitive["compton"];
    }
    Double_t GetEnergyInSensitiveFromProcessEIoni() {
        if (!PerProcessEnergyInitFlag) {
            InitializePerProcessEnergyInSensitive();
        }
        return PerProcessEnergyInSensitive["electron_ionization"];
    }
    Double_t GetEnergyInSensitiveFromProcessIonIoni() {
        if (!PerProcessEnergyInitFlag) {
            InitializePerProcessEnergyInSensitive();
        }
        return PerProcessEnergyInSensitive["ion_ionization"];
    }
    Double_t GetEnergyInSensitiveFromProcessAlphaIoni() {
        if (!PerProcessEnergyInitFlag) {
            InitializePerProcessEnergyInSensitive();
        }
        return PerProcessEnergyInSensitive["alpha_ionization"];
    }
    Double_t GetEnergyInSensitiveFromProcessMsc() {
        if (!PerProcessEnergyInitFlag) {
            InitializePerProcessEnergyInSensitive();
        }
        return PerProcessEnergyInSensitive["msc"];
    }
    Double_t GetEnergyInSensitiveFromProcessHadronIoni() {
        if (!PerProcessEnergyInitFlag) {
            InitializePerProcessEnergyInSensitive();
        }
        return PerProcessEnergyInSensitive["hadronic_ionization"];
    }
    Double_t GetEnergyInSensitiveFromProcessProtonIoni() {
        if (!PerProcessEnergyInitFlag) {
            InitializePerProcessEnergyInSensitive();
        }
        return PerProcessEnergyInSensitive["proton_ionization"];
    }
    Double_t GetEnergyInSensitiveFromProcessHadronElastic() {
        if (!PerProcessEnergyInitFlag) {
            InitializePerProcessEnergyInSensitive();
        }
        return PerProcessEnergyInSensitive["hadronic_elastic"];
    }
    Double_t GetEnergyInSensitiveFromProcessNeutronElastic() {
        if (!PerProcessEnergyInitFlag) {
            InitializePerProcessEnergyInSensitive();
        }
        return PerProcessEnergyInSensitive["neutron_elastic"];
    }

    void SetPrimaryEventOrigin(TVector3 pos) { fPrimaryEventOrigin = pos; }
    void SetPrimaryEventDirection(TVector3 dir) { fPrimaryEventDirection.push_back(dir); }
    void SetPrimaryEventParticleName(TString pName) { fPrimaryParticleName.push_back(pName); }
    void SetPrimaryEventEnergy(Double_t en) { fPrimaryEventEnergy.push_back(en); }
    void ActivateVolumeForStorage(Int_t n) { fVolumeStored[n] = 1; }
    void DisableVolumeForStorage(Int_t n) { fVolumeStored[n] = 0; }

    void AddActiveVolume(const string& volumeName);
    void ClearVolumes();
    void AddEnergyToSensitiveVolume(Double_t en) { fSensitiveVolumeEnergy += en; }

    void SetEnergyDepositedInVolume(Int_t volID, Double_t eDep) { fVolumeDepositedEnergy[volID] = eDep; }
    void SetSensitiveVolumeEnergy(Double_t en) { fSensitiveVolumeEnergy = en; }

    Int_t GetLowestTrackID() {
        Int_t lowestID = 0;
        if (fNTracks > 0) lowestID = GetTrack(0)->GetTrackID();

        for (int i = 0; i < fNTracks; i++) {
            TRestGeant4Track* tr = GetTrack(i);
            if (tr->GetTrackID() < lowestID) lowestID = tr->GetTrackID();
        }

        return lowestID;
    }

    void SetTrackSubEventID(Int_t n, Int_t id);
    void AddTrack(TRestGeant4Track trk);

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

    Bool_t isphotonNuclear() {
        for (int n = 0; n < GetNumberOfTracks(); n++)
            if (GetTrack(n)->isphotonNuclear()) return true;
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
    void PrintActiveVolumes();
    void PrintEvent(int maxTracks = 0, int maxHits = 0);

    TPad* DrawEvent(TString option = "") { return DrawEvent(option, true); }
    TPad* DrawEvent(TString option, Bool_t autoBoundaries);

    // Construtor
    TRestGeant4Event();
    // Destructor
    virtual ~TRestGeant4Event();

    ClassDef(TRestGeant4Event, 5);  // REST event superclass
};
#endif
