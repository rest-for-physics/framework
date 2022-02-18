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

#include "TFunction.h"
#include "TMethodArg.h"
#include "TRestManager.h"
#include "TRestReflector.h"
#include "TRestStringHelper.h"

ClassImp(TRestTask);

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

    if (mode == TASK_MACRO) {
        // we parse the macro file, get the method's name and argument list
        string filename = TRestTools::SeparatePathAndName((string)TaskString).second;
        int n = filename.find_last_of('.');
        string funcName = "";
        if (n > 0) {
            funcName = filename.substr(0, n);
        } else {
            funcName = TaskString;
        }
        TFunction* f = gROOT->GetGlobalFunction(funcName.c_str());
        if (f == nullptr) {
            fMode = TASK_ERROR;
        } else {
            fNRequiredArgument = f->GetNargs() - f->GetNargsOpt();
            fInvokeMethod = funcName;
            // indicates whether the argument is string/TString/const char *. If so, the value would be 1. We
            // need to add "" mark when constructing command. Otherwise the value is 0.

            TList* list = f->GetListOfMethodArgs();
            for (int i = 0; i < list->GetSize(); i++) {
                TMethodArg* arg = (TMethodArg*)list->At(i);
                string type = arg->GetTypeName();
                fArgumentTypes.push_back(type == "string" || type == "char" || type == "TString");
                fArgumentNames.push_back(arg->GetName());
            }
            string fConstructedCommand = "";
        }

    } else if (mode == TASK_CPPCMD) {
        // we parse the command, get the target object, method to be invoked, and
        // the argument list
        string cmd = (string)TaskString;

        string name;
        string call;
        if (Split(cmd, "->").size() != 2) {
            warning << "command"
                    << " \"" << cmd << "\" "
                    << "is illegal!" << endl;
            fMode = TASK_ERROR;
            return;
        } else {
            name = Split(cmd, "->")[0];
            call = Split(cmd, "->")[1];
        }

        int p1 = call.find_first_of("(");
        int p2 = call.find_last_of(")");
        if (p1 == -1 || p2 == -1 || p1 >= p2) {
            warning << "command"
                    << " \"" << cmd << "\" "
                    << "is illegal!" << endl;
            fMode = TASK_ERROR;
            return;
        }
        fInvokeObject = name;
        fInvokeMethod = call.substr(0, p1);

        string args = call.substr(p1 + 1, p2 - p1 - 1);
        fArgumentValues = Split(args, ",");
        fConstructedCommand = cmd;
    } else if (mode == TASK_CLASS) {
        // I don't think we can get here
    } else if (mode == TASK_SHELLCMD) {
        fConstructedCommand = TaskString;
        fInvokeMethod = Split(fConstructedCommand, " ")[0];
    }
}

///////////////////////////////////////////////
/// \brief Starter method. Looks through the rml sections and set
/// argument/datamenber value
///
void TRestTask::InitFromConfigFile() { ReadAllParameters(); }

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
    fArgumentValues = arg;
}

///////////////////////////////////////////////
/// \brief Run the task with command line
///
void TRestTask::RunTask(TRestManager* mgr) {
    if (fInvokeMethod == "") {
        ferr << "no task specified for TRestTask!!!" << endl;
        exit(-1);
    } else {
        if (fMode == TASK_MACRO) {
            // call gInterpreter to run a command
            for (int i = 0; i < fArgumentValues.size(); i++) {
                if (fArgumentValues[i] == "NOT SET") {
                    ferr << "TRestTask : argument " << i << " not set! Task will not run!" << endl;
                }
            }

            fConstructedCommand = fInvokeMethod + "(";
            for (int i = 0; i < fArgumentValues.size(); i++) {
                if (fArgumentTypes[i] == 1) {
                    fConstructedCommand += "\"" + fArgumentValues[i] + "\"";
                } else {
                    fConstructedCommand += fArgumentValues[i];
                }

                if (i < fArgumentValues.size() - 1) fConstructedCommand += ",";
            }
            fConstructedCommand += ")";

            cout << "Command : " << fConstructedCommand << endl;

            gInterpreter->ProcessLine(fConstructedCommand.c_str());
            return;
        } else if (fMode == TASK_CPPCMD) {
            //
            if (mgr == nullptr) {
                ferr << "no target specified for the command:" << endl;
                ferr << fConstructedCommand << endl;
                exit(-1);
            } else {
                TRestMetadata* meta = mgr->GetMetadata(fInvokeObject);
                if (meta == nullptr) {
                    ferr << "cannot file metadata: " << fInvokeObject << " in TRestManager" << endl;
                    ferr << "command: " << fConstructedCommand << endl;
                    exit(-1);
                } else {
                    string type = meta->ClassName();
                    string cmd = Form("%s* %s = (%s*)%s;", type.c_str(), fInvokeObject.c_str(), type.c_str(),
                                      ToString(meta).c_str());

                    TInterpreter::EErrorCode err;
                    gInterpreter->ProcessLine(cmd.c_str(), &err);
                    if (err != TInterpreter::kNoError) {
                        ferr << "TRestTask::RunTask(): unknown error" << endl;
                        ferr << "code: " << err << endl;
                        exit(-1);
                    }
                    gInterpreter->ProcessLine(fConstructedCommand.c_str(), &err);
                    if (err != TInterpreter::kNoError) {
                        ferr << "TRestTask: failed to execute cpp command, error code: " << err << endl;
                        ferr << fConstructedCommand << endl;
                        ferr << "Check your <AddTask section!" << endl;
                        exit(-1);
                    }
                }
            }
        } else if (fMode == TASK_SHELLCMD) {
            system(fConstructedCommand.c_str());
        }
    }
}

///////////////////////////////////////////////
/// \brief Defalut helper method both for TRestTask and any TRestTask-inherted
/// class
///
void TRestTask::PrintArgumentHelp() {
    if (fMode == 0) {
        ferr << fInvokeMethod << "() Gets invailed input!" << endl;
        cout << "You should give the following arguments (* is mandatory input):" << endl;
        int n = fArgumentNames.size();
        for (int i = 0; i < n; i++) {
            cout << (i < fNRequiredArgument ? "*" : "") << fArgumentNames[i] << endl;
        }
    } else if (fMode == 1) {
    } else if (fMode == 2) {
        ferr << "Macro class \"" << this->ClassName() << "\" gets invailed input!" << endl;
        ferr << "You should give the following arguments ( * : necessary input):" << endl;
        int n = any(this).GetNumberOfDataMembers();
        for (int i = 1; i < n; i++) {
            if (i < fNRequiredArgument + 1) ferr << "*";
            ferr << any(this).GetDataMember(i).name << endl;
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
TRestTask* TRestTask::GetTaskFromMacro(TString taskName) {
    string macfilelists =
        TRestTools::Execute("find $REST_PATH/macros -name *" + (string)taskName + (string) ".*");
    auto macfiles = Split(macfilelists, "\n");

    if (macfiles.size() != 0 && macfiles[0] != "") {
        info << "Found MacroFile " << macfiles[0] << noClass::endl;
        // system("echo \"#define REST_MANAGER\" > /tmp/tmpMacro.c");
        // system(("cat " + macfiles[0] + " >> /tmp/tmpMacro.c").c_str());
        if (gInterpreter->LoadFile(macfiles[0].c_str()) == 0) {
            TRestTask* tsk = new TRestTask(macfiles[0].c_str(), TASK_MACRO);
            if (tsk->GetMode() == TASK_ERROR) {
                ferr << "Task file: " << macfiles[0]
                     << " loaded but method not found. Make sure it contains the method with same name as "
                        "file name"
                     << noClass::endl;
                return NULL;
            }
            return tsk;
        } else {
            ferr << "Task file: " << macfiles[0] << " contains error" << noClass::endl;
            return NULL;
        }

    } else {
        // initialize from a class which is inherited from TRestTask
        TRestTask* tsk = REST_Reflection::Assembly((string)taskName);
        if (tsk != nullptr && tsk->InheritsFrom("TRestTask")) {
            tsk->SetMode(TASK_CLASS);
            return tsk;
        }
    }
    ferr << "REST ERROR. Task : " << taskName << " not found!!" << noClass::endl;
    return NULL;
}

TRestTask* TRestTask::GetTaskFromCommand(TString cmd) {
    REST_TASKMODE mode = TASK_CPPCMD;
    if (((string)cmd).find("->") == -1) mode = TASK_SHELLCMD;

    auto tsk = new TRestTask(cmd, mode);
    if (tsk->GetMode() == TASK_ERROR) {
        delete tsk;
        return NULL;
    } else {
        return tsk;
    }
}
