
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
        ferr << "invalid process name! REST process name must be start with \"TRest\" " << endl;
        return -1;
    }

    ofstream headerfile(name + ".h");
    if (headerfile.fail()) {
        ferr << "failed to create file!" << endl;
        return -1;
    }

    headerfile << "/*************************************************************************" << endl;
    headerfile << " * This file is part of the REST software framework.                     *" << endl;
    headerfile << " *                                                                       *" << endl;
    headerfile << " * Copyright (C) 2016 GIFNA/TREX (University of Zaragoza)                *" << endl;
    headerfile << " * For more information see https://gifna.unizar.es/trex                 *" << endl;
    headerfile << " *                                                                       *" << endl;
    headerfile << " * REST is free software: you can redistribute it and/or modify          *" << endl;
    headerfile << " * it under the terms of the GNU General Public License as published by  *" << endl;
    headerfile << " * the Free Software Foundation, either version 3 of the License, or     *" << endl;
    headerfile << " * (at your option) any later version.                                   *" << endl;
    headerfile << " *                                                                       *" << endl;
    headerfile << " * REST is distributed in the hope that it will be useful,               *" << endl;
    headerfile << " * but WITHOUT ANY WARRANTY; without even the implied warranty of        *" << endl;
    headerfile << " * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the          *" << endl;
    headerfile << " * GNU General Public License for more details.                          *" << endl;
    headerfile << " *                                                                       *" << endl;
    headerfile << " * You should have a copy of the GNU General Public License along with   *" << endl;
    headerfile << " * REST in $REST_PATH/LICENSE.                                           *" << endl;
    headerfile << " * If not, see https://www.gnu.org/licenses/.                            *" << endl;
    headerfile << " * For the list of contributors see $REST_PATH/CREDITS.                  *" << endl;
    headerfile << " *************************************************************************/" << endl;
    headerfile << endl;
    headerfile << "#ifndef REST_" << name << endl;
    headerfile << "#define REST_" << name << endl;
    headerfile << endl;
    headerfile << "#include \"TRestMetadata.h\"" << endl;
    headerfile << endl;
    headerfile << "/// UPDATE Write here a brief description. Just one line!" << endl;
    headerfile << "class " << name << " : public TRestMetadata {" << endl;
    headerfile << "private:" << endl;
    headerfile << "" << endl;
    headerfile << "    // REMOVE COMMENT. Add here the members/parameters for your metadata class." << endl;
    headerfile << "    // You can set their default values here together. " << endl;
    headerfile << "    // Note: add \"//!\" mark at the end of the member definition" << endl;
    headerfile
        << "    // if you don't want to save them to disk. The following dummy member are given as examples."
        << endl;
    headerfile << "" << endl;
    headerfile << "    /// REMOVE MEMBER. A dummy member that will be written to the ROOT file." << endl;
    headerfile << "    Double_t fDummy = 3.14; //<" << endl;
    headerfile << "" << endl;
    headerfile << "    /// REMOVE MEMBER. A dummy member that will be NOT written to the ROOT file." << endl;
    headerfile << "    Double_t fDummyVar = 3.14; //!" << endl;
    headerfile << endl;
    headerfile << "    void Initialize();" << endl;
    headerfile << endl;
    headerfile << "public:" << endl;
    headerfile << "    void PrintMetadata();" << endl;
    headerfile << endl;
    headerfile << "    " << name << "();" << endl;
    headerfile << "    " << name << "(const char* cfgFileName, std::string name = \"\")" << endl;
    headerfile << "    ~" << name << "();" << endl;
    headerfile << endl;
    headerfile << "    // REMOVE COMMENT. ROOT class definition helper. Increase the number in it every time"
               << endl;
    headerfile << "    // you add/rename/remove the metadata members" << endl;
    headerfile << "    ClassDef(" << name << ", 1);" << endl;
    headerfile << endl;
    headerfile << "};" << endl;
    headerfile << "#endif" << endl;

    headerfile.flush();
    headerfile.close();

    ofstream sourcefile(name + ".cxx");
    if (headerfile.fail()) {
        ferr << "failed to create file!" << endl;
        return -1;
    }

    sourcefile << "/*************************************************************************" << endl;
    sourcefile << " * This file is part of the REST software framework.                     *" << endl;
    sourcefile << " *                                                                       *" << endl;
    sourcefile << " * Copyright (C) 2016 GIFNA/TREX (University of Zaragoza)                *" << endl;
    sourcefile << " * For more information see https://gifna.unizar.es/trex                 *" << endl;
    sourcefile << " *                                                                       *" << endl;
    sourcefile << " * REST is free software: you can redistribute it and/or modify          *" << endl;
    sourcefile << " * it under the terms of the GNU General Public License as published by  *" << endl;
    sourcefile << " * the Free Software Foundation, either version 3 of the License, or     *" << endl;
    sourcefile << " * (at your option) any later version.                                   *" << endl;
    sourcefile << " *                                                                       *" << endl;
    sourcefile << " * REST is distributed in the hope that it will be useful,               *" << endl;
    sourcefile << " * but WITHOUT ANY WARRANTY; without even the implied warranty of        *" << endl;
    sourcefile << " * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the          *" << endl;
    sourcefile << " * GNU General Public License for more details.                          *" << endl;
    sourcefile << " *                                                                       *" << endl;
    sourcefile << " * You should have a copy of the GNU General Public License along with   *" << endl;
    sourcefile << " * REST in $REST_PATH/LICENSE.                                           *" << endl;
    sourcefile << " * If not, see https://www.gnu.org/licenses/.                            *" << endl;
    sourcefile << " * For the list of contributors see $REST_PATH/CREDITS.                  *" << endl;
    sourcefile << " *************************************************************************/" << endl;
    sourcefile << "" << endl;
    sourcefile << "/////////////////////////////////////////////////////////////////////////" << endl;
    sourcefile << "/// Write the class description Here                                     " << endl;
    sourcefile << "/// " << endl;
    sourcefile << "/// ### Parameters" << endl;
    sourcefile << "/// Describe any parameters this process receives: " << endl;
    sourcefile << "/// * **parameter1**: This parameter ..." << endl;
    sourcefile << "/// * **parameter2**: This parameter is ..." << endl;
    sourcefile << "/// " << endl;
    sourcefile << "/// " << endl;
    sourcefile << "/// ### Examples" << endl;
    sourcefile << "/// Give examples of usage and RML descriptions that can be tested.      " << endl;
    sourcefile << "/// \\code" << endl;
    sourcefile << "///     <WRITE A CODE EXAMPLE HERE>" << endl;
    sourcefile << "/// \\endcode" << endl;
    sourcefile << "/// " << endl;
    sourcefile << "/// ### Running pipeline example" << endl;
    sourcefile << "/// Add the examples to a pipeline to guarantee the code will be running " << endl;
    sourcefile << "/// on future framework upgrades.                                        " << endl;
    sourcefile << "/// " << endl;
    sourcefile << "/// " << endl;
    sourcefile << "/// Please, add any figure that may help to illustrate the process       " << endl;
    sourcefile << "/// " << endl;
    sourcefile
        << "/// \\htmlonly <style>div.image img[src=\"trigger.png\"]{width:500px;}</style> \\endhtmlonly"
        << endl;
    sourcefile << "/// ![An illustration of the trigger definition](trigger.png)             " << endl;
    sourcefile << "/// " << endl;
    sourcefile << "/// The png image should be uploaded to the ./images/ directory          " << endl;
    sourcefile << "///                                                                      " << endl;
    sourcefile << "///----------------------------------------------------------------------" << endl;
    sourcefile << "///                                                                      " << endl;
    sourcefile << "/// REST-for-Physics - Software for Rare Event Searches Toolkit 	    " << endl;
    sourcefile << "///                                                                      " << endl;
    sourcefile << "/// History of developments:                                             " << endl;
    sourcefile << "///                                                                      " << endl;
    sourcefile << "/// YEAR-Month: First implementation of " << name << endl;
    sourcefile << "/// WRITE YOUR FULL NAME " << endl;
    sourcefile << "///                                                                      " << endl;
    sourcefile << "/// \\class " << name << "                                               " << endl;
    sourcefile << "/// \\author: TODO. Write full name and e-mail:        " << getenv("USER") << endl;
    sourcefile << "///                                                                      " << endl;
    sourcefile << "/// <hr>                                                                 " << endl;
    sourcefile << "///                                                                      " << endl;
    sourcefile << endl;
    sourcefile << "#include \"" << name << ".h\"" << endl;
    sourcefile << endl;
    sourcefile << "ClassImp(" << name << ");" << endl;
    sourcefile << endl;
    sourcefile << "///////////////////////////////////////////////                          " << endl;
    sourcefile << "/// \\brief Default constructor                                          " << endl;
    sourcefile << "///                                                                      " << endl;
    sourcefile << name << "::" << name << "() {" << endl;
    sourcefile << "    Initialize();" << endl;
    sourcefile << "}" << endl;
    sourcefile << endl;
    sourcefile << "/////////////////////////////////////////////" << endl;
    sourcefile << "/// \\brief Constructor loading data from a config file" << endl;
    sourcefile << "///" << endl;
    sourcefile << "/// If no configuration path is defined using TRestMetadata::SetConfigFilePath" << endl;
    sourcefile << "/// the path to the config file must be specified using full path, absolute or" << endl;
    sourcefile << "/// relative." << endl;
    sourcefile << "///" << endl;
    sourcefile << "/// The default behaviour is that the config file must be specified with" << endl;
    sourcefile << "/// full path, absolute or relative." << endl;
    sourcefile << "///" << endl;
    sourcefile << "/// \\param cfgFileName A const char* giving the path to an RML file." << endl;
    sourcefile << "/// \\param name The name of the specific metadata. It will be used to find the" << endl;
    sourcefile << "/// corresponding TRestAxionMagneticField section inside the RML." << endl;
    sourcefile << "///" << endl;
    sourcefile << name << "::" << name
               << "(const char* cfgFileName, string name) : TRestMetadata(cfgFileName) {" << endl;
    sourcefile << "    LoadConfigFromFile(fConfigFileName, name);" << endl;
    sourcefile << "" << endl;
    sourcefile << "	 if (GetVerboseLevel() >= REST_Info) PrintMetadata();" << endl;
    sourcefile << "}" << endl;
    sourcefile << endl;
    sourcefile << "///////////////////////////////////////////////                          " << endl;
    sourcefile << "/// \\brief Default destructor                                           " << endl;
    sourcefile << "///                                                                      " << endl;
    sourcefile << name << "::~" << name << "() {" << endl;
    sourcefile << "}" << endl;
    sourcefile << endl;
    sourcefile << "///////////////////////////////////////////////                          " << endl;
    sourcefile << "/// \\brief Function to initialize input/output event members and define " << endl;
    sourcefile << "/// the section name                                                     " << endl;
    sourcefile << "///                                                                      " << endl;
    sourcefile << "void " << name << "::Initialize() {" << endl;
    sourcefile << "    SetSectionName(this->ClassName());" << endl;
    sourcefile << "    SetLibraryVersion(LIBRARY_VERSION);" << endl;
    sourcefile << endl;
    sourcefile << "    // REMOVE COMMENT. Initialize here any special data members if needed" << endl;
    sourcefile << endl;
    sourcefile << "}" << endl;
    sourcefile << endl;
    sourcefile << "/////////////////////////////////////////////" << endl;
    sourcefile
        << "/// \\brief Prints on screen the information about the metadata members of TRestAxionSolarFlux"
        << endl;
    sourcefile << "///" << endl;
    sourcefile << "void " << name << "::PrintMetadata() {" << endl;
    sourcefile << "    TRestMetadata::PrintMetadata();" << endl;
    sourcefile << endl;
    sourcefile << "    metadata << \" - Dummy member : \" << fDummy << endl;" << endl;
    sourcefile << "}" << endl;
    sourcefile << endl;

    sourcefile.flush();
    sourcefile.close();

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
