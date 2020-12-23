
///______________________________________________________________________________
///______________________________________________________________________________
///
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestGeant4PhysicsLists.cxx
///
///             G4 class description
///
///             Apr 2017:   First concept
///                     A metadata class to store the physics lists to be used
///                     in a REST-Geant4 simulation.
///
///                 Javier Galan
///_______________________________________________________________________________

#include "TRestTools.h"

#include "TRestGeant4PhysicsLists.h"
using namespace std;

ClassImp(TRestGeant4PhysicsLists)
    //______________________________________________________________________________
    TRestGeant4PhysicsLists::TRestGeant4PhysicsLists()
    : TRestMetadata() {
    // TRestGeant4PhysicsLists default constructor
    Initialize();
}

//______________________________________________________________________________
TRestGeant4PhysicsLists::TRestGeant4PhysicsLists(char* cfgFileName, string name) : TRestMetadata(cfgFileName) {
    Initialize();

    LoadConfigFromFile(fConfigFileName, name);

    PrintMetadata();
}

//______________________________________________________________________________
TRestGeant4PhysicsLists::~TRestGeant4PhysicsLists() {
    // TRestGeant4PhysicsLists destructor
}

void TRestGeant4PhysicsLists::Initialize() { SetSectionName(this->ClassName()); }

//______________________________________________________________________________
void TRestGeant4PhysicsLists::InitFromConfigFile() {
    this->Initialize();

    fCutForGamma = GetDblParameterWithUnits("cutForGamma", 0.01);
    fCutForElectron = GetDblParameterWithUnits("cutForElectron", 1);
    fCutForPositron = GetDblParameterWithUnits("cutForPositron", 1);
    fCutForMuon = GetDblParameterWithUnits("cutForMuon", 1);
    fCutForNeutron = GetDblParameterWithUnits("cutForNeutron", 1);
    fMinEnergyRangeProductionCuts = GetDblParameterWithUnits("minEnergyRangeProductionCuts", 1);
    fMaxEnergyRangeProductionCuts = GetDblParameterWithUnits("maxEnergyRangeProductionCuts", 1e6);

    TiXmlElement* physicsListDefinition = GetElement("physicsList");
    while (physicsListDefinition != NULL) {
        // PhysicsList name
        TString phName = GetFieldValue("name", physicsListDefinition);

        if (!PhysicsListExists(phName)) {
            cout << "REST WARNING. TRestPhysicsList : Skipping physics list : " << phName << endl;
            continue;
        }

        TString optionString = "";
        TiXmlElement* optionDefinition = GetElement("physicsList", physicsListDefinition);
        while (optionDefinition != NULL) {
            TString optionName = GetFieldValue("name", optionDefinition);
            TString optionValue = GetFieldValue("value", optionDefinition);

            if (optionString != "") optionString += ":";

            optionString += optionName + ":" + optionValue;
        }

        fPhysicsLists.push_back(phName);
        fPhysicsListOptions.push_back(optionString);
        physicsListDefinition = GetNextElement(physicsListDefinition);
    }
}

Int_t TRestGeant4PhysicsLists::FindPhysicsList(TString phName) {
    if (!PhysicsListExists(phName)) return -1;

    for (unsigned int n = 0; n < fPhysicsLists.size(); n++)
        if (fPhysicsLists[n] == phName) return (Int_t)n;

    return -1;
}

TString TRestGeant4PhysicsLists::GetPhysicsListOptionString(TString phName) {
    Int_t index = FindPhysicsList(phName);

    if (index == -1) return "";

    return fPhysicsListOptions[index];
}

TString TRestGeant4PhysicsLists::GetPhysicsListOptionValue(TString phName, TString option) {
    vector<string> optList = TRestTools::GetOptions((string)GetPhysicsListOptionString(phName));

    for (unsigned int n = 0; n < optList.size(); n = n + 2)
        if (optList[n] == option) return optList[n + 1];

    return "NotDefined";
}

Bool_t TRestGeant4PhysicsLists::PhysicsListExists(TString phName) {
    if (phName == "G4DecayPhysics") return true;
    if (phName == "G4RadioactiveDecayPhysics") return true;
    if (phName == "G4RadioactiveDecay") return true;
    if (phName == "G4EmLivermorePhysics") return true;
    if (phName == "G4EmPenelopePhysics") return true;
    if (phName == "G4EmStandardPhysics_option3") return true;
    if (phName == "G4HadronElasticPhysicsHP") return true;
    if (phName == "G4IonBinaryCascadePhysics") return true;
    if (phName == "G4HadronPhysicsQGSP_BIC_HP") return true;
    if (phName == "G4NeutronTrackingCut") return true;
    if (phName == "G4EmExtraPhysics") return true;

    cout << endl;
    cout << "REST Warning. TRestGeant4PhysicsLists::PhysicsListExists." << endl;
    cout << "Physics list " << phName << " does NOT exist in TRestGeant4PhysicsLists." << endl;
    cout << endl;

    return false;
}

void TRestGeant4PhysicsLists::PrintMetadata() {
    TRestMetadata::PrintMetadata();

    metadata << "Cut for electrons : " << fCutForElectron << " mm" << endl;
    metadata << "Cut for positrons : " << fCutForPositron << " mm" << endl;
    metadata << "Cut for gammas : " << fCutForGamma << " mm" << endl;
    metadata << "Cut for muons : " << fCutForMuon << " mm" << endl;
    metadata << "Cut for neutrons : " << fCutForNeutron << " mm" << endl;
    metadata << "Min Energy for particle production: " << fMinEnergyRangeProductionCuts << " keV" << endl;
    metadata << "Max Energy for particle production: " << fMaxEnergyRangeProductionCuts << " keV" << endl;
    metadata << "---------------------------------------" << endl;
    for (unsigned int n = 0; n < fPhysicsLists.size(); n++) {
        metadata << "Physics list " << n << " : " << fPhysicsLists[n] << endl;
        vector<string> optList = TRestTools::GetOptions((string)fPhysicsListOptions[n]);
        for (unsigned int m = 0; m < optList.size(); m = m + 2)
            metadata << " - Option " << m / 2 << " : " << optList[m] << " = " << optList[m + 1] << endl;
    }
    metadata << "******************************************" << endl;
    metadata << endl;
    metadata << endl;
}
