//////////////////////////////////////////////////////////////////////////
///
///             RESTSoft: Software for Rare Event Searches with TPCs
///
///             TRestParticleGenerator.h
///
///             This class handles particle generation: spatial, angular and energy sampling according to the
///             information stored in this class, which is supposed to be loaded from a `TRestG4Metadata`
///             object. This class is invoked in the GEANT4 code to define primary sources.
///
///             Aug 2019: Introduction. Luis Obis (@lobis)
///
//////////////////////////////////////////////////////////////////////////

#ifndef REST_TRESTPARTICLEGENERATOR_H
#define REST_TRESTPARTICLEGENERATOR_H

#endif  // REST_TRESTPARTICLEGENERATOR_H

#include <TRestEnums.h>

#include <TVector3.h>
#include <map>
#include <set>

using namespace std;
using namespace TRestParticleGeneratorConfig;

class TRestParticleGenerator {
   private:
    TVector3 particlePosition;
    TVector3 particleDirection;
    Double_t particleEnergy;
    // configuration

    std::map<string, spatialGeneratorTypes> spatialGeneratorTypesMap = {
        {"FILE", spatialGeneratorTypes::FILE},
        {"VOLUME", spatialGeneratorTypes::VOLUME},
        {"SURFACE", spatialGeneratorTypes::SURFACE},
        {"POINT", spatialGeneratorTypes::POINT},
        {"VIRTUAL_WALL", spatialGeneratorTypes::VIRTUAL_WALL},
        {"VIRTUAL_BOX", spatialGeneratorTypes::VIRTUAL_BOX},
        {"VIRTUAL_SPHERE", spatialGeneratorTypes::VIRTUAL_SPHERE},
        {"VIRTUAL_CIRCLE_WALL", spatialGeneratorTypes::VIRTUAL_CIRCLE_WALL},
        {"VIRTUAL_CYLINDER", spatialGeneratorTypes::VIRTUAL_CYLINDER},
    };

    std::map<string, angularGeneratorTypes> angularGeneratorTypesMap = {
        {"TH1D", angularGeneratorTypes::ROOT_HIST},
        {"ISOTROPIC", angularGeneratorTypes::ISOTROPIC},
        {"FLUX", angularGeneratorTypes::FLUX},
        {"BACK_TO_BACK", angularGeneratorTypes::BACK_TO_BACK},
    };

    map<string, energyGeneratorTypes> energyGeneratorTypesMap = {
        {"T1HD", energyGeneratorTypes::ROOT_HIST},
        {"MONO", energyGeneratorTypes::MONO},
        {"FLAT", energyGeneratorTypes::FLAT},
    };

    spatialGeneratorTypes spatialGeneratorType;
    angularGeneratorTypes angularGeneratorType;
    energyGeneratorTypes energyGeneratorType;

   public:
    inline string GetSpatialGeneratorType() {
        for (auto const& pair : spatialGeneratorTypesMap) {
            if (pair.second == spatialGeneratorType) {
                return pair.first;
            }
        }
    }
    inline string GetAngularGeneratorType() {
        for (auto const& pair : angularGeneratorTypesMap) {
            if (pair.second == angularGeneratorType) {
                return pair.first;
            }
        }
    }
    inline string GetEnergyGeneratorType() {
        for (auto const& pair : energyGeneratorTypesMap) {
            if (pair.second == energyGeneratorType) {
                return pair.first;
            }
        }
    }

    TRestParticleGenerator();
};