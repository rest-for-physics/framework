#include "SteppingAction.hh"

#include <TRestG4Event.h>
#include <TRestG4Metadata.h>
#include <TRestG4Track.h>

#include <G4SystemOfUnits.hh>

#include "G4DynamicParticle.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleTable.hh"
#include "G4SteppingManager.hh"
#include "G4UnitsTable.hh"
#include "globals.hh"

extern TRestG4Event* restG4Event;
extern TRestG4Metadata* restG4Metadata;
extern TRestG4Track* restTrack;
extern Int_t biasing;

//_____________________________________________________________________________
SteppingAction::SteppingAction() {
    if (biasing > 1) restBiasingVolume = restG4Metadata->GetBiasingVolume(biasing - 1);
}
//_____________________________________________________________________________
SteppingAction::~SteppingAction() {}
//_____________________________________________________________________________
void SteppingAction::UserSteppingAction(const G4Step* aStep) {
    // Variables that describe a step are taken.
    nom_vol = aStep->GetPreStepPoint()->GetPhysicalVolume()->GetName();
    nom_part = aStep->GetTrack()->GetDefinition()->GetParticleName();

    ener_dep = aStep->GetTotalEnergyDeposit();
    eKin = aStep->GetTrack()->GetKineticEnergy() / keV;

    if (restTrack->GetParticleName() == "geantino" &&
        (G4String)restG4Metadata->GetSensitiveVolume() == nom_vol) {
        restG4Metadata->SetSaveAllEvents(true);
    }

    if (!aStep->GetPostStepPoint()->GetProcessDefinedStep()) {
        G4cout << endl;
        G4cout << endl;
        G4cout << "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++" << endl;
        G4cout << "An ERROR was detected on the G4Step process definition." << endl;
        G4cout << "This is a sign of problem in the restG4 particle definition" << endl;
        G4cout << endl;
        G4cout << "E.g. A definition of a gamma with 0keV energy" << endl;
        G4cout << endl;
        G4cout << "Please, review your TRestG4Metadata RML definition" << endl;
        G4cout << "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++" << endl;
        G4cout << endl;
        G4cout << endl;

        exit(0);
    }

    nom_proc = aStep->GetPostStepPoint()->GetProcessDefinedStep()->GetProcessName();

    G4Track* aTrack = aStep->GetTrack();
    parentID = aTrack->GetParentID();
    trackID = aTrack->GetTrackID();

    Double_t x = aTrack->GetPosition().x() / mm;
    Double_t y = aTrack->GetPosition().y() / mm;
    Double_t z = aTrack->GetPosition().z() / mm;

    //   G4cout << "Step direction : " << aTrack->GetMomentumDirection() <<
    //   G4endl;

    if (biasing > 0) {
        // In biasing mode we do not store hits. Just check if we observe a gamma
        // inside the volume
        if (restBiasingVolume.isInside(x, y, z) && nom_part == "gamma") {
            Double_t eKinetic = aStep->GetPreStepPoint()->GetKineticEnergy() / keV;

            // we add the gamma energy to the energy spectrum
            if (eKinetic > restBiasingVolume.GetMinEnergy() && eKinetic < restBiasingVolume.GetMaxEnergy()) {
                G4ThreeVector position = aStep->GetPreStepPoint()->GetPosition();
                G4ThreeVector positionNorm = -aStep->GetPreStepPoint()->GetPosition().unit();
                G4ThreeVector momentum = aStep->GetPreStepPoint()->GetMomentumDirection();

                Double_t angle;
                if (restBiasingVolume.GetBiasingVolumeType() == "virtualBox") {
                    if (absDouble(position.x()) > absDouble(position.y()) &&
                        absDouble(position.x()) > absDouble(position.z())) {
                        // launching event from side x
                        if (position.x() > 0)
                            positionNorm.set(-1, 0, 0);
                        else
                            positionNorm.set(1, 0, 0);

                    } else if (absDouble(position.y()) > absDouble(position.x()) &&
                               absDouble(position.y()) > absDouble(position.z())) {
                        // launching event from side y
                        if (position.y() > 0)
                            positionNorm.set(0, -1, 0);
                        else
                            positionNorm.set(0, 1, 0);
                    } else {
                        // launching event from side y
                        if (position.z() > 0)
                            positionNorm.set(0, 0, -1);
                        else
                            positionNorm.set(0, 0, 1);
                    }
                }
                angle = momentum.angle(positionNorm);

                fBiasingSpectrum->Fill(eKinetic);
                fAngularDistribution->Fill(angle);

                if (restBiasingVolume.GetBiasingVolumeType() == "virtualSphere")
                    fSpatialDistribution->Fill(position.getPhi(), position.getTheta());
                else
                    fSpatialDistribution->Fill(position.x(), position.y());
            }

            // and we cancel the track
            //           aTrack->SetTrackStatus( fStopAndKill );
            aTrack->SetTrackStatus(fKillTrackAndSecondaries);

            // previousDirection = momentum;
        }

    } else {
        if ((G4String)restG4Metadata->GetSensitiveVolume() == nom_vol) {
            restG4Event->AddEnergyToSensitiveVolume(ener_dep / keV);
        }

        TVector3 hitPosition(x, y, z);
        Int_t pcsID = restTrack->GetProcessID(nom_proc);
        Double_t hit_global_time = aStep->GetPreStepPoint()->GetGlobalTime() / second;
        G4ThreeVector momentum = aStep->GetPreStepPoint()->GetMomentumDirection();
        TVector3 momentumDirection = TVector3(momentum.x(), momentum.y(), momentum.z());  //.Unit();

        Int_t volume = -1;
        Bool_t alreadyStored = false;
        // We check if the hit must be stored and keep it on restG4Track
        for (int volID = 0; volID < restG4Metadata->GetNumberOfActiveVolumes(); volID++) {
            if (restG4Event->isVolumeStored(volID)) {
                if (restG4Metadata->GetVerboseLevel() >= REST_Extreme)
                    G4cout << "Step volume :" << nom_vol << "::("
                           << (G4String)restG4Metadata->GetActiveVolumeName(volID) << ")" << G4endl;

                // We store the hit if we have activated in the config
                Bool_t isActiveVolume = (nom_vol == (G4String)restG4Metadata->GetActiveVolumeName(volID));

                if (isActiveVolume) {
                    volume = volID;
                    if (restG4Metadata->GetVerboseLevel() >= REST_Extreme) G4cout << "Storing hit" << G4endl;
                    restTrack->AddG4Hit(hitPosition, ener_dep / keV, hit_global_time, pcsID, volID, eKin,
                                        momentumDirection);
                    alreadyStored = true;
                }
            }
        }

        // See issue #65.
        // If the radiactive decay occurs in a non active volume then the id will be -1
        Bool_t isDecay = (nom_proc == (G4String) "RadioactiveDecay");
        if (!alreadyStored && isDecay)
            restTrack->AddG4Hit(hitPosition, ener_dep / keV, hit_global_time, pcsID, volume, eKin,
                                momentumDirection);
    }
}
//_____________________________________________________________________________
