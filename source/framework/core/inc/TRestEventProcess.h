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

   private:
    // unused datamember, set as private
    ///< not used, keep for compatibility
    TRestEvent* fInputEvent = nullptr;  //!
    ///< not used, keep for compatibility
    TRestEvent* fOutputEvent = nullptr;  //!
    /// not used, keep for compatibility
    REST_Process_Output fOutputLevel;  //!/// not used

    /// Stores a list of friendly processes. Sometimes the process may behave differently
    /// according to the friend processes added. It can also get parameter or output event
    /// from friend processes
    std::vector<TRestEventProcess*> fFriendlyProcesses;  //!
    /// Stores a list of parallel processes if multithread is enabled
    std::vector<TRestEventProcess*> fParallelProcesses;  //!
   protected:
    ///< Canvas for some viewer event
    TCanvas* fCanvas = nullptr;  //!
    /// Canvas size
    TVector2 fCanvasSize;  //!
    ///< Pointer to the analysis tree where to store observable definitions and values.
    /// Note that this tree does not write data to disk. It only keeps a list of observables
    /// and their addresses. It is the one from TRestRun that reads this tree's structure,
    /// calls Fill() method, and writes data to disk. In other words, this tree is dummy and
    /// has zero entries. To get the real one, use GetFullAnalysisTree()
    TRestAnalysisTree* fAnalysisTree = nullptr;  //!
    ///< Pointer to TRestRun object where to find metadata.
    TRestRun* fRunInfo = nullptr;  //!
    /// It defines if the process reads event data from an external source.
    bool fIsExternal = false;  //!
    /// It defines if the process can run only under single std::thread mode. If true, the whole process
    /// chain will not use multithread. Useful for processes with viewing functionality. Always true for
    /// external processes.
    bool fSingleThreadOnly = false;  //!
    /// not used, keep for compatibility
    bool fReadOnly = false;  //!
    /// It defines whether to use added observables only or all the observables appear in the code.
    bool fDynamicObs = false;  //!
    /// It defines if observable names should be added to the validation list
    bool fValidateObservables = false;  //!
    /// Stores the list of process observables updated when processing this event
    std::map<std::string, int> fObservablesUpdated;  //!     [name, id in AnalysisTree]
    /// Stores the list of all the appeared process observables in the code
    std::map<std::string, int> fObservablesDefined;  //!     [name, id in AnalysisTree]
    /// Stores cut definitions. Any listed observables should be in the range.
    std::vector<std::pair<std::string, TVector2>> fCuts;  //!  [name, cut range]

    // utils
    void BeginPrintProcess();
    void EndPrintProcess();
    //////////////////////////////////////////////////////////////////////////
    /// \brief Get a metadata object from the host TRestRun
    ///
    /// Directly input the type as template argument. This helps to simplify the code and prevents
    /// mis-spelling. For example: `fReadout = GetMetadata<TRestReadout>();`. No need for type
    /// conversion.
    template <class T>
    T* GetMetadata() {
        std::string type = REST_Reflection::GetTypeName<T>();
        return (T*)GetMetadata(type);
    }
    TRestMetadata* GetMetadata(std::string nameortype);
    TRestEventProcess* GetFriend(std::string nameortype);
    TRestEventProcess* GetFriendLive(std::string nameortype);
    int GetNumberOfParallelProcesses() { return fParallelProcesses.size(); }
    TRestEventProcess* GetParallel(int i);
    //////////////////////////////////////////////////////////////////////////
    /// \brief Set observable value for analysistree.
    ///
    /// It will rename the observable to "processName_obsName"
    /// If use dynamic observable, it will try to create new observable
    /// in the AnalysisTree if the observable is not found
    template <class T>
    void SetObservableValue(std::string name, const T& value) {
        if (fAnalysisTree != nullptr) {
            std::string obsname = this->GetName() + (std::string) "_" + (std::string)name;

            if (fValidateObservables) {
                int id = fAnalysisTree->GetObservableID(obsname);
                if (id != -1) {
                    fObservablesDefined[obsname] = id;
                    fObservablesUpdated[obsname] = id;
                    fAnalysisTree->SetObservable(obsname, value);
                } else if (fDynamicObs) {
                    fAnalysisTree->SetObservable(obsname, value);
                    int n = fAnalysisTree->GetObservableID(obsname);
                    if (n != -1) {
                        fObservablesDefined[obsname] = id;
                        fObservablesUpdated[obsname] = id;
                    }
                }
            } else {
                int id = fAnalysisTree->GetObservableID(obsname);
                if (id != -1) {
                    fAnalysisTree->SetObservableValue(id, value);
                } else if (fDynamicObs) {
                    fAnalysisTree->SetObservableValue(obsname, value);
                }
            }
            // if (fObservableInfo.count(obsname) != 0) {
            //    if (fValidateObservables) fObservableForValidation[obsname] = fObservableInfo[obsname];
            //    fAnalysisTree->SetObservableValue(fObservableInfo[obsname], value);
            //} else if (fDynamicObs && fObservableInfo.count(obsname) == 0) {
            //    // create new branch for this observable
            //    int n = fAnalysisTree->AddObservable<T>(obsname);
            //    if (n != -1) {
            //        fObservableInfo[obsname] = n;
            //        if (fValidateObservables) fObservableForValidation[obsname] = fObservableInfo[obsname];
            //        fAnalysisTree->SetObservableValue(fObservableInfo[obsname], value);
            //    }
            //}
        }
    }

    /// Create the canvas
    void CreateCanvas() {
        if (fCanvas != nullptr) return;
        fCanvas = new TCanvas(this->GetName(), this->GetTitle(), fCanvasSize.X(), fCanvasSize.Y());
    }

    bool ApplyCut();

   public:
    Int_t LoadSectionMetadata();
    virtual void InitFromConfigFile() {
        std::map<std::string, std::string> parameters = GetParametersList();

        for (auto& p : parameters)
            p.second = ReplaceMathematicalExpressions(fRunInfo->ReplaceMetadataMembers(p.second));

        ReadParametersList(parameters);
    }
    std::vector<std::string> ReadObservables();
    // open a list of input files to be processed, only used if is external process
    virtual Bool_t OpenInputFiles(std::vector<std::string> files);
    // add an input file during process
    virtual Bool_t AddInputFile(std::string file) { return false; }
    // reset the entry by moving file ptr to 0 with fseek
    virtual Bool_t ResetEntry() { return false; }

    void SetObservableValidation(bool validate) { fValidateObservables = validate; }
    // void EnableObservableValidation() { fValidateObservables = true; }
    // void DisableObservableValidation() { fValidateObservables = false; }

    // process running methods
    /// To be executed at the beginning of the run (outside event loop)
    virtual void InitProcess() {}
    /// Begin of event process, preparation work. Called right before ProcessEvent()
    virtual void BeginOfEventProcess(TRestEvent* evInput = nullptr);
    /// Process one event
    virtual TRestEvent* ProcessEvent(TRestEvent* evInput) = 0;
    /// End of event process. Nothing to do. Called directly after ProcessEvent()
    virtual void EndOfEventProcess(TRestEvent* evInput = nullptr);
    /// To be executed at the end of the run (outside event loop)
    virtual void EndProcess() {}

    // setters
    /// Set analysis tree of this process, then add observables to it
    void SetAnalysisTree(TRestAnalysisTree* tree);
    /// Set TRestRun for this process
    void SetRunInfo(TRestRun* r) { fRunInfo = r; }
    /// Set canvas size
    void SetCanvasSize(Int_t x, Int_t y) { fCanvasSize = TVector2(x, y); }
    /// Add friendly process to this process
    void SetFriendProcess(TRestEventProcess* p);
    /// Add parallel process to this process
    void SetParallelProcess(TRestEventProcess* p);

    // getters
    /// Get pointer to input event. Must be implemented in the derived class
    virtual RESTValue GetInputEvent() = 0;
    /// Get pointer to output event. Must be implemented in the derived class
    virtual RESTValue GetOutputEvent() = 0;
    /// Interface to external file reading, get the total bytes of input binary file. To be implemented in
    /// external processes.
    virtual Long64_t GetTotalBytes() { return -1; }
    /// Interface to external file reading, get the readed bytes. To be implemented in external processes.
    virtual Long64_t GetTotalBytesReaded() { return 0; }
    /// Return whether this process is single std::thread only
    Bool_t singleThreadOnly() { return fSingleThreadOnly; }
    /// Return whether this process is external process
    Bool_t isExternal() { return fIsExternal; }
    /// Return the pointer of the hosting TRestRun object
    TRestRun* GetRunInfo() { return fRunInfo; }
    /// Return the local analysis tree (dummy)
    TRestAnalysisTree* GetAnalysisTree() { return fAnalysisTree; }
    TRestAnalysisTree* GetFullAnalysisTree();
    /// Get canvas
    TCanvas* GetCanvas() { return fCanvas; }
    std::vector<std::string> GetListOfAddedObservables();

    // Constructor
    TRestEventProcess();
    // Destructor
    ~TRestEventProcess();

    ClassDef(TRestEventProcess, 3);  // Base class for a REST process
};
#endif
