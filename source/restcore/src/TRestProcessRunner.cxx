//////////////////////////////////////////////////////////////////////////
///
/// This class implements REST's main functionality - process running. multi-
/// thread is enabled here which improves efficiency. Pause menu and progress
/// bar is also provided which makes the work easier!
///
/// \class TRestProcessRunner
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

#include "Math/MinimizerOptions.h"
#include "TInterpreter.h"
#include "TMinuitMinimizer.h"
#include "TMutex.h"
#include "TROOT.h"
#include "TRestManager.h"
#include "TRestThread.h"

std::mutex mutexx;

#ifdef TIME_MEASUREMENT
#include <chrono>
using namespace chrono;
int deltaTime;
int writeTime;
int readTime;
high_resolution_clock::time_point tS, tE;
#endif

int ncalculated = 10;
Long64_t bytesReaded_last = 0;
Double_t prog_last = 0;
Int_t prog_last_printed = 0;
vector<Long64_t> bytesAdded(ncalculated, 0);
vector<Double_t> progAdded(ncalculated, 0);
int poscalculated = 0;
int printInterval = 200000;  // 0.2s
int inputtreeentries = 0;

ClassImp(TRestProcessRunner);

TRestProcessRunner::TRestProcessRunner() { Initialize(); }

TRestProcessRunner::~TRestProcessRunner() {}

///////////////////////////////////////////////
/// \brief Setting default values of class' data member
///
void TRestProcessRunner::Initialize() {
    fRunInfo = NULL;
    fInputEvent = NULL;
    fOutputEvent = NULL;
    fEventTree = NULL;
    fAnalysisTree = NULL;
    fTempOutputDataFile = NULL;
    fThreads.clear();
    ProcessInfo.clear();

    fThreadNumber = 0;
    firstEntry = 0;
    eventsToProcess = 0;
    fProcessedEvents = 0;
    fProcessNumber = 0;
    fProcStatus = kNormal;
}

///////////////////////////////////////////////
/// \brief Reads information from rml config file
///
/// It first checks if a friendly TRestRun object is initialized in
/// TRestManager, if so, it reads the following configuration items:
/// 1. firstEntry, lastEntry, eventsToProcess. These indicates how many events
/// we need to process.
/// 2. Tree branch list. can be inputAnalysis, inputEvent, outputEvent.
/// 3. Number of thread needed. A list TRestThread will then be instantiated.
void TRestProcessRunner::BeginOfInit() {
    info << endl;
    if (fHostmgr != NULL) {
        fRunInfo = fHostmgr->GetRunInfo();
        if (fRunInfo == NULL) {
            ferr << "File IO has not been specified, " << endl;
            ferr << "please make sure the \"TRestFiles\" section is ahead of the "
                    "\"TRestProcessRunner\" section"
                 << endl;
            exit(0);
        }
    } else {
        ferr << "manager not initialized!" << endl;
        exit(0);
    }

    // Exiting as soon as possible in order to avoid seg.fault at TRestThread.
    // And sometimes gets stuck - probably a thread not closed? - and the execution
    // of restManager does not end even if TRestThread calls exit(1)
    // I believe it is risky to exit() at TRestThread without closing threads.
    // It is a guess (J.G.)
    if (!fRunInfo->GetFileProcess() && fRunInfo->GetEntries() == 0) {
        ferr << "TRestProcessRunner::BeginOfInit. The input file is a valid REST file but entries are 0!"
             << endl;
        exit(1);
    }

    firstEntry = StringToInteger(GetParameter("firstEntry", "0"));
    int lastEntry = StringToInteger(GetParameter("lastEntry", "0"));
    eventsToProcess = StringToInteger(GetParameter("eventsToProcess", "0"));
    if (lastEntry - firstEntry > 0 && eventsToProcess == 0) {
        eventsToProcess = lastEntry - firstEntry;
    } else if (eventsToProcess > 0 && lastEntry - firstEntry > 0 &&
               lastEntry - firstEntry != eventsToProcess) {
        warning << "Conflict number of events to process!" << endl;
    } else if (eventsToProcess > 0 && lastEntry - firstEntry == 0) {
        lastEntry = firstEntry + eventsToProcess;
    } else if (eventsToProcess == 0 && firstEntry == 0 && lastEntry == 0) {
        eventsToProcess = REST_MAXIMUM_EVENTS;
        lastEntry = REST_MAXIMUM_EVENTS;
    } else {
        warning << "error setting of event number" << endl;
        eventsToProcess = eventsToProcess > 0 ? eventsToProcess : REST_MAXIMUM_EVENTS;
        firstEntry = firstEntry > 0 ? firstEntry : 0;
        lastEntry = lastEntry == firstEntry + eventsToProcess ? lastEntry : REST_MAXIMUM_EVENTS;
    }
    fRunInfo->SetCurrentEntry(firstEntry);

    fUseTestRun = StringToBool(GetParameter("useTestRun", "ON"));
    fValidateObservables = StringToBool(GetParameter("validateObservables", "OFF"));
    fThreadNumber = StringToDouble(GetParameter("threadNumber", "1"));
    if (ToUpper(GetParameter("inputAnalysis", "ON")) == "ON") fOutputItem[0] = true;
    if (ToUpper(GetParameter("inputEvent", "OFF")) == "ON") fOutputItem[1] = true;
    if (ToUpper(GetParameter("outputEvent", "ON")) == "ON") fOutputItem[2] = true;
    fOutputItem[3] = true;

    // fOutputItem = Split(GetParameter("treeBranches",
    // "inputevent:outputevent:inputanalysis"), ":");
    if (fThreadNumber < 1) fThreadNumber = 1;
    if (fThreadNumber > 15) fThreadNumber = 15;

    for (int i = 0; i < fThreadNumber; i++) {
        TRestThread* t = new TRestThread();
        t->SetProcessRunner(this);
        t->SetVerboseLevel(fVerboseLevel);
        t->SetThreadId(i);
        fThreads.push_back(t);
    }
}

///////////////////////////////////////////////
/// \brief method to deal with iterated child elements
///
/// If child element is declared as "addProcess", then multiple new process will
/// be instantiated using sequential startup method, by calling
/// InstantiateProcess() The processes will be added into each TRestThread
/// instance. If the process is external process, then it will be sent to
/// TRestRun.
Int_t TRestProcessRunner::ReadConfig(string keydeclare, TiXmlElement* e) {
    if (keydeclare == "addProcess") {
        string active = GetParameter("value", e, "");
        if (active != "" && ToUpper(active) != "ON") return 0;

        string processName = GetParameter("name", e, "");

        string processType = GetParameter("type", e, "");

        if (processType == "") {
            warning << "Bad expression of addProcess" << endl;
            return 0;
        } else if (processName == "") {
            warning << "Event process " << processType << " has no name, it will be skipped" << endl;
            return 0;
        }

        info << "adding process " << processType << " \"" << processName << "\"" << endl;
        for (int i = 0; i < fThreadNumber; i++) {
            TRestEventProcess* p = InstantiateProcess(processType, e);
            if (p != NULL) {
                if (p->isExternal()) {
                    fRunInfo->SetExtProcess(p);
                    return 0;
                } else if (p->GetVerboseLevel() >= REST_Debug || p->singleThreadOnly()) {
                    fProcStatus = kIgnore;
                    info << "multi-threading is disabled due to process \"" << p->GetName() << "\"" << endl;
                    info << "This process is in debug mode or is single thread only" << endl;

                    if (fThreadNumber > 1) {
                        for (i = fThreadNumber; i > 1; i--) {
                            fThreads.erase(fThreads.end() - 1);
                            fThreadNumber--;
                        }
                        fThreads[0]->AddProcess(p);

                        break;
                    }
                }
                fThreads[i]->AddProcess(p);
            } else {
                return 0;
            }
        }

        fProcessNumber++;
        debug << "process \"" << processType << "\" has been added!" << endl;
        return 0;
    }

    return 0;
}

///////////////////////////////////////////////
/// \brief Ending of the startup procedure.
///
/// It first sets input event as the first event in process chain, and then do a
/// Validation of the process chain. Finally it calls ReadProcInfo() and create
/// a process info list
void TRestProcessRunner::EndOfInit() {
    debug << "Validating process chain..." << endl;

    if (fRunInfo->GetFileProcess() != NULL) {
        fInputEvent = fRunInfo->GetFileProcess()->GetOutputEvent();
    } else {
        if (fThreads[0]->GetProcessnum() > 0 && fThreads[0]->GetProcess(0)->GetInputEvent().address != NULL) {
            string name = fThreads[0]->GetProcess(0)->GetInputEvent().type;
            TRestEvent* a = REST_Reflection::Assembly(name);
            a->Initialize();
            fRunInfo->SetInputEvent(a);
        }
        fInputEvent = fRunInfo->GetInputEvent();
    }
    if (fInputEvent == NULL) {
        ferr << "Cannot determine input event, validating process chain failed!" << endl;
        exit(1);
    }

    if (fProcessNumber > 0) {
        if (fThreads[0]->ValidateChain(fInputEvent) == -1) exit(1);
    }

    ReadProcInfo();
}

///////////////////////////////////////////////
/// \brief Create a process info list which used called by
/// TRestRun::FormFormat().
///
/// Items: FirstProcess, LastProcess, ProcNumber
void TRestProcessRunner::ReadProcInfo() {
    if (fRunInfo->GetFileProcess() != NULL) {
        ProcessInfo["FirstProcess"] = fRunInfo->GetFileProcess()->GetName();
    } else {
        if (fProcessNumber > 0) ProcessInfo["FirstProcess"] = fThreads[0]->GetProcess(0)->GetName();
    }
    int n = fProcessNumber;
    ProcessInfo["LastProcess"] =
        (n == 0 ? ProcessInfo["FirstProcess"] : fThreads[0]->GetProcess(n - 1)->GetName());
    ProcessInfo["ProcNumber"] = ToString(n);
}

///////////////////////////////////////////////
/// \brief The main executer of event process
///
/// Things doing in this method:
/// 1. Call each thread to prepare their process chain, output tree and output
/// file. The method is PrepareToProcess().
/// 2. Print metadata for each process
/// 3. Set output tree by cloning the TRestThread output tree
/// 4. Reset run count, modify ROOT mutex to make it support multi-thread.
/// 5. Call each thread to start. The threads running the processes are
/// detatched after this calling.
/// 6. The main thread loops for progress bar while waiting the child threads to
/// finish.
/// 7. After finished, print some information, reset ROOT mutes.
/// 8. Call ConfigOutputFile() method to save the output file
///
void TRestProcessRunner::RunProcess() {
    debug << "Creating output File " << fRunInfo->GetOutputFileName() << endl;

    TString filename = fRunInfo->FormFormat(fRunInfo->GetOutputFileName());
    fTempOutputDataFile = new TFile(filename, "recreate");
    if (!fTempOutputDataFile->IsOpen()) {
        ferr << "Failed to create output file: " << fTempOutputDataFile << endl;
        exit(1);
    }
    info << endl;
    info << "TRestProcessRunner : preparing threads..." << endl;
    fRunInfo->ResetEntry();
    fRunInfo->SetCurrentEntry(firstEntry);
    for (int i = 0; i < fThreadNumber; i++) {
        fThreads[i]->PrepareToProcess(fOutputItem);
    }

    // print metadata
    if (fRunInfo->GetFileProcess() != NULL) {
        essential << this->ClassName() << ": 1 + " << fProcessNumber << " processes loaded, " << fThreadNumber
                  << " threads prepared!" << endl;
    } else {
        essential << this->ClassName() << ": " << fProcessNumber << " processes loaded, " << fThreadNumber
                  << " threads prepared!" << endl;
    }
    if (fVerboseLevel >= REST_Essential) {
        if (fRunInfo->GetFileProcess() != NULL) fRunInfo->GetFileProcess()->PrintMetadata();

        for (int i = 0; i < fProcessNumber; i++) {
            fThreads[0]->GetProcess(i)->PrintMetadata();
        }
    } else {
        if (fRunInfo->GetFileProcess() != NULL) {
            fout << "(external) " << fRunInfo->GetFileProcess()->ClassName() << " : "
                 << fRunInfo->GetFileProcess()->GetName() << endl;
        }
        for (int i = 0; i < fProcessNumber; i++) {
            fout << "++ " << fThreads[0]->GetProcess(i)->ClassName() << " : "
                 << fThreads[0]->GetProcess(i)->GetName() << endl;
        }
    }
    fout << "=" << endl;

    // copy thread's event tree to local
    fTempOutputDataFile->cd();
    TTree* tree = fThreads[0]->GetEventTree();
    if (tree != NULL) {
        fEventTree = (TRestAnalysisTree*)tree->Clone();
        fEventTree->SetName("EventTree");
        string outputeventname;
        if (fThreads[0]->GetOutputEvent() != NULL) {
            outputeventname = fThreads[0]->GetOutputEvent()->ClassName();
        }

        fEventTree->SetTitle((outputeventname + "Tree").c_str());
        fEventTree->SetDirectory(fTempOutputDataFile);
    } else {
        fEventTree = NULL;
    }

    // initialize analysis tree
    fAnalysisTree = new TRestAnalysisTree("AnalysisTree", "REST Process Analysis Tree");
    fAnalysisTree->SetDirectory(fTempOutputDataFile);

    tree = fThreads[0]->GetAnalysisTree();
    if (tree != NULL) {
        nBranches = tree->GetNbranches();
    } else {
        ferr << "Threads are not initialized! No AnalysisTree!" << endl;
        exit(1);
    }

    // reset runner
    this->ResetRunTimes();
    fProcessedEvents = 0;
    fRunInfo->ResetEntry();
    fRunInfo->SetCurrentEntry(firstEntry);
    inputtreeentries = fRunInfo->GetEntries();

    // set root mutex
    //!!!!!!!!!!!!Important!!!!!!!!!!!!
    ROOT::Math::MinimizerOptions::SetDefaultMinimizer("Minuit");
    TMinuitMinimizer::UseStaticMinuit(false);
    if (gGlobalMutex == NULL) {
        gGlobalMutex = new TMutex(true);
        gROOTMutex = gGlobalMutex;
        gInterpreterMutex = gGlobalMutex;
    }

#ifdef TIME_MEASUREMENT
    high_resolution_clock::time_point t3 = high_resolution_clock::now();
#endif

    // start the thread!
    fout << this->ClassName() << ": Starting the Process.." << endl;
    for (int i = 0; i < fThreadNumber; i++) {
        fThreads[i]->StartThread();
    }

    while (fProcStatus == kPause ||
           (fRunInfo->GetInputEvent() != NULL && eventsToProcess > fProcessedEvents)) {
        PrintProcessedEvents(100);

        if (fProcStatus == kNormal && Console::kbhit())  // if keyboard inputs
        {
            int a = Console::ReadKey();  // get char

            if (a == 'p') {
                fProcStatus = kPause;
                usleep(100000);  // wait 0.1s for the processes to finish;
                TRestStringOutput cout(REST_Silent, COLOR_BOLDWHITE, "|", kBorderedMiddle);
                Console::ClearLinesAfterCursor();
                cout << endl;
                cout << "-" << endl;
                cout << "PROCESS PAUSED!" << endl;
                cout << "-" << endl;
                cout << " " << endl;
            }
        }

        if (fProcStatus == kPause) {
            PauseMenu();
        }
        if (fProcStatus == kStop) {
            break;
        }

#ifdef WIN32
        _sleep(50);
#else
        usleep(printInterval);
#endif

        // cout << eventsToProcess << " " << fProcessedEvents << " " << lastEntry <<
        // " " << fCurrentEvent << endl; cout << fProcessedEvents << "\r";
        // printf("%d processed events now...\r", fProcessedEvents); fflush(stdout);
    }

    if (Console::kbhit())
        while (getchar() != '\n')
            ;  // clear buffer

    // CursorDown(4);

    essential << "Waiting for processes to finish ..." << endl;

    while (1) {
#ifdef WIN32
        _sleep(50);
#else
        usleep(100000);
#endif
        bool finish = fThreads[0]->Finished();
        for (int i = 1; i < fThreadNumber; i++) {
            finish = finish && fThreads[i]->Finished();
        }
        if (finish) break;
    }

    fAnalysisTree->GetEntry(fAnalysisTree->GetEntries() - 1);
    for (int i = 0; i < fThreadNumber; i++) {
        fThreads[i]->EndProcess();
    }

#ifdef TIME_MEASUREMENT
    high_resolution_clock::time_point t4 = high_resolution_clock::now();
    deltaTime = (int)duration_cast<microseconds>(t4 - t3).count();
#endif

    // reset the mutex to null
    delete gGlobalMutex;
    gGlobalMutex = NULL;
    gROOTMutex = NULL;
    gInterpreterMutex = NULL;

    fout << this->ClassName() << ": " << fProcessedEvents << " processed events" << endl;

#ifdef TIME_MEASUREMENT
    info << "Total processing time : " << ((Double_t)deltaTime) / 1000. << " ms" << endl;
    info << "Average read time from disk (per event) : " << ((Double_t)readTime) / fProcessedEvents / 1000.
         << " ms" << endl;
    info << "Average process time (per event) : "
         << ((Double_t)(deltaTime - readTime - writeTime)) / fProcessedEvents / 1000. << " ms" << endl;
    info << "Average write time to disk (per event) : " << ((Double_t)writeTime) / fProcessedEvents / 1000.
         << " ms" << endl;
    info << "=" << endl;
#endif

    if (fRunInfo->GetOutputFileName() != "/dev/null") {
        ConfigOutputFile();
    }
}

///////////////////////////////////////////////
/// \brief A pause menu providing some functions during the process
///
/// It can:
/// 1. Change verbose level
/// 2. Change number of events to process
/// 3. Push forward event by event
/// 4. Print the latest processed event
/// 5. Quit the process directly with file saved
void TRestProcessRunner::PauseMenu() {
    TRestStringOutput cout(REST_Silent, COLOR_BOLDWHITE, "|", kBorderedMiddle);
    Console::ClearLinesAfterCursor();

    cout << "--------------MENU--------------" << endl;
    cout << "\"v\" : change the verbose level" << endl;
    cout << "\"n\" : push foward one event, then pause" << endl;
    cout << "\"l\" : print the latest processed event" << endl;
    cout << "\"d\" : detach the current process" << endl;
    cout << "\"q\" : stop and quit the process" << endl;
    cout << "press \"p\" to continue process..." << endl;
    cout << "-" << endl;
    cout << endl;
    cout << endl;
    int menuupper = 15;
    int infobar = 11;
    while (1) {
        // Console::CursorUp(1);
        Console::ClearCurrentLine();
        int b = Console::ReadKey();  // no need to press enter

        if (b == 'v') {
            Console::CursorUp(infobar);
            cout.setcolor(COLOR_BOLDGREEN);
            cout << "Changing verbose level for all the processes..." << endl;
            cout.setcolor(COLOR_BOLDWHITE);
            Console::CursorDown(1);
            Console::ClearLinesAfterCursor();
            cout << "type \"0\"/\"s\" to set level silent" << endl;
            cout << "type \"1\"/\"e\" to set level essential" << endl;
            cout << "type \"2\"/\"i\" to set level info" << endl;
            cout << "type \"3\"/\"d\" to set level debug" << endl;
            cout << "type \"4\"/\"x\" to set level extreme" << endl;
            cout << "type other to return the pause menu" << endl;
            cout << "-" << endl;
            cout << endl;
            cout << endl;
            while (1) {
                Console::CursorUp(1);
                int c = Console::Read();
                if (c != '\n')
                    while (Console::Read() != '\n')
                        ;
                REST_Verbose_Level l;
                if (c == '0' || c == 's') {
                    l = REST_Silent;
                } else if (c == '1' || c == 'e') {
                    l = REST_Essential;
                } else if (c == '2' || c == 'i') {
                    l = REST_Info;
                } else if (c == '3' || c == 'd') {
                    l = REST_Debug;
                } else if (c == '4' || c == 'x') {
                    l = REST_Extreme;
                } else {
                    Console::CursorUp(infobar);
                    cout.setcolor(COLOR_BOLDYELLOW);
                    cout << "Verbose level not set!" << endl;
                    cout.setcolor(COLOR_BOLDWHITE);
                    break;
                }

                gVerbose = l;
                this->SetVerboseLevel(l);
                for (int i = 0; i < fThreadNumber; i++) {
                    fThreads[i]->SetVerboseLevel(l);
                    for (int j = 0; j < fThreads[i]->GetProcessnum(); j++) {
                        fThreads[i]->GetProcess(j)->SetVerboseLevel(l);
                    }
                }
                Console::CursorUp(infobar);
                cout.setcolor(COLOR_BOLDGREEN);
                cout << "Verbose level has been set to " << ToString(l) << "!" << endl;
                cout.setcolor(COLOR_BOLDWHITE);
                break;
            }
            Console::ClearLinesAfterCursor();
            break;
        } else if (b == 'd') {
            Console::CursorUp(infobar);
            cout.setcolor(COLOR_BOLDGREEN);
            cout << "Detaching restManager to backend" << endl;
            cout.setcolor(COLOR_BOLDWHITE);
            Console::CursorDown(1);
            Console::ClearLinesAfterCursor();
            cout << "type filename for output redirect" << endl;
            cout << "leave blank to redirect to /dev/null" << endl;
            cout << " " << endl;
            cout << " " << endl;
            cout << " " << endl;
            cout << " " << endl;
            cout << "-" << endl;
            cout << endl;
            cout << endl;

            string file;

            Console::CursorUp(1);
            file = Console::ReadLine();
            if (file == "") file = "/dev/null";
            if (TRestTools::fileExists(file)) {
                if (!TRestTools::isPathWritable(file)) {
                    Console::CursorUp(infobar);
                    cout.setcolor(COLOR_BOLDYELLOW);
                    cout << "file not writeable!" << endl;
                    cout.setcolor(COLOR_BOLDWHITE);
                    break;
                }
            } else {
                if (!TRestTools::isPathWritable(TRestTools::SeparatePathAndName(file).first)) {
                    Console::CursorUp(infobar);
                    cout.setcolor(COLOR_BOLDYELLOW);
                    cout << "path not writeable!" << endl;
                    cout.setcolor(COLOR_BOLDWHITE);
                    break;
                }
            }

            pid_t pid;
            pid = fork();
            if (pid < 0) {
                perror("fork error:");
                exit(1);
            }
            // child process
            if (pid == 0) {
                fout << "Child process created! pid: " << getpid() << endl;
                info << "Restarting threads" << endl;
                mutexx.unlock();
                for (int i = 0; i < fThreadNumber; i++) {
                    fThreads[i]->StartThread();
                }
                info << "Re-directing output to " << file << endl;
                freopen(file.c_str(), "w", stdout);
                Console::CompatibilityMode = true;
            }
            // father process
            else {
                exit(0);
            }
            fProcStatus = kNormal;
            info << "Continue processing..." << endl;
            break;
        } else if (b == 'n') {
            fProcStatus = kStep;
            break;
        } else if (b == 'l') {
            // Console::ClearScreen();
            fOutputEvent->PrintEvent();
            break;
        } else if (b == 'q') {
            fProcStatus = kStop;
            break;
        } else if (b == 'p') {
            Console::CursorUp(menuupper);
            Console::ClearLinesAfterCursor();
            if (fVerboseLevel >= REST_Debug) {
                fProcStatus = kIgnore;
            } else {
                fProcStatus = kNormal;
            }
            break;
        } else if (b == '\n') {
            // CursorUp(1);
        } else {
            Console::CursorUp(infobar);
            cout.setcolor(COLOR_BOLDYELLOW);
            cout << "Invailed option \"" << (char)b << "\" (key value: " << b << ") !" << endl;
            cout.setcolor(COLOR_BOLDWHITE);
            Console::CursorDown(infobar - 1);
        }
    }
}

///////////////////////////////////////////////
/// \brief Get next event and copy it to the address of **targetevt**.
///
/// If can, it will also set the observal value of **targettree** according to
/// the local analysis tree.
///
/// This method is locked by mutex. There can never be two of it running
/// simultaneously in two threads.
///
/// If there is a single thread process, the local input event will be set to
/// the out put of this process. The **targettree** will not be changed.
///
/// If not, the local input event and analysis tree will be updated after
/// calling TTree::GetEntry(). The observables in the local tree will be copyed
/// to the **targettree**.
///
/// Finally the data in the input event will get cloned to the **targetevt** by
/// root streamer.
///
/// If the current entry is the last entry of the input tree, or the single
/// thread process stops to give a concret pointer as the output, the process is
/// over. This method returns -1.
///
Int_t TRestProcessRunner::GetNextevtFunc(TRestEvent* targetevt, TRestAnalysisTree* targettree) {
    mutexx.lock();  // lock on
    while (fProcStatus == kPause) {
#ifdef WIN32
        _sleep(50);
#else
        usleep(100000);
#endif
    }
#ifdef TIME_MEASUREMENT
    high_resolution_clock::time_point t1 = high_resolution_clock::now();
#endif
    int n;
    if (fProcessedEvents >= eventsToProcess || targetevt == NULL || fProcStatus == kStop) {
        n = -1;
    } else {
        if (fOutputItem[0] == false) {
            n = fRunInfo->GetNextEvent(targetevt, NULL);
        } else {
            n = fRunInfo->GetNextEvent(targetevt, targettree);
        }
    }

#ifdef TIME_MEASUREMENT
    high_resolution_clock::time_point t2 = high_resolution_clock::now();
    readTime += (int)duration_cast<microseconds>(t2 - t1).count();
#endif
    mutexx.unlock();
    return n;
}

///////////////////////////////////////////////
/// \brief Calling back the FillEvent() method in TRestThread.
///
/// This method is locked by mutex. There can never be two of it running
/// simultaneously in two threads. As a result threads will not write their
/// files together, thus preventing segmentaion violation.
void TRestProcessRunner::FillThreadEventFunc(TRestThread* t) {
    mutexx.lock();
#ifdef TIME_MEASUREMENT
    high_resolution_clock::time_point t5 = high_resolution_clock::now();
#endif
    if (t->GetOutputEvent() != NULL) {
        fOutputEvent = t->GetOutputEvent();
        // copy address of analysis tree of the given thread
        // to the local tree, then fill the local tree
        TObjArray* branchesT;
        TObjArray* branchesL;

        if (fAnalysisTree != NULL) {
            TRestAnalysisTree* remotetree = t->GetAnalysisTree();

            // t->GetAnalysisTree()->SetEventInfo(t->GetOutputEvent());
            // branchesT = t->GetAnalysisTree()->GetListOfBranches();
            // branchesL = fAnalysisTree->GetListOfBranches();
            // for (int i = 0; i < nBranches; i++) {
            //    TBranch* branchT = (TBranch*)branchesT->UncheckedAt(i);
            //    TBranch* branchL = (TBranch*)branchesL->UncheckedAt(i);
            //    branchL->SetAddress(branchT->GetAddress());
            //}
            fAnalysisTree->SetEventInfo(fOutputEvent);
            for (int n = 0; n < remotetree->GetNumberOfObservables(); n++) {
                fAnalysisTree->SetObservable(n, remotetree->GetObservable(n));
            }

            fAnalysisTree->Fill();
        }

        if (fEventTree != NULL) {
            // t->GetEventTree()->FillEvent(t->GetOutputEvent());
            branchesT = t->GetEventTree()->GetListOfBranches();
            branchesL = fEventTree->GetListOfBranches();
            for (int i = 0; i < branchesT->GetLast() + 1; i++) {
                TBranch* branchT = (TBranch*)branchesT->UncheckedAt(i);
                TBranch* branchL = (TBranch*)branchesL->UncheckedAt(i);
                branchL->SetAddress(branchT->GetAddress());
            }
            fEventTree->Fill();

            // cout << t->GetOutputEvent()->GetID() << endl;
        }
        fProcessedEvents++;

#ifdef TIME_MEASUREMENT
        high_resolution_clock::time_point t6 = high_resolution_clock::now();
        writeTime += (int)duration_cast<microseconds>(t6 - t5).count();
#endif

        if (fProcStatus == kStep) {
            fProcStatus = kPause;
            cout << "Processed events:" << fProcessedEvents << endl;
        }
    }
    mutexx.unlock();
}

///////////////////////////////////////////////
/// \brief Forming an output file
///
/// It first saves process metadata in to the main output file, then calls
/// TRestRun::FormOutputFile() to merge the main file with process's tmp file.
void TRestProcessRunner::ConfigOutputFile() {
    essential << "Configuring output file, merging thread files together" << endl;
#ifdef TIME_MEASUREMENT
    ProcessInfo["ProcessTime"] = ToString(deltaTime) + "ms";
#endif

    vector<string> files_to_merge;

    // add data file
    // string savemetadata = GetParameter("saveMetadata", "true");
    // if (savemetadata == "true" || savemetadata == "True" || savemetadata ==
    // "yes" || savemetadata == "ON")
    //{
    fTempOutputDataFile->cd();
    fRunInfo->Write();
    this->Write();
    char tmpString[256];
    if (fRunInfo->GetFileProcess() != NULL) {
        sprintf(tmpString, "Process-%d. %s", 0, fRunInfo->GetFileProcess()->GetName());
        fRunInfo->GetFileProcess()->Write();
    }
    for (int i = 0; i < fProcessNumber; i++) {
        sprintf(tmpString, "Process-%d. %s", i + 1, fThreads[0]->GetProcess(i)->GetName());
        fThreads[0]->GetProcess(i)->Write();
    }

    //}
    if (fEventTree != NULL) fEventTree->Write(0, kWriteDelete);
    if (fAnalysisTree != NULL) fAnalysisTree->Write(0, kWriteDelete);
    fTempOutputDataFile->Close();
    // files_to_merge.push_back(fTempOutputDataFile->GetName());

    // add threads file
    // processes may have their own TObject output. They are stored in the threads
    // file these files are mush smaller that data file, so they are merged to the
    // data file.
    for (int i = 0; i < fThreadNumber; i++) {
        TFile* f = fThreads[i]->GetOutputFile();
        if (f != NULL) f->Close();
        files_to_merge.push_back(f->GetName());
    }

    fRunInfo->MergeToOutputFile(files_to_merge, fTempOutputDataFile->GetName());
}

// tools
///////////////////////////////////////////////
/// \brief Reset running time count to 0
///
void TRestProcessRunner::ResetRunTimes() {
#ifdef TIME_MEASUREMENT
    readTime = 0;
    writeTime = 0;
    deltaTime = 0;
#endif
    time_t tt = time(NULL);
    ProcessInfo["ProcessDate"] = Split(ToDateTimeString(tt), " ")[0];
}

///////////////////////////////////////////////
/// \brief InstantiateProcess in sequential start up
///
/// It instantiates a the object by the method TClass::GetClass(), giving it
/// type name. Then it asks the process object to LoadConfigFromFile() with an
/// xml section.
TRestEventProcess* TRestProcessRunner::InstantiateProcess(TString type, TiXmlElement* ele) {
    TRestEventProcess* pc = REST_Reflection::Assembly((string)type);
    if (pc == NULL) return NULL;

    pc->SetRunInfo(this->fRunInfo);
    pc->SetHostmgr(fHostmgr);
    pc->SetObservableValidation(fValidateObservables);
    pc->LoadConfigFromElement(ele, fElementGlobal, fVariables);

    return pc;
}

double TRestProcessRunner::GetReadingSpeed() {
    Long64_t bytes = 0;
    for (auto& n : bytesAdded) bytes += n;
    double speedbyte = bytes / (double)printInterval * (double)1000000 / ncalculated;
    return speedbyte;
}

///////////////////////////////////////////////
/// \brief Print number of events processed, file read speed, ETA and a progress
/// bar.
///
void TRestProcessRunner::PrintProcessedEvents(Int_t rateE) {
    if (fProcStatus == kNormal || fProcStatus == kIgnore) {
        // clearLinesAfterCursor();

        // TRestStringOutput cout;
        // cout.setcolor(COLOR_BOLDWHITE);
        // cout.setorientation(0);
        // cout.setborder("|");
        // CursorDown(1);

        double speedbyte = GetReadingSpeed();

        double progsum = 0;
        for (auto& n : progAdded) progsum += n;
        double progspeed = progsum / ncalculated / printInterval * 1000000;

        double prog = 0;
        if (eventsToProcess == REST_MAXIMUM_EVENTS && fRunInfo->GetFileProcess() != NULL)
        // Nevents is unknown, reading external data file
        {
            prog = fRunInfo->GetBytesReaded() / (double)fRunInfo->GetTotalBytes() * 100;
        } else if (fRunInfo->GetFileProcess() != NULL)
        // Nevents is known, reading external data file
        {
            prog = fProcessedEvents / (double)eventsToProcess * 100;
        } else if (eventsToProcess == REST_MAXIMUM_EVENTS)
        // Nevents is unknown, reading root file
        {
            prog = fRunInfo->GetCurrentEntry() / (double)inputtreeentries * 100;
        }

        else {
            prog = fProcessedEvents / (double)eventsToProcess * 100;
        }

        char* buffer = new char[500]();
        if (fRunInfo->GetFileProcess() != NULL) {
            sprintf(buffer, "%d Events (%.1fMB/s), ", fProcessedEvents, speedbyte / 1024 / 1024);
        } else {
            sprintf(buffer, "%d Events, ", fProcessedEvents);
        }

        string s1(buffer);

        if (fProcStatus == kNormal) {
            sprintf(buffer, "%.1f min ETA, (Pause: \"p\") ", (100 - prog_last) / progspeed / 60);
        } else {
            sprintf(buffer, "%.1f min ETA, (Pause Disabled) ", (100 - prog_last) / progspeed / 60);
        }
        string s2(buffer);

        int barlength = 0;
        if (Console::CompatibilityMode) {
            barlength = 50;
        } else {
            barlength = Console::GetWidth() - s1.size() - s2.size() - 9;
        }
        sprintf(buffer, ("%.1f%[" + MakeProgressBar(prog, barlength) + "]").c_str(), prog);
        string s3(buffer);

        delete[] buffer;

        if (Console::CompatibilityMode) {
            if (((int)prog) != prog_last_printed) {
                cout << s1 << s2 << s3 << endl;
                prog_last_printed = (int)prog;
            }
        } else if (fThreads[0]->GetVerboseLevel() < REST_Debug) {
            printf("%s", (s1 + s2 + s3 + "\r").c_str());
            fflush(stdout);
        }

        // CursorUp(4);

        bytesAdded[poscalculated] = fRunInfo->GetBytesReaded() - bytesReaded_last;
        bytesReaded_last = fRunInfo->GetBytesReaded();
        progAdded[poscalculated] = prog - prog_last;
        prog_last = prog;

        poscalculated++;
        if (poscalculated >= ncalculated) poscalculated -= ncalculated;
    }
}

///////////////////////////////////////////////
/// \brief Make a string of progress bar with given length and percentage
///
string TRestProcessRunner::MakeProgressBar(int progress100, int length) {
    string progressbar(length, '-');
    int n = (double)progress100 / 100 * length;
    if (n < 0) n = 0;
    if (n > length + 1) n = length + 1;
    for (int i = 0; i < n; i++) {
        progressbar[i] = '=';
    }
    if (n < length + 1) progressbar[n] = '>';
    return progressbar;
}

TRestEvent* TRestProcessRunner::GetInputEvent() { return fRunInfo->GetInputEvent(); }

TRestAnalysisTree* TRestProcessRunner::GetInputAnalysisTree() { return fRunInfo->GetAnalysisTree(); }

void TRestProcessRunner::PrintMetadata() {
    // cout.precision(10);
    TRestMetadata::PrintMetadata();

    /*
    TRestStringOutput cout;
    cout.setborder("||");
    cout.setorientation(1);
    cout.setlength(100);
    */

    string status;
    if (fProcStatus == kNormal)
        status = "Normal";
    else if (fProcStatus == kStop)
        status = "Terminated";
    else
        status = "Unknown";

    metadata << "Status : " << status << endl;
    metadata << "Processesed events : " << fProcessedEvents << endl;
    metadata << "Analysis tree branches : " << nBranches << endl;
    metadata << "Thread number : " << fThreadNumber << endl;
    metadata << "Processes in each thread : " << fProcessNumber << endl;

    // cout << "Input filename : " << fInputFilename << endl;
    // cout << "Output filename : " << fOutputFilename << endl;
    // cout << "Number of initial events : " << GetNumberOfEvents() << endl;
    // cout << "Number of processed events : " << fProcessedEvents << endl;
    metadata << "******************************************" << endl;
    metadata << endl;
    metadata << endl;
}
