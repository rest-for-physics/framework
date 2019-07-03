//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "PrimaryGeneratorAction.hh"

#include "G4Event.hh"
#include "G4Geantino.hh"
#include "G4IonTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleTable.hh"
#include "G4SystemOfUnits.hh"
#include "G4UnitsTable.hh"
#include "Randomize.hh"

#include <TRestG4Event.h>
#include <TRestG4Metadata.h>

extern TRestG4Metadata* restG4Metadata;
extern TRestG4Event* restG4Event;

extern Int_t biasing;

Int_t face = 0;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PrimaryGeneratorAction::PrimaryGeneratorAction(DetectorConstruction* pDetector)
    : G4VUserPrimaryGeneratorAction(), fParticleGun(0), fDetector(pDetector) {
    G4int n_particle = 1;
    fParticleGun = new G4ParticleGun(n_particle);

    nCollections = restG4Metadata->GetPrimaryGenerator().GetNumberOfCollections();

    nBiasingVolumes = restG4Metadata->GetNumberOfBiasingVolumes();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PrimaryGeneratorAction::~PrimaryGeneratorAction() { delete fParticleGun; }

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void PrimaryGeneratorAction::GeneratePrimaries(G4Event* geant4_event) {
    if (restG4Metadata->GetVerboseLevel() >= REST_Info) {
        cout << "INFO: Primary generation" << endl;
    }
    // We have to initialize here and not in start of the event because
    // GeneratePrimaries is called first, and we want to store event origin and
    // position inside
    // we should have already written the information from previous event to disk
    // (in endOfEventAction)
    restG4Event->Initialize();

    // Singling out generator type "file" so as to...
    // ...not randomize its sequence.
    // ...set a distinct position for each particle.

    string generator_type_name = (string)restG4Metadata->GetGeneratorType();
    generator_type_name = g4_metadata_parameters::CleanString(generator_type_name);
    g4_metadata_parameters::generator_types generator_type;

    if (restG4Metadata->GetVerboseLevel() >= REST_Debug) {
        cout << "DEBUG: Generator type: " << generator_type_name << endl;
    }

    if (g4_metadata_parameters::generator_types_map.count(generator_type_name)) {
        generator_type = g4_metadata_parameters::generator_types_map[generator_type_name];
    } else {
        // if we get here it means the parameter is not valid, we can either assign a default value or stop
        // execution default value
        cout << "Invalid generator type (" + generator_type_name + ") valid values are: ";
        for (const auto& pair : g4_metadata_parameters::generator_types_map) {
            cout << pair.first << ", ";
        }
        cout << endl;

        throw "Invalid generator type";
    }

    // If there are particle collections stored is because we are using a
    // generator from file
    if (nCollections > 0) {
        Int_t rndCollection;
        if (generator_type ==
            g4_metadata_parameters::generator_types::FILE) {  // Generator type "file": no randomisation
            static int nEvts = 0;
            rndCollection = nEvts++;
        } else
            rndCollection = (Int_t)(G4UniformRand() * nCollections);

        restG4Metadata->SetParticleCollection(rndCollection);
    }

    Int_t nParticles = restG4Metadata->GetNumberOfPrimaries();

    if (generator_type != g4_metadata_parameters::generator_types::FILE)  // Except for generator "file"...
        // ...Position is common for all particles
        SetParticlePosition();

    for (int j = 0; j < nParticles; j++) {
        if (generator_type == g4_metadata_parameters::generator_types::FILE)  // Generator type "file"...
            // ...Get position from particle collection
            SetParticlePosition(j);

        // ParticleDefinition should be always declared first (after position).
        SetParticleDefinition(j);

        // Particle Direction must be always set before energy
        SetParticleEnergy(j);

        SetParticleDirection(j);

        fParticleGun->GeneratePrimaryVertex(geant4_event);
    }
}

//_____________________________________________________________________________
G4ParticleDefinition* PrimaryGeneratorAction::SetParticleDefinition(int n) {
    string particle_name = (string)restG4Metadata->GetParticleSource(n).GetParticleName();

    Double_t excited_energy = (double)restG4Metadata->GetParticleSource(n).GetExcitationLevel();

    Int_t charge = restG4Metadata->GetParticleSource(n).GetParticleCharge();

    if (restG4Metadata->GetVerboseLevel() >= REST_Debug) {
        cout << "DEBUG: Particle name: " << particle_name << endl;
        // cout << "DEBUG: Particle charge: " << charge << endl;
        cout << "DEBUG: Particle excited energy: " << excited_energy << endl;
    }

    G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
    G4ParticleDefinition* particle = particleTable->FindParticle(particle_name);

    if ((particle == nullptr)) {
        // There might be a better way to do this
        for (int Z = 1; Z <= 110; Z++)
            for (int A = 2 * Z; A <= 3 * Z; A++) {
                //   cout << "Ion name : " << G4IonTable::GetIonTable()->GetIonName ( Z,
                //   A ) << endl;
                if (particle_name == G4IonTable::GetIonTable()->GetIonName(Z, A)) {
                    particle = G4IonTable::GetIonTable()->GetIon(Z, A, excited_energy);
                    fParticleGun->SetParticleCharge(charge);
                    cout << "Found ion: " << particle_name << " Z " << Z << " A " << A << " excited energy "
                         << excited_energy << endl;
                }
            }
    }

    fParticleGun->SetParticleDefinition(particle);

    restG4Event->SetPrimaryEventParticleName(particle_name);

    return particle;
}

void PrimaryGeneratorAction::SetParticleDirection(int n) {
    G4ThreeVector direction;
    // TODO: maybe reduce code redundancy by defining some functions?
    // TODO: fix bug when giving TH1D with lowercase (e.g. Th1D). string conversion is OK but integral gives
    // exception.
    string angular_dist_type_name = (string)restG4Metadata->GetParticleSource(n).GetAngularDistType();
    angular_dist_type_name = g4_metadata_parameters::CleanString(angular_dist_type_name);
    g4_metadata_parameters::angular_dist_types angular_dist_type;

    if (restG4Metadata->GetVerboseLevel() >= REST_Debug) {
        cout << "DEBUG: Angular distribution: " << angular_dist_type_name << endl;
    }
    // we first check if it is a valid parameter
    if (g4_metadata_parameters::angular_dist_types_map.count(angular_dist_type_name)) {
        angular_dist_type = g4_metadata_parameters::angular_dist_types_map[angular_dist_type_name];
    } else {
        // if we get here it means the parameter is not valid, we can either assign a default value or stop
        // execution default value
        cout << "Invalid angular distribution (" + angular_dist_type_name + ") valid values are: ";
        for (const auto& pair : g4_metadata_parameters::angular_dist_types_map) {
            cout << pair.first << ", ";
        }
        cout << std::endl;
        throw "Invalid angular distribution";
    }
    // generator type
    string generator_type_name = (string)restG4Metadata->GetGeneratorType();
    generator_type_name = g4_metadata_parameters::CleanString(generator_type_name);
    g4_metadata_parameters::generator_types generator_type;
    if (g4_metadata_parameters::generator_types_map.count(generator_type_name)) {
        generator_type = g4_metadata_parameters::generator_types_map[generator_type_name];
    } else {
        // if we get here it means the parameter is not valid, we can either assign a default value or stop
        // execution default value
        cout << "Invalid generator type (" + generator_type_name + ") valid values are: ";
        for (const auto& pair : g4_metadata_parameters::generator_types_map) {
            cout << pair.first << ", ";
        }
        cout << std::endl;
        throw "Invalid generator type";
    }

    if (angular_dist_type == g4_metadata_parameters::angular_dist_types::ISOTROPIC) {
        if (generator_type == g4_metadata_parameters::generator_types::VIRTUAL_BOX) {
            if (face == 0) direction.set(0, -1, 0);
            if (face == 1) direction.set(0, 1, 0);
            if (face == 2) direction.set(-1, 0, 0);
            if (face == 3) direction.set(1, 0, 0);
            if (face == 4) direction.set(0, 0, -1);
            if (face == 5) direction.set(0, 0, 1);

            Double_t theta = GetCosineLowRandomThetaAngle();
            // recording the primaries distribution
            G4ThreeVector referenceOrigin = direction;

            // We rotate the origin direction by the angular distribution angle
            G4ThreeVector orthoVector = direction.orthogonal();
            direction.rotate(theta, orthoVector);

            // We rotate a random angle along the original direction
            Double_t randomAngle = G4UniformRand() * 2 * M_PI;
            direction.rotate(randomAngle, referenceOrigin);
        } else if (generator_type == g4_metadata_parameters::generator_types::VIRTUAL_SPHERE) {
            direction = -fParticleGun->GetParticlePosition().unit();

            Double_t theta = GetCosineLowRandomThetaAngle();

            G4ThreeVector referenceOrigin = direction;

            // We rotate the origin direction by the angular distribution angle
            G4ThreeVector orthoVector = direction.orthogonal();
            direction.rotate(theta, orthoVector);

            // We rotate a random angle along the original direction
            Double_t randomAngle = G4UniformRand() * 2 * M_PI;
            direction.rotate(randomAngle, referenceOrigin);

        } else {
            direction = GetIsotropicVector();
        }
    } else if (angular_dist_type == g4_metadata_parameters::angular_dist_types::TH1D) {
        Double_t angle = 0;
        Double_t value = G4UniformRand() * (fAngularDistribution->Integral());
        Double_t sum = 0;
        // deltaAngle is the constant x distance between bins
        Double_t deltaAngle = fAngularDistribution->GetBinCenter(2) - fAngularDistribution->GetBinCenter(1);
        // we sample the CDF (uniform between 0 and the distribution integral which should be equal to 1)
        // the inverse of CDF of the uniformly sampled value will follow a distribution given by the PDF, we
        // compute this inverse
        for (int bin = 1; bin <= fAngularDistribution->GetNbinsX(); bin++) {
            sum += fAngularDistribution->GetBinContent(bin);

            if (sum >= value) {
                angle = fAngularDistribution->GetBinCenter(bin) + deltaAngle * (0.5 - G4UniformRand());
                break;
            }
        }

        // A vector pointing to the origin (virtualSphere )
        direction = -fParticleGun->GetParticlePosition().unit();

        if (direction.x() == 0 && direction.y() == 0 && direction.z() == 0) {
            cout << "----------------------------------------------------------------"
                    "-----"
                 << endl;
            cout << "REST WARNNING : Particle being launched from the ORIGIN!! Wrong "
                    "momentum direction!"
                 << endl;
            cout << "Setting direction to (1,0,0)" << endl;
            cout << "REST angular distribution is just implemented for virtualBox "
                    "and virtualSphere"
                 << endl;
            cout << "Other spatial distributions can be set but it will launch the "
                    "event\n with a distribution direction to the origin of "
                    "coordinates"
                 << endl;
            cout << "----------------------------------------------------------------"
                    "-----"
                 << endl;
            direction.set(1, 0, 0);
        }

        if (generator_type == g4_metadata_parameters::generator_types::VIRTUAL_BOX) {
            if (face == 0) direction.set(0, -1, 0);
            if (face == 1) direction.set(0, 1, 0);
            if (face == 2) direction.set(-1, 0, 0);
            if (face == 3) direction.set(1, 0, 0);
            if (face == 4) direction.set(0, 0, -1);
            if (face == 5) direction.set(0, 0, 1);
        }

        if (generator_type == g4_metadata_parameters::generator_types::VIRTUAL_WALL) {
            /*
            The default plane (virtualWall) is an XY plane so the default normal vector is (0,0,1).
            We will rotate this vector according to the generator rotation so that keeps being normal to the
            plane
            */
            TVector3 normal(0, 0, 1);

            normal.RotateX(M_PI * restG4Metadata->GetGeneratorRotation().X() / 180);
            normal.RotateY(M_PI * restG4Metadata->GetGeneratorRotation().Y() / 180);
            normal.RotateZ(M_PI * restG4Metadata->GetGeneratorRotation().Z() / 180);

            /*
            Depending on which rotation we chose for the plane the normal vector can now point outwards of the
            detector.
            We rotate so that it always looks towards the center (0,0,0)
            */
            TVector3 generator_position = restG4Metadata->GetGeneratorPosition().Unit();
            if (generator_position.x() * normal.x() + generator_position.y() * normal.y() +
                    generator_position.z() * normal.z() >
                0) {
                normal = (-1) * normal;
            }

            direction.set(normal.x(), normal.y(), normal.z());
        }

        G4ThreeVector referenceOrigin = direction;

        // We rotate the origin direction by the angular distribution angle
        G4ThreeVector orthoVector = direction.orthogonal();
        direction.rotate(angle, orthoVector);

        // We rotate a random angle along the radial direction
        Double_t randomAngle = G4UniformRand() * 2 * M_PI;
        direction.rotate(randomAngle, referenceOrigin);

        //       G4cout << "Angle  " << direction.angle( referenceOrigin ) << "
        //       should be = to " << angle << G4endl;

    } else if (angular_dist_type == g4_metadata_parameters::angular_dist_types::FLUX) {
        TVector3 v = restG4Metadata->GetParticleSource(n).GetDirection();

        v = v.Unit();

        direction.set(v.X(), v.Y(), v.Z());

    } else if (angular_dist_type == g4_metadata_parameters::angular_dist_types::BACK_TO_BACK) {
        // This should never crash. In TRestG4Metadata we have defined that if the
        // first source is backtoback we set it to isotropic
        TVector3 v = restG4Event->GetPrimaryEventDirection(n - 1);
        v = v.Unit();

        direction.set(-v.X(), -v.Y(), -v.Z());
    } else {
        G4cout << "WARNING: Generator angular distribution was not recognized. "
                  "Launching particle to (1,0,0)"
               << G4endl;
    }

    // storing the direction in TRestG4Event class
    TVector3 eventDirection(direction.x(), direction.y(), direction.z());
    restG4Event->SetPrimaryEventDirection(eventDirection);

    if (restG4Metadata->GetVerboseLevel() >= REST_Debug) {
        cout << "DEBUG: Event direction (normalized): "
             << "(" << restG4Event->GetPrimaryEventDirection(n).X() << ", "
             << restG4Event->GetPrimaryEventDirection(n).Y() << ", "
             << restG4Event->GetPrimaryEventDirection(n).Z() << ")" << endl;
    }

    // setting particle direction
    fParticleGun->SetParticleMomentumDirection(direction);
}

//_____________________________________________________________________________
void PrimaryGeneratorAction::SetParticleEnergy(int n) {
    Double_t energy = 0;

    string energy_dist_type_name = (string)restG4Metadata->GetParticleSource(n).GetEnergyDistType();
    energy_dist_type_name = g4_metadata_parameters::CleanString(energy_dist_type_name);

    if (restG4Metadata->GetVerboseLevel() >= REST_Debug) {
        cout << "DEBUG: Energy distribution: " << energy_dist_type_name << endl;
    }

    g4_metadata_parameters::energy_dist_types energy_dist_type;
    if (g4_metadata_parameters::energy_dist_types_map.count(energy_dist_type_name)) {
        energy_dist_type = g4_metadata_parameters::energy_dist_types_map[energy_dist_type_name];
    } else {
        // if we get here it means the parameter is not valid, we can either assign a default value or stop
        // execution default value in this case is 1 keV
        cout << "Invalid energy distribution (" + energy_dist_type_name + ") valid values are: ";
        for (const auto& pair : g4_metadata_parameters::energy_dist_types_map) {
            cout << pair.first << ", ";
        }
        cout << std::endl;
        G4cout << "WARNING! Energy distribution type was not recognized. Setting "
                  "energy to 1keV"
               << G4endl;
        energy = 1 * keV;
        // maybe it would be better to set type as mono and change energy of particle, or define the default
        // energy there throw "Invalid energy distribution type";
    }

    if (energy_dist_type == g4_metadata_parameters::energy_dist_types::MONO) {
        energy = restG4Metadata->GetParticleSource(n).GetEnergy() * keV;
    } else if (energy_dist_type == g4_metadata_parameters::energy_dist_types::FLAT) {
        TVector2 enRange = restG4Metadata->GetParticleSource(n).GetEnergyRange();

        energy = ((enRange.Y() - enRange.X()) * G4UniformRand() + enRange.X()) * keV;
    } else if (energy_dist_type == g4_metadata_parameters::energy_dist_types::TH1D) {
        Double_t value = G4UniformRand() * fSpectrumIntegral;
        Double_t sum = 0;
        Double_t deltaEnergy = fSpectrum->GetBinCenter(2) - fSpectrum->GetBinCenter(1);
        for (int bin = startEnergyBin; bin <= endEnergyBin; bin++) {
            sum += fSpectrum->GetBinContent(bin);

            if (sum > value) {
                energy = energyFactor *
                         (Double_t)(fSpectrum->GetBinCenter(bin) + deltaEnergy * (0.5 - G4UniformRand())) *
                         keV;
                break;
            }
        }
    } else {
        G4cout << "WARNING! Energy distribution type was not recognized. Setting "
                  "energy to 1keV"
               << G4endl;
        energy = 1 * keV;
    }

    fParticleGun->SetParticleEnergy(energy);

    restG4Event->SetPrimaryEventEnergy(energy / keV);

    if (restG4Metadata->GetVerboseLevel() >= REST_Debug)
        cout << "DEBUG: Particle energy: " << energy / keV << " keV" << endl;
}

//_____________________________________________________________________________
void PrimaryGeneratorAction::SetParticlePosition() {
    double x = 0, y = 0, z = 0;
    string generator_type_name = (string)restG4Metadata->GetGeneratorType();
    g4_metadata_parameters::generator_types generator_type =
        g4_metadata_parameters::generator_types_map[g4_metadata_parameters::CleanString(generator_type_name)];

    if (generator_type == g4_metadata_parameters::generator_types::VOLUME) {
        double xMin = fDetector->GetBoundingX_min();
        double xMax = fDetector->GetBoundingX_max();
        double yMin = fDetector->GetBoundingY_min();
        double yMax = fDetector->GetBoundingY_max();
        double zMin = fDetector->GetBoundingZ_min();
        double zMax = fDetector->GetBoundingZ_max();

        do {
            x = xMin + (xMax - xMin) * G4UniformRand();
            y = yMin + (yMax - yMin) * G4UniformRand();
            z = zMin + (zMax - zMin) * G4UniformRand();
        } while (fDetector->GetGeneratorSolid()->Inside(G4ThreeVector(x, y, z)) != kInside);

        x = x + fDetector->GetGeneratorTranslation().x();
        y = y + fDetector->GetGeneratorTranslation().y();
        z = z + fDetector->GetGeneratorTranslation().z();
    } else if (generator_type == g4_metadata_parameters::generator_types::SURFACE) {
        // TODO there is a problem, probably with G4 function GetPointOnSurface
        // It produces a point on the surface but it is not uniformly distributed
        // May be it is just an OPENGL drawing issue?

        G4ThreeVector position = fDetector->GetGeneratorSolid()->GetPointOnSurface();

        x = position.x();
        y = position.y();
        z = position.z();

        x = x + fDetector->GetGeneratorTranslation().x();
        y = y + fDetector->GetGeneratorTranslation().y();
        z = z + fDetector->GetGeneratorTranslation().z();
    } else if (generator_type == g4_metadata_parameters::generator_types::POINT) {
        TVector3 position = restG4Metadata->GetGeneratorPosition();

        x = position.X();
        y = position.Y();
        z = position.Z();

    } else if (generator_type == g4_metadata_parameters::generator_types::VIRTUAL_WALL) {
        Double_t side = restG4Metadata->GetGeneratorSize();

        x = side * (G4UniformRand() - 0.5);
        y = side * (G4UniformRand() - 0.5);

        G4ThreeVector rndPos = G4ThreeVector(x, y, 0);
        rndPos.rotateX(M_PI * restG4Metadata->GetGeneratorRotation().X() / 180.);
        rndPos.rotateY(M_PI * restG4Metadata->GetGeneratorRotation().Y() / 180.);
        rndPos.rotateZ(M_PI * restG4Metadata->GetGeneratorRotation().Z() / 180.);

        TVector3 center = restG4Metadata->GetGeneratorPosition();

        x = rndPos.x() + center.X();
        y = rndPos.y() + center.Y();
        z = rndPos.z() + center.Z();
    } else if (generator_type == g4_metadata_parameters::generator_types::VIRTUAL_CIRCLE_WALL) {
        Double_t radius = restG4Metadata->GetGeneratorSize();

        do {
            x = 2 * radius * (G4UniformRand() - 0.5);
            y = 2 * radius * (G4UniformRand() - 0.5);
            //       cout << "x : " << x << " y : " << y << endl;
        } while (x * x + y * y > radius * radius);

        G4ThreeVector rndPos = G4ThreeVector(x, y, 0);
        rndPos.rotateX(M_PI * restG4Metadata->GetGeneratorRotation().X() / 180.);
        rndPos.rotateY(M_PI * restG4Metadata->GetGeneratorRotation().Y() / 180.);
        rndPos.rotateZ(M_PI * restG4Metadata->GetGeneratorRotation().Z() / 180.);

        TVector3 center = restG4Metadata->GetGeneratorPosition();

        x = rndPos.x() + center.X();
        y = rndPos.y() + center.Y();
        z = rndPos.z() + center.Z();
    } else if (generator_type == g4_metadata_parameters::generator_types::VIRTUAL_SPHERE) {
        G4ThreeVector rndPos = GetIsotropicVector();

        Double_t radius = restG4Metadata->GetGeneratorSize();

        TVector3 center = restG4Metadata->GetGeneratorPosition();

        x = radius * rndPos.x() + center.X();
        y = radius * rndPos.y() + center.Y();
        z = radius * rndPos.z() + center.Z();
    } else if (generator_type == g4_metadata_parameters::generator_types::VIRTUAL_CYLINDER) {
        Double_t angle = 2 * M_PI * G4UniformRand();

        Double_t radius = restG4Metadata->GetGeneratorSize();
        Double_t length = restG4Metadata->GetGeneratorLength();

        x = radius * cos(angle);
        y = radius * sin(angle);
        z = length * (G4UniformRand() - 0.5);

        G4ThreeVector rndPos = G4ThreeVector(x, y, z);
        rndPos.rotateX(M_PI * restG4Metadata->GetGeneratorRotation().X() / 180.);
        rndPos.rotateY(M_PI * restG4Metadata->GetGeneratorRotation().Y() / 180.);
        rndPos.rotateZ(M_PI * restG4Metadata->GetGeneratorRotation().Z() / 180.);

        TVector3 center = restG4Metadata->GetGeneratorPosition();

        x = rndPos.x() + center.X();
        y = rndPos.y() + center.Y();
        z = rndPos.z() + center.Z();
    } else if (generator_type == g4_metadata_parameters::generator_types::VIRTUAL_BOX) {
        Double_t side = restG4Metadata->GetGeneratorSize();

        x = side * (G4UniformRand() - 0.5);
        y = side * (G4UniformRand() - 0.5);

        G4ThreeVector rndPos = G4ThreeVector(x, y, 0);

        Double_t rndOrientation = 3 * G4UniformRand();
        Double_t rndFace = G4UniformRand();
        if (rndOrientation <= 1) {
            // Event is in plane XZ
            rndPos.rotateX(M_PI / 2.);
            x = rndPos.x();
            y = rndPos.y();
            z = rndPos.z();

            // Some rounding error after rotate
            y = 0;

            // cout << "Event in plane XZ. x : " << x << " y : " << y << " z : " << z
            // << endl;

            if (rndFace <= 0.5) {
                y = y + side / 2.;
                face = 0;
            } else {
                y = y - side / 2.;
                face = 1;
            }
        } else if (rndOrientation <= 2) {
            // Event is in plane YZ
            rndPos.rotateY(M_PI / 2.);
            x = rndPos.x();
            y = rndPos.y();
            z = rndPos.z();

            // Some rounding error after rotate
            x = 0;

            //           cout << "Event in plane YZ. x : " << x << " y : " << y << " z
            //           : " << z << endl;

            if (rndFace <= 0.5) {
                x = x + side / 2.;
                face = 2;
            } else {
                x = x - side / 2.;
                face = 3;
            }
        } else {
            // Event is in plane XY
            x = rndPos.x();
            y = rndPos.y();
            z = rndPos.z();

            //         cout << "Event in plane XY. x : " << x << " y : " << y << " z :
            //         " << z << endl;

            //      cout << "z : " << y;
            if (rndFace <= 0.5) {
                z = z + side / 2.;
                face = 4;
            } else {
                z = z - side / 2.;
                face = 5;
            }
        }

        TVector3 center = restG4Metadata->GetGeneratorPosition();

        // G4cout << "X : " << x << " Y : " << y << " Z : " << z << G4endl;

        x = x + center.X();
        y = y + center.Y();
        z = z + center.Z();
    } else {
        G4cout << "WARNING! Generator type was not recognized. Launching particle "
                  "from origin (0,0,0)"
               << G4endl;
    }

    // storing the direction in TRestG4Event class
    TVector3 eventPosition(x, y, z);
    restG4Event->SetPrimaryEventOrigin(eventPosition);

    if (restG4Metadata->GetVerboseLevel() >= REST_Debug) {
        cout << "DEBUG: Event origin: "
             << "(" << restG4Event->GetPrimaryEventOrigin().X() << ", "
             << restG4Event->GetPrimaryEventOrigin().Y() << ", " << restG4Event->GetPrimaryEventOrigin().Z()
             << ")"
             << " mm" << endl;
    }

    // setting particle position
    fParticleGun->SetParticlePosition(G4ThreeVector(x, y, z));
}

//_____________________________________________________________________________
void PrimaryGeneratorAction::SetParticlePosition(int n) {
    // Storing particle's position to that retrieved from TRestParticle
    TVector3 pos = restG4Metadata->GetParticleSource(n).GetOrigin();
    restG4Event->SetPrimaryEventOrigin(pos);

    if (restG4Metadata->GetVerboseLevel() >= REST_Debug) {
        cout << "DEBUG: Event origin: "
             << "(" << restG4Event->GetPrimaryEventOrigin().X() << ", "
             << restG4Event->GetPrimaryEventOrigin().Y() << ", " << restG4Event->GetPrimaryEventOrigin().Z()
             << ")"
             << " mm" << endl;
    }

    // Setting particle position
    fParticleGun->SetParticlePosition(G4ThreeVector(pos.X(), pos.Y(), pos.Z()));
}

//_____________________________________________________________________________
G4ThreeVector PrimaryGeneratorAction::GetIsotropicVector() {
    G4double a, b, c;
    G4double n;
    do {
        a = (G4UniformRand() - 0.5) / 0.5;
        b = (G4UniformRand() - 0.5) / 0.5;
        c = (G4UniformRand() - 0.5) / 0.5;
        n = a * a + b * b + c * c;
    } while (n > 1 || n == 0.0);

    n = std::sqrt(n);
    a /= n;
    b /= n;
    c /= n;
    return G4ThreeVector(a, b, c);
}
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//
Double_t PrimaryGeneratorAction::GetAngle(G4ThreeVector x, G4ThreeVector y) {
    Double_t angle = y.angle(x);

    return angle;
}

Double_t PrimaryGeneratorAction::GetCosineLowRandomThetaAngle() {
    // We obtain an angle with a cos(theta)*sin(theta) distribution
    Double_t value = G4UniformRand();
    double dTheta = 0.01;
    for (double theta = 0; theta < M_PI / 2; theta = theta + dTheta) {
        // sin(theta)^2 is the integral of sin(theta)*cos(theta)
        if (sin(theta) * sin(theta) >= value) {
            if (theta > 0)
                return theta - dTheta * (0.5 - G4UniformRand());
            else
                return theta;
        }
    }
    return M_PI / 2;
}
