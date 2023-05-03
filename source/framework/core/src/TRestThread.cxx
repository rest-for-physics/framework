
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

using namespace std;

#ifdef TIME_MEASUREMENT
#include <chrono>
using namespace chrono;
#endif

///////////////////////////////////////////////
/// \brief Set variables by default during initialization.
///
void TRestThread::Initialize() {
    fOutputEvent = nullptr;
    fInputEvent = nullptr;

    fAnalysisTree = nullptr;
    fEventTree = nullptr;

    fOutputFile = nullptr;

    fProcessChain.clear();

    isFinished = false;

    fCompressionLevel = 1;
    fVerboseLevel = TRestStringOutput::REST_Verbose_Level::REST_Essential;
}

///////////////////////////////////////////////
/// \brief Check if the input/output of each process in the process chain
/// matches
///
/// Input event of the whole process chain(i.e., the input event of first process)
/// should match the one read from input file.
///
/// Outevent of the previous process should be of the same type as the input
/// event of the later process. Both of them cannot be null.
///
Int_t TRestThread::ValidateChain(TRestEvent* input) {
    RESTInfo << "thread " << fThreadId << ": validating process chain" << RESTendl;

    // add the non-general processes to list for validation
    vector<TRestEventProcess*> processes;
    for (unsigned int i = 0; i < fProcessChain.size(); i++) {
        RESTValue inEvent = fProcessChain[i]->GetInputEvent();
        RESTValue outEvent = fProcessChain[i]->GetOutputEvent();

        if (outEvent.type == "TRestEvent" && inEvent.type == "TRestEvent") {
            RESTInfo << "general process: " << fProcessChain[i]->GetName() << RESTendl;
            continue;
        } else if (outEvent.cl && outEvent.cl->InheritsFrom("TRestEvent") && inEvent.cl &&
                   inEvent.cl->InheritsFrom("TRestEvent")) {
            processes.push_back(fProcessChain[i]);
        } else {
            RESTError << "Process: " << fProcessChain[i]->ClassName()
                      << " not properly written, the input/output event is illegal!" << RESTendl;
            RESTError << "Hint: they must be inherited from TRestEvent" << RESTendl;
            abort();
        }
    }

    if (processes.size() > 0) {
        // verify that the input event of first process is OK
        if (input != nullptr) {
            if ((string)input->ClassName() != processes[0]->GetInputEvent().type) {
                RESTError << "(ValidateChain): Input event type does not match!" << RESTendl;
                cout << "Input type of the first non-external process in chain: "
                     << processes[0]->GetInputEvent().type << endl;
                cout << "The event type from file: " << input->ClassName() << endl;
                cout << "No events will be processed. Please correct event process "
                        "input/output."
                     << endl;
            }
        }

        // verify that the output event type is good to be the input event of the next process
        for (unsigned int i = 0; i < processes.size() - 1; i++) {
            string outEventType = processes[i]->GetOutputEvent().type;
            string nextinEventType = processes[i + 1]->GetInputEvent().type;
            if (outEventType != nextinEventType && outEventType != "TRestEvent" &&
                nextinEventType != "TRestEvent") {
                RESTError << "(ValidateChain): Event process input/output does not match" << RESTendl;
                RESTError << "The event output for process " << processes[i]->GetName() << " is "
                          << outEventType << RESTendl;
                RESTError << "The event input for process " << processes[i + 1]->GetName() << " is "
                          << nextinEventType << RESTendl;
                RESTError << "No events will be processed. Please correctly connect the process chain!"
                          << RESTendl;
                GetChar();
                return -1;
            }
        }
    }
    return 0;
}

void TRestThread::SetThreadId(Int_t id) {
    fThreadId = id;
    if (fThreadId != 0 && fVerboseLevel > TRestStringOutput::REST_Verbose_Level::REST_Essential)
        fVerboseLevel = TRestStringOutput::REST_Verbose_Level::REST_Essential;
}

///////////////////////////////////////////////
/// \brief Make a test run of our process chain
///
/// The reason we use test run is that we need to determine the real output
/// event address of a process chain. This is because when we write our code
/// like this: \code TRestEvent* TRestRawSignalAnalysisProcess::ProcessEvent(
/// TRestEvent *inputEvent )
/// {
/// 	fSignalEvent = (TRestRawSignalEvent *)inputEvent;
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
bool TRestThread::TestRun() {
    RESTDebug << "Processing ..." << RESTendl;
    for (int i = 0; i < 5; i++) {
        TRestEvent* ProcessedEvent = fInputEvent;
        RESTDebug << "Test run " << i << " : Input Event ---- " << fInputEvent->ClassName() << "("
                  << fInputEvent << ")" << RESTendl;
        for (unsigned int j = 0; j < fProcessChain.size(); j++) {
            RESTDebug << "t" << fThreadId << "p" << j << ": " << fProcessChain[j]->ClassName() << RESTendl;

            fProcessChain[j]->SetObservableValidation(true);

            fProcessChain[j]->BeginOfEventProcess(ProcessedEvent);
            ProcessedEvent = fProcessChain[j]->ProcessEvent(ProcessedEvent);
            if (fProcessChain[j]->ApplyCut()) ProcessedEvent = nullptr;
            // if the output of ProcessEvent() is NULL we assume the event is cut.
            // we try to use GetOutputEvent()
            if (ProcessedEvent == nullptr) {
                ProcessedEvent = fProcessChain[j]->GetOutputEvent();
            }
            // if still null we perform another try
            if (ProcessedEvent == nullptr) {
                RESTDebug << "  ----  NULL" << RESTendl;
                break;
            }
            // check if the output event is same as the processed event
            TRestEvent* outputevent = fProcessChain[j]->GetOutputEvent();
            if (outputevent != ProcessedEvent) {
                RESTWarning
                    << "Test run, in " << fProcessChain[j]->ClassName()
                    << " : output event is different with process returned event! Please check to assign "
                       "the TRestEvent datamember as inputEvent in ProcessEvent() method"
                    << RESTendl;
            }

            fProcessChain[j]->EndOfEventProcess();

            fProcessChain[j]->SetObservableValidation(false);

            RESTDebug << " ....  " << ProcessedEvent->ClassName() << "(" << ProcessedEvent << ")" << RESTendl;
        }

        fOutputEvent = ProcessedEvent;
        fHostRunner->GetNextevtFunc(fInputEvent, fAnalysisTree);
        if (fOutputEvent != nullptr) {
            RESTDebug << "Output Event ---- " << fOutputEvent->ClassName() << "(" << fOutputEvent << ")"
                      << RESTendl;
            break;
        } else {
            RESTDebug << "Null output, trying again" << RESTendl;
        }
    }
    if (fOutputEvent == nullptr) {
        // fOutputEvent = fProcessChain[fProcessChain.size() - 1]->GetOutputEvent();
        return false;
    }
    return true;
}

///////////////////////////////////////////////
/// \brief Prepare some thing before we can start process
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
void TRestThread::PrepareToProcess(bool* outputConfig) {
    RESTDebug << "Entering TRestThread::PrepareToProcess" << RESTendl;

    string threadFileName;
    if (fHostRunner->GetOutputDataFile()->GetName() == (string) "/dev/null") {
        threadFileName = "/dev/null";
    } else {
        threadFileName = REST_TMP_PATH + "rest_thread_tmp" + ToString(this) + ".root";
    }

    bool outputConfigToDel = false;
    if (outputConfig == nullptr) {
        outputConfigToDel = true;
        outputConfig = new bool[4];
        for (int i = 0; i < 4; i++) {
            outputConfig[i] = true;
        }
    }
    if (outputConfig[3] == false) {
        cout << "Error! output analysis must be on!" << endl;
        exit(1);
    }

    if (fProcessChain.size() > 0) {
        RESTDebug << "TRestThread: Creating file : " << threadFileName << RESTendl;
        fOutputFile = new TFile(threadFileName.c_str(), "recreate");
        fOutputFile->SetCompressionLevel(fCompressionLevel);
        fAnalysisTree = new TRestAnalysisTree("AnalysisTree_" + ToString(fThreadId), "dummyTree");
        fAnalysisTree->DisableQuickObservableValueSetting();

        RESTDebug << "TRestThread: Finding first input event of process chain..." << RESTendl;
        if (fHostRunner->GetInputEvent() == nullptr) {
            RESTError
                << "Input event is not initialized from TRestRun! Please check your input file and file "
                   "reading process!"
                << RESTendl;
            exit(1);
        }
        fInputEvent = (TRestEvent*)fHostRunner->GetInputEvent()->Clone();
        string chainInputType = fProcessChain[0]->GetInputEvent().type;
        // we check if the process chain input type matches fInputEvent
        if (chainInputType != "TRestEvent" && chainInputType != (string)fInputEvent->ClassName()) {
            cout << "REST ERROR: Input event type does not match!" << endl;
            cout << "Process input type : " << chainInputType
                 << ", File input type : " << fInputEvent->ClassName() << endl;
            exit(1);
        }

        RESTDebug << "TRestThread: Reading input event and input observable..." << RESTendl;
        if (fHostRunner->GetNextevtFunc(fInputEvent, fAnalysisTree) != 0) {
            RESTError << "In thread " << fThreadId << ")::Failed to read input event, process cannot start!"
                      << RESTendl;
            exit(1);
        }

        RESTDebug << "TRestThread: Init process..." << RESTendl;
        for (unsigned int i = 0; i < fProcessChain.size(); i++) {
            fProcessChain[i]->SetAnalysisTree(fAnalysisTree);
            for (unsigned int j = 0; j < fProcessChain.size(); j++) {
                fProcessChain[i]->SetFriendProcess(fProcessChain[j]);
            }
            RESTDebug << "InitProcess() process for " << fProcessChain[i]->ClassName() << RESTendl;
            fProcessChain[i]->InitProcess();
        }

        // test run
        if (fHostRunner->UseTestRun()) {
            RESTDebug << "Test Run..." << RESTendl;
            if (!TestRun()) {
                RESTError << "In thread " << fThreadId << ")::test run failed!" << RESTendl;
                RESTError << "One of the processes has NULL pointer fOutputEvent!" << RESTendl;
                if (fVerboseLevel < TRestStringOutput::REST_Verbose_Level::REST_Debug)
                    RESTError << "To see more detail, turn on debug mode for "
                                 "TRestProcessRunner!"
                              << RESTendl;
                exit(1);
            }
            RESTDebug << "Test Run complete!" << RESTendl;
        } else {
            RESTDebug << "Initializing output event" << RESTendl;
            string chainOutputType = fProcessChain[fProcessChain.size() - 1]->GetOutputEvent().type;
            fOutputEvent = REST_Reflection::Assembly(chainOutputType);
            if (fOutputEvent == nullptr) {
                exit(1);
            }
        }

        //////////////////////////////////////////
        // create dummy tree to store events
        fEventTree = new TTree((TString) "EventTree_" + ToString(fThreadId), "dummyTree");
        vector<pair<TString, TRestEvent*>> branchesToAdd;
        // avoid duplicated branch
        // if event type is same, we only create branch for the last of this type
        // event
        if (outputConfig[1] == true) {
            // outputConfig[1]: the input events of each processes in the process chain(can be many)

            // the input event of process chain
            TRestEvent* evt = fInputEvent;
            {
                TString BranchName = (TString)evt->GetName() + "Branch";
                if (branchesToAdd.size() == 0)
                    branchesToAdd.push_back(pair<TString, TRestEvent*>(BranchName, evt));
                else
                    for (unsigned int j = 0; j < branchesToAdd.size(); j++) {
                        if (branchesToAdd[j].first == BranchName)
                            branchesToAdd[j].second = evt;
                        else if (j == branchesToAdd.size() - 1)
                            branchesToAdd.push_back(pair<TString, TRestEvent*>(BranchName, evt));
                    }
            }
            // the input event of other processes
            for (unsigned int i = 0; i < fProcessChain.size(); i++) {
                TRestEvent* evt = fProcessChain[i]->GetOutputEvent();
                if (evt != nullptr) {
                    TString BranchName = (TString)evt->GetName() + "Branch";
                    if (branchesToAdd.size() == 0)
                        branchesToAdd.push_back(pair<TString, TRestEvent*>(BranchName, evt));
                    else
                        for (unsigned int j = 0; j < branchesToAdd.size(); j++) {
                            if (branchesToAdd[j].first == BranchName)
                                branchesToAdd[j].second = evt;
                            else if (j == branchesToAdd.size() - 1)
                                branchesToAdd.push_back(pair<TString, TRestEvent*>(BranchName, evt));
                        }
                }
            }
        }

        if (outputConfig[2] == true) {
            // outputConfig[2]: output event: the output event of the last process in the process chain
            TRestEvent* evt = fOutputEvent;
            {
                TString BranchName = (TString)evt->GetName() + "Branch";
                if (branchesToAdd.size() == 0)
                    branchesToAdd.push_back(pair<TString, TRestEvent*>(BranchName, evt));
                else
                    for (unsigned int j = 0; j < branchesToAdd.size(); j++) {
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
            fEventTree = nullptr;
        }

        // fAnalysisTree->CreateBranches();

        // create output temp file for process-defined output object

        fOutputFile->cd();
        fOutputFile->Clear();
        for (unsigned int i = 0; i < fProcessChain.size(); i++) {
            fProcessChain[i]->InitProcess();
        }

        RESTDebug << "Thread " << fThreadId << " Ready!" << RESTendl;
    } else {
        string tmp = fHostRunner->GetInputEvent()->ClassName();
        fInputEvent = REST_Reflection::Assembly(tmp);
        fOutputEvent = fInputEvent;
        fOutputFile = new TFile(threadFileName.c_str(), "recreate");
        fOutputFile->SetCompressionLevel(fCompressionLevel);
        fOutputFile->cd();

        RESTDebug << "Creating Analysis Tree..." << RESTendl;
        fAnalysisTree = new TRestAnalysisTree("AnalysisTree_" + ToString(fThreadId), "dummyTree");
        fAnalysisTree->DisableQuickObservableValueSetting();
        fEventTree = new TTree((TString) "EventTree_" + ToString(fThreadId), "dummyTree");
        // fEventTree->CreateEventBranches();

        if (outputConfig[2]) {
            TString BranchName = (TString)fInputEvent->GetName() + "Branch";
            if (fEventTree->GetBranch(BranchName) == nullptr)  // avoid duplicated branch
            {
                fEventTree->Branch(BranchName, fInputEvent->ClassName(), fInputEvent);
            }
        }
        // currently, external process analysis is not supported!
    }
    if (outputConfigToDel) delete outputConfig;
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
/// EndProcess() in this class. The idea to do so is to make a unified
/// management of these i-o related methods. In TRestRun the three methods are
/// mutex locked. They will be paused until the host run allows it to run. This
/// prevents segmentation violation due to simultaneously read/write.
void TRestThread::StartProcess() {
    isFinished = false;

    while (fHostRunner->GetNextevtFunc(fInputEvent, fAnalysisTree) == 0) {
        ProcessEvent();
        /*if (fOutputEvent != nullptr) */ fHostRunner->FillThreadEventFunc(this);
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
    RESTDebug << "Entering TRestThread::AddProcess" << RESTendl;

    fProcessChain.push_back(process);
    // if (Console::CompatibilityMode && process->GetVerboseLevel() >= REST_Debug) {
    //    warning << "REST WARNING! Cannot use \"debug\" output level for process " << process->GetName()
    //            << endl;
    //    process->SetVerboseLevel(REST_Info);
    //}
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
    fProcessNullReturned = false;

    if (fVerboseLevel >= TRestStringOutput::REST_Verbose_Level::REST_Debug) {
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
            if (fProcessChain[j]->ApplyCut()) ProcessedEvent = nullptr;
            fProcessChain[j]->EndOfEventProcess();

#ifdef TIME_MEASUREMENT
            high_resolution_clock::time_point t2 = high_resolution_clock::now();
            processtime[j] = (int)duration_cast<microseconds>(t2 - t1).count();
#endif

            if (ProcessedEvent == nullptr) {
                cout << "------- End of process " + (string)fProcessChain[j]->GetName() +
                            " (NULL returned) -------"
                     << endl;
                fProcessNullReturned = true;
                break;
            } else {
                cout << "------- End of process " + (string)fProcessChain[j]->GetName() + " -------" << endl;
            }

            if (fProcessChain[j]->GetVerboseLevel() >= TRestStringOutput::REST_Verbose_Level::REST_Extreme &&
                j < fProcessChain.size() - 1) {
                GetChar();
            }
        }

#ifdef TIME_MEASUREMENT
        cout << "Process timing summary : " << endl;
        for (unsigned int j = 0; j < fProcessChain.size(); j++) {
            cout << fProcessChain[j]->ClassName() << "(" << fProcessChain[j]->GetName()
                 << ") : " << processtime[j] / (double)1000 << " ms" << endl;
        }
#endif

        if (fHostRunner->UseTestRun()) {
            fOutputEvent = ProcessedEvent;
        } else {
            cout << "Transferring output event..." << endl;
            if (ProcessedEvent != nullptr) {
                ProcessedEvent->CloneTo(fOutputEvent);
            }
        }

        GetChar(
            "======= End of Event " +
            ((fOutputEvent == nullptr) ? ToString(fInputEvent->GetID()) : ToString(fOutputEvent->GetID())) +
            " =======");
    } else {
        for (unsigned int j = 0; j < fProcessChain.size(); j++) {
            fProcessChain[j]->BeginOfEventProcess(ProcessedEvent);
            ProcessedEvent = fProcessChain[j]->ProcessEvent(ProcessedEvent);
            if (fProcessChain[j]->ApplyCut()) ProcessedEvent = nullptr;
            fProcessChain[j]->EndOfEventProcess();
            if (ProcessedEvent == nullptr) {
                fProcessNullReturned = true;
                break;
            }
        }

        if (fHostRunner->UseTestRun()) {
            fOutputEvent = ProcessedEvent;
        } else {
            if (ProcessedEvent != nullptr) {
                ProcessedEvent->CloneTo(fOutputEvent);
            }
        }
    }
}

///////////////////////////////////////////////
/// \brief Write and close the output file
///
/// This method is called back at the end of TRestProcessRunner::RunProcess() in
/// TRestProcessRunner.
void TRestThread::EndProcess() {
    RESTInfo << "TRestThread : Writing temp file. Thread id : " << fThreadId << RESTendl;

    if (fOutputFile == nullptr) return;

    fOutputFile->cd();
    Int_t nErrors = 0;
    Int_t nWarnings = 0;
    for (unsigned int i = 0; i < fProcessChain.size(); i++) {
        // The processes must call object->Write in this method
        fProcessChain[i]->EndProcess();
        if (fProcessChain[i]->GetError()) nErrors++;
        if (fProcessChain[i]->GetWarning()) nWarnings++;
    }

    if (nWarnings)
        RESTWarning << nWarnings
                    << " process warnings were found! Use run0->PrintWarnings(); to get additional info."
                    << RESTendl;
    if (nErrors)
        RESTError << nErrors << " process errors were found! Use run0->PrintErrors(); to get additional info."
                  << RESTendl;

    delete fAnalysisTree;
}
