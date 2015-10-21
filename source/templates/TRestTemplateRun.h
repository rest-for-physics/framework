///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///             
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestTemplateRun.h
///
///             G4 class to be used to design REST metadata classes to be 
///             inherited from TRestMetadata
///             How to use: replace TRestTemplateRun by your class name, 
///             fill the required functions following instructions and add all
///             needed additional members and funcionality
///
///             aug 2015:   First concept. Javier Galan
//  
///_______________________________________________________________________________


#ifndef RestCore_TRestTemplateRun
#define RestCore_TRestTemplateRun

#include <stdio.h>
#include <stdlib.h>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <fstream>
#include <vector>
using namespace std;

#include <TRestRun.h>

class TRestTemplateRun:public TRestRun {
    private:

        //add here the members of your event process
        Int_t fDummyMember1;			// change this by your members

        // The members must be filled here using the methods from metadata
        void InitFromConfigFile();

        // The specific run must define its output filename
        void SetRunFilenameAndIndex();

        // Objects to be stored independently inside the ROOT file should be written here
        void Save();

    public:
        // Define here setters/getters to access the private members of this class

        void PrintSpecificInfo( );

        //Constructors
        TRestTemplateRun();
        TRestTemplateRun( char *cfgFileName);
        //Destructor
        ~TRestTemplateRun();

        void Initialize();

        ClassDef(TRestTemplateRun, 1); 
};
#endif
