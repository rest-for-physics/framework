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
using namespace std;


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
        string GetFieldValue( string fieldName, string definition );

        string GetKEYStructure( string keyName );
        string GetKEYStructure( string keyName, string buffer );
        string GetKEYStructure( string keyName, size_t &fromPosition );
        string GetKEYStructure( string keyName, size_t &fromPosition, string buffer );

        string GetKEYDefinition( string keyName );
        string GetKEYDefinition( string keyName, string buffer );
        string GetKEYDefinition( string keyName, size_t &fromPosition, string buffer );
        string GetKEYDefinition( string keyName, size_t &fromPosition );

	string GetParameter( string parName, TString defaultValue = PARAMETER_NOT_FOUND_STR );
        string GetParameter( string parName, size_t &pos, string inputString );
        string GetMyParameter( string &value, size_t &pos );

        string fConfigFileName;		// string with the name of the config file
        string fSectionName;        // section name given in the constructor of TRestSpecificMetadata

        // This method must be implemented in the derived class to fill the class fields with a given section
        // It should be a pure virtual method. But there are problems INPUT/OUTPUT in ROOT
        virtual void InitFromConfigFile( ) = 0; 
 //       { cout << "WARNING Please Implement the InitFromConfigFile( ) method in the TRestMetadata derived class!!!!!" << endl; };

        virtual void Initialize() = 0;// { cout << __PRETTY_FUNCTION__ << endl; };
        Int_t LoadSectionMetadata( string section, string cfgFileName );

        Int_t LoadConfig( string section, string cfgFileName )
        {
            Int_t result = LoadSectionMetadata( section, cfgFileName );
            if( result == 0 ) InitFromConfigFile();
            return result;
        }

    private:

        ifstream configFile;    //! pointer to config file to load metadata
        string configBuffer;

        string fConfigFilePath;		//  string with the path to the config file
        
        TString fDataPath;
        REST_Verbose_Level fVerboseLevel;
        
        void SetDefaultConfigFilePath();

        Int_t CheckConfigFile( );

        string GetFieldValue( string fieldName, size_t fromPosition );
        string GetFieldFromKEY( string parName, string key );

        string EvaluateExpression( string exp );

        string ExpandForLoops( string buffer );
        string ReplaceMathematicalExpressions( string buffer );
        string ExtractLoopStructure( string in, size_t pos );

        string GetSectionByNameFromFile( string nref, string fref );
        Int_t FindSection( string buffer, size_t startPos = 0 );

    public:
        void CheckSection( );

        void SetConfigFilePath(const char *configFilePath);

        TString GetDataPath( ) { return fDataPath; }

        void SetDataPath( TString path ) { fDataPath = path; }

        REST_Verbose_Level GetVerboseLevel( ) { return fVerboseLevel; }

        Int_t FindEndSection( Int_t initPos );

        TString GetMainDataPath() { return fDataPath; }
        
        // String helper classes. Declared static to be able to access them without having to instantiate TRestMetadata.
        // Probably they should be in a different class (i.e. TRestStrings)
        static Int_t isANumber( string in );
        Int_t isAExpression( string in );
        static string trim(string str);
        static Double_t StringToDouble( string in );
        static Int_t StringToInteger( string in );
        static TVector3 StringTo3DVector( string in );
        static TVector2 StringTo2DVector( string in );
        static string RemoveWhiteSpaces( string in );
        static string Replace( string in, string thisString, string byThisString, size_t fromPosition );
        static Int_t Count( string s, string sbstring);
        static bool fileExists(const std::string& filename);


        //////////////////////////////////////////////////

        void virtual PrintMetadata() = 0;

        //Constructor
        TRestMetadata();
        TRestMetadata( char *cfgFileName);
        //Destructor
        ~TRestMetadata();

        /*
           string SearchString(const char *name);
           Int_t SearchInt(const char *name);
           Double_t SearchDouble(const char *name);
           */

        ClassDef(TRestMetadata, 1); // Rest metadata Base class 
};
#endif
