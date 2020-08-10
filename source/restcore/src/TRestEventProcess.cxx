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

#include "TBuffer.h"
#include "TClass.h"
#include "TDataMember.h"
#include "TRestManager.h"
#include "TRestRun.h"
using namespace std;

ClassImp(TRestEventProcess);

//////////////////////////////////////////////////////////////////////////
/// TRestEventProcess default constructor
///
TRestEventProcess::TRestEventProcess() {
    fObservableInfo.clear();
    fSingleThreadOnly = false;
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
    vector<string> obsnames;
    vector<string> obstypes;
    vector<string> obsdesc;

    while (e != NULL) {
        const char* obschr = e->Attribute("name");
        const char* _value = e->Attribute("value");
        const char* _type = e->Attribute("type");
        const char* _desc = e->Attribute("description");

        string value;
        if (_value == NULL)
            value = "ON";
        else
            value = _value;
        string type;
        if (_type == NULL)
            type = "double";
        else
            type = _type;
        string description;
        if (_desc == NULL)
            description = "";
        else
            description = _desc;

        if (ToUpper(value) == "ON") {
            if (obschr != NULL) {
                debug << this->ClassName() << " : setting observable \"" << obschr << "\"" << endl;
                // vector<string> tmp = Split(obsstring, ":");
                obsnames.push_back(obschr);
                obstypes.push_back(type);
                obsdesc.push_back(description);
            }
        }

        e = e->NextSiblingElement("observable");

    }  // now we get a list of all observal names, we add them into fAnalysisTree and fObservableInfo

    for (int i = 0; i < obsnames.size(); i++) {
        int id = fAnalysisTree->AddObservable((this->GetName() + (string) "_" + obsnames[i]).c_str(),
                                              obstypes[i], obsdesc[i]);
        if (id != -1) {
            fObservableInfo[(string)GetName() + "_" + obsnames[i]] = id;
        }
    }

    return obsnames;
}

void TRestEventProcess::SetAnalysisTree(TRestAnalysisTree* tree) {
    fAnalysisTree = tree;
    ConfigAnalysisTree();
}

//////////////////////////////////////////////////////////////////////////
/// \brief Add a process to the friendly process list.
///
/// Processes can get access to each other's parameter and observable
void TRestEventProcess::SetFriendProcess(TRestEventProcess* p) {
    if (p == NULL) return;
    for (int i = 0; i < fFriendlyProcesses.size(); i++) {
        if (fFriendlyProcesses[i]->GetName() == p->GetName()) return;
    }
    fFriendlyProcesses.push_back(p);
}

//////////////////////////////////////////////////////////////////////////
/// \brief Set branches for analysis tree according to the output level, read observables.
///
void TRestEventProcess::ConfigAnalysisTree() {
    if (fAnalysisTree == NULL) return;

    ReadObservables();
}

//////////////////////////////////////////////////////////////////////////
/// Interface to external file reading, open input file. To be implemented in external processes.
Bool_t TRestEventProcess::OpenInputFiles(vector<string> files) { return false; }

//////////////////////////////////////////////////////////////////////////
/// \brief Load extra section metadata: outputlevel after calling
/// TRestMetadata::LoadSectionMetadata()
///
Int_t TRestEventProcess::LoadSectionMetadata() {
    TRestMetadata::LoadSectionMetadata();

    if (ToUpper(GetParameter("observable", "")) == "ALL") {
        fDynamicObs = true;
    }

    // load cuts
    fCuts.clear();
    if (ToUpper(GetParameter("cutsEnabled", "false")) == "TRUE") {
        TiXmlElement* ele = fElement->FirstChildElement();
        while (ele != NULL) {
            if (ele->Value() != NULL && (string)ele->Value() == "cut") {
                if (ele->Attribute("name") != NULL && ele->Attribute("value") != NULL) {
                    string name = ele->Attribute("name");
                    name = (string) this->GetName() + "_" + name;
                    TVector2 value = StringTo2DVector(ele->Attribute("value"));
                    if (value.X() != value.Y()) fCuts.push_back(pair<string, TVector2>(name, value));
                }
            }

            else if (ele->Value() != NULL && (string)ele->Value() == "parameter") {
                if (ele->Attribute("name") != NULL && ele->Attribute("value") != NULL) {
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
/// GetMetadata("TRestReadout"), GetMetadata("readout_140")
TRestMetadata* TRestEventProcess::GetMetadata(string name) {
    TRestMetadata* m = fRunInfo->GetMetadata(name);
    if (m == NULL) m = fRunInfo->GetMetadataClass(name);
    return m;
}

//////////////////////////////////////////////////////////////////////////
/// \brief Get friendly TRestEventProcess object
///
/// Either name or type as input argument is accepted. For example,
/// GetFriend("TRestSignalZeroSupressionProcess"), GetFriend("zS").
/// Both lively process(in the process chain) and metadata process
/// (retrieved from input file) are searched.
///
/// With this method, processes can know what's the process chain is like,
/// and acts differently according to the added friends. For example, we can
/// retrieve some common parameters from the friend process. We can also re-use
/// the input/output event to compare the difference.
TRestEventProcess* TRestEventProcess::GetFriend(string nameortype) {
    TRestEventProcess* proc = GetFriendLive(nameortype);
    if (proc == NULL) {
        TRestMetadata* friendfromfile = GetMetadata(nameortype);
        if (friendfromfile != NULL && friendfromfile->InheritsFrom("TRestEventProcess")) {
            return (TRestEventProcess*)friendfromfile;
        }
        return NULL;
    } else {
        return proc;
    }
}

//////////////////////////////////////////////////////////////////////////
/// \brief Get friendly TRestEventProcess object
///
/// Only lively process(in the process chain) is searched.
TRestEventProcess* TRestEventProcess::GetFriendLive(string nameortype) {
    for (int i = 0; i < fFriendlyProcesses.size(); i++) {
        if ((string)fFriendlyProcesses[i]->GetName() == nameortype ||
            (string)fFriendlyProcesses[i]->ClassName() == nameortype) {
            return fFriendlyProcesses[i];
        }
    }
    return NULL;
}

//////////////////////////////////////////////////////////////////////////
/// \brief Apply cut according to the cut conditions saved in fCut
///
/// returns true if the event should be cut and not stored.
bool TRestEventProcess::ApplyCut() {
    for (auto cut : fCuts) {
        string type = (string)fAnalysisTree->GetObservableType(cut.first);
        if (fAnalysisTree != NULL && type == "double") {
            double val = fAnalysisTree->GetObservableValue<double>(cut.first);
            if (val > cut.second.Y() || val < cut.second.X()) {
                return true;
            }
        }
        if (fAnalysisTree != NULL && type == "int") {
            int val = fAnalysisTree->GetObservableValue<int>(cut.first);
            if (val > cut.second.Y() || val < cut.second.X()) {
                return true;
            }
        }
        return false;
    }
}

/*
//______________________________________________________________________________
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
    debug << "Entering " << ClassName() << "::BeginOfEventProcess, Initializing output event..." << endl;
    if (inEv != NULL && GetOutputEvent().address != NULL && (TRestEvent*)GetOutputEvent() != inEv) {
        TRestEvent* outEv = GetOutputEvent();
        outEv->Initialize();

        outEv->SetID(inEv->GetID());
        outEv->SetSubID(inEv->GetSubID());
        outEv->SetSubEventTag(inEv->GetSubEventTag());

        outEv->SetRunOrigin(inEv->GetRunOrigin());
        outEv->SetSubRunOrigin(inEv->GetSubRunOrigin());

        outEv->SetTime(inEv->GetTime());
    }

    // TODO if fIsExternal and we already have defined the fAnalysisTree run#,
    // evId#, timestamp, etc at the analysisTree we could stamp the output event
    // here.
}

/*
//______________________________________________________________________________
void TRestEventProcess::ProcessEvent( TRestEvent *eventInput )
{
// virtual function to be executed for every event to be processed
}
*/

//////////////////////////////////////////////////////////////////////////
/// \brief End of event process. Nothing to do. Called directly after ProcessEvent()
void TRestEventProcess::EndOfEventProcess(TRestEvent* evInput) {
    debug << "Entering TRestEventProcess::EndOfEventProcess (" << ClassName() << ")" << endl;
}

/*
//______________________________________________________________________________
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
    metadata.setcolor(COLOR_BOLDGREEN);
    metadata.setborder("||");
    metadata.setlength(100);
    // metadata << " " << endl;
    cout << endl;
    metadata << "=" << endl;
    metadata << "Process : " << ClassName() << endl;
    metadata << "Name: " << GetName() << "  Title: " << GetTitle()
             << "  VerboseLevel: " << GetVerboseLevelString() << endl;
    metadata << " ----------------------------------------------- " << endl;
    metadata << " " << endl;

    if (fObservableInfo.size() > 0) {
        metadata << " Analysis tree observables added by this process " << endl;
        metadata << " +++++++++++++++++++++++++++++++++++++++++++++++ " << endl;
    }

    auto iter = fObservableInfo.begin();
    while (iter != fObservableInfo.end()) {
        metadata << " ++ " << iter->first << endl;
        iter++;
    }

    if (fObservableInfo.size() > 0) {
        metadata << " +++++++++++++++++++++++++++++++++++++++++++++++ " << endl;
        metadata << " " << endl;
    }
}

//////////////////////////////////////////////////////////////////////////
/// \brief Pre-defined printer, can be used at the ending in the implementation
/// of PrintMetadata()
///
/// Prints several separators. Prints cuts.
void TRestEventProcess::EndPrintProcess() {
    if (fCuts.size() > 0) {
        metadata << "Cuts enabled" << endl;
        metadata << "------------" << endl;

        auto iter = fCuts.begin();
        while (iter != fCuts.end()) {
            if (iter->second.X() != iter->second.Y())
                metadata << iter->first << ", range : ( " << iter->second.X() << " , " << iter->second.Y()
                         << " ) " << endl;
            iter++;
        }
    }

    metadata << " " << endl;
    metadata << "=" << endl;
    metadata << endl;
    metadata.resetcolor();
    metadata.setborder("");
    metadata.setlength(10000);
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
    if (fHostmgr != NULL && fHostmgr->GetProcessRunner() != NULL)
        return fHostmgr->GetProcessRunner()->GetOutputAnalysisTree();
    return NULL;
}

//////////////////////////////////////////////////////////////////////////
/// Get list of observables, convert map to vector.
std::vector<string> TRestEventProcess::GetListOfAddedObservables() {
    vector<string> list;
    auto iter = fObservableInfo.begin();
    while (iter != fObservableInfo.end()) {
        list.push_back(iter->first);
        iter++;
    }
    return list;
}

void TRestEventProcess::ValidateObservables() {
    std::vector<string> definedObservables;
    for (auto const& x : fObservableInfo) {
        definedObservables.push_back(x.first);
    }

    std::vector<string> usedObservables;
    for (auto const& x : fObservableForValidation) {
        usedObservables.push_back(x.first);
    }

    std::vector<string> diff;
    std::set_difference(definedObservables.begin(), definedObservables.end(), usedObservables.begin(),
                        usedObservables.end(), std::inserter(diff, diff.begin()));

    for (auto const& x : diff) {
        ferr << "----" << endl;
        ferr << "The observable  '" << x << "' could not be identified as a valid observable of "
             << this->ClassName() << endl;
        ferr << "Please, verify the corresponding <observable definition inside the process." << endl;
        ferr << "The observable names are case sensitive. Please, double-check the name is properly "
                "formed."
             << endl;
        ferr << "----" << endl;
        GetChar();
    }
}
