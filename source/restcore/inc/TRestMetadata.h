/*************************************************************************
 * This file is part of the REST software framework.                     *
 *                                                                       *
 * Copyright (C) 2016 GIFNA/TREX (University of Zaragoza)                *
 * For more information see http://gifna.unizar.es/trex                  *
 *                                                                       *
 * REST is free software: you can redistribute it and/or modify          *
 * it under the terms of the GNU General Public License as published by  *
 * the Free Software Foundation, either version 3 of the License, or     *
 * (at your option) any later version.                                   *
 *                                                                       *
 * REST is distributed in the hope that it will be useful,               *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the          *
 * GNU General Public License for more details.                          *
 *                                                                       *
 * You should have a copy of the GNU General Public License along with   *
 * REST in $REST_PATH/LICENSE.                                           *
 * If not, see http://www.gnu.org/licenses/.                             *
 * For the list of contributors see $REST_PATH/CREDITS.                  *
 *************************************************************************/

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

#include <TRestSystemOfUnits.h>

const int PARAMETER_NOT_FOUND_INT = -99999999; 
const double PARAMETER_NOT_FOUND_DBL = -99999999;
const TString PARAMETER_NOT_FOUND_STR = "-99999999";

enum REST_Verbose_Level {REST_Silent, REST_Warning, REST_Info, REST_Debug, REST_Extreme };

//! A base class for any REST metadata class
class TRestMetadata:public TNamed {

    protected:

        std::string fConfigFileName;	///< Name of the config file associated to this metadata 
        std::string fSectionName;       ///< Section name given in the constructor of the derived metadata class

        std::string GetFieldValue( std::string fieldName, std::string definition, size_t fromPosition = 0 );

        Double_t GetDblFieldValueWithUnits( string fieldName, string definition, size_t fromPosition = 0 );
        TVector2 Get2DVectorFieldValueWithUnits( string fieldName, string definition, size_t fromPosition = 0 );
        TVector3 Get3DVectorFieldValueWithUnits( string fieldName, string definition, size_t fromPosition = 0 );

        std::string GetKEYStructure( std::string keyName );
        std::string GetKEYStructure( std::string keyName, std::string buffer );
        std::string GetKEYStructure( std::string keyName, size_t &fromPosition );
        std::string GetKEYStructure( std::string keyName, size_t &fromPosition, std::string buffer );

        std::string GetKEYDefinition( std::string keyName );
        std::string GetKEYDefinition( std::string keyName, std::string buffer );
        std::string GetKEYDefinition( std::string keyName, size_t &fromPosition, std::string buffer );
        std::string GetKEYDefinition( std::string keyName, size_t &fromPosition );

        std::string GetParameter( std::string parName, size_t &pos, std::string inputString );
        Double_t GetDblParameterWithUnits( std::string parName, size_t &pos, std::string inputString );
        TVector2 Get2DVectorParameterWithUnits( std::string parName, size_t &pos, std::string inputString );
        TVector3 Get3DVectorParameterWithUnits( std::string parName, size_t &pos, std::string inputString );

        vector <string> GetObservablesList( );
        vector <string> GetObservableDescriptionsList( );

        std::string GetMyParameter( std::string &value, size_t &pos );

        std::string GetSectionName( ) { return fSectionName; }

        void SetSectionName( std::string sName ) { fSectionName = sName; }

        /// This method must be implemented in the derived class to fill specific metadata members using a RML file.
        virtual void InitFromConfigFile( ) = 0;

        /// This method must be implemented in the derived class to initialize/clear the metadata members from the derived class.
        virtual void Initialize() = 0;

        Int_t LoadSectionMetadata( std::string section, std::string cfgFileName, string name="" );
        Int_t LoadConfigFromFile( string cfgFileName, std::string name );
        Int_t LoadConfigFromFile( std::string cfgFileName );

        std::string configBuffer;   ///< The buffer where the corresponding metadata section is stored

    private:

        ifstream configFile;        //!< Pointer to config file to load metadata

        std::string fConfigFilePath;	//!< Path to the config file associated with this metadata
        
        TString fDataPath;              //!< REST Data path
        REST_Verbose_Level fVerboseLevel;   //!< Verbose level used to print debug info

#ifndef __CINT__
        Bool_t fStore;  //!< This variable is used to determine if the metadata structure should be stored in the ROOT file.

        TString fGasDataPath; //!< The path where the gas pre-generated files are stored.
#endif
        
        void SetDefaultConfigFilePath();

        void SetConfigFile( std::string cfgFileName );

        Int_t CheckConfigFile( );

        void SetEnvVariable( size_t &pos );
        std::string GetFieldValue( std::string fieldName, size_t fromPosition );
        std::string GetUnits( string definition, size_t fromPosition );
        std::string GetFieldFromKEY( std::string parName, std::string key );

        std::string EvaluateExpression( std::string exp );

        std::string ExpandForLoops( std::string buffer );
        std::string ReplaceMathematicalExpressions( std::string buffer );
	std::string ReplaceIncludeDefinitions( const string buffer );
        std::string ReplaceEnvironmentalVariables( const std::string buffer );
        std::string ExtractLoopStructure( std::string in, size_t pos );
        std::string RemoveComments( string in );

        std::string GetSectionByNameFromFile( std::string nref, std::string fref );
        Int_t FindSection( std::string buffer, size_t startPos = 0 );

    public:
        /// Returns a string with the path used for data storage
        TString GetDataPath( ) { return fDataPath; }
        
        /// Returns a string with the path used for pre-generated gas files
        TString GetGasDataPath( ) { return fGasDataPath; }

        /// Sets the path that will be used for data storage
        void SetDataPath( TString path ) { fDataPath = path; }
        
        /// Sets the path that will be used for pre-generated gas files
        void SetGasDataPath( TString path ) { fGasDataPath = path; }

        /// Gets the verbose level used to dump on screen different levels of information
        REST_Verbose_Level GetVerboseLevel( ) { return fVerboseLevel; }
        
        TString GetVerboseLevelString( );

        /// Gets a string with the path used for data storage
        TString GetMainDataPath() { return fDataPath; }

        Int_t FindEndSection( Int_t initPos );

        void CheckSection( );

        void SetConfigFilePath(const char *configFilePath);

        std::string GetParameter( std::string parName, TString defaultValue = PARAMETER_NOT_FOUND_STR );
        Double_t GetDblParameterWithUnits( std::string parName, Double_t defaultValue = PARAMETER_NOT_FOUND_DBL );
        TVector2 Get2DVectorParameterWithUnits( string parName, TVector2 defaultValue = TVector2(-1,-1) );
        TVector3 Get3DVectorParameterWithUnits( string parName, TVector3 defaultValue = TVector3( -1, -1, -1) );

        void PrintTimeStamp( Double_t timeStamp );
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
        static bool fileExists( const std::string& filename );
        static bool isRootFile( const std::string& filename ); 
        static bool isPathWritable( const std::string& path );

        /// Calling this method will ask the user to press a key to continue
        static void GetChar(){ cout << "Press a KEY to continue ..." << endl; getchar(); }

        /// If this method is called the metadata information will **not** be stored in disk. I/O is handled by TRestRun.
        void DoNotStore( ) { fStore = false; }
        
        /// If this method is called the metadata information will be stored in disk. This is the default behaviour.
        Bool_t Store( ) { return fStore; }


        //////////////////////////////////////////////////

        /// Must be imlemented in the derived metadata class to print out specific metadata information.
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
