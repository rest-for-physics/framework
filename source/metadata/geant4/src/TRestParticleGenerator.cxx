//////////////////////////////////////////////////////////////////////////
///
///             RESTSoft: Software for Rare Event Searches with TPCs
///
///             TRestParticleGenerator.cxx
///
///             This class handles particle generation: spatial, angular and energy sampling according to the
///             information stored in this class, which is supposed to be loaded from a `TRestG4Metadata`
///             object. This class is invoked in the GEANT4 code to define primary sources.
///
///             Aug 2019: Added. Luis Obis (@lobis)
///
//////////////////////////////////////////////////////////////////////////

#include <TRestG4Metadata.h>

#include <TRestParticleGenerator.h>

using namespace std;

/*
namespace {

enum class energyGeneratorTypes {
    ROOT_HIST,
    MONO,
    FLAT,
};
enum class angularGeneratorTypes {
    ROOT_HIST,
    ISOTROPIC,
    FLUX,
    BACK_TO_BACK,
};
enum class spatialGeneratorTypes {
    FILE,
    VOLUME,
    SURFACE,
    POINT,
    VIRTUAL_WALL,
    VIRTUAL_BOX,
    VIRTUAL_SPHERE,
    VIRTUAL_CIRCLE_WALL,
    VIRTUAL_CYLINDER,
};

}

const std::map<string, spatialGeneratorTypes> spatialGeneratorTypesMap = {
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

const std::map<string, angularGeneratorTypes> angularGeneratorTypesMap = {
    {"TH1D", angularGeneratorTypes::ROOT_HIST},
    {"ISOTROPIC", angularGeneratorTypes::ISOTROPIC},
    {"FLUX", angularGeneratorTypes::FLUX},
    {"BACK_TO_BACK", angularGeneratorTypes::BACK_TO_BACK},
};

const map<string, energyGeneratorTypes> energyGeneratorTypesMap = {
    {"T1HD", energyGeneratorTypes::ROOT_HIST},
    {"MONO", energyGeneratorTypes::MONO},
    {"FLAT", energyGeneratorTypes::FLAT},
};
*/

TRestParticleGenerator::TRestParticleGenerator() {}

TRestParticleGenerator::TRestParticleGenerator(const TRestG4Metadata& restG4Metadata) {
    // load random seed
    Long_t randomSeed = restG4Metadata.GetSeed();
    SetRandomSeed(randomSeed);
    // load spatial generator
    string generatorTypeString = (string)restG4Metadata.GetGeneratorType();
    SetSpatialGeneratorType(generatorTypeString);
}