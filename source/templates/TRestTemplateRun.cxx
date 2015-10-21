///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///             
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestTemplateRun.cxx
///
///             G4 class description
///
///             jul 2015:   First concept
///                 Created as part of the conceptualization of existing REST 
///                 software.
///                 Javier Galan
///_______________________________________________________________________________


#include "TRestTemplateRun.h"

ClassImp(TRestTemplateRun)
//______________________________________________________________________________
TRestTemplateRun::TRestTemplateRun() : TRestRun()
{

   // TRestTemplateRun default constructor
   Initialize();
}


//______________________________________________________________________________
TRestTemplateRun::TRestTemplateRun( char *cfgFileName) : TRestRun (cfgFileName)
{

    Initialize();

    LoadSectionMetadata( "restTemplateRun", fConfigFileName ); 

    PrintInfo();
}


//______________________________________________________________________________
TRestTemplateRun::~TRestTemplateRun()
{
    if( fOutputFile != NULL )
    {
        fOutputFile->cd();
        this->Write();
    }
   // TRestTemplateRun destructor
}

void TRestTemplateRun::Initialize()
{
    TRestRun::Initialize();

    fRunClassName = "TRestG4Run";

    cout << __PRETTY_FUNCTION__ << endl;


    // This class members must be initialized here
}

void TRestTemplateRun::Save( )
{

    // Write here any object/class member of this class that should be stored independently inside the ROOT file
    // if ( fRestObject != NULL ) fRestObject->Write( fRestObject->GetName() );
}

//______________________________________________________________________________
void TRestTemplateRun::InitFromConfigFile()
{
    cout << __PRETTY_FUNCTION__ << endl;


    // Initialize the metadata members from a configfile
    // fDummyMember1 = GetParameter( "dummyParameter" );

    // should return sucess or fail

}

void TRestTemplateRun::SetRunFilenameAndIndex()
{ 
    fRunFilename = "Run_Template.root";

    // This code is reserved to define the filename output

}

void TRestTemplateRun::PrintMetadata( )
{
        cout << endl;
        cout << "+++++++++++++++++++++++++++++++++++++++++++++" << endl;
        cout << "TRestTemplateRun content" << endl;
        cout << "+++++++++++++++++++++++++++++++++++++++++++++" << endl;
}

