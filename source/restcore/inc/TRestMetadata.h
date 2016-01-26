///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///             
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestMetadata.h
///
///             Base class from which to inherit all other REST metadata classes (config, etc...) 
///             TRestMetadata controls the access to config files.
///
///             jun 2014:   First concept
///                 Created as part of the conceptualization of existing REST 
///                 software.
///                 Igor G. Irastorza
//              Jul 2015:    Implementation  
//                  J. Galan
///_______________________________________________________________________________


#ifndef RestCore_TRestMetadata
#define RestCore_TRestMetadata

#include <stdio.h>
#include <stdlib.h>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <fstream>
#include <sstream> 
#include <algorithm> 
#include <sys/stat.h>


#include <TFormula.h>
#include <TVector2.h>
#include <TVector3.h>
#include <TNamed.h>
#include "string.h"

const int PARAMETER_NOT_FOUND_INT = -99999999;
const double PARAMETER_NOT_FOUND_DBL = -99999999;
const TString PARAMETER_NOT_FOUND_STR = "-99999999";

enum REST_Verbose_Level {REST_Silent, REST_Warning, REST_Info, REST_Debug };


class TRestMetadata:public TNamed {

    protected:
        std::string GetFieldValue( std::string fieldName, std::string definition );

        std::string GetKEYStructure( std::string keyName );
        std::string GetKEYStructure( std::string keyName, std::string buffer );
        std::string GetKEYStructure( std::string keyName, size_t &fromPosition );
        std::string GetKEYStructure( std::string keyName, size_t &fromPosition, std::string buffer );

        std::string GetKEYDefinition( std::string keyName );
        std::string GetKEYDefinition( std::string keyName, std::string buffer );
        std::string GetKEYDefinition( std::string keyName, size_t &fromPosition, std::string buffer );
        std::string GetKEYDefinition( std::string keyName, size_t &fromPosition );

        std::string GetParameter( std::string parName, size_t &pos, std::string inputString );
        std::string GetMyParameter( std::string &value, size_t &pos );

        std::string fConfigFileName;		// std::string with the name of the config file
        std::string fSectionName;        // section name given in the constructor of TRestSpecificMetadata

        // This method must be implemented in the derived class to fill the class fields with a given section
        // It should be a pure virtual method. But there are problems INPUT/OUTPUT in ROOT
        virtual void InitFromConfigFile( ) = 0; 
 //       { cout << "WARNING Please Implement the InitFromConfigFile( ) method in the TRestMetadata derived class!!!!!" << endl; };

        virtual void Initialize() = 0;// { cout << __PRETTY_FUNCTION__ << endl; };
        Int_t LoadSectionMetadata( std::string section, std::string cfgFileName );

        Int_t LoadConfigFromFile( std::string cfgFileName );

    private:

        ifstream configFile;    //! pointer to config file to load metadata
        std::string configBuffer;

        std::string fConfigFilePath;		//  std::string with the path to the config file
        
        TString fDataPath;
        REST_Verbose_Level fVerboseLevel;
        
        void SetDefaultConfigFilePath();

        Int_t CheckConfigFile( );

        std::string GetFieldValue( std::string fieldName, size_t fromPosition );
        std::string GetFieldFromKEY( std::string parName, std::string key );

        std::string EvaluateExpression( std::string exp );

        std::string ExpandForLoops( std::string buffer );
        std::string ReplaceMathematicalExpressions( std::string buffer );
        std::string ExtractLoopStructure( std::string in, size_t pos );

        std::string GetSectionByNameFromFile( std::string nref, std::string fref );
        Int_t FindSection( std::string buffer, size_t startPos = 0 );

    public:
        void CheckSection( );

        void SetConfigFilePath(const char *configFilePath);

        TString GetDataPath( ) { return fDataPath; }

        void SetDataPath( TString path ) { fDataPath = path; }

        REST_Verbose_Level GetVerboseLevel( ) { return fVerboseLevel; }

        Int_t FindEndSection( Int_t initPos );

        TString GetMainDataPath() { return fDataPath; }

        std::string GetParameter( std::string parName, TString defaultValue = PARAMETER_NOT_FOUND_STR );

        void PrintConfigBuffer( );
        
        // String helper classes. Declared static to be able to access them without having to instantiate TRestMetadata.
        // Probably they should be in a different class (i.e. TRestStrings)
        static Int_t isANumber( std::string in );
        Int_t isAExpression( std::string in );
        static std::string trim(std::string str);
        static Double_t StringToDouble( std::string in );
        static Int_t StringToInteger( std::string in );
        static TVector3 StringTo3DVector( std::string in );
        static TVector2 StringTo2DVector( std::string in );
        static std::string RemoveWhiteSpaces( std::string in );
        static std::string Replace( std::string in, std::string thisString, std::string byThisString, size_t fromPosition );
        static Int_t Count( std::string s, std::string sbstring);
        static bool fileExists(const std::string& filename);


        //////////////////////////////////////////////////

        void virtual PrintMetadata() = 0;

        //Constructor
        TRestMetadata();
        TRestMetadata( const char *cfgFileName);
        //Destructor
        ~TRestMetadata();

        /*
           std::string SearchString(const char *name);
           Int_t SearchInt(const char *name);
           Double_t SearchDouble(const char *name);
           */

        ClassDef(TRestMetadata, 1); // Rest metadata Base class 
};
#endif
