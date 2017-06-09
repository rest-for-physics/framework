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
#include "Particles.hh"
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

Int_t emCounter = 0;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PhysicsList::PhysicsList() : G4VModularPhysicsList()
{
    cout << "restG4. PhysicsList. Wrong constructor!!" << endl;
}

PhysicsList::PhysicsList( TRestPhysicsLists *physicsLists ) : G4VModularPhysicsList()
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

    defaultCutValue = 0.1 * mm;

    restPhysList = physicsLists;

    fEmPhysicsList = NULL;
    fDecPhysicsList = NULL;
    fRadDecPhysicsList = NULL;

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
    if( restPhysList->FindPhysicsList( "G4DecayPhysics" ) >= 0 )
        fDecPhysicsList = new G4DecayPhysics();
    else if ( restPhysList->GetVerboseLevel() >= REST_Debug )
    {
        G4cout << "restG4. PhysicsList. G4DecayPhysics is not enabled!!" << G4endl;
    }

    // RadioactiveDecay physicsList
    if( restPhysList->FindPhysicsList( "G4RadioactiveDecayPhysics" ) >= 0 )
        fRadDecPhysicsList = new G4RadioactiveDecayPhysics();
    else if ( restPhysList->GetVerboseLevel() >= REST_Debug )
    {
        G4cout << "restG4. PhysicsList. G4RadioactiveDecayPhysics is not enabled!!" << G4endl;
    }

    // Electromagnetic physicsList
    if( restPhysList->FindPhysicsList( "G4EmLivermorePhysics" ) >= 0 )
    {
        if( fEmPhysicsList == NULL ) fEmPhysicsList = new G4EmLivermorePhysics();
        emCounter++;
    }

    if( restPhysList->FindPhysicsList( "G4EmPenelopePhysics" ) >= 0 )
    {
        if( fEmPhysicsList == NULL ) fEmPhysicsList = new G4EmPenelopePhysics();
        emCounter++;
    }

    if( restPhysList->FindPhysicsList( "G4EmStandardPhysics_option3" ) >= 0 )
    {
        if( fEmPhysicsList == NULL ) fEmPhysicsList = new G4EmStandardPhysics_option3();
        emCounter++;
    }

    if( restPhysList->GetVerboseLevel() >= REST_Warning && emCounter == 0 )
    {
        G4cout << "REST WARNING : No electromagenetic physics list has been enabled!!" << G4endl;
    }

    if( emCounter > 1 )
    {
        G4cout << "REST ERROR: restG4. PhysicsList. More than 1 EM PhysicsList enabled." << G4endl;
        exit(1);
    }

    //Hadronic PhysicsList
    if( restPhysList->FindPhysicsList( "G4HadronPhysicsQGSP_BIC_HP" ) >= 0 )
        fHadronPhys.push_back( new G4HadronElasticPhysicsHP() );

    if( restPhysList->FindPhysicsList( "G4IonBinaryCascadePhysics" ) >= 0 )
        fHadronPhys.push_back( new G4IonBinaryCascadePhysics() );

    if( restPhysList->FindPhysicsList( "G4HadronElasticPhysicsHP" ) >= 0 )
        fHadronPhys.push_back( new G4HadronPhysicsQGSP_BIC_HP() );

    if( restPhysList->FindPhysicsList( "G4NeutronTrackingCut" ) >= 0 )
        fHadronPhys.push_back( new G4NeutronTrackingCut() );

    if( restPhysList->FindPhysicsList( "G4EmExtraPhysics" ) >= 0 )
        fHadronPhys.push_back( new G4EmExtraPhysics() );

    G4cout << "Number of hadronic physics lists added "<< fHadronPhys.size() << G4endl;

}
//%%%%%%%%%%%%%%%%%%%%%%%%%%%
void PhysicsList::ConstructParticle()
{
    // pseudo-particles
    G4Geantino::GeantinoDefinition();

    //particles defined in PhysicsLists
    if( fDecPhysicsList != NULL )
        fDecPhysicsList->ConstructParticle();
    
    if( fEmPhysicsList != NULL )
        fEmPhysicsList->ConstructParticle();

    if( fRadDecPhysicsList != NULL )
        fRadDecPhysicsList->ConstructParticle();

    for( size_t i = 0; i < fHadronPhys.size(); i++) 
        fHadronPhys[i]->ConstructParticle();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void PhysicsList::ConstructProcess()
{
    AddTransportation();

    // Electromagnetic physics list
    if( fEmPhysicsList != NULL )
    {
        fEmPhysicsList->ConstructProcess();
        em_config.AddModels();
    }

    // Decay physics list
    if( fDecPhysicsList != NULL )
        fDecPhysicsList->ConstructProcess();

    // Radioactive decay 
    if( fRadDecPhysicsList != NULL )
        fRadDecPhysicsList->ConstructProcess();
    
    // hadronic physics lists
    for( size_t i = 0; i < fHadronPhys.size(); i++) 
        fHadronPhys[i]->ConstructProcess();

    if( restPhysList->FindPhysicsList( "G4RadioactiveDecay" ) )
    {
        G4RadioactiveDecay* radioactiveDecay = new G4RadioactiveDecay();

        radioactiveDecay->SetHLThreshold(nanosecond);

        // Setting Internal Conversion (ICM) option.
        if( restPhysList->GetPhysicsListOptionValue( "G4RadioactiveDecay", "ICM" ) == "true" )
            radioactiveDecay->SetICM(true);                //Internal Conversion
        else if( restPhysList->GetPhysicsListOptionValue( "G4RadioactiveDecay", "ICM" ) == "false" )
            radioactiveDecay->SetICM(false);                //Internal Conversion
        else if( restPhysList->GetVerboseLevel( ) >= REST_Warning )
                G4cout << "REST WARNING. restG4. PhysicsList. G4RadioactiveDecay. Option ICM not defined." << G4endl;

        // Enabling electron re-arrangment (ARM) option.
        if( restPhysList->GetPhysicsListOptionValue( "G4RadioactiveDecay", "ARM" ) == "true" )
            radioactiveDecay->SetARM(true);                //Internal Conversion
        else if( restPhysList->GetPhysicsListOptionValue( "G4RadioactiveDecay", "ARM" ) == "false" )
            radioactiveDecay->SetARM(false);                //Internal Conversion
        else if( restPhysList->GetVerboseLevel( ) >= REST_Warning )
                G4cout << "REST WARNING. restG4. PhysicsList. G4RadioactiveDecay. Option ARM not defined." << G4endl;
    }

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

    SetCutValue( restPhysList->GetCutForGamma() * mm, "gamma" );
    SetCutValue( restPhysList->GetCutForElectron() * mm, "e-" );
    SetCutValue( restPhysList->GetCutForPositron() * mm, "e+" );
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
