#ifndef DetectorConstruction_h
#define DetectorConstruction_h 1

#include <G4PhysicalVolumeStore.hh>
#include "G4VUserDetectorConstruction.hh"
//#include <G4SystemOfUnits.hh>

#include <G4GDMLParser.hh>

#include <TRestG4Event.h>
#include <TRestG4Metadata.h>

class DetectorConstruction : public G4VUserDetectorConstruction {
   private:
    G4GDMLParser* parser;
    G4VSolid* generatorSolid;
    G4ThreeVector fGeneratorTranslation;

    Double_t boundBox_xMin, boundBox_xMax;
    Double_t boundBox_yMin, boundBox_yMax;
    Double_t boundBox_zMin, boundBox_zMax;

   public:
    G4GDMLParser* GetGeometry() { return parser; }
    G4VPhysicalVolume* GetPhysicalVolume(G4String physVolName);
    G4VSolid* GetGeneratorSolid() { return generatorSolid; }
    G4ThreeVector GetGeneratorTranslation() { return fGeneratorTranslation; }

    Double_t GetBoundingX_min() { return boundBox_xMin; }
    Double_t GetBoundingX_max() { return boundBox_xMax; }

    Double_t GetBoundingY_min() { return boundBox_yMin; }
    Double_t GetBoundingY_max() { return boundBox_yMax; }

    Double_t GetBoundingZ_min() { return boundBox_zMin; }
    Double_t GetBoundingZ_max() { return boundBox_zMax; }

    DetectorConstruction();
    ~DetectorConstruction();

   public:
    G4VPhysicalVolume* Construct();
};
#endif
