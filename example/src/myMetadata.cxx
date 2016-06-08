///______________________________________________________________________________
///______________________________________________________________________________
///             
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             myMetadata.cxx
///
///             june 2016
///                 Javier Galan
///_______________________________________________________________________________


#include "myMetadata.h"
using namespace std;

ClassImp(myMetadata)
//______________________________________________________________________________
myMetadata::myMetadata() : TRestMetadata()
{

   // myMetadata default constructor
   Initialize();
}


//______________________________________________________________________________
myMetadata::myMetadata( const char *cfgFileName, string name ) : TRestMetadata (cfgFileName)
{

    Initialize();

    LoadConfigFromFile( fConfigFileName, name );

    PrintMetadata();
}


//______________________________________________________________________________
myMetadata::~myMetadata()
{
    // myMetadata destructor
}

void myMetadata::Initialize()
{
	SetName( "myMetadataSection" );

    fDummy = -1;
}

//______________________________________________________________________________
void myMetadata::InitFromConfigFile()
{
    this->Initialize();

    // Initialize the metadata members from a configfile
    fDummy = StringToInteger( GetParameter( "dummy" ) );

}

void myMetadata::PrintMetadata( )
{

    cout << endl;
    cout << "+++++++++++++++++++++++++++++++++++++++++++++" << endl;
    cout << "myMetadata content" << endl;
    cout << "+++++++++++++++++++++++++++++++++++++++++++++" << endl;
	cout << " Dummy number : " << fDummy << endl;
	cout << " --------------------------------------------" << endl;

}

