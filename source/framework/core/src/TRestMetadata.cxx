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
/// \section metadata_rml RML file structure
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
/// \section metadata_start Sequencial start up procedure of a metadata class
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
/// void LoadConfigFromFile(const char *configFilename, string sectionName = "");
/// void LoadConfigFromFile(TiXmlElement* eSectional, TiXmlElement* eGlobal);
/// void LoadConfigFromFile(TiXmlElement* eSectional, TiXmlElement* eGlobal,
/// map<string, string> envs);
///
/// \endcode
///
/// If no arguments are provided, LoadConfigFromFile() will only call the Initialize()
/// method. If given the rml file name, it will find out the needed rml sections. We
/// can also directly give the xml sections to the method. Two xml sections are used
/// to startup the class: the section for the class and a global section. Additionally
/// we can give a map object to the method to import additional variables.
///
/// The "section for the class" is an xml section with the value of class name.
/// It is the main information source for the class's startup. The "global"
/// section is a special xml section in the rml file, containing global information
/// which could be seen by all the class sections.
///
/// With the xml sections given, LoadConfigFromFile() first merge them together. Then it
/// calls LoadSectionMetadata(), which loads some universal parameters like
/// name, title and verbose level. This method also preprocesses the config
/// sections, expanding the include/for definition and replacing the variables.
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
/// 	while (e != nullptr)
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
/// \section metadata_replace Replacement of variables and expressions
///
/// By default, LoadConfigFromFile() will look into only the first-level child sections
/// of both global and sectional section. If the section value is either
/// "variable" or "constant", the class will keep them for replacement.
///
/// \code
///   <global>
///       <variable name = "CHANNELS" value = "64" overwrite = "false" / >
///       //this variable can be loaded by both
///   </global>
///
///   <TRestRun name="userGivenName" title="User given title" >
///       <constant name="nChannels" value="${CHANNELS}" /> //this variable
///       will be added to the class "TRestDetectorReadout"
///
///       <TRestDetectorReadout name="aaaa" >
///           <variable .... / > //this variable cannot be loaded by the class
///           "TRestDetectorReadout"
///       </TRestDetectorReadout>
///       <parameter name="Ch" value="nChannels+${CHANNELS}-2" />
///   </TRestRun>
///
/// \endcode
///
/// LoadConfigFromFile() will replace the field values of xml sections before
/// they are used. The procedure of replacing is as following:
///
/// 1. recognize the strings surrounded by "${}". Seek and replace in system
/// env first. If not found, replace with variable/constant.
/// 2. directly replace the strings matching the name of constant by their value.
///
/// After replacement, LoadConfigFromFile() will call TFormula to evaluate the
/// string if it is identified as an math expression.
///
/// The result string of the field value is either a number string or a string
/// with concrete content. In the example code above, the section declared with
/// `parameter` will have its field value reset to string 126.
///
/// See also the section \ref metadata_globals.
///
/// \section metadata_external Including external RML files in a main RML file
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
/// \section metadata_loop For loop definition
///
/// The definition of FOR loops is implemented in RML in order to allow extense
/// definitions, where many elements may need to be added to an existing array
/// in our metadata structure. The use of FOR loops allows to introduce more
/// versatil and extense definitions. Its implementation was fundamentally
/// triggered by its use in the construction of complex, multi-channel generic
/// readouts by TRestDetectorReadout.
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
/// \section metadata_if If definition
///
/// It is supported in rml to use <if structure. If the condition matches, the inner
/// sections of <if section will be expaned to and seen by the parent section.
/// This helps to identify different use cases of the rml. For example:
///
/// \code
/// <TRestXXX>
///		<if condition="${HOME}==/home/nkx">
///        <addProcess type="TRestRawZeroSuppresionProcess" name="zS" value="ON" file="processes.rml"/>
///     </if>
///     <if evaluate="date +%Y-%m-%d" condition=">2019-08-21">
///        <addProcess type = "TRestDetectorSignalToHitsProcess" name = "signalToHits" value = "ON" file =
///        "processes.rml" />
///     </if>
/// </TRestXXX>
/// \endcode
///
/// Here "condition" specifies the comparing condition, it can either be a complete expression or just
/// the right side of it. In incomplete case, the left side shall be expressed by "evaluate",
/// which is a shell command to be executed.
///
/// In the example, if the home directory is "/home/nkx", the process "TRestRawZeroSuppresionProcess"
/// will be added. If the date is larger than 2019-08-21, the process "TRestDetectorSignalToHitsProcess"
/// will be added.
///
/// Condition markers supports: `==`, `!=`, `>`, `<`, `<=`, `>=`. Its better to escape the ">", "<" markers.
/// Note that those markers are also valid for strings. If the two sides of the expression
/// are not numbers, REST will perform string comparasion. The ordering is according to the alphabet.
/// A common misleading fact is that "1235">"12345", rather than 1235<12345 as number.
///
/// \section metadata_globals The globals section
///
/// The *globals* section allows to specify common definitions that can be used by any other metadata or
/// process definitions found in the same RML. We may define a parameter (such as the `sampling` rate of
/// electronics) which is usually common to different rawsignal processes. Or we can define a
/// particular value is to be used many times by different classes, it is better to create a `constant`
/// or `variable` inside the `globals` section.
///
/// Possible key values included in this section are:
/// - **searchPath**: It will be used by TRestMetadata::SearchFile to find the full path to a specific
/// file. The paths defined this way will have priority over those defined internally inside REST. The
/// method TRestMetadata::GetSearchPath returns the paths with the corresponding priority.
///
/// - **parameter**: It will be used as a metadata parameter by other metadata/process definitions. If
/// a given metadata or process does not define that parameter, then this will be just ignored.
///
/// - **variable**: It defines an internal variable which will be used in the same way as an system
/// environmental variable. Thus, when using it in other parts of the RML we must use the `${}` specifier.
///
/// - **constant**: It works the same way as a variable but we do not need to use the `${}` specifier.
///
/// \code
///	<globals>
///		<searchPath value = "$ENV{REST_INPUTDATA}/definitions/" />
///		<parameter name="verboseLevel" value="info" />
///		<parameter name="mainDataPath" value="" />
/// 	<parameter name="sampling" value="5us" />
///
/// 	<variable name="DetectorPosZ" value="${SYSTEM_POSITION_Z}" />
/// 	<variable name="field" value="5" /> // V/cm
/// 	<constant name="OpticsPosZ" value="3" />
///	</globals>
/// \endcode
///
///
/// The global section will have effect on *all the metadata structures* (or sections) that are defined
/// in a same RML file. It does not affect to other possible linked sections defined by reference using
/// for example nameref.
///
/// See also the section \ref metadata_replace.
///
/// \section metadata_search Universal file search path
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
/// \section metadata_default Default fields for a section
///
/// Three fields in the first line of an xml section will be looked for before
/// anything else. They are: name, title, and verboseLevel. If not specified,
/// LoadConfigFromFile() will find in the *globals* section. If still not fond, it will
/// use the default value.
///
/// Field "name" and "title" is needed by TNamed classes. The "verboseLevel" is
/// used for changing the amount of output infomation. The following line would
/// print on screen any debug message implemented in TRestDetectorSignalAnalysisProcess.
///
/// \code
/// <TRestDetectorSignalAnalysisProcess name="sgnlAna" title="Data analysis"
/// verboseLevel="debug" >
///     ...
/// </TRestDetectorSignalAnalysisProcess>
/// \endcode
///
///
/// \section metadata_units Using physical units in fields definitions
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
/// \section metadata_other Other useful public tools
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

#include <TFormula.h>
#include <TMath.h>
#include <TStreamerInfo.h>

#include <iomanip>

#include "TRestDataBase.h"

// implementation of version methods in namespace rest_version
/*
namespace REST_VersionGlob {
       inline TString GetRESTVersion() const { return REST_RELEASE; }
       inline int GetRESTVersionCode() const { return ConvertVersionCode(REST_RELEASE); }
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
TRestMetadata::TRestMetadata() : RESTendl(this) {
    fStore = true;
    fElementGlobal = nullptr;
    fElement = nullptr;
    fVerboseLevel = gVerbose;
    fVariables.clear();
    fConstants.clear();
    fHostmgr = nullptr;

    fConfigFileName = "null";
    configBuffer = "";
    RESTMetadata.setlength(100);

#ifdef WIN32
    fOfficialRelease = true;
    fCleanState = true;
#else
    if (TRestTools::Execute("rest-config --release") == "Yes") fOfficialRelease = true;
    if (TRestTools::Execute("rest-config --clean") == "Yes") fCleanState = true;
#endif
}

///////////////////////////////////////////////
/// \brief constructor
///
TRestMetadata::TRestMetadata(const char* configFilename) : RESTendl(this) {
    fStore = true;
    fElementGlobal = nullptr;
    fElement = nullptr;
    fVerboseLevel = gVerbose;
    fVariables.clear();
    fConstants.clear();
    fHostmgr = nullptr;

    fConfigFileName = configFilename;
    configBuffer = "";
    RESTMetadata.setlength(100);

#ifdef WIN32
    fOfficialRelease = true;
    fCleanState = true;
#else
    if (TRestTools::Execute("rest-config --release") == "Yes") fOfficialRelease = true;
    if (TRestTools::Execute("rest-config --clean") == "Yes") fCleanState = true;
#endif
}

///////////////////////////////////////////////
/// \brief TRestMetadata default destructor
///
TRestMetadata::~TRestMetadata() {
    delete fElementGlobal;
    delete fElement;
}

///////////////////////////////////////////////
/// \brief Give the file name, find out the corresponding section. Then call the
/// main starter.
///
Int_t TRestMetadata::LoadConfigFromFile(const string& configFilename, const string& sectionName) {
    fConfigFileName = configFilename;

    const string thisSectionName = sectionName.empty() ? this->ClassName() : sectionName;

    if (!TRestTools::fileExists(fConfigFileName)) fConfigFileName = SearchFile(fConfigFileName);

    if (TRestTools::fileExists(fConfigFileName)) {
        // find the xml section corresponding to the sectionName
        TiXmlElement* Sectional = GetElementFromFile(fConfigFileName, thisSectionName);
        if (Sectional == nullptr) {
            RESTError << "cannot find xml section \"" << ClassName() << "\" with name \"" << sectionName
                      << "\"" << RESTendl;
            RESTError << "in config file: " << fConfigFileName << RESTendl;
            exit(1);
        }

        // find the "globals" section. Multiple sections are supported.
        TiXmlElement* rootEle = GetElementFromFile(fConfigFileName);
        TiXmlElement* Global = GetElement("globals", rootEle);
        if (Global != nullptr) ReadElement(Global);
        if (Global != nullptr && Global->NextSiblingElement("globals") != nullptr) {
            TiXmlElement* ele = Global->NextSiblingElement("globals");
            if (ele != nullptr) ReadElement(ele);
            while (ele != nullptr) {
                TiXmlElement* e = ele->FirstChildElement();
                while (e != nullptr) {
                    Global->InsertEndChild(*e);
                    e = e->NextSiblingElement();
                }
                ele = ele->NextSiblingElement("globals");
            }
        }

        // call the real loading method
        int result = LoadConfigFromElement(Sectional, Global, {});
        delete Sectional;
        delete rootEle;
        return result;
    } else {
        RESTError << "Filename: " << fConfigFileName << RESTendl;
        RESTError << "Config File does not exist. Right path/filename?" << RESTendl;
        GetChar();
        return -1;
    }

    // find the xml section corresponding to the sectionName
    TiXmlElement* sectional = GetElementFromFile(fConfigFileName, thisSectionName);
    if (sectional == nullptr) {
        RESTError << "cannot find xml section \"" << ClassName() << "\" with name \"" << thisSectionName
                  << "\"" << RESTendl;
        RESTError << "in config file: " << fConfigFileName << RESTendl;
        exit(1);
    }

    // find the "globals" section. Multiple sections are supported.
    TiXmlElement* rootEle = GetElementFromFile(fConfigFileName);
    TiXmlElement* global = GetElement("globals", rootEle);
    if (global != nullptr) ReadElement(global);
    if (global != nullptr && global->NextSiblingElement("globals") != nullptr) {
        TiXmlElement* ele = global->NextSiblingElement("globals");
        if (ele != nullptr) ReadElement(ele);
        while (ele != nullptr) {
            TiXmlElement* e = ele->FirstChildElement();
            while (e != nullptr) {
                global->InsertEndChild(*e);
                e = e->NextSiblingElement();
            }
            ele = ele->NextSiblingElement("globals");
        }
    }

    // call the real loading method
    int result = LoadConfigFromElement(sectional, global, {});
    delete sectional;
    delete rootEle;
    return result;
}

///////////////////////////////////////////////
/// \brief Main starter method.
///
/// First merge the sectional and global sections together, then save the input
/// env section. To make start up it calls the following methods in sequence:
/// LoadSectionMetadata(), InitFromConfigFile()
///
Int_t TRestMetadata::LoadConfigFromElement(TiXmlElement* eSectional, TiXmlElement* eGlobal,
                                           map<string, string> envs) {
    Initialize();
    TiXmlElement* theElement;
    if (eSectional != nullptr && eGlobal != nullptr) {
        // Sectional and global elements are first combined.
        theElement = (TiXmlElement*)eSectional->Clone();
        TiXmlElement* echild = eGlobal->FirstChildElement();
        while (echild != nullptr) {
            theElement->LinkEndChild(echild->Clone());
            echild = echild->NextSiblingElement();
        }
        // for (int i = 0; i < eEnv.size(); i++) {
        //    theElement->LinkEndChild(eEnv[i]->Clone());
        //}
    } else if (eSectional != nullptr) {
        theElement = (TiXmlElement*)eSectional->Clone();
    } else if (eGlobal != nullptr) {
        theElement = (TiXmlElement*)eGlobal->Clone();
    } else {
        return 0;
    }
    fElement = theElement;
    fElementGlobal = eGlobal ? (TiXmlElement*)eGlobal->Clone() : nullptr;
    fVariables = envs;

    int result = LoadSectionMetadata();
    if (result == 0) InitFromConfigFile();
    RESTDebug << ClassName() << " has finished preparing config data" << RESTendl;
    return result;
}

///////////////////////////////////////////////
/// \brief Initialize data from a string element buffer.
///
/// This method is usually called when the object is retrieved from root file. It will call
/// InitFromRootFile() after parsing configBuffer(string) to fElement(TiXmlElement)
Int_t TRestMetadata::LoadConfigFromBuffer() {
    if (configBuffer != "") {
        fElement = StringToElement(configBuffer);
        configBuffer = "";
        InitFromRootFile();
        return 0;
    }
    return -1;
}

///////////////////////////////////////////////
/// \brief This method will retrieve a new TRestMetadata instance of a child element
/// of the present TRestMetadata instance based on the `index` given by argument,
/// which defines the element order to be retrieved, 0 for first element found, 1 for
/// the second element found, etc.
///
/// In brief, it will create an instance of `TRestChildClass` in the following example:
///
/// \code
///    <TRestThisMetadataClass ...
///         <TRestChildClass ...> <!-- if index = 0 -->
///         <TRestChildClass ...> <!-- if index = 1 -->
///         <TRestChildClass ...> <!-- if index = 2 -->
/// \endcode
///
/// An optional argument may help to restrict the search to a particular metadata
/// element.
///
/// - *pattern*: If a pattern value is given, then the pattern must be contained inside
/// the metadata class name. I.e. pattern="TRestGeant4" will require that the class
/// belongs to the geant4 library.
///
/// Otherwise, the first child section that satisfies that it starts by `TRest` will be
/// considered.
///
/// If no child element is found with the required criteria, `nullptr` will be returned.
///
TRestMetadata* TRestMetadata::InstantiateChildMetadata(int index, std::string pattern) {
    int count = 0;
    auto paraele = fElement->FirstChildElement();
    while (paraele != nullptr) {
        std::string xmlChild = paraele->Value();
        if (xmlChild.find("TRest") == 0) {
            if (pattern == "" || xmlChild.find(pattern) != string::npos) {
                if (count == index) {
                    TClass* c = TClass::GetClass(xmlChild.c_str());
                    if (c)  // this means that the metadata class was found
                    {
                        TRestMetadata* md = (TRestMetadata*)c->New();
                        if (!md) return nullptr;
                        md->SetConfigFile(fConfigFileName);
                        TiXmlElement* rootEle = GetElementFromFile(fConfigFileName);
                        TiXmlElement* Global = GetElement("globals", rootEle);
                        md->LoadConfigFromElement(paraele, Global, {});
                        md->Initialize();
                        return md;
                    }
                }
                count++;
            }
        }
        paraele = paraele->NextSiblingElement();
    }
    return nullptr;
}

///////////////////////////////////////////////
/// \brief This method will retrieve a new TRestMetadata instance of a child element
/// of the present TRestMetadata instance based on the `name` given by argument.
///
/// In brief, it will create an instance of `TRestChildClass` in the following example:
///
/// \code
///    <TRestThisMetadataClass ...
///         <TRestChildClass ...>
/// \endcode
///
/// Two optional arguments may help to restrict the search to a particular metadata
/// class name and user given name.
///
/// - *pattern*: If a pattern value is given, then the pattern must be contained inside
/// the metadata class name. I.e. pattern="TRestGeant4" will require that the class
/// belongs to the geant4 library.
///
/// - *name*: It can be specified a specific given name.
///
/// Otherwise, the first child section that satisfies that it starts by `TRest` will be
/// returned.
///
/// If no child element is found with the required criteria, `nullptr` will be returned.
///
TRestMetadata* TRestMetadata::InstantiateChildMetadata(std::string pattern, std::string name) {
    auto paraele = fElement->FirstChildElement();
    while (paraele != nullptr) {
        std::string xmlChild = paraele->Value();
        if (xmlChild.find("TRest") == 0) {
            if (pattern.empty() || xmlChild.find(pattern) != string::npos) {
                if (name.empty() || (!name.empty() && name == (string)paraele->Attribute("name"))) {
                    TClass* c = TClass::GetClass(xmlChild.c_str());
                    if (c)  // this means we have the metadata class was found
                    {
                        TRestMetadata* md = (TRestMetadata*)c->New();
                        TiXmlElement* rootEle = GetElementFromFile(fConfigFileName);
                        TiXmlElement* Global = GetElement("globals", rootEle);
                        md->LoadConfigFromElement(paraele, Global, {});
                        md->Initialize();
                        return md;
                    }
                }
            }
        }
        paraele = paraele->NextSiblingElement();
    }
    return nullptr;
}

///////////////////////////////////////////////
/// \brief This method does some preparation of xml section.
///
/// Preparation includes: setting the name, title and verbose level of the
/// current class. Finding out and saving the env sections.
///
/// By calling TRestMetadata::ReadElement(), is also expands for loops and
/// include definitions, and replaces env and expressions in rml config section.
///
Int_t TRestMetadata::LoadSectionMetadata() {
    // get debug level
    string debugStr = GetParameter("verboseLevel", ToString(static_cast<int>(fVerboseLevel)));
    fVerboseLevel = StringToVerboseLevel(debugStr);

    RESTDebug << "Loading Config for : " << this->ClassName() << RESTendl;

    // set env first from global section
    if (fElementGlobal != nullptr) {
        TiXmlElement* e = fElementGlobal->FirstChildElement();
        while (e != nullptr) {
            ReplaceElementAttributes(e);
            ReadEnvInElement(e);
            e = e->NextSiblingElement();
        }
    }

    // then from local section
    TiXmlElement* e = fElement->FirstChildElement();
    while (e != nullptr) {
        ReplaceElementAttributes(e);
        ReadEnvInElement(e);
        e = e->NextSiblingElement();
    }

    // look through the elements and expand for, include, etc. structures
    ReadElement(fElement);

    // get debug level again in case it is defined in the included file
    debugStr = GetParameter("verboseLevel", ToString(static_cast<int>(fVerboseLevel)));
    fVerboseLevel = StringToVerboseLevel(debugStr);

    // fill the general metadata info: name, title, fstore
    this->SetName(GetParameter("name", "default" + string(this->ClassName())).c_str());
    this->SetTitle(GetParameter("title", "Default " + string(this->ClassName())).c_str());
    this->SetSectionName(this->ClassName());

    fStore = StringToBool(GetParameter("store", to_string(true)));

    return 0;
}

///////////////////////////////////////////////
/// \brief replace the field value(attribute) of the given xml element
///
/// it calls ReplaceVariables(), ReplaceConstants() and
/// ReplaceMathematicalExpressions() in sequence. "name" attribute won't be
/// replaced by constants to avoid conflict.
TiXmlElement* TRestMetadata::ReplaceElementAttributes(TiXmlElement* e) {
    if (e == nullptr) return nullptr;

    RESTDebug << "Entering ... TRestMetadata::ReplaceElementAttributes" << RESTendl;

    std::string parName = "";
    TiXmlAttribute* attr = e->FirstAttribute();
    while (attr != nullptr) {
        const char* val = attr->Value();
        const char* name = attr->Name();
        RESTDebug << "Element name : " << name << " value : " << val << RESTendl;

        string newVal = val != nullptr ? val : "";
        newVal = ReplaceVariables(newVal);

        // for name attribute, don't replace constants
        if (strcmp(name, "name") != 0) newVal = ReplaceConstants(newVal);

        e->SetAttribute(name, ReplaceMathematicalExpressions(newVal));

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
    if (e == nullptr) return;

    const char* name = e->Attribute("name");
    if (name == nullptr) return;
    const char* value = e->Attribute("value");
    if (value == nullptr) return;

    if ((string)e->Value() == "variable") {
        // if overwrite is false, try to replace the value from system env.
        const char* overwritesysenv = e->Attribute("overwrite");
        if (overwritesysenv == nullptr) overwritesysenv = "false";
        if (!StringToBool(overwritesysenv)) {
            char* sysenv = getenv(name);
            if (sysenv != nullptr) value = sysenv;
        }
        if (!overwrite && fVariables.count(name) > 0) return;
        fVariables[name] = value;
    } else if ((string)e->Value() == "constant") {
        if (!overwrite && fVariables.count(name) > 0) return;
        fConstants[name] = value;
    } else if ((string)e->Value() == "myParameter") {
        RESTWarning << "myParameter is obsolete now! use \"constant\" instead" << RESTendl;
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
    RESTDebug << ClassName() << "::ReadElement(<" << e->Value() << ")" << RESTendl;
    if (e == nullptr) return;

    ReplaceElementAttributes(e);
    ReadEnvInElement(e);

    if ((string)e->Value() == "for") {
        ExpandForLoops(e, {});
    } else if (e->Attribute("file") != nullptr) {
        ExpandIncludeFile(e);
    } else if ((string)e->Value() == "if") {
        ExpandIfSections(e);
    } else if (e->FirstChildElement() != nullptr) {
        TiXmlElement* contentelement = e->FirstChildElement();
        // we won't expand child TRestXXX sections unless forced recursive. The expansion of
        // these sections will be executed individually by the corresponding TRestXXX class
        while (contentelement != nullptr) {
            TiXmlElement* nxt = contentelement->NextSiblingElement();
            if (recursive || ((string)contentelement->Value()).find("TRest") == string::npos) {
                RESTDebug << "into child element \"" << contentelement->Value() << "\" of \"" << e->Value()
                          << "\"" << RESTendl;
                ReadElement(contentelement, recursive);
            } else {
                RESTDebug << "skipping child element \"" << contentelement->Value() << "\" of \""
                          << e->Value() << "\"" << RESTendl;
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
///        <addProcess type="TRestRawZeroSuppresionProcess" name="zS" value="ON" file="processes.rml"/>
///     </if>
///     <if evaluate="date +%Y-%m-%d" condition=">2019-08-21">
///        <addProcess type = "TRestDetectorSignalToHitsProcess" name = "signalToHits" value = "ON" file =
///        "processes.rml" />
///     </if>
/// </TRestXXX>
/// \endcode
/// "evaluate" specifies the shell command, the output of which is used.
/// "condition" specifies the comparing condition.
/// So here if the home directory is "/home/nkx", the process "TRestRawZeroSuppresionProcess" will be
/// added If the current date is larger than 2019-08-21, the process "TRestDetectorSignalToHitsProcess"
/// will be added
///
/// Supports condition markers: `==`, `!=`, `>`, `<`, `<=`, `>=`. Its better to escape the ">", "<"
/// markers. Note that the `>`, `<` calculation is also valid for strings. The ordering is according to
/// the alphabet
///
void TRestMetadata::ExpandIfSections(TiXmlElement* e) {
    if (e == nullptr) return;
    if ((string)e->Value() != "if") return;

    const char* evaluate = e->Attribute("evaluate");
    const char* condition = e->Attribute("condition");

    if (condition == nullptr || string(condition).find_first_of("=!<>") == string::npos) {
        RESTWarning << "Invalid \"IF\" structure!" << RESTendl;
        return;
    }

    int p1 = string(condition).find_first_of("=!<>");
    int p2 = string(condition).find_first_not_of("=!<>", p1);

    string v1 = "";
    bool matches = false;
    if (evaluate != nullptr) {
        v1 = TRestTools::Execute(evaluate);
    } else if (p1 > 0) {
        v1 = string(condition).substr(0, p1);
    } else {
        RESTWarning << "Invalid \"IF\" structure!" << RESTendl;
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
        RESTWarning << "Invalid \"IF\" structure!" << RESTendl;
        return;
    }

    if (matches) {
        TiXmlElement* parele = (TiXmlElement*)e->Parent();
        if (parele == nullptr) return;
        TiXmlElement* contentelement = e->FirstChildElement();
        while (contentelement != nullptr) {
            TiXmlElement* attachedelement = (TiXmlElement*)contentelement->Clone();
            ReadElement(attachedelement, true);
            // RESTDebug << *attachedelement << RESTendl;
            parele->InsertBeforeChild(e, *attachedelement);
            delete attachedelement;
            contentelement = contentelement->NextSiblingElement();
        }
    }
}

///////////////////////////////////////////////
/// \brief Helper method for TRestMetadata::ExpandForLoops().
void TRestMetadata::ExpandForLoopOnce(TiXmlElement* e, const map<string, string>& forLoopVar) {
    if (e == nullptr) {
        return;
    }

    TiXmlElement* parele = (TiXmlElement*)e->Parent();
    TiXmlElement* contentelement = e->FirstChildElement();
    while (contentelement != nullptr) {
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
            // RESTDebug << *attachedelement << RESTendl;
            parele->InsertBeforeChild(e, *attachedelement);
            delete attachedelement;
            contentelement = contentelement->NextSiblingElement();
        }
    }
}

///////////////////////////////////////////////
/// \brief Helper method for TRestMetadata::ExpandForLoops().
///
void TRestMetadata::ReplaceForLoopVars(TiXmlElement* e, map<string, string> forLoopVar) {
    if (e == nullptr) return;

    RESTDebug << "Entering ... TRestMetadata::ReplaceForLoopVars" << RESTendl;
    std::string parName;
    TiXmlAttribute* attr = e->FirstAttribute();
    while (attr != nullptr) {
        const char* val = attr->Value();
        const char* name = attr->Name();
        RESTDebug << "Attribute name : " << name << " value : " << val << RESTendl;

        if (strcmp(name, "name") == 0) parName = (string)val;

        // set attribute except name field
        if (strcmp(name, "name") != 0) {
            string outputBuffer = val;

            if (outputBuffer.find('[') != string::npos || outputBuffer.find(']') != string::npos) {
                RESTError << "TRestMetadata::ReplaceForLoopVars. Old for-loop construction identified"
                          << RESTendl;
                RESTError << "Please, replace [] variable nomenclature by ${}." << RESTendl;
                exit(1);
            }

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

                if (!proenv.empty()) {
                    outputBuffer.replace(replacePos, replaceLen, proenv);
                    endPosition = 0;
                } else {
                    RESTError << this->ClassName() << ", replace for loop env : cannot find \"{" << expression
                              << "}\"" << RESTendl;
                    exit(1);
                }
            }

            e->SetAttribute(name, ReplaceMathematicalExpressions(
                                      outputBuffer, 0,
                                      "Please, check parameter name: " + parName + " (ReplaceForLoopVars)")
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
    if (e == nullptr) return;
    if ((string)e->Value() != "for") return;
    RESTDebug << "Entering ... ExpandForLoops" << RESTendl;
    ReplaceElementAttributes(e);

    TString varname = TString(e->Attribute("variable"));
    TString varfrom = TString(e->Attribute("from"));
    TString varto = TString(e->Attribute("to"));
    TString varstep = TString(e->Attribute("step"));
    TString varin = TString(e->Attribute("in"));

    RESTDebug << "variable: " << varname << " from: " << varfrom << " to: " << varto << " step: " << varstep
              << " in: " << varin << RESTendl;

    if ((varin == "") && (varname == "" || varfrom == "" || varto == "")) return;
    if (varstep == "") varstep = "1";
    TiXmlElement* parele = (TiXmlElement*)e->Parent();
    if (parele == nullptr) return;

    string _name = (string)varname;
    string _from = (string)varfrom;
    string _to = (string)varto;
    string _step = (string)varstep;
    string _in = (string)varin;
    RESTDebug << "_from: " << _from << " _to: " << _to << " _step: " << _step << RESTendl;
    if (isANumber(_from) && isANumber(_to) && isANumber(_step)) {
        double from = StringToDouble(_from);
        double to = StringToDouble(_to);
        double step = StringToDouble(_step);

        RESTDebug << "----expanding for loop----" << RESTendl;
        double i = 0;
        for (i = from; i <= to; i = i + step) {
            forloopvar[_name] = ToString(i);
            fVariables[_name] = ToString(i);
            ExpandForLoopOnce(e, forloopvar);
        }
        parele->RemoveChild(e);

        if (fVerboseLevel >= TRestStringOutput::REST_Verbose_Level::REST_Extreme) parele->Print(stdout, 0);
        RESTDebug << "----end of for loop----" << RESTendl;
    } else if (!_in.empty()) {
        vector<string> loopvars = Split(_in, ":");

        RESTDebug << "----expanding for loop----" << RESTendl;
        for (const string& loopvar : loopvars) {
            forloopvar[_name] = loopvar;
            fVariables[_name] = loopvar;
            ExpandForLoopOnce(e, forloopvar);
        }
        parele->RemoveChild(e);

        if (fVerboseLevel >= TRestStringOutput::REST_Verbose_Level::REST_Extreme) parele->Print(stdout, 0);
        RESTDebug << "----end of for loop----" << RESTendl;
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
    RESTDebug << "Entering ... " << __PRETTY_FUNCTION__ << RESTendl;
    if (e == nullptr) return;

    ReplaceElementAttributes(e);
    const char* _filename = e->Attribute("file");
    if (_filename == nullptr) return;

    string filename;
    if (string(_filename) == "server" || TRestTools::isURL(_filename)) {
        // Let TRestRun to retrieve data according to run number later-on

        // match the database, runNumber=0(default data), type="META_RML", tag=<section name>
        auto url = gDataBase->query_data(DBEntry(0, "META_RML", e->Value())).value;
        if (url.empty()) {
            // don't really understand this "database" code, this just works
            url = _filename;
        }

        filename = TRestTools::DownloadRemoteFile(url);
    } else {
        filename = SearchFile(_filename);
    }

    if (filename.empty()) {
        RESTError << "TRestMetadata::ExpandIncludeFile. Include file \"" << _filename << "\" does not exist!"
                  << RESTendl;
        exit(1);
        return;
    }
    if (!TRestTools::isRootFile(filename))  // root file inclusion is implemented in TRestRun
    {
        RESTDebug << "----expanding include file----" << RESTendl;
        // we find the local element(the element to receive content)
        // and the remote element(the element to provide content)
        TiXmlElement* remoteele = nullptr;
        TiXmlElement* localele = nullptr;
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
            if (localele == nullptr) return;
            if (localele->Attribute("expanded") == nullptr
                    ? false
                    : ((string)localele->Attribute("expanded") == "true")) {
                RESTDebug << "----already expanded----" << RESTendl;
                return;
            }

            remoteele = new TiXmlElement("Config");

            TiXmlElement* ele = GetElementFromFile(filename);
            if (ele == nullptr) {
                RESTError << "TRestMetadata::ExpandIncludeFile. No xml elements contained in the include "
                             "file \""
                          << filename << "\"" << RESTendl;
                exit(1);
            }
            while (ele != nullptr) {
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
            if (localele->Attribute("expanded") == nullptr
                    ? false
                    : ((string)localele->Attribute("expanded") == "true")) {
                RESTDebug << "----already expanded----" << RESTendl;
                return;
            }

            type = e->Attribute("type") != nullptr ? e->Attribute("type") : e->Value();
            name = localele->Attribute("name") == nullptr ? "" : localele->Attribute("name");

            // get the root element
            TiXmlElement* rootele = GetElementFromFile(filename);
            if (rootele == nullptr) {
                RESTError << "TRestMetaddata::ExpandIncludeFile. Include file " << filename
                          << " is of wrong xml format!" << RESTendl;
                exit(1);
                return;
            }
            if ((string)rootele->Value() == type) {
                // if root element in the included file is of given type, directly use
                // it
                remoteele = rootele;
            } else {
                // import env first
                if (type != "globals" && GetElement("globals", rootele) != nullptr) {
                    TiXmlElement* globaldef = GetElement("globals", rootele)->FirstChildElement();
                    while (globaldef != nullptr) {
                        ReadEnvInElement(globaldef, false);
                        globaldef = globaldef->NextSiblingElement();
                    }
                }

                // find its child section according to type and name
                if (name != "") {
                    // we find only according to the name
                    vector<TiXmlElement*> eles;
                    TiXmlElement* ele = rootele->FirstChildElement();
                    while (ele != nullptr) {
                        if (ele->Attribute("name") != nullptr && (string)ele->Attribute("name") == name) {
                            eles.push_back(ele);
                        }
                        ele = ele->NextSiblingElement();
                    }
                    // more than 1 element found
                    if (eles.size() > 1) {
                        RESTWarning << "(expand include file): find multiple xml sections with same name!"
                                    << RESTendl;
                        RESTWarning << "Using the first one!" << RESTendl;
                    }

                    if (eles.size() > 0) remoteele = (TiXmlElement*)eles[0]->Clone();
                } else if (type != "") {
                    remoteele = (TiXmlElement*)GetElement(type, rootele)->Clone();
                }

                if (remoteele == nullptr) {
                    RESTWarning << "Cannot find the needed xml section in "
                                   "include file!"
                                << RESTendl;
                    RESTWarning << "type: \"" << type << "\" , name: \"" << name << "\" . Skipping"
                                << RESTendl;
                    RESTWarning << RESTendl;
                    return;
                }
                delete rootele;
            }
        }

        RESTDebug << "Target xml element spotted" << RESTendl;

        ///////////////////////////////////////
        // begin inserting remote element into local element
        ReadElement(remoteele, true);
        int nattr = 0;
        int nele = 0;
        TiXmlAttribute* attr = remoteele->FirstAttribute();
        while (attr != nullptr) {
            if (localele->Attribute(attr->Name()) == nullptr) {
                localele->SetAttribute(attr->Name(), attr->Value());
                nattr++;
            }
            attr = attr->Next();
        }
        TiXmlElement* ele = remoteele->FirstChildElement();
        while (ele != nullptr) {
            // ReadElement(ele);
            if ((string)ele->Value() != "for") {
                localele->InsertEndChild(*ele);
                nele++;
            }
            ele = ele->NextSiblingElement();
        }

        localele->SetAttribute("expanded", "true");
        if (fVerboseLevel >= TRestStringOutput::REST_Verbose_Level::REST_Debug) {
            localele->Print(stdout, 0);
            cout << endl;
        }
        delete remoteele;
        RESTDebug << nattr << " attributes and " << nele << " xml elements added by inclusion" << RESTendl;
        RESTDebug << "----end of expansion file----" << RESTendl;
    }
}

///////////////////////////////////////////////
/// \brief Returns corresponding REST Metadata parameter from multiple sources
///
/// It first finds the parameter from REST arguments from command line.
/// If not found, it calls GetParameter() method to find parameter in TRestMetadata::fElement
/// If still not found, it returns the default value.
///
/// \param parName The name of the parameter from which we want to obtain the
/// value. \param defaultValue The default value if the parameter is not found
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
/// parameter \param defaultValue The default value if the parameter is not
/// found
///
/// \return A string of result, with env and expressions replaced
string TRestMetadata::GetParameter(std::string parName, TiXmlElement* e, TString defaultValue) {
    if (e == nullptr) {
        RESTDebug << "Element is null" << RESTendl;
        return (string)defaultValue;
    }
    string result = (string)defaultValue;
    // first find in attribute
    if (e->Attribute(parName.c_str()) != nullptr) {
        result = e->Attribute(parName.c_str());
    }
    // then find in child sections/elements
    else {
        TiXmlElement* element = GetElementWithName("parameter", parName, e);
        if (element != nullptr && element->Attribute("value") != nullptr) {
            result = element->Attribute("value");
        } else {
            RESTDebug << ClassName() << ": Parameter : " << parName << " not found!" << RESTendl;
        }
    }

    return ReplaceMathematicalExpressions(ReplaceConstants(ReplaceVariables(result)), 0,
                                          "Please, check parameter name: " + parName);
}

Double_t TRestMetadata::GetDblParameterWithUnits(std::string parName, TiXmlElement* ele,
                                                 Double_t defaultVal) {
    if (ele == nullptr) return defaultVal;
    pair<string, string> val_unit = GetParameterAndUnits(parName, ele);
    string val = val_unit.first;
    string unit = val_unit.second;
    if (val == PARAMETER_NOT_FOUND_STR) {
        return defaultVal;
    } else {
        Double_t value = StringToDouble(val);
        return REST_Units::ConvertValueToRESTUnits(value, unit);
    }

    return defaultVal;
}

TVector2 TRestMetadata::Get2DVectorParameterWithUnits(std::string parName, TiXmlElement* ele,
                                                      TVector2 defaultVal) {
    if (ele == nullptr) return defaultVal;
    pair<string, string> val_unit = GetParameterAndUnits(parName, ele);
    string val = val_unit.first;
    string unit = val_unit.second;
    if (val == PARAMETER_NOT_FOUND_STR) {
        return defaultVal;
    } else {
        TVector2 value = StringTo2DVector(val);
        Double_t valueX = REST_Units::ConvertValueToRESTUnits(value.X(), unit);
        Double_t valueY = REST_Units::ConvertValueToRESTUnits(value.Y(), unit);
        return TVector2(valueX, valueY);
    }

    return defaultVal;
}

TVector3 TRestMetadata::Get3DVectorParameterWithUnits(std::string parName, TiXmlElement* ele,
                                                      TVector3 defaultVal) {
    if (ele == nullptr) return defaultVal;
    pair<string, string> val_unit = GetParameterAndUnits(parName, ele);
    string val = val_unit.first;
    string unit = val_unit.second;
    if (val == PARAMETER_NOT_FOUND_STR) {
        return defaultVal;
    } else {
        TVector3 value = StringTo3DVector(val);
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
/// element. If not found, the returned string is "Not defined"
///
std::string TRestMetadata::GetFieldValue(std::string parName, TiXmlElement* e) {
    if (e == nullptr) {
        RESTDebug << "Element is null" << RESTendl;
        return "Not defined";
    }
    const char* val = e->Attribute(parName.c_str());
    if (val == nullptr) {
        return "Not defined";
    }

    string result = (string)val;
    result = Replace(result, " AND ", " && ");
    result = Replace(result, " OR ", " || ");

    return result;
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
    pair<string, string> val_unit = GetParameterAndUnits(parName);
    string val = val_unit.first;
    string unit = val_unit.second;
    if (val == PARAMETER_NOT_FOUND_STR) {
        return defaultVal;
    } else {
        Double_t value = StringToDouble(val);
        return REST_Units::ConvertValueToRESTUnits(value, unit);
    }
    return defaultVal;
}

TVector2 TRestMetadata::Get2DVectorParameterWithUnits(std::string parName, TVector2 defaultVal) {
    pair<string, string> val_unit = GetParameterAndUnits(parName);
    string val = val_unit.first;
    string unit = val_unit.second;
    if (val == PARAMETER_NOT_FOUND_STR) {
        return defaultVal;
    } else {
        TVector2 value = StringTo2DVector(val);
        Double_t valueX = REST_Units::ConvertValueToRESTUnits(value.X(), unit);
        Double_t valueY = REST_Units::ConvertValueToRESTUnits(value.Y(), unit);
        return TVector2(valueX, valueY);
    }
    return defaultVal;
}

TVector3 TRestMetadata::Get3DVectorParameterWithUnits(std::string parName, TVector3 defaultVal) {
    pair<string, string> val_unit = GetParameterAndUnits(parName);
    string val = val_unit.first;
    string unit = val_unit.second;
    if (val == PARAMETER_NOT_FOUND_STR) {
        return defaultVal;
    } else {
        TVector3 value = StringTo3DVector(val);
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
TiXmlElement* TRestMetadata::GetElementFromFile(std::string configFilename, std::string NameOrDeclare) {
    TiXmlDocument doc;
    TiXmlElement* rootele;

    string filename = configFilename;
    if (TRestMetadata_UpdatedConfigFile.count(filename) > 0)
        filename = TRestMetadata_UpdatedConfigFile[filename];

    if (!TRestTools::fileExists(filename)) {
        RESTError << "Config file does not exist. The file is: " << filename << RESTendl;
        exit(1);
    }

    if (!doc.LoadFile(filename.c_str())) {
        RESTError << "Failed to load xml file, syntax maybe wrong. The file is: " << filename << RESTendl;
        exit(1);
    }

    rootele = doc.RootElement();
    if (rootele == nullptr) {
        RESTError << "The rml file \"" << configFilename << "\" does not contain any valid elements!"
                  << RESTendl;
        exit(1);
    }
    if (NameOrDeclare == "") {
        return (TiXmlElement*)rootele->Clone();
    }
    // search with either name or declare in either root element or sub-root
    // element
    while (rootele != nullptr) {
        if (rootele->Value() != nullptr && (string)rootele->Value() == NameOrDeclare) {
            return (TiXmlElement*)rootele->Clone();
        }

        if (rootele->Attribute("name") != nullptr && (string)rootele->Attribute("name") == NameOrDeclare) {
            return (TiXmlElement*)rootele->Clone();
        }

        TiXmlElement* etemp = GetElement(NameOrDeclare, rootele);
        if (etemp != nullptr) {
            return (TiXmlElement*)etemp->Clone();
        }

        etemp = GetElementWithName("", NameOrDeclare, rootele);

        if (etemp != nullptr) {
            return (TiXmlElement*)etemp->Clone();
        }

        rootele = rootele->NextSiblingElement();
    }

    return nullptr;
    /*ferr << "Cannot find xml element with name \""<< NameOrDeclare <<"\" in rml
    file \"" << configFilename << endl; GetChar(); exit(1);*/
}

///////////////////////////////////////////////
/// \brief Get an xml element from a given parent element, according to its
/// declaration
///
TiXmlElement* TRestMetadata::GetElement(std::string eleDeclare, TiXmlElement* e) {
    if (e == nullptr) e = fElement;
    return e->FirstChildElement(eleDeclare.c_str());
}

///////////////////////////////////////////////
/// \brief Get the next sibling xml element of this element, with same eleDeclare
///
TiXmlElement* TRestMetadata::GetNextElement(TiXmlElement* e) {
    if (e == nullptr) return nullptr;
    return e->NextSiblingElement(e->Value());
}

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
    if (e == nullptr) return nullptr;
    if (eleDeclare == "")  // find only with name
    {
        TiXmlElement* ele = e->FirstChildElement();
        while (ele != nullptr) {
            if (ele->Attribute("name") != nullptr) {
                std::string nameValue = (string)ele->Attribute("name");
                nameValue = ReplaceVariables(nameValue);
                if (nameValue == eleName) {
                    return ele;
                }
            }
            ele = ele->NextSiblingElement();
        }
        return ele;
    } else  // find with name and declare
    {
        TiXmlElement* ele = e->FirstChildElement(eleDeclare.c_str());
        while (ele != nullptr) {
            if (ele->Attribute("name") != nullptr) {
                std::string nameValue = (string)ele->Attribute("name");
                nameValue = ReplaceVariables(nameValue);
                if (nameValue == eleName) {
                    return ele;
                }
            }
            ele = ele->NextSiblingElement(eleDeclare.c_str());
        }
        return ele;
    }

    return nullptr;
}

///////////////////////////////////////////////
/// \brief Returns a string with the unit name given in the given xml element
///
/// if given the target attribute, it will find the unit for this.
/// e.g. value="(1,-13)mm"  "-3mm"  "50,units=mm"  can both be recoginzed.
/// if not given, it will find the unit as a parameter of the element.
///	e.g. <... value="3" units="mm" .../>
string TRestMetadata::GetUnits(TiXmlElement* e) {
    if (e == nullptr) {
        RESTWarning << "TRestMetadata::GetUnits(): NULL element given!" << RESTendl;
        return "";
    }

    string valstr = e->Attribute("value") == nullptr ? "" : e->Attribute("value");
    string unitattr = e->Attribute("units") == nullptr ? "" : e->Attribute("units");

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
pair<string, string> TRestMetadata::GetParameterAndUnits(string parName, TiXmlElement* e) {
    string parvalue;
    if (e == nullptr) {
        parvalue = GetParameter(parName);
        e = fElement;
    } else {
        parvalue = GetParameter(parName, e);
    }

    if (parvalue == PARAMETER_NOT_FOUND_STR) {
        return {parvalue, ""};
    } else {
        // first try to use unit embeded in parvalue
        string unit = REST_Units::FindRESTUnitsInString(parvalue);
        // then try to find unit in corresponding "parameter" section
        if (unit == "") {
            TiXmlElement* paraele = GetElementWithName("parameter", parName, e);
            if (paraele != nullptr) {
                unit = GetUnits(paraele);
            }
        }
        // finally try to find unit in local section attribute
        if (unit == "") {
            unit = GetUnits(e);
        }
        return {REST_Units::RemoveUnitsFromString(parvalue), unit};
    }
    return {parvalue, ""};
}

///////////////////////////////////////////////
/// \brief Parsing a string into TiXmlElement object
///
/// This method creates TiXmlElement object with the alloator "new".
/// Be advised to delete the object after using it!
TiXmlElement* TRestMetadata::StringToElement(string definition) {
    TiXmlElement* ele = new TiXmlElement("temp");
    // TiXmlDocument*doc = new TiXmlDocument();
    ele->Parse(definition.c_str(), nullptr, TIXML_ENCODING_UTF8);
    return ele;
}

///////////////////////////////////////////////
/// \brief Convert an TiXmlElement object to string
///
/// This method does't arrange the output. All the contents are written in one
/// line.
string TRestMetadata::ElementToString(TiXmlElement* ele) {
    if (ele != nullptr) {
        // remove comments
        TiXmlNode* n = ele->FirstChild();
        while (n != nullptr) {
            TiXmlComment* cmt = n->ToComment();
            if (cmt != nullptr) {
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
    RESTDebug << "Finding " << fromPosition << "th appearance of KEY Structure \"" << keyName << "\"..."
              << RESTendl;

    TiXmlElement* childele = ele->FirstChildElement(keyName);
    for (unsigned int i = 0; childele != nullptr && i < fromPosition; i++) {
        childele = childele->NextSiblingElement(keyName);
    }
    if (childele != nullptr) {
        string result = ElementToString(childele);
        fromPosition = fromPosition + 1;
        RESTDebug << "Found Key : " << result << RESTendl;
        return result;
    }

    RESTDebug << "Finding hit the end, KEY Structure not found!!" << RESTendl;
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
    if (e == nullptr) return nullptr;

    TiXmlAttribute* attr = e->FirstAttribute();
    while (attr != nullptr) {
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
/// \brief Identifies environmental variable replacing marks in the input buffer,
/// and replace them with corresponding value.
///
/// Replacing marks is like ${VARIABLE_NAME}. "variables" include system env, values
/// added through <variable section, and rest command line arguments(--c option).
/// The replacing sequence is same. i.e. try to replace with system env first, if not
/// found, try to replace <variable section, if still not found, try to replace
/// with command line arguments. If all not found, return the initial value.
string TRestMetadata::ReplaceVariables(const string buffer) {
    RESTDebug << "Entering ... TRestMetadata::ReplaceVariables (" << buffer << ")" << RESTendl;
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

        string sysenv = getenv(expression.c_str()) != nullptr ? getenv(expression.c_str()) : "";
        string proenv = fVariables.count(expression) > 0 ? fVariables[expression] : "";
        string argenv = REST_ARGS.count(expression) > 0 ? REST_ARGS[expression] : "";

        if (sysenv != "") {
            outputBuffer.replace(replacePos, replaceLen, sysenv);
            endPosition = 0;
        } else if (argenv != "") {
            outputBuffer.replace(replacePos, replaceLen, argenv);
            endPosition = 0;
        } else if (proenv != "") {
            outputBuffer.replace(replacePos, replaceLen, proenv);
            endPosition = 0;
        } else {
            RESTError << this->ClassName() << ", replace env : cannot find \"${" << expression
                      << "}\" in either system or program env, exiting..." << RESTendl;
            exit(1);
        }
    }

    if (buffer != outputBuffer) RESTDebug << "Replaced by : " << outputBuffer << RESTendl;
    return outputBuffer;
}

///////////////////////////////////////////////
/// \brief Identifies "constants" in the input buffer, and replace them with corresponding value.
///
/// Constans are the substrings directly appeared in the buffer
string TRestMetadata::ReplaceConstants(const string buffer) {
    RESTDebug << "Entering ... TRestMetadata::ReplaceConstants (" << buffer << ")" << RESTendl;
    string outputBuffer = buffer;

    // replace bare constant name. ignore sub strings.
    // e.g. variable "nCh" with value "3" cannot replace the string "nChannels+1"
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

    if (buffer != outputBuffer) RESTDebug << "Replaced by : " << outputBuffer << RESTendl;
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
        auto pathString = GetSearchPath();
        auto paths = Split((string)pathString, ":");
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
    if (fElement != nullptr) {
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
    if (fElement != nullptr) {
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

    RESTError << "Something missing here. Call the police" << RESTendl;
}

void TRestMetadata::PrintMessageBuffer() { cout << messageBuffer << endl; }

///////////////////////////////////////////////
/// \brief Prints metadata content on screen. Usually overloaded by the derived
/// metadata class.
///
void TRestMetadata::PrintMetadata() {
    RESTMetadata << "+++++++++++++++++++++++++++++++++++++++++++++" << RESTendl;
    RESTMetadata << this->ClassName() << " content" << RESTendl;
    RESTMetadata << "Config file : " << fConfigFileName << RESTendl;
    RESTMetadata << "+++++++++++++++++++++++++++++++++++++++++++++" << RESTendl;
    RESTMetadata << "Name : " << GetName() << RESTendl;
    RESTMetadata << "Title : " << GetTitle() << RESTendl;
    RESTMetadata << "REST Version : " << GetVersion() << RESTendl;
    if (fOfficialRelease)
        RESTMetadata << "REST Official release: Yes" << RESTendl;
    else
        RESTMetadata << "REST Official release: No" << RESTendl;
    if (fCleanState)
        RESTMetadata << "Clean state: Yes" << RESTendl;
    else
        RESTMetadata << "Clean state: No" << RESTendl;
    RESTMetadata << "REST Commit : " << GetCommit() << RESTendl;
    if (GetLibraryVersion() != "0.0")
        RESTMetadata << "REST Library version : " << GetLibraryVersion() << RESTendl;
    RESTMetadata << "---------------------------------------" << RESTendl;
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
        RESTError << "version is a static value, you cannot set version "
                     "for a class!"
                  << RESTendl;
    else {
        fVersion = REST_RELEASE;
    }
}

///////////////////////////////////////////////
/// \brief Resets the version of TRestRun to -1, in case the file is old REST file.
/// Only TRestRun is allowed to update version.
void TRestMetadata::UnSetVersion() {
    if (!this->InheritsFrom("TRestRun"))
        RESTError << "version is a static value, you cannot set version "
                     "for a class!"
                  << RESTendl;
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
    if (a != string::npos) return fSectionName.substr(0, a);
    return fSectionName;
}

///////////////////////////////////////////////
/// \brief Returns the config section of this class
std::string TRestMetadata::GetConfigBuffer() { return configBuffer; }

///////////////////////////////////////////////
/// \brief Get the value of data member as string.
///
/// All kinds of data member can be found, including non-streamed
/// data member and base-class data member
string TRestMetadata::GetDataMemberValue(string memberName) {
    return RESTValue(this, this->ClassName()).GetDataMemberValueString(memberName);
}

///////////////////////////////////////////////
/// \brief Get the value of datamember as a vector of strings.
///
/// If the datamember specified in the argument is a vector with several
/// elements, those elements will be assigned to the std::vector. If the
/// argument requests a data member that is not a vector in nature, this
/// method will still return a valid vector string with a single element.
///
/// All kinds of data member can be found, including non-streamed
/// data member and base-class data member
///
/// If precision value is higher than 0, then the resulting values will be
/// truncated after finding ".". This can be used to define a float precision.
///
std::vector<string> TRestMetadata::GetDataMemberValues(string memberName, Int_t precision) {
    string result = GetDataMemberValue(memberName);

    result = Replace(result, "{", "");
    result = Replace(result, "}", "");
    result = Replace(result, "(", "");
    result = Replace(result, ")", "");

    std::vector<std::string> results = REST_StringHelper::Split(result, ",");

    for (auto& x : results) x = REST_StringHelper::CropWithPrecision(x, precision);

    return results;
}

///////////////////////////////////////////////
/// \brief Returns a string corresponding to current verbose level.
///
TString TRestMetadata::GetVerboseLevelString() {
    TString level = "unknown";
    if (this->GetVerboseLevel() == TRestStringOutput::REST_Verbose_Level::REST_Debug) level = "debug";
    if (this->GetVerboseLevel() == TRestStringOutput::REST_Verbose_Level::REST_Info) level = "info";
    if (this->GetVerboseLevel() == TRestStringOutput::REST_Verbose_Level::REST_Essential) level = "warning";
    if (this->GetVerboseLevel() == TRestStringOutput::REST_Verbose_Level::REST_Silent) level = "silent";

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
///
TString TRestMetadata::GetSearchPath() {
    string result = "";

    // If fElement=0 we haven't initialized the class from RML.
    // Then we skip adding user paths
    if (fElement) {
        TiXmlElement* ele = fElement->FirstChildElement("searchPath");
        while (ele != nullptr) {
            if (ele->Attribute("value") != nullptr) {
                result += (string)ele->Attribute("value") + ":";
            }
            ele = ele->NextSiblingElement("searchPath");
        }
    }

    if (getenv("configPath")) result += getenv("configPath") + (string) ":";
    result += REST_PATH + "/data/:";
    result += REST_PATH + "/examples/:";
    // We give priority to the official /data/ and /examples/ path.
    result += REST_USER_PATH + ":";
    if (result.back() == ':') result.erase(result.size() - 1);

    return ReplaceConstants(ReplaceVariables(result));
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
/// It will loop over all the parameters in the rml and gDetector. (The repeated one
/// won't override the existing one. rml parameters in prior.) Then it will find the
/// corresponding datamember for the parameter. If found, it will set the datamember value.
/// For example, we write:
/// \code
/// class TRestXXX: public TRestMetadata{
/// int fPar0;
/// }
///
/// <TRestXXX name="..." par0="10"/>
/// \endcode
/// After loading the rml file and calling this method, the value of "fPar0" will
/// be set to 10.
///
/// We have a naming convention for the parameters in rml and the data members in class.
/// The names of data member shall all start from "f" and have the second character in
/// capital form. For example, data member "fTargetName" is linked to parameter "targetName".
/// In the previous code "fPar0" is linked to "par0".
///
/// Note that parameters include <parameter section and all the attributes in fElement.
///
void TRestMetadata::ReadAllParameters() {
    // we shall first add all the parameters to a temporary map to avoid
    // first parameter being overriden by the repeated parameter section
    map<string, string> parameters = GetParametersList();

    ReadParametersList(parameters);
}

///////////////////////////////////////////////
/// \brief It reads a parameter list and associates it to its corresponding metadata
/// member. par0 --> fPar0.
///
void TRestMetadata::ReadParametersList(std::map<string, string>& list) {
    for (auto i : list) {
        ReadOneParameter(i.first, i.second);
    }
}

///////////////////////////////////////////////
/// \brief It retrieves a map of all parameter:value found in the metadata class
///
std::map<string, string> TRestMetadata::GetParametersList() {
    // we shall first add all the parameters to a temporary map to avoid
    // first parameter being overriden by the repeated parameter section
    map<string, string> parameters;

    // Loop over REST_ARGS
    auto iter = REST_ARGS.begin();
    while (iter != REST_ARGS.end()) {
        // if (parameters.count(iter->first) == 0) {
        parameters[iter->first] = iter->second;
        //}
        iter++;
    }

    // Loop over attribute set
    auto paraattr = fElement->FirstAttribute();
    while (paraattr != nullptr) {
        string name = paraattr->Name();
        string value = paraattr->Value();

        if (parameters.count(name) == 0) {
            parameters[name] = value;
        }
        // ReadOneParameter((string)name, (string)value);
        paraattr = paraattr->Next();
    }

    // Loop over <parameter section
    auto paraele = fElement->FirstChildElement("parameter");
    while (paraele != nullptr) {
        string name = paraele->Attribute("name");
        string value = paraele->Attribute("value");
        // In case <parameter section contains units definitions in extra attribute field,
        // not together with "value" attribute field
        // We should concat the value and units together.
        TString units = paraele->Attribute("units");

        if (name == "") {
            RESTWarning << "bad <parameter section: " << *paraele << RESTendl;
        } else {
            if (parameters.count(name) == 0) {
                parameters[name] = value + units;
            }
            // ReadOneParameter((string)name, (string)value);
        }
        paraele = paraele->NextSiblingElement("parameter");
    }

    return parameters;
}

void TRestMetadata::ReadOneParameter(string name, string value) {
    if (name == "name" || name == "title" || name == "verboseLevel" || name == "type" || name == "value" ||
        name == "store") {
        // we omit these parameters since they are already loaded in LoadSectionMetadata()
    } else {
        RESTValue thisactual(this, this->ClassName());
        string datamembername = ParameterNameToDataMemberName(name);
        if (datamembername != "") {
            RESTValue datamember = thisactual.GetDataMember(datamembername);
            if (!datamember.IsZombie()) {
                RESTDebug << this->ClassName() << "::ReadAllParameters(): parsing value \"" << value
                          << "\" to data member \"" << datamembername << "\"" << RESTendl;

                if (REST_Units::FindRESTUnitsInString(value) != "") {
                    // there is units contained in this parameter.
                    string val = REST_Units::RemoveUnitsFromString(value);
                    string unit = REST_Units::FindRESTUnitsInString(value);

                    if (datamember.type == "double") {
                        Double_t value = StringToDouble(val);
                        *(double*)datamember = REST_Units::ConvertValueToRESTUnits(value, unit);
                    } else if (datamember.type == "TVector2") {
                        TVector2 value = StringTo2DVector(val);
                        Double_t valueX = REST_Units::ConvertValueToRESTUnits(value.X(), unit);
                        Double_t valueY = REST_Units::ConvertValueToRESTUnits(value.Y(), unit);
                        *(TVector2*)datamember = TVector2(valueX, valueY);
                    } else if (datamember.type == "TVector3") {
                        TVector3 value = StringTo3DVector(val);
                        Double_t valueX = REST_Units::ConvertValueToRESTUnits(value.X(), unit);
                        Double_t valueY = REST_Units::ConvertValueToRESTUnits(value.Y(), unit);
                        Double_t valueZ = REST_Units::ConvertValueToRESTUnits(value.Z(), unit);
                        *(TVector3*)datamember = TVector3(valueX, valueY, valueZ);
                    } else {
                        RESTWarning << this->ClassName() << " find unit definition in parameter: " << name
                                    << ", but the corresponding data member doesn't support it. Data "
                                       "member type: "
                                    << datamember.type << RESTendl;
                        datamember.ParseString(value);
                    }
                } else {
                    datamember.ParseString(value);
                }
            } else if (datamember.name != "") {
                // this mean the datamember is found with type not recognized.
                // We won't try to find the misspelling
            } else {
                RESTDebug << this->ClassName() << "::ReadAllParameters(): parameter \"" << name
                          << "\" not recognized for automatic load" << RESTendl;
                vector<string> availableparameters;

                vector<string> datamembers = thisactual.GetListOfDataMembers();
                for (unsigned int i = 0; i < datamembers.size(); i++) {
                    string parameter = DataMemberNameToParameterName(datamembers[i]);
                    if (parameter != "") {
                        if (parameter == "name" || parameter == "title" || parameter == "verboseLevel" ||
                            parameter == "type" || parameter == "value" || parameter == "store") {
                        } else {
                            availableparameters.push_back(parameter);
                        }
                    }
                }

                int mindiff = 100;
                string hintParameter = "";
                for (auto parameter : availableparameters) {
                    int diff = DiffString(name, parameter);
                    if (diff < mindiff) {
                        mindiff = diff;
                        hintParameter = parameter;
                    }
                }

                // we regard the unset parameter with less than 2 characters different from
                // the data member as "misspelling" parameter. We prompt a warning for it.
                if (hintParameter != "" && mindiff <= 2) {
                    RESTWarning << this->ClassName() << "::ReadAllParameters(): parameter \"" << name
                                << "\" not recognized for automatic load, did you mean \"" << hintParameter
                                << "\" ?" << RESTendl;
                    GetChar();
                }
            }
        }
    }
}

TRestStringOutput& TRestStringOutput::operator<<(endl_t et) {
    if (et.TRestMetadataPtr->GetVerboseLevel() <= TRestStringOutput::REST_Verbose_Level::REST_Info) {
        et.TRestMetadataPtr->AddLog(this->buf.str());
    }

    if (this->iserror) {
        if (this->verbose == TRestStringOutput::REST_Verbose_Level::REST_Warning) {
            et.TRestMetadataPtr->SetWarning(this->buf.str(), false);
        }
        if (this->verbose == TRestStringOutput::REST_Verbose_Level::REST_Silent) {
            et.TRestMetadataPtr->SetError(this->buf.str(), false);
        }
    }

    if (et.TRestMetadataPtr->GetVerboseLevel() >= this->verbose) {
        this->flushstring();
    } else {
        this->resetstring();
    }

    return *this;
}

void TRestMetadata::AddLog(string log, bool print) {
    messageBuffer += log + "\n";
    if (messageBuffer.size() > 1000) {
        messageBuffer.erase(0, messageBuffer.size() - 1000);
    }
}

void TRestMetadata::SetError(string message, bool print, int maxPrint) {
    fError = true;
    fNErrors++;
    if (message != "") {
        fErrorMessage += message + "\n";
        if (print && fNErrors < maxPrint) {
            cout << message << endl;
        }
    }
}

void TRestMetadata::SetWarning(string message, bool print, int maxPrint) {
    fWarning = true;
    fNWarnings++;
    if (message != "") {
        fWarningMessage += message + "\n";
        if (print && fNWarnings < maxPrint) {
            RESTWarning << message << RESTendl;
        }
    }
}

TString TRestMetadata::GetErrorMessage() {
    if (GetError())
        return fErrorMessage;
    else
        return "No error!";
}

TString TRestMetadata::GetWarningMessage() {
    if (GetWarning())
        return fWarningMessage;
    else
        return "No warning!";
}

void TRestMetadata::Merge(const TRestMetadata& metadata) {
    if (!metadata.InheritsFrom(ClassName())) {
        RESTError << "TRestMetadata::Merge. Metadata is not of type " << ClassName() << RESTendl;
        exit(1);
    }

    if (fName.IsNull()) {
        fName = metadata.GetName();
    }
}
