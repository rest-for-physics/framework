//////////////////////////////////////////////////////////////////////////
///
/// REST macros are actually ROOT scripts written in C++. TRestTask wraps
/// them and turns them into application mateadata class. This is done by
/// calling method TInterpreter::LoadFile() and TInterpreter::ProcessLine().
/// Turning REST-macro-defined functions into classes means that we
/// are able to run the macros from rml file, by adding a section under the
/// section "TRestManager". Another way to launch TRestTask is through
/// restManager executable. By directly typing "restManager TASKNAME [ARG]"
/// in bash we can run the macros like executables. We also allow users to
/// write TRestTask inherted class in REST macros. This can enable more
/// functionalities.
///
/// \class TRestTask
///
///--------------------------------------------------------------------------
///
/// RESTsoft - Software for Rare Event Searches with TPCs
///
/// History of developments:
///
/// 2017-Nov:  First concept and implementation of TRestTask
///            Kaixiang Ni
///
/// <hr>
//////////////////////////////////////////////////////////////////////////

#include "TRestTask.h"

#include "TRestManager.h"
#include "TRestReflector.h"
ClassImp(TRestTask);

using namespace REST_Reflection;
///////////////////////////////////////////////
/// \brief TRestTask default constructor
///
TRestTask::TRestTask() {
    Initialize();
    fNRequiredArgument = 0;
    if (this->ClassName() != (string) "TRestTask") {
        fMode = TASK_CLASS;
    }
}

///////////////////////////////////////////////
/// \brief TRestTask constructor with macro file name
///
/// The first method in the file is identified with its name and require
/// arguments saved in the class. They will be used in forming the command
/// line in the method TRestTask::InitTask()
TRestTask::TRestTask(TString TaskString, REST_TASKMODE mode) {
    Initialize();
    fNRequiredArgument = 0;
    fMode = mode;

    if (mode == 0) {
        // we parse the macro file, get the method's name and argument list
        ifstream in(TaskString);
        if (!in.is_open()) {
            ferr << "Error opening file" << endl;
        }
        char buffer[256];
        while (!in.eof()) {
            in.getline(buffer, 256);
            string line = buffer;

            if (line.find('(') != -1 && line.find(' ') > 0) {
                // this line is a definition of the macro method
                methodname = line.substr(line.find(' ') + 1, line.find('(') - line.find(' ') - 1);
                SetName(methodname.c_str());

                while (line.find(')') == -1) {
                    if (in.eof()) {
                        ferr << "invaild macro file!" << endl;
                    }
                    in.getline(buffer, 256);
                    line = line + (string)buffer;
                }

                string args = line.substr(line.find('(') + 1, line.find(')') - line.find('(') - 1);
                auto list = Split(args, ",");
                argumentname.clear();
                argumenttype.clear();
                for (int i = 0; i < list.size(); i++) {
                    auto tmp = Split(list[i], " ");
                    if (Count(tmp[0], "TString") > 0) {
                        argumenttype.push_back(65);
                    } else if (Count(ToUpper(tmp[0]), "INT") > 0) {
                        argumenttype.push_back(3);
                    } else if (Count(ToUpper(tmp[0]), "DOUBLE") > 0) {
                        argumenttype.push_back(8);
                    } else {
                        argumenttype.push_back(-1);
                    }

                    argumentname.push_back(Split(tmp[1], "=")[0]);
                    if (Split(list[i], "=").size() > 1) {
                        argument.push_back(Split(list[i], "=")[1]);
                    } else {
                        argument.push_back("NOT SET");
                        fNRequiredArgument++;
                    }
                }

                break;
            }
        }
    } else if (mode == 1) {
        // we parse the command, get the target object, method to be invoked, and
        // the argument list
        string cmd = (string)TaskString;

        string name;
        string call;
        if (Split(cmd, "->").size() != 2) {
            if (Split(cmd, ".").size() != 2) {
                warning << "command"
                        << " \"" << cmd << "\" "
                        << "is illegal!" << endl;
                fMode = TASK_ERROR;
                return;
            } else {
                name = Split(cmd, ".")[0];
                call = Split(cmd, ".")[1];
            }
        } else {
            name = Split(cmd, "->")[0];
            call = Split(cmd, "->")[1];
        }
        if (Count(call, "(") != 1 || Count(call, ")") != 1)  // we can only use one bracket
        {
            warning << "command"
                    << " \"" << cmd << "\" "
                    << "is illegal!" << endl;
            fMode = TASK_ERROR;
            return;
        }
        methodname = Split(call, "(")[0];
        argument.push_back(Split(Split(call, "(")[1], ")").size() == 0 ? ""
                                                                       : Split(Split(call, "(")[1], ")")[0]);
        targetname = name;

    } else if (mode == 2) {
        // I don't think we can get here
    }
}

///////////////////////////////////////////////
/// \brief Starter method. Looks through the rml sections and set
/// argument/datamenber value
///
void TRestTask::InitFromConfigFile() {
    if (fMode == TASK_MACRO) {
        TiXmlElement* ele = fElement->FirstChildElement("parameter");
        while (ele != NULL) {
            if (ele->Attribute("name") == NULL || ele->Attribute("value") == NULL) continue;
            string name = ele->Attribute("name");
            string value = ele->Attribute("value");
            for (int i = 0; i < argumentname.size(); i++) {
                if (name == argumentname[i]) {
                    argument[i] = value;
                }
            }
            ele = ele->NextSiblingElement("parameter");
        }

    } else if (fMode == TASK_CLASS) {
        // load config for the inherited task class
        SetDataMemberValFromConfig();
    }
}

///////////////////////////////////////////////
/// \brief Set argument directly with a list of string
///
/// Argument list will be overwritten by the input list. It will also set the
/// data member value for derived class
void TRestTask::SetArgumentValue(vector<string> arg) {
    if (arg.size() < fNRequiredArgument) {
        PrintArgumentHelp();
        exit(0);
    }
    argument = arg;
    if (fMode == TASK_CLASS) {
        SetDataMemberValFromConfig();
    }
}

///////////////////////////////////////////////
/// \brief Forms the command string
///
void TRestTask::ConstructCommand() {
    // check if all the arguments have been set
    for (int i = 0; i < argument.size(); i++) {
        if (argument[i] == "NOT SET") {
            ferr << "TRestTask : argument " << i << " not set! Task will not run!" << endl;
        }
    }

    string methodname = GetName();
    cmdstr = methodname + "(";
    for (int i = 0; i < argument.size(); i++) {
        if (argumenttype[i] == 65) {
            cmdstr += "\"" + argument[i] + "\"";
        } else {
            cmdstr += argument[i];
        }

        if (i < argument.size() - 1) cmdstr += ",";
    }
    cmdstr += ")";

    cout << "Command : " << cmdstr << endl;
}

///////////////////////////////////////////////
/// \brief Run the task with command line
///
void TRestTask::RunTask(TRestManager* mgr) {
    if (methodname == "") {
        ferr << "no task specified for TRestTask!!!" << endl;
        exit(-1);
    } else {
        if (fMode == 0) {
            // call gInterpreter to run a command
            ConstructCommand();
            gInterpreter->ProcessLine(cmdstr.c_str());
            return;
        } else if (fMode == 1) {
            //
            if (mgr == NULL) {
                ferr << "no target specified for the command:" << endl;
                ferr << cmdstr << endl;
                exit(-1);
            } else {
                TRestMetadata* meta = mgr->GetApplicationWithName(targetname);
                if (meta == NULL) {
                    ferr << "cannot file metadata: " << targetname << " in TRestManager" << endl;
                    ferr << "command: " << cmdstr << endl;
                    exit(-1);
                } else {
                    string arg;
                    //////////////////////////////////
                    // consuruct arguments

                    gInterpreter->Execute(meta, meta->IsA(), methodname.c_str(), arg.c_str());
                }
            }
        }
    }
}

///////////////////////////////////////////////
/// \brief Defalut helper method both for TRestTask and any TRestTask-inherted
/// class
///
void TRestTask::PrintArgumentHelp() {
    if (fMode == 0) {
        ferr << GetName() << "() Gets invailed input!" << endl;
        ferr << "You should give the following arguments ( * : necessary input):" << endl;
        int n = argument.size();
        for (int i = 0; i < n; i++) {
            if (i < fNRequiredArgument) ferr << "*";
            ferr << argumentname[i] << endl;
        }
    } else if (fMode == 1) {
    } else if (fMode == 2) {
        ferr << "Macro class \"" << this->ClassName() << "\" gets invailed input!" << endl;
        ferr << "You should give the following arguments ( * : necessary input):" << endl;
        int n = GetNumberOfDataMembers(this);
        for (int i = 1; i < n; i++) {
            if (i < fNRequiredArgument + 1) ferr << "*";
            ferr << GetDataMember(this, i).name << endl;
        }
    }
}

///////////////////////////////////////////////
/// \brief Static method to instantiate a TRestTask object with "MacroName"
///
/// REST macros are saved in the directory ./macros. They follow a naming
/// logic, the naming style is like following:
/// REST_[MacroName].hh
///
/// e.g. REST_ViewEvents.hh. Here we must add a prefix "REST_" with a macro name
/// usually in verb form. When given the macro name, this method first calls
/// TClass::GetClass() to find if there is a TRestTask-inherted class which has
/// this name. If so, it returns the found class, if not, it finds a
/// corresponding macro file and calls gInterpreter to load it, and then
/// instaintiates a TRestTask class wrapping this file.
TRestTask* TRestTask::GetTask(TString MacroName) {
    string macfilelists =
        TRestTools::Execute("find $REST_PATH/macros -name *" + (string)MacroName + (string) ".*");
    auto macfiles = Split(macfilelists, "\n");

    if (macfiles.size() != 0 && macfiles[0] != "") {
        std::cout << "Found MacroFile " << macfiles[0] << std::endl;
        // system("echo \"#define REST_MANAGER\" > /tmp/tmpMacro.c");
        // system(("cat " + macfiles[0] + " >> /tmp/tmpMacro.c").c_str());
        if (gInterpreter->LoadFile(macfiles[0].c_str()) == 0) {
            auto tsk = new TRestTask(macfiles[0].c_str(), TASK_MACRO);
            // system("rm /tmp/tmpMacro.c");
            return tsk;
        }

    } else {
        // initialize from a class which is inherited from TRestTask
        TRestTask* tsk = REST_Reflection::Assembly((string)MacroName);
        if (tsk != NULL && tsk->InheritsFrom("TRestTask")) {
            tsk->SetMode(TASK_CLASS);
            return tsk;
        }
    }
    return NULL;
}

TRestTask* TRestTask::ParseCommand(TString cmd) {
    auto tsk = new TRestTask(cmd, TASK_CPPCMD);
    if (tsk->GetMode() == TASK_ERROR) {
        delete tsk;
        return NULL;
    } else {
        return tsk;
    }
}
