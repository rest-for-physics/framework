///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///             
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             myManager.cxx
///
///             june 2016    Javier Galan
///_______________________________________________________________________________


#include "myManager.h"

// We include our process inside our manager so that it can be read inside the manager section.
// We must load it in LoadExternalProcess
#include <myProcess.h>

// We want to load myMetadata inside TRestRun defined in TRestManager. 
// We will implement the read in myMetadata
#include <myMetadata.h>

using namespace std;

const int debug = 0;

ClassImp(myManager)
    //______________________________________________________________________________
myManager::myManager()
{
    Initialize();

}

myManager::myManager( const char *cfgFileName, const char *name) : TRestManager (cfgFileName, name)
{
    Initialize();
}

// Initialize here the members of myManager
void myManager::Initialize()
{
    TString metadataCfgFile = GetParameter( "myMetadataFile" );

    size_t position = 0;
    string myMetadataString = GetKEYDefinition( "myMetadata", position );
    
    TString name = GetFieldValue( "name", myMetadataString );

    myMetadata *md = new myMetadata( metadataCfgFile.Data() , (string) name );

    this->AddMetadata( md );
}


//______________________________________________________________________________
myManager::~myManager()
{
}

// We need to add our processes to LoadExternalProcess method
void myManager::LoadExternalProcess( TString processType, std::string processesCfgFile, std::string processName ) 
{
    if( processType == "myProcess" )
        this->AddProcess( new myProcess( ), processesCfgFile, processName );
}

