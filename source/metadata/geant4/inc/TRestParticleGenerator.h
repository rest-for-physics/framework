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

#include <TRandom3.h>
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

    TVector3 fParticlePosition;
    TVector3 fParticleDirection;
    Double_t fParticleEnergy;
    Long_t fSeed = 0;

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
    // geometric parameters
    const std::map<string, geometricParameters> geometricParameterMap = {
        {"size", geometricParameters::SIZE},     {"radius", geometricParameters::RADIUS},
        {"length", geometricParameters::LENGTH}, {"lenX", geometricParameters::LENGTH_X},
        {"lenY", geometricParameters::LENGTH_Y},
    };
    inline string GetGeometricParameterName(geometricParameters geometricParameter) {
        for (auto const& pair : geometricParameterMap) {
            if (pair.second == geometricParameter) {
                return pair.first;
            }
        }
        return "ERROR!";
    }
    Double_t notAssignedValueDouble = -1;  // all parameters are positive
    std::map<geometricParameters, Double_t> geometricParameterValues = {
        {geometricParameters::SIZE, notAssignedValueDouble},
        {geometricParameters::RADIUS, notAssignedValueDouble},
        {geometricParameters::LENGTH, notAssignedValueDouble},
        {geometricParameters::LENGTH_X, notAssignedValueDouble},
        {geometricParameters::LENGTH_Y, notAssignedValueDouble},
    };
    // which parameters are required for each spatial generator
    set<geometricParameters> GetRequiredGeometricParameters(spatialGeneratorTypes) const;

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

    string noGeneratorTypeSpecified = "NOT DEFINED!";
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
            exit(0);
            return "ERROR!";
        }
        return noGeneratorTypeSpecified;
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
    inline void SetRandomSeed(Long_t seed) {
        fSeed = seed;
        fRandom = TRandom3(fSeed);
    }
    TRandom3 fRandom = TRandom3(fSeed);

    inline void SetGeometricParameter(string parameterName, Double_t parameterValue) {
        auto requiredParameters = GetRequiredGeometricParameters(fSpatialGeneratorType);
        // check if a valid parameter name was specified
        if (geometricParameterMap.count(parameterName) == 0) {
            cout << "bad parameter name: " << parameterName << endl;
            cout << "valid parameter names for generator: " << GetSpatialGeneratorType() << " are: ";
            for (auto const& parameter : requiredParameters) {
                cout << GetGeometricParameterName(parameter) << ", ";
            }
            cout << endl;
            return;
        }
        auto geometricParameter = geometricParameterMap.at(parameterName);
        geometricParameterValues[geometricParameter] = parameterValue;
    }
    void Sample();
    inline TVector3 GetParticlePosition() const { return fParticlePosition; }
    inline TVector3 GetParticleDirection() const { return fParticleDirection.Unit(); }
    inline Double_t GetParticleEnergy() const { return fParticleEnergy; }
    inline void SetParticlePosition(TVector3 position) { fParticlePosition = position; }
    inline void SetParticleDirection(TVector3 direction) { fParticleDirection = direction.Unit(); }
    inline void SetParticleEnergy(Double_t energy) { fParticleEnergy = energy; }
    // TRestMetadata methods
   public:
    void PrintMetadata();

   private:
    void InitFromConfigFile();
};
#endif  // REST_TRESTPARTICLEGENERATOR_H
