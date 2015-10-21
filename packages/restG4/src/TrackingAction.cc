//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
/// \file radioactivedecay/rdecay01/src/TrackingAction.cc
/// \brief Implementation of the TrackingAction class
//
//
// $Id: TrackingAction.cc 71485 2013-06-17 08:14:54Z gcosmo $
// 
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "TrackingAction.hh"
#include "RunAction.hh"
#include "EventAction.hh"

#include "G4Track.hh"
#include "G4ParticleTypes.hh"
#include "G4RunManager.hh"
#include "G4SystemOfUnits.hh"
#include "G4UnitsTable.hh"


extern TRestG4Metadata *restG4Metadata;
extern TRestG4Event *restG4Event;
extern TRestG4Track *restTrack;

//#include "OutputDictionary.hh"

G4double prevTime = 0;
G4String aux;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

TrackingAction::TrackingAction(RunAction* RA, EventAction* EA)
:G4UserTrackingAction(),
 fRun(RA),fEvent(EA)
 
{
    fFullChain = false;

    fFullChain = restG4Metadata->isFullChainActivated();

    if( fFullChain ) G4cout << "Full chain is active" << G4endl;
    else G4cout << "Full chain is NOT active" << G4endl;

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

TrackingAction::~TrackingAction()
{
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void TrackingAction::PreUserTrackingAction(const G4Track* track)
{

    if( restG4Metadata->GetVerboseLevel() >= REST_Debug )G4cout << "Track Starts" << endl;
    G4ParticleDefinition* particle = track->GetDefinition();
    G4String name   = particle->GetParticleName();
    fCharge = particle->GetPDGCharge();

    restTrack->RemoveHits();

    restTrack->SetTrackID ( track->GetTrackID() );
    restTrack->SetParentID ( track->GetParentID() );
    restTrack->SetKineticEnergy( track->GetKineticEnergy()/keV );
    restTrack->SetParticleName( name );
    restTrack->SetGlobalTrackTime( track->GetGlobalTime()/second );

    G4ThreeVector trkOrigin = track->GetPosition( );
    restTrack->SetTrackOrigin( trkOrigin.x(), trkOrigin.y(), trkOrigin.z() );


    // We finish after the de-excitation of the resulting nucleus (we skip the full chain, just first decay)
    // On future we must add an option through TRestG4Metadata to store a given number of decays

    //   if( fFullChain == true ) G4cout << "Full chain active" << G4endl;
    //   else  G4cout << "Full chain not active" << G4endl;

    Int_t ID = track->GetTrackID();
    if( fFullChain == false && fCharge > 2 && ID > 1 && !name.contains("[") )
    {
        G4Track* tr = (G4Track*) track;
        tr->SetTrackStatus(fStopAndKill);
    }

    /*
    if ( fFullChain == true && fCharge > 2  && ID > 1 && !name.contains("["))
    {
        restTrack->IncreaseSubEventID();
    }
    */

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void TrackingAction::PostUserTrackingAction(const G4Track* track)
{
    if( restG4Metadata->GetVerboseLevel() >= REST_Debug ) G4cout << "Track Ends" << endl;

    restTrack->SetTrackTimeLength( track->GetLocalTime()/microsecond );

 //   G4cout << "Storing track : Number of hits : " << restTrack->GetNumberOfHits() << G4endl;

    restG4Event->AddTrack( *restTrack );
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......


