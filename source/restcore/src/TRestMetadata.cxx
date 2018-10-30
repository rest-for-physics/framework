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


//////////////////////////////////////////////////////////////////////////
///
/// One of the core classes of REST. Abstract class
/// from which all REST "metadata classes" must derive.
/// A metadata class in REST is any holder of data other than event data
/// that is relevant to understand the origin and history of 
/// transformations that a given set of event data has gone through. 
/// For example the geometry of a simulation,
/// the parameters of a process, the properties of a gas, 
/// the readout pattern used to "pixelize" data, etc... are examples 
/// of metadata.
/// All metadata classes can be "initialized" via
/// configuration (.rml) files that the user can write. Alternatively
/// they can be read from root files. TRestMetadata contains 
/// the common functionality that allows metadata to be read from .rml
/// files or previously `stored` TRestMetadata structures stored in a 
///  ROOT file.
///
/// ### RML file structure 
///
/// A class deriving from TRestMetadata can retrieve information from a plain text
/// configuration file (or RML file). The syntaxis in an RML file is imposed by
/// TRestMetadata. The rml file is encoded in standard xml format and has a structure 
/// similar to the real class structure. The following piece of code shows the common 
/// structure of the metadata description corresponding to a specific metadata class.
///
/// \code
///
/// <ClassName name="userGivenName" title="User given title" >
///
///     <SomeCommand field1="value1" field2="value2"> 
///
///     <ContainedClassName field1="value1" field2="value2" ... >
///
///         <SomeCommand field1="value1" field2="value2">  
///         
///          ...
///
///     </ContainedClassName>
///
/// </ClassName>
///
/// \endcode
///
/// In REST, the key word "ClassName", "SomeCommand", "ContainedClassName"
/// shown above are all called **xml decalration**. The key words like
/// "field1="value1"" or "name="userGivenName"" are called **fields** or **xml attributes**.
/// All the sealed xml structure is called **xml element** or **xml section**. 
///
/// Note that the decalration
/// "include", "for", "variable" and "myParameter" is reserved for the software.
/// They works differently than others, which we will talk later.
/// 
/// ### Sequencial start up procedure of a metadata class
/// 
/// The rml file is designed to start up/instruct all the metadata classes. Usually we implement 
/// the method Initialize() and calls it in the constructor to do some default settings.
/// 
/// \code
/// TRestSpecificMetadata::TRestSpecificMetadata()
/// {
/// 	Initialize();
/// }
/// 
/// void TRestSpecificMetadata::Initialize( )
/// {
///
///     SetSectionName( this->ClassName() );
///
///     ....
///
/// }
/// \endcode
/// 
/// The starter method LoadConfigFromFile() is implemented in this class, with four overloads, as shown
/// in the following. 
/// 
/// \code
///
/// void LoadConfigFromFile();
/// void LoadConfigFromFile(const char *cfgFileName);
/// void LoadConfigFromFile(TiXmlElement* eSectional, TiXmlElement* eGlobal);
/// void LoadConfigFromFile(TiXmlElement* eSectional, TiXmlElement* eGlobal, vector<TiXmlElement*> eEnv);
///
/// \endcode
/// 
/// If no arguments are provided, the starter will only call the Initialize() method. If given the rml
/// file name, it will find out its rml sections. We can also directly give the xml sections to it.
/// Three xml sections are used to startup the class. The sectional section is the section with the
/// same name as the class, which are providing the basic infomation. The global section is a special 
/// xml section in the rml file, containing some global settings such as datapath or file format.
/// The env section is optional, through which the host class inputs the variables into its resident class.
/// 
/// With the xml sections given, The starter first merge them together. Then it calls LoadSectionMetadata(),
/// which loads some universal parameters like name, title and verbose level. This method also preprocesses 
/// the config sections, expanding the include/for decinition and replacing the variables. After this, 
/// the starter calls the method InitFromConfigFile().
///
/// InitFromConfigFile() is a pure virtual method and every child classes have to implement it. This method 
/// defines how the metadata class loads its xml config section in rml file. If the user just want a simple 
/// startup logic, he can implement it with few lines of GetParameter().


/// \code
///
/// <TRestMuonAnalysisProcess name = "muAna" title = "Example" verboseLevel = "info" >
/// 	<parameter name="XROI" value="(100,300)"/>
/// 	<parameter name="YROI" value="(0,-200)"/>
/// </TRestMuonAnalysisProcess>
///
/// void TRestMuonAnalysisProcess::InitFromConfigFile()
/// {
/// 	TVector2 XROI = StringTo2DVector(GetParameter("XROI", "(-100,100)"));
/// 	TVector2 YROI = StringTo2DVector(GetParameter("YROI", "(-100,100)"));
/// 
/// 	X1 = XROI.X(), X2 = XROI.Y(), Y1 = YROI.X(), Y2 = YROI.Y();
/// }
///
/// \endcode


/// 
/// The sequential startup trick lies in that, when we want to initialize another TRestMetadata class
/// (resident) in our main class(host), we write the residient's config section as a child section of 
/// host's section in the rml file. When doing initialization, we iterate all the child elements of the 
/// host's config element. Then when we find a child element declared as an available class name, we can
/// instantiate the class. We then send this child element as config section to is resident class, calling
/// its LoadConfigFromFile() method. The rml hierarchy will be the same as class residence.
///
/// For example, when received an xml section declared "TRestRun", the host "TRestManager" will pass 
/// this section (together with its global section) to its resident "TRestRun". The TRestRun class can 
/// therefor perform a startup using these sections.
///
/// \code
///
/// <TRestManager name = "CoBoDataAnalysis" title = "Example" verboseLevel = "info" >
/// 	<TRestRun name = "SJTU_Proto" >
/// 		<addMetadata name = "PandaReadout_MxM" file = "readouts.root" />
/// 	</TRestRun>
///		...
/// </TRestManager>
///
/// void TRestManager::InitFromConfigFile()
/// {
///		if (fElement != NULL)
///		{
/// 		TiXmlElement*e = fElement->FirstChildElement();
/// 		while (e != NULL)
/// 		{
///				string value = e->Value();
/// 			if (value == "variable" || value == "myParameter" || value == "constant") { e = e->NextSiblingElement(); continue; }
///				ReadConfig((string)e->Value(), e);
///				e = e->NextSiblingElement();
/// 		}
///		}
/// }
///
/// Int_t TRestManager::ReadConfig(string keydeclare, TiXmlElement* e)
/// {
/// 	if (keydeclare == "TRestRun") {
/// 		fRunInfo = new TRestRun();
/// 		fRunInfo->LoadConfigFromFile(e, fElementGlobal);
/// 		return 0;
/// 	}
/// }
/// 
/// \endcode
/// 
/// 
/// ### Replacement of variables and expressions
/// 
/// By default, the starter will look into only the first-level child sections of both global 
/// and sectional section. The value of them will be saved in the metadata. In this level the decalration
/// "myParameter" and "variable" is the same. 
/// 
/// \code
/// 
///   <ClassName name="userGivenName" title="User given title" >
///       <myParameter name="nChannels" value="${CHANNELS}" /> //this variable cannot be loaded by the class "ContainedClassName"
///
///       <ContainedClassName field1="value1" field2="value2"  >
///           <variable .... / > //this variable cannot be loaded by the class "ClassName"
///       </ContainedClassName>
///       <parameter name="Ch" value="nChannels+{CHANNELS}-2" />
///   </ClassName>
/// 
///   <global>
///       <variable name = "CHANNELS" value = "64" overwrite = "false" / > //this variable can be loaded by both
///   </global>
///
/// \endcode
///
///
/// After this process, the starter will replace the field values of xml sections before they are used.
/// This work is done by the method PreprocessElement(). The procedure of replacing is as following.
/// 
/// 1. recognize the strings surrounded by "$ENV{}". Seek in system env and replace these strings.
/// 1. recognize the strings surrounded by "${}". Seek in system env as well as "variable" and replace these strings.
/// 2. directly replace the strings matching the name of "myParameter" and "constant" by its value.
/// 3. Judge if the string is an expressions, if so, try evaluate it using TFormula. Replace it by the result.
///
/// The result string of the field value is either a number string or a string with concrete content.
/// In the exapmle code above, the section declared with "parameter" is has its field value reset to string 126.
///
/// ### Including external RML files in a main RML file
///
/// It is possible to link to other files in any section.
/// There are two include modes: 
/// 1. raw include. the starter will parse all the lines in the file as xml element 
/// and insert them inside the local section.
/// \code
/// <TRestXXX>
///		<include file="abc.txt"/>
/// </TRestXXX>
/// \endcode
/// 2. auto insert. the starter will automatically find corresponding section in the file. 
/// If the file can be parsed by tinyxml, it will first import its globals section.
/// When searching the section, the starter searches according to the name and type. Here "type"
/// can either be the element declare or attribute "type". After finding the section, 
/// its child sections as well as attributes will be inserted into the local element. 
/// \code
/// <TRestXXX name="sAna" file="abc.rml"/>
/// \endcode
/// Or
/// \code
/// <doXXX type="TRestXXX" name="sAna" file="abc.rml"/>
/// \endcode
///
/// After the expansion is done, variable replacement is also performed.
///
/// If the target file is a root file, there will be a different way to load, see 
/// TRestRun::ImportMetadata()
/// 
/// ### For loop definition
///
/// The definition of FOR loops is implemented in RML in order to allow extense
/// definitions, where many elements may need to be added to an existing array in
/// our metadata structure. The use of FOR loops allows to introduce more
/// versatil and extense definitions. Its implementation was fundamentally triggered 
/// by its use in the construction of complex, multi-channel generic readouts by
/// TRestReadout.
/// 
/// The for loop definition is as follows, where *pitch* and *nChannels* are previously 
/// defined myParameters, and *nCh* and *nPix* are the *for* loop iteration variables.
///
/// \code
/// <for variable = "nCh" from = "0" to = "nChannels-2" step = "1" >
///	<readoutChannel id = "${nCh}" >
///		<for variable = "nPix" from = "0" to = "nChannels-1" step = "1" >
///			<addPixel id = "${nPix}" origin = "((1+${nCh})*pitch,pitch/4+${nPix}*pitch)" size = "(pixelSize,pixelSize)" rotation = "45" / >
///		</for>
///		<addPixel id = "nChannels" origin = "(${nCh}*pitch,pitch/4+(nChannels-1)*pitch+pitch/2)" size = "(pitch+pitch/2,pitch/2)" rotation = "0" / >
///	</readoutChannel>
/// </for>
/// \endcode
///
/// The starter will recongize the fields "variable", "from", "to", "step" in the header of the 
/// for loop definition. The variable "nCh", definded at the header of the for loop definition, 
/// will be added to the environment variable list. The value of the variable will be updated in each loop
/// The content of the loop will be normally prepeocessed, replacing the variables and expressions, 
/// and then expanded in the local section.
/// 
/// To pass the loop infomarion into the resident TRestMetadata class, one needs to call the fourth 
/// overload of the LoadConfigFromFile() starter methods. The resident class can get access to its host's
/// variable list in this overload.
///
///
/// ### The globals section
///
/// The *globals* section allows to specify few common definitions used in the REST 
/// framework. This section will maintain the same and will be passed though the whole sequencial 
/// startup procedure. If a section is to be used many times by different classes, it is better 
/// to put it in the global section.
///
/// \code
///	<globals>
///		<searchPath value = "$ENV{REST_INPUTDATA}/definitions/" />
///		<parameter name = "outputLevel" value = "internalvar" />
///		<parameter name = "verboseLevel" value = "essential" />
///		<parameter name = "inputFile" value = "${REST_INPUTFILE}" />
///		<parameter name = "inputFormat" value = "run[RunNumber]_file[Fragment]_[Time-d]_[Time].graw" />
///		<parameter name = "outputFile" value = "RUN[RunNumber]_[Time]_[LastProcess].root" />
///		<parameter name = "mainDataPath" value = "" />
///	</globals>
/// \endcode
///
/// The global section will have effect on *all the metadata structures* (or sections) that are
/// defined in a same RML file. It does not affect to other possible linked sections defined by 
/// reference using for example nameref.
///
/// ### Universal file search path
///
/// Some times we don't want to write a long full path to specify files, especially when 
/// multiple files are in a same remote directory. REST provides a universal file path definition
/// in rml. 
///
/// \code
///	<globals>
///		<searchPath value = "$ENV{REST_INPUTDATA}/definitions/" />
///		<searchPath value = "$ENV{REST_INPUTDATA}/gasFiles/" />
///	</globals>
/// \endcode
///
/// When calling TRestMetadata::SearchFile(), REST will search the file in all the paths defined
/// in section "searchPath", and return a full name if found. Include definition has already adopted 
/// this search strategy. Child classes can also take advantage of it.
///
/// ### Default fields for a section
///
/// Three fields in the first line of an xml section will be looked for before anything else. 
/// They are: name, title, and verboseLevel. If not specified, the starter will find in the 
/// *globals* section. If still not fond, it will use the default value.
/// 
/// Field "name" and "title" is needed by TNamed classes. The "verboseLevel" is used for 
/// changing the amount of output infomation. The following line would print on screen any 
/// debug message implemented in TRestSignalAnalysisProcess.
///
/// \code
/// <TRestSignalAnalysisProcess name="sgnlAna" title="Data analysis" verboseLevel="debug" >
///     ...
/// </TRestSignalAnalysisProcess>
/// \endcode
///
///
/// ### Using physical units in fields definitions 
///
/// Some physical parameters are need to specify the unit so that the provided value makes 
/// sense. For example, when defining the electric field we must provide its units.
///
/// \code
/// <parameter name="electricField" value="1000" units="V/cm" />
/// \endcode
///
/// The physical field values wrote in this way will be converted to the standard unit 
/// system used by REST. REST_Units namespace provides details on the different existing 
/// units, unit conversion and unit definition. 
///
/// When we retrieve any value from a REST member in a TRestMetadata class the value 
/// will be returned in the default REST units (mm, keV, V/cm, us). We may convert
/// this member to the desired units (as defined in REST_Units), by doing:
///
/// \code
/// Double_t valueInMeV = value * REST_Units::MeV;
/// \endcode
///
///
/// ### Other usefule public tools
///
/// GetParameter(), GetElement(), GetElementWithName(), SearchFile(). Details are shown in the function's document
///
///--------------------------------------------------------------------------
///
/// RESTsoft - Software for Rare Event Searches with TPCs
///
/// History of developments:
///
/// 2014-june: First concept. As part of conceptualization of previous REST
///            code (REST v2)
///            Igor G. Irastorza
///
/// 2015-jul:  Re-implementation to read .rml files with xml-inspired 
///            syntax
///            Javier Galán
/// 
/// 2017-Aug:  Major change to xml reading and class startup procedure
///            Kaixiang Ni
/// 
/// \class      TRestMetadata
///
/// <hr>
///

#include <iomanip>

#include <TMath.h>
#include <TSystem.h>
#include "TRestMetadata.h"
#include "v5/TFormula.h"
using namespace std;
using namespace REST_Units;

bool TRestMetadata_ConfigFileUpdated = true;

ClassImp(TRestMetadata)
///////////////////////////////////////////////
/// \brief TRestMetadata default constructor
///
TRestMetadata::TRestMetadata()
{
	fStore = true;
	fElementGlobal = NULL;
	fElement = NULL;
	fVerboseLevel = REST_Essential;
	//helper = new TRestStringHelper();
	fElementEnv.clear();
	fHostmgr = NULL;
}

///////////////////////////////////////////////
/// \brief constructor
///
TRestMetadata::TRestMetadata(const char *cfgFileName)
{
	fStore = true;
	fElementGlobal = NULL;
	fElement = NULL;
	fVerboseLevel = REST_Essential;
	fConfigFileName = cfgFileName;
	//helper = new TRestStringHelper();
	fElementEnv.clear();
	fHostmgr = NULL;
}

///////////////////////////////////////////////
/// \brief TRestMetadata default destructor
///
TRestMetadata::~TRestMetadata()
{
	//delete fElementGlobal;
	//delete fElementSectional;
}

///////////////////////////////////////////////
/// \brief Default starter. Just call again the Initialize() method.
/// 
Int_t TRestMetadata::LoadConfigFromFile()
{
	Initialize();
	return 0;
}

///////////////////////////////////////////////
/// \brief Give the file name, find out the corresponding section. Then call the main starter.
/// 
Int_t TRestMetadata::LoadConfigFromFile(string cfgFileName,string sectionName)
{
	if (fileExists(cfgFileName)) {
		TiXmlElement* Sectional;
		if (sectionName == "")
		{
			Sectional = GetElement(ClassName(), cfgFileName);
			if (Sectional == NULL) {
				error << "cannot find xml section \"" << ClassName() << "\" in config file: " << cfgFileName << endl;
				exit(1);
			}
		}
		else
		{
			TiXmlElement*ele = GetRootElementFromFile(cfgFileName);
			if (ele->Value() == (string)ClassName()) { Sectional = ele; }
			else { Sectional = GetElementWithName(ClassName(), sectionName, ele); }
			if (Sectional == NULL) {
				error << "cannot find xml section \"" << ClassName() << "\" with name \""<< sectionName <<"\""<<endl;
				error << "in config file: " << cfgFileName << endl;
				exit(1);
			}
		}
		TiXmlElement* Global = GetElement("globals", cfgFileName);
		vector<TiXmlElement*> a;
		a.clear();
		return LoadConfigFromFile(Sectional, Global, a);
	}
	else
	{
		error << "Filename : " << cfgFileName << endl;
		error << "REST ERROR. Config File does not exist. Right path/filename?" << endl;
		GetChar();
		return -1;
	}
}

///////////////////////////////////////////////
/// \brief Calling the main starter
/// 
Int_t TRestMetadata::LoadConfigFromFile(TiXmlElement* eSectional, TiXmlElement* eGlobal)
{
	vector<TiXmlElement*> a;
	a.clear();
	return LoadConfigFromFile(eSectional, eGlobal, a);
}

///////////////////////////////////////////////
/// \brief Main starter. 
///
/// First merge the sectional and global sections together, then save the input env section.
/// To make start up it calls the following methods in sequence: LoadSectionMetadata(), InitFromConfigFile()
/// 
Int_t TRestMetadata::LoadConfigFromFile(TiXmlElement* eSectional, TiXmlElement* eGlobal, vector<TiXmlElement*> eEnv)
{
	Initialize();
	TiXmlElement* theElement;
	if (eSectional != NULL && eGlobal != NULL)
	{
		//Sectional and global elements are first combined.
		theElement = (TiXmlElement*)eSectional->Clone();
		TiXmlElement* echild = eGlobal->FirstChildElement();
		while (echild != NULL) {
			theElement->LinkEndChild(echild->Clone());
			echild = echild->NextSiblingElement();
		}
		for (int i = 0; i < eEnv.size(); i++)
		{
			theElement->LinkEndChild(eEnv[i]->Clone());
		}
	}
	else if (eSectional != NULL)
	{
		theElement = eSectional;
	}
	else if (eGlobal != NULL)
	{
		theElement = eGlobal;
	}
	else
	{
		return 0;
	}
	fElement = theElement;
	fElementGlobal = eGlobal;
	fElementEnv = eEnv;

	int result = LoadSectionMetadata();
	if (result == 0)
		InitFromConfigFile();
	debug << "**** " << ClassName() << " has finished preparing config data ****" << endl;
	return result;
}

///////////////////////////////////////////////
/// \brief This method does some preparation for the starter.
///
/// Preparation includes: seting the name, title and verbose level of the current class. 
/// Finding out and saving the env sections. 
/// 
/// By calling TRestMetadata::ExpandElement(), is also expands for loops and include definitions, 
/// and replaces env and expressions in rml config section.
/// 
Int_t TRestMetadata::LoadSectionMetadata()
{
	//general metadata: name, title, verboselevel
	this->SetName(GetParameter("name", "defaultName").c_str());
	this->SetTitle(GetParameter("title", "defaultTitle").c_str());
	this->SetSectionName(this->ClassName());
	string debugStr = GetParameter("verboseLevel", "essential");
	if (debugStr == "silent" || debugStr == "0")
		fVerboseLevel = REST_Silent;
	if (debugStr == "essential" || debugStr == "1")
		fVerboseLevel = REST_Essential;
	if (debugStr == "info" || debugStr == "2")
		fVerboseLevel = REST_Info;
	if (debugStr == "debug" || debugStr == "3")
		fVerboseLevel = REST_Debug;
	if (debugStr == "extreme" || debugStr == "4")
		fVerboseLevel = REST_Extreme;

	fStore = ToUpper(GetParameter("store", "true")) == "TRUE" || ToUpper(GetParameter("store", "true")) == "ON";

	debug << "Loading Config for : " << this->ClassName() << endl;


	//first set env from global section
	if (fElementGlobal != NULL) {
		TiXmlElement* e = fElementGlobal->FirstChildElement();
		while (e != NULL)
		{
			if ((string)e->Value() == "variable" || (string)e->Value() == "myParameter" || (string)e->Value() == "constant")
			{
				ReplaceElementAttributes(e);
				SetEnv(e);
			}
			e = e->NextSiblingElement();
		}
	}

	//then from local section
	TiXmlElement* e = fElement->FirstChildElement();
	while (e != NULL)
	{
		if ((string)e->Value() == "variable" || (string)e->Value() == "myParameter" || (string)e->Value() == "constant")
		{
			ReplaceElementAttributes(e);
			SetEnv(e);
		}
		e = e->NextSiblingElement();
	}


	//finally do this replacement for all child elements and expand for/include definitions
	ExpandElement(fElement);

	configBuffer = ElementToString(fElement);

	return 0;
}

void TRestMetadata::InitFromRootFile() {

	if (GetSectionContent() != "") {
		fElement = StringToElement(GetSectionContent());
		//this->InitFromConfigFile();
	}
}

///////////////////////////////////////////////
/// \brief replace the field value(attribute) of the given xml element
///
/// it calls ReplaceEnvironmentalVariables() and ReplaceMathematicalExpressions() 
/// in sequence. "name" attribute won't be replaced
TiXmlElement * TRestMetadata::ReplaceElementAttributes(TiXmlElement * e)
{
	if (e == NULL)return NULL;

	TiXmlAttribute* attr = e->FirstAttribute();
	while (attr != NULL)
	{
		const char* val = attr->Value();
		const char* name = attr->Name();

		//set attribute except the item : name="" 
		if (name != "name") {
			string temp = ReplaceEnvironmentalVariables(val);
			e->SetAttribute(name, ReplaceMathematicalExpressions(temp).c_str());
		}

		attr = attr->Next();
	}

	return e;
}

///////////////////////////////////////////////
/// \brief Identify an environmental variable section and add it into env section list
///
/// Vaild section declaration: "variable", "myParameter", "constant". If the section 
/// exists already, its value will be updated if "updateexisting" is true. If a system
/// env with the same name has been defined already, then the system env will be used, 
/// unless the attribute "overwrite" is true.
/// 
/// Example of environmental variable section: 
/// \code 
/// <variable name="TEST" value="VALUE" overwrite="true" /> 
/// \endcode
///
void TRestMetadata::SetEnv(TiXmlElement* e, bool updateexisting)
{
	if (e == NULL)return;

	//cout << this->ClassName() << " " << (string)e->Value()<< " " << e->Attribute("value") << endl;

	const char* name = e->Attribute("name");
	if (name == NULL)return;
	const char* value = e->Attribute("value");
	if (value == NULL)return;

	//if overwrite is false, try to replace the value from system env.
	const char* overwrite = e->Attribute("overwrite");
	if (overwrite == NULL)overwrite = "false";
	if ((string)overwrite == "true" || (string)overwrite == "True" || (string)overwrite == "yes") {
		//setenv(name, value,1);
	}
	else
	{
		char* sysenv = getenv(name);
		if (sysenv != NULL)value = sysenv;
	}

	//SetEnv(name, value, overwriteexisting);

	//find the existing and set its value
	for (int i = 0; i < fElementEnv.size(); i++)
	{
		string name2 = fElementEnv[i]->Attribute("name");
		if ((string)e->Value() == (string)fElementEnv[i]->Value() && name2 == (string)name) 
		{
			if(updateexisting)
				fElementEnv[i]->SetAttribute("value", value);
			return;
		}
	}

	//if not find, add it directly.
	fElementEnv.push_back((TiXmlElement*)e->Clone());
}

///////////////////////////////////////////////
/// \brief Expand for loop and include definitions in the given xml section
///
/// The expansion is done recursively except for child sections declared after "TRest".
/// They are supposed to be a metadata class and to be doing the expansion themselves. 
/// If the argument "recursive" is true, these child sections will also be processed.
/// Before expansion, ReplaceElementAttributes() will first be called.
void TRestMetadata::ExpandElement(TiXmlElement*e, bool recursive)
{
	ReplaceElementAttributes(e);
	if ((string)e->Value() == "for") 
	{
		ExpandForLoops(e);
	}
	else if (e->Attribute("file") != NULL)
	{
		ExpandIncludeFile(e);
	}
	else if ((string)e->Value() == "variable" || (string)e->Value() == "myParameter" || (string)e->Value() == "constant")
	{
		SetEnv(e);
	}
	else if (e->FirstChildElement() != NULL) 
	{
		TiXmlElement* contentelement = e->FirstChildElement();
		//we won't expand child section unless forced recursive. The expansion of this 
		//section will be executed by the resident TRestXXX class
		if (contentelement != NULL && (recursive || ((string)contentelement->Value()).find("TRest") == -1)) {
			debug << "into child elements of: " << e->Value() << endl;
		}
		while (contentelement != NULL && (recursive || ((string)contentelement->Value()).find("TRest") == -1))
		{
			TiXmlElement*nxt = contentelement->NextSiblingElement();
			ExpandElement(contentelement, recursive);
			contentelement = nxt;
		}
	}
}

///////////////////////////////////////////////
/// \brief Expands the loop structures found in the given xml section.
/// 
/// The expansion is done by creating new TiXmlElement objects and inserting them in the 
/// given xml section. Loop variable is treated samely as REST "variable"
///
void TRestMetadata::ExpandForLoops(TiXmlElement*e)
{
	if ((string)e->Value() != "for")return;
	//ReplaceElementAttributes(e);

	const char* varname = e->Attribute("variable");
	const char* varfrom = e->Attribute("from");
	const char* varto = e->Attribute("to");
	const char* varstep = e->Attribute("step");

	if (varname == NULL || varfrom == NULL || varto == NULL)return;
	if (varstep == NULL)varstep == "1";
	TiXmlElement*parele = (TiXmlElement*)e->Parent();
	if (parele == NULL)return;

	string _from = ReplaceMathematicalExpressions(ReplaceEnvironmentalVariables(varfrom));
	string _to = ReplaceMathematicalExpressions(ReplaceEnvironmentalVariables(varto));
	string _step = ReplaceMathematicalExpressions(ReplaceEnvironmentalVariables(varstep));
	if (isANumber(_from) && isANumber(_to)&&isANumber(_step)) {
		double from = StringToDouble(_from);
		double to = StringToDouble(_to);
		double step = StringToDouble(_step);

		debug << "----expanding for loop----" << endl;
		double i = 0;
		for (i = from; i <= to; i = i + step)
		{
			ostringstream ss;
			ss << i;
			SetEnv(varname, ss.str(), true);
			TiXmlElement* contentelement = e->FirstChildElement();
			while (contentelement != NULL)
			{
				if ((string)contentelement->Value() == "for") {
					TiXmlElement*newforloop = (TiXmlElement*)contentelement->Clone();
					//ReplaceElementAttributes(newforloop);
					TiXmlElement*tempnew = (TiXmlElement*)parele->InsertBeforeChild(e, *newforloop);
					delete newforloop;
					newforloop = tempnew;
					ExpandForLoops(newforloop);
					contentelement = contentelement->NextSiblingElement();
				}
				else
				{
					TiXmlElement*attatchedalament = (TiXmlElement*)contentelement->Clone();
					ExpandElement(attatchedalament,true);
					//debug << *attatchedalament << endl;
					parele->InsertBeforeChild(e, *attatchedalament);
					delete attatchedalament;
					contentelement = contentelement->NextSiblingElement();
				}

			}
		}

		parele->RemoveChild(e);

		if (fVerboseLevel >=REST_Extreme)
			parele->Print(stdout, 0);
		debug << "----end of for loop----" << endl;
	}
	

}

///////////////////////////////////////////////
/// \brief Open the given rml file and find the corresponding section. 
/// 
/// It will search rml file in both current directory and "searchPath". 
/// Two include modes: 
/// 1. raw include. It will parse all the lines in the file as xml element 
/// and insert them inside the local section.
/// \code
/// <TRestXXX>
///		<include file="abc.txt"/>
/// </TRestXXX>
/// \endcode
/// 2. auto insert. It will automatically find the section in the file, according to
/// "type" and "name". At least one of the two definitions should be specified. 
/// Here "type" can either be the element declare or its attribute. After finding the 
/// remote section, this method will insert its child sections and attributes into the 
/// local xml element. 
/// \code
/// <TRestXXX name="sAna" file="abc.rml"/>
/// \endcode
/// Or
/// \code
/// <doXXX type="TRestXXX" name="sAna" file="abc.rml"/>
/// \endcode
/// If the target file is a root file, there will be a different way to load, see 
/// TRestRun::ImportMetadata()
void TRestMetadata::ExpandIncludeFile(TiXmlElement * e)
{
	ReplaceElementAttributes(e);
	const char* _filename = e->Attribute("file");
	if (_filename == NULL)return;

	string filename = SearchFile(_filename);
	if (filename == "") {
		warning << "REST WARNING(expand include file): Include file \"" << _filename << "\" does not exist!" << endl;
		warning << endl;
		return;
	}
	if (!isRootFile(filename)) //root file inclusion is implemented in TRestRun
	{
		debug << "----expanding include file----" << endl;
		//we find the local element(the element to receive content) 
		//and the remote element(the element to provide content)
		TiXmlElement* remoteele = NULL;
		TiXmlElement* localele = NULL;
		string type;
		string name;

		////////////////////////////////////////////////
		//condition 1(raw file include): 
		//   <TRestXXX name="" .....>
		//     <include file="aaa.rml"/>
		//     ....
		//   </TRestXXX>
		//
		//We will insert all the xml elements in aaa.rml into this section
		if ((string)e->Value() == "include")
		{
			localele = (TiXmlElement*)e->Parent();
			if (localele == NULL)return;
			if (localele->Attribute("expanded") == NULL ? false : ((string)localele->Attribute("expanded") == "true"))
			{
				debug << "----already expanded----" << endl;
				return;
			}

			remoteele = new TiXmlElement("Config");

			TiXmlElement*ele = GetRootElementFromFile(filename);
			if (ele == NULL)warning << "REST Waring: no xml elements contained in the include file \"" << filename << "\"" << endl;
			while (ele != NULL) {
				remoteele->InsertEndChild(*ele);
				ele = ele->NextSiblingElement();
			}

		}

		///////////////////////////////////
		//condition 2(auto insert): 
		//   <TRestXXX file=""/>
		//or
		//   <TRestXXX name="" ... file="aaa.rml" .../>
		//or
		//   <addXXX name="" ... file="aaa.rml" .../>
		//or
		//   <addXXX type="" name="" ... file="aaa.rml" .../>
		//
		//Here TRestXXX will be "type". we will find the corresponding section, and insert all 
		//its attributes and child elements into this section. "name" overwrites "type"
		else
		{
			localele = e;
			if (localele->Attribute("expanded") == NULL ? false : ((string)localele->Attribute("expanded") == "true"))
			{
				debug << "----already expanded----" << endl;
				return;
			}

			type = e->Attribute("type") != NULL ? e->Attribute("type") : e->Value();
			name = localele->Attribute("name") == NULL ? "" : localele->Attribute("name");

			//get the root element
			TiXmlElement* rootele = GetRootElementFromFile(filename);
			if (rootele == NULL) {
				warning << "REST WARNING(expand include file): Include file " << filename << " is of wrong xml format!" << endl;
				warning << endl;
				return;
			}
			if ((string)rootele->Value() == type) {
				//if root element in the included file is of given type, directly use it
				remoteele = rootele;
			}
			else {
				//import env first
				if (type != "globals"&&GetElement("globals", rootele) != NULL) {
					TiXmlElement*globaldef = GetElement("globals", rootele)->FirstChildElement();
					while (globaldef != NULL)
					{
						if ((string)globaldef->Value() == "variable" || (string)globaldef->Value() == "myParameter" || (string)globaldef->Value() == "constant")
						{
							SetEnv(globaldef, false);
						}
						globaldef = globaldef->NextSiblingElement();
					}
				}
				//find its child section according to type and name
				if (name != ""&&GetElementWithName("", name, rootele) != NULL)
					remoteele = GetElementWithName("", name, rootele);
				else if (GetElement(type, rootele) != NULL)
					remoteele = GetElement(type, rootele);

				if (remoteele == NULL)
				{
					warning << "REST WARNING(expand include file): Cannot get corresponding xml section!" << endl;
					warning << "type: " << type << " , name: " << name << " . Skipping" << endl;
					warning << endl;
					return;
				}
			}
		}



		debug << "Target xml element spotted" << endl;

		///////////////////////////////////////
		//begin inserting remote element into local element
		ExpandElement(remoteele, true);
		int nattr = 0;
		int nele = 0;
		TiXmlAttribute*attr = remoteele->FirstAttribute();
		while (attr != NULL) {
			if (localele->Attribute(attr->Name()) == NULL)
			{
				localele->SetAttribute(attr->Name(), attr->Value());
				nattr++;
			}
			attr = attr->Next();
		}
		TiXmlElement* ele = remoteele->FirstChildElement();
		while (ele != NULL)
		{
			//ExpandElement(ele);
			if ((string)ele->Value() != "for")
			{
				localele->InsertEndChild(*ele);
				nele++;
			}
			ele = ele->NextSiblingElement();
		}

		localele->SetAttribute("expanded", "true");
		if (fVerboseLevel >= REST_Debug) {
			localele->Print(stdout, 0);
			cout << endl;
		}
		debug << nattr << " attributes and " << nele << " xml elements added by inclusion" << endl;
		debug << "----end of expansion file----" << endl;
	}
}

///////////////////////////////////////////////
/// \brief Returns the value for the parameter name **parName** found in fElement(main data element)
/// 
/// It first finds the parameter from the system's env, if the name matches it will directly return the env.
/// If no env is available, it calls GetParameter() method, specifying the search element
/// TRestMetadata::fElement. See more detail there.
///
/// \param parName The name of the parameter from which we want to obtain the value.
/// \param defaultValue The default value if the paremeter is not found
///
/// \return A string of result
string TRestMetadata::GetParameter(std::string parName, TString defaultValue)
{
	//first search the parameter in system env
	char* val = getenv(parName.c_str());
	if (val != NULL)
	{
		return val;
	}

	//then look within local xml element
	return GetParameter(parName, fElement, defaultValue);

}


///////////////////////////////////////////////
/// \brief Returns the value for the parameter name **parName** found in fElement(main data element)
/// 
/// There are two kinds of *parameter* in REST.
/// 1. <parameter name="verboseLevel" value="silent" />
/// 2. <addReadoutModule id="0" name="module" rotation = "0" firstDaqChannel = "272" / >
///
/// The first one is obviously a parameter. The xml element itself serves as a peice of parameter. The name
/// and the value are given in its fields. This is the classic definition.
/// We also generalize the concept of parameter to the elements' fields.
/// All the fields in an element can be seen as parameter. So there are 4 parameters in the second example, 
/// including: id, name, rotation and firstDaqChannel. This method first finds parameter in the
/// fields of the given element. If not find, it searches its the child elements. If still not find, 
/// it returns the default value. If not specified, the default value string is "NO_SUCH_PARA".
///
/// \param parName The name of the parameter from which we want to obtain the value.
/// \param e The target eml element where the program is to search the parameter
/// \param defaultValue The default value if the paremeter is not found
///
/// \return A string of result, with env and expressions replaced
string TRestMetadata::GetParameter(std::string parName, TiXmlElement* e, TString defaultValue)
{
	if (e == NULL) {
		if (GetVerboseLevel() > REST_Debug) { cout << "Element is null" << endl; }
		return (string)defaultValue;
	}
	string result = (string)defaultValue;
	//first find in attribute
	if (e->Attribute(parName.c_str()) != NULL) {
		result = e->Attribute(parName.c_str());
	}
	//then find in child sections/elements 
	else
	{
		TiXmlElement* element = GetElementWithName("parameter", parName, e);
		if (element != NULL&&element->Attribute("value") != NULL) {
			result = element->Attribute("value");
		}
		else
		{
			if (GetVerboseLevel() > REST_Debug) { cout << "Parameter not found!" << endl; }
		}
	}

	return ReplaceMathematicalExpressions(ReplaceEnvironmentalVariables(result));
}

///////////////////////////////////////////////
/// \brief Returns the field value of an xml element which has the specified name. 
///
/// A version of GetParameter() but only find parameter in the fields of xml element.
///
std::string TRestMetadata::GetFieldValue(std::string parName, TiXmlElement* e)
{
	return GetParameter(parName, e, "Not defined");
}


///////////////////////////////////////////////
/// \brief Gets the value of the parameter name **parName**, after applying unit conversion.
///
/// Searches the parameter in given element. The parameter must be defined providing the additional 
/// units, in the same field value or some where in the element. As in the following example :
///
/// \code <parameter name="electricField" value="1kV/m"/> \endcode
/// \code <parameter name="electricField" value="1,kV/m"/> \endcode
/// \code <parameter name="electricField" value="1,units=kV/m"/> \endcode
/// \code <parameter name="electricField" value="1" units="kV/m" /> \endcode
/// Or
/// \code <TRestDetectorSetup electricField="1kV/m"/> \endcode
/// \code <TRestDetectorSetup electricField="1,kV/m"/> \endcode
/// \code <TRestDetectorSetup electricField="1,units=kV/m"/> \endcode
/// \code <TRestDetectorSetup electricField="1" units="kV/m"/> \endcode
///
/// We recomment defining units in the same field value, which makes things clear.
///
/// \param parName The name of the parameter from which we want to obtain the value.
/// \param ele The target element in which we are going to search.
/// \param defaultVal The default return value if it fails to find such parameter with unit.
///
/// \return A double/2DVector/3DVector value in the default correspoding REST units (keV, us, mm, Vcm).
///
Double_t TRestMetadata::GetDblParameterWithUnits(std::string parName, TiXmlElement* ele, Double_t defaultVal)
{
	string a = GetParameter(parName, ele);
	if (a == PARAMETER_NOT_FOUND_STR)
	{
		return defaultVal;
	}
	else
	{
		string units = GetUnits(ele, parName);
		double val = StringToDouble(a.substr(0, a.find_last_of("1234567890()") + 1));
		return REST_Units::GetValueInRESTUnits(val, units);
	}

	return defaultVal;
}
Double_t TRestMetadata::GetDblParameterWithUnits(std::string parName, Double_t defaultVal) {
	return GetDblParameterWithUnits(parName, fElement, defaultVal);
}
TVector2 TRestMetadata::Get2DVectorParameterWithUnits(std::string parName, TiXmlElement* ele, TVector2 defaultVal)
{
	string a = GetParameter(parName, ele);
	if (a == PARAMETER_NOT_FOUND_STR)
	{
		return defaultVal;
	}
	else
	{
		string unit = GetUnits(ele, parName);
		TVector2 value = StringTo2DVector(a.substr(0, a.find_last_of("1234567890().") + 1));
		Double_t valueX = REST_Units::GetValueInRESTUnits(value.X(), unit);
		Double_t valueY = REST_Units::GetValueInRESTUnits(value.Y(), unit);
		return TVector2(valueX, valueY);
	}

	return defaultVal;

}
TVector2 TRestMetadata::Get2DVectorParameterWithUnits(std::string parName, TVector2 defaultVal) {
	return Get2DVectorParameterWithUnits(parName, fElement,defaultVal);
}
TVector3 TRestMetadata::Get3DVectorParameterWithUnits(std::string parName, TiXmlElement* ele, TVector3 defaultVal)
{
	string a = GetParameter(parName, ele);
	if (a == PARAMETER_NOT_FOUND_STR)
	{
		return defaultVal;
	}
	else
	{
		string unit = GetUnits(ele, parName);
		TVector3 value = StringTo3DVector(a.substr(0, a.find_last_of("1234567890().") + 1));
		Double_t valueX = REST_Units::GetValueInRESTUnits(value.X(), unit);
		Double_t valueY = REST_Units::GetValueInRESTUnits(value.Y(), unit);
		Double_t valueZ = REST_Units::GetValueInRESTUnits(value.Z(), unit);
		return TVector3(valueX, valueY,valueZ);
	}

	return defaultVal;

}
TVector3 TRestMetadata::Get3DVectorParameterWithUnits(std::string parName, TVector3 defaultVal) {
	return Get3DVectorParameterWithUnits(parName, fElement,defaultVal);
}

///////////////////////////////////////////////
/// \brief Open an xml encoded file and get its root element. 
///
/// The root element is the parent of any other xml elements in the file. 
/// There could be only one root element in each xml encoded file.
///
/// Exits the whole program if the xml file does not exist or is in wrong in syntax.
///
TiXmlElement* TRestMetadata::GetRootElementFromFile(std::string cfgFileName)
{
	TiXmlDocument* doc = new TiXmlDocument();

	if (!fileExists(cfgFileName)) {
		error << "Config file does not exist. The file is: " << cfgFileName << endl;
		GetChar();
		exit(1);
	}
	if (!doc->LoadFile(cfgFileName.c_str()))
	{
		if (TRestMetadata_ConfigFileUpdated) {
			error << "Failed to load xml file, syntax maybe wrong. The file is: " << cfgFileName << endl;
			int result = system(("xmllint " + cfgFileName + "> /tmp/xmlerror.txt").c_str());

			if (result == 256) { system("cat /tmp/xmlerror.txt"); }
			else { error << "To do syntax check for the file, please install the package \"xmllint\"" << endl; }
			system("rm /tmp/xmlerror.txt");

			GetChar();
			exit(1);
		}
		else {
			//in future we will implement rml version update tool in c++
			string newcfgGileName = cfgFileName;
			TRestMetadata_ConfigFileUpdated = true;
			GetRootElementFromFile(newcfgGileName);
		}
	}

	TiXmlElement* root = doc->RootElement();
	if (root != NULL) {
		return root;
	}
	else
	{
		error << "Succeeded to load xml file, but no element contained" << endl;
		GetChar();
		exit(1);
	}

}

///////////////////////////////////////////////
/// \brief Get an xml element from default location(TRestMetadata::fElement), according to its declaration
///
TiXmlElement * TRestMetadata::GetElement(std::string eleDeclare)
{
	return GetElement(eleDeclare, fElement);
}

///////////////////////////////////////////////
/// \brief Get an xml element from a given parent element, according to its declaration
///
TiXmlElement * TRestMetadata::GetElement(std::string eleDeclare, TiXmlElement * e)
{
	//cout << eleDeclare << " " << e << endl;
	TiXmlElement* ele = e->FirstChildElement(eleDeclare.c_str());
	while (ele != NULL)
	{
		string a = ele->Value();
		if (a == eleDeclare)
			break;
		ele = ele->NextSiblingElement();
	}
	return ele;
}

///////////////////////////////////////////////
/// \brief Get an xml element from the root element of a xml encoded file, according to its declaration
///
TiXmlElement * TRestMetadata::GetElement(std::string eleDeclare, std::string cfgFileName)
{
	TiXmlElement* root = GetRootElementFromFile(cfgFileName);
	if ((string)root->Value() == eleDeclare) {
		return root;
	}
	return GetElement(eleDeclare, root);
}

///////////////////////////////////////////////
/// \brief Get an xml element from the default location, according to its declaration and its field "name"
///
TiXmlElement * TRestMetadata::GetElementWithName(std::string eleDeclare, std::string eleName)
{
	return GetElementWithName(eleDeclare, eleName, fElement);
}

///////////////////////////////////////////////
/// \brief Get an xml element from a given parent element, according to its declaration and its field "name"
///
TiXmlElement * TRestMetadata::GetElementWithName(std::string eleDeclare, std::string eleName, TiXmlElement * e)
{
	if (eleDeclare == "") {
		TiXmlElement* ele = e->FirstChildElement();
		while (ele != NULL)
		{
			if (ele->Attribute("name") != NULL && (string)ele->Attribute("name") == eleName)
			{
				return ele;
			}
			ele = ele->NextSiblingElement();
		}
		return ele;
	}
	else
	{
		TiXmlElement* ele = e->FirstChildElement(eleDeclare.c_str());
		while (ele != NULL)
		{
			if (ele->Attribute("name") != NULL && (string)ele->Attribute("name") == eleName)
			{
				return ele;
			}
			ele = ele->NextSiblingElement(eleDeclare.c_str());
		}
		return ele;
	}

	return NULL;
}

///////////////////////////////////////////////
/// \brief Returns a string with the unit name provided inside the given element.
/// 
/// It calls the method GetUnits(TiXmlElement,string) with the current elemnet.
string TRestMetadata::GetUnits(string whoseunits)
{
	return GetUnits(fElement, whoseunits);
}

///////////////////////////////////////////////
/// \brief Returns a string with the unit name given in the given xml element
/// 
/// if given the target attribute, it will find the unit for this.
/// e.g. value="(1,-13)mm"  "-3mm"  "50,units=mm"  can both be recoginzed.
/// if not given, it will find the unit as a parameter of the element.
///	e.g. <... value="3" units="mm" .../>
string TRestMetadata::GetUnits(TiXmlElement* e, string whoseunits)
{
	string unitstring = "";
	if (whoseunits == "") {
		unitstring = GetParameter("units", e);
		if (IsUnit(unitstring)) {
			debug << "Found unit definition \"" << unitstring << "\" in element " << e->Value() << endl;
			debug << endl;
			return unitstring;
		}
		else
		{
			warning << "No units are defined in element "<<e->Value()<<" , returning blank unit" << endl;
			warning << "The field value will be directly returned with blank unit." << endl;
			warning << endl;
			return "";
		}
	}
	else
	{
		string a = GetParameter(whoseunits,e);
		unitstring = REST_Units::GetRESTUnitsInString(a);

		if (IsUnit(unitstring)) {
			return unitstring;
		}
		else
		{
			debug << "Parameter \"" << whoseunits << " = " << a << "\" dose not contain any units" << endl;
			debug << "Trying to find unit in element..." << endl;
			if (GetElementWithName("parameter", whoseunits, e) != NULL) {
				return GetUnits(GetElementWithName("parameter", whoseunits, e), "");
			}
			else
			{
				return GetUnits(e, "");
			}
		}
	}

	return unitstring;
}


///////////////////////////////////////////////
/// \brief Parsing a string into TiXmlElement object
/// 
/// This method creates TiXmlElement object with the alloator "new". 
/// Be advised to delete the object after using it!
TiXmlElement* TRestMetadata::StringToElement(string definition)
{
	TiXmlElement*ele = new TiXmlElement("temp");
	//TiXmlDocument*doc = new TiXmlDocument();
	ele->Parse(definition.c_str(), NULL, TIXML_ENCODING_UTF8);
	return ele;
}

///////////////////////////////////////////////
/// \brief Convert an TiXmlElement object to string
/// 
/// This method does't arrange the output. All the contents are written in one line.
string TRestMetadata::ElementToString(TiXmlElement*ele)
{
	if (ele != NULL) {

		//remove comments
		TiXmlNode*n = ele->FirstChild();
		while (n != NULL) {
			TiXmlComment*cmt = n->ToComment();
			if (cmt != NULL) {
				TiXmlNode*nn = n;
				n = n->NextSibling();
				ele->RemoveChild(nn);
				continue;
			}
			n = n->NextSibling();
		}

		stringstream ss;
		ss << (*ele);
		string s = ss.str();

		//int pos = 0;
		//int pos2 = 0;
		//while ((pos = s.find("<!--", pos)) != -1 && (pos2 = s.find("-->", pos)) != -1)
		//{
		//	s.replace(pos, pos2 - pos + 3, "");//3 is the length of "-->"
		//	pos = pos + 1;
		//}

		return s;
	}


	return " ";
}

///////////////////////////////////////////////
/// \brief Gets the first key structure for **keyName** found inside **buffer** after **fromPosition**.
///
/// A key definition is written as follows:
/// \code <keyName field1="value1" field2="value2" > 
///
///     ....
///
///  </keyName>
/// \endcode
///
string TRestMetadata::GetKEYStructure(std::string keyName)
{
	size_t Position = 0;
	string result = GetKEYStructure(keyName, Position, fElement);
	if (result == "") result = "NotFound";
	return result;
}
string TRestMetadata::GetKEYStructure(std::string keyName, size_t &Position)
{
	string result = GetKEYStructure(keyName, Position, fElement);
	if (result == "") result = "NotFound";
	return result;
}
string TRestMetadata::GetKEYStructure(std::string keyName, string buffer)
{
	size_t Position = 0;
	string result = GetKEYStructure(keyName, Position, buffer);
	if (result == "") result = "NotFound";
	return result;
}
string TRestMetadata::GetKEYStructure(std::string keyName, size_t &fromPosition, string buffer) {
	TiXmlElement*ele = StringToElement(buffer);
	string result= GetKEYStructure(keyName, fromPosition, ele);
	delete ele;
	return result;
}
string TRestMetadata::GetKEYStructure(std::string keyName, size_t &fromPosition, TiXmlElement*ele) {
	size_t position = fromPosition;

	debug << "Finding " << fromPosition << "th appearance of KEY Structure \"" << keyName << "\"..." << endl;

	TiXmlElement*childele = ele->FirstChildElement(keyName);
	for (int i = 0; childele != NULL && i < fromPosition; i++)
	{
		childele = childele->NextSiblingElement(keyName);
	}
	if (childele != NULL) {
		string result = ElementToString(childele);
		//fromPosition = fromPosition + i + 1;
		debug << "Found Key : " << result << endl;
		//debug << "New position : " << fromPosition << endl;
		return result;
	}

	debug << "Finding hit the end, KEY Structure not found!!" << endl;
	return "";
}

///////////////////////////////////////////////
/// \brief Gets the first key definition for **keyName** found inside **buffer** starting at **fromPosition**.
///
/// A key definition is written as follows:
/// \code <keyName field1="value1" field2="value2" > \endcode
/// 
/// The returned key definition will be fixed to:
/// \code <keyName field1="value1" field2="value2" /> \endcode
/// which is in standard xml form
string TRestMetadata::GetKEYDefinition(string keyName)
{
	string buffer = ElementToString(fElement);
	size_t Position = 0;
	return GetKEYDefinition(keyName, Position, buffer);
}
string TRestMetadata::GetKEYDefinition(string keyName, size_t &fromPosition)
{
	string buffer = ElementToString(fElement);
	return GetKEYDefinition(keyName, fromPosition, buffer);
}
string TRestMetadata::GetKEYDefinition(string keyName, string buffer)
{
	size_t Position = 0;
	return GetKEYDefinition(keyName, Position, buffer);
}
string TRestMetadata::GetKEYDefinition(string keyName, size_t &fromPosition, string buffer)
{
	string key = "<" + keyName;
	size_t startPos = buffer.find(key, fromPosition);
	if (startPos == string::npos) return "";
	size_t endPos = buffer.find(">", startPos);
	if (endPos == string::npos) return "";

	fromPosition = endPos;

	Int_t notDefinitionEnd = 1;

	while (notDefinitionEnd)
	{
		// We might find a problem when we insert > symbol inside a field value.
		// As for example: condition=">100" This patch checks if the definition 
		// finishes in "= If it is the case it searches the next > symbol ending 
		// the definition.

		string def = RemoveWhiteSpaces(buffer.substr(startPos, endPos - startPos));

		if ((TString)def[def.length() - 1] == "\"" && (TString)def[def.length() - 2] == "=")
			endPos = buffer.find(">", endPos + 1);
		else
			notDefinitionEnd = 0;
	}

	string result= buffer.substr(startPos, endPos - startPos+1);
	if(result[result.size()-2]!='/')
		result.insert(result.size() - 1, 1, '/');
	//cout << result << endl << endl;
	//getchar();
	return result;
}

///////////////////////////////////////////////
/// \brief Gets field value in an xml element string by parsing it as TiXmlElement
///
std::string TRestMetadata::GetFieldValue(std::string fieldName, std::string definition, size_t fromPosition)
{
	TiXmlElement*ele = StringToElement(definition);
	string value = GetFieldValue(fieldName, ele);
	delete ele;
	return value;
}
Double_t TRestMetadata::GetDblFieldValueWithUnits(string fieldName, string definition, size_t fromPosition)
{
	TiXmlElement*ele = StringToElement(definition);
	TiXmlElement*e = ele->FirstChildElement();
	while (e != NULL) {
		TiXmlElement*tmp = e;
		e = e->NextSiblingElement();
		ele->RemoveChild(tmp);
	}
	auto value= GetDblParameterWithUnits(fieldName,ele);
	delete ele;
	return value;
}
TVector2 TRestMetadata::Get2DVectorFieldValueWithUnits(string fieldName, string definition, size_t fromPosition)
{
	TiXmlElement*ele = StringToElement(definition);
	TiXmlElement*e = ele->FirstChildElement();
	while (e != NULL) {
		TiXmlElement*tmp = e;
		e = e->NextSiblingElement();
		ele->RemoveChild(tmp);
	}
	auto value= Get2DVectorParameterWithUnits(fieldName, ele);
	delete ele;
	return value;
}
TVector3 TRestMetadata::Get3DVectorFieldValueWithUnits(string fieldName, string definition, size_t fromPosition)
{
	TiXmlElement*ele = StringToElement(definition);
	TiXmlElement*e = ele->FirstChildElement();
	while (e != NULL) {
		TiXmlElement*tmp = e;
		e = e->NextSiblingElement();
		ele->RemoveChild(tmp);
	}
	auto value = Get3DVectorParameterWithUnits(fieldName, ele);
	delete ele;
	return value;
}

///////////////////////////////////////////////
/// \brief Returns the value for the parameter name **parName** found in **inputString**. 
/// 
/// The methods starts searching in **inputString** after a given position **pos**.
///
string TRestMetadata::GetParameter(string parName, size_t &pos, string inputString)
{
	pos = inputString.find(parName, pos);

	TiXmlElement*ele = StringToElement(inputString);

	string value = GetParameter(parName, ele);

	delete ele;

	return value;
}

///////////////////////////////////////////////
/// \brief Gets the double value of the parameter name **parName**, found in **inputString**, after applying unit conversion.
///
/// The parameter must defined providing the additional field units just behind the parameter value. As in the following example :
///
/// \code <parameter name="electricField" value="1" units="kVm" > \endcode
///
/// \param parName The name of the parameter from which we want to obtain the value.
/// \param pos Defines the position inside **inputString** where to start searching the definition of **parName**.
///
/// \return A double value in the default correspoding REST units (keV, us, mm, Vcm).
///
Double_t TRestMetadata::GetDblParameterWithUnits(std::string parName, size_t &pos, std::string inputString)
{
	pos = inputString.find(parName, pos);

	TiXmlElement*ele = StringToElement(inputString);

	double value = GetDblParameterWithUnits(parName, ele);

	delete ele;

	return value;
}

///////////////////////////////////////////////
/// \brief Returns a 2D vector value of the parameter name **parName**, found in **inputString**, after applying unit conversion.
///
/// The parameter must defined providing the additional field units just behind the parameter value. As in the following example :
///
/// \code <parameter name="position" value="(10,0)" units="mm" > \endcode
///
/// \param parName The name of the parameter from which we want to obtain the value.
/// \param pos Defines the position inside **inputString** where to start searching the definition of **parName**.
///
/// \return A 2D vector value in the default correspoding REST units (keV, us, mm, Vcm).
///
TVector2 TRestMetadata::Get2DVectorParameterWithUnits(std::string parName, size_t &pos, std::string inputString)
{
	pos = inputString.find(parName, pos);

	TiXmlElement*ele = StringToElement(inputString);

	TVector2 value = Get2DVectorParameterWithUnits(parName, ele);

	delete ele;

	return value;
}

///////////////////////////////////////////////
/// \brief Returns a 3D vector value of the parameter name **parName**, found in **inputString**, after applying unit conversion.
///
/// The parameter must defined providing the additional field units just behind the parameter value. As in the following example :
///
/// \code <parameter name="position" value="(10,0,-10)" units="mm" > \endcode
///
/// \param parName The name of the parameter from which we want to obtain the value.
/// \param pos Defines the position inside **inputString** where to start searching the definition of **parName**.
///
/// \return A 3D vector value in the default correspoding REST units (keV, us, mm, Vcm).
///
TVector3 TRestMetadata::Get3DVectorParameterWithUnits(std::string parName, size_t &pos, std::string inputString)
{
	pos = inputString.find(parName, pos);

	TiXmlElement*ele = StringToElement(inputString);

	TVector3 value = Get3DVectorParameterWithUnits(parName, ele);

	delete ele;

	return value;
}


///////////////////////////////////////////////
/// \brief Identifies enviromental variable replacing marks in the input buffer, and replace them with corresponding value.
///
/// Replacing marks: 
/// 1. $ENV{VARIABLE_NAME} : search the system env and replace it if found.
/// 2. ${VARIABLE_NAME} : search both system env and program env and replace it if found.
/// 3. [VARIABLE_NAME] : search only program env and replace it if found. This is used for for loop.
/// 4. VARIABLE_NAME    : try match the names of myParameter or constant and replace it if matched.
string TRestMetadata::ReplaceEnvironmentalVariables(const string buffer)
{
	string outputBuffer = buffer;

	//replace system env only
	int startPosition = 0;
	int endPosition = 0;
	while ((startPosition = outputBuffer.find("$ENV{", endPosition)) != (int)string::npos)
	{
		char envValue[256];
		endPosition = outputBuffer.find("}", startPosition + 1);
		if (endPosition == (int)string::npos) break;

		string expression = outputBuffer.substr(startPosition + 5, endPosition - startPosition - 5);

		if (getenv(expression.c_str()) != NULL)
		{
			sprintf(envValue, "%s", getenv(expression.c_str()));
			outputBuffer.replace(startPosition, endPosition - startPosition + 1, envValue);
			endPosition = 0;
		}
		else
		{
			debug << "cannot find \"$ENV{" << expression << "}\" in system env, returning raw expression..."<< endl;
		}
	}

	//replace env with mark ${}
	startPosition = 0;
	endPosition = 0;
	while ((startPosition = outputBuffer.find("${", endPosition)) != (int)string::npos)
	{
		endPosition = outputBuffer.find("}", startPosition + 2);
		if (endPosition == (int)string::npos) break;

		string expression = outputBuffer.substr(startPosition + 2, endPosition - startPosition - 2);

		int replacePos=startPosition;
		int replaceLen=endPosition-startPosition+1;

		char* sysenv = getenv(expression.c_str());
		char* proenv = NULL;
		int envindex = 0;
		for (int i = 0; i < fElementEnv.size(); i++) {
			if ((string)fElementEnv[i]->Value() == "variable"&&expression == (string)fElementEnv[i]->Attribute("name"))
			{
				if (fElementEnv[i]->Attribute("value") != NULL)
				{
					proenv = const_cast<char*>(fElementEnv[i]->Attribute("value"));
					envindex = i;
					break;
				}
			}
		}

		if (proenv != NULL)
		{
			outputBuffer.replace(replacePos, replaceLen, proenv);
			endPosition = 0;
		}
		else if (sysenv != NULL)
		{
			outputBuffer.replace(replacePos, replaceLen, sysenv);
			endPosition = 0;
		}
		else
		{
			debug << "replace env " << startPosition << ": cannot find \"${" << expression << "}\" in system and program env, returning raw expression..." << endl;
		}
	}

	//replace only program env with mark []
	startPosition = 0;
	endPosition = 0;
	while ((startPosition = outputBuffer.find("[", endPosition)) != (int)string::npos)
	{
		endPosition = outputBuffer.find("]", startPosition + 1);
		if (endPosition == (int)string::npos) break;

		string expression = outputBuffer.substr(startPosition + 1, endPosition - startPosition - 1);

		int replacePos = startPosition;
		int replaceLen = endPosition - startPosition + 1;
		if (startPosition != 0 && outputBuffer[startPosition - 1] == '$') {
			replacePos = startPosition - 1;
			replaceLen = endPosition - startPosition + 2;
		}

		bool replaced = false;
		for (int i = 0; i < fElementEnv.size(); i++) {
			if ((string)fElementEnv[i]->Value() == "variable"&&expression == (string)fElementEnv[i]->Attribute("name"))
			{
				if (fElementEnv[i]->Attribute("value") != NULL)
				{
					outputBuffer.replace(replacePos, replaceLen, fElementEnv[i]->Attribute("value"));
					replaced = true;
					break;
				}
			}
		}

		if(!replaced)
		{
			debug << "replace env " << startPosition << ": cannot find \"[" << expression << "]\" for for loop, returning raw expression..."<< endl;
		}
	}

	//replace myParameter
	startPosition = 0;
	endPosition = 0;
	for (int i = 0; i < fElementEnv.size(); i++) {
		if ((string)fElementEnv[i]->Value() == "myParameter"|| (string)fElementEnv[i]->Value() == "constant")
		{
			outputBuffer = Replace(outputBuffer, (string)fElementEnv[i]->Attribute("name"), fElementEnv[i]->Attribute("value"), 0);
		}
	}

	return outputBuffer;
}

///////////////////////////////////////////////
/// \brief Set the program env with given env name, value and overwrite permission.
///
/// It will change the value if the env already exists and the overwrite permission is true.
/// Otherwise it will generate a new TiXmlElement object and save it at the end of the env list.
void TRestMetadata::SetEnv(string name, string value, bool overwriteexisting)
{
	for (int i = 0; i < fElementEnv.size(); i++) {
		if ((string)fElementEnv[i]->Value() == "variable" && (string)fElementEnv[i]->Attribute("name") == name) {
			if (overwriteexisting) {
				fElementEnv[i]->SetAttribute("value", value.c_str());
			}
			return;
		}
	}
	TiXmlElement* e = new TiXmlElement("variable");
	e->SetAttribute("name", name.c_str());
	e->SetAttribute("value", value.c_str());
	fElementEnv.push_back(e);


}

///////////////////////////////////////////////
/// \brief Search files in current directory and directories specified in "searchPath" section
///
/// Return blank string if file not found, return directly filename if found in current 
/// directory, return full name (path+name) if found in "searchPath". 
string TRestMetadata::SearchFile(string filename) {
	if (fileExists(filename)) {
		return filename;
	}
	else
	{
		
		auto pathstring = GetSearchPath();
		auto paths = Spilt((string)pathstring, ":");
		return SearchFileInPath(paths, filename);
	}
}

///////////////////////////////////////////////
/// \brief Prints a UNIX timestamp in human readable format.
///
void TRestMetadata::PrintTimeStamp(Double_t timeStamp)
{
	cout.precision(10);

	time_t tt = (time_t)timeStamp;
	struct tm *tm = localtime(&tt);

	char date[20];
	strftime(date, sizeof(date), "%Y-%m-%d", tm);
	cout << "Date : " << date << endl;

	char time[20];
	strftime(time, sizeof(time), "%H:%M:%S", tm);
	cout << "Time : " << time << endl;
	cout << "++++++++++++++++++++++++" << endl;
}

///////////////////////////////////////////////
/// \brief Prints current config buffer on screen
///
void TRestMetadata::PrintConfigBuffer()
{
	if (fElement != NULL)
	{
		fElement->Print(stdout, 0);
		cout << endl;
	}
	else
	{
		cout << GetSectionContent() << endl;
	}


}

int TRestMetadata::GetChar(string hint) 
{ 
	if (gApplication!=NULL&&!gApplication->IsRunning()) {
		thread t = thread(&TApplication::Run, gApplication, true);
		t.detach();
		
		cout << hint << endl;
		int result = getchar();
		gSystem->ExitLoop();
		return result;
	}
	else
	{
		cout << hint << endl;
		return getchar();
	}
	return -1;
}

///////////////////////////////////////////////
/// \brief Prints metadata content on screen. Usually overloaded by the derived metadata class.
///
void TRestMetadata::PrintMetadata()
{
	TRestStringOutput cout;
	cout.setborder("||");
	cout.setorientation(1);
	cout.setlength(100);
	cout << "+++++++++++++++++++++++++++++++++++++++++++++" << endl;
	cout << this->ClassName() << " content" << endl;
	cout << "Config file : " << fConfigFileName << endl;
	cout << "+++++++++++++++++++++++++++++++++++++++++++++" << endl;
	cout << "Name : " << GetName() << endl;
	cout << "Title : " << GetTitle() << endl;
	cout << "---------------------------------------" << endl;
}

///////////////////////////////////////////////
/// \brief Returns the section name of this class, defined at the beginning of fSectionName
std::string TRestMetadata::GetSectionName()
{
	auto a = fSectionName.find('\n', 0);
	if (a != -1)
		return fSectionName.substr(0, a);
	return fSectionName;
}

///////////////////////////////////////////////
/// \brief Returns the config section of this class, defined after section name in fSectionName
std::string TRestMetadata::GetSectionContent()
{
	auto a = fSectionName.find('\n', 0);
	if (a != -1)
		return fSectionName.substr(a + 1, -1);
	return configBuffer;
}

///////////////////////////////////////////////
/// \brief Returns a string corresponding to current verbose level.
///
TString TRestMetadata::GetVerboseLevelString()
{
	TString level = "unknown";
	if (this->GetVerboseLevel() == REST_Debug) level = "debug";
	if (this->GetVerboseLevel() == REST_Info) level = "info";
	if (this->GetVerboseLevel() == REST_Essential) level = "warning";
	if (this->GetVerboseLevel() == REST_Silent) level = "silent";

	return level;
}

///////////////////////////////////////////////
/// Returns a string with the path defined in sections "searchPath". 
/// 
/// To add a searchPath, use:
/// \code
/// <searchPath value="$ENV{REST_INPUTDATA}/definitions/"/>
/// \endcode
/// Or
/// \code
/// <searchPath value="$ENV{REST_INPUTDATA}/definitions/:$ENV{REST_INPUTDATA}/gasFiles/"/>
/// \endcode
/// "searchPath" can also be added multiple times. Both of them will be added into the output string.
/// A separator ":" is inserted between each defined paths. To separate them, use inline method
/// Spilt() provided by TRestStringHelper. Uniformed search path definition provides us uniformed 
/// file search tool, see TRestMetadata::SearchFile().
TString TRestMetadata::GetSearchPath() {
	TiXmlElement*e = fElement;
	//string result = "";
	string result = getenv("configPath") == NULL ? ":" : getenv("configPath") + (string)":";
	TiXmlElement* ele = e->FirstChildElement("searchPath");
	while (ele != NULL)
	{
		if (ele->Attribute("value") != NULL) {
			result += (string)ele->Attribute("value") + ":";
		}
		ele = ele->NextSiblingElement("searchPath");
	}
	if (result[result.size() - 1] == ':') {
		result.erase(result.size() - 1);
	}

	return ReplaceEnvironmentalVariables(result);
}


TClass*c;
TVirtualStreamerInfo *vs;
TObjArray* ses;
///////////////////////////////////////////////
/// \brief Reflection methods, Get the class's datamember info according to its name. 
///
/// The info returned is wrapped in TStreamerElement
TStreamerElement* TRestMetadata::GetDataMemberWithName(string name)
{
	int n = GetNumberOfDataMember();
	for (int i = 0; i < n; i++) {
		TStreamerElement *se = (TStreamerElement*)ses->At(i);
		if ((string)se->GetFullName() == name)
		{
			return se;
		}
	}
	return NULL;

}

///////////////////////////////////////////////
/// \brief Reflection methods, Get the class's datamember info according to its id. 
///
TStreamerElement* TRestMetadata::GetDataMemberWithID(int ID)
{
	int n = GetNumberOfDataMember();
	if (ID < n)return (TStreamerElement*)ses->At(ID);
	return NULL;
}

///////////////////////////////////////////////
/// \brief Reflection methods, Get the number of the class's datamembers
///
int TRestMetadata::GetNumberOfDataMember()
{
	if (c==NULL||(c!=NULL&&c->GetName()!=this->ClassName())) {
		c = this->IsA();
		vs = c->GetStreamerInfo();
		ses = vs->GetElements();
	}
	return ses->GetLast() + 1;
}

///////////////////////////////////////////////
/// \brief Reflection methods, Get the value of a datamember assuming it is double type
///
double TRestMetadata::GetDblDataMemberVal(TStreamerElement*ele)
{
	if (ele != NULL&&ele->GetType() == 8)
		return *(double*)((char*)this + ele->GetOffset());
	return 0;
}

///////////////////////////////////////////////
/// \brief Reflection methods, Get the value of a datamember assuming it is int type
///
int TRestMetadata::GetIntDataMemberVal(TStreamerElement*ele)
{
	if (ele != NULL&&ele->GetType() == 3)
		return *(int*)((char*)this + ele->GetOffset());
	return 0;
}

///////////////////////////////////////////////
/// \brief Reflection methods, Get the address of a datamember(class address+offset)
///
char* TRestMetadata::GetDataMemberRef(TStreamerElement*ele) {
	if (ele == NULL)return 0;
	return ((char*)this + ele->GetOffset());
}

///////////////////////////////////////////////
/// \brief Reflection methods, Returns a string of the value of a datamember, if supported
///
/// Supported type: double, int, TString. If not supported, returns the type name.
/// This method can be used as a real time inspector of TRestMetadata objects.
string TRestMetadata::GetDataMemberValString(TStreamerElement*ele)
{
	if (ele == NULL)
		return "";
	if (ele != NULL && ele->GetType() == 8)//double
		return ToString(*(double*)((char*)this + ele->GetOffset()));
	if (ele != NULL && ele->GetType() == 3)//int
		return ToString(*(int*)((char*)this + ele->GetOffset()));
	if (ele != NULL && ele->GetType() == 65)//TString
		return ToString(*(TString*)((char*)this + ele->GetOffset()));
	return ele->GetTypeName();
}

///////////////////////////////////////////////
/// \brief Reflection methods, Set the value of a datamember if the type is supported
///
/// Supported type: double, int, TString. The address of target value variable needs 
/// to be given.
void TRestMetadata::SetDataMemberVal(TStreamerElement*ele, char*ptr) {

	if (ele != NULL && ele->GetType() == 8)//double
		*((double*)((char*)this + ele->GetOffset())) = *((double*)ptr);
	if (ele != NULL && ele->GetType() == 3)//int
		*((int*)((char*)this + ele->GetOffset())) = *((int*)ptr);
	if (ele != NULL && ele->GetType() == 65)//TString
		*((TString*)((char*)this + ele->GetOffset())) = *((TString*)ptr);

}

///////////////////////////////////////////////
/// \brief Reflection methods, Set the value of a datamember if the type is supported
///
/// Supported type: double, int, TString. A string of value needs to be given.
void TRestMetadata::SetDataMemberVal(TStreamerElement*ele, string valdef)
{
	if (ele != NULL && ele->GetType() == 8)//double
		*((double*)((char*)this + ele->GetOffset())) = StringToDouble(valdef);
	if (ele != NULL && ele->GetType() == 3)//int
		*((int*)((char*)this + ele->GetOffset())) = StringToInteger(valdef);
	if (ele != NULL && ele->GetType() == 65)//TString
		*((TString*)((char*)this + ele->GetOffset())) = (TString)(valdef);
}

///////////////////////////////////////////////
/// \brief Reflection methods, Set value of a datamember in class according to TRestMetadata::fElement
///
/// It will search for "parameter" section with the same name of the datamember, and set it's value.
/// For example
/// \code
/// class TRestXXX: public TRestMetadata{
/// int par0;
/// }
/// 
/// <TRestXXX name="..." par0="10"/>
/// \endcode
/// After loading the rml file and calling this method, the value of "par0" will be set to 10
/// 
void TRestMetadata::SetDataMemberValFromConfig(TStreamerElement*ele)
{
	if (ele != NULL) {
		if (GetParameter(ele->GetName()) != PARAMETER_NOT_FOUND_STR)
		{
			SetDataMemberVal(ele, GetParameter(ele->GetName()));
		}
	}
}



