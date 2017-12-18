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
/// The rml file is designed to start up/instruct all the metadata classes. The constructor first
/// calls the method Initialize() to do some default settings. This method must be implemented in any
/// non-abstract metadata class. In the Initialize() method, the class needs to define its section name
/// (usually it is just the class name), which will be used to extract the corresponding rml section.
/// 
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
/// The starter is called by calling the method LoadConfigFromFile(). It is implemented in this base class, 
/// with four overloads, as shown in the following. 
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
/// With the xml sections given, The starter first goes through all of them and find out the child sections
/// with key word "variable" and "myParameter". These two are regarded as environmental variables. 
/// The starter will do a check and save them if necessary. Then call the method BeginOfInit() which is 
/// optionally implemented in the derived class. Then the starter loops all the child xml sections, and calls the 
/// ReadConfig() method several times. In this method the pointer to each looped xml section is 
/// given to the derived class, and the derived class choose what to do according to the given element.
/// Finally the EndOfInit() method is called, after which the startup is complete.
/// 
/// The host class can also call the LoadConfigFromFile() method for starting up its resident class. For example, when 
/// received an xml section declared "TRestRun", the "TRestManager" class will pass this section (together with 
/// its global section) to its resident "TRestRun" class. The TRestRun class can therefor perform a startup
/// using these sections. This is called *sequencial startup*.
///
/// \code
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
/// ### Preprocess xml sections and replace variables and expressions
/// 
/// By default, the starter will look into only the first-level child sections of both global 
/// and sectional section. The value of them will be saved in the metadata. In this level the decalration
/// "myParameter" and "variable" is the same. 
/// 
/// \code
/// 
///   <ClassName name="userGivenName" title="User given title" >
///       <myParameter name="nChannels" value="{CHANNELS}" /> //this variable cannot be loaded by the class "ContainedClassName"
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
/// 1. recognize the strings surrounded by "${}". Seek in the system env and replace these strings.
/// 2. recognize the strings surrounded by "{}". Seek in decalration type "variable" and replace these strings.
/// 3. directly replace the strings matching the name of "myParameter" by its value.
/// 4. Judge if the string is an expressions, if so, try evaluate it using TFormula. Replace it by the result.
///
/// The result string of the field value is either a number string or a string with concrete content.
/// In the exapmle code above, the section declared with "parameter" is has its field value reset to string 126.
///
/// ### Including external RML files in a main RML file
///
/// It is possible to link to other rml files in any section. The starter will open the linked file and searches 
/// for the section with the same name of the current section. Then the child sections in that file will be 
/// prepeocessed and sent to ReadConfig() method, just as normal sections. After the including, the main section will be expanded.
/// To include an external rml file one should use xml declaration "include". Then followed by field "file="xxx"".
/// 
/// \code
///
///   <TRestRun name = "TemplateEventProcess" verboseLevel = "silent">
///       <include file = "processes.rml" / >
///       <addProcess type = "TRestMultiCoBoAsAdToSignalProcess" name = "virtualDAQ" value = "ON" / >
///   </TRestRun>
/// \endcode
/// 
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
///	<readoutChannel id = "{nCh}" >
///		<for variable = "nPix" from = "0" to = "nChannels-1" step = "1" >
///			<addPixel id = "{nPix}" origin = "((1+{nCh})*pitch,pitch/4+{nPix}*pitch)" size = "(pixelSize,pixelSize)" rotation = "45" / >
///		</for>
///		<addPixel id = "nChannels" origin = "({nCh}*pitch,pitch/4+(nChannels-1)*pitch+pitch/2)" size = "(pitch+pitch/2,pitch/2)" rotation = "0" / >
///	</readoutChannel>
/// </for>
/// \endcode
///
/// The starter will recongize the fields "variable", "from", "to", "step" in the header of the 
/// for loop definition. The variable "nCh", definded at the header of the for loop definition, 
/// will be added to the environment variable list. The value of the variable will be updated in each loop
/// The content of the loop will be normally prepeocessed, replacing the variables and expressions, 
/// and then sent to ReadConfig() method. Unlike including, the original section will NOT be expanded.
/// 
/// To pass the loop infomarion into the resident class, one need to call the fourth overload
/// of the LoadConfigFromFile() starter methods. The resident class can get access to its host's variable list in this overload.
///
///
/// ### The globals section
///
/// The *globals* section allows to specify few common definitions used in the REST 
/// framework. This section will maintain the same and will be passed though the whole sequencial 
/// startup procedure. If a section is to be used many times by different classes, it is better 
/// to put it in the global section. If necessary, one can even start up resident class by finding 
/// sections inside this global section.
///
/// \code
/// <globals>
///    <parameter name="mainDataPath" value="{REST_DATAPATH}" />
///    <parameter name="gasDataPath" value="{GAS_PATH}" /> 
///    <parameter name="verboseLevel" value="debug" /> 
///	<parameter name = "inputFile" value = "${REST_INPUTFILE}" / >
///	<parameter name = "inputFormat" value = "run[RunNumber]_file[Fragment]_[Time-d]_[Time].graw" / >
///	<parameter name = "outputFormat" value = "RUN[RunNumber]_[Time-d]_[LastProcess].root" / >
///	<parameter name = "outputPath" value = "./" / >
///	<parameter name = "experiment" value = "PandaX-III" / >
/// 
///	<TRestDetectorSetup>
///		<parameter name = "runNumber" value = "70" / >
///		<parameter name = "runTag" value = "tagTemplate" / >
///		<parameter name = "runDescription" value = "Run description template" / >
///		<parameter name = "meshVoltage" value = "500000" / >
///		<parameter name = "driftField" value = "500" / >
///		<parameter name = "detectorPressure" value = "10" / >
///	</TRestDetectorSetup>
/// 
/// </globals>
/// \endcode
///
/// The global section will have effect on *all the metadata structures* (or sections) that are
/// defined in a same RML file. It does not affect to other possible linked sections defined by 
/// reference using for example nameref.
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
/// GetParameter(), GetElement(), GetElementWithName(). Details are shown in the function's document
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

const int NAME_NOT_FOUND = -2;
const int NOT_FOUND = -1;
const int ERROR = -1;
const int OK = 0;

ClassImp(TRestMetadata)
///////////////////////////////////////////////
/// \brief TRestMetadata default constructor
///
TRestMetadata::TRestMetadata()
{
	fStore = true;
	fElementGlobal = NULL;
	fElement = NULL;
	fVerboseLevel = REST_Silent;
	//helper = new TRestStringHelper();
	fElementEnv.clear();
}

///////////////////////////////////////////////
/// \brief constructor
///
TRestMetadata::TRestMetadata(const char *cfgFileNamecfgFileName)
{
	fStore = true;
	fElementGlobal = NULL;
	fElement = NULL;
	fVerboseLevel = REST_Silent;
	//helper = new TRestStringHelper();
	fElementEnv.clear();
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
Int_t TRestMetadata::LoadConfigFromFile(string cfgFileName)
{
	if (ChecktheFile(cfgFileName) == OK) {
		TiXmlElement* Sectional = GetElement(GetSectionName(), cfgFileName);
		TiXmlElement* Global = GetElement("globals", cfgFileName);
		vector<TiXmlElement*> a;
		a.clear();
		return LoadConfigFromFile(Sectional, Global, a);
	}
	else
	{
		GetChar();
		exit(1);
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
///First combine the sectional and global sections together, then save the input env section.
/// To make start up it calls the following methods in sequence: LoadSectionMetadata(), BeginOfInit(), LoopElements(), EndOfInit()
/// 
Int_t TRestMetadata::LoadConfigFromFile(TiXmlElement* eSectional, TiXmlElement* eGlobal, vector<TiXmlElement*> eEnv)
{
	Initialize();
	TiXmlElement* parentelement;
	if (eSectional != NULL)
	{
		//Sectional and global elements are first combined.
		parentelement = (TiXmlElement*)eSectional->Clone();
		TiXmlElement* echild = eGlobal->FirstChildElement();
		while (echild != NULL) {
			parentelement->LinkEndChild(echild->Clone());
			echild = echild->NextSiblingElement();
		}
		for (int i = 0; i < eEnv.size(); i++)
		{
			parentelement->LinkEndChild(eEnv[i]->Clone());
		}
	}
	else if (eGlobal != NULL)
	{
		parentelement = eGlobal;
	}
	else
	{
		return 0;
	}
	fElement = parentelement;
	fElementGlobal = eGlobal;
	fElementEnv = eEnv;

	int result = LoadSectionMetadata();
	if (result == 0)
		InitFromConfigFile();
	return result;
}

///////////////////////////////////////////////
/// \brief Do some preparation.
///
/// Preparation includes: seting the name, title and verbose level of the current class. 
/// Finding out and saving the env sections. Self-replace the env and expressions to make them ready.
Int_t TRestMetadata::LoadSectionMetadata()
{
	//general metadata: name, title, verboselevel
	this->SetName(GetParameter("name", "defaultName").c_str());
	this->SetTitle(GetParameter("title", "defaultTitle").c_str());
	this->SetSectionName(this->ClassName());
	string debugStr = GetParameter("verboseLevel", "info");
	if (debugStr == "silent")
		fVerboseLevel = REST_Silent;
	if (debugStr == "info")
		fVerboseLevel = REST_Info;
	if (debugStr == "essential")
		fVerboseLevel = REST_Essential;
	if (debugStr == "debug")
		fVerboseLevel = REST_Debug;
	if (debugStr == "extreme")
		fVerboseLevel = REST_Extreme;

	debug << "Loading Config for : " << this->ClassName() << endl;


	//first set env from global section
	if (fElementGlobal != NULL) {
		TiXmlElement* e = fElementGlobal->FirstChildElement();
		while (e != NULL)
		{
			if ((string)e->Value() == "variable" || (string)e->Value() == "myParameter")
			{
				ReplaceElementAttributes(e);
				SetEnvWithElement(e);
			}
			e = e->NextSiblingElement();
		}
	}

	//then from local section
	TiXmlElement* e = fElement->FirstChildElement();
	while (e != NULL)
	{
		if ((string)e->Value() == "variable" || (string)e->Value() == "myParameter")
		{
			ReplaceElementAttributes(e);
			SetEnvWithElement(e);
		}
		e = e->NextSiblingElement();
	}


	//finally do this replacement for all child elements and expand for/include definitions
	ExpandElement(fElement);
	debug << ClassName() << " has finished preparing config data" << endl;

	return 0;
}


void TRestMetadata::InitFromConfigFile()
{

	BeginOfInit();
	if (fElement != NULL)
	{
		TiXmlElement*e = fElement->FirstChildElement();
		while (e != NULL)
		{
			//cout << 111111111111 << endl;
			string value = e->Value();
			string name = "";
			const char* a = e->Attribute("name");
			if (a != NULL) name = a;
			if (value == "variable" || value == "myParameter") { e = e->NextSiblingElement(); continue; }

			if (ReadConfig((string)e->Value(), e) == 0) {
				debug << "rml Element \"" << e->Value() << "\" with name \"" << name << "\" has been loaded by: " << GetSectionName() << endl;
			}
			e = e->NextSiblingElement();

		}
	}
	EndOfInit();

}




///////////////////////////////////////////////
///\brief replace the field value(attribute) of the given xml element
///
///it will replace:
///1.variables formatted like ${ABC} or {ABC} or ABC.
///(system env, rml env, sectional para, respectivally)
///3.math expressions
TiXmlElement * TRestMetadata::ReplaceElementAttributes(TiXmlElement * e)
{
	if (e == NULL)return NULL;

	TiXmlAttribute* attr = e->FirstAttribute();
	while (attr != NULL)
	{
		const char* val = attr->Value();
		const char* name = attr->Name();

		string temp = ReplaceEnvironmentalVariables(val);
		e->SetAttribute(name, ReplaceMathematicalExpressions(temp).c_str());

		attr = attr->Next();
	}

	return e;
}




///////////////////////////////////////////////
/// \brief Finds an environment variable definition inside the xml section and sets it.
///
/// The xml declaration "variable" and myParameter are both vaild. If the environment 
/// variable exists already, its value can be overriden here. Using *overwrite="true"*.
/// 
/// Example of environmental variable definition : \code <variable name="TEST" value="VALUE" overwrite="true" /> \endcode
///
void TRestMetadata::SetEnvWithElement(TiXmlElement* e, bool overwriteexisting)
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
			if(overwriteexisting)
				fElementEnv[i]->SetAttribute("value", value);
			return;
		}
	}

	//if not find, add it directly.
	fElementEnv.push_back((TiXmlElement*)e->Clone());
}

///////////////////////////////////////////////
/// \brief Expand for loop and include definition for all the child elements.
///
void TRestMetadata::ExpandElement(TiXmlElement*e) 
{
	ReplaceElementAttributes(e);
	TiXmlElement* contentelement = e->FirstChildElement();
	while (contentelement != NULL) 
	{
		TiXmlElement*nxt= contentelement->NextSiblingElement();
		//cout << contentelement->Value() << endl;
		if ((string)contentelement->Value() == "for") 
		{
			//debug << "expanding for loop" << endl;
			ExpandForLoops(contentelement);
		}
		else if(contentelement->Attribute("file")!=NULL)
		{
			ExpandIncludeFile(contentelement);
		}
		else if (contentelement->FirstChildElement() != NULL)
		{
			debug << "into child element of "<< contentelement->Value() << endl;
			ExpandElement(contentelement);
		}
		else
		{
			ReplaceElementAttributes(contentelement);
		}
		contentelement = nxt;
	}
}


///////////////////////////////////////////////
/// \brief Expands the loop structures found in **buffer** by substituting the running indexes by their values.
///
void TRestMetadata::ExpandForLoops(TiXmlElement*e)
{
	if ((string)e->Value() != "for")return;

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
					ExpandElement(attatchedalament);
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
/// After finding the section, it will insert its child into the given given
/// xml element. 
void TRestMetadata::ExpandIncludeFile(TiXmlElement * e)
{
	const char* filename = e->Attribute("file");
	if (filename == NULL)return;
	if (ChecktheFile(filename) == -1) { 
		warning << "Include file "<<filename<<" does not exist!" << endl; 
		return;
	}
	if (!isRootFile(filename)) //root file inclusion should be implemented in the derived class
	{
		//get the root element
		TiXmlElement* rootele = GetRootElementFromFile(filename);
		if (rootele == NULL) {
			warning << "Include file " << filename << " is of wrong xml format!" << endl;
			return;
		}

		//we find the target element(the element to receive content) 
		//and the config element(the element to provide content)
		TiXmlElement*targetele;
		string type;
		string name;
		//condition 1: 
		//   <a name="" .....>
		//     <include file="aaa.rml"/>
		//     ....
		//   </a>
		//element "a" is the target element
		if ((string)e->Value() == "include")
		{
			targetele = (TiXmlElement*)e->Parent();
			if (targetele == NULL)return;

			type = targetele->Value();
			name = e->Attribute("name") == NULL ? "" : e->Attribute("name");

		}
		//condition 2: 
		//   <a name="" ... file="aaa.rml" .../>
		//element "a" is the target element
		else
		{
			targetele = e;
			if (e->Attribute("type") != NULL) {
				type = e->Attribute("type");
			}
			else
			{
				type = e->Value();
			}
			name = e->Attribute("name") == NULL ? "" : e->Attribute("name");
		}

		debug << "----expanding include file----" << endl;

		//find config elemnt according to type and name
		TiXmlElement*configele = NULL;
		//1. root element in the included file is of given type
		if ((string)rootele->Value() == type) {
			configele = rootele;
		}
		//2. name is not specified
		else if (name == ""&&GetElement(type, rootele) != NULL)
		{
			if (type != "globals"&&GetElement("globals", rootele) != NULL) {
				TiXmlElement*globaldef = GetElement("globals", rootele)->FirstChildElement();
				while (globaldef != NULL)
				{
					if ((string)globaldef->Value() == "variable" || (string)globaldef->Value() == "myParameter")
					{
						SetEnvWithElement(globaldef, false);
					}
					globaldef = globaldef->NextSiblingElement();
				}
			}

			configele = GetElement(type, rootele);
		}
		//3. child element in the root element is of given type/name, we need to import env
		else if (GetElementWithName(type, name, rootele) != NULL)
		{
			if (type != "globals"&&GetElement("globals", rootele) != NULL) {
				TiXmlElement*global = GetElement("globals", rootele);
				TiXmlElement*globaldef = global->FirstChildElement();
				while (globaldef != NULL)
				{
					if ((string)globaldef->Value() == "variable" || (string)globaldef->Value() == "myParameter")
					{
						ReplaceElementAttributes(globaldef);
						debug << "setting env" << globaldef->Value() << globaldef->Attribute("name") << endl;
						SetEnvWithElement(globaldef, false);
					}
					globaldef = globaldef->NextSiblingElement();
				}
			}

			configele = GetElementWithName(type, name, rootele);

		}

		if(configele==NULL)
		{
			warning << "Cannot get corresponding xml section(class name: " << type << " , name: " << name << " ). Skipping" << endl;
			return;
		}



		ExpandElement(configele);

		//expand the included file content into the parent element
		//this is called when the xml is like
		//   <a name="" .....>
		//     <include file="aaa.rml"/>
		//     ....
		//   </a>
		if ((string)e->Value() == "include")
		{
			if (targetele->Attribute("expanded") == NULL ? true : ((string)targetele->Attribute("expanded") != "true")) {
				TiXmlElement* ele = configele->FirstChildElement();
				while (ele != NULL)
				{
					//ExpandElement(ele);
					if ((string)ele->Value() != "for")
						targetele->InsertBeforeChild(e, *ele->Clone());
					ele = ele->NextSiblingElement();
				}

				targetele->RemoveChild(e);
			}
		}
		else//expand the included file content into itself
			//this is called when the element is like
			//<a name="" ... file="aaa.rml" .../>
		{
			if (targetele->Attribute("expanded") == NULL ? true : ((string)targetele->Attribute("expanded") != "true")) {
				TiXmlAttribute*attr = configele->FirstAttribute();
				while (attr != NULL) {
					if(targetele->Attribute(attr->Name())==NULL)
						targetele->SetAttribute(attr->Name(), attr->Value());
					attr = attr->Next();
				}
				TiXmlElement* ele = configele->FirstChildElement();
				while (ele != NULL)
				{
					//ExpandElement(ele);
					if ((string)ele->Value() != "for")
					{
						targetele->InsertEndChild(*ele->Clone());
					}
					ele = ele->NextSiblingElement();
				}
			}
		}

		targetele->SetAttribute("expanded", "true");
		if (fVerboseLevel >= REST_Debug) {
			targetele->Print(stdout, 0);
			cout << endl;
		}

		debug << "----end of expansion file----" << endl;
	}
}



///////////////////////////////////////////////
/// \brief Returns the value for the parameter name **parName** found in fElement(main data element)
/// 
/// It first finds the parameter from the system's env, if the name matches it will directly return the env.
/// If no env is available, it calls GetParameter() method. See more detail there
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
/// and the value are given in its fields. This is classic because the element itself declares
/// it is a prarmeter. We also generalize the concept of parameter to the elements' fields.
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
	if (e == NULL) {
		if (GetVerboseLevel() > REST_Debug) { cout << "Element is null" << endl; }
		return "";
	}
	if (e->Attribute(parName.c_str()) != NULL) {
		return	ReplaceMathematicalExpressions(ReplaceEnvironmentalVariables(e->Attribute(parName.c_str())));
	}
	else
	{
		return "";
	}
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
/// The root element is the parent of
/// any other xml elements in the file. There is only one root element in each xml encoded file.
///
/// Exits the whole program if the xml file does not exist or is in wrong in syntax.
///
TiXmlElement* TRestMetadata::GetRootElementFromFile(std::string cfgFileName)
{
	TiXmlDocument* doc = new TiXmlDocument();

	if (!doc->LoadFile(cfgFileName.c_str()))
	{
		cout << "Failed to load xml file, syntax maybe wrong. The file is: " << cfgFileName << endl;

		GetChar();
		exit(1);
	}

	TiXmlElement* root = doc->RootElement();
	if (root != NULL) {
		return root;
	}
	else
	{
		cout << "Succeeded to load xml file, but no element contained" << endl;
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
	TiXmlElement* ele = e->FirstChildElement(eleDeclare.c_str());
	while (ele != NULL)
	{
		if ((string)ele->Value() == eleDeclare)
		{
			if (ele->Attribute("name") != NULL && (string)ele->Attribute("name") == eleName)
			{
				return ele;
			}
		}
		ele = ele->NextSiblingElement();
	}
	return ele;

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
			warning << "Found unit definition \"" << unitstring << "\" in element " << e->Value() << endl;
			warning << "This way of definition of units is not recommended, use <... value=\"3mm\" .../> instead" << endl << endl;
			return unitstring;
		}
		else
		{
			warning << "No units are defined in element "<<e->Value()<<" , returning default unit" << endl;
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
			warning << "REST WARNING : Parameter \"" << whoseunits << " = " << a << "\" dose not contain any units" << endl;
			warning << "Trying to find unit in element..." << endl;
			return GetUnits(e, "");
		}
	}

	return unitstring;
}










TiXmlElement* TRestMetadata::StringToElement(string definition)
{
	TiXmlElement*ele = new TiXmlElement("temp");
	//TiXmlDocument*doc = new TiXmlDocument();
	ele->Parse(definition.c_str(), NULL, TIXML_ENCODING_UTF8);
	return ele;
}


string TRestMetadata::ElementToString(TiXmlElement*ele)
{
	if (ele != NULL) {
		stringstream ss;
		ss << (*ele);
		string s = ss.str();

		//remove comments
		int pos = 0;
		int pos2 = 0;
		while ((pos = s.find("<!--", pos)) != -1 && (pos2 = s.find("-->", pos)) != -1)
		{
			s.replace(pos, pos2 - pos + 3, "");//3 is the length of "-->"
			pos = pos + 1;
		}

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
	string buffer = ElementToString(fElement);
	size_t Position = 0;
	string result = GetKEYStructure(keyName, Position, buffer);
	if (result == "") result = "NotFound";
	return result;
}
string TRestMetadata::GetKEYStructure(std::string keyName, size_t &Position)
{
	string buffer = ElementToString(fElement);
	string result = GetKEYStructure(keyName, Position, buffer);
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
	size_t position = fromPosition;

	debug << "Finding KEY Structure " << keyName << endl;
	debug << "Buffer : " << buffer << endl;
	debug << "Start position : " << position << endl;

	string startKEY = "<" + keyName;
	string endKEY = "/" + keyName;

	debug << "Reduced buffer : " << buffer.substr(position) << endl;

	size_t initPos = buffer.find(startKEY, position);
	debug << "initPos : " << initPos << endl;

	if (initPos == string::npos) { debug << "KEY not found!!" << endl; return ""; }

	size_t endPos = buffer.find(endKEY, initPos);

	debug << "End position : " << endPos << endl;

	//TODO Check if a new section starts. If not it might get two complex strings if the KEY_Structure was not closed using /KEY

	if (endPos == string::npos) { debug << "END KEY not found!!" << endl; return ""; }

	debug << endl;

	fromPosition = endPos;

	return buffer.substr(initPos, endPos - initPos + endKEY.length() + 1);
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
	return GetDblParameterWithUnits(fieldName,ele);
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
	return Get2DVectorParameterWithUnits(fieldName, ele);
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
	return Get3DVectorParameterWithUnits(fieldName, ele);
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
/// \brief Evaluates a complex numerical expression and returns the resulting value using TFormula.
///
string TRestMetadata::EvaluateExpression(string exp)
{
	if (!isAExpression(exp)) { return exp; }

	//NOTE!!! In root6 the expression like "1/2" will be computed using the input as int number,
	//which will return 0, and cause problem.
	//we roll back to TFormula of version 5
	ROOT::v5::TFormula formula("tmp", exp.c_str());

	ostringstream sss;
	Double_t number = formula.EvalPar(0);
	if (number > 0 && number < 1.e-300)
	{
		warning << "REST Warning! Expression not recognized --> " << exp << endl;  return exp;
	}

	sss << number;
	string out = sss.str();

	return out;
}


///////////////////////////////////////////////
/// \brief Identifies enviromental variable replacing marks in the input buffer, and replace them with corresponding value.
///
/// Replacing marks: 
/// 1. ${VARIABLE_NAME} : search the system env and replace it if found.
/// 2. {VARIABLE_NAME}  : search the program env and replace it if found.
/// 3. [VARIABLE_NAME]  : search the program provided env and replace it if found.
/// 4. VARIABLE_NAME    : try match the names of myParameter and replace it if matched.
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
			debug << "cannot find \"" << expression << "\", returning raw expression " << outputBuffer.substr(startPosition, endPosition - startPosition + 1) << endl;
		}
	}


	//replace env
	startPosition = 0;
	endPosition = 0;
	while ((startPosition = outputBuffer.find("{", endPosition)) != (int)string::npos)
	{
		endPosition = outputBuffer.find("}", startPosition + 1);
		if (endPosition == (int)string::npos) break;

		string expression = outputBuffer.substr(startPosition + 1, endPosition - startPosition - 1);

		int replacePos=startPosition;
		int replaceLen=endPosition-startPosition+1;
		if (startPosition!=0&&outputBuffer[startPosition - 1] == '$') {
			replacePos = startPosition - 1;
			replaceLen = endPosition - startPosition + 2;
		}

		//search for both system env and program env
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
			debug << "replace env " << startPosition << ": cannot find \"" << expression << "\", returning raw expression " << outputBuffer.substr(replacePos, replaceLen) << endl;
		}
	}
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

		//search for both system env and program env
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
			debug << "replace env " << startPosition << ": cannot find \"" << expression << "\", returning raw expression " << outputBuffer.substr(replacePos, replaceLen) << endl;
		}
	}


	//replace myParameter
	startPosition = 0;
	endPosition = 0;
	for (int i = 0; i < fElementEnv.size(); i++) {
		if ((string)fElementEnv[i]->Value() == "myParameter")
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
/// \brief Evaluates and replaces valid mathematical expressions found in the input string **buffer**.
///
string TRestMetadata::ReplaceMathematicalExpressions(const string buffer)
{
	//we spilt the unit part and the expresstion part
	int pos = buffer.find_last_of("1234567890().");

	string unit = buffer.substr(pos + 1, -1);
	string temp = buffer.substr(0, pos + 1);
	string result = "";

	bool erased = false;
	if (temp[0] == '(' && temp[temp.length() - 1] == ')')
	{
		temp.erase(temp.size() - 1, 1);
		temp.erase(0, 1);
		erased = true;
	}

	std::vector<std::string> Expressions;
	temp += ",";
	for (int i = 0; i < temp.size(); i++) {
		int pos = temp.find(",", i);
		if (pos < temp.size())
		{
			std::string s = temp.substr(i, pos - i);
			Expressions.push_back(s);
			i = pos;
		}
	}

	for (int i = 0; i < Expressions.size(); i++)
	{
		if (!isAExpression(Expressions[i])) { result += Expressions[i] + ","; continue; }
		result += EvaluateExpression(Expressions[i]) + ",";
	}

	result.erase(result.size() - 1, 1);

	if (erased)
	{
		result = "(" + result + ")";
	}

	return result+unit;

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


void TRestMetadata::PrintConfigBuffer()
{
	fElement->Print(stdout, 0);
	cout << endl;
}


///////////////////////////////////////////////
/// \brief Prints metadata content on screen. Usually overloaded by the derived metadata class.
///
void TRestMetadata::PrintMetadata()
{
	cout << "TRestMetadata content" << endl;
	cout << "-----------------------" << endl;
	cout << "Section name : " << fSectionName << endl;        // section name given in the constructor of TRestSpecificMetadata
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



TClass*c;
TVirtualStreamerInfo *vs;
TObjArray* ses;
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

TStreamerElement* TRestMetadata::GetDataMemberWithID(int ID)
{
	int n = GetNumberOfDataMember();
	if (ID < n)return (TStreamerElement*)ses->At(ID);
	return NULL;
}

int TRestMetadata::GetNumberOfDataMember()
{
	if (ses == NULL) {
		c = this->IsA();
		vs = c->GetStreamerInfo();
		ses = vs->GetElements();
	}
	return ses->GetLast() + 1;
}


double TRestMetadata::GetDblDataMemberVal(TStreamerElement*ele)
{
	if (ele != NULL&&ele->GetType() == 8)
		return *(double*)((char*)this + ele->GetOffset());
	return 0;
}


int TRestMetadata::GetIntDataMemberVal(TStreamerElement*ele)
{
	if (ele != NULL&&ele->GetType() == 3)
		return *(int*)((char*)this + ele->GetOffset());
	return 0;
}


char* TRestMetadata::GetDataMemberRef(TStreamerElement*ele) {
	return ((char*)this + ele->GetOffset());
}


void TRestMetadata::SetDataMemberVal(TStreamerElement*ele, char*ptr) {

	if (ele != NULL&&ele->GetType() == 8)//double
		*((double*)((char*)this + ele->GetOffset())) = *((double*)ptr);
	if (ele != NULL&&ele->GetType() == 3)//int
		*((int*)((char*)this + ele->GetOffset())) = *((int*)ptr);
	if (ele != NULL&&ele->GetType() == 365)//string
		*((string*)((char*)this + ele->GetOffset())) = *((string*)ptr);
	if (ele != NULL&&ele->GetType() == 65)//TString
		*((TString*)((char*)this + ele->GetOffset())) = *((TString*)ptr);
	if (ele != NULL&&ele->GetType() == 8)//other
		return;
}

void TRestMetadata::SetDataMemberVal(TStreamerElement*ele, string valdef)
{
	if (ele != NULL&&ele->GetType() == 8)//double
	{
		*((double*)((char*)this + ele->GetOffset())) = StringToDouble(valdef);
	}
	if (ele != NULL&&ele->GetType() == 3)//int
	{
		*((int*)((char*)this + ele->GetOffset())) = StringToInteger(valdef);

	}
	if (ele != NULL&&ele->GetType() == 365)//string
	{
		*((string*)((char*)this + ele->GetOffset())) = valdef;

	}
	if (ele != NULL&&ele->GetType() == 65)//TString
	{
		*((TString*)((char*)this + ele->GetOffset())) = (TString)(valdef);
	}
}


void TRestMetadata::SetDataMemberValFromConfig(TStreamerElement*ele)
{
	if (GetParameter(ele->GetName()) != PARAMETER_NOT_FOUND_STR)
	{
		SetDataMemberVal(ele, GetParameter(ele->GetName()));
	}
}

string TRestMetadata::GetDataMemberValString(TStreamerElement*ele)
{
	if (ele != NULL&&ele->GetType() == 8)//double
		return ToString(*(double*)((char*)this + ele->GetOffset()));
	if (ele != NULL&&ele->GetType() == 3)//int
		return ToString(*(int*)((char*)this + ele->GetOffset()));
	if (ele != NULL&&ele->GetType() == 365)//string
		return ToString(*(string*)((char*)this + ele->GetOffset()));
	if (ele != NULL&&ele->GetType() == 65)//TString
		return ToString(*(TString*)((char*)this + ele->GetOffset()));
	if (ele != NULL&&ele->GetType() == 8)//other
		return ele->GetTypeName();
}













