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
///             Aug 2019: Added. Luis Obis (@lobis)
///
//////////////////////////////////////////////////////////////////////////

#ifndef REST_TRESTPARTICLEGENERATOR_H
#define REST_TRESTPARTICLEGENERATOR_H

#include <TRestEnums.h>
#include <TRestMetadata.h>

#include <TVector3.h>

#include <iostream>
#include <map>
#include <set>

using namespace std;
using namespace TRestParticleGeneratorConfig;

class TRestParticleGenerator : public TRestMetadata {
   public:
    // Constructors
    TRestParticleGenerator();
    TRestParticleGenerator(const string&, const string& name = "TRestG4Metadata");
    // Destructor
    inline ~TRestParticleGenerator(){};
    ClassDef(TRestParticleGenerator, 1);

   private:
    inline void Initialize() { SetSectionName(this->ClassName()); }
    void InitFromConfigFile();

    TVector3 particlePosition;
    TVector3 particleDirection;
    Double_t particleEnergy;
    Long_t fSeed;

    string fFromVolume;
    TVector3 fGeneratorPosition;
    TVector3 fGeneratorNormal;  // normal vector used to define orientation
    inline void SetGeneratorNormal(TVector3 normalVector) { fGeneratorNormal = normalVector.Unit(); }
    // configuration
    inline bool IsGeneratorPositionRequired() const {
        if (fSpatialGeneratorType == spatialGeneratorTypes::SURFACE ||
            fSpatialGeneratorType == spatialGeneratorTypes::POINT ||
            fSpatialGeneratorType == spatialGeneratorTypes::VIRTUAL_WALL ||
            fSpatialGeneratorType == spatialGeneratorTypes::VIRTUAL_BOX ||
            fSpatialGeneratorType == spatialGeneratorTypes::VIRTUAL_SPHERE ||
            fSpatialGeneratorType == spatialGeneratorTypes::VIRTUAL_CIRCLE_WALL ||
            fSpatialGeneratorType == spatialGeneratorTypes::VIRTUAL_CYLINDER) {
            return true;
        }
        return false;
    }
    inline bool IsGeneratorNormalRequired() const {
        if (fSpatialGeneratorType == spatialGeneratorTypes::SURFACE ||
            fSpatialGeneratorType == spatialGeneratorTypes::VIRTUAL_WALL ||
            fSpatialGeneratorType == spatialGeneratorTypes::VIRTUAL_BOX ||
            fSpatialGeneratorType == spatialGeneratorTypes::VIRTUAL_CIRCLE_WALL ||
            fSpatialGeneratorType == spatialGeneratorTypes::VIRTUAL_CYLINDER) {
            return true;
        }
        return false;
    };

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

    spatialGeneratorTypes fSpatialGeneratorType;
    angularGeneratorTypes fAngularGeneratorType;
    energyGeneratorTypes fEnergyGeneratorType;

    template <class generatorTypes>
    string GeneratorEnumToString(generatorTypes type) const {
        // type is in either 'spatialGeneratorTypes', 'angularGeneratorTypes' or 'energyGeneratorTypes'
        if (typeid(generatorTypes) == typeid(spatialGeneratorTypes)) {
            for (auto const& pair : spatialGeneratorTypesMap) {
                if (pair.second == fSpatialGeneratorType) {
                    return pair.first;
                }
            }
        } else if (typeid(generatorTypes) == typeid(angularGeneratorTypes)) {
            for (auto const& pair : angularGeneratorTypesMap) {
                if (pair.second == fAngularGeneratorType) {
                    return pair.first;
                }
            }
        } else if (typeid(generatorTypes) == typeid(energyGeneratorTypes)) {
            for (auto const& pair : energyGeneratorTypesMap) {
                if (pair.second == fEnergyGeneratorType) {
                    return pair.first;
                }
            }
        } else {
            // error
            return "ERROR!";
        }
        return "NONE! (error)";
    }
    inline string NormalizeTypeString(string type) const {
        std::transform(type.begin(), type.end(), type.begin(), ::tolower);
        // remove '_'
        string string_to_remove = "_";
        while (type.find(string_to_remove) != string::npos) {
            type.replace(type.find("_"), string_to_remove.length(), "");
        }
        return type;
    }
    inline void SetGeneratorTypeFromStringAndCategory(string type, string generator_category) {
        // generator_category must be one of the following: 'spatial', 'angular', 'energy'
        if (generator_category == "spatial") {
            for (auto const& pair : spatialGeneratorTypesMap) {
                if (NormalizeTypeString(pair.first) == NormalizeTypeString(type)) {
                    fSpatialGeneratorType = pair.second;
                    return;
                }
            }
        } else if (generator_category == "angular") {
            for (auto const& pair : angularGeneratorTypesMap) {
                if (NormalizeTypeString(pair.first) == NormalizeTypeString(type)) {
                    fAngularGeneratorType = pair.second;
                    return;
                }
            }
        } else if (generator_category == "energy") {
            for (auto const& pair : energyGeneratorTypesMap) {
                if (NormalizeTypeString(pair.first) == NormalizeTypeString(type)) {
                    fEnergyGeneratorType = pair.second;
                    return;
                }
            }
        } else {
            // generator_category not valid
            cout << "ERROR: category " << generator_category << " not valid. nothing is set";
            return;
        }

        cout << "ERROR when setting " << generator_category << " generator type to: " << type << endl;
        cout << "Valid values are: ";
        string to_print = "";

        if (generator_category == "spatial") {
            for (auto const& pair : spatialGeneratorTypesMap) {
                to_print += pair.first;
                to_print += ", ";
            }
        } else if (generator_category == "angular") {
            for (auto const& pair : angularGeneratorTypesMap) {
                to_print += pair.first;
                to_print += ", ";
            }
        } else if (generator_category == "energy") {
            for (auto const& pair : energyGeneratorTypesMap) {
                to_print += pair.first;
                to_print += ", ";
            }
        }
        // remove the last ", "
        if (to_print.size() > 0) to_print.resize(to_print.size() - 2);  // '2' is the size of ", "
        cout << to_print << endl;
        exit(0);
    }

   public:
    // generators
    inline string GetSpatialGeneratorType() const { return GeneratorEnumToString(fSpatialGeneratorType); }
    inline string GetAngularGeneratorType() const { return GeneratorEnumToString(fAngularGeneratorType); }
    inline string GetEnergyGeneratorType() const { return GeneratorEnumToString(fEnergyGeneratorType); }

    inline void SetSpatialGeneratorType(spatialGeneratorTypes type) { fSpatialGeneratorType = type; }
    inline void SetSpatialGeneratorType(string type) {
        SetGeneratorTypeFromStringAndCategory(type, "spatial");
    }
    inline void SetAngularGeneratorType(angularGeneratorTypes type) { fAngularGeneratorType = type; }
    inline void SetAngularGeneratorType(string type) {
        SetGeneratorTypeFromStringAndCategory(type, "angular");
    }
    inline void SetEnergyGeneratorType(energyGeneratorTypes type) { fEnergyGeneratorType = type; }
    inline void SetEnergyGeneratorType(string type) { SetGeneratorTypeFromStringAndCategory(type, "energy"); }
    // random seed
    inline Long_t GetRandomSeed() const { return fSeed; }
    inline void SetRandomSeed(Long_t seed) { fSeed = seed; }

    // TRestMetadata methods
    void PrintMetadata();
};
#endif  // REST_TRESTPARTICLEGENERATOR_H
