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
/// \file radioactivedecay/rdecay01/src/PhysicsList.cc
/// \brief Implementation of the PhysicsList class
//
//
// $Id: PhysicsList.cc 73284 2013-08-23 08:35:02Z gcosmo $
// 
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo...... 

#include "PhysicsList.hh"
#include "G4UnitsTable.hh"
#include "G4ParticleTypes.hh"
#include "G4RadioactiveDecay.hh"
//#include "G4ScreenedNuclearRecoil.hh"
#include "G4UAtomicDeexcitation.hh"
#include "G4LossTableManager.hh"
#include "G4SystemOfUnits.hh"
#include "CLHEP/Units/PhysicalConstants.h"

#include "G4EmLivermorePhysics.hh"
#include "G4EmPenelopePhysics.hh"
#include "G4EmStandardPhysics_option3.hh"

//#include "PhysListEmStandard.hh"
//#include "PhysListEmStandardSS.hh"
//#include "PhysListEmStandardNR.hh"

#include "G4RegionStore.hh"
#include "G4Region.hh"
#include "G4ProductionCuts.hh"
#include "G4ProcessManager.hh"
#include "G4ParticleTypes.hh"
#include "G4ParticleTable.hh"

#include "G4EmExtraPhysics.hh"
#include "G4HadronElasticPhysics.hh"
#include "G4HadronElasticPhysicsHP.hh"
#include "G4StoppingPhysics.hh"
#include "G4IonBinaryCascadePhysics.hh"
#include "G4RadioactiveDecayPhysics.hh"
#include "G4NeutronTrackingCut.hh"
#include "G4DecayPhysics.hh"
#include "G4HadronPhysicsQGSP_BIC_HP.hh"
#include "G4BraggIonGasModel.hh"
#include "G4BetheBlochIonGasModel.hh"
#include "G4IonFluctuations.hh"
#include "G4IonParametrisedLossModel.hh"
#include "G4UniversalFluctuation.hh"

#include <G4StepLimiter.hh>


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PhysicsList::PhysicsList() : G4VModularPhysicsList(),
    fCutForGamma( 10.*um), fCutForElectron(1*mm), fCutForPositron(1*mm)
{
    //add new units for radioActive decays
    // 
    const G4double minute = 60*second;
    const G4double hour   = 60*minute;
    const G4double day    = 24*hour;
    const G4double year   = 365*day;
    new G4UnitDefinition("minute", "min", "Time", minute);
    new G4UnitDefinition("hour",   "h",   "Time", hour);
    new G4UnitDefinition("day",    "d",   "Time", day);
    new G4UnitDefinition("year",   "y",   "Time", year);        

    //SetVerboseLevel(0);

    defaultCutValue = 0.1 * mm;

    InitializePhysicsLists();

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PhysicsList::~PhysicsList()
{
    delete fEmPhysicsList;

    delete fDecPhysicsList;
    delete fRadDecPhysicsList;
    for( size_t i = 0; i < fHadronPhys.size(); i++) {delete fHadronPhys[i];}

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void PhysicsList::InitializePhysicsLists()
{

    // Decay physics and all particles
    fDecPhysicsList = new G4DecayPhysics();

    // RadioactiveDecay physicsList
    fRadDecPhysicsList = new G4RadioactiveDecayPhysics();

    // Electromagnetic physicsList
    fEmPhysicsList = new G4EmLivermorePhysics();
    // TODO: This should be added as an option through metadata
    // fEmPhysicsList = new G4EmPenelopePhysics();
    // fEmPhysicsList = new G4EmStandardPhysics_option3();
    // fEmPhysicsList = new PhysListEmStandardNR();

    //Hadronic PhysicsList
    fHadronPhys.push_back( new G4HadronElasticPhysicsHP());
    fHadronPhys.push_back(new G4IonBinaryCascadePhysics());
    fHadronPhys.push_back(new G4HadronPhysicsQGSP_BIC_HP());
    fHadronPhys.push_back(new G4NeutronTrackingCut());
    fHadronPhys.push_back(new G4EmExtraPhysics());

    G4cout << "Number of hadronic physics lists added "<< fHadronPhys.size() << G4endl;

}
//%%%%%%%%%%%%%%%%%%%%%%%%%%%
void PhysicsList::ConstructParticle()
{
    // pseudo-particles
    G4Geantino::GeantinoDefinition();

    //particles defined in PhysicsLists
    fDecPhysicsList->ConstructParticle();
    fEmPhysicsList->ConstructParticle();
    fRadDecPhysicsList->ConstructParticle();

    for( size_t i = 0; i < fHadronPhys.size(); i++) 
        fHadronPhys[i]->ConstructParticle();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void PhysicsList::ConstructProcess()
{
    AddTransportation();
    // electromagnetic physics list
    //
    fEmPhysicsList->ConstructProcess();
    em_config.AddModels();

    // decay physics list

    fDecPhysicsList->ConstructProcess();
    // Radioactive decay 
    fRadDecPhysicsList->ConstructProcess();
    // hadronic physics lists
    for( size_t i = 0; i < fHadronPhys.size(); i++) 
        fHadronPhys[i]->ConstructProcess();

    G4RadioactiveDecay* radioactiveDecay = new G4RadioactiveDecay();
    radioactiveDecay->SetHLThreshold(nanosecond);
    radioactiveDecay->SetICM(true);                //Internal Conversion

    // When there is electronic capture X-rays cab be emitted
    radioactiveDecay->SetARM(true);               //Atomic Rearangement

    /*
       G4ScreenedNuclearRecoil* nucr = new G4ScreenedNuclearRecoil();
       G4double energyLimit = 100.*MeV;
       nucr->SetMaxEnergyForScattering(energyLimit);
       ph->RegisterProcess( nucr, G4GenericIon::GenericIon());
       */

    /* theParticleIterator->reset();
    while ((*theParticleIterator)())
    {
        G4ParticleDefinition* particle = theParticleIterator->value();
        G4String partname = particle->GetParticleName();
        if(partname == "alpha" || partname == "He3" || partname == "GenericIon") {
            G4BraggIonGasModel* mod1 = new G4BraggIonGasModel();
            G4BetheBlochIonGasModel* mod2 = new G4BetheBlochIonGasModel();
            G4double eth = 2.*MeV*particle->GetPDGMass()/CLHEP::proton_mass_c2;
            em_config.SetExtraEmModel(partname,"braggIoni",mod1,"",0.0,eth,
                    new G4IonFluctuations());
            em_config.SetExtraEmModel(partname,"betheIoni",mod2,"",eth,100*TeV,
                    new G4UniversalFluctuation());

        }
    } */

    theParticleIterator->reset();
    while((*theParticleIterator)()) 
    {
        G4ParticleDefinition* particle = theParticleIterator->value();
        G4String partname = particle->GetParticleName();
        G4ProcessManager* processManager = particle->GetProcessManager();

        if(partname =="e-") processManager->AddDiscreteProcess(new G4StepLimiter("e-Step")); 
        else if(partname =="e+") processManager->AddDiscreteProcess(new G4StepLimiter("e+Step")); 
    }

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void PhysicsList::SetCuts()
{
    SetCutsWithDefault();

    SetCutValue( fCutForGamma, "gamma" );
    SetCutValue( fCutForElectron, "e-" );
    SetCutValue( fCutForPositron, "e+" );
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
