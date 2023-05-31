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
///
/// One of the core classes of REST. Absract class
/// from which all REST "event process classes" must derive.
/// An event process in REST is an atomic operation on one single
/// (input) event, potentially producing a different (output) event.
/// Which type of (input/output) events will depent on the type of
/// process. Processes can be tailored via metadata classes.
/// All this needs to be defined in the inherited class.
/// TRestEventProcess provides the basic structure (virtual functions)
///
/// \class TRestEventProcess
///
///--------------------------------------------------------------------------
///
/// RESTsoft - Software for Rare Event Searches with TPCs
///
/// History of developments:
///
/// 2014-june: First concept. As part of conceptualization of previous REST
///       code (REST v2)
///       Igor G. Irastorza
///
/// 2017-Aug:  Major change: added for multi-thread capability
///            Kaixiang Ni
///
/// <hr>
//////////////////////////////////////////////////////////////////////////

#include "TRestEventProcess.h"

#include <TBuffer.h>
#include <TClass.h>
#include <TDataMember.h>

#include "TRestManager.h"
#include "TRestRun.h"

using namespace std;

ClassImp(TRestEventProcess);

//////////////////////////////////////////////////////////////////////////
/// TRestEventProcess default constructor
///
TRestEventProcess::TRestEventProcess() {
    fObservablesDefined.clear();
    fObservablesUpdated.clear();
    fSingleThreadOnly = false;
    fCanvasSize = TVector2(800, 600);
}

//////////////////////////////////////////////////////////////////////////
/// TRestEventProcess destructor
///
TRestEventProcess::~TRestEventProcess() {}

//////////////////////////////////////////////////////////////////////////
/// \brief Loop child sections declared as "observable" and set them in the
/// analysis tree
///
/// \code <observable name="abc" value="ON"/>
/// There are two types of observables: datamember observable and realtime
/// observable. Datamember observable has the same name with a class-defined
/// data member. The address of this data member will be found and be used to
/// crate the tree branch. On the other hand, realtime observable is only
/// defined during the analysis process. We need to call the method
/// TRestAnalysisTree::SetObservableValue() after the value is calculated.
///
/// After all the processes have finished the event, observable branches will
/// automatically be filled.
///
/// Note that realtime observables can only be double type. If one wants to save
/// other types of analysis result, e.g. vector or map, he needs to use
/// datamember observables.
vector<string> TRestEventProcess::ReadObservables() {
    TiXmlElement* e = GetElement("observable");
    vector<string> obsNames;
    vector<string> obsTypes;
    vector<string> obsDesc;

    while (e != nullptr) {
        const char* obschr = e->Attribute("name");
        const char* _value = e->Attribute("value");
        const char* _type = e->Attribute("type");
        const char* _desc = e->Attribute("description");

        string value;
        if (_value == nullptr)
            value = "ON";
        else
            value = _value;
        string type;
        if (_type == nullptr)
            type = "double";
        else
            type = _type;
        string description;
        if (_desc == nullptr)
            description = "";
        else
            description = _desc;

        if (ToUpper(value) == "ON") {
            if (obschr != nullptr) {
                RESTDebug << this->ClassName() << " : setting observable \"" << obschr << "\"" << RESTendl;
                // vector<string> tmp = Split(obsstring, ":");
                obsNames.emplace_back(obschr);
                obsTypes.push_back(type);
                obsDesc.push_back(description);
            }
        }

        e = e->NextSiblingElement("observable");

    }  // now we get a list of all observal names, we add them into fAnalysisTree and fObservableInfo

    for (unsigned int i = 0; i < obsNames.size(); i++) {
        string obsName = this->GetName() + (string) "_" + obsNames[i];
        fAnalysisTree->AddObservable(obsName, obsTypes[i], obsDesc[i]);
        int id = fAnalysisTree->GetObservableID(obsName);
        if (id != -1) {
            fObservablesDefined[(string)GetName() + "_" + obsNames[i]] = id;
        }
    }

    return obsNames;
}

void TRestEventProcess::SetAnalysisTree(TRestAnalysisTree* tree) {
    fAnalysisTree = tree;
    if (fAnalysisTree == nullptr) return;
    ReadObservables();
}

//////////////////////////////////////////////////////////////////////////
/// \brief Add a process to the friendly process list.
///
/// Processes can get access to each other's parameter and observable
void TRestEventProcess::SetFriendProcess(TRestEventProcess* p) {
    if (p == nullptr) return;
    for (const auto& fFriendlyProcess : fFriendlyProcesses) {
        if (fFriendlyProcess->GetName() == p->GetName()) {
            return;
        }
    }
    fFriendlyProcesses.push_back(p);
}

/// Add parallel process to this process
void TRestEventProcess::SetParallelProcess(TRestEventProcess* p) {
    if (p == nullptr) return;
    for (const auto& fParallelProcess : fParallelProcesses) {
        if (fParallelProcess == p || this == p) {
            return;
        }
    }
    fParallelProcesses.push_back(p);
}

//////////////////////////////////////////////////////////////////////////
/// Interface to external file reading, open input file. To be implemented in external processes.
Bool_t TRestEventProcess::OpenInputFiles(const vector<string>& files) { return false; }

//////////////////////////////////////////////////////////////////////////
/// \brief Load extra section metadata: outputlevel after calling
/// TRestMetadata::LoadSectionMetadata()
///
Int_t TRestEventProcess::LoadSectionMetadata() {
    TRestMetadata::LoadSectionMetadata();

    if (ToUpper(GetParameter("observable", "")) == "ALL" ||
        ToUpper(GetParameter("observables", "")) == "ALL") {
        fDynamicObs = true;
    }

    // load cuts
    fCuts.clear();
    if (ToUpper(GetParameter("cutsEnabled", "false")) == "TRUE") {
        TiXmlElement* ele = fElement->FirstChildElement();
        while (ele != nullptr) {
            if (ele->Value() != nullptr && (string)ele->Value() == "cut") {
                if (ele->Attribute("name") != nullptr && ele->Attribute("value") != nullptr) {
                    string name = ele->Attribute("name");
                    name = (string)this->GetName() + "_" + name;
                    TVector2 value = StringTo2DVector(ele->Attribute("value"));
                    if (value.X() != value.Y()) fCuts.push_back(pair<string, TVector2>(name, value));
                }
            }

            else if (ele->Value() != nullptr && (string)ele->Value() == "parameter") {
                if (ele->Attribute("name") != nullptr && ele->Attribute("value") != nullptr) {
                    string name = ele->Attribute("name");
                    if (name.find("Cut") == name.size() - 3 || name.find("CutRange") == name.size() - 8) {
                        name = name.substr(0, name.find("Cut") + 3);
                        TVector2 value = StringTo2DVector(ele->Attribute("value"));
                        if (value.X() != value.Y()) fCuts.push_back(pair<string, TVector2>(name, value));
                    }
                }
            }

            ele = ele->NextSiblingElement();
        }
    }

    return 0;
}

//////////////////////////////////////////////////////////////////////////
/// \brief Get a metadata object from the host TRestRun
///
/// Either name or type as input argument is accepted. For example,
/// GetMetadata("TRestDetectorReadout"), GetMetadata("readout_140")
TRestMetadata* TRestEventProcess::GetMetadata(const string& name) {
    TRestMetadata* m = nullptr;
    if (fRunInfo != nullptr) {
        m = fRunInfo->GetMetadata(name);
        if (m == nullptr) m = fRunInfo->GetMetadataClass(name);
    }
    if (fHostmgr != nullptr) {
        if (m == nullptr) m = fHostmgr->GetMetadata(name);
        if (m == nullptr) m = fHostmgr->GetMetadataClass(name);
    }
    return m;
}

//////////////////////////////////////////////////////////////////////////
/// \brief Get friendly TRestEventProcess object
///
/// Either name or type as input argument is accepted. For example,
/// GetFriend("TRestDetectorSignalZeroSupressionProcess"), GetFriend("zS").
/// Both lively process(in the process chain) and metadata process
/// (retrieved from input file) are searched.
///
/// With this method, processes can know what's the process chain is like,
/// and acts differently according to the added friends. For example, we can
/// retrieve some common parameters from the friend process. We can also re-use
/// the input/output event to compare the difference.
TRestEventProcess* TRestEventProcess::GetFriend(const string& nameOrType) {
    TRestEventProcess* proc = GetFriendLive(nameOrType);
    if (proc == nullptr) {
        TRestMetadata* friendfromfile = GetMetadata(nameOrType);
        if (friendfromfile != nullptr && friendfromfile->InheritsFrom("TRestEventProcess")) {
            return (TRestEventProcess*)friendfromfile;
        }
        return nullptr;
    } else {
        return proc;
    }
}

//////////////////////////////////////////////////////////////////////////
/// \brief Get friendly TRestEventProcess object
///
/// Only lively process(in the process chain) is searched.
TRestEventProcess* TRestEventProcess::GetFriendLive(const string& nameOrType) {
    for (const auto& fFriendlyProcess : fFriendlyProcesses) {
        if ((string)fFriendlyProcess->GetName() == nameOrType ||
            (string)fFriendlyProcess->ClassName() == nameOrType) {
            return fFriendlyProcess;
        }
    }
    return nullptr;
}

//////////////////////////////////////////////////////////////////////////
/// \brief Get a list of parallel processes from this process
///
/// Parallel process means the process in other threads. It differs from "friend process"
/// in another dimension. For example, we set up the process chain with one
/// `TRestRawSignalAnalysisProcess` and one `TRestRawToSignalProcess`, and calls 2 threads to
/// run the data. Then, for `TRestRawSignalAnalysisProcess` in thread 1, it has a parallel
/// process `TRestRawSignalAnalysisProcess` from thread 2, and a friend process
/// `TRestRawToSignalProcess` from thread 1.
TRestEventProcess* TRestEventProcess::GetParallel(int i) {
    if (i >= 0 && i < (int)fParallelProcesses.size()) {
        return fParallelProcesses[i];
    }
    return nullptr;
}

//////////////////////////////////////////////////////////////////////////
/// \brief Apply cut according to the cut conditions saved in fCut
///
/// returns true if the event should be cut and not stored.
bool TRestEventProcess::ApplyCut() {
    for (auto cut : fCuts) {
        string type = (string)fAnalysisTree->GetObservableType(cut.first);
        if (fAnalysisTree != nullptr && type == "double") {
            double val = fAnalysisTree->GetObservableValue<double>(cut.first);
            if (val > cut.second.Y() || val < cut.second.X()) {
                return true;
            }
        }
        if (fAnalysisTree != nullptr && type == "int") {
            int val = fAnalysisTree->GetObservableValue<int>(cut.first);
            if (val > cut.second.Y() || val < cut.second.X()) {
                return true;
            }
        }
    }
    return false;
}

/*

void TRestEventProcess::InitProcess()
{
// virtual function to be executed once at the beginning of process
// (before starting the process of the events)
cout << GetName() << ": Process initialization..." << endl;
}
*/

//////////////////////////////////////////////////////////////////////////
/// \brief Begin of event process, preparation work. Called right before ProcessEvent()
///
/// This method is called before calling ProcessEvent(). We initialize the process's output event if not
/// null
/// and not same as input event. The event's basic info (ID, timestamp, etc.) will also be set to the same
/// as
/// input event
void TRestEventProcess::BeginOfEventProcess(TRestEvent* inEv) {
    RESTDebug << "Entering " << ClassName() << "::BeginOfEventProcess, Initializing output event..."
              << RESTendl;
    if (inEv != nullptr && GetOutputEvent().address != nullptr && (TRestEvent*)GetOutputEvent() != inEv) {
        TRestEvent* outEv = GetOutputEvent();
        outEv->Initialize();

        outEv->SetID(inEv->GetID());
        outEv->SetSubID(inEv->GetSubID());
        outEv->SetSubEventTag(inEv->GetSubEventTag());

        outEv->SetRunOrigin(inEv->GetRunOrigin());
        outEv->SetSubRunOrigin(inEv->GetSubRunOrigin());

        outEv->SetTime(inEv->GetTime());
    }

    fObservablesUpdated.clear();

    // TODO if fIsExternal and we already have defined the fAnalysisTree run#,
    // evId#, timestamp, etc at the analysisTree we could stamp the output event
    // here.
}

/*

void TRestEventProcess::ProcessEvent( TRestEvent *inputEvent )
{
// virtual function to be executed for every event to be processed
}
*/

//////////////////////////////////////////////////////////////////////////
/// \brief End of event process. Validate the updated observable number matches total defined observable
/// number
void TRestEventProcess::EndOfEventProcess(TRestEvent* inputEvent) {
    RESTDebug << "Entering TRestEventProcess::EndOfEventProcess (" << ClassName() << ")" << RESTendl;
    if (fValidateObservables) {
        if (fObservablesDefined.size() != fObservablesUpdated.size()) {
            for (auto x : fObservablesDefined) {
                if (fObservablesUpdated.count(x.first) == 0) {
                    // the observable is added through <observable section but not set in the process
                    RESTWarning
                        << "The observable  '" << x.first << "' is defined but not set by "
                        << this->ClassName()
                        << ", the event is empty? Makesure all observables are set through ProcessEvent()"
                        << RESTendl;
                }
            }
        }
    }
}

/*

void TRestEventProcess::EndProcess()
{
// virtual function to be executed once at the end of the process
// (after all events have been processed)
cout << GetName() << ": Process ending..." << endl;
}
*/

//////////////////////////////////////////////////////////////////////////
/// \brief Pre-defined printer, can be used at the beginning in the
/// implementation of PrintMetadata()
///
/// Prints process type, name, title, verboselevel, outputlevel, input/output
/// event type, and several separators
void TRestEventProcess::BeginPrintProcess() {
    RESTMetadata.setcolor(COLOR_BOLDGREEN);
    RESTMetadata.setborder("||");
    RESTMetadata.setlength(100);
    // metadata << " " << endl;
    cout << endl;
    RESTMetadata << "=" << RESTendl;
    RESTMetadata << "Process : " << ClassName() << RESTendl;
    RESTMetadata << "Name: " << GetName() << "  Title: " << GetTitle()
                 << "  VerboseLevel: " << GetVerboseLevelString() << RESTendl;
    RESTMetadata << " ----------------------------------------------- " << RESTendl;
    RESTMetadata << " " << RESTendl;

    if (!fObservablesDefined.empty()) {
        RESTMetadata << " Analysis tree observables added by this process " << RESTendl;
        RESTMetadata << " +++++++++++++++++++++++++++++++++++++++++++++++ " << RESTendl;
    }

    auto iter = fObservablesDefined.begin();
    while (iter != fObservablesDefined.end()) {
        RESTMetadata << " ++ " << iter->first << RESTendl;
        iter++;
    }

    if (!fObservablesDefined.empty()) {
        RESTMetadata << " +++++++++++++++++++++++++++++++++++++++++++++++ " << RESTendl;
        RESTMetadata << " " << RESTendl;
    }
}

//////////////////////////////////////////////////////////////////////////
/// \brief Pre-defined printer, can be used at the ending in the implementation
/// of PrintMetadata()
///
/// Prints several separators. Prints cuts.
void TRestEventProcess::EndPrintProcess() {
    if (fCuts.size() > 0) {
        RESTMetadata << "Cuts enabled" << RESTendl;
        RESTMetadata << "------------" << RESTendl;

        auto iter = fCuts.begin();
        while (iter != fCuts.end()) {
            if (iter->second.X() != iter->second.Y())
                RESTMetadata << iter->first << ", range : ( " << iter->second.X() << " , " << iter->second.Y()
                             << " ) " << RESTendl;
            iter++;
        }
    }

    RESTMetadata << " " << RESTendl;
    RESTMetadata << "=" << RESTendl;
    RESTMetadata << RESTendl;
    RESTMetadata.resetcolor();
    RESTMetadata.setborder("");
    RESTMetadata.setlength(10000);
}

////////////////////////////////////////////////////////////////////////////
///// \brief Retrieve parameter with name "parName" from friendly process
///// "className"
/////
///// \param className string with name of metadata class to access
///// \param parName  string with name of parameter to retrieve
/////
// Double_t TRestEventProcess::GetDoubleParameterFromFriends(string className, string parName) {
//    for (size_t i = 0; i < fFriendlyProcesses.size(); i++)
//        if ((string)fFriendlyProcesses[i]->ClassName() == (string)className)
//            return StringToDouble(fFriendlyProcesses[i]->GetParameter((string)parName));
//
//    return PARAMETER_NOT_FOUND_DBL;
//}
//
////////////////////////////////////////////////////////////////////////////
///// \brief Retrieve parameter with name "parName" from friendly process
///// "className", with units
/////
///// \param className string with name of metadata class to access
///// \param parName  string with name of parameter to retrieve
/////
// Double_t TRestEventProcess::GetDoubleParameterFromFriendsWithUnits(string className, string parName) {
//    for (size_t i = 0; i < fFriendlyProcesses.size(); i++)
//        if ((string)fFriendlyProcesses[i]->ClassName() == (string)className)
//            return fFriendlyProcesses[i]->GetDblParameterWithUnits((string)parName);
//
//    return PARAMETER_NOT_FOUND_DBL;
//}

//////////////////////////////////////////////////////////////////////////
/// Get the full analysis tree from TRestRun, with non-empty entries.
TRestAnalysisTree* TRestEventProcess::GetFullAnalysisTree() {
    if (fHostmgr != nullptr && fHostmgr->GetProcessRunner() != nullptr)
        return fHostmgr->GetProcessRunner()->GetOutputAnalysisTree();
    return nullptr;
}

//////////////////////////////////////////////////////////////////////////
/// Get list of observables, convert map to vector.
std::vector<string> TRestEventProcess::GetListOfAddedObservables() {
    vector<string> list;
    auto iter = fObservablesDefined.begin();
    while (iter != fObservablesDefined.end()) {
        list.push_back(iter->first);
        iter++;
    }
    return list;
}
