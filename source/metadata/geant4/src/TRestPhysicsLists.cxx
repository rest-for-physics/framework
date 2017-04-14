
///______________________________________________________________________________
///______________________________________________________________________________
///             
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestPhysicsLists.cxx
///
///             G4 class description
///
///             Apr 2017:   First concept
///                     A metadata class to store the physics lists to be used in a REST-Geant4 simulation.
///                 
///                 Javier Galan
///_______________________________________________________________________________


#include "TRestTools.h"


#include "TRestPhysicsLists.h"
using namespace std;

ClassImp(TRestPhysicsLists)
//______________________________________________________________________________
TRestPhysicsLists::TRestPhysicsLists() : TRestMetadata()
{

   // TRestPhysicsLists default constructor
   Initialize();
}


//______________________________________________________________________________
TRestPhysicsLists::TRestPhysicsLists( char *cfgFileName, string name ) : TRestMetadata (cfgFileName)
{

    Initialize();

    LoadConfigFromFile( fConfigFileName, name );

    PrintMetadata();
}

//______________________________________________________________________________
TRestPhysicsLists::~TRestPhysicsLists()
{
    // TRestPhysicsLists destructor
}

void TRestPhysicsLists::Initialize()
{
    SetSectionName( this->ClassName() );
}

//______________________________________________________________________________
void TRestPhysicsLists::InitFromConfigFile()
{
    this->Initialize();

    fCutForGamma = GetDblParameterWithUnits( "cutForGamma", 0.01  );
    fCutForElectron = GetDblParameterWithUnits( "cutForElectron", 1  );
    fCutForPositron = GetDblParameterWithUnits( "cutForPositron", 1  ); 

    size_t position = 0;
    string physicsListString;

    while( ( physicsListString = GetKEYStructure( "physicsList", position ) ) != "NotFound" )
    {
        // PhysicsList name 
        TString phName = GetFieldValue( "name", GetKEYDefinition( "physicsList", physicsListString ) );

        if( !PhysicsListExists( phName ) ) 
        {
            cout << "REST WARNING. TRestPhysicsList : Skipping physics list : " << phName << endl;
            continue;
        }
        size_t pos = 0;
        string optionDefinition;

        TString optionString = "";
        while( ( optionDefinition = GetKEYDefinition( "option", pos, physicsListString ) ) != "" )
        {
            TString optionName = GetFieldValue( "name", optionDefinition );
            TString optionValue = GetFieldValue( "value", optionDefinition );

            if( optionString != "" )
                optionString += ":";

            optionString += optionName + ":" + optionValue;
        }

        fPhysicsLists.push_back( phName );
        fPhysicsListOptions.push_back( optionString );
    }
}

Int_t TRestPhysicsLists::FindPhysicsList( TString phName )
{
    if( !PhysicsListExists( phName ) ) return -1;

    for( unsigned int n = 0; n < fPhysicsLists.size(); n++ )
        if( fPhysicsLists[n] == phName ) return (Int_t) n;


    return -1;
}

TString TRestPhysicsLists::GetPhysicsListOptionString( TString phName )
{
    Int_t index = FindPhysicsList( phName );

    if( index == -1 ) return "";

    return fPhysicsListOptions[index];
}

TString TRestPhysicsLists::GetPhysicsListOptionValue( TString phName, TString option )
{
    
    vector <TString> optList = TRestTools::GetOptions( GetPhysicsListOptionString( phName ) );

    for( unsigned int n = 0; n < optList.size(); n = n + 2 )
        if( optList[n] == phName ) return optList[n+1];

    return "NotDefined";
}

Bool_t TRestPhysicsLists::PhysicsListExists( TString phName )
{
    if( phName == "G4DecayPhysics" ) return true;
    if( phName == "G4RadioactiveDecayPhysics" ) return true;
    if( phName == "G4RadioactiveDecay" ) return true;
    if( phName == "G4EmLivermorePhysics" ) return true;
    if( phName == "G4EmPenelopePhysics" ) return true;
    if( phName == "G4EmStandardPhysics_option3" ) return true;
    if( phName == "G4HadronElasticPhysicsHP" ) return true;
    if( phName == "G4IonBinaryCascadePhysics" ) return true;
    if( phName == "G4HadronPhysicsQGSP_BIC_HP" ) return true;
    if( phName == "G4NeutronTrackingCut" ) return true;
    if( phName == "G4EmExtraPhysics" ) return true;

    cout << endl;
    cout << "REST Warning. TRestPhysicsLists::PhysicsListExists." << endl;
    cout << "Physics list " << phName << " does NOT exist in TRestPhysicsLists." << endl;
    cout << endl;

    return false;
}

void TRestPhysicsLists::PrintMetadata( )
{
    cout << endl;
    cout << "+++++++++++++++++++++++++++++++++++++++++++++" << endl;
    cout << "TRestPhysicsLists content" << endl;
    cout << "+++++++++++++++++++++++++++++++++++++++++++++" << endl;
    cout << "Config file : " << fConfigFileName << endl;
    cout << "Section name : " << fSectionName << endl;        // section name given in the constructor of TRestSpecificMetadata
    cout << "---------------------------------------" << endl;
    cout << "Name : " << GetName() << endl;
    cout << "Title : " << GetTitle() << endl;
    cout << "---------------------------------------" << endl;
    cout << "Cut for electrons : " << fCutForElectron << " mm" << endl;
    cout << "Cut for positrons : " << fCutForPositron << " mm" << endl;
    cout << "Cut for gammas : " << fCutForGamma << " mm" << endl;
    cout << "---------------------------------------" << endl;
    for( unsigned int n = 0; n < fPhysicsLists.size(); n++ )
    {
        cout << "Physics list " << n << " : " << fPhysicsLists[n] << endl;
        vector <TString> optList = TRestTools::GetOptions( fPhysicsListOptions[n] );
        for( unsigned int m = 0; m < optList.size(); m = m + 2 )
            cout << " - Option " << m/2 << " : " << optList[m] << " = " << optList[m+1] << endl;
    }
    cout << "******************************************" << endl;
    cout << endl;
}
