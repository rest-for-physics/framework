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

ClassImp(TRestParticleGenerator)

    TRestParticleGenerator::TRestParticleGenerator()
    : TRestMetadata() {
    Initialize();
}
TRestParticleGenerator::TRestParticleGenerator(const string& configFileName, const string& name)
    : TRestMetadata(configFileName.c_str()) {
    Initialize();
    LoadConfigFromFile(fConfigFileName, name);
    PrintMetadata();
}

void TRestParticleGenerator::InitFromConfigFile() {
    string parameterNotFoundString = "Not defined";

    string generatorString = GetKEYStructure("generator");
    string generatorDefinition = GetKEYDefinition("generator", generatorString);

    // we load random seed if present
    fSeed = (Long_t)StringToInteger(GetParameter("seed", "0"));

    // we define the spatial generator type (box, virtualWall etc.)
    string generatorType = GetFieldValue("type", generatorDefinition);
    SetSpatialGeneratorType(generatorType);

    // we run through all possible parameters
    // TODO: this should be changed in TRestMetadata. We should not use hardcoded values like this.
    // 'from' (volume)
    string fromVolume = GetFieldValue("from", generatorDefinition);
    if (fromVolume != parameterNotFoundString) {
        if (fSpatialGeneratorType == spatialGeneratorTypes::VOLUME) {
            fFromVolume = fromVolume;
        } else {
            // "from is defined but it does not mean anything, configuration is invalid
            cout << "'from' value is defined for spatial generator type other than 'VOLUME'" << endl;
            exit(0);
        }
    }
    // position
    string positionString = GetFieldValue("position", generatorDefinition);
    if (positionString != parameterNotFoundString) {
        if (IsGeneratorPositionRequired()) {
            fGeneratorPosition = Get3DVectorFieldValueWithUnits("position", generatorDefinition);
        } else {
            // "from is defined but it does not mean anything, configuration is invalid
            cout << "'position' should not be defined for this type of generator" << endl;
            exit(0);
        }
    } else {
        // default position to (0,0,0)
        fGeneratorPosition = TVector3(0, 0, 0);
    }
    // rotation or normal vector (mutually exclusive)
    string rotationString = GetFieldValue("rotation", generatorDefinition);
    string normalVectorString = GetFieldValue("normal", generatorDefinition);
    // check if both are defined and give error
    if (rotationString != parameterNotFoundString && normalVectorString != parameterNotFoundString) {
        cout << "'rotation' and 'normal' cannot be defined at the same time" << endl;
        exit(0);
    } else if (rotationString != parameterNotFoundString || normalVectorString != parameterNotFoundString) {
        // only one is defined (correct)
        if (!IsGeneratorNormalRequired()) {
            // is defined but makes no sense
            cout << "generator orientation ('rotation' or 'normal') should not be defined for this type of "
                    "generator"
                 << endl;
            exit(0);
        }
    }
    // if we get here we can safely assign orientation
    if (normalVectorString != parameterNotFoundString) {
        TVector3 normalVector = Get3DVectorFieldValueWithUnits("normal", generatorDefinition);
        SetGeneratorNormal(normalVector);
    } else if (rotationString != parameterNotFoundString) {
        // only rotation is defined
        cout << "WARNING: you are using 'rotation' to specify spatial generator orientation. It is "
                "recommended that you use 'normal' instead (normal vector). 'rotation' might not be "
                "supported in the future"
             << endl;
        // rotation is defined in degrees
        TVector3 rotationVector = StringTo3DVector(GetFieldValue("rotation", generatorDefinition));
        TVector3 normalVector(0, 0, 1);

        normalVector.RotateX(M_PI * rotationVector.X() / 180);
        normalVector.RotateY(M_PI * rotationVector.Y() / 180);
        normalVector.RotateZ(M_PI * rotationVector.Z() / 180);

        SetGeneratorNormal(normalVector);
    } else {
        // none is defined, we default to normal = (0,0,1)
        SetGeneratorNormal(TVector3(0, 0, 1));
    }
    // we make sure our normal vector points towards the origin of the generator
    if (IsGeneratorNormalRequired()) {
        if (fGeneratorPosition.x() * fGeneratorNormal.x() + fGeneratorPosition.y() * fGeneratorNormal.y() +
                fGeneratorPosition.z() * fGeneratorNormal.z() >
            0) {
            SetGeneratorNormal((-1) * fGeneratorNormal);
        }
    }

    // geometric parameters
    Double_t parameterNotFoundDouble = PARAMETER_NOT_FOUND_DBL;
    // get necessary geometric parameters
    set<geometricParameters> requiredParameters = GetRequiredGeometricParameters(fSpatialGeneratorType);
    string parameterName;
    Double_t parameterValue;
    for (geometricParameters const& geometricParameter : requiredParameters) {
        // get parameter name
        parameterName = GetGeometricParameterName(geometricParameter);
        parameterValue = GetDblFieldValueWithUnits(parameterName, generatorDefinition);
        if (parameterValue != parameterNotFoundDouble) {
            geometricParameterValues[geometricParameter] = parameterValue;
        } else {
            // a parameter is missing
            // if type only requires one parameter (sphere: radius) and only size is specified we assign the
            // value of size
            if (requiredParameters.count(geometricParameters::SIZE) == 1 && requiredParameters.size() == 1) {
                parameterName = GetGeometricParameterName(geometricParameters::SIZE);
                parameterValue = GetDblFieldValueWithUnits(parameterName, generatorDefinition);
                geometricParameterValues[geometricParameter] = parameterValue;
            } else if (requiredParameters.count(geometricParameters::LENGTH_X) == 1 &&
                       requiredParameters.count(geometricParameters::LENGTH_Y) == 1 &&
                       requiredParameters.size() == 2) {
                // if X and Y length are required but only size is specified we set equal X and Y length
                parameterName = GetGeometricParameterName(geometricParameters::SIZE);
                parameterValue = GetDblFieldValueWithUnits(parameterName, generatorDefinition);
                geometricParameterValues[geometricParameters::LENGTH_X] = parameterValue;
                geometricParameterValues[geometricParameters::LENGTH_Y] = parameterValue;
            } else {
                cout << "you need to specify parameter: " << parameterName << endl;
                exit(0);
            }
        }
    }
}

set<geometricParameters> TRestParticleGenerator::GetRequiredGeometricParameters(
    spatialGeneratorTypes type) const {
    set<geometricParameters> parameterSet;
    switch (type) {
        case spatialGeneratorTypes::POINT:
            // no geometric parameters
            break;
        case spatialGeneratorTypes::VIRTUAL_WALL:
            parameterSet = {geometricParameters::LENGTH_X, geometricParameters::LENGTH_Y};
            break;
        case spatialGeneratorTypes::VIRTUAL_SPHERE:
            parameterSet = {geometricParameters::RADIUS};
            break;
        case spatialGeneratorTypes::VIRTUAL_CYLINDER:
            parameterSet = {geometricParameters::RADIUS, geometricParameters::LENGTH};
            break;
        case spatialGeneratorTypes::VIRTUAL_CIRCLE_WALL:
            parameterSet = {geometricParameters::RADIUS};
            break;
        case spatialGeneratorTypes::VIRTUAL_BOX:
            parameterSet = {geometricParameters::SIZE};
            break;
        default:
            // by default no geometric parameters required (e.g. VOLUME)
            break;
    }
    return parameterSet;
}

void TRestParticleGenerator::PrintMetadata() {
    TRestMetadata::PrintMetadata();

    metadata << "Spatial generator type: " << GetSpatialGeneratorType() << endl;
    if (IsGeneratorPositionRequired()) {
        metadata << "Generator position: (" << fGeneratorPosition.X() << "," << fGeneratorPosition.Y() << ","
                 << fGeneratorPosition.Z() << ") mm" << endl;
    }
    if (IsGeneratorNormalRequired()) {
        metadata << "Generator orientation (normal vector): (" << fGeneratorNormal.X() << ","
                 << fGeneratorNormal.Y() << "," << fGeneratorNormal.Z() << ")" << endl;
    }
    set<geometricParameters> requiredParameters = GetRequiredGeometricParameters(fSpatialGeneratorType);
    string parameterName;
    Double_t parameterValue;
    for (geometricParameters const& geometricParameter : requiredParameters) {
        // get parameter name
        parameterName = GetGeometricParameterName(geometricParameter);
        parameterValue = geometricParameterValues[geometricParameter];
        metadata << parameterName << ": " << parameterValue << " mm" << endl;
    }
    metadata << "Angular distribution type: " << GetAngularGeneratorType() << endl;
    metadata << "Energy distribution type: " << GetEnergyGeneratorType() << endl;

    metadata << "******************************************" << endl;
    metadata << endl;
    metadata << endl;
}

void TRestParticleGenerator::Sample() {
    if (GetSpatialGeneratorType() == noGeneratorTypeSpecified) {
        cout << "cannot sample if no spatial generator type is specified" << endl;
        return;
    } else if (fSpatialGeneratorType == spatialGeneratorTypes::FILE) {
    } else if (fSpatialGeneratorType == spatialGeneratorTypes::VOLUME) {
    } else if (fSpatialGeneratorType == spatialGeneratorTypes::SURFACE) {
    } else if (fSpatialGeneratorType == spatialGeneratorTypes::POINT) {
    } else if (fSpatialGeneratorType == spatialGeneratorTypes::VIRTUAL_WALL) {
    } else if (fSpatialGeneratorType == spatialGeneratorTypes::VIRTUAL_BOX) {
    } else if (fSpatialGeneratorType == spatialGeneratorTypes::VIRTUAL_SPHERE) {
        Float_t radius = geometricParameterValues.at(geometricParameters::RADIUS);  // this has to be defined
        if (radius <= 0) {
            cout << "ERROR: radius needs to be defined before sampling!" << endl;
            // return;
        }
        Float_t theta = TMath::Pi() * fRandom.Rndm();
        Float_t phi = TMath::ACos(1 - 2 * fRandom.Rndm());

        TVector3 position(radius * TMath::Sin(theta) * TMath::Cos(phi),
                          radius * TMath::Sin(theta) * TMath::Sin(phi), radius * TMath::Cos(theta));
        TVector3 direction = -1 / radius * position;

        SetParticlePosition(position + fGeneratorPosition);
        SetParticleDirection(direction);
    } else if (fSpatialGeneratorType == spatialGeneratorTypes::VIRTUAL_CIRCLE_WALL) {
    } else if (fSpatialGeneratorType == spatialGeneratorTypes::VIRTUAL_CYLINDER) {
    }
    // we should never get here probably missing to list some type of generator
    cout << "WARNING: this section of code should not be reachable in `TRestParticleGenerator::Sample()`"
         << endl;
    return;
}