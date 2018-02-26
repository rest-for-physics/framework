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

#ifndef RestCore_TRestEventProcess
#define RestCore_TRestEventProcess

#include "TNamed.h"
#include "TCanvas.h"


#include "TRestEvent.h"
#include "TRestMetadata.h"
#include "TRestAnalysisTree.h"
#include "TRestRun.h"


class TRestGas;
class TRestReadout;
class TRestG4Metadata;
class TRestDetectorSetup;

class TRestEventProcess :public TRestMetadata {
public:

	enum REST_Process_Output
	{
		No_Output,//!< no autosave for any variables, user can still manually save what he wants.
		Observable,//!< +saving observables in the root directory of the tree
		Internal_Var,//!< +saving internal variables as branches
		Full_Output,//!< +saving output event together in the branch
	};

	//Constructor
	TRestEventProcess();
	//Destructor
	~TRestEventProcess();

	ClassDef(TRestEventProcess, 1);      // Base class for a REST process

	Int_t LoadSectionMetadata();
	vector<string> ReadObservables();

	virtual Bool_t OpenInputFiles(vector<TString> files) { return false; }

	virtual void InitProcess() { } ///< To be executed at the beginning of the run
	virtual void BeginOfEventProcess() { }
	virtual TRestEvent *ProcessEvent(TRestEvent *evInput) = 0; ///< Process one event
	virtual void EndOfEventProcess() { } ///< To be executed after processing event
	virtual void EndProcess() { } ///< To be executed at the end of the run
	
	virtual void ConfigAnalysisTree();
								  
	//setters
	void SetOutputLevel(REST_Process_Output lvl) { fOutputLevel = lvl; if (fOutputLevel < Observable)fReadOnly = true; }
	void SetAnalysisTree(TRestAnalysisTree *tree);
	void SetRunInfo(TRestRun*r) { fRunInfo = r; }
	void SetCanvasSize(Int_t x, Int_t y) { fCanvasSize = TVector2(x, y); }
	void SetFriendProcess(TRestEventProcess*p);

	//getters
	REST_Process_Output GetOutputLevel() { return fOutputLevel; }
	virtual TRestEvent *GetInputEvent() { return fInputEvent; } ///< Get pointer to input event
	virtual TRestEvent *GetOutputEvent() { return fOutputEvent; } ///< Get pointer to output event
	Bool_t singleThreadOnly() { return fSingleThreadOnly; }
	TRestRun* GetRunInfo() { return fRunInfo; }
	vector<string> GetAvailableObservals();
	TRestAnalysisTree *GetAnalysisTree() { return fAnalysisTree; }
	TCanvas *GetCanvas() { return fCanvas; }



protected:

	TRestEvent *fInputEvent = NULL;	//!///< Pointer to input event
	TRestEvent *fOutputEvent = NULL;    //!///< Pointer to output event

	TCanvas *fCanvas=NULL;//!
	TVector2 fCanvasSize;//!

	TRestAnalysisTree *fAnalysisTree = NULL; //!///< Pointer to analysis tree where to store the observables. 

	Bool_t fSingleThreadOnly; //!///< It defines if the process reads event data from an external source.

	REST_Process_Output fOutputLevel;//!

	TRestRun* fRunInfo = NULL;//!

	vector<TRestEventProcess*> fFriendlyProcesses;//!

	std::vector <TString> fObservableNames;
	//std::vector <Double_t*> fObservableRefs;//!

	bool fReadOnly=false;//!





	//utils
	void BeginPrintProcess();
	void EndPrintProcess();
	TRestMetadata *GetMetadata(string type);
	TRestMetadata *GetGasMetadata() { return GetMetadata("TRestGas"); }
	TRestMetadata *GetReadoutMetadata() { return GetMetadata("TRestReadout"); }
	TRestMetadata *GetGeant4Metadata() { return GetMetadata("TRestG4Metadata"); }
	TRestMetadata *GetDetectorSetup() { return GetMetadata("TRestDetectorSetup"); }
	Double_t GetDoubleParameterFromClass(TString className, TString parName);
	Double_t GetDoubleParameterFromClassWithUnits(TString className, TString parName);
	void StampOutputEvent(TRestEvent *inEv);
	void CreateCanvas()
	{
		if (fCanvas != NULL) return;

		fCanvas = new TCanvas(this->GetName(), this->GetTitle(), fCanvasSize.X(), fCanvasSize.Y());
	}
	
	void TransferEvent(TRestEvent*evOutput, TRestEvent*evInput)
	{
		if (evInput == NULL)return;
		if (evOutput != NULL)
		{
			//copy without changing address
			evInput->CloneTo(evOutput);
		}
		else
		{
			//clone and set the address of output event
			evOutput = (TRestEvent*)evInput->Clone();
		}
		
	}


};
#endif
