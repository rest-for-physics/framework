
#include "Particles.hh"
#include "G4BaryonConstructor.hh"
#include "G4BosonConstructor.hh"
#include "G4IonConstructor.hh"
#include "G4LeptonConstructor.hh"
#include "G4MesonConstructor.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleTypes.hh"
#include "G4ShortLivedConstructor.hh"
#include "G4SystemOfUnits.hh"

Particles::Particles(const G4String& name) : G4VPhysicsConstructor(name) {}

Particles::~Particles() {}

void Particles::ConstructParticle() {
    G4Gamma::GammaDefinition();

    // *******//
    // leptons//
    // *******//

    G4Electron::ElectronDefinition();
    G4Positron::PositronDefinition();
    G4MuonPlus::MuonPlusDefinition();
    G4MuonMinus::MuonMinusDefinition();
    G4NeutrinoE::NeutrinoEDefinition();
    G4AntiNeutrinoE::AntiNeutrinoEDefinition();
    G4NeutrinoMu::NeutrinoMuDefinition();
    G4AntiNeutrinoMu::AntiNeutrinoMuDefinition();

    // ********//
    //  mesons //
    // ********//
    G4PionPlus::PionPlusDefinition();
    G4PionMinus::PionMinusDefinition();
    G4PionZero::PionZeroDefinition();
    G4KaonPlus::KaonPlusDefinition();
    G4KaonMinus::KaonMinusDefinition();

    // **********//
    //  barions  //
    // **********//
    G4Proton::ProtonDefinition();
    G4AntiProton::AntiProtonDefinition();
    G4Neutron::NeutronDefinition();
    G4AntiNeutron::AntiNeutronDefinition();

    // ******//
    //  ions //
    // ******//
    G4Deuteron::DeuteronDefinition();
    G4Triton::TritonDefinition();
    G4He3::He3Definition();
    G4Alpha::AlphaDefinition();
    G4GenericIon::GenericIonDefinition();

    G4LeptonConstructor lepton;
    lepton.ConstructParticle();

    G4BosonConstructor boson;
    boson.ConstructParticle();

    G4MesonConstructor meson;
    meson.ConstructParticle();

    G4BaryonConstructor baryon;
    baryon.ConstructParticle();

    G4ShortLivedConstructor shortLived;
    shortLived.ConstructParticle();

    G4IonConstructor ion;
    ion.ConstructParticle();
}
