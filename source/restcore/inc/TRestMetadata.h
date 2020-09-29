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

#define TIXML_USE_STL
#include <mutex>
#include <thread>

#include "TApplication.h"
#include "TClass.h"
#include "TRestDataBase.h"
#include "TRestDetector.h"
#include "TRestPhysics.h"
#include "TRestReflector.h"
#include "TRestStringHelper.h"
#include "TRestStringOutput.h"
#include "TRestSystemOfUnits.h"
#include "TRestTools.h"
#include "TRestVersion.h"
#include "TStreamerElement.h"
#include "TVirtualStreamerInfo.h"
#include "tinyxml.h"

/* We keep using REST_RELEASE, REST_VERSION(2,X,Y) and REST_VERSION_CODE
   to determine the installed REST version and avoid too much prototyping

REST version is connected independently to each metadata structure. TRestRun
 should be our reference to determine the version of REST used to write the
ROOT file.

By including #include "TRestVersion.h" we get access to the definition of
 REST_RELEASE, REST_VERSION_CODE, REST_VERSION(2,X,Y) which is the REST
version we are using.

// storing the STATIC version of this installation
namespace REST_VersionGlob {
        TString GetRESTVersion();
        int GetRESTVersionCode();
};
inline TString GetRESTVersion() { return REST_VersionGlob::GetRESTVersion(); }
inline int GetRESTVersionCode() { return REST_VersionGlob::GetRESTVersionCode(); }

*/

class TRestManager;

//! A base class for any REST metadata class
class TRestMetadata : public TNamed {
   private:
    void ReadEnvInElement(TiXmlElement* e, bool overwrite = true);
    void ReadElement(TiXmlElement* e, bool recursive = false);
    void ReplaceForLoopVars(TiXmlElement* e, map<string, string> forLoopVar);
    void ExpandForLoopOnce(TiXmlElement* e, map<string, string> forLoopVar);
    void ExpandIfSections(TiXmlElement* e);
    void ExpandForLoops(TiXmlElement* e);
    void ExpandIncludeFile(TiXmlElement* e);
    string GetUnits(TiXmlElement* e);
    string FieldNamesToUpper(string inputString);

    /// REST version string, only used for archive and retrieve
    TString fVersion = REST_RELEASE;  //<
    TString fCommit = REST_COMMIT;    //<
    TString fLibraryVersion = "0";    //<

   protected:
    // new xml utilities
    std::string GetFieldValue(std::string parName, TiXmlElement* e);
    string GetParameter(std::string parName, TiXmlElement* e, TString defaultValue = PARAMETER_NOT_FOUND_STR);
    Double_t GetDblParameterWithUnits(std::string parName, TiXmlElement* e,
                                      Double_t defaultVal = PARAMETER_NOT_FOUND_DBL);
    TVector2 Get2DVectorParameterWithUnits(std::string parName, TiXmlElement* e,
                                           TVector2 defaultValue = TVector2(-1, -1));
    TVector3 Get3DVectorParameterWithUnits(std::string parName, TiXmlElement* e,
                                           TVector3 defaultValue = TVector3(-1, -1, -1));
    TiXmlElement* GetElementFromFile(std::string cfgFileName, std::string NameOrDecalre = "");
    TiXmlElement* GetElement(std::string eleDeclare);
    TiXmlElement* GetElement(std::string eleDeclare, TiXmlElement* e);
    TiXmlElement* GetNextElement(TiXmlElement* e);
    TiXmlElement* GetElementWithName(std::string eleDeclare, std::string eleName, TiXmlElement* e);
    TiXmlElement* GetElementWithName(std::string eleDeclare, std::string eleName);
    string GetParameterUnits(string parname);
    string GetParameterUnits(string parname, TiXmlElement* e);
    TiXmlElement* ReplaceElementAttributes(TiXmlElement* e);
    TiXmlElement* StringToElement(string definition);
    string ElementToString(TiXmlElement* ele);

    // old xml parser interface.
    string GetKEYStructure(std::string keyName);
    string GetKEYStructure(std::string keyName, size_t& Position);
    string GetKEYStructure(std::string keyName, string buffer);
    string GetKEYStructure(std::string keyName, size_t& Position, string buffer);
    string GetKEYStructure(std::string keyName, size_t& Position, TiXmlElement* ele);
    string GetKEYDefinition(std::string keyName);
    string GetKEYDefinition(std::string keyName, size_t& Position);
    string GetKEYDefinition(std::string keyName, string buffer);
    string GetKEYDefinition(std::string keyName, size_t& Position, string buffer);
    string GetParameter(std::string parName, size_t& pos, std::string inputString);
    string GetFieldValue(std::string fieldName, std::string definition, size_t fromPosition = 0);

    // some utils
    std::string ReplaceEnvironmentalVariables(const std::string buffer);
    string SearchFile(string filename);
    TString GetSearchPath();
    void ReSetVersion();
    void UnSetVersion();
    void SetLibraryVersion(TString version);

    //////////////////////////////////////////////////
    /// Data members
    /// NOTE!! In root6 the "#ifndef __CINT__" structure is not helpful any more!
    /// Attatch "//! something" structure in comment line to avoid variables being saved by root.
    /// see more detail in
    /// https://root.cern.ch/root/htmldoc/guides/users-guide/ROOTUsersGuide.html#automatically-generated-streamers

    /// Full name of the rml file
    std::string fConfigFileName;
    /// Section name given in the constructor of the derived metadata class
    std::string fSectionName;
    /// The buffer where the corresponding metadata section is stored. Filled only during Write()
    std::string configBuffer;
    /// The buffer to store the output message through TRestStringOutput in this class
    std::string messageBuffer;

    /// Verbose level used to print debug info
    REST_Verbose_Level fVerboseLevel;  //!
    /// Termination flag object for TRestStringOutput
    endl_t endl;  //!
    /// All metadata classes can be initialized and managed by TRestManager
    TRestManager* fHostmgr;  //!
    /// This variable is used to determine if the metadata structure should be stored in the ROOT file.
    Bool_t fStore;  //!
    /// Saving the sectional element together with global element
    TiXmlElement* fElement;  //!
    /// Saving the global element, to be passed to the resident class, if necessary.
    TiXmlElement* fElementGlobal;  //!
    /// Saving a list of rml variables. name-value pair.
    map<string, string> fVariables;  //!
    /// Saving a list of rml constants. name-value pair. Constants are temporary for this class only.
    map<string, string> fConstants;  //!

    /// It can be used as a way to identify that something went wrong using SetError method.
    Bool_t fError = false;  //!

    /// A string to store an optional error message through method SetError.
    TString fErrorMessage = "";  //!

    /// A metadata class may use this method to signal that something went wrong
    void SetError(TString message = "") {
        fError = true;
        fErrorMessage = message;
    }

    /// It allows to modify the error message. Only if SetError was called previously.
    void SetErrorMessage(TString message) {
        if (GetError()) fErrorMessage = message;
    }

   public:
    /// It returns true if an error was identified by a derived metadata class
    Bool_t GetError() { return fError; }

    /// Returns a string containing the error message
    TString GetErrorMessage() {
        if (GetError())
            return fErrorMessage;
        else
            return "No error!";
    }

    Int_t LoadConfigFromFile();
    Int_t LoadConfigFromFile(TiXmlElement* eSectional, TiXmlElement* eGlobal);
    Int_t LoadConfigFromFile(TiXmlElement* eSectional, TiXmlElement* eGlobal, map<string, string> envs);
    Int_t LoadConfigFromFile(string cfgFileName, string sectionName = "");

    /// Load global setting for the rml section, e.g., name, title.
    virtual Int_t LoadSectionMetadata();

    ///  To make settings from rml file. This method must be implemented in the derived class.
    virtual void InitFromConfigFile() = 0;

    /// Method called after the object is retrieved from root file.
    virtual void InitFromRootFile();

    /// Making default settings.
    virtual void Initialize() {}

    /// Implementing TObject::Print() method
    void Print() { PrintMetadata(); }  // *MENU*

    /// Implemented it in the derived metadata class to print out specific metadata information.
    virtual void PrintMetadata();

    /// Print the current time on local machine.
    void PrintTimeStamp(Double_t timeStamp);

    /// Print the config xml section stored in the class
    void PrintConfigBuffer();  // *MENU*

    /// Writes the config buffer to a file in append mode
    void WriteConfigBuffer(string fname);

    /// Print the buffered message
    void PrintMessageBuffer();  // *MENU*

    /// helps to pause the program, printing a message before pausing.
    /// ROOT GUI won't be jammed during this pause
    int GetChar(string hint = "Press a KEY to continue ...");

    // getters and setters
    std::string GetSectionName();

    std::string GetConfigBuffer();

    string GetDataMemberValue(string memberName);

    std::vector<string> GetDataMemberValues(string memberName);

    TString GetVersion();  // *MENU*

    TString GetCommit();

    TString GetLibraryVersion();

    Int_t GetVersionCode();
    /// Returns a string with the path used for data storage
    TString GetDataPath() { return GetParameter("mainDataPath", ""); }

    /// returns the verboselevel in type of REST_Verbose_Level enumerator
    REST_Verbose_Level GetVerboseLevel() { return fVerboseLevel; }

    /// returns the verboselevel in type of TString
    TString GetVerboseLevelString();

    /// Gets a string with the path used for data storage
    TString GetMainDataPath() { return GetDataPath(); }

    std::string GetParameter(std::string parName, TString defaultValue = PARAMETER_NOT_FOUND_STR);  // *MENU*

    Double_t GetDblParameterWithUnits(std::string parName, Double_t defaultValue = PARAMETER_NOT_FOUND_DBL);

    TVector2 Get2DVectorParameterWithUnits(string parName, TVector2 defaultValue = TVector2(-1, -1));

    TVector3 Get3DVectorParameterWithUnits(string parName, TVector3 defaultValue = TVector3(-1, -1, -1));

    /// If this method is called the metadata information will **not** be stored in disk.
    void DoNotStore() { fStore = false; }
    /// If this method is called the metadata information will be stored in disk.
    void Store() { fStore = true; }
    /// set the section name, clear the section content
    void SetSectionName(std::string sName) { fSectionName = sName; }
    /// set config file path from external
    void SetConfigFile(std::string cfgFileName) { fConfigFileName = cfgFileName; }
    /// Set the hoster manager for this class.
    void SetHostmgr(TRestManager* m) { fHostmgr = m; }
    /// sets the verboselevel
    void SetVerboseLevel(REST_Verbose_Level v) { fVerboseLevel = v; }

    void SetDataMemberValFromConfig();

    /// overwriting the write() method with fStore considered
    virtual Int_t Write(const char* name = 0, Int_t option = 0, Int_t bufsize = 0);

    TRestMetadata();
    ~TRestMetadata();
    TRestMetadata(const char* cfgFileNamecfgFileName);

    /// Call CINT to generate streamers for this class
    ClassDef(TRestMetadata, 7);
};

#endif
