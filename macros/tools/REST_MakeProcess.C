#include <iostream>

#include "TRestStringHelper.h"
#include "TString.h"

Int_t REST_MakeProcess(TString name, TString inputevent = "TRestEvent", TString outputevent = "TRestEvent") {
    if (name.First("TRest") != 0 || name.Contains("Process") == false) {
        ferr << "invalid process name! REST process name must be start with \"TRest\" and ends with "
                "\"Process\"!"
             << endl;
        return -1;
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
        ferr << "failed to create file!" << endl;
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
    headerfile << "class " << name << " : public TRestEventProcess {" << endl;
    headerfile << "private:" << endl;
    headerfile << "    // We define specific input/output event data holders" << endl;

    if (inputevent == outputevent) {
        headerfile << "    " << inputevent << "* " << inputeventname << ";  //!" << endl;
    } else {
        headerfile << "    " << inputevent << "* " << inputeventname << ";  //!" << endl;
        headerfile << "    " << outputevent << "* " << outputeventname << ";  //!" << endl;
    }

    headerfile << endl;
    headerfile << "    void InitFromConfigFile();" << endl;
    headerfile << endl;
    headerfile << "    void Initialize();" << endl;
    headerfile << endl;
    headerfile << "    // Add here the members or parameters for your event process." << endl;
    headerfile << "    // Note: add \"//!\" mark at the end of the member definition" << endl;
    headerfile << "    // if you don't want to save them as \"metadata\"." << endl;
    headerfile << endl;
    headerfile << "public:" << endl;
    headerfile << "    any GetInputEvent() { return " << inputeventname << "; }" << endl;
    headerfile << "    any GetOutputEvent() { return " << outputeventname << "; }" << endl;
    headerfile << endl;
    headerfile << "    void InitProcess();" << endl;
    headerfile << endl;
    headerfile << "    TRestEvent* ProcessEvent(TRestEvent* eventInput);" << endl;
    headerfile << endl;
    headerfile << "    void EndProcess();" << endl;
    headerfile << endl;
    headerfile << "    void PrintMetadata() {" << endl;
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
    sourcefile << " /////////////////////////////////////////////////////////////////////////" << endl;
    sourcefile << " /// Enter your description Here                                          " << endl;
    sourcefile << " ///                                                                      " << endl;
    sourcefile << " /// \\class " << name << "                                             " << endl;
    sourcefile << " ///                                                                      " << endl;
    sourcefile << " ///----------------------------------------------------------------------" << endl;
    sourcefile << " ///                                                                      " << endl;
    sourcefile << " /// By REST process template generator                                   " << endl;
    sourcefile << " /// User: " << getenv("USER") << endl;
    sourcefile << " /// Date: " << ToDateTimeString(time(0)) << endl;
    sourcefile << " ///                                                                      " << endl;
    sourcefile << " /// <hr>                                                                 " << endl;
    sourcefile << " /////////////////////////////////////////////////////////////////////////" << endl;
    sourcefile << endl;
    sourcefile << "#include \"" << name << ".h\"" << endl;
    sourcefile << endl;
    sourcefile << "ClassImp(" << name << ");" << endl;
    sourcefile << endl;
    sourcefile << name << "::" << name << "() {" << endl;
    sourcefile << "    Initialize();" << endl;
    sourcefile << "}" << endl;
    sourcefile << endl;
    sourcefile << name << "::~" << name << "() {" << endl;
    if (outputeventname != inputeventname) {
        sourcefile << "    delete " << outputeventname << ";" << endl;
    }
    sourcefile << "}" << endl;
    sourcefile << endl;
    sourcefile << "void " << name << "::Initialize() {" << endl;
    sourcefile << "    SetSectionName(this->ClassName());" << endl;
    sourcefile << "    " << inputeventname << " = NULL;" << endl;
    if (outputeventname != inputeventname) {
        sourcefile << "    " << outputeventname << " = new " << outputevent << "();" << endl;
    }
    sourcefile << endl;
    sourcefile << "    // Assign initial values for the parameters here" << endl;
    sourcefile << endl;
    sourcefile << "}" << endl;
    sourcefile << endl;
    sourcefile << "void " << name << "::InitProcess() {" << endl;
    sourcefile << "    // Write here the jobs to do before processing" << endl;
    sourcefile << endl;
    sourcefile << "}" << endl;
    sourcefile << endl;
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
    sourcefile << "void " << name << "::EndProcess() {" << endl;
    sourcefile << "    // Write here the jobs to do when all the events are processed" << endl;
    sourcefile << endl;
    sourcefile << "}" << endl;
    sourcefile << endl;
    sourcefile << "void " << name << "::InitFromConfigFile() {" << endl;
    sourcefile << "    // Define here how to read the config file. What parameter to get." << endl;
    sourcefile << endl;
    sourcefile << "}" << endl;

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