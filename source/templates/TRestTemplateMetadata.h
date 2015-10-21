///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///             
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestTemplateMetadata.h
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


#ifndef RestCore_TRestTemplateMetadata
#define RestCore_TRestTemplateMetadata

#include <stdio.h>
#include <stdlib.h>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <fstream>
using namespace std;

#include "TRestMetadata.h"
#include "TString.h"

class TRestTemplateMetadata:public TRestMetadata {
 protected:
   //add here the members of your event process
   Int_t fDummyMember1;			// change this by your members

 public:
   
   void InitFromConfigFile();

   //Constructors
	TRestTemplateMetadata();
	TRestTemplateMetadata(char *cfgFileName);
   //Destructor
   ~TRestTemplateMetadata();
	
    ClassDef(TRestTemplateMetadata, 1); // Template for REST metadata classes
};
#endif
