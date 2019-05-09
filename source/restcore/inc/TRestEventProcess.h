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

#include "TCanvas.h"
#include "TNamed.h"

#include "TRestAnalysisTree.h"
#include "TRestEvent.h"
#include "TRestMetadata.h"
#include "TRestRun.h"

class TRestGas;
class TRestReadout;
class TRestG4Metadata;
class TRestDetectorSetup;

/// A base class for any REST event process
class TRestEventProcess : public TRestMetadata {
   protected:
    enum REST_Process_Output {
        No_Output,     //!< creates no branch in analysis tree, user can still manually save what he wants.
        Observable,    //!< +saving observables as branches in root directory of the tree
        Internal_Var,  //!< +saving the process itself as a branch. branch name is the process name. Internal
                       //!< variables can be found as leaf of the branch
        Full_Output,   //!< +saving output event as another branch. branch name is process name+"_evtBranch"
    };

    TRestEvent* fInputEvent = NULL;   //!///< Pointer to input event
    TRestEvent* fOutputEvent = NULL;  //!///< Pointer to output event

    TCanvas* fCanvas = NULL;  //!
    TVector2 fCanvasSize;     //!

    TRestAnalysisTree* fAnalysisTree = NULL;  //!///< Pointer to analysis tree where to store the observables.
    TRestRun* fRunInfo = NULL;                //!

    bool fIsExternal = false;  //!///< It defines if the process reads event data from an external source.
    bool fSingleThreadOnly = false;  //!///< It defines if the process can run only under single thread
    bool fReadOnly = false;          //!
    bool fDynamicObs = false;        //!

    REST_Process_Output fOutputLevel;  //!

    vector<pair<string, TVector2>> fCuts;           //!  [name, cut range]
    map<string, int> fObservableInfo;               //!     [name, id in AnalysisTree]
    vector<TRestEventProcess*> fFriendlyProcesses;  //!

    // utils
    void BeginPrintProcess();
    void EndPrintProcess();
    TRestMetadata* GetMetadata(string type);
    TRestMetadata* GetGasMetadata() { return GetMetadata("TRestGas"); }
    TRestMetadata* GetReadoutMetadata() { return GetMetadata("TRestReadout"); }
    TRestMetadata* GetGeant4Metadata() { return GetMetadata("TRestG4Metadata"); }
    TRestMetadata* GetDetectorSetup() { return GetMetadata("TRestDetectorSetup"); }
    Double_t GetDoubleParameterFromClass(string className, string parName);
    Double_t GetDoubleParameterFromClassWithUnits(string className, string parName);

    //////////////////////////////////////////////////////////////////////////
    /// \brief Set observable value for analysistree.
    ///
    /// recommended as it is more efficienct than calling
    /// fAnalysisTree->SetObservableValue( obsName, obsValue )
    template <class T>
    void SetObservableValue(string name, const T& value) {
        if (fAnalysisTree != NULL) {
            string obsname = this->GetName() + (string) "_" + (string)name;
            if (fObservableInfo.count(obsname) != 0) {
                fAnalysisTree->SetObservableValue(fObservableInfo[obsname], value);
            } else if (fDynamicObs && fObservableInfo.count(obsname) == 0) {
                // create new branch for this observable
                int n = fAnalysisTree->AddObservable<T>(obsname);
                if (n != -1) {
                    fObservableInfo[obsname] = n;
                    fAnalysisTree->SetObservableValue(fObservableInfo[obsname], value);
                }
            }
        }
    }

    void CreateCanvas() {
        if (fCanvas != NULL) return;

        fCanvas = new TCanvas(this->GetName(), this->GetTitle(), fCanvasSize.X(), fCanvasSize.Y());
    }

    void TransferEvent(TRestEvent* evOutput, TRestEvent* evInput) {
        if (evInput == NULL) return;
        if (evOutput != NULL) {
            // copy without changing address
            evInput->CloneTo(evOutput);
        } else {
            // clone and set the address of output event
            evOutput = (TRestEvent*)evInput->Clone();
        }
    }

   public:
    Int_t LoadSectionMetadata();
    vector<string> ReadObservables();

    virtual Bool_t OpenInputFiles(vector<string> files) { return false; }

    virtual void InitProcess() {}  ///< To be executed at the beginning of the run

    void BeginOfEventProcess(TRestEvent* evInput = NULL);  ///< To be executed before processing event

    virtual TRestEvent* ProcessEvent(TRestEvent* evInput) = 0;  ///< Process one event

    void EndOfEventProcess(TRestEvent* evInput = NULL);  ///< To be executed after processing event

    virtual void EndProcess() {}  ///< To be executed at the end of the run

    virtual void ConfigAnalysisTree();

    // setters
    void SetOutputLevel(REST_Process_Output lvl) {
        fOutputLevel = lvl;
        if (fOutputLevel < Observable) fReadOnly = true;
    }
    void SetAnalysisTree(TRestAnalysisTree* tree);
    void SetRunInfo(TRestRun* r) { fRunInfo = r; }
    void SetCanvasSize(Int_t x, Int_t y) { fCanvasSize = TVector2(x, y); }
    void SetFriendProcess(TRestEventProcess* p);

    // getters
    REST_Process_Output GetOutputLevel() { return fOutputLevel; }
    virtual TRestEvent* GetInputEvent() { return fInputEvent; }    ///< Get pointer to input event
    virtual TRestEvent* GetOutputEvent() { return fOutputEvent; }  ///< Get pointer to output event
    virtual Long64_t GetTotalBytes() { return -1; }
    virtual Long64_t GetTotalBytesReaded() { return 0; }
    Bool_t singleThreadOnly() { return fSingleThreadOnly; }
    Bool_t isExternal() { return fIsExternal; }
    TRestRun* GetRunInfo() { return fRunInfo; }
    TRestAnalysisTree* GetAnalysisTree() { return fAnalysisTree; }
    TCanvas* GetCanvas() { return fCanvas; }
    std::vector<string> GetListOfAddedObservables();

    // Constructor
    TRestEventProcess();
    // Destructor
    ~TRestEventProcess();

    ClassDef(TRestEventProcess, 2);  // Base class for a REST process
};
#endif
