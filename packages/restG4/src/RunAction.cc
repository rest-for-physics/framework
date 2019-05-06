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
/// \file radioactivedecay/rdecay01/src/RunAction.cc
/// \brief Implementation of the RunAction class
//
//
// $Id: RunAction.cc 68030 2013-03-13 13:51:27Z gcosmo $
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "RunAction.hh"
#include "PrimaryGeneratorAction.hh"

#include <iomanip>
#include "G4PhysicalConstants.hh"
#include "G4Run.hh"
#include "G4RunManager.hh"
#include "G4SystemOfUnits.hh"
#include "G4UnitsTable.hh"

#include <TRestG4Metadata.h>

extern TRestG4Metadata* restG4Metadata;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

RunAction::RunAction(PrimaryGeneratorAction* gen)
    : G4UserRunAction(), fPrimary(gen) {}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

RunAction::~RunAction() {}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void RunAction::BeginOfRunAction(const G4Run*) {
  // inform the runManager to save random number seed
  G4RunManager::GetRunManager()->SetRandomNumberStore(false);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void RunAction::EndOfRunAction(const G4Run* run) {
  G4int nbEvents = run->GetNumberOfEvent();
  if (nbEvents == 0) {
    return;
  }

  G4ParticleDefinition* particle =
      fPrimary->GetParticleGun()->GetParticleDefinition();
  G4String partName = particle->GetParticleName();
  // G4double eprimary = fPrimary->GetParticleGun()->GetParticleEnergy();

  G4cout << "======================== run summary ======================";
  G4cout << "\n" << nbEvents << " Events simulated\n";
  G4cout << "===========================================================";
  G4cout << G4endl;

  // restG4Metadata->PrintMetadata();

  /*

  G4int prec = 4, wid = prec + 2;
  G4int dfprec = G4cout.precision(prec);

  //particle count
  //
  G4cout << " Nb of generated particles: \n" << G4endl;

  std::map<G4String,G4int>::iterator it;
  for (it = fParticleCount.begin(); it != fParticleCount.end(); it++) {
      G4String name = it->first;
      G4int count   = it->second;
      G4double eMean = fEmean[name]/count;
      G4double eMin = fEmin[name], eMax = fEmax[name];

      G4cout << "  " << std::setw(13) << name << ": " << std::setw(7) << count
          << "  Emean = " << std::setw(wid) << G4BestUnit(eMean, "Energy")
          << "\t( "  << G4BestUnit(eMin, "Energy")
          << " --> " << G4BestUnit(eMax, "Energy")
          << ")" << G4endl;
  }

  //energy momentum balance
  //

  if (fDecayCount > 0) {
      G4double Ebmean = fEkinTot[0]/fDecayCount;
      G4double Pbmean = fPbalance[0]/fDecayCount;

      G4cout << "\n   Ekin Total (Q): mean = "
          << std::setw(wid) << G4BestUnit(Ebmean, "Energy")
          << "\t( "  << G4BestUnit(fEkinTot[1], "Energy")
          << " --> " << G4BestUnit(fEkinTot[2], "Energy")
          << ")" << G4endl;

      G4cout << "\n   Momentum balance (excluding gamma desexcitation): mean = "
          << std::setw(wid) << G4BestUnit(Pbmean, "Energy")
          << "\t( "  << G4BestUnit(fPbalance[1], "Energy")
          << " --> " << G4BestUnit(fPbalance[2], "Energy")
          << ")" << G4endl;
  }

  // remove all contents in fParticleCount
  //
  fParticleCount.clear();
  fEmean.clear();  fEmin.clear(); fEmax.clear();

  // restore default precision
  //
  G4cout.precision(dfprec);
  */
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
