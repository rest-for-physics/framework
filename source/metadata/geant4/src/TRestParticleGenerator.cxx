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
TRestParticleGenerator::TRestParticleGenerator(const string& configFileName, const string& name = "")
    : TRestMetadata(configFileName.c_str()) {
    Initialize();
    LoadConfigFromFile(fConfigFileName, name);
    PrintMetadata();
}

void TRestParticleGenerator::InitFromConfigFile() {
    /*
    fCutForGamma = GetDblParameterWithUnits("cutForGamma", 0.01);
    fCutForElectron = GetDblParameterWithUnits("cutForElectron", 1);
    fCutForPositron = GetDblParameterWithUnits("cutForPositron", 1);
    fCutForMuon = GetDblParameterWithUnits("cutForMuon", 1);
    fCutForNeutron = GetDblParameterWithUnits("cutForNeutron", 1);
    fMinEnergyRangeProductionCuts = GetDblParameterWithUnits("minEnergyRangeProductionCuts", 1);
    fMaxEnergyRangeProductionCuts = GetDblParameterWithUnits("maxEnergyRangeProductionCuts", 1e6);
    size_t position = 0;
    string physicsListString;

    while ((physicsListString = GetKEYStructure("physicsList", position)) != "NotFound") {
        // PhysicsList name
        TString phName = GetFieldValue("name", GetKEYDefinition("physicsList", physicsListString));

        if (!PhysicsListExists(phName)) {
            cout << "REST WARNING. TRestPhysicsList : Skipping physics list : " << phName << endl;
            continue;
        }
        size_t pos = 0;
        string optionDefinition;

        TString optionString = "";
        while ((optionDefinition = GetKEYDefinition("option", pos, physicsListString)) != "") {
            TString optionName = GetFieldValue("name", optionDefinition);
            TString optionValue = GetFieldValue("value", optionDefinition);

            if (optionString != "") optionString += ":";

            optionString += optionName + ":" + optionValue;
        }

        fPhysicsLists.push_back(phName);
        fPhysicsListOptions.push_back(optionString);
    }
    */
}