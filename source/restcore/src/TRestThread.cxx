
///
/// Multiple instances of TRestThread is created inside TRestProcessRunner.
/// Each of them can detach a thread containing a process chain, which
/// implements multi thread functionality. Preparation of process chain
/// is also done inside this class.
///
/// \class TRestThread
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
/// 2017-Aug:  Major change: added for multi-thread capability
///            Kaixiang Ni
///
/// <hr>
//////////////////////////////////////////////////////////////////////////

#include "TRestThread.h"

#ifdef TIME_MEASUREMENT
#include <chrono>
using namespace chrono;
#endif

ClassImp(TRestThread);

///////////////////////////////////////////////
/// \brief Set variables by default during initialization.
///
void TRestThread::Initialize() {
    fOutputEvent = NULL;
    fInputEvent = NULL;

    fAnalysisTree = NULL;
    fEventTree = NULL;

    fOutputFile = NULL;

    fProcessChain.clear();

    isFinished = false;
}

///////////////////////////////////////////////
/// \brief Check if the output of **input** matches the input event of process
/// chain.
///
/// Sometimes there is an external process ahead of the threaded processes. We
/// need to check if the event type matches.
///
Int_t TRestThread::ValidateInput(TRestEvent* input) {
    if (input == NULL && fProcessChain[0]->GetInputEvent() != NULL) {
        cout << "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++"
                "++++++++++"
             << endl;
        cout << "REST ERROR (ValidateInput): Given input event is null!" << endl;
        cout << "If you have a external process, make sure its output event is "
                "instantiated"
             << endl;
        cout << "in the Initialize() function." << endl;
        cout << "If you are using rest file as imput, make sure its contains event "
                "tree."
             << endl;
        cout << "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++"
                "++++++++++"
             << endl;
        GetChar();
        return -1;
    }
    // general event input is accepted
    if ((string)fProcessChain[0]->GetInputEvent()->ClassName() == "TRestEvent") return 0;
    if (input->ClassName() != fProcessChain[0]->GetInputEvent()->ClassName()) {
        cout << "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++"
                "++++++++++"
             << endl;
        cout << "REST ERROR (ValidateInput): Input event type does not match the "
                "input of the "
             << endl;
        cout << "first non-external process in the chain!" << endl;
        cout << "The gievn input type: " << input->ClassName() << endl;
        cout << "Input of the first non-external process in the chain: "
             << fProcessChain[0]->GetInputEvent()->ClassName() << endl;
        cout << "No events will be processed. Please correct event process "
                "input/output."
             << endl;
        cout << "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++"
                "++++++++++"
             << endl;
        GetChar();
        return -1;
    }
    return 0;
}

///////////////////////////////////////////////
/// \brief Check if the input/output of each process in the process chain
/// matches
///
/// The outevent of the previous process should be of the same type as the input
/// event of the later process. Both of them cannot be null.
///
Int_t TRestThread::ValidateChain() {
    for (unsigned int i = 0; i < fProcessChain.size() - 1; i++) {
        TRestEvent* outEvent = fProcessChain[i]->GetOutputEvent();
        TRestEvent* inEvent = fProcessChain[i + 1]->GetInputEvent();

        if (outEvent == NULL) {
            cout << "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++"
                    "++++++++++++"
                 << endl;
            cout << "REST ERROR : output Event of " << fProcessChain[i]->GetName()
                 << " has not been initialized!" << endl;
            cout << "You should instantiate the i-o event into type of TRestEvent in "
                    "the Initialize() function"
                 << endl;
            cout << "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++"
                    "++++++++++++"
                 << endl;
            GetChar();
            return -1;
        }
        if (inEvent == NULL) {
            cout << "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++"
                    "++++++++++++"
                 << endl;
            cout << "REST ERROR : input Event of " << fProcessChain[i + 1]->GetName()
                 << " has not been initialized!" << endl;
            cout << "You should instantiate the i-o event into type of TRestEvent in "
                    "the Initialize() function"
                 << endl;
            cout << "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++"
                    "++++++++++++"
                 << endl;
            GetChar();
            return -1;
        }
        TString outEventType = outEvent->ClassName();
        TString inEventType = inEvent->ClassName();
        // general input type
        if (inEventType == "TRestEvent" || outEventType == "TRestEvent") return 0;
        if (outEventType != inEventType) {
            cout << "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++"
                    "++++++++++++"
                 << endl;
            cout << "REST ERROR : Event process input/output does not match" << endl;
            cout << "The event output for process " << fProcessChain[i]->GetName() << " is " << outEventType
                 << endl;
            cout << "The event input for process " << fProcessChain[i + 1]->GetName() << " is " << inEventType
                 << endl;
            cout << "No events will be processed. Please correct event process "
                    "input/output."
                 << endl;
            cout << "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++"
                    "++++++++++++"
                 << endl;
            GetChar();
            return -1;
        }
    }
    return 0;
}

void TRestThread::SetThreadId(Int_t id) {
    fThreadId = id;
    if (fThreadId != 0 && fVerboseLevel > REST_Essential) fVerboseLevel = REST_Essential;
}

///////////////////////////////////////////////
/// \brief Make a test run of our process chain
///
/// The reason we use test run is that we need to determine the real output
/// event address of a process chain. This is because when we write our code
/// like this: \code TRestEvent* TRestRawSignalAnalysisProcess::ProcessEvent(
/// TRestEvent *evInput )
/// {
/// 	fSignalEvent = (TRestRawSignalEvent *)evInput;
/// 	fOutputEvent = fSignalEvent;
///		...
/// }
/// \endcode
/// The output event address can be changed after running.
///
/// Test run calls TRestEventProcess::ProcessEvent() first, and uses the output
/// of TRestEventProcess::GetOutputEvent() as the next process's input event.
/// This avoids NULL returned events from ProcessEvent due to cut. In debug
/// mode, we can also observe a process sequence printed by this method
///
/// returns false when fOutputEvent is null after 5 times of retry, returns true
/// when fOutputEvent address is determined.
bool TRestThread::TestRun(TRestAnalysisTree* tempTree) {
    debug << "Processing ..." << endl;
    for (int i = 0; i < 5; i++) {
        TRestEvent* ProcessedEvent = fInputEvent;
        debug << "Test run " << i << " : Input Event ---- " << fInputEvent->ClassName() << "(" << fInputEvent
              << ")" << endl;
        for (unsigned int j = 0; j < fProcessChain.size(); j++) {
            fProcessChain[j]->BeginOfEventProcess(ProcessedEvent);
            fProcessChain[j]->ProcessEvent(ProcessedEvent);
            ProcessedEvent = fProcessChain[j]->GetOutputEvent();
            if (ProcessedEvent == NULL) {
                debug << "  ----  NULL" << endl;
                break;
            }
            fProcessChain[j]->EndOfEventProcess();
            debug << " ....  " << ProcessedEvent->ClassName() << "(" << ProcessedEvent << ")" << endl;
        }

        fOutputEvent = ProcessedEvent;
        fHostRunner->GetNextevtFunc(fInputEvent, tempTree);
        if (fOutputEvent != NULL) {
            debug << "Output Event ---- " << fOutputEvent->ClassName() << "(" << fOutputEvent << ")" << endl;
            break;
        } else {
            debug << "Null output, trying again" << endl;
        }
    }
    if (fOutputEvent == NULL) {
        // fOutputEvent = fProcessChain[fProcessChain.size() - 1]->GetOutputEvent();
        return false;
    }
    return true;
}

///////////////////////////////////////////////
/// \brief Propare some thing before we can satrt process
///
/// This method will:
/// 1. Setup the processes in process chain:(set analysis tree, set readonly,
/// call the method InitProcess())
/// 2. Instantiate the input event and make a test run, calling TestRun(). The
/// address of output event is fixed after the test run.
/// 3. Open the output file and create output tree inside it. Creating branches
/// according to the output level.
/// 4. Reset the processes by calling InitProcess() again
///
/// Note: this methed runs under single thread node, so there is no conflict
/// when creating files.
void TRestThread::PrepareToProcess(bool* outputConfig, bool testrun) {
    debug << "Entering TRestThread::PrepareToProcess( testrun=" << testrun << " )" << endl;

    if (fProcessChain.size() > 0) {
        stringstream Filename;
        Filename << "/tmp/rest_thread_tmp" << ToString(this) << ".root";
        debug << "Creating file : " << Filename.str() << endl;
        fOutputFile = new TFile(Filename.str().c_str(), "recreate");
        fOutputFile->SetCompressionLevel(0);

        TRestAnalysisTree* tempTree = new TRestAnalysisTree("AnalysisTree_tmp", "anaTree_tmp");
        if (outputConfig == NULL) {
            outputConfig = new bool[4];
            for (int i = 0; i < 4; i++) {
                outputConfig[i] = true;
            }
        }
        if (outputConfig[0] == true) {
            // item: input analysis
            if (fHostRunner->GetInputAnalysisTree() != NULL)
                tempTree->CopyObservableList(fHostRunner->GetInputAnalysisTree(), "old_");
        }
        if (outputConfig[3] == false) {
            cout << "Error! output analysis must be on!" << endl;
            exit(1);
        }

        for (unsigned int i = 0; i < fProcessChain.size(); i++) {
            fProcessChain[i]->SetAnalysisTree(tempTree);
            for (unsigned int j = 0; j < fProcessChain.size(); j++) {
                fProcessChain[i]->SetFriendProcess(fProcessChain[j]);
            }
            debug << "InitProcess() process for " << fProcessChain[i]->ClassName() << endl;
            fProcessChain[i]->InitProcess();
        }

        fInputEvent = (TRestEvent*)fProcessChain[0]->GetInputEvent();
        if (fInputEvent == NULL) {
            error << "REST ERROR("
                  << "In thread " << fThreadId << ")::Input event of the first process is not specified!"
                  << endl;
            GetChar();
            exit(1);
        }
        fInputEvent = (TRestEvent*)fInputEvent->Clone();
        if (fHostRunner->GetNextevtFunc(fInputEvent, tempTree) != 0) {
            error << "REST ERROR("
                  << "In thread " << fThreadId
                  << ")::Failed to get the first input event, process cannot start!" << endl;
            GetChar();
            exit(1);
        }

        // test run
        if (testrun) {
            debug << "Test Run..." << endl;
            if (!TestRun(tempTree)) {
                error << "REST WARNING("
                      << "In thread " << fThreadId << ")::test run failed!" << endl;
                error << "One of the processes has NULL pointer fOutputEvent!" << endl;
                if (fVerboseLevel < REST_Debug)
                    error << "To see more detail, turn on debug mode for "
                             "TRestProcessRunner!"
                          << endl;
                exit(1);
            }
            debug << "Test Run complete!" << endl;
        } else {
            debug << "Setting output event address without test run... This may "
                     "cause empty event problem!"
                  << endl;
            fOutputEvent = fProcessChain[fProcessChain.size() - 1]->GetOutputEvent();
        }

        //////////////////////////////////////////
        // create dummy tree to store branch addresses.
        debug << "Creating Analysis Tree..." << endl;
        fAnalysisTree = new TRestAnalysisTree("AnalysisTree_" + ToString(fThreadId), "dummyTree");
        fAnalysisTree->CreateEventBranches();
        fAnalysisTree->CopyObservableList(tempTree);
        fEventTree = new TTree((TString) "EventTree_" + ToString(fThreadId), "dummyTree");
        for (unsigned int i = 0; i < fProcessChain.size(); i++) {
            // fProcessChain[i]->GetListOfAddedObservables().clear();
            fProcessChain[i]->SetAnalysisTree(fAnalysisTree);
            fProcessChain[i]->ConfigAnalysisTree();
        }
        vector<pair<TString, TRestEvent*>> branchesToAdd;
        // avoid duplicated branch
        // if event type is same, we only create branch for the last of this type
        // event
        if (outputConfig[1] == true) {
            // item: input event
            TRestEvent* evt = fInputEvent;
            {
                TString BranchName = (TString)evt->GetName() + "Branch";
                if (branchesToAdd.size() == 0)
                    branchesToAdd.push_back(pair<TString, TRestEvent*>(BranchName, evt));
                else
                    for (int j = 0; j < branchesToAdd.size(); j++) {
                        if (branchesToAdd[j].first == BranchName)
                            branchesToAdd[j].second = evt;
                        else if (j == branchesToAdd.size() - 1)
                            branchesToAdd.push_back(pair<TString, TRestEvent*>(BranchName, evt));
                    }
            }
            for (unsigned int i = 0; i < fProcessChain.size(); i++) {
                TRestEvent* evt = fProcessChain[i]->GetOutputEvent();
                {
                    TString BranchName = (TString)evt->GetName() + "Branch";
                    if (branchesToAdd.size() == 0)
                        branchesToAdd.push_back(pair<TString, TRestEvent*>(BranchName, evt));
                    else
                        for (int j = 0; j < branchesToAdd.size(); j++) {
                            if (branchesToAdd[j].first == BranchName)
                                branchesToAdd[j].second = evt;
                            else if (j == branchesToAdd.size() - 1)
                                branchesToAdd.push_back(pair<TString, TRestEvent*>(BranchName, evt));
                        }
                }
            }
        }

        if (outputConfig[2] == true) {
            // item: output event
            TRestEvent* evt = fOutputEvent;
            {
                TString BranchName = (TString)evt->GetName() + "Branch";
                if (branchesToAdd.size() == 0)
                    branchesToAdd.push_back(pair<TString, TRestEvent*>(BranchName, evt));
                else
                    for (int j = 0; j < branchesToAdd.size(); j++) {
                        if (branchesToAdd[j].first == BranchName)
                            branchesToAdd[j].second = evt;
                        else if (j == branchesToAdd.size() - 1)
                            branchesToAdd.push_back(pair<TString, TRestEvent*>(BranchName, evt));
                    }
            }
        }

        auto iter = branchesToAdd.begin();
        while (iter != branchesToAdd.end()) {
            fEventTree->Branch(iter->first, iter->second->ClassName(), iter->second);
            iter++;
        }

        if (fEventTree->GetListOfBranches()->GetLast() < 0)  // if no event branches are created
        {
            delete fEventTree;
            fEventTree = NULL;
        }

        fAnalysisTree->CreateObservableBranches();

        // create output temp file for process-defined output object

        fOutputFile->cd();
        fOutputFile->Clear();
        for (unsigned int i = 0; i < fProcessChain.size(); i++) {
            fProcessChain[i]->InitProcess();
        }

        delete tempTree;
        debug << "Thread " << fThreadId << " Ready!" << endl;
    } else {
        string tmp = fHostRunner->GetInputEvent()->ClassName();
        fInputEvent = (TRestEvent*)TClass::GetClass(tmp.c_str())->New();
        fOutputEvent = fInputEvent;
        stringstream Filename;
        Filename << "/tmp/rest_thread_tmp" << ToString(this) << ".root";
        fOutputFile = new TFile(Filename.str().c_str(), "recreate");
        fOutputFile->SetCompressionLevel(0);
        fOutputFile->cd();

        debug << "Creating Analysis Tree..." << endl;
        fAnalysisTree = new TRestAnalysisTree("AnalysisTree_" + ToString(fThreadId), "dummyTree");
        fAnalysisTree->CreateEventBranches();
        fEventTree = new TTree((TString) "EventTree_" + ToString(fThreadId), "dummyTree");
        // fEventTree->CreateEventBranches();

            if (outputConfig[2] == true) {
                TString BranchName = (TString)fInputEvent->GetName() + "Branch";
                if (fEventTree->GetBranch(BranchName) == NULL)  // avoid duplicated branch
                    fEventTree->Branch(BranchName, fInputEvent->ClassName(), fInputEvent);
            }
            if (outputConfig[3] == false) {
                cout << "Error! output analysis must be on!" << endl;
                exit(1);
            }
            // currently external process analysis is not supported!
        
        // if (fEventTree->GetListOfBranches()->GetLast() < 1)
        //{
        //	delete fEventTree; fEventTree = NULL;
        //}
        fAnalysisTree->CreateObservableBranches();
    }
}

///////////////////////////////////////////////
/// \brief The main function of this class. Thread will run this function until
/// the end.
///
/// It will start a loop, calling GetNextevtFunc(), ProcessEvent(), and
/// FillThreadEventFunc() repeatedly. If the function GetNextEvent() returns
/// false, the loop will break, meaning that we are at the end of the process.
/// Before return it will set "isFinished" to true.
///
/// Note: The methods GetNextevtFunc() and FillThreadEventFunc() are all from
/// TRestProcessRunner. The later two will call back the method FillEvent(),
/// WriteFile() in this class. The idea to do so is to make a unified
/// managemenet of these i-o related methods. In TRestRun the three methods are
/// mutex locked. They will be paused until the host run allows it to run. This
/// prevents segmentation violation due to simultaneously read/write.
void TRestThread::StartProcess() {
    isFinished = false;

    while (fHostRunner->GetNextevtFunc(fInputEvent, fAnalysisTree) == 0) {
        ProcessEvent();
        if (fOutputEvent != NULL) fHostRunner->FillThreadEventFunc(this);
    }

    // fHostRunner->WriteThreadFileFunc(this);
    isFinished = true;
}

///////////////////////////////////////////////
/// \brief Create a thread with the method StartProcess().
///
/// The thread will be detached afterwards.
void TRestThread::StartThread() {
    t = thread(&TRestThread::StartProcess, this);
    t.detach();
    // t.join();
}

///////////////////////////////////////////////
/// \brief Add a process.
///
/// The process will be added to the end of the process chain. It will also
/// increase TRestThread's verbose level if the added process's verbose level is
/// higher. Note that we cannot use "debug" verbose level under compatibility
/// output mode, i.e. in condor jobs.
void TRestThread::AddProcess(TRestEventProcess* process) {
    debug << "Entering TRestThread::AddProcess" << endl;

    fProcessChain.push_back(process);
    if (fout.CompatibilityMode() && process->GetVerboseLevel() >= REST_Debug) {
        warning << "REST WARNING! Cannot use \"debug\" output level for process " << process->GetName()
                << endl;
        process->SetVerboseLevel(REST_Info);
    }
    if (process->GetVerboseLevel() > fVerboseLevel) {
        SetVerboseLevel(process->GetVerboseLevel());
    }
}

///////////////////////////////////////////////
/// \brief Process a single event.
///
/// This is the only method that can running purely under multi thread mode
/// here. It gives the input event to the first process in process chain, then
/// it gives the process result to the next process, so on. Finally it gets a
/// result and saves it in the local output event.
void TRestThread::ProcessEvent() {
    TRestEvent* ProcessedEvent = fInputEvent;

    if (fVerboseLevel >= REST_Debug) {
#ifdef TIME_MEASUREMENT
        vector<int> processtime(fProcessChain.size());
#endif

        for (unsigned int j = 0; j < fProcessChain.size(); j++) {
            cout << "------- Starting process " + (string)fProcessChain[j]->GetName() + " -------" << endl;

#ifdef TIME_MEASUREMENT
            high_resolution_clock::time_point t1 = high_resolution_clock::now();
#endif

            fProcessChain[j]->BeginOfEventProcess(ProcessedEvent);
            ProcessedEvent = fProcessChain[j]->ProcessEvent(ProcessedEvent);
            fProcessChain[j]->EndOfEventProcess();

#ifdef TIME_MEASUREMENT
            high_resolution_clock::time_point t2 = high_resolution_clock::now();
            processtime[j] = (int)duration_cast<microseconds>(t2 - t1).count();
#endif

            if (ProcessedEvent == NULL) {
                cout << "------- End of process " + (string)fProcessChain[j]->GetName() +
                            " (NULL returned) -------"
                     << endl;
                break;
            } else {
                cout << "------- End of process " + (string)fProcessChain[j]->GetName() + " -------" << endl;
            }

            if (fProcessChain[j]->GetVerboseLevel() >= REST_Extreme && j < fProcessChain.size() - 1) {
                GetChar();
            }
        }

        fOutputEvent = ProcessedEvent;

#ifdef TIME_MEASUREMENT
        cout << "Process timing summary : " << endl;
        for (unsigned int j = 0; j < fProcessChain.size(); j++) {
            cout << fProcessChain[j]->ClassName() << "(" << fProcessChain[j]->GetName()
                 << ") : " << processtime[j] / (double)1000 << " ms" << endl;
        }
#endif

        GetChar("======= End of Event " +
                ((fOutputEvent == NULL) ? ToString(fInputEvent->GetID()) : ToString(fOutputEvent->GetID())) +
                " =======");
    } else {
        for (unsigned int j = 0; j < fProcessChain.size(); j++) {
            fProcessChain[j]->BeginOfEventProcess(ProcessedEvent);
            ProcessedEvent = fProcessChain[j]->ProcessEvent(ProcessedEvent);
            fProcessChain[j]->EndOfEventProcess();
            if (ProcessedEvent == NULL) break;
        }
        fOutputEvent = ProcessedEvent;
    }
}

///////////////////////////////////////////////
/// \brief Write and close the output file
///
/// This method is called back at the end of TRestProcessRunner::RunProcess() in
/// TRestProcessRunner.
void TRestThread::WriteFile() {
    info << "TRestThread : Writing temp file" << endl;

    if (fOutputFile == NULL) return;

    fOutputFile->cd();
    for (unsigned int i = 0; i < fProcessChain.size(); i++) {
        fProcessChain[i]->EndProcess();  // the processes must call
                                         // "object->Write()" in this method
    }

    delete fAnalysisTree;
    // fOutputFile->Close();
}
