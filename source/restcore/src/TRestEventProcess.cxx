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
#include "TRestRun.h"
#include "TClass.h"
#include "TDataMember.h"
#include "TBuffer.h"
using namespace std;

ClassImp(TRestEventProcess)

//////////////////////////////////////////////////////////////////////////
/// TRestEventProcess default constructor

TRestEventProcess::TRestEventProcess()
{
	fObservableNames.clear();
	fSingleThreadOnly = false;
}

//////////////////////////////////////////////////////////////////////////
/// TRestEventProcess destructor

TRestEventProcess::~TRestEventProcess()
{
}

//////////////////////////////////////////////////////////////////////////
/// \brief Loop child sections declared as "observable" and set them in the analysis tree
///
/// \code <observable name="abc" value="ON"/>
/// There are two types of observables: datamember observable and realtime observable
/// datamember observable has the same name with a class-defined data member. The address
/// of this data member will be found and be used to crate the tree branch. On the other hand,
/// realtime observable is only defined during the analysis process. We need to call the 
/// method TRestAnalysisTree::SetObservableValue() after the value is calculated.
///
/// After all the processes have finished the event, observable branches will automatically
/// be filled.
///
/// Note that observables can only be double type. If one wants to save other types of 
/// analysis result, e.g. vector or map, he can only define them as process's data member 
/// and save the wholel process in analysis tree.
vector<string> TRestEventProcess::ReadObservables()
{
	TiXmlElement* e = GetElement("observable");
	vector<string> obsnames;

	while (e != NULL)
	{
		const char* obschr = e->Attribute("name");
		const char* _value = e->Attribute("value");
		string value;
		if (_value == NULL)value = "ON";
		else { value = _value; }
		if (value == "ON" || value == "On" || value == "on") {
			if (obschr != NULL) {
				string obsstring = (string)obschr;
				debug << this->ClassName() << " : setting following observables " << obsstring << endl;
				vector<string> tmp = Spilt(obsstring, ":");
				obsnames.insert(obsnames.end(), tmp.begin(), tmp.end());
			}
		}

		e = e->NextSiblingElement("observable");
	}//now we get a list of all observal names
	if (fObservableNames.size() != 0) {
		return obsnames;
	}


	//if fObservableNames is empty, add observables.
	//1. observable is datamember of the process class
	//then the address of this datamember is found, and associated to a branch
	//it will be automatically saved at the end of each process loop
	//2. observable is not datamember of the process class
	//then REST will create a new double value as observable.
	//the user needs to call fAnalysisTree->SetObservableValue( obsName, obsValue ) during each process
	
	for (int i = 0; i < obsnames.size(); i++) {
		TStreamerElement* se = GetDataMemberWithName(obsnames[i]);
		if (se != NULL)
		{
			if (se->GetType() != 8) {
				warning << "Only double type datamember can be used as observable, \""<<obsnames[i]<<"\" will be skipped" << endl;
				continue;
			}
			if (fAnalysisTree->AddObservable((TString)GetName() + "_" + obsnames[i], (double*)GetDataMemberRef(se))!=-1)
				fObservableNames.push_back((TString)GetName() + "_" + obsnames[i]);
		}
		else
		{
			double*d = new double();
			if (fAnalysisTree->AddObservable((TString)GetName() + "_" + obsnames[i], d)!=-1)
			{
				fObservableNames.push_back((TString)GetName() + "_" + obsnames[i]);
			}
			else
			{
				delete d;
			}
		}
	}

	return obsnames;
}

//////////////////////////////////////////////////////////////////////////
/// \brief Set analysis tree of this process
///
void TRestEventProcess::SetAnalysisTree(TRestAnalysisTree *tree)
{
	debug << "setting analysis tree for " << this->ClassName() << endl;
	fAnalysisTree = tree;

	
}

//////////////////////////////////////////////////////////////////////////
/// \brief Add a process to the friendly process list.
///
/// Processes can get access to each other's parameter and observable
void TRestEventProcess::SetFriendProcess(TRestEventProcess*p) 
{
	if (p == NULL)
		return;
	for (int i = 0; i < fFriendlyProcesses.size(); i++) {
		if (fFriendlyProcesses[i]->GetName() == p->GetName())
			return;
	}
	fFriendlyProcesses.push_back(p);
}

//////////////////////////////////////////////////////////////////////////
/// \brief Set branches for analysis tree according to the output level
///
void TRestEventProcess::ConfigAnalysisTree() {
	if (fAnalysisTree == NULL)return;

	if (fOutputLevel >= Observable)ReadObservables();
	if (fOutputLevel >= Internal_Var)fAnalysisTree->Branch(this->GetName(), this);
	if (fOutputLevel >= Full_Output) fAnalysisTree->Branch(this->GetName() + (TString)"_evtBranch", GetOutputEvent());
	
}

//////////////////////////////////////////////////////////////////////////
/// \brief Load extra section metadata: outputlevel after calling TRestMetadata::LoadSectionMetadata()
///
Int_t TRestEventProcess::LoadSectionMetadata()
{
	TRestMetadata::LoadSectionMetadata();

	//load output level
	REST_Process_Output lvl;
	string s = GetParameter("outputLevel", "internalvar");
	if (s == "nooutput" || s == "0") {
		lvl = No_Output;
	}
	else if (s == "observable" || s == "1")
	{
		lvl = Observable;
	}
	else if (s == "internalvar" || s == "2")
	{
		lvl = Internal_Var;
	}
	else if (s == "fulloutput" || s == "3")
	{
		lvl = Full_Output;
	}
	else
	{
		warning << this->ClassName()<<" : invailed output level! use default(Internal_Var)" << endl;
	}
	SetOutputLevel(lvl);

	//load cuts
	fCuts.clear();
	if (ToUpper(GetParameter("cutsEnabled", "false")) == "TRUE") {
		TiXmlElement*ele = fElement->FirstChildElement();
		while (ele != NULL) {
			if (ele->Value() != NULL && (string)ele->Value() == "cut")
			{
				if (ele->Attribute("name") != NULL && ele->Attribute("value") != NULL) {
					string name = ele->Attribute("name");
					TVector2 value = StringTo2DVector(ele->Attribute("value"));
					if (value.X() != value.Y())
						fCuts.push_back(pair<string, TVector2>(name, value));
				}
			}

			else if (ele->Value() != NULL && (string)ele->Value() == "parameter")
			{
				if (ele->Attribute("name") != NULL && ele->Attribute("value") != NULL) {
					string name = ele->Attribute("name");
					if (name.find("Cut") == name.size() - 3 || name.find("CutRange") == name.size() - 8) {
						name = name.substr(0, name.find("Cut") + 3);
						TVector2 value = StringTo2DVector(ele->Attribute("value"));
						if (value.X() != value.Y())
							fCuts.push_back(pair<string, TVector2>(name, value));
					}
				}
			}

			ele = ele->NextSiblingElement();
		}
	}

	return 0;
}

//////////////////////////////////////////////////////////////////////////
/// \brief Get a metadata object from friendly TRestRun object
///
TRestMetadata *TRestEventProcess::GetMetadata(string name)
{
	TRestMetadata*m= fRunInfo->GetMetadata(name);
	if (m == NULL)m = fRunInfo->GetMetadataClass(name);
	return m;
}

//////////////////////////////////////////////////////////////////////////
/// \brief Get a list of available datamember-observables in the class
///
/// This method uses GetDataMemberWithID() from TRestMetadata.
vector<string> TRestEventProcess::GetAvailableObservals()
{
	vector<string> result;
	result.clear();
	int n = GetNumberOfDataMember();
	for (int i = 1; i < n; i++) {
		TStreamerElement *se = GetDataMemberWithID(i);
		if (se->GetType() == 8)
		{
			result.push_back(se->GetFullName());
		}
	}
	return result;
}

//////////////////////////////////////////////////////////////////////////
/// \brief Copy six basic event information items to fOutputEvent from "inEv"
///
void TRestEventProcess::StampOutputEvent(TRestEvent *inEv)
{
	fOutputEvent->Initialize();

	fOutputEvent->SetID(inEv->GetID());
	fOutputEvent->SetSubID(inEv->GetSubID());
	fOutputEvent->SetSubEventTag(inEv->GetSubEventTag());

	fOutputEvent->SetRunOrigin(inEv->GetRunOrigin());
	fOutputEvent->SetSubRunOrigin(inEv->GetSubRunOrigin());

	fOutputEvent->SetTime(inEv->GetTime());
}

/*
//______________________________________________________________________________
void TRestEventProcess::InitProcess()
{
// virtual function to be executed once at the beginning of process
// (before starting the process of the events)
cout << GetName() << ": Process initialization..." << endl;
}

//______________________________________________________________________________
void TRestEventProcess::ProcessEvent( TRestEvent *eventInput )
{
// virtual function to be executed for every event to be processed
}

//______________________________________________________________________________
void TRestEventProcess::EndProcess()
{
// virtual function to be executed once at the end of the process
// (after all events have been processed)
cout << GetName() << ": Process ending..." << endl;
}
*/

//////////////////////////////////////////////////////////////////////////
/// \brief Pre-defined printer, can be used at the beginning in the implementation of PrintMetadata()
///
/// Prints process type, name, title, verboselevel, outputlevel, input/output 
/// event type, and several separators
void TRestEventProcess::BeginPrintProcess()
{
	essential.setcolor(COLOR_BOLDGREEN);
	essential.setborder("||");
	essential.setlength(100);
	essential << endl;
	essential << "=" << endl;
	essential << "Process : " << ClassName() << endl;
	essential << "Name: " << GetName() << "  Title: " << GetTitle() << "  VerboseLevel: " << GetVerboseLevelString() << endl;
	essential << " ----------------------------------------------- " << endl;
	essential << " " << endl;

	if (fObservableNames.size() > 0)
	{
		essential << " Analysis tree observables added by this process " << endl;
		essential << " +++++++++++++++++++++++++++++++++++++++++++++++ " << endl;
	}

	for (unsigned int i = 0; i < fObservableNames.size(); i++)
	{
		essential << " ++ " << fObservableNames[i] << endl;
	}

	if (fObservableNames.size() > 0)
	{
		essential << " +++++++++++++++++++++++++++++++++++++++++++++++ " << endl;
		essential << " " << endl;
	}
}

//////////////////////////////////////////////////////////////////////////
/// \brief Pre-defined printer, can be used at the ending in the implementation of PrintMetadata()
///
/// Prints several separators
void TRestEventProcess::EndPrintProcess()
{
	essential << " " << endl;
	essential << "=" << endl;
	essential << endl;
	essential << endl;
	essential.resetcolor();
	essential.setborder("");
	essential.setlength(10000);
}

//////////////////////////////////////////////////////////////////////////
/// \brief Retrieve parameter with name "parName" from friendly process "className"
///
/// \param className string with name of metadata class to access
/// \param parName  string with name of parameter to retrieve
///
Double_t TRestEventProcess::GetDoubleParameterFromClass(TString className, TString parName)
{
	for (size_t i = 0; i < fFriendlyProcesses.size(); i++)
		if ((string)fFriendlyProcesses[i]->ClassName() == (string)className)
			return StringToDouble(fFriendlyProcesses[i]->GetParameter((string)parName));

	return PARAMETER_NOT_FOUND_DBL;
}

//////////////////////////////////////////////////////////////////////////
/// \brief Retrieve parameter with name "parName" from friendly process "className", with units
///
/// \param className string with name of metadata class to access
/// \param parName  string with name of parameter to retrieve
///
Double_t TRestEventProcess::GetDoubleParameterFromClassWithUnits(TString className, TString parName)
{
	for (size_t i = 0; i < fFriendlyProcesses.size(); i++)
		if ((string)fFriendlyProcesses[i]->ClassName() == (string)className)
			return fFriendlyProcesses[i]->GetDblParameterWithUnits((string)parName);

	return PARAMETER_NOT_FOUND_DBL;
}
