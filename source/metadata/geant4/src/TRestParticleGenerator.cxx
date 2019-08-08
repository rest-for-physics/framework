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
    string generatorString = GetKEYStructure("generator");
    string generatorDefinition = GetKEYDefinition("generator", generatorString);

    // we first define the spatial generator type (box, virtualWall etc.)
    string generatorType = GetFieldValue("type", generatorDefinition);
    SetSpatialGeneratorType(generatorType);

    // we run through all possible parameters
    // TODO: this should be changed in TRestMetadata. We should not use hardcoded values like this.
    string parameterNotFound = "Not defined";
    // 'from' (volume)
    string fromVolume = GetFieldValue("from", generatorDefinition);
    if (fromVolume != parameterNotFound) {
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
    if (positionString != parameterNotFound) {
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
    if (rotationString != parameterNotFound && normalVectorString != parameterNotFound) {
        cout << "'rotation' and 'normal' cannot be defined at the same time" << endl;
        exit(0);
    } else if (rotationString != parameterNotFound || normalVectorString != parameterNotFound) {
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
    if (normalVectorString != parameterNotFound) {
        TVector3 normalVector = Get3DVectorFieldValueWithUnits("normal", generatorDefinition);
        SetGeneratorNormal(normalVector);
    } else if (rotationString != parameterNotFound) {
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
    metadata << "******************************************" << endl;
    metadata << endl;
    metadata << endl;
}