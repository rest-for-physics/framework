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

#include <TApplication.h>
#include <TClass.h>
#include <TStreamerElement.h>
#include <TVirtualStreamerInfo.h>

#include <mutex>
#include <thread>

#include "TRestDataBase.h"
#include "TRestPhysics.h"
#include "TRestReflector.h"
#include "TRestStringHelper.h"
#include "TRestStringOutput.h"
#include "TRestSystemOfUnits.h"
#include "TRestTools.h"
#include "TRestVersion.h"
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
inline TString GetRESTVersion() const { return REST_VersionGlob::GetRESTVersion(); }
inline int GetRESTVersionCode() const { return REST_VersionGlob::GetRESTVersionCode(); }

*/

class TRestManager;

//! A base class for any REST metadata class
class TRestMetadata : public TNamed {
   private:
    void ReadEnvInElement(TiXmlElement* e, bool overwrite = true);
    void ReadElement(TiXmlElement* e, bool recursive = false);
    void ReplaceForLoopVars(TiXmlElement* e, std::map<std::string, std::string> forLoopVar);
    void ExpandForLoopOnce(TiXmlElement* e, std::map<std::string, std::string> forLoopVar);
    void ExpandForLoops(TiXmlElement* e, std::map<std::string, std::string> forLoopVar);
    void ExpandIfSections(TiXmlElement* e);
    void ExpandIncludeFile(TiXmlElement* e);
    std::string GetUnits(TiXmlElement* e);
    std::string FieldNamesToUpper(std::string inputString);
    void ReadOneParameter(std::string name, std::string value);
    TiXmlElement* ReplaceElementAttributes(TiXmlElement* e);

    /// REST version std::string, only used for archive and retrieve
    TString fVersion = REST_RELEASE;  //<
    TString fCommit = REST_COMMIT;    //<
    TString fLibraryVersion = "0";    //<

    Bool_t fOfficialRelease = false;  //<
    Bool_t fCleanState = false;       //<

   protected:
    // new xml utilities
    std::string GetFieldValue(std::string parName, TiXmlElement* e);
    std::string GetParameter(std::string parName, TiXmlElement* e,
                             TString defaultValue = PARAMETER_NOT_FOUND_STR);
    Double_t GetDblParameterWithUnits(std::string parName, TiXmlElement* e,
                                      Double_t defaultVal = PARAMETER_NOT_FOUND_DBL);
    TVector2 Get2DVectorParameterWithUnits(std::string parName, TiXmlElement* e,
                                           TVector2 defaultValue = TVector2(-1, -1));
    TVector3 Get3DVectorParameterWithUnits(std::string parName, TiXmlElement* e,
                                           TVector3 defaultValue = TVector3(-1, -1, -1));
    TiXmlElement* GetElementFromFile(std::string configFilename, std::string NameOrDecalre = "");
    TiXmlElement* GetElement(std::string eleDeclare, TiXmlElement* e = nullptr);
    TiXmlElement* GetNextElement(TiXmlElement* e);
    TiXmlElement* GetElementWithName(std::string eleDeclare, std::string eleName, TiXmlElement* e);
    TiXmlElement* GetElementWithName(std::string eleDeclare, std::string eleName);
    std::pair<std::string, std::string> GetParameterAndUnits(std::string parname, TiXmlElement* e = nullptr);
    TiXmlElement* StringToElement(std::string definition);
    std::string ElementToString(TiXmlElement* ele);

    // old xml parser interface.
    std::string GetKEYStructure(std::string keyName);
    std::string GetKEYStructure(std::string keyName, size_t& Position);
    std::string GetKEYStructure(std::string keyName, std::string buffer);
    std::string GetKEYStructure(std::string keyName, size_t& Position, std::string buffer);
    std::string GetKEYStructure(std::string keyName, size_t& Position, TiXmlElement* ele);
    std::string GetKEYDefinition(std::string keyName);
    std::string GetKEYDefinition(std::string keyName, size_t& Position);
    std::string GetKEYDefinition(std::string keyName, std::string buffer);
    std::string GetKEYDefinition(std::string keyName, size_t& Position, std::string buffer);
    std::string GetParameter(std::string parName, size_t& pos, std::string inputString);
    std::string GetFieldValue(std::string fieldName, std::string definition, size_t fromPosition = 0);

    // some utils
    std::string ReplaceVariables(const std::string buffer);
    std::string ReplaceConstants(const std::string buffer);
    std::string SearchFile(std::string filename);
    TString GetSearchPath();
    void ReSetVersion();
    void UnSetVersion();
    void SetLibraryVersion(TString version);

    // Load global setting for the rml section, e.g., name, title.
    virtual Int_t LoadSectionMetadata();
    /// To make settings from rml file. This method must be implemented in the derived class.
    virtual void InitFromConfigFile() {
        std::map<std::string, std::string> parameters = GetParametersList();

        for (auto& p : parameters) p.second = ReplaceMathematicalExpressions(p.second);

        ReadParametersList(parameters);
    }

    /// Method called after the object is retrieved from root file.
    virtual void InitFromRootFile() {}

    void ReadParametersList(std::map<std::string, std::string>& list);

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
    TRestStringOutput::REST_Verbose_Level fVerboseLevel;  //!
    /// Termination flag object for TRestStringOutput
    endl_t RESTendl;  //!

    /// All metadata classes can be initialized and managed by TRestManager
    TRestManager* fHostmgr;  //!
    /// This variable is used to determine if the metadata structure should be stored in the ROOT file.
    Bool_t fStore;  //!
    /// Saving the sectional element together with global element
    TiXmlElement* fElement;  //!
    /// Saving the global element, to be passed to the resident class, if necessary.
    TiXmlElement* fElementGlobal;  //!
    /// Saving a list of rml variables. name-value std::pair.
    std::map<std::string, std::string> fVariables;  //!
    /// Saving a list of rml constants. name-value std::pair. Constants are temporary for this class only.
    std::map<std::string, std::string> fConstants;  //!

    /// It can be used as a way to identify that something went wrong using SetError method.
    Bool_t fError = false;

    /// It counts the number of errors notified
    Int_t fNErrors = 0;

    /// It can be used as a way to identify that something went wrong using SetWarning method.
    Bool_t fWarning = false;

    /// It counts the number of warnings notified
    Int_t fNWarnings = 0;

    /// A std::string to store an optional error message through method SetError.
    TString fErrorMessage = "";

    /// It can be used as a way to identify that something went wrong using SetWarning method.
    TString fWarningMessage = "";

    std::map<std::string, std::string> GetParametersList();
    void ReadAllParameters();

    TRestMetadata();
    TRestMetadata(const char* configFilename);

   public:
    /// It returns true if an error was identified by a derived metadata class
    inline Bool_t GetError() const { return fError; }

    /// It returns true if an error was identified by a derived metadata class
    inline Bool_t GetWarning() const { return fWarning; }

    /// Add logs to messageBuffer
    void AddLog(std::string log = "", bool print = true);

    /// A metadata class may use this method to signal that something went wrong
    void SetError(std::string message = "", bool print = true, int maxPrint = 5);

    /// A metadata class may use this method to signal that something went wrong
    void SetWarning(std::string message = "", bool print = true, int maxPrint = 5);

    /// Returns a std::string containing the error message
    TString GetErrorMessage();

    /// Returns a std::string containing the warning message
    TString GetWarningMessage();

    inline Int_t GetNumberOfErrors() const { return fNErrors; }

    inline Int_t GetNumberOfWarnings() const { return fNWarnings; }

    Int_t LoadConfigFromElement(TiXmlElement* eSectional, TiXmlElement* eGlobal,
                                std::map<std::string, std::string> envs = {});
    Int_t LoadConfigFromFile(const std::string& configFilename, const std::string& sectionName = "");
    Int_t LoadConfigFromBuffer();

    TRestMetadata* InstantiateChildMetadata(int index, std::string pattern = "");
    TRestMetadata* InstantiateChildMetadata(std::string pattern = "", std::string name = "");

    /// Making default settings.
    virtual void Initialize() {}

    /// Implementing TObject::Print() method
    void Print() { PrintMetadata(); }  // *MENU*

    /// Implemented it in the derived metadata class to print out specific metadata information.
    virtual void PrintMetadata();

    /// Method to allow implementation of specific metadata members updates at inherited classes
    virtual void UpdateMetadataMembers() {}

    /// Print the current time on local machine.
    void PrintTimeStamp(Double_t timeStamp);

    /// Print the config xml section stored in the class
    void PrintConfigBuffer();  // *MENU*

    /// Writes the config buffer to a file in append mode
    void WriteConfigBuffer(std::string fName);

    /// Print the buffered message
    void PrintMessageBuffer();  // *MENU*

    // getters and setters
    std::string GetSectionName();

    std::string GetConfigBuffer();

    std::string GetDataMemberValue(std::string memberName);

    std::vector<std::string> GetDataMemberValues(std::string memberName, Int_t precision = 0);

    TString GetVersion();  // *MENU*

    TString GetCommit();

    TString GetLibraryVersion();

    inline Bool_t isOfficialRelease() const { return fOfficialRelease; }

    inline Bool_t isCleanState() const { return fCleanState; }

    Int_t GetVersionCode();
    /// Returns a std::string with the path used for data storage
    inline TString GetDataPath() {
        std::string dataPath = GetParameter("mainDataPath", "");
        if (dataPath == "") {
            dataPath = "./";
        }
        return dataPath;
    }

    /// returns the verboselevel in type of REST_Verbose_Level enumerator
    inline TRestStringOutput::REST_Verbose_Level GetVerboseLevel() { return fVerboseLevel; }

    /// returns the verbose level in type of TString
    TString GetVerboseLevelString();

    /// Gets a std::string with the path used for data storage
    inline TString GetMainDataPath() { return GetDataPath(); }

    std::string GetParameter(std::string parName, TString defaultValue = PARAMETER_NOT_FOUND_STR);  // *MENU*

    Double_t GetDblParameterWithUnits(std::string parName, Double_t defaultValue = PARAMETER_NOT_FOUND_DBL);

    TVector2 Get2DVectorParameterWithUnits(std::string parName, TVector2 defaultValue = TVector2(-1, -1));

    TVector3 Get3DVectorParameterWithUnits(std::string parName, TVector3 defaultValue = TVector3(-1, -1, -1));

    /// If this method is called the metadata information will **not** be stored in disk.
    void DoNotStore() { fStore = false; }
    /// If this method is called the metadata information will be stored in disk.
    void Store() { fStore = true; }
    /// set the section name, clear the section content
    void SetSectionName(std::string sName) { fSectionName = sName; }
    /// set config file path from external
    void SetConfigFile(std::string configFilename) { fConfigFileName = configFilename; }
    /// Set the host manager for this class.
    void SetHostmgr(TRestManager* m) { fHostmgr = m; }

    /// sets the verbose level
    void SetVerboseLevel(TRestStringOutput::REST_Verbose_Level v) { fVerboseLevel = v; }
    /// overwriting the write() method with fStore considered
    virtual Int_t Write(const char* name = nullptr, Int_t option = 0, Int_t bufsize = 0);

    ~TRestMetadata();

    // Making class constructors protected to keep this class abstract
    TRestMetadata& operator=(const TRestMetadata&) = delete;
    TRestMetadata(const TRestMetadata&) = delete;

    /// Call CINT to generate streamers for this class
    ClassDef(TRestMetadata, 9);
};

#endif
