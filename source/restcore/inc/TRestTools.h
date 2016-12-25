///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///             
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRest.h
///
///             Dec 2016:   First concept
///                 author : Javier Galan
///_______________________________________________________________________________


#ifndef RestCore_TRestTools
#define RestCore_TRestTools

#include <iostream>
using namespace std;

#include <TString.h>
#include <TList.h>

//! This namespace defines different miscelaneous methods 

//! TODO: Write a detailed description HERE
namespace REST_Tools {

    vector <TString> GetListOfRESTLibraries(  );

    vector <TString> GetListOfPathsInEnvVariable( TString envVariable  );

    TString GetFirstPath( TString &path );

    vector <TString> GetFilesInDirectory( TString path );

    vector <TString> GetOptions( TString optionsStr  );

    TString GetFirstOption( TString &path );
}
#endif
