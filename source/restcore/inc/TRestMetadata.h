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


#include "TRestStringHelper.h"
#include "TRestStringOutput.h"
#include "TRestTools.h"
#define TIXML_USE_STL
#include "tinyxml.h"

#include "TRestSystemOfUnits.h"
#include "TVirtualStreamerInfo.h"
#include "TClass.h"
#include "TStreamerElement.h"

const int PARAMETER_NOT_FOUND_INT = -99999999;
const double PARAMETER_NOT_FOUND_DBL = -99999999;
const std::string PARAMETER_NOT_FOUND_STR = "NO_SUCH_PARA";

class TRestManager;

//! A base class for any REST metadata class
class TRestMetadata :public TNamed {

public:

	///program message verbose level enumerator. Controlinng the density of output message.


	///file output level enumerator. Setting different save types of output file.


	TRestMetadata();
	~TRestMetadata();
	TRestMetadata(const char *cfgFileNamecfgFileName);

	/// Call CINT to generate streamers for this class
	ClassDef(TRestMetadata, 1);



	Int_t LoadConfigFromFile();
	Int_t LoadConfigFromFile(string fgFileName);
	Int_t LoadConfigFromFile(TiXmlElement* eSectional, TiXmlElement* eGlobal);
	Int_t LoadConfigFromFile(TiXmlElement* eSectional, TiXmlElement* eGlobal, vector<TiXmlElement*> eEnv);
	Int_t LoadConfigFromFile(string cfgFileName, string name) { LoadConfigFromFile(cfgFileName.c_str()); return 0; };
	virtual Int_t LoadSectionMetadata(string section, string cfgFileName, string name) { LoadSectionMetadata(); return 0; }
	virtual Int_t LoadSectionMetadata();
	virtual void InitFromConfigFile();




	///////////////////////////////////////////////////////////////
	/// Making default settings. This method must be implemented in the derived class.
	virtual void Initialize() = 0;

	/// These methods can be overridden in the child class.
	/// During initialization, the three methods will be called in sequence. 
	/// A single method is enough to make a complete initialization,
	virtual void BeginOfInit() { return; };

	/// These methods can be overridden in the child class.
	/// During initialization, the three methods will be called in sequence. 
	/// A single method is enough to make a complete initialization,
	virtual Int_t ReadConfig(string keydeclare, TiXmlElement* e) { return -1; };

	/// These methods can be overridden in the child class.
	/// During initialization, the three methods will be called in sequence. 
	/// A single method is enough to make a complete initialization,
	virtual void EndOfInit() { return; };

	/// It is implemented in the derived metadata class to print out specific metadata information.
	virtual void PrintMetadata();

	void PrintTimeStamp(Double_t timeStamp);
	/// This is a dummy function
	void PrintConfigBuffer();

	/// helps to pause the program, printing a message before pausing
	void GetChar() { cout << "Press a KEY to continue ..." << endl; getchar(); }



	//getters and setters
	/// returns the name of this section, usuall the name of the derived class
	std::string GetSectionName() { return fSectionName; }
	/// set the section name
	void SetSectionName(std::string sName) { fSectionName = sName; }
	/// set config file path from external
	void SetConfigFile(std::string cfgFileName) { fConfigFileName = cfgFileName; }
	/// Set the hoster manager for this class.
	void SetHostmgr(TRestManager*m) { fHostmgr = m; }
	/// Returns a string with the path used for data storage
	TString GetDataPath() { return GetParameter("gasDataPath"); }
	/// Returns a string with the path used for pre-generated gas files
	TString GetGasDataPath() { return GetParameter("dataPath"); }
	/// returns the verboselevel in type of REST_Verbose_Level enumerator
	REST_Verbose_Level GetVerboseLevel() { return fVerboseLevel; }

	TString GetVerboseLevelString();
	/// Gets a string with the path used for data storage
	TString GetMainDataPath() { return GetDataPath(); }
	std::string GetParameter(std::string parName, TString defaultValue = PARAMETER_NOT_FOUND_STR);
	Double_t GetDblParameterWithUnits(std::string parName, Double_t defaultValue = PARAMETER_NOT_FOUND_DBL);
	Double_t GetDoubleParameterWithUnits(std::string parName, Double_t defaultValue = PARAMETER_NOT_FOUND_DBL) {
		return GetDblParameterWithUnits(parName, defaultValue);
	}
	TVector2 Get2DVectorParameterWithUnits(string parName, TVector2 defaultValue = TVector2(-1, -1));
	TVector3 Get3DVectorParameterWithUnits(string parName, TVector3 defaultValue = TVector3(-1, -1, -1));
	/// sets the verboselevel
	void SetVerboseLevel(REST_Verbose_Level v) { fVerboseLevel = v; }
	/// If this method is called the metadata information will **not** be stored in disk. I/O is handled by TRestRun.
	void DoNotStore() { fStore = false; }
	/// If this method is called the metadata information will be stored in disk. This is the default behaviour.
	Bool_t Store() { return fStore; }


protected:
	//new xml utilities
	std::string GetFieldValue(std::string parName, TiXmlElement* e);
	string GetParameter(std::string parName, TiXmlElement* e, TString defaultValue = PARAMETER_NOT_FOUND_STR);

	Double_t GetDblParameterWithUnits(std::string parName, TiXmlElement* e, Double_t defaultVal = PARAMETER_NOT_FOUND_DBL);
	TVector2 Get2DVectorParameterWithUnits(std::string parName, TiXmlElement* e, TVector2 defaultValue = TVector2(-1, -1));
	TVector3 Get3DVectorParameterWithUnits(std::string parName, TiXmlElement* e, TVector3 defaultValue = TVector3(-1, -1, -1));



	TiXmlElement* GetRootElementFromFile(std::string cfgFileName);

	TiXmlElement* GetElement(std::string eleDeclare);
	TiXmlElement* GetElement(std::string eleDeclare, TiXmlElement* e);
	TiXmlElement* GetElement(std::string eleDeclare, std::string cfgFileName);
	TiXmlElement* GetElementWithName(std::string eleDeclare, std::string eleName, TiXmlElement* e);
	TiXmlElement* GetElementWithName(std::string eleDeclare, std::string eleName);
	std::string GetElementDeclare(TiXmlElement* e) { return e->Value(); }
	std::string GetUnits();
	std::string GetUnits(TiXmlElement* e);
	//replace mathematical expressions and env variables in the element attribute
	//all the elements will get proprocessed before the real operation.
	TiXmlElement* ReplaceElementAttributes(TiXmlElement* e);


	//adapting to old xml parser.
	//the following merthods are re-written, converting internal xml elements to string
	TiXmlElement* StringToElement(string definition);
	string ElementToString(TiXmlElement*ele);
	string GetKEYStructure(std::string keyName);
	string GetKEYStructure(std::string keyName, size_t &Position);
	string GetKEYDefinition(std::string keyName);
	string GetKEYDefinition(std::string keyName, size_t &Position);
	std::string GetFieldValue(std::string fieldName, std::string definition, size_t fromPosition = 0);
	Double_t GetDblFieldValueWithUnits(string fieldName, string definition, size_t fromPosition = 0);
	TVector2 Get2DVectorFieldValueWithUnits(string fieldName, string definition, size_t fromPosition = 0);
	TVector3 Get3DVectorFieldValueWithUnits(string fieldName, string definition, size_t fromPosition = 0);

	//the following methods require string xml element, and is directly copied form the old code
	string GetKEYStructure(std::string keyName, string buffer);
	string GetKEYStructure(std::string keyName, size_t &Position, string buffer);
	string GetKEYDefinition(std::string keyName, string buffer);
	string GetKEYDefinition(std::string keyName, size_t &Position, string buffer);
	std::string GetParameter(std::string parName, size_t &pos, std::string inputString);
	Double_t GetDblParameterWithUnits(std::string parName, size_t &pos, std::string inputString);
	TVector2 Get2DVectorParameterWithUnits(std::string parName, size_t &pos, std::string inputString);
	TVector3 Get3DVectorParameterWithUnits(std::string parName, size_t &pos, std::string inputString);


	//string utils
	std::string EvaluateExpression(std::string exp);
	std::string ReplaceMathematicalExpressions(std::string buffer);
	std::string ReplaceEnvironmentalVariables(const std::string buffer);

	void SetEnv(string name, string value, string overwrite);
	/// clear the env sections list
	void ClearEnv() { fElementEnv.clear(); }


	//data tools
	TStreamerElement* GetDataMemberWithName(string name);
	TStreamerElement* GetDataMemberWithID(int ID);
	int GetNumberOfDataMember();
	double GetDblDataMemberVal(TStreamerElement*);
	int GetIntDataMemberVal(TStreamerElement*);
	char* GetDataMemberRef(TStreamerElement*);
	string GetDataMemberValString(TStreamerElement*);
	void SetDataMemberVal(TStreamerElement*, char*);
	void SetDataMemberVal(TStreamerElement*, string);
	void SetDataMemberValFromConfig(TStreamerElement*);



	//////////////////////////////////////////////////
	///Data members
	///NOTE!! In root6 the "#ifndef __CINT__" structure is not helpful any more!
	///Attatch "//! something" structure in comment line to avoid variables being saved by root.
	///see more detail in https://root.cern.ch/root/htmldoc/guides/users-guide/ROOTUsersGuide.html#automatically-generated-streamers
#ifndef __CINT__

	//Name;(Derived from TNamed)
	//Title;(Derived from TNamed)
	///Full name of rml file 
	std::string fConfigFileName;
	///Section name given in the constructor of the derived metadata class
	std::string fSectionName;
	///Verbose level used to print debug info
	REST_Verbose_Level fVerboseLevel;   //! 
	///All metadata classes can be initialized and managed by TRestManager
	TRestManager* fHostmgr;//!
	///This variable is used to determine if the metadata structure should be stored in the ROOT file.
	Bool_t fStore;  //! 
	///Saving the sectional element together with global element
	TiXmlElement* fElement;//!
	///Saving the global element, to be passed to the resident class, if necessary.
	TiXmlElement* fElementGlobal;//! 
	///Saving a list of environmental variables
	vector<TiXmlElement*> fElementEnv;//! 




	///formatted message output, used for print metadata
	TRestLeveledOutput<REST_Silent> fout = TRestLeveledOutput<REST_Silent>(fVerboseLevel, COLOR_BOLDBLUE, "==");//! 
	TRestLeveledOutput<REST_Silent> error = TRestLeveledOutput<REST_Silent>(fVerboseLevel, COLOR_BOLDRED, "", 1);//! 
	TRestLeveledOutput<REST_Essential> warning = TRestLeveledOutput<REST_Essential>(fVerboseLevel, COLOR_BOLDYELLOW, "", 1);//! 
	TRestLeveledOutput<REST_Essential> essential = TRestLeveledOutput<REST_Essential>(fVerboseLevel, COLOR_BOLDGREEN);//! 
	TRestLeveledOutput<REST_Info> info = TRestLeveledOutput<REST_Info>(fVerboseLevel, COLOR_BOLDGREEN);//! 
	TRestLeveledOutput<REST_Debug> debug = TRestLeveledOutput<REST_Debug>(fVerboseLevel, COLOR_RESET, "", 1);//! 
	TRestLeveledOutput<REST_Extreme> extreme = TRestLeveledOutput<REST_Extreme>(fVerboseLevel, COLOR_RESET, "", 1);//! 



#endif


private:

	void ProcessElement(TiXmlElement* e);
	void SetEnvVariable(TiXmlElement* e);
	//void ExecuteForLoops(TiXmlElement* e);
	//void LoadConfigInIncludeFile(TiXmlElement* e);
	void ExpandElement(TiXmlElement*e);
	void ExpandForLoops(TiXmlElement*e);
	void ExpandIncludeFile(TiXmlElement* e);




};



#endif
