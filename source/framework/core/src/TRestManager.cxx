//////////////////////////////////////////////////////////////////////////
///
/// This class provides a runtime for other REST application class. Its
/// rml element is usually the root element in the file, inside which
/// other REST application class elements are defined. TRestManager
/// instantiates class objects according to the rml file and performs
/// sequential startup for them. Then it runs specific tasks, which is
/// also defined following the applications, to do the jobs. The tasks
/// are handled by class TRestTask.
///
/// \class TRestManager
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
/// 2017-Aug:  Major change to xml reading and class startup procedure
///            Kaixiang Ni
///
/// <hr>
//////////////////////////////////////////////////////////////////////////

#include "TRestManager.h"

#include "TInterpreter.h"
#include "TRestTask.h"
#include "TSystem.h"

using namespace std;
ClassImp(TRestManager);

TRestManager::TRestManager() { Initialize(); }

TRestManager::~TRestManager() {}

///////////////////////////////////////////////
/// \brief Set the class name as section name during initialization.
///
void TRestManager::Initialize() {
    SetSectionName(this->ClassName());

    fMetaObjects.clear();

    fMetaObjects.push_back(this);
}

int TRestManager::LoadSectionMetadata() { return TRestMetadata::LoadSectionMetadata(); }

///////////////////////////////////////////////
/// \brief Respond to the input xml element.
///
/// If the declaration of the input element is:
/// 1. TRestXXX: Startup the TRestXXX class with this xml element(sequential
/// startup).
/// 2. AddTask: Do some special operations for the managed application
/// TRestMetadata class
///
/// Suppported tasks:
/// 1. processEvents, analysisPlot, saveMetadata: directly do the jobs
/// 2. name of a REST macro: instaintate TRestTask to parse it, then run this
/// TRestTask.
/// 3. C++ style command: call gInterpreter to execute it.
/// Other types of declarations will be omitted.
///
Int_t TRestManager::ReadConfig(string keydeclare, TiXmlElement* e) {
    // if (keydeclare == "TRestRun") {
    //	TRestRun* fRunInfo = new TRestRun();
    //	fRunInfo->SetHostmgr(this);
    //	fRunInfo->LoadConfigFromFile(e, fElementGlobal);
    //	fMetaObjects.push_back(fRunInfo);
    //	gROOT->Add(fRunInfo);
    //	return 0;
    //}

    // else if (keydeclare == "TRestProcessRunner") {
    //	TRestProcessRunner* fProcessRunner = new TRestProcessRunner();
    //	fProcessRunner->SetHostmgr(this);
    //	fProcessRunner->LoadConfigFromFile(e, fElementGlobal);
    //	fMetaObjects.push_back(fProcessRunner);
    //	gROOT->Add(fProcessRunner);
    //	return 0;
    //}

    // cout << "----------- " << gROOT->FindObject("SJTU_Proto") << endl;

    if (Count(keydeclare, "TRest") > 0) {
        TRestMetadata* meta = REST_Reflection::Assembly(keydeclare);
        if (meta == nullptr) return -1;
        meta->SetHostmgr(this);
        fMetaObjects.push_back(meta);
        meta->SetConfigFile(fConfigFileName);
        meta->LoadConfigFromElement(e, fElementGlobal, fVariables);

        return 0;
    }

    else if (keydeclare == "addTask") {
        string active = GetParameter("value", e, "");
        if (active != "ON" && active != "On" && active != "on") {
            RESTDebug << "Inactived task : \"" << ElementToString(e) << "\"" << RESTendl;
            return 0;
        }
        RESTDebug << "Loading Task...";

        const char* type = e->Attribute("type");
        const char* cmd = e->Attribute("command");
        if (type == nullptr && cmd == nullptr) {
            RESTWarning << "command or type should be given!" << RESTendl;
            return -1;
        }
        if (type != nullptr) {
            RESTDebug << " \"" << type << "\" " << RESTendl;
            if ((string)type == "processEvents") {
                auto pr = GetProcessRunner();
                if (pr != nullptr) pr->RunProcess();
            } else if ((string)type == "analysisPlot") {
                auto ap = GetAnaPlot();
                if (ap != nullptr) ap->PlotCombinedCanvas();
            } else if ((string)type == "saveMetadata") {
                auto ri = GetRunInfo();
                if (ri != nullptr) {
                    ri->FormOutputFile();
                    ri->CloseFile();
                }
            } else {
                TRestTask* tsk = TRestTask::GetTaskFromMacro(type);
                if (tsk == nullptr) {
                    RESTWarning << "REST ERROR. Task : " << type << " not found!!" << RESTendl;
                    RESTWarning << "This task will be skipped." << RESTendl;
                    return -1;
                }
                tsk->LoadConfigFromElement(e, fElementGlobal);
                tsk->RunTask(this);
                return 0;
            }
        } else if (cmd != nullptr) {
            RESTDebug << " \"" << cmd << "\" " << RESTendl;

            TRestTask* tsk = TRestTask::GetTaskFromCommand(cmd);
            if (tsk == nullptr) {
                RESTWarning << "REST ERROR. Command : " << cmd << " cannot be parsed!!" << RESTendl;
                RESTWarning << "This task will be skipped." << RESTendl;
                return -1;
            }
            tsk->RunTask(this);
            return 0;
        }
    }

    return -1;
}

void TRestManager::InitFromTask(string taskName, vector<string> arguments) {
    TRestTask* tsk = TRestTask::GetTaskFromMacro(taskName);
    if (tsk == nullptr) {
        gSystem->Exit(-1);
    }
    tsk->SetArgumentValue(arguments);
    tsk->RunTask(nullptr);
    gSystem->Exit(0);
}

///////////////////////////////////////////////
/// \brief Get the application metadata class, according to the type
///
TRestMetadata* TRestManager::GetMetadataClass(string type) {
    for (int i = 0; i < fMetaObjects.size(); i++) {
        if ((string)fMetaObjects[i]->ClassName() == type) {
            return fMetaObjects[i];
        }
    }
    return nullptr;
}

///////////////////////////////////////////////
/// \brief Get the application metadata class, according to the name
///
TRestMetadata* TRestManager::GetMetadata(string name) {
    for (int i = 0; i < fMetaObjects.size(); i++) {
        if (fMetaObjects[i]->GetName() == name) {
            return fMetaObjects[i];
        }
    }
    return nullptr;
}

///////////////////////////////////////////////
/// \brief PrintMetadata of this class
///
/// Not implemented.
///
void TRestManager::PrintMetadata() { return; }
