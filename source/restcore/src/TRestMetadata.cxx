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
/// A class deriving from TRestMetadata can retrieve information from a plain
/// text configuration file (or RML file). The syntaxis in an RML file is
/// imposed by TRestMetadata. The rml file is encoded in standard xml format and
/// has a structure similar to the real class structure. The following piece of
/// code shows the common structure of the metadata description corresponding to
/// a specific metadata class.
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
/// "field1="value1"" or "name="userGivenName"" are called **fields** or **xml
/// attributes**. All the sealed xml structure is called **xml element** or
/// **xml section**.
///
/// Note that the decalration
/// "include", "for", "variable" and "constant" is reserved for the software.
/// They works differently than others, which we will talk later.
///
/// ### Sequencial start up procedure of a metadata class
///
/// The rml file is designed to start up/instruct all the metadata classes.
/// Usually we implement the method Initialize() and calls it in the constructor
/// to do some default settings.
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
/// The starter method LoadConfigFromFile() is implemented in this class, with
/// four overloads, as shown in the following.
///
/// \code
///
/// void LoadConfigFromFile();
/// void LoadConfigFromFile(const char *cfgFileName, string sectionName = "");
/// void LoadConfigFromFile(TiXmlElement* eSectional, TiXmlElement* eGlobal);
/// void LoadConfigFromFile(TiXmlElement* eSectional, TiXmlElement* eGlobal,
/// map<string, string> envs);
///
/// \endcode
///
/// If no arguments are provided, LoadConfigFromFile() will only call the Initialize()
/// method. If given the rml file name, it will find out the needed rml sections. We
/// can also directly give the xml sections to the method. Two xml sections are used
/// to startup the class: the section for the class and a global section. Additionaly
/// we can give a map object to the method to import additional variables.
///
/// The "section for the class" is an xml section with the value of class name.
/// It is the main information souce for the class's startup. The "global"
/// section is a special xml section in the rml file, containing global information
/// which could be seen by all the class sections.
///
/// With the xml sections given, LoadConfigFromFile() first merge them together. Then it
/// calls LoadSectionMetadata(), which loads some universal parameters like
/// name, title and verbose level. This method also preprocesses the config
/// sections, expanding the include/for decinition and replacing the variables.
/// After this, LoadConfigFromFile() calls the method InitFromConfigFile().
///
/// **InitFromConfigFile()** is a pure virtual method and every child classes have
/// to implement it. This method defines how the metadata class loads its xml
/// config section. A simple kind of implementation is to add few lines of GetParameter():
///
/// \code
///
/// <TRestMuonAnalysisProcess name = "muAna" title = "Example" verboseLevel =
/// "info" > 	<parameter name="XROI" value="(100,300)"/> 	<parameter
/// name="YROI" value="(0,-200)"/>
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
/// A more advanced usage is sequential startup, when the metadata class contains
/// another metadata class. We can write in the host class's InitFromConfigFile()
/// to new the resident class, and the call the resident class's LoadConfigFromFile()
/// method, giving the child section as the resident class's config section. The rml
/// hierarchy could therefore be the same as class residence.
///
/// For example, when received an xml section declared "TRestRun", the host
/// "TRestManager" will pass this section (together with its global section) to
/// its resident "TRestRun". The TRestRun class can therefore perform a startup
/// using these sections.
///
/// \code
///
/// <TRestManager name = "CoBoDataAnalysis" title = "Example" verboseLevel =
/// "info" > 	<TRestRun name = "SJTU_Proto" > 		<addMetadata name
/// = "PandaReadout_MxM" file = "readouts.root" />
/// 	</TRestRun>
///		...
/// </TRestManager>
///
/// void TRestManager::InitFromConfigFile()
/// {
/// 	TiXmlElement*e = fElement->FirstChildElement();
/// 	while (e != NULL)
/// 	{
///			string value = e->Value();
/// 		if (value == "TRestRun") {
/// 		    fRunInfo = new TRestRun();
/// 		    fRunInfo->LoadConfigFromFile(e, fElementGlobal);
///			}
/// 		else if (value == "TRestAnalysisPlot") {
/// 		    fPlot = new TRestAnalysisPlot();
/// 		    fPlot->LoadConfigFromFile(e, fElementGlobal);
///			}
/// 	}
/// }
///
/// \endcode
///
///
/// ### Replacement of variables and expressions
///
/// By default, LoadConfigFromFile() will look into only the first-level child sections
/// of both global and sectional section. if the section's value is either
/// "variable" or "constant", the class will keep them for replacement.
///
/// \code
///
///   <TRestRun name="userGivenName" title="User given title" >
///       <variable name="nChannels" value="${CHANNELS}" /> //this variable
///       will be added to the class "TRestReadout"
///
///       <TRestReadout name="aaaa" >
///           <variable .... / > //this variable cannot be loaded by the class
///           "TRestReadout"
///       </TRestReadout>
///       <parameter name="Ch" value="nChannels+{CHANNELS}-2" />
///   </TRestRun>
///
///   <global>
///       <variable name = "CHANNELS" value = "64" overwrite = "false" / >
///       //this variable can be loaded by both
///   </global>
///
/// \endcode
///
/// LoadConfigFromFile() will replace the field values of xml sections before
/// they are used. The procedure of replacing is as following:
///
/// 1. recognize the strings surrounded by "${}". Seek and replace in system
/// env first. If not found, replace with variable/constant.
/// 2. directly replace the strings matching the name of variable/constant
/// by their value.
///
/// After replacement, LoadConfigFromFile() will call TFormula to evaluate the
/// string if it is identified as an math expression.
///
/// The result string of the field value is either a number string or a string
/// with concrete content. In the exapmle code above, the section declared with
/// "parameter" will have its field value reset to string 126.
///
/// ### Including external RML files in a main RML file
///
/// It is possible to link to other files in any section.
/// There are two include modes:
/// 1. raw include. LoadConfigFromFile() will parse all the lines in the file as xml
/// element and insert them inside the local section. \code <TRestXXX>
///		<include file="abc.txt"/>
/// </TRestXXX>
/// \endcode
/// 2. auto insert. LoadConfigFromFile() will automatically find corresponding section in
/// the file. If the file can be parsed by tinyxml, it will first import its
/// globals section. When searching the section, LoadConfigFromFile() searches according
/// to the name and type. Here "type" can either be the element declare or
/// attribute "type". After finding the section, its child sections as well as
/// attributes will be inserted into the local element. \code <TRestXXX
/// name="sAna" file="abc.rml"/> \endcode Or \code <doXXX type="TRestXXX"
/// name="sAna" file="abc.rml"/> \endcode
///
/// After the expansion is done, variable replacement is also performed.
///
/// If the target file is a root file, there will be a different way to load,
/// see TRestRun::ImportMetadata()
///
/// ### For loop definition
///
/// The definition of FOR loops is implemented in RML in order to allow extense
/// definitions, where many elements may need to be added to an existing array
/// in our metadata structure. The use of FOR loops allows to introduce more
/// versatil and extense definitions. Its implementation was fundamentally
/// triggered by its use in the construction of complex, multi-channel generic
/// readouts by TRestReadout.
///
/// The for loop definition can be either `from-to-step` structure or `in` structure.
/// The loop variable is defined in `variable` attribute, which is treated same as
/// rml variable. In `from-to-step` structure, the value must be numbers. REST will
/// loop form `from` to `to` with step size `step`. In `in` structure, the values
/// are treated as string, and must be separated with `:`. For example:
///
/// \code
/// <for variable="nCh" from="0" to="63" step="1" >
///	  <readoutChannel id="${nCh}" >
///		<for variable="nPix" from="0" to="63" step="1">
///       <addPixel id="${nPix}" origin="((1+${nCh})*pitch,pitch/4+${nPix}*pitch)"
///         size="(pixelSize,pixelSize)" rotation="45" />
///		</for>
///    </readoutChannel>
/// </for>
///
/// <for variable="nMod" in="0:2:3:4:6:8:9" >
///   <TRestAnalysisPlot name="ModuleFirstXYHitMap${nMod}"  previewPlot="false">
///     <canvas size="(800,600)"  save="M${nMod}_Hitmap.png"/>
///     <plot name="aaa" title="First X/Y Hitmap of Module ${nMod}" xlabel="X channel"
///       ylabel="Y channel" value="ON" option="colz">
///       <variable name="rA_ModuleFirstX.second" range="(0,64)" nbins="64" />
///       <variable name="rA_ModuleFirstY.second" range="(64,128)" nbins="64" />
///       <cutString string="rA_ModuleFirstY.first==${nMod}"/>
///     </plot>
///   </TRestAnalysisPlot>
/// </for>
/// \endcode
///
/// The first for loop definition will be expanded to 64 <readoutChannel sections
/// with 64 <addPixel sections in each. The nCh and nPix variables will be 0~64
/// in each section. The second for loop definition will be expanded to 7 <TRestAnalysisPlot
/// sections with nMod be valued 0,2,3,4,6,8,9 respectivelly.
///
/// ### If definition
///
/// It is supported in rml to use <if structure. If the condition matches, the inner
/// sections of <if section will be expaned to and seen by the parent section.
/// This helps to identify different use cases of the rml. For example:
///
/// \code
/// <TRestXXX>
///		<if condition="${HOME}==/home/nkx">
///        <addProcess type="TRestSignalZeroSuppresionProcess" name="zS" value="ON" file="processes.rml"/>
///     </if>
///     <if evaluate="date +%Y-%m-%d" condition=">2019-08-21">
///        <addProcess type = "TRestSignalToHitsProcess" name = "signalToHits" value = "ON" file =
///        "processes.rml" />
///     </if>
/// </TRestXXX>
/// \endcode
///
/// Here "condition" specifies the comparing condition, it can either be a complete expression or just
/// the right side of it. In incomplete case, the left side shall be expressed by "evaluate",
/// which is a shell command to be executed.
///
/// In the example, if the home directory is "/home/nkx", the process "TRestSignalZeroSuppresionProcess"
/// will be added. If the date is larger than 2019-08-21, the process "TRestSignalToHitsProcess"
/// will be added.
///
/// Condition markers supports: `==`, `!=`, `>`, `<`, `<=`, `>=`. Its better to escape the ">", "<" markers.
/// Note that those markers are also valid for strings. If the two sides of the expression
/// are not numbers, REST will perform string comparasion. The ordering is according to the alphabet.
/// A common misleading fact is that "1235">"12345", rather than 1235<12345 as number.
///
/// ### The globals section
///
/// The *globals* section allows to specify few common definitions used in the
/// REST framework. This section will maintain the same and will be passed
/// though the whole sequencial startup procedure. If a section is to be used
/// many times by different classes, it is better to put it in the global
/// section.
///
/// \code
///	<globals>
///		<searchPath value = "$ENV{REST_INPUTDATA}/definitions/" />
///		<parameter name = "outputLevel" value = "internalvar" />
///		<parameter name = "verboseLevel" value = "essential" />
///		<parameter name = "inputFile" value = "${REST_INPUTFILE}" />
///		<parameter name = "inputFormat" value =
///"run[RunNumber]_file[Fragment]_[Time-d]_[Time].graw" /> 		<parameter
/// name = "outputFile" value = "RUN[RunNumber]_[Time]_[LastProcess].root" />
///< parameter name = "mainDataPath" value = "" />
///	</globals>
/// \endcode
///
/// The global section will have effect on *all the metadata structures* (or
/// sections) that are defined in a same RML file. It does not affect to other
/// possible linked sections defined by reference using for example nameref.
///
/// ### Universal file search path
///
/// Some times we don't want to write a long full path to specify files,
/// especially when multiple files are in a same remote directory. REST provides
/// a universal file path definition in rml.
///
/// \code
///	<globals>
///		<searchPath value = "$ENV{REST_INPUTDATA}/definitions/" />
///		<searchPath value = "$ENV{REST_INPUTDATA}/gasFiles/" />
///	</globals>
/// \endcode
///
/// When calling TRestMetadata::SearchFile(), REST will search the file in all
/// the paths defined in section "searchPath", and return a full name if found.
/// Include definition has already adopted this search strategy. Child classes
/// can also take advantage of it.
///
/// ### Default fields for a section
///
/// Three fields in the first line of an xml section will be looked for before
/// anything else. They are: name, title, and verboseLevel. If not specified,
/// LoadConfigFromFile() will find in the *globals* section. If still not fond, it will
/// use the default value.
///
/// Field "name" and "title" is needed by TNamed classes. The "verboseLevel" is
/// used for changing the amount of output infomation. The following line would
/// print on screen any debug message implemented in TRestSignalAnalysisProcess.
///
/// \code
/// <TRestSignalAnalysisProcess name="sgnlAna" title="Data analysis"
/// verboseLevel="debug" >
///     ...
/// </TRestSignalAnalysisProcess>
/// \endcode
///
///
/// ### Using physical units in fields definitions
///
/// Some physical parameters are need to specify the unit so that the provided
/// value makes sense. For example, when defining the electric field we must
/// provide its units.
///
/// \code
/// <parameter name="electricField" value="1000" units="V/cm" />
/// \endcode
///
/// The physical field values wrote in this way will be converted to the
/// standard unit system used by REST. REST_Units namespace provides details on
/// the different existing units, unit conversion and unit definition.
///
/// When we retrieve any value from a REST member in a TRestMetadata class the
/// value will be returned in the default REST units (mm, keV, V/cm, us). We may
/// convert this member to the desired units (as defined in REST_Units), by
/// doing:
///
/// \code
/// Double_t valueInMeV = value * REST_Units::MeV;
/// \endcode
///
///
/// ### Other usefule public tools
///
/// GetParameter(), GetElement(), GetElementWithName(), SearchFile(). Details
/// are shown in the function's document
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
/// \author     Igor Irastorza
/// \author     Javier Galan
/// \author     Kaixiang Ni
///
/// <hr>
///

#include "TRestMetadata.h"

#include <TMath.h>
#include <TSystem.h>

#include <iomanip>

#include "RmlUpdateTool.h"
#include "TRestDataBase.h"
#include "TStreamerInfo.h"
#include "v5/TFormula.h"

// implementation of version methods in namespace rest_version
/*
namespace REST_VersionGlob {
        TString GetRESTVersion() { return REST_RELEASE; }
        int GetRESTVersionCode() { return ConvertVersionCode(REST_RELEASE); }
}
*/
using namespace std;
using namespace REST_Units;
using namespace REST_Physics;

// We introduce the gases file here.
// But, should we have a corner somewhere to define hard-coded globals?
// nkx, 20200228: We now define the global data servers in database file
// const char* gasesFile = "https://sultan.unizar.es/gasFiles/gases.rml";

map<string, string> TRestMetadata_UpdatedConfigFile;

ClassImp(TRestMetadata);

///////////////////////////////////////////////
/// \brief TRestMetadata default constructor
///
TRestMetadata::TRestMetadata() : endl(fVerboseLevel, messageBuffer) {
    fStore = true;
    fElementGlobal = NULL;
    fElement = NULL;
    fVerboseLevel = gVerbose;
    fVariables.clear();
    fConstants.clear();
    fHostmgr = NULL;

    fConfigFileName = "null";
    configBuffer = "";
    metadata.setlength(100);
}

///////////////////////////////////////////////
/// \brief constructor
///
TRestMetadata::TRestMetadata(const char* cfgFileName) : endl(fVerboseLevel, messageBuffer) {
    fStore = true;
    fElementGlobal = NULL;
    fElement = NULL;
    fVerboseLevel = gVerbose;
    fVariables.clear();
    fConstants.clear();
    fHostmgr = NULL;

    fConfigFileName = cfgFileName;
    configBuffer = "";
    metadata.setlength(100);
}

///////////////////////////////////////////////
/// \brief TRestMetadata default destructor
///
TRestMetadata::~TRestMetadata() {
    if (fElementGlobal) delete fElementGlobal;
    if (fElement) delete fElement;
}

///////////////////////////////////////////////
/// \brief Default starter. Just call again the Initialize() method.
///
Int_t TRestMetadata::LoadConfigFromFile() {
    Initialize();
    return 0;
}

///////////////////////////////////////////////
/// \brief Give the file name, find out the corresponding section. Then call the
/// main starter.
///
Int_t TRestMetadata::LoadConfigFromFile(string cfgFileName, string sectionName) {
    fConfigFileName = cfgFileName;
    if (TRestTools::fileExists(fConfigFileName)) {
        if (sectionName == "") {
            sectionName = this->ClassName();
        }

        // find the xml section corresponding to the sectionName
        TiXmlElement* Sectional = GetElementFromFile(fConfigFileName, sectionName);
        if (Sectional == NULL) {
            ferr << "cannot find xml section \"" << ClassName() << "\" with name \"" << sectionName << "\""
                 << endl;
            ferr << "in config file: " << fConfigFileName << endl;
            exit(1);
        }

        // find the "globals" section. Multiple sections are supported.
        TiXmlElement* rootEle = GetElementFromFile(fConfigFileName);
        TiXmlElement* Global = GetElement("globals", rootEle);
        if (Global != NULL) ReadElement(Global);
        if (Global != NULL && Global->NextSiblingElement("globals") != NULL) {
            TiXmlElement* ele = Global->NextSiblingElement("globals");
            if (ele != NULL) ReadElement(ele);
            while (ele != NULL) {
                TiXmlElement* e = ele->FirstChildElement();
                while (e != NULL) {
                    Global->InsertEndChild(*e);
                    e = e->NextSiblingElement();
                }
                ele = ele->NextSiblingElement("globals");
            }
        }

        // call the real loading method
        int result = LoadConfigFromFile(Sectional, Global, {});
        delete Sectional;
        delete rootEle;
        return result;
    } else {
        ferr << "Filename : " << fConfigFileName << endl;
        ferr << "Config File does not exist. Right path/filename?" << endl;
        GetChar();
        return -1;
    }
}

///////////////////////////////////////////////
/// \brief Calling the main starter
///
Int_t TRestMetadata::LoadConfigFromFile(TiXmlElement* eSectional, TiXmlElement* eGlobal) {
    return LoadConfigFromFile(eSectional, eGlobal, {});
}

///////////////////////////////////////////////
/// \brief Main starter method.
///
/// First merge the sectional and global sections together, then save the input
/// env section. To make start up it calls the following methods in sequence:
/// LoadSectionMetadata(), InitFromConfigFile()
///
Int_t TRestMetadata::LoadConfigFromFile(TiXmlElement* eSectional, TiXmlElement* eGlobal,
                                        map<string, string> envs) {
    Initialize();
    TiXmlElement* theElement;
    if (eSectional != NULL && eGlobal != NULL) {
        // Sectional and global elements are first combined.
        theElement = (TiXmlElement*)eSectional->Clone();
        TiXmlElement* echild = eGlobal->FirstChildElement();
        while (echild != NULL) {
            theElement->LinkEndChild(echild->Clone());
            echild = echild->NextSiblingElement();
        }
        // for (int i = 0; i < eEnv.size(); i++) {
        //    theElement->LinkEndChild(eEnv[i]->Clone());
        //}
    } else if (eSectional != NULL) {
        theElement = (TiXmlElement*)eSectional->Clone();
    } else if (eGlobal != NULL) {
        theElement = (TiXmlElement*)eGlobal->Clone();
    } else {
        return 0;
    }
    fElement = theElement;
    fElementGlobal = eGlobal ? (TiXmlElement*)eGlobal->Clone() : NULL;
    fVariables = envs;

    int result = LoadSectionMetadata();
    if (result == 0) InitFromConfigFile();
    debug << ClassName() << " has finished preparing config data" << endl;
    return result;
}

///////////////////////////////////////////////
/// \brief This method does some preparation of xml section.
///
/// Preparation includes: seting the name, title and verbose level of the
/// current class. Finding out and saving the env sections.
///
/// By calling TRestMetadata::ReadElement(), is also expands for loops and
/// include definitions, and replaces env and expressions in rml config section.
///
Int_t TRestMetadata::LoadSectionMetadata() {
    // get debug level
    string debugStr = GetParameter("verboseLevel", ToString(fVerboseLevel));
    fVerboseLevel = StringToVerboseLevel(debugStr);

    debug << "Loading Config for : " << this->ClassName() << endl;

    // set env first from global section
    if (fElementGlobal != NULL) {
        TiXmlElement* e = fElementGlobal->FirstChildElement();
        while (e != NULL) {
            ReplaceElementAttributes(e);
            ReadEnvInElement(e);
            e = e->NextSiblingElement();
        }
    }

    // then from local section
    TiXmlElement* e = fElement->FirstChildElement();
    while (e != NULL) {
        ReplaceElementAttributes(e);
        ReadEnvInElement(e);
        e = e->NextSiblingElement();
    }

    // look through the elements and expand for, include, etc. structures
    ReadElement(fElement);

    // get debug level again in case it is defined in the included file
    debugStr = GetParameter("verboseLevel", ToString(fVerboseLevel));
    fVerboseLevel = StringToVerboseLevel(debugStr);

    // fill the general metadata info: name, title, fstore
    this->SetName(GetParameter("name", "defaultName").c_str());
    this->SetTitle(GetParameter("title", "defaultTitle").c_str());
    this->SetSectionName(this->ClassName());
    fStore =
        ToUpper(GetParameter("store", "true")) == "TRUE" || ToUpper(GetParameter("store", "true")) == "ON";

    return 0;
}

void TRestMetadata::InitFromRootFile() {
    if (configBuffer != "") {
        fElement = StringToElement(configBuffer);
        configBuffer = "";
        // this->InitFromConfigFile();
    }
    gDetector->RegisterMetadata(this);
}

///////////////////////////////////////////////
/// \brief replace the field value(attribute) of the given xml element
///
/// it calls ReplaceEnvironmentalVariables() and
/// ReplaceMathematicalExpressions() in sequence. "name" attribute won't be
/// replaced
TiXmlElement* TRestMetadata::ReplaceElementAttributes(TiXmlElement* e) {
    if (e == NULL) return NULL;

    std::string parName = "";
    TiXmlAttribute* attr = e->FirstAttribute();
    while (attr != NULL) {
        const char* val = attr->Value();
        const char* name = attr->Name();

        if (strcmp(name, "name") == 0) parName = (string)val;

        // set attribute except name field
        if (strcmp(name, "name") != 0) {
            string temp = ReplaceEnvironmentalVariables(val);
            e->SetAttribute(
                name, ReplaceMathematicalExpressions(
                          temp, "Please, check parameter name: " + parName + " (ReplaceElementAttributes)")
                          .c_str());
        }

        attr = attr->Next();
    }

    return e;
}

///////////////////////////////////////////////
/// \brief Identify an environmental variable section and add it into env
/// section list
///
/// Vaild section declaration: "variable", "constant". If the
/// section exists already, its value will be updated if "updateexisting" is
/// true. If a system env with the same name has been defined already, then the
/// system env will be used, unless the attribute "overwrite" is true.
///
/// Example of environmental variable section:
/// \code
/// <variable name="TEST" value="VALUE" overwrite="true" />
/// \endcode
///
void TRestMetadata::ReadEnvInElement(TiXmlElement* e, bool overwrite) {
    if (e == NULL) return;

    const char* name = e->Attribute("name");
    if (name == NULL) return;
    const char* value = e->Attribute("value");
    if (value == NULL) return;

    if ((string)e->Value() == "variable") {
        // if overwrite is false, try to replace the value from system env.
        const char* overwritesysenv = e->Attribute("overwrite");
        if (overwritesysenv == NULL) overwritesysenv = "false";
        if (!StringToBool(overwritesysenv)) {
            char* sysenv = getenv(name);
            if (sysenv != NULL) value = sysenv;
        }
        if (!overwrite && fVariables.count(name) > 0) return;
        fVariables[name] = value;
    } else if ((string)e->Value() == "constant") {
        if (!overwrite && fVariables.count(name) > 0) return;
        fConstants[name] = value;
    } else if ((string)e->Value() == "myParameter") {
        warning << "myParameter is obsolete now! use \"constant\" instead" << endl;
        if (!overwrite && fVariables.count(name) > 0) return;
        fConstants[name] = value;
    }
}

///////////////////////////////////////////////
/// \brief Read the given xml section, applying rml syntax(if, for, include, etc.)
///
/// The expansion is done recursively except for child sections declared after
/// "TRest". They are supposed to be a metadata class and to be doing the
/// expansion themselves. If the argument "recursive" is true, these child
/// sections will also be processed. Before expansion,
/// ReplaceElementAttributes() will first be called.
void TRestMetadata::ReadElement(TiXmlElement* e, bool recursive) {
    debug << ClassName() << "::ReadElement(<" << e->Value() << ")" << endl;

    ReplaceElementAttributes(e);
    ReadEnvInElement(e);

    if ((string)e->Value() == "for") {
        ExpandForLoops(e, {});
    } else if (e->Attribute("file") != NULL) {
        ExpandIncludeFile(e);
    } else if ((string)e->Value() == "if") {
        ExpandIfSections(e);
    } else if (e->FirstChildElement() != NULL) {
        TiXmlElement* contentelement = e->FirstChildElement();
        // we won't expand child TRestXXX sections unless forced recursive. The expansion of
        // these sections will be executed individually by the corresponding TRestXXX class
        while (contentelement != NULL) {
            TiXmlElement* nxt = contentelement->NextSiblingElement();
            if (recursive || ((string)contentelement->Value()).find("TRest") == -1) {
                debug << "into child element \"" << contentelement->Value() << "\" of \"" << e->Value()
                      << "\"" << endl;
                ReadElement(contentelement, recursive);
            } else {
                debug << "skipping child element \"" << contentelement->Value() << "\" of \"" << e->Value()
                      << "\"" << endl;
            }
            contentelement = nxt;
        }
    }
}

///////////////////////////////////////////////
/// \brief Judge the if condition and expands the elements inside it.
///
/// The example IF structure:
/// \code
/// <TRestXXX>
///		<if condition="${HOME}==/home/nkx">
///        <addProcess type="TRestSignalZeroSuppresionProcess" name="zS" value="ON" file="processes.rml"/>
///     </if>
///     <if evaluate="date +%Y-%m-%d" condition=">2019-08-21">
///        <addProcess type = "TRestSignalToHitsProcess" name = "signalToHits" value = "ON" file =
///        "processes.rml" />
///     </if>
/// </TRestXXX>
/// \endcode
/// "evaluate" specifies the shell command, the output of which is used.
/// "condition" specifies the comparing condition.
/// So here if the home directory is "/home/nkx", the process "TRestSignalZeroSuppresionProcess" will be added
/// If the current date is larger than 2019-08-21, the process "TRestSignalToHitsProcess" will be added
///
/// Supports condition markers: `==`, `!=`, `>`, `<`, `<=`, `>=`. Its better to escape the ">", "<" markers.
/// Note that the `>`, `<` calculation is also valid for strings. The ordering is according to the alphabet
///
void TRestMetadata::ExpandIfSections(TiXmlElement* e) {
    if ((string)e->Value() != "if") return;

    const char* evaluate = e->Attribute("evaluate");
    const char* condition = e->Attribute("condition");

    if (condition == NULL || string(condition).find_first_of("=!<>") == -1) {
        warning << "Invalid \"IF\" structure!" << endl;
        return;
    }

    int p1 = string(condition).find_first_of("=!<>");
    int p2 = string(condition).find_first_not_of("=!<>", p1);

    string v1 = "";
    bool matches = false;
    if (evaluate != NULL) {
        v1 = TRestTools::Execute(evaluate);
    } else if (p1 > 0) {
        v1 = string(condition).substr(0, p1);
    } else {
        warning << "Invalid \"IF\" structure!" << endl;
        return;
    }

    string con = string(condition).substr(p1, p2 - p1);
    string v2 = string(condition).substr(p2, -1);

    if (con == "==") {
        if (isANumber(v1) && isANumber(v2)) {
            if (atof(v1.c_str()) == atof(v2.c_str())) matches = true;
        } else {
            if (v1 == v2) matches = true;
        }
    } else if (con == "!=") {
        if (isANumber(v1) && isANumber(v2)) {
            if (atof(v1.c_str()) != atof(v2.c_str())) matches = true;
        } else {
            if (v1 != v2) matches = true;
        }
    } else if (con == ">") {
        if (isANumber(v1) && isANumber(v2)) {
            if (atof(v1.c_str()) > atof(v2.c_str())) matches = true;
        } else {
            if (v1 > v2) matches = true;
        }
    } else if (con == "<") {
        if (isANumber(v1) && isANumber(v2)) {
            if (atof(v1.c_str()) < atof(v2.c_str())) matches = true;
        } else {
            if (v1 < v2) matches = true;
        }
    } else if (con == ">=") {
        if (isANumber(v1) && isANumber(v2)) {
            if (atof(v1.c_str()) >= atof(v2.c_str())) matches = true;
        } else {
            if (v1 >= v2) matches = true;
        }
    } else if (con == "<=") {
        if (isANumber(v1) && isANumber(v2)) {
            if (atof(v1.c_str()) <= atof(v2.c_str())) matches = true;
        } else {
            if (v1 <= v2) matches = true;
        }
    } else {
        warning << "Invalid \"IF\" structure!" << endl;
        return;
    }

    if (matches) {
        TiXmlElement* parele = (TiXmlElement*)e->Parent();
        if (parele == NULL) return;
        TiXmlElement* contentelement = e->FirstChildElement();
        while (contentelement != NULL) {
            TiXmlElement* attachedelement = (TiXmlElement*)contentelement->Clone();
            ReadElement(attachedelement, true);
            // debug << *attachedelement << endl;
            parele->InsertBeforeChild(e, *attachedelement);
            delete attachedelement;
            contentelement = contentelement->NextSiblingElement();
        }
    }
}

///////////////////////////////////////////////
/// \brief Helper method for TRestMetadata::ExpandForLoops().
void TRestMetadata::ExpandForLoopOnce(TiXmlElement* e, map<string, string> forLoopVar) {
    TiXmlElement* parele = (TiXmlElement*)e->Parent();
    TiXmlElement* contentelement = e->FirstChildElement();
    while (contentelement != NULL) {
        if ((string)contentelement->Value() == "for") {
            TiXmlElement* newforloop = (TiXmlElement*)contentelement->Clone();
            // ReplaceElementAttributes(newforloop);
            TiXmlElement* tempnew = (TiXmlElement*)parele->InsertBeforeChild(e, *newforloop);
            delete newforloop;
            newforloop = tempnew;
            ExpandForLoops(newforloop, forLoopVar);
            contentelement = contentelement->NextSiblingElement();
        } else {
            TiXmlElement* attachedelement = (TiXmlElement*)contentelement->Clone();
            ReplaceForLoopVars(attachedelement, forLoopVar);
            ReadElement(attachedelement, true);
            // debug << *attachedelement << endl;
            parele->InsertBeforeChild(e, *attachedelement);
            delete attachedelement;
            contentelement = contentelement->NextSiblingElement();
        }
    }
}

///////////////////////////////////////////////
/// \brief Helper method for TRestMetadata::ExpandForLoops().
void TRestMetadata::ReplaceForLoopVars(TiXmlElement* e, map<string, string> forLoopVar) {
    if (e == NULL) return;

    std::string parName = "";
    TiXmlAttribute* attr = e->FirstAttribute();
    while (attr != NULL) {
        const char* val = attr->Value();
        const char* name = attr->Name();

        if (strcmp(name, "name") == 0) parName = (string)val;

        // set attribute except name field
        if (strcmp(name, "name") != 0) {
            string outputBuffer = val;

            // replace variables with mark ${}
            int startPosition = 0;
            int endPosition = 0;
            while ((startPosition = outputBuffer.find("{", endPosition)) != (int)string::npos) {
                endPosition = outputBuffer.find("}", startPosition + 1);
                if (endPosition == (int)string::npos) break;
                if (startPosition >= 1 && outputBuffer[startPosition - 1] == '$')
                    break;  // to prevent replacing ${} mark

                string expression = outputBuffer.substr(startPosition + 1, endPosition - startPosition - 1);

                int replacePos = startPosition;
                int replaceLen = endPosition - startPosition + 1;

                string proenv = forLoopVar.count(expression) > 0 ? forLoopVar[expression] : "";

                if (proenv != "") {
                    outputBuffer.replace(replacePos, replaceLen, proenv);
                    endPosition = 0;
                } else {
                    ferr << this->ClassName() << ", replace for loop env : cannot find \"{" << expression
                         << "}\"" << endl;
                    exit(1);
                }
            }

            e->SetAttribute(
                name, ReplaceMathematicalExpressions(
                          outputBuffer, "Please, check parameter name: " + parName + " (ReplaceForLoopVars)")
                          .c_str());
        }

        attr = attr->Next();
    }
}

///////////////////////////////////////////////
/// \brief Expands the loop structures found in the given xml section.
///
/// The expansion is done by creating new TiXmlElement objects and inserting
/// them in the given xml section. Loop variable is treated samely as REST
/// "variable"
///
void TRestMetadata::ExpandForLoops(TiXmlElement* e, map<string, string> forloopvar) {
    if ((string)e->Value() != "for") return;
    // ReplaceElementAttributes(e);

    TString varname = TString(e->Attribute("variable"));
    TString varfrom = TString(e->Attribute("from"));
    TString varto = TString(e->Attribute("to"));
    TString varstep = TString(e->Attribute("step"));
    TString varin = TString(e->Attribute("in"));

    if ((varin == "") && (varname == "" || varfrom == "" || varto == "")) return;
    if (varstep == "") varstep = "1";
    TiXmlElement* parele = (TiXmlElement*)e->Parent();
    if (parele == NULL) return;

    string _name = (string)varname;
    string _from = (string)varfrom;
    string _to = (string)varto;
    string _step = (string)varstep;
    string _in = (string)varin;
    if (isANumber(_from) && isANumber(_to) && isANumber(_step)) {
        double from = StringToDouble(_from);
        double to = StringToDouble(_to);
        double step = StringToDouble(_step);

        debug << "----expanding for loop----" << endl;
        double i = 0;
        for (i = from; i <= to; i = i + step) {
            forloopvar[_name] = ToString(i);
            fVariables[_name] = ToString(i);
            ExpandForLoopOnce(e, forloopvar);
        }
        parele->RemoveChild(e);

        if (fVerboseLevel >= REST_Extreme) parele->Print(stdout, 0);
        debug << "----end of for loop----" << endl;
    } else if (_in != "") {
        vector<string> loopvars = Split(_in, ":");

        debug << "----expanding for loop----" << endl;
        for (string loopvar : loopvars) {
            forloopvar[_name] = loopvar;
            fVariables[_name] = loopvar;
            ExpandForLoopOnce(e, forloopvar);
        }
        parele->RemoveChild(e);

        if (fVerboseLevel >= REST_Extreme) parele->Print(stdout, 0);
        debug << "----end of for loop----" << endl;
    }
    // variable defined in for loop should be temporal
    fVariables.erase(_name);
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
/// 2. auto insert. It will automatically find the section in the file,
/// according to "type" and "name". At least one of the two definitions should
/// be specified. Here "type" can either be the element declare or its
/// attribute. After finding the remote section, this method will insert its
/// child sections and attributes into the local xml element. \code <TRestXXX
/// name="sAna" file="abc.rml"/> \endcode Or \code <doXXX type="TRestXXX"
/// name="sAna" file="abc.rml"/> \endcode If the target file is a root file,
/// there will be a different way to load, see TRestRun::ImportMetadata()
void TRestMetadata::ExpandIncludeFile(TiXmlElement* e) {
    debug << "Entering ... " << __PRETTY_FUNCTION__ << endl;

    ReplaceElementAttributes(e);
    const char* _filename = e->Attribute("file");
    if (_filename == NULL) return;

    string filename;
    if (string(_filename) == "server") {
        // Let TRestRun to retrieve data according to run number later-on
        // if ((string) this->ClassName() == "TRestRun") return;

        // match the database, runNumber=0(default data), type="META_RML", tag=<section name>
        auto url = gDataBase->query_data(DBEntry(0, "META_RML", e->Value())).value;
        filename = TRestTools::DownloadRemoteFile(url);
    } else {
        filename = SearchFile(_filename);
    }

    if (filename == "") {
        warning << "REST WARNING(expand include file): Include file \"" << _filename << "\" does not exist!"
                << endl;
        warning << endl;
        return;
    }
    if (!TRestTools::isRootFile(filename))  // root file inclusion is implemented in TRestRun
    {
        debug << "----expanding include file----" << endl;
        // we find the local element(the element to receive content)
        // and the remote element(the element to provide content)
        TiXmlElement* remoteele = NULL;
        TiXmlElement* localele = NULL;
        string type;
        string name;

        ////////////////////////////////////////////////
        // condition 1(raw file include):
        //   <TRestXXX name="" .....>
        //     <include file="aaa.rml"/>
        //     ....
        //   </TRestXXX>
        //
        // We will insert all the xml elements in aaa.rml into this section
        if ((string)e->Value() == "include") {
            localele = (TiXmlElement*)e->Parent();
            if (localele == NULL) return;
            if (localele->Attribute("expanded") == NULL ? false : ((string)localele->Attribute("expanded") ==
                                                                   "true")) {
                debug << "----already expanded----" << endl;
                return;
            }

            remoteele = new TiXmlElement("Config");

            TiXmlElement* ele = GetElementFromFile(filename);
            if (ele == NULL)
                warning << "REST Waring: no xml elements contained in the include file \"" << filename << "\""
                        << endl;
            while (ele != NULL) {
                remoteele->InsertEndChild(*ele);
                ele = ele->NextSiblingElement();
            }

        }

        ///////////////////////////////////
        // condition 2(auto insert):
        //   <TRestXXX file=""/>
        // or
        //   <TRestXXX name="" ... file="aaa.rml" .../>
        // or
        //   <addXXX name="" ... file="aaa.rml" .../>
        // or
        //   <addXXX type="" name="" ... file="aaa.rml" .../>
        //
        // Here TRestXXX will be "type". we will find the corresponding section, and
        // insert all its attributes and child elements into this section. "name"
        // overwrites "type"
        else {
            localele = e;
            if (localele->Attribute("expanded") == NULL ? false : ((string)localele->Attribute("expanded") ==
                                                                   "true")) {
                debug << "----already expanded----" << endl;
                return;
            }

            type = e->Attribute("type") != NULL ? e->Attribute("type") : e->Value();
            name = localele->Attribute("name") == NULL ? "" : localele->Attribute("name");

            // get the root element
            TiXmlElement* rootele = GetElementFromFile(filename);
            if (rootele == NULL) {
                warning << "REST WARNING(expand include file): Include file " << filename
                        << " is of wrong xml format!" << endl;
                warning << endl;
                return;
            }
            if ((string)rootele->Value() == type) {
                // if root element in the included file is of given type, directly use
                // it
                remoteele = rootele;
            } else {
                // import env first
                if (type != "globals" && GetElement("globals", rootele) != NULL) {
                    TiXmlElement* globaldef = GetElement("globals", rootele)->FirstChildElement();
                    while (globaldef != NULL) {
                        ReadEnvInElement(globaldef, false);
                        globaldef = globaldef->NextSiblingElement();
                    }
                }

                // find its child section according to type and name
                if (name != "") {
                    // we find only according to the name
                    vector<TiXmlElement*> eles;
                    TiXmlElement* ele = rootele->FirstChildElement();
                    while (ele != NULL) {
                        if (ele->Attribute("name") != NULL && (string)ele->Attribute("name") == name) {
                            eles.push_back(ele);
                        }
                        ele = ele->NextSiblingElement();
                    }
                    // more than 1 elements found
                    if (eles.size() > 1) {
                        warning << "(expand include file): find multiple xml sections with same name!"
                                << endl;
                        warning << "Using the first one!" << endl;
                    }

                    if (eles.size() > 0) remoteele = (TiXmlElement*)eles[0]->Clone();
                } else if (type != "") {
                    remoteele = (TiXmlElement*)GetElement(type, rootele)->Clone();
                }

                if (remoteele == NULL) {
                    warning << "Cannot find the needed xml section in "
                               "include file!"
                            << endl;
                    warning << "type: \"" << type << "\" , name: \"" << name << "\" . Skipping" << endl;
                    warning << endl;
                    return;
                }
                delete rootele;
            }
        }

        debug << "Target xml element spotted" << endl;

        ///////////////////////////////////////
        // begin inserting remote element into local element
        ReadElement(remoteele, true);
        int nattr = 0;
        int nele = 0;
        TiXmlAttribute* attr = remoteele->FirstAttribute();
        while (attr != NULL) {
            if (localele->Attribute(attr->Name()) == NULL) {
                localele->SetAttribute(attr->Name(), attr->Value());
                nattr++;
            }
            attr = attr->Next();
        }
        TiXmlElement* ele = remoteele->FirstChildElement();
        while (ele != NULL) {
            // ReadElement(ele);
            if ((string)ele->Value() != "for") {
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
        delete remoteele;
        debug << nattr << " attributes and " << nele << " xml elements added by inclusion" << endl;
        debug << "----end of expansion file----" << endl;
    }
}

///////////////////////////////////////////////
/// \brief Returns corresponding REST Metadata parameter from multiple sources
///
/// It first finds the parameter from REST arguments from command line.
/// If not found, it calls GetParameter() method to find parameter in TRestMetadata::fElement
/// If not found, it calls TRestDetector::GetParameter()
/// If still not found, it returns the default value.
///
/// \param parName The name of the parameter from which we want to obtain the
/// value. \param defaultValue The default value if the paremeter is not found
///
/// \return A string of result
string TRestMetadata::GetParameter(std::string parName, TString defaultValue) {
    // first search the parameter in REST args
    if (REST_ARGS.count(parName) != 0) {
        return REST_ARGS[parName];
    }

    // then look within local xml element
    string result = GetParameter(parName, fElement);
    if (result != PARAMETER_NOT_FOUND_STR) {
        return result;
    }

    // finally look into parameters of gDetector
    result = gDetector->GetParameter(parName);
    if (result != PARAMETER_NOT_FOUND_STR) {
        return result;
    }

    return (string)defaultValue;
}

///////////////////////////////////////////////
/// \brief Returns the value for the parameter named **parName** in the given section
///
/// There are two kinds of *parameter* in REST.
/// 1. <parameter name="verboseLevel" value="silent" />
/// 2. <addReadoutModule id="0" name="module" rotation = "0" firstDaqChannel =
/// "272" / >
///
/// The first one is obviously a parameter. The xml element itself serves as a
/// peice of parameter. The name and the value are given in its fields. This is
/// the classic definition. We also generalize the concept of parameter to the
/// elements' fields. All the fields in an element can be seen as parameter. So
/// there are 4 parameters in the second example, including: id, name, rotation
/// and firstDaqChannel. This method first finds parameter in the fields of the
/// given element. If not find, it searches its the child elements. If still not
/// find, it returns the default value. If not specified, the default value
/// string is "NO_SUCH_PARA".
///
/// \param parName The name of the parameter from which we want to obtain the
/// value. \param e The target eml element where the program is to search the
/// parameter \param defaultValue The default value if the paremeter is not
/// found
///
/// \return A string of result, with env and expressions replaced
string TRestMetadata::GetParameter(std::string parName, TiXmlElement* e, TString defaultValue) {
    if (e == NULL) {
        if (GetVerboseLevel() > REST_Debug) {
            cout << "Element is null" << endl;
        }
        return (string)defaultValue;
    }
    string result = (string)defaultValue;
    // first find in attribute
    if (e->Attribute(parName.c_str()) != NULL) {
        result = e->Attribute(parName.c_str());
    }
    // then find in child sections/elements
    else {
        TiXmlElement* element = GetElementWithName("parameter", parName, e);
        if (element != NULL && element->Attribute("value") != NULL) {
            result = element->Attribute("value");
        } else {
            debug << ClassName() << ": Parameter : " << parName << " not found!" << endl;
        }
    }

    return ReplaceMathematicalExpressions(ReplaceEnvironmentalVariables(result),
                                          "Please, check parameter name: " + parName);
}

Double_t TRestMetadata::GetDblParameterWithUnits(std::string parName, TiXmlElement* ele,
                                                 Double_t defaultVal) {
    string a = GetParameter(parName, ele);
    if (a == PARAMETER_NOT_FOUND_STR) {
        return defaultVal;
    } else {
        string unit = GetParameterUnits(parName, ele);
        string valuestr = REST_Units::RemoveUnitsFromString(a);
        Double_t value = StringToDouble(valuestr);
        return REST_Units::ConvertValueToRESTUnits(value, unit);
    }

    return defaultVal;
}

TVector2 TRestMetadata::Get2DVectorParameterWithUnits(std::string parName, TiXmlElement* ele,
                                                      TVector2 defaultVal) {
    string a = GetParameter(parName, ele);
    if (a == PARAMETER_NOT_FOUND_STR) {
        return defaultVal;
    } else {
        string unit = GetParameterUnits(parName, ele);
        string valuestr = REST_Units::RemoveUnitsFromString(a);
        TVector2 value = StringTo2DVector(valuestr);
        Double_t valueX = REST_Units::ConvertValueToRESTUnits(value.X(), unit);
        Double_t valueY = REST_Units::ConvertValueToRESTUnits(value.Y(), unit);
        return TVector2(valueX, valueY);
    }

    return defaultVal;
}

TVector3 TRestMetadata::Get3DVectorParameterWithUnits(std::string parName, TiXmlElement* ele,
                                                      TVector3 defaultVal) {
    string a = GetParameter(parName, ele);
    if (a == PARAMETER_NOT_FOUND_STR) {
        return defaultVal;
    } else {
        string unit = GetParameterUnits(parName, ele);
        string valuestr = REST_Units::RemoveUnitsFromString(a);
        TVector3 value = StringTo3DVector(valuestr);
        Double_t valueX = REST_Units::ConvertValueToRESTUnits(value.X(), unit);
        Double_t valueY = REST_Units::ConvertValueToRESTUnits(value.Y(), unit);
        Double_t valueZ = REST_Units::ConvertValueToRESTUnits(value.Z(), unit);
        return TVector3(valueX, valueY, valueZ);
    }

    return defaultVal;
}

///////////////////////////////////////////////
/// \brief Returns the field value of an xml element which has the specified
/// name.
///
/// A version of GetParameter() but only find parameter in the fields of xml
/// element.
///
std::string TRestMetadata::GetFieldValue(std::string parName, TiXmlElement* e) {
    return GetParameter(parName, e, "Not defined");
}

///////////////////////////////////////////////
/// \brief Gets the value of the parameter name **parName**, after applying unit
/// conversion.
///
/// Searches the parameter in given element. The parameter must be defined
/// providing the additional units, in the same field value or some where in the
/// element. As in the following example :
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
/// We recomment defining units in the same field value, which makes things
/// clear.
///
/// \param parName The name of the parameter from which we want to obtain the
/// value. \param ele The target element in which we are going to search. \param
/// defaultVal The default return value if it fails to find such parameter with
/// unit.
///
/// \return A double/2DVector/3DVector value in the default correspoding REST
/// units (keV, us, mm, Vcm).
///
Double_t TRestMetadata::GetDblParameterWithUnits(std::string parName, Double_t defaultVal) {
    string a = GetParameter(parName);
    if (a == PARAMETER_NOT_FOUND_STR) {
        return defaultVal;
    } else {
        string unit = GetParameterUnits(parName);
        string valuestr = REST_Units::RemoveUnitsFromString(a);
        Double_t value = StringToDouble(valuestr);
        return REST_Units::ConvertValueToRESTUnits(value, unit);
    }
    return defaultVal;
}

TVector2 TRestMetadata::Get2DVectorParameterWithUnits(std::string parName, TVector2 defaultVal) {
    string a = GetParameter(parName);
    if (a == PARAMETER_NOT_FOUND_STR) {
        return defaultVal;
    } else {
        string unit = GetParameterUnits(parName);
        string valuestr = REST_Units::RemoveUnitsFromString(a);
        TVector2 value = StringTo2DVector(valuestr);
        Double_t valueX = REST_Units::ConvertValueToRESTUnits(value.X(), unit);
        Double_t valueY = REST_Units::ConvertValueToRESTUnits(value.Y(), unit);
        return TVector2(valueX, valueY);
    }
    return defaultVal;
}

TVector3 TRestMetadata::Get3DVectorParameterWithUnits(std::string parName, TVector3 defaultVal) {
    string a = GetParameter(parName);
    if (a == PARAMETER_NOT_FOUND_STR) {
        return defaultVal;
    } else {
        string unit = GetParameterUnits(parName);
        string valuestr = REST_Units::RemoveUnitsFromString(a);
        TVector3 value = StringTo3DVector(valuestr);
        Double_t valueX = REST_Units::ConvertValueToRESTUnits(value.X(), unit);
        Double_t valueY = REST_Units::ConvertValueToRESTUnits(value.Y(), unit);
        Double_t valueZ = REST_Units::ConvertValueToRESTUnits(value.Z(), unit);
        return TVector3(valueX, valueY, valueZ);
    }
    return defaultVal;
}

///////////////////////////////////////////////
/// \brief Open an xml encoded file and find its element.
///
/// If NameOrDecalre is a blank string, then it will return the first root
/// element Otherwise it will search within all the root elements and sub-root
/// elements
///
/// The root element is the parent of any other xml elements in the file.
/// There could be only one root element in each xml encoded file in standard
/// xml foamat. We recommened the users to write rml in this way, however,
/// multi-root element is still supported in this method.
///
/// Exits the whole program if the xml file does not exist, or is in wrong in
/// syntax. Returns NULL if no element matches NameOrDecalre
///
TiXmlElement* TRestMetadata::GetElementFromFile(std::string cfgFileName, std::string NameOrDecalre) {
    TiXmlDocument doc;
    TiXmlElement* rootele;

    string filename = cfgFileName;
    if (TRestMetadata_UpdatedConfigFile.count(filename) > 0)
        filename = TRestMetadata_UpdatedConfigFile[filename];

    if (!TRestTools::fileExists(filename)) {
        ferr << "Config file does not exist. The file is: " << filename << endl;
        GetChar();
        exit(1);
    }
    if (!doc.LoadFile(filename.c_str())) {
        RmlUpdateTool t(filename, true);
        if (t.UpdateSucceed()) {
            TRestStringOutput cout;
            cout.setcolor(COLOR_BOLDYELLOW);
            cout << "REST WARNING : You are still using V2.1 config file, this file "
                    "is successfully"
                 << endl;
            cout << "updated by REST. In future we may remove this self-adaption "
                    "functionality."
                 << endl;
            cout << "So it is recommended to check and use the generated new file as "
                    "soon as possible!"
                 << endl;
            cout << filename << "  -->  " << t.GetOutputFile() << endl;
            GetChar();
            TRestMetadata_UpdatedConfigFile[filename] = t.GetOutputFile();
            return GetElementFromFile(t.GetOutputFile());
        } else {
            ferr << "Failed to load xml file, syntax maybe wrong. The file is: " << filename << endl;
            exit(1);
        }
    }

    rootele = doc.RootElement();
    if (rootele == NULL) {
        ferr << "The rml file \"" << cfgFileName << "\" does not contain any valid elements!" << endl;
        GetChar();
        exit(1);
    }
    if (NameOrDecalre == "") {
        return (TiXmlElement*)rootele->Clone();
    }
    // search with either name or declare in either root element or sub-root
    // element
    while (rootele != NULL) {
        if (rootele->Value() != NULL && (string)rootele->Value() == NameOrDecalre) {
            return (TiXmlElement*)rootele->Clone();
        }

        if (rootele->Attribute("name") != NULL && (string)rootele->Attribute("name") == NameOrDecalre) {
            return (TiXmlElement*)rootele->Clone();
        }

        TiXmlElement* etemp = GetElement(NameOrDecalre, rootele);
        if (etemp != NULL) {
            return (TiXmlElement*)etemp->Clone();
        }

        etemp = GetElementWithName("", NameOrDecalre, rootele);
        if (etemp != NULL) {
            return (TiXmlElement*)etemp->Clone();
        }

        rootele = rootele->NextSiblingElement();
    }

    return NULL;
    /*ferr << "Cannot find xml element with name \""<< NameOrDecalre <<"\" in rml
    file \"" << cfgFileName << endl; GetChar(); exit(1);*/
}

///////////////////////////////////////////////
/// \brief Get an xml element from default location(TRestMetadata::fElement),
/// according to its declaration
///
TiXmlElement* TRestMetadata::GetElement(std::string eleDeclare) { return GetElement(eleDeclare, fElement); }

///////////////////////////////////////////////
/// \brief Get an xml element from a given parent element, according to its
/// declaration
///
TiXmlElement* TRestMetadata::GetElement(std::string eleDeclare, TiXmlElement* e) {
    return e->FirstChildElement(eleDeclare.c_str());
}

///////////////////////////////////////////////
/// \brief Get the next sibling xml element of this element, with same eleDeclare
///
TiXmlElement* TRestMetadata::GetNextElement(TiXmlElement* e) { return e->NextSiblingElement(e->Value()); }

///////////////////////////////////////////////
/// \brief Get an xml element from the default location, according to its
/// declaration and its field "name"
///
TiXmlElement* TRestMetadata::GetElementWithName(std::string eleDeclare, std::string eleName) {
    return GetElementWithName(eleDeclare, eleName, fElement);
}

///////////////////////////////////////////////
/// \brief Get an xml element from a given parent element, according to its
/// declaration and its field "name"
///
TiXmlElement* TRestMetadata::GetElementWithName(std::string eleDeclare, std::string eleName,
                                                TiXmlElement* e) {
    if (eleDeclare == "")  // find only with name
    {
        TiXmlElement* ele = e->FirstChildElement();
        while (ele != NULL) {
            if (ele->Attribute("name") != NULL && (string)ele->Attribute("name") == eleName) {
                return ele;
            }
            ele = ele->NextSiblingElement();
        }
        return ele;
    } else  // find with name and declare
    {
        TiXmlElement* ele = e->FirstChildElement(eleDeclare.c_str());
        while (ele != NULL) {
            if (ele->Attribute("name") != NULL && (string)ele->Attribute("name") == eleName) {
                return ele;
            }
            ele = ele->NextSiblingElement(eleDeclare.c_str());
        }
        return ele;
    }

    return NULL;
}

///////////////////////////////////////////////
/// \brief Returns a string with the unit name given in the given xml element
///
/// if given the target attribute, it will find the unit for this.
/// e.g. value="(1,-13)mm"  "-3mm"  "50,units=mm"  can both be recoginzed.
/// if not given, it will find the unit as a parameter of the element.
///	e.g. <... value="3" units="mm" .../>
string TRestMetadata::GetUnits(TiXmlElement* e) {
    if (e == NULL) {
        warning << "TRestMetadata::GetUnits(): NULL element given!" << endl;
        return "";
    }

    string valstr = e->Attribute("value") == NULL ? "" : e->Attribute("value");
    string unitattr = e->Attribute("units") == NULL ? "" : e->Attribute("units");

    string unitembeded = REST_Units::FindRESTUnitsInString(valstr);
    if (IsUnit(unitembeded)) {
        return unitembeded;
    }
    if (IsUnit(unitattr)) {
        return unitattr;
    }
    return "";
}

///////////////////////////////////////////////
/// \brief Returns the unit string of the given parameter of the given xml section
///
/// It will firstly find the parameter section from the given xml section. 
/// Then it will search units definition in:
/// 1. value string of this parameter
/// 2. "units" attribute of the parameter section
/// 3. "units" attribute of the given section
///
/// If argument section is not given(==NULL), it will use the local section(fElement)
string TRestMetadata::GetParameterUnits(string parName, TiXmlElement* e) {
    if (e == NULL) e = fElement;
    string parvalue = GetParameter(parName, e);
    if (parvalue == PARAMETER_NOT_FOUND_STR) {
        return "";
    } else {
        // first try to use unit embeded in parvalue
        string unit = REST_Units::FindRESTUnitsInString(parvalue);
        // then try to find unit in corresponding "parameter" section
        if (unit == "") {
            TiXmlElement* paraele = GetElementWithName("parameter", parName, e);
            if (paraele != NULL) {
                unit = GetUnits(paraele);
            }
        }
        // finally try to find unit in local section attribute
        if (unit == "") {
            unit = GetUnits(e);
        }
        return unit;
    }
    return "";
}

///////////////////////////////////////////////
/// \brief Parsing a string into TiXmlElement object
///
/// This method creates TiXmlElement object with the alloator "new".
/// Be advised to delete the object after using it!
TiXmlElement* TRestMetadata::StringToElement(string definition) {
    TiXmlElement* ele = new TiXmlElement("temp");
    // TiXmlDocument*doc = new TiXmlDocument();
    ele->Parse(definition.c_str(), NULL, TIXML_ENCODING_UTF8);
    return ele;
}

///////////////////////////////////////////////
/// \brief Convert an TiXmlElement object to string
///
/// This method does't arrange the output. All the contents are written in one
/// line.
string TRestMetadata::ElementToString(TiXmlElement* ele) {
    if (ele != NULL) {
        // remove comments
        TiXmlNode* n = ele->FirstChild();
        while (n != NULL) {
            TiXmlComment* cmt = n->ToComment();
            if (cmt != NULL) {
                TiXmlNode* nn = n;
                n = n->NextSibling();
                ele->RemoveChild(nn);
                continue;
            }
            n = n->NextSibling();
        }

        stringstream ss;
        ss << (*ele);
        string s = ss.str();

        // int pos = 0;
        // int pos2 = 0;
        // while ((pos = s.find("<!--", pos)) != -1 && (pos2 = s.find("-->", pos))
        // != -1)
        //{
        //	s.replace(pos, pos2 - pos + 3, "");//3 is the length of "-->"
        //	pos = pos + 1;
        //}

        return s;
    }

    return " ";
}

///////////////////////////////////////////////
/// \brief Gets the first key structure for **keyName** found inside **buffer**
/// after **fromPosition**.
///
/// A key definition is written as follows:
/// \code <keyName field1="value1" field2="value2" >
///
///     ....
///
///  </keyName>
/// \endcode
///
string TRestMetadata::GetKEYStructure(std::string keyName) {
    size_t Position = 0;
    string result = GetKEYStructure(keyName, Position, fElement);
    if (result == "") result = "NotFound";
    return result;
}
string TRestMetadata::GetKEYStructure(std::string keyName, size_t& Position) {
    string result = GetKEYStructure(keyName, Position, fElement);
    if (result == "") result = "NotFound";
    return result;
}
string TRestMetadata::GetKEYStructure(std::string keyName, string buffer) {
    size_t Position = 0;
    string result = GetKEYStructure(keyName, Position, buffer);
    if (result == "") result = "NotFound";
    return result;
}
string TRestMetadata::GetKEYStructure(std::string keyName, size_t& fromPosition, string buffer) {
    TiXmlElement* ele = StringToElement(buffer);
    string result = GetKEYStructure(keyName, fromPosition, ele);
    delete ele;
    return result;
}
string TRestMetadata::GetKEYStructure(std::string keyName, size_t& fromPosition, TiXmlElement* ele) {
    size_t position = fromPosition;

    debug << "Finding " << fromPosition << "th appearance of KEY Structure \"" << keyName << "\"..." << endl;

    TiXmlElement* childele = ele->FirstChildElement(keyName);
    for (int i = 0; childele != NULL && i < fromPosition; i++) {
        childele = childele->NextSiblingElement(keyName);
    }
    if (childele != NULL) {
        string result = ElementToString(childele);
        fromPosition = fromPosition + 1;
        debug << "Found Key : " << result << endl;
        // debug << "New position : " << fromPosition << endl;
        return result;
    }

    debug << "Finding hit the end, KEY Structure not found!!" << endl;
    return "";
}

///////////////////////////////////////////////
/// \brief Gets the first key definition for **keyName** found inside **buffer**
/// starting at **fromPosition**.
///
/// A key definition is written as follows:
/// \code <keyName field1="value1" field2="value2" > \endcode
///
/// The returned key definition will be fixed to:
/// \code <keyName field1="value1" field2="value2" /> \endcode
/// which is in standard xml form
string TRestMetadata::GetKEYDefinition(string keyName) {
    string buffer = ElementToString(fElement);
    size_t Position = 0;
    return GetKEYDefinition(keyName, Position, buffer);
}
string TRestMetadata::GetKEYDefinition(string keyName, size_t& fromPosition) {
    string buffer = ElementToString(fElement);
    return GetKEYDefinition(keyName, fromPosition, buffer);
}
string TRestMetadata::GetKEYDefinition(string keyName, string buffer) {
    size_t Position = 0;
    return GetKEYDefinition(keyName, Position, buffer);
}
string TRestMetadata::GetKEYDefinition(string keyName, size_t& fromPosition, string buffer) {
    string key = "<" + keyName;
    size_t startPos = buffer.find(key, fromPosition);
    if (startPos == string::npos) return "";
    size_t endPos = buffer.find(">", startPos);
    if (endPos == string::npos) return "";

    fromPosition = endPos;

    Int_t notDefinitionEnd = 1;

    while (notDefinitionEnd) {
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

    string result = buffer.substr(startPos, endPos - startPos + 1);
    if (result[result.size() - 2] != '/') result.insert(result.size() - 1, 1, '/');
    // cout << result << endl << endl;
    // getchar();
    return result;
}

///////////////////////////////////////////////
/// \brief This method updates all the field names inside the definition
/// string provided by argument to make them upper case, the result will
/// be given in the return string
///
string TRestMetadata::FieldNamesToUpper(string definition) {
    string result = definition;
    TiXmlElement* e = StringToElement(definition);
    if (e == NULL) return NULL;

    TiXmlAttribute* attr = e->FirstAttribute();
    while (attr != NULL) {
        string parName = std::string(attr->Name());

        size_t pos = 0;
        result = Replace(result, parName, ToUpper(parName), pos);

        attr = attr->Next();
    }

    return result;
}

///////////////////////////////////////////////
/// \brief Gets field value in an xml element string by parsing it as
/// TiXmlElement
///
std::string TRestMetadata::GetFieldValue(std::string fieldName, std::string definition, size_t fromPosition) {
    TiXmlElement* ele = StringToElement(FieldNamesToUpper(definition));
    string value = GetFieldValue(ToUpper(fieldName), ele);
    delete ele;
    return value;
}

///////////////////////////////////////////////
/// \brief Returns the value for the parameter name **parName** found in
/// **inputString**.
///
/// The methods starts searching in **inputString** after a given position
/// **pos**.
///
string TRestMetadata::GetParameter(string parName, size_t& pos, string inputString) {
    pos = inputString.find(parName, pos);

    TiXmlElement* ele = StringToElement(inputString);
    string value = GetParameter(parName, ele);

    delete ele;
    return value;
}

///////////////////////////////////////////////
/// \brief Identifies enviromental variable replacing marks in the input buffer,
/// and replace them with corresponding value.
///
/// Replacing marks:
/// 1. ${VARIABLE_NAME} : search system env and variable/constant. system env in prior
/// 2. VARIABLE_NAME    : try match the names of variable/constant and replace it if matched.
string TRestMetadata::ReplaceEnvironmentalVariables(const string buffer) {
    string outputBuffer = buffer;

    // replace variables with mark ${}
    int startPosition = 0;
    int endPosition = 0;
    while ((startPosition = outputBuffer.find("${", endPosition)) != (int)string::npos) {
        endPosition = outputBuffer.find("}", startPosition + 2);
        if (endPosition == (int)string::npos) break;

        string expression = outputBuffer.substr(startPosition + 2, endPosition - startPosition - 2);

        int replacePos = startPosition;
        int replaceLen = endPosition - startPosition + 1;

        string sysenv = getenv(expression.c_str()) != NULL ? getenv(expression.c_str()) : "";
        string proenv = fVariables.count(expression) > 0 ? fVariables[expression] : "";

        if (sysenv != "") {
            outputBuffer.replace(replacePos, replaceLen, sysenv);
            endPosition = 0;
        } else if (proenv != "") {
            outputBuffer.replace(replacePos, replaceLen, proenv);
            endPosition = 0;
        } else {
            ferr << this->ClassName() << ", replace env : cannot find \"${" << expression << "}\"" << endl;
            ferr << "(position: " << startPosition << ") in either system or program env, exiting..." << endl;
            exit(1);
        }
    }

    // replace bare constant name. ignore sub strings.
    // e.g. variable "nCh" with value "3" cannot replace the string "nChannels+1"
    startPosition = 0;
    endPosition = 0;
    for (auto iter : fConstants) {
        int pos = outputBuffer.find(iter.first, 0);
        while (pos != -1) {
            char next =
                (pos + iter.first.size()) >= outputBuffer.size() ? 0 : outputBuffer[pos + iter.first.size()];
            char prev = pos == 0 ? 0 : outputBuffer[pos - 1];
            if (!isalpha(next) && !isalpha(prev)) {
                outputBuffer.replace(pos, iter.first.size(), iter.second);
                pos = outputBuffer.find(iter.first, pos + iter.second.size());
            } else {
                pos = outputBuffer.find(iter.first, pos + iter.first.size());
            }
        }
    }

    return outputBuffer;
}

///////////////////////////////////////////////
/// \brief Search files in current directory and directories specified in
/// "searchPath" section
///
/// Return blank string if file not found, return directly filename if found in
/// current directory, return full name (path+name) if found in "searchPath".
string TRestMetadata::SearchFile(string filename) {
    if (TRestTools::fileExists(filename)) {
        return filename;
    } else {
        auto pathstring = GetSearchPath();
        auto paths = Split((string)pathstring, ":");
        return TRestTools::SearchFileInPath(paths, filename);
    }
}

///////////////////////////////////////////////
/// \brief Prints a UNIX timestamp in human readable format.
///
void TRestMetadata::PrintTimeStamp(Double_t timeStamp) {
    cout.precision(10);

    time_t tt = (time_t)timeStamp;
    struct tm* tm = localtime(&tt);

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
void TRestMetadata::PrintConfigBuffer() {
    if (fElement != NULL) {
        fElement->Print(stdout, 0);
        cout << endl;
    } else {
        if (configBuffer != "") {
            auto ele = StringToElement(configBuffer);
            ele->Print(stdout, 0);
            cout << endl;
            delete ele;
        } else {
            cout << "N/A" << endl;
        }
    }
}

void TRestMetadata::WriteConfigBuffer(string fname) {
    if (fElement != NULL) {
        FILE* f = fopen(fname.c_str(), "at");
        fElement->Print(f, 0);
        fclose(f);
        return;
    } else if (configBuffer != "") {
        FILE* f = fopen(fname.c_str(), "at");
        auto ele = StringToElement(configBuffer);
        ele->Print(f, 0);
        fclose(f);
        delete ele;
        return;
    }

    ferr << "Something missing here. Call the police" << endl;
}

void TRestMetadata::PrintMessageBuffer() { cout << messageBuffer << endl; }

int TRestMetadata::GetChar(string hint) {
    if (gApplication != NULL && !gApplication->IsRunning()) {
        thread t = thread(&TApplication::Run, gApplication, true);
        t.detach();

        cout << hint << endl;
        int result = getchar();
        gSystem->ExitLoop();
        return result;
    } else {
        cout << hint << endl;
        return getchar();
    }
    return -1;
}

///////////////////////////////////////////////
/// \brief Prints metadata content on screen. Usually overloaded by the derived
/// metadata class.
///
void TRestMetadata::PrintMetadata() {
    metadata << "+++++++++++++++++++++++++++++++++++++++++++++" << endl;
    metadata << this->ClassName() << " content" << endl;
    metadata << "Config file : " << fConfigFileName << endl;
    metadata << "+++++++++++++++++++++++++++++++++++++++++++++" << endl;
    metadata << "Name : " << GetName() << endl;
    metadata << "Title : " << GetTitle() << endl;
    metadata << "REST Version : " << GetVersion() << endl;
    metadata << "REST Commit : " << GetCommit() << endl;
    metadata << "REST Library version : " << GetLibraryVersion() << endl;
    metadata << "---------------------------------------" << endl;
}

///////////////////////////////////////////////
/// \brief Returns the REST version stored in fVersion
///
TString TRestMetadata::GetVersion() { return fVersion; }

///////////////////////////////////////////////
/// \brief Returns the REST commit value stored in fCommit
///
TString TRestMetadata::GetCommit() { return fCommit; }

///////////////////////////////////////////////
/// \brief Returns the REST libraty version stored in fLibraryVersion
///
TString TRestMetadata::GetLibraryVersion() { return fLibraryVersion; }

///////////////////////////////////////////////
/// \brief Resets the version of TRestRun to REST_RELEASE. Only TRestRun is
/// allowed to update version.
void TRestMetadata::ReSetVersion() {
    if (!this->InheritsFrom("TRestRun"))
        ferr << "version is a static value, you cannot set version "
                "for a class!"
             << endl;
    else {
        fVersion = REST_RELEASE;
    }
}

///////////////////////////////////////////////
/// \brief Resets the version of TRestRun to -1, in case the file is old REST file.
/// Only TRestRun is allowed to update version.
void TRestMetadata::UnSetVersion() {
    if (!this->InheritsFrom("TRestRun"))
        ferr << "version is a static value, you cannot set version "
                "for a class!"
             << endl;
    else {
        fVersion = -1;
        fCommit = -1;
    }
}

///////////////////////////////////////////////
/// \brief Set the library version of this metadata class
///
void TRestMetadata::SetLibraryVersion(TString version) { fLibraryVersion = version; }

Int_t TRestMetadata::GetVersionCode() { return TRestTools::ConvertVersionCode((string)GetVersion()); }

///////////////////////////////////////////////
/// \brief Returns the section name of this class, defined at the beginning of
/// fSectionName
std::string TRestMetadata::GetSectionName() {
    auto a = fSectionName.find('\n', 0);
    if (a != -1) return fSectionName.substr(0, a);
    return fSectionName;
}

///////////////////////////////////////////////
/// \brief Returns the config section of this class
std::string TRestMetadata::GetConfigBuffer() { return configBuffer; }

///////////////////////////////////////////////
/// \brief Get the value of datamember as string.
///
/// Note that only streamed datamembers can be read, others will just
/// return an empty string.
///
string TRestMetadata::GetDataMemberValue(string memberName) {
    any member = REST_Reflection::GetDataMember(any((char*)this, this->ClassName()), memberName);
    if (!member.IsZombie()) {
        return ToString(member);
    }
    return "";
}

///////////////////////////////////////////////
/// \brief Get the value of datamember as a vector of strings.
///
/// If the datamember specified in the argument is a vector with several
/// elements, those elements will be assigned to the std::vector. If the
/// argument requests a data member that is not a vector in nature, this
/// method will still return a valid vector string with a single element.
///
/// Note that only streamed datamembers can be read, others will just
/// return an empty string.
///
std::vector<string> TRestMetadata::GetDataMemberValues(string memberName) {
    string result = GetDataMemberValue(memberName);

    result = Replace(result, "{", "");
    result = Replace(result, "}", "");

    return Split(result, ",");
}

///////////////////////////////////////////////
/// \brief Returns a string corresponding to current verbose level.
///
TString TRestMetadata::GetVerboseLevelString() {
    TString level = "unknown";
    if (this->GetVerboseLevel() == REST_Debug) level = "debug";
    if (this->GetVerboseLevel() == REST_Info) level = "info";
    if (this->GetVerboseLevel() == REST_Essential) level = "warning";
    if (this->GetVerboseLevel() == REST_Silent) level = "silent";

    return level;
}

///////////////////////////////////////////////
/// Returns a string with a list of pathes defined, in decreasing order of
/// precedence:
///  1) in sections "searchPath",
///  2) in "configPath"（the path of main rml file）,
///  3) default data path: "$REST_PATH/data/"
///
/// To add a searchPath, use:
/// \code
/// <searchPath value="$ENV{REST_INPUTDATA}/definitions/"/>
/// \endcode
/// Or
/// \code
/// <searchPath
/// value="$ENV{REST_INPUTDATA}/definitions/:$ENV{REST_INPUTDATA}/gasFiles/"/>
/// \endcode
/// "searchPath" can also be added multiple times. Both of them will be added
/// into the output string. A separator ":" is inserted between each defined
/// paths. To separate them, use inline method Split() provided by
/// TRestStringHelper. Uniformed search path definition provides us uniformed
/// file search tool, see TRestMetadata::SearchFile().
TString TRestMetadata::GetSearchPath() {
    string result = "";

    // If fElement=0 we haven't initialized the class from RML.
    // Then we skip adding user paths
    if (fElement) {
        TiXmlElement* ele = fElement->FirstChildElement("searchPath");
        while (ele != NULL) {
            if (ele->Attribute("value") != NULL) {
                result += (string)ele->Attribute("value") + ":";
            }
            ele = ele->NextSiblingElement("searchPath");
        }
    }

    if (getenv("configPath")) result += getenv("configPath") + (string) ":";
    result += REST_PATH + "/data/:";
    if (result.back() == ':') result.erase(result.size() - 1);

    return ReplaceEnvironmentalVariables(result);
}

Int_t TRestMetadata::Write(const char* name, Int_t option, Int_t bufsize) {
    if (fStore) {
        configBuffer = ElementToString(fElement);
        return TNamed::Write(name, option, bufsize);
    }
    return -1;
}

///////////////////////////////////////////////
/// \brief Reflection methods, Set value of a datamember in class according to
/// TRestMetadata::fElement
///
/// It will search for "parameter" section with the same name of the datamember,
/// and set it's value. For example \code class TRestXXX: public TRestMetadata{
/// int par0;
/// }
///
/// <TRestXXX name="..." par0="10"/>
/// \endcode
/// After loading the rml file and calling this method, the value of "par0" will
/// be set to 10
///
void TRestMetadata::SetDataMemberValFromConfig() {}
