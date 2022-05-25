
#include <TRestReflector.h>
#include <TRestStringHelper.h>
#include <TString.h>

#include <iostream>

#ifndef RESTTask_MakeMetadata
#define RESTTask_MakeMetadata

//*******************************************************************************************************
//***
//*** This macro will generate a source .cxx and a header file serving as a template for new
//*** metadata classes. Please, replace any REMOVE comment with useful information.
//***
//*******************************************************************************************************
Int_t REST_MakeMetadata(TString name) {
    if (name.First("TRest") != 0) {
        RESTError << "invalid process name! REST process name must be start with \"TRest\" " << RESTendl;
        return -1;
    }

    ofstream headerFile(name + ".h");
    if (headerFile.fail()) {
        RESTError << "failed to create file!" << RESTendl;
        return -1;
    }

    headerFile << "/*************************************************************************" << endl;
    headerFile << " * This file is part of the REST software framework.                     *" << endl;
    headerFile << " *                                                                       *" << endl;
    headerFile << " * Copyright (C) 2016 GIFNA/TREX (University of Zaragoza)                *" << endl;
    headerFile << " * For more information see https://gifna.unizar.es/trex                 *" << endl;
    headerFile << " *                                                                       *" << endl;
    headerFile << " * REST is free software: you can redistribute it and/or modify          *" << endl;
    headerFile << " * it under the terms of the GNU General Public License as published by  *" << endl;
    headerFile << " * the Free Software Foundation, either version 3 of the License, or     *" << endl;
    headerFile << " * (at your option) any later version.                                   *" << endl;
    headerFile << " *                                                                       *" << endl;
    headerFile << " * REST is distributed in the hope that it will be useful,               *" << endl;
    headerFile << " * but WITHOUT ANY WARRANTY; without even the implied warranty of        *" << endl;
    headerFile << " * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the          *" << endl;
    headerFile << " * GNU General Public License for more details.                          *" << endl;
    headerFile << " *                                                                       *" << endl;
    headerFile << " * You should have a copy of the GNU General Public License along with   *" << endl;
    headerFile << " * REST in $REST_PATH/LICENSE.                                           *" << endl;
    headerFile << " * If not, see https://www.gnu.org/licenses/.                            *" << endl;
    headerFile << " * For the list of contributors see $REST_PATH/CREDITS.                  *" << endl;
    headerFile << " *************************************************************************/" << endl;
    headerFile << endl;
    headerFile << "#ifndef REST_" << name << endl;
    headerFile << "#define REST_" << name << endl;
    headerFile << endl;
    headerFile << "#include \"TRestMetadata.h\"" << endl;
    headerFile << endl;
    headerFile << "/// UPDATE Write here a brief description. Just one line!" << endl;
    headerFile << "class " << name << " : public TRestMetadata {" << endl;
    headerFile << "private:" << endl;
    headerFile << "" << endl;
    headerFile << "    // REMOVE COMMENT. Add here the members/parameters for your metadata class." << endl;
    headerFile << "    // You can set their default values here together. " << endl;
    headerFile << "    // Note: add \"//!\" mark at the end of the member definition" << endl;
    headerFile
        << "    // if you don't want to save them to disk. The following dummy member are given as examples."
        << endl;
    headerFile << "" << endl;
    headerFile << "    /// REMOVE MEMBER. A dummy member that will be written to the ROOT file." << endl;
    headerFile << "    Double_t fDummy = 3.14; //<" << endl;
    headerFile << "" << endl;
    headerFile << "    /// REMOVE MEMBER. A dummy member that will be NOT written to the ROOT file." << endl;
    headerFile << "    Double_t fDummyVar = 3.14; //!" << endl;
    headerFile << endl;
    headerFile << "public:" << endl;
    headerFile << "    /// UPDATE Documentation of dummy getter" << endl;
    headerFile << "    Double_t GetDummy() { return fDummy;}" << endl;
    headerFile << endl;
    headerFile << "    /// UPDATE Documentation of dummy getter" << endl;
    headerFile << "    Double_t GetDummyVar() { return fDummy;}" << endl;
    headerFile << endl;
    headerFile << "    /// UPDATE Documentation of dummy setter" << endl;
    headerFile << "    void SetDummy( const Double_t &dummy) { fDummy = dummy;}" << endl;
    headerFile << endl;
    headerFile << "    /// UPDATE Documentation of dummy setter" << endl;
    headerFile << "    void SetDummyVar( const Double_t &dummy) { fDummyVar = dummy;}" << endl;
    headerFile << endl;
    headerFile << "    void Initialize() override;" << endl;
    headerFile << endl;
    headerFile << "public:" << endl;
    headerFile << "    void PrintMetadata() override;" << endl;
    headerFile << endl;
    headerFile << "    " << name << "();" << endl;
    headerFile << "    " << name << "(const char* configFilename, std::string name = \"\");" << endl;
    headerFile << "    ~" << name << "();" << endl;
    headerFile << endl;
    headerFile << "    // REMOVE COMMENT. ROOT class definition helper. Increase the number in it every time"
               << endl;
    headerFile << "    // you add/rename/remove the metadata members" << endl;
    headerFile << "    ClassDefOverride(" << name << ", 1);" << endl;
    headerFile << endl;
    headerFile << "};" << endl;
    headerFile << "#endif" << endl;

    headerFile.flush();
    headerFile.close();

    ofstream sourceFile(name + ".cxx");
    if (headerFile.fail()) {
        RESTError << "failed to create file!" << RESTendl;
        return -1;
    }

    sourceFile << "/*************************************************************************" << endl;
    sourceFile << " * This file is part of the REST software framework.                     *" << endl;
    sourceFile << " *                                                                       *" << endl;
    sourceFile << " * Copyright (C) 2016 GIFNA/TREX (University of Zaragoza)                *" << endl;
    sourceFile << " * For more information see https://gifna.unizar.es/trex                 *" << endl;
    sourceFile << " *                                                                       *" << endl;
    sourceFile << " * REST is free software: you can redistribute it and/or modify          *" << endl;
    sourceFile << " * it under the terms of the GNU General Public License as published by  *" << endl;
    sourceFile << " * the Free Software Foundation, either version 3 of the License, or     *" << endl;
    sourceFile << " * (at your option) any later version.                                   *" << endl;
    sourceFile << " *                                                                       *" << endl;
    sourceFile << " * REST is distributed in the hope that it will be useful,               *" << endl;
    sourceFile << " * but WITHOUT ANY WARRANTY; without even the implied warranty of        *" << endl;
    sourceFile << " * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the          *" << endl;
    sourceFile << " * GNU General Public License for more details.                          *" << endl;
    sourceFile << " *                                                                       *" << endl;
    sourceFile << " * You should have a copy of the GNU General Public License along with   *" << endl;
    sourceFile << " * REST in $REST_PATH/LICENSE.                                           *" << endl;
    sourceFile << " * If not, see https://www.gnu.org/licenses/.                            *" << endl;
    sourceFile << " * For the list of contributors see $REST_PATH/CREDITS.                  *" << endl;
    sourceFile << " *************************************************************************/" << endl;
    sourceFile << "" << endl;
    sourceFile << "/////////////////////////////////////////////////////////////////////////" << endl;
    sourceFile << "/// Write the class description Here                                     " << endl;
    sourceFile << "/// " << endl;
    sourceFile << "/// ### Parameters" << endl;
    sourceFile << "/// Describe any parameters this process receives: " << endl;
    sourceFile << "/// * **parameter1**: This parameter ..." << endl;
    sourceFile << "/// * **parameter2**: This parameter is ..." << endl;
    sourceFile << "/// " << endl;
    sourceFile << "/// " << endl;
    sourceFile << "/// ### Examples" << endl;
    sourceFile << "/// Give examples of usage and RML descriptions that can be tested.      " << endl;
    sourceFile << "/// \\code" << endl;
    sourceFile << "///     <WRITE A CODE EXAMPLE HERE>" << endl;
    sourceFile << "/// \\endcode" << endl;
    sourceFile << "/// " << endl;
    sourceFile << "/// ### Running pipeline example" << endl;
    sourceFile << "/// Add the examples to a pipeline to guarantee the code will be running " << endl;
    sourceFile << "/// on future framework upgrades.                                        " << endl;
    sourceFile << "/// " << endl;
    sourceFile << "/// " << endl;
    sourceFile << "/// Please, add any figure that may help to illustrate the process       " << endl;
    sourceFile << "/// " << endl;
    sourceFile
        << "/// \\htmlonly <style>div.image img[src=\"trigger.png\"]{width:500px;}</style> \\endhtmlonly"
        << endl;
    sourceFile << "/// ![An illustration of the trigger definition](trigger.png)             " << endl;
    sourceFile << "/// " << endl;
    sourceFile << "/// The png image should be uploaded to the ./images/ directory          " << endl;
    sourceFile << "///                                                                      " << endl;
    sourceFile << "///----------------------------------------------------------------------" << endl;
    sourceFile << "///                                                                      " << endl;
    sourceFile << "/// REST-for-Physics - Software for Rare Event Searches Toolkit 	    " << endl;
    sourceFile << "///                                                                      " << endl;
    sourceFile << "/// History of developments:                                             " << endl;
    sourceFile << "///                                                                      " << endl;
    sourceFile << "/// YEAR-Month: First implementation of " << name << endl;
    sourceFile << "/// WRITE YOUR FULL NAME " << endl;
    sourceFile << "///                                                                      " << endl;
    sourceFile << "/// \\class " << name << "                                               " << endl;
    sourceFile << "/// \\author: TODO. Write full name and e-mail:        " << getenv("USER") << endl;
    sourceFile << "///                                                                      " << endl;
    sourceFile << "/// <hr>                                                                 " << endl;
    sourceFile << "///                                                                      " << endl;
    sourceFile << endl;
    sourceFile << "#include \"" << name << ".h\"" << endl;
    sourceFile << endl;
    sourceFile << "ClassImp(" << name << ");" << endl;
    sourceFile << endl;
    sourceFile << "///////////////////////////////////////////////                          " << endl;
    sourceFile << "/// \\brief Default constructor                                          " << endl;
    sourceFile << "///                                                                      " << endl;
    sourceFile << name << "::" << name << "() {" << endl;
    sourceFile << "    Initialize();" << endl;
    sourceFile << "}" << endl;
    sourceFile << endl;
    sourceFile << "/////////////////////////////////////////////" << endl;
    sourceFile << "/// \\brief Constructor loading data from a config file" << endl;
    sourceFile << "///" << endl;
    sourceFile << "/// If no configuration path is defined using TRestMetadata::SetConfigFilePath" << endl;
    sourceFile << "/// the path to the config file must be specified using full path, absolute or" << endl;
    sourceFile << "/// relative." << endl;
    sourceFile << "///" << endl;
    sourceFile << "/// The default behaviour is that the config file must be specified with" << endl;
    sourceFile << "/// full path, absolute or relative." << endl;
    sourceFile << "///" << endl;
    sourceFile << "/// \\param configFilename A const char* giving the path to an RML file." << endl;
    sourceFile << "/// \\param name The name of the specific metadata. It will be used to find the" << endl;
    sourceFile << "/// corresponding TRestAxionMagneticField section inside the RML." << endl;
    sourceFile << "///" << endl;
    sourceFile << name << "::" << name
               << "(const char* configFilename, std::string name) : TRestMetadata(configFilename) {" << endl;
    sourceFile << "    LoadConfigFromFile(fConfigFileName, name);" << endl;
    sourceFile << "" << endl;
    sourceFile << "	 if (GetVerboseLevel() >= REST_Info) PrintMetadata();" << endl;
    sourceFile << "}" << endl;
    sourceFile << endl;
    sourceFile << "///////////////////////////////////////////////                          " << endl;
    sourceFile << "/// \\brief Default destructor                                           " << endl;
    sourceFile << "///                                                                      " << endl;
    sourceFile << name << "::~" << name << "() {" << endl;
    sourceFile << "}" << endl;
    sourceFile << endl;
    sourceFile << "///////////////////////////////////////////////                          " << endl;
    sourceFile << "/// \\brief Function to initialize input/output event members and define " << endl;
    sourceFile << "/// the section name                                                     " << endl;
    sourceFile << "///                                                                      " << endl;
    sourceFile << "void " << name << "::Initialize() {" << endl;
    sourceFile << "    SetSectionName(this->ClassName());" << endl;
    sourceFile << "    // REMOVE COMMENT. Remove SetLibraryVersion line code if" << endl;
    sourceFile << "    // your metadata class is a framework class." << endl;
    sourceFile << "    SetLibraryVersion(LIBRARY_VERSION);" << endl;
    sourceFile << endl;
    sourceFile << "    // REMOVE COMMENT. Initialize here any special data members if needed" << endl;
    sourceFile << endl;
    sourceFile << "}" << endl;
    sourceFile << endl;
    sourceFile << "/////////////////////////////////////////////" << endl;
    sourceFile
        << "/// \\brief Prints on screen the information about the metadata members of TRestAxionSolarFlux"
        << endl;
    sourceFile << "///" << endl;
    sourceFile << "void " << name << "::PrintMetadata() {" << endl;
    sourceFile << "    TRestMetadata::PrintMetadata();" << endl;
    sourceFile << endl;
    sourceFile << "    metadata << \" - Dummy member : \" << fDummy << endl;" << endl;
    sourceFile << "}" << endl;
    sourceFile << endl;

    sourceFile.flush();
    sourceFile.close();

    cout << endl;
    cout << "--------------------------------" << endl;
    cout << "Metadata class code generation complete!" << endl;
    cout << "* " << name << ".h" << endl;
    cout << "* " << name << ".cxx" << endl;
    cout << endl;
    cout << "Put the two files into proper directories and re-run cmake/make" << endl;
    cout << "Remember to replace REMOVE/UPDATE statements by meaningful documentation" << endl;
    cout << "--------------------------------" << endl;
    cout << endl;

    return 0;
}
#endif
