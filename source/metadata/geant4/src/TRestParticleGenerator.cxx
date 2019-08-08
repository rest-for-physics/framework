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

TRestParticleGenerator::TRestParticleGenerator() {}

TRestParticleGenerator::TRestParticleGenerator(const TRestG4Metadata& restG4Metadata) {
    string generatorTypeString = (string)restG4Metadata.GetGeneratorType();
    SetSpatialGeneratorType(generatorTypeString);
}