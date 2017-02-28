///______________________________________________________________________________
///______________________________________________________________________________
///             
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestTools.h
///
///             Dec 2016:   First concept
///                 author: Javier Galan
///_______________________________________________________________________________

#ifndef RestCore_TRestTools
#define RestCore_TRestTools

#include <iostream>
#include <TString.h>
#include <TList.h>

class TRestTools:public TObject {

    public:
        static std::vector <TString> GetListOfRESTLibraries(  );

        static std::vector <TString> GetListOfPathsInEnvVariable( TString envVariable  );

        static TString GetFirstPath( TString &path );

        static std::vector <TString> GetRESTLibrariesInDirectory( TString path );

        static std::vector <TString> GetOptions( TString optionsStr  );

        static TString GetFirstOption( TString &path );

        ClassDef(TRestTools, 1); // Rest tools class 
};
#endif
