///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///             
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestTemplateMetadata.cxx
///
///             Template class to be used to design REST metadata classes to be 
///             inherited from TRestMetadata
///             How to use: replace TRestTemplateMetadata by your class name, 
///             fill the required functions following instructions and add all
///             needed additional members and funcionality
///
///             jun 2014:   First concept
///                 Created as part of the conceptualization of existing REST 
///                 software.
///                 Igor G. Irastorza
///_______________________________________________________________________________


#include "TRestTemplateMetadata.h"

ClassImp(TRestTemplateMetadata)
//______________________________________________________________________________
TRestTemplateMetadata::TRestTemplateMetadata() : TRestMetadata()
{
   // TRestTemplateMetadata default constructor
   fSectionName = "sectionName";
}


//______________________________________________________________________________
TRestTemplateMetadata::TRestTemplateMetadata(char *cfgFileName) : TRestMetadata (cfgFileName)
{
   fSectionName = "sectionName";
   // TRestTemplateMetadata constructor loading data from config file
	//(call to InitFromConfigFile)
}

//______________________________________________________________________________
TRestTemplateMetadata::~TRestTemplateMetadata()
{
   // TRestTemplateMetadata destructor
}

//______________________________________________________________________________
void TRestTemplateMetadata::InitFromConfigFile()
{
   // Initialize the metadata members from a configfile
}
