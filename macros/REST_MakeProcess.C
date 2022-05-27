#include <iostream>

#include "TRestReflector.h"
#include "TRestStringHelper.h"
#include "TString.h"

#ifndef RESTTask_MakeProcess
#define RESTTask_MakeProcess

//*******************************************************************************************************
//***
//*** Your HELP is needed to verify, validate and document this macro
//*** This macro might need update/revision.
//***
//*******************************************************************************************************
Int_t REST_MakeProcess(TString name, TString inputevent = "TRestEvent", TString outputevent = "TRestEvent") {
    // REST_MakeProcess("TRestXXXProcess", "TRestDetectorHitsEvent")
    //    --> generate an analysis process template for TRestDetectorHitsEvent
    // restMakeProcess TRestXXXProcess TRestDetectorHitsEvent TRestTrackEvent
    //    --> generate a data-conversion process template form TRestDetectorHitsEvent to TRestTrackEvent

    if (name.First("TRest") != 0 || name.Contains("Process") == false) {
        RESTError << "invalid process name! REST process name must be start with \"TRest\" and ends with "
                     "\"Process\"!"
                  << RESTendl;
        return -1;
    }

    if (TClass::GetClass(inputevent) == NULL) {
        cout << "Warning! unrecognized type for input event: " << inputevent
             << ", generated process may not be compilable!" << endl;
    }
    if (TClass::GetClass(outputevent) == NULL) {
        cout << "Warning! unrecognized type for output event: " << outputevent
             << ", generated process may not be compilable!" << endl;
    }

    // if the output event type is in default, we make it same as input event
    if (outputevent == "TRestEvent") {
        outputevent = inputevent;
    }

    TString _inputevent = inputevent;
    TString _outputevent = outputevent;
    TString _input = _inputevent.ReplaceAll("TRest", "").ReplaceAll("Event", "");
    TString _output = _outputevent.ReplaceAll("TRest", "").ReplaceAll("Event", "");

    TString inputeventname;
    TString outputeventname;
    if (inputevent == outputevent) {
        if (_input != "") {
            inputeventname = "fAnaEvent";
            outputeventname = "fAnaEvent";
        } else {
            inputeventname = "fEvent";
            outputeventname = "fEvent";
        }
    } else {
        inputeventname = "fInput" + _input + "Event";
        outputeventname = "fOutput" + _output + "Event";
    }

    ofstream headerfile(name + ".h");
    if (headerfile.fail()) {
        RESTError << "failed to create file!" << RESTendl;
        return -1;
    }

    headerfile << "/*************************************************************************" << endl;
    headerfile << " * This file is part of the REST software framework.                     *" << endl;
    headerfile << " *                                                                       *" << endl;
    headerfile << " * Copyright (C) 2016 GIFNA/TREX (University of Zaragoza)                *" << endl;
    headerfile << " * For more information see http://gifna.unizar.es/trex                  *" << endl;
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
    headerfile << " * If not, see http://www.gnu.org/licenses/.                             *" << endl;
    headerfile << " * For the list of contributors see $REST_PATH/CREDITS.                  *" << endl;
    headerfile << " *************************************************************************/" << endl;
    headerfile << endl;
    headerfile << "#ifndef RESTProc_" << name << endl;
    headerfile << "#define RESTProc_" << name << endl;
    headerfile << endl;
    headerfile << "#include \"" << inputevent << ".h\"" << endl;
    headerfile << "#include \"" << outputevent << ".h\"" << endl;
    headerfile << "#include \"TRestEventProcess.h\"" << endl;
    headerfile << endl;
    headerfile << "/// TODO Write here a brief description. Just one line!" << endl;
    headerfile << "class " << name << " : public TRestEventProcess {" << endl;
    headerfile << "private:" << endl;

    if (inputevent == outputevent) {
        headerfile << "    "
                   << "/// A pointer to the specific " << inputevent << " input event" << endl;
        headerfile << "    " << inputevent << "* " << inputeventname << ";  //!" << endl;
    } else {
        headerfile << "    "
                   << "/// A pointer to the specific " << inputevent << " input event" << endl;
        headerfile << "    " << inputevent << "* " << inputeventname << ";  //!" << endl;
        headerfile << "    "
                   << "/// A pointer to the specific " << outputevent << " output event" << endl;
        headerfile << "    " << outputevent << "* " << outputeventname << ";  //!" << endl;
    }

    headerfile << endl;
    headerfile << "    void Initialize() override;" << endl;
    headerfile << endl;
    headerfile << "    // Add here the members or parameters for your event process." << endl;
    headerfile << "    // You can set their default values here together. " << endl;
    headerfile << "    // Note: add \"//!\" mark at the end of the member definition" << endl;
    headerfile << "    // if you don't want to save them to disk." << endl;
    headerfile << "" << endl;
    headerfile << "    /// REMOVE THIS MEMBER! A dummy member that will be written to the ROOT file." << endl;
    headerfile << "    Double_t fDummy = 3.14; //<" << endl;
    headerfile << "" << endl;
    headerfile << "    /// REMOVE THIS MEMBER! A dummy member that will be NOT written to the ROOT file."
               << endl;
    headerfile << "    Double_t fDummyVar = 3.14; //!" << endl;
    headerfile << endl;
    headerfile << "public:" << endl;
    headerfile << "    any GetInputEvent() const override { return " << inputeventname << "; }" << endl;
    headerfile << "    any GetOutputEvent() const override { return " << outputeventname << "; }" << endl;
    headerfile << endl;
    headerfile << "    void InitProcess() override;" << endl;
    headerfile << endl;
    headerfile << "const char* GetProcessName() const override { return \"CHANGEME!\"; }" << endl;
    headerfile << endl;
    headerfile << "    TRestEvent* ProcessEvent (TRestEvent* eventInput) override;" << endl;
    headerfile << endl;
    headerfile << "    void EndProcess() override;" << endl;
    headerfile << endl;
    headerfile << "    ///  It prints out the process parameters stored in the metadata structure" << endl;
    headerfile << "    void PrintMetadata() override {" << endl;
    headerfile << "        BeginPrintProcess();" << endl;
    headerfile << endl;
    headerfile << "        // Write here how to print the added process members and parameters." << endl;
    headerfile << endl;
    headerfile << "        EndPrintProcess();" << endl;
    headerfile << "    }" << endl;
    headerfile << endl;
    headerfile << "    " << name << "();" << endl;
    headerfile << "    ~" << name << "();" << endl;
    headerfile << endl;
    headerfile << "    // ROOT class definition helper. Increase the number in it every time" << endl;
    headerfile << "    // you add/rename/remove the process parameters" << endl;
    headerfile << "    ClassDefOverride(" << name << ", 1);" << endl;
    headerfile << endl;
    headerfile << "};" << endl;
    headerfile << "#endif" << endl;

    headerfile.flush();
    headerfile.close();

    ofstream sourcefile(name + ".cxx");
    if (headerfile.fail()) {
        RESTError << "failed to create file!" << RESTendl;
        return -1;
    }

    sourcefile << "/*************************************************************************" << endl;
    sourcefile << " * This file is part of the REST software framework.                     *" << endl;
    sourcefile << " *                                                                       *" << endl;
    sourcefile << " * Copyright (C) 2016 GIFNA/TREX (University of Zaragoza)                *" << endl;
    sourcefile << " * For more information see http://gifna.unizar.es/trex                  *" << endl;
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
    sourcefile << " * If not, see http://www.gnu.org/licenses/.                             *" << endl;
    sourcefile << " * For the list of contributors see $REST_PATH/CREDITS.                  *" << endl;
    sourcefile << " *************************************************************************/" << endl;
    sourcefile << "" << endl;
    sourcefile << "/////////////////////////////////////////////////////////////////////////" << endl;
    sourcefile << "/// Write the process description Here                                   " << endl;
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
    sourcefile << "/// Please, add any figure that may help to ilustrate the process        " << endl;
    sourcefile << "/// " << endl;
    sourcefile
        << "/// \\htmlonly <style>div.image img[src=\"trigger.png\"]{width:500px;}</style> \\endhtmlonly"
        << endl;
    sourcefile << "/// ![An ilustration of the trigger definition](trigger.png)             " << endl;
    sourcefile << "/// " << endl;
    sourcefile << "/// The png image should be uploaded to the ./images/ directory          " << endl;
    sourcefile << "///                                                                      " << endl;
    sourcefile << "///----------------------------------------------------------------------" << endl;
    sourcefile << "///                                                                      " << endl;
    sourcefile << "/// REST-for-Physics - Software for Rare Event Searches Toolkit 		    " << endl;
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
    sourcefile << "///////////////////////////////////////////////                          " << endl;
    sourcefile << "/// \\brief Default destructor                                           " << endl;
    sourcefile << "///                                                                      " << endl;
    sourcefile << name << "::~" << name << "() {" << endl;
    if (outputeventname != inputeventname) {
        sourcefile << "    delete " << outputeventname << ";" << endl;
    }
    sourcefile << "}" << endl;
    sourcefile << endl;
    sourcefile << "///////////////////////////////////////////////                          " << endl;
    sourcefile << "/// \\brief Function to initialize input/output event members and define  " << endl;
    sourcefile << "/// the section name                                                     " << endl;
    sourcefile << "///                                                                      " << endl;
    sourcefile << "void " << name << "::Initialize() {" << endl;
    sourcefile << "    SetSectionName(this->ClassName());" << endl;
    sourcefile << "    SetLibraryVersion(LIBRARY_VERSION);" << endl;
    sourcefile << "    " << inputeventname << " = NULL;" << endl;
    if (outputeventname != inputeventname) {
        sourcefile << "    " << outputeventname << " = new " << outputevent << "();" << endl;
    }
    sourcefile << endl;
    sourcefile << "    // Initialize here the values of class data members if needed       " << endl;
    sourcefile << endl;
    sourcefile << "}" << endl;
    sourcefile << endl;
    sourcefile << "///////////////////////////////////////////////                           " << endl;
    sourcefile << "/// \\brief Process initialization. Observable names can be re-interpreted " << endl;
    sourcefile << "/// here. Any action in the process required before starting event process " << endl;
    sourcefile << "/// might be added here.                                                 " << endl;
    sourcefile << "///                                                                      " << endl;
    sourcefile << "void " << name << "::InitProcess() {" << endl;
    sourcefile << "    // Write here the jobs to do before processing" << endl;
    sourcefile << "    // i.e., initialize histograms and auxiliary vectors," << endl;
    sourcefile << "    // read TRestRun metadata, or load additional rml sections" << endl;
    sourcefile << endl;
    sourcefile << "}" << endl;
    sourcefile << endl;
    sourcefile << "///////////////////////////////////////////////                          " << endl;
    sourcefile << "/// \\brief The main processing event function                           " << endl;
    sourcefile << "///                                                                      " << endl;
    sourcefile << "TRestEvent* " << name << "::ProcessEvent(TRestEvent * evInput) {" << endl;
    sourcefile << "    " << inputeventname << " = (" << inputevent << "*)evInput;" << endl;
    sourcefile << endl;
    sourcefile << "    // Write here the main logic of process: " << name << endl;
    sourcefile
        << "    // Read data from input event, write data to output event, and save observables to tree"
        << endl;
    sourcefile << endl;
    sourcefile << "    return " << outputeventname << ";" << endl;
    sourcefile << "}" << endl;
    sourcefile << endl;
    sourcefile << "///////////////////////////////////////////////                          " << endl;
    sourcefile << "/// \\brief Function to include required actions after all events have been" << endl;
    sourcefile << "/// processed.                                                            " << endl;
    sourcefile << "///                                                                       " << endl;
    sourcefile << "void " << name << "::EndProcess() {" << endl;
    sourcefile << "    // Write here the jobs to do when all the events are processed" << endl;
    sourcefile << endl;
    sourcefile << "}" << endl;
    sourcefile << endl;

    sourcefile.flush();
    sourcefile.close();

    cout << endl;
    cout << "--------------------------------" << endl;
    cout << "Process code generation complete!" << endl;
    cout << "* " << name << ".h" << endl;
    cout << "* " << name << ".cxx" << endl;
    cout << endl;
    cout << "Put the two files into proper directories and re-run cmake/make" << endl;
    cout << "--------------------------------" << endl;
    cout << endl;

    return 0;
}
#endif
