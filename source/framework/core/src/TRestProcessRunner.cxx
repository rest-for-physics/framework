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
#include "TBranchRef.h"
#include "TInterpreter.h"
#include "TMinuitMinimizer.h"
#include "TMutex.h"
#include "TROOT.h"
#include "TRestManager.h"
#include "TRestThread.h"

#ifdef WIN32
#include <io.h>
#else
#include "unistd.h"
#endif  // !WIN32

std::mutex mutex_write;

using namespace std;
#ifdef TIME_MEASUREMENT
#include <chrono>
using namespace std::chrono;
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
    fRunInfo = nullptr;
    fInputEvent = nullptr;
    fOutputEvent = nullptr;
    fEventTree = nullptr;
    fAnalysisTree = nullptr;
    fOutputDataFile = nullptr;
    fOutputDataFileName = "";

    fThreads.clear();
    fProcessInfo.clear();

    fThreadNumber = 0;
    fFirstEntry = 0;
    fNFilesSplit = 0;
    fEventsToProcess = 0;
    fProcessedEvents = 0;
    fProcessNumber = 0;
    fProcStatus = kNormal;
    fFileSplitSize = 10000000000LL;  // 10GB maximum
    fFileCompression = 2;            // default compression level

    fUseTestRun = true;
    fUsePauseMenu = true;
    fValidateObservables = false;
    fSortOutputEvents = true;
    fInputAnalysisStorage = true;
    fInputEventStorage = true;
    fOutputEventStorage = true;
    fOutputAnalysisStorage = true;
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
    RESTInfo << RESTendl;
    if (fHostmgr != nullptr) {
        fRunInfo = fHostmgr->GetRunInfo();
        if (fRunInfo == nullptr) {
            RESTError << "File IO has not been specified, " << RESTendl;
            RESTError << "please make sure the \"TRestFiles\" section is ahead of the "
                         "\"TRestProcessRunner\" section"
                      << RESTendl;
            exit(0);
        }
    } else {
        RESTError << "manager not initialized!" << RESTendl;
        exit(0);
    }

    // Exiting as soon as possible in order to avoid seg.fault at TRestThread.
    // And sometimes gets stuck - probably a thread not closed? - and the execution
    // of restManager does not end even if TRestThread calls exit(1)
    // I believe it is risky to exit() at TRestThread without closing threads.
    // It is a guess (J.G.)
    if (!fRunInfo->GetFileProcess() && fRunInfo->GetEntries() == 0) {
        RESTError << "TRestProcessRunner::BeginOfInit. The input file is a valid REST file but entries are 0!"
                  << RESTendl;
        exit(1);
    }

    ReadAllParameters();

    // firstEntry = StringToInteger(GetParameter("firstEntry", "0"));
    // eventsToProcess = StringToInteger(GetParameter("eventsToProcess", "0"));
    int lastEntry = StringToInteger(GetParameter("lastEntry", "0"));
    if (lastEntry - fFirstEntry > 0 && fEventsToProcess == 0) {
        fEventsToProcess = lastEntry - fFirstEntry;
    } else if (fEventsToProcess > 0 && lastEntry - fFirstEntry > 0 &&
               lastEntry - fFirstEntry != fEventsToProcess) {
        RESTWarning << "Conflict number of events to process!" << RESTendl;
    } else if (fEventsToProcess > 0 && lastEntry - fFirstEntry == 0) {
        lastEntry = fFirstEntry + fEventsToProcess;
    } else if (fEventsToProcess == 0 && fFirstEntry == 0 && lastEntry == 0) {
        fEventsToProcess = REST_MAXIMUM_EVENTS;
        lastEntry = REST_MAXIMUM_EVENTS;
    } else {
        RESTWarning << "error setting of event number" << RESTendl;
        fEventsToProcess = fEventsToProcess > 0 ? fEventsToProcess : REST_MAXIMUM_EVENTS;
        fFirstEntry = fFirstEntry > 0 ? fFirstEntry : 0;
        lastEntry = lastEntry == fFirstEntry + fEventsToProcess ? lastEntry : REST_MAXIMUM_EVENTS;
    }
    fRunInfo->SetCurrentEntry(fFirstEntry);

    if (fFileSplitSize < 50000000LL || fFileSplitSize > 100000000000LL) {
        RESTWarning << "automatic file splitting size cannot < 10MB or > 100GB, setting to default (10GB)."
                    << RESTendl;
        fFileSplitSize = 10000000000LL;
    }

    // fUseTestRun = StringToBool(GetParameter("useTestRun", "ON"));
    // fUsePauseMenu = StringToBool(GetParameter("usePauseMenu", "OFF"));
    if (!fUsePauseMenu || fVerboseLevel >= TRestStringOutput::REST_Verbose_Level::REST_Debug)
        fProcStatus = kIgnore;
    if (fOutputAnalysisStorage == false) {
        RESTError << "output analysis must be turned on to process data!" << RESTendl;
        exit(1);
    }
    // fValidateObservables = StringToBool(GetParameter("validateObservables", "OFF"));
    // fSortOutputEvents = StringToBool(GetParameter("sortOutputEvents", "ON"));
    // fThreadNumber = StringToDouble(GetParameter("threadNumber", "1"));
    // if (ToUpper(GetParameter("inputAnalysis", "ON")) == "ON") fOutputItem[0] = true;
    // if (ToUpper(GetParameter("inputEvent", "OFF")) == "ON") fOutputItem[1] = true;
    // if (ToUpper(GetParameter("outputEvent", "ON")) == "ON") fOutputItem[2] = true;
    // fOutputItem[3] = true;

    // fOutputItem = Split(GetParameter("treeBranches",
    // "inputevent:outputevent:inputanalysis"), ":");
    if (fThreadNumber < 1) fThreadNumber = 1;
    if (fThreadNumber > 15) fThreadNumber = 15;

    for (int i = 0; i < fThreadNumber; i++) {
        TRestThread* t = new TRestThread();
        t->SetProcessRunner(this);
        t->SetVerboseLevel(fVerboseLevel);
        t->SetThreadId(i);
        t->SetCompressionLevel(fFileCompression);
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
            RESTWarning << "Bad expression of addProcess" << RESTendl;
            return 0;
        } else if (processName == "") {
            RESTWarning << "Event process " << processType << " has no name, it will be skipped" << RESTendl;
            return 0;
        }

        RESTInfo << "adding process " << processType << " \"" << processName << "\"" << RESTendl;
        vector<TRestEventProcess*> processes;
        for (int i = 0; i < fThreadNumber; i++) {
            TRestEventProcess* p = InstantiateProcess(processType, e);
            if (p != nullptr) {
                if (p->isExternal()) {
                    fRunInfo->SetExtProcess(p);
                    return 0;
                }
                if ((p->GetVerboseLevel() >= TRestStringOutput::REST_Verbose_Level::REST_Debug ||
                     p->singleThreadOnly())) {
                    fUsePauseMenu = false;
                    fProcStatus = kIgnore;
                    if (fThreadNumber > 1) {
                        RESTInfo << "multi-threading is disabled due to process \"" << p->GetName() << "\""
                                 << RESTendl;
                        RESTInfo << "This process is in debug mode or is single thread only" << RESTendl;
                        for (i = fThreadNumber; i > 1; i--) {
                            // delete (*fThreads.end());
                            fThreads.erase(fThreads.end() - 1);
                            fThreadNumber--;
                        }
                    }
                }
                processes.push_back(p);
            } else {
                return 1;
            }
        }

        for (int i = 0; i < fThreadNumber; i++) {
            TRestEventProcess* p = processes[i];
            for (int j = 0; j < fThreadNumber; j++) {
                p->SetParallelProcess(processes[j]);
            }
            fThreads[i]->AddProcess(p);
        }

        fProcessNumber++;
        RESTDebug << "process \"" << processType << "\" has been added!" << RESTendl;
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
    RESTDebug << "Validating process chain..." << RESTendl;

    if (fRunInfo->GetFileProcess() != nullptr) {
        fInputEvent = fRunInfo->GetFileProcess()->GetOutputEvent();
    } else {
        if (fThreads[0]->GetProcessnum() > 0 &&
            fThreads[0]->GetProcess(0)->GetInputEvent().address != nullptr) {
            string name = fThreads[0]->GetProcess(0)->GetInputEvent().type;
            TRestEvent* a = REST_Reflection::Assembly(name);
            a->Initialize();
            fRunInfo->SetInputEvent(a);
        }
        fInputEvent = fRunInfo->GetInputEvent();
    }
    if (fInputEvent == nullptr) {
        RESTError << "Cannot determine input event, validating process chain failed!" << RESTendl;
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
    if (fRunInfo->GetFileProcess() != nullptr) {
        fProcessInfo["FirstProcess"] = fRunInfo->GetFileProcess()->GetName();
    } else {
        if (fProcessNumber > 0) fProcessInfo["FirstProcess"] = fThreads[0]->GetProcess(0)->GetName();
    }
    int n = fProcessNumber;
    fProcessInfo["LastProcess"] =
        (n == 0 ? fProcessInfo["FirstProcess"] : fThreads[0]->GetProcess(n - 1)->GetName());
    fProcessInfo["ProcNumber"] = ToString(n);
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
    RESTDebug << "Creating output File " << fRunInfo->GetOutputFileName() << RESTendl;

    TString filename = fRunInfo->FormFormat(fRunInfo->GetOutputFileName());
    fOutputDataFileName = filename;
    fOutputDataFile = new TFile(filename, "recreate");
    // set compression level here will cause problem in pipeline
    // we must set in each threadCompression
    // fOutputDataFile->SetCompressionLevel(fFile);
    if (!fOutputDataFile->IsOpen()) {
        RESTError << "Failed to create output file: " << fOutputDataFile->GetName() << RESTendl;
        exit(1);
    }
    RESTInfo << RESTendl;
    RESTInfo << "TRestProcessRunner : preparing threads..." << RESTendl;
    fRunInfo->ResetEntry();
    fRunInfo->SetCurrentEntry(fFirstEntry);
    for (int i = 0; i < fThreadNumber; i++) {
        fThreads[i]->PrepareToProcess(&fInputAnalysisStorage);
    }

    // print metadata
    if (fRunInfo->GetFileProcess() != nullptr) {
        RESTEssential << this->ClassName() << ": 1 + " << fProcessNumber << " processes loaded, "
                      << fThreadNumber << " threads prepared!" << RESTendl;
    } else {
        RESTEssential << this->ClassName() << ": " << fProcessNumber << " processes loaded, " << fThreadNumber
                      << " threads prepared!" << RESTendl;
    }
    if (fVerboseLevel >= TRestStringOutput::REST_Verbose_Level::REST_Essential) {
        if (fRunInfo->GetFileProcess() != nullptr) fRunInfo->GetFileProcess()->PrintMetadata();

        for (int i = 0; i < fProcessNumber; i++) {
            fThreads[0]->GetProcess(i)->PrintMetadata();
        }
    } else {
        if (fRunInfo->GetFileProcess() != nullptr) {
            RESTcout << "(external) " << fRunInfo->GetFileProcess()->ClassName() << " : "
                     << fRunInfo->GetFileProcess()->GetName() << RESTendl;
        }
        for (int i = 0; i < fProcessNumber; i++) {
            RESTcout << "++ " << fThreads[0]->GetProcess(i)->ClassName() << " : "
                     << fThreads[0]->GetProcess(i)->GetName() << RESTendl;
        }
    }
    RESTcout << "=" << RESTendl;

    // copy thread's event tree to local
    fOutputDataFile->cd();
    TTree* tree = fThreads[0]->GetEventTree();
    if (tree != nullptr) {
        fEventTree = (TTree*)tree->Clone();
        fEventTree->SetName("EventTree");
        fEventTree->SetTitle("REST Event Tree");
        fEventTree->SetDirectory(fOutputDataFile);
        fEventTree->SetMaxTreeSize(100000000000LL > fFileSplitSize * 2
                                       ? 100000000000LL
                                       : fFileSplitSize * 2);  // the default size is 100GB
    } else {
        fEventTree = nullptr;
    }

    // initialize analysis tree
    fAnalysisTree = new TRestAnalysisTree("AnalysisTree", "REST Process Analysis Tree");
    fAnalysisTree->SetDirectory(fOutputDataFile);
    fAnalysisTree->SetMaxTreeSize(100000000000LL > fFileSplitSize * 2 ? 100000000000LL : fFileSplitSize * 2);

    tree = fThreads[0]->GetAnalysisTree();
    if (tree != nullptr) {
        fNBranches = tree->GetNbranches();
    } else {
        RESTError << "Threads are not initialized! No AnalysisTree!" << RESTendl;
        exit(1);
    }

    ConfigOutputFile();

    // reset runner
    this->ResetRunTimes();
    fProcessedEvents = 0;
    fRunInfo->ResetEntry();
    fRunInfo->SetCurrentEntry(fFirstEntry);
    inputtreeentries = fRunInfo->GetEntries();

    // set root mutex
    //!!!!!!!!!!!!Important!!!!!!!!!!!!
    ROOT::Math::MinimizerOptions::SetDefaultMinimizer("Minuit");
    TMinuitMinimizer::UseStaticMinuit(false);
    if (gGlobalMutex == nullptr) {
        gGlobalMutex = new TMutex(true);
        gROOTMutex = gGlobalMutex;
        gInterpreterMutex = gGlobalMutex;
    }

#ifdef TIME_MEASUREMENT
    high_resolution_clock::time_point t3 = high_resolution_clock::now();
#endif

    // start the thread!
    RESTcout << this->ClassName() << ": Starting the Process.." << RESTendl;
    for (int i = 0; i < fThreadNumber; i++) {
        fThreads[i]->StartThread();
    }

    while (fProcStatus == kPause ||
           (fRunInfo->GetInputEvent() != nullptr && fEventsToProcess > fProcessedEvents)) {
        PrintProcessedEvents(100);

        if (fProcStatus == kNormal && Console::kbhit())  // if keyboard inputs
        {
            int a = Console::ReadKey();  // get char

            if (a == 'p') {
                fProcStatus = kPause;
                usleep(100000);  // wait 0.1s for the processes to finish;
                TRestStringOutput RESTLog(TRestStringOutput::REST_Verbose_Level::REST_Silent, COLOR_BOLDWHITE,
                                          "| |", TRestStringOutput::REST_Display_Orientation::kMiddle);
                Console::ClearLinesAfterCursor();
                RESTLog << RESTendl;
                RESTLog << "-" << RESTendl;
                RESTLog << "PROCESS PAUSED!" << RESTendl;
                RESTLog << "-" << RESTendl;
                RESTLog << " " << RESTendl;
            }
        }

        if (fProcStatus == kPause) {
            PauseMenu();
        }
        if (fProcStatus == kStopping) {
            break;
        }

        usleep(printInterval);

        // cout << eventsToProcess << " " << fProcessedEvents << " " << lastEntry <<
        // " " << fCurrentEvent << endl; cout << fProcessedEvents << "\r";
        // printf("%d processed events now...\r", fProcessedEvents); fflush(stdout);
    }

    if (fProcStatus != kIgnore && Console::kbhit())
        while (getchar() != '\n')
            ;  // clear buffer

    RESTEssential << "Waiting for processes to finish ..." << RESTendl;

    while (1) {
        usleep(100000);
        bool finish = fThreads[0]->Finished();
        for (int i = 1; i < fThreadNumber; i++) {
            finish = finish && fThreads[i]->Finished();
        }
        if (finish) break;
    }

    // make dummy analysis tree filled with observables
    fAnalysisTree->GetEntry(fAnalysisTree->GetEntries() - 1);
    // call EndProcess() for all processes
    for (int i = 0; i < fThreadNumber; i++) {
        fThreads[i]->EndProcess();
    }
    if (fRunInfo->GetFileProcess()) {
        fRunInfo->GetFileProcess()->EndProcess();
    }
    fProcStatus = kFinished;

#ifdef TIME_MEASUREMENT
    high_resolution_clock::time_point t4 = high_resolution_clock::now();
    deltaTime = (int)duration_cast<microseconds>(t4 - t3).count();
#endif

    // reset the mutex to null
    delete gGlobalMutex;
    gGlobalMutex = nullptr;
    gROOTMutex = nullptr;
    gInterpreterMutex = nullptr;

    RESTcout << this->ClassName() << ": " << fProcessedEvents << " processed events" << RESTendl;

#ifdef TIME_MEASUREMENT
    RESTInfo << "Total processing time : " << ((Double_t)deltaTime) / 1000. << " ms" << RESTendl;
    RESTInfo << "Average read time from disk (per event) : "
             << ((Double_t)readTime) / fProcessedEvents / 1000. << " ms" << RESTendl;
    RESTInfo << "Average process time (per event) : "
             << ((Double_t)(deltaTime - readTime - writeTime)) / fProcessedEvents / 1000. << " ms"
             << RESTendl;
    RESTInfo << "Average write time to disk (per event) : "
             << ((Double_t)writeTime) / fProcessedEvents / 1000. << " ms" << RESTendl;
    RESTInfo << "=" << RESTendl;
#endif

    if (fRunInfo->GetOutputFileName() != "/dev/null") {
        ConfigOutputFile();
        MergeOutputFile();
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
    TRestStringOutput RESTLog(TRestStringOutput::REST_Verbose_Level::REST_Silent, COLOR_BOLDWHITE, "| |",
                              TRestStringOutput::REST_Display_Orientation::kMiddle);
    Console::ClearLinesAfterCursor();

    RESTLog << "--------------MENU--------------" << RESTendl;
    RESTLog << "\"v\" : change the verbose level" << RESTendl;
    RESTLog << "\"n\" : push foward one event, then pause" << RESTendl;
    RESTLog << "\"l\" : print the latest processed event" << RESTendl;
    RESTLog << "\"d\" : detach the current process" << RESTendl;
    RESTLog << "\"q\" : stop and quit the process" << RESTendl;
    RESTLog << "press \"p\" to continue process..." << RESTendl;
    RESTLog << "-" << RESTendl;
    RESTLog << RESTendl;
    RESTLog << RESTendl;
    int menuupper = 15;
    int infobar = 11;
    while (1) {
        // Console::CursorUp(1);
        Console::ClearCurrentLine();
        int b = Console::ReadKey();  // no need to press enter

        if (b == 'v') {
            Console::CursorUp(infobar);
            RESTLog.setcolor(COLOR_BOLDGREEN);
            RESTLog << "Changing verbose level for all the processes..." << RESTendl;
            RESTLog.setcolor(COLOR_BOLDWHITE);
            Console::CursorDown(1);
            Console::ClearLinesAfterCursor();
            RESTLog << "type \"0\"/\"s\" to set level silent" << RESTendl;
            RESTLog << "type \"1\"/\"e\" to set level essential" << RESTendl;
            RESTLog << "type \"2\"/\"i\" to set level info" << RESTendl;
            RESTLog << "type \"3\"/\"d\" to set level debug" << RESTendl;
            RESTLog << "type \"4\"/\"x\" to set level extreme" << RESTendl;
            RESTLog << "type other to return the pause menu" << RESTendl;
            RESTLog << "-" << RESTendl;
            RESTLog << RESTendl;
            RESTLog << RESTendl;
            while (1) {
                Console::CursorUp(1);
                int c = Console::Read();
                if (c != '\n')
                    while (Console::Read() != '\n')
                        ;
                TRestStringOutput::REST_Verbose_Level l;
                if (c == '0' || c == 's') {
                    l = TRestStringOutput::REST_Verbose_Level::REST_Silent;
                } else if (c == '1' || c == 'e') {
                    l = TRestStringOutput::REST_Verbose_Level::REST_Essential;
                } else if (c == '2' || c == 'i') {
                    l = TRestStringOutput::REST_Verbose_Level::REST_Info;
                } else if (c == '3' || c == 'd') {
                    l = TRestStringOutput::REST_Verbose_Level::REST_Debug;
                } else if (c == '4' || c == 'x') {
                    l = TRestStringOutput::REST_Verbose_Level::REST_Extreme;
                } else {
                    Console::CursorUp(infobar);
                    RESTLog.setcolor(COLOR_BOLDYELLOW);
                    RESTLog << "Verbose level not set!" << RESTendl;
                    RESTLog.setcolor(COLOR_BOLDWHITE);
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
                RESTLog.setcolor(COLOR_BOLDGREEN);
                RESTLog << "Verbose level has been set to " << ToString(static_cast<int>(l)) << "!"
                        << RESTendl;
                RESTLog.setcolor(COLOR_BOLDWHITE);
                break;
            }
            Console::ClearLinesAfterCursor();
            break;
        } else if (b == 'd') {
            Console::CursorUp(infobar);
            RESTLog.setcolor(COLOR_BOLDGREEN);
            RESTLog << "Detaching restManager to backend" << RESTendl;
            RESTLog.setcolor(COLOR_BOLDWHITE);
            Console::CursorDown(1);
            Console::ClearLinesAfterCursor();
            RESTLog << "type filename for output redirect" << RESTendl;
            RESTLog << "leave blank to redirect to /dev/null" << RESTendl;
            RESTLog << " " << RESTendl;
            RESTLog << " " << RESTendl;
            RESTLog << " " << RESTendl;
            RESTLog << " " << RESTendl;
            RESTLog << "-" << RESTendl;
            RESTLog << RESTendl;
            RESTLog << RESTendl;

            string file;

            Console::CursorUp(1);
            file = Console::ReadLine();
            if (file == "") file = "/dev/null";
            if (TRestTools::fileExists(file)) {
                if (!TRestTools::isPathWritable(file)) {
                    Console::CursorUp(infobar);
                    RESTLog.setcolor(COLOR_BOLDYELLOW);
                    RESTLog << "file not writeable!" << RESTendl;
                    RESTLog.setcolor(COLOR_BOLDWHITE);
                    break;
                }
            } else {
                if (!TRestTools::isPathWritable(TRestTools::SeparatePathAndName(file).first)) {
                    Console::CursorUp(infobar);
                    RESTLog.setcolor(COLOR_BOLDYELLOW);
                    RESTLog << "path not writeable!" << RESTendl;
                    RESTLog.setcolor(COLOR_BOLDWHITE);
                    break;
                }
            }

#ifdef WIN32
            RESTWarning << "fork not available on windows!" << RESTendl;
#else
            pid_t pid;
            pid = fork();
            if (pid < 0) {
                perror("fork error:");
                exit(1);
            }
            // child process
            if (pid == 0) {
                RESTcout << "Child process created! pid: " << getpid() << RESTendl;
                RESTInfo << "Restarting threads" << RESTendl;
                mutex_write.unlock();
                for (int i = 0; i < fThreadNumber; i++) {
                    fThreads[i]->StartThread();
                }
                RESTInfo << "Re-directing output to " << file << RESTendl;
                freopen(file.c_str(), "w", stdout);
                REST_Display_CompatibilityMode = true;
            }
            // father process
            else {
                exit(0);
            }
            fProcStatus = kNormal;
            RESTInfo << "Continue processing..." << RESTendl;

#endif  // WIN32

            break;
        } else if (b == 'n') {
            fProcStatus = kStep;
            break;
        } else if (b == 'l') {
            // Console::ClearScreen();
            fOutputEvent->PrintEvent();
            break;
        } else if (b == 'q') {
            fProcStatus = kStopping;
            break;
        } else if (b == 'p') {
            Console::CursorUp(menuupper);
            Console::ClearLinesAfterCursor();
            if (fVerboseLevel >= TRestStringOutput::REST_Verbose_Level::REST_Debug) {
                fProcStatus = kIgnore;
            } else {
                fProcStatus = kNormal;
            }
            break;
        } else if (b == '\n') {
            // CursorUp(1);
        } else {
            Console::CursorUp(infobar);
            RESTLog.setcolor(COLOR_BOLDYELLOW);
            RESTLog << "Invailed option \"" << (char)b << "\" (key value: " << b << ") !" << RESTendl;
            RESTLog.setcolor(COLOR_BOLDWHITE);
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
    mutex_write.lock();  // lock on
    while (fProcStatus == kPause) {
        usleep(100000);
    }
#ifdef TIME_MEASUREMENT
    high_resolution_clock::time_point t1 = high_resolution_clock::now();
#endif
    int n;
    if (fProcessedEvents >= fEventsToProcess || targetevt == nullptr || fProcStatus == kStopping) {
        n = -1;
    } else {
        if (fInputAnalysisStorage == false) {
            n = fRunInfo->GetNextEvent(targetevt, nullptr);
        } else {
            n = fRunInfo->GetNextEvent(targetevt, targettree);
        }
    }

#ifdef TIME_MEASUREMENT
    high_resolution_clock::time_point t2 = high_resolution_clock::now();
    readTime += (int)duration_cast<microseconds>(t2 - t1).count();
#endif
    mutex_write.unlock();
    return n;
}

///////////////////////////////////////////////
/// \brief Calling back the FillEvent() method in TRestThread.
///
/// This method is locked by mutex. There can never be two of it running
/// simultaneously in two threads. As a result threads will not write their
/// files together, thus preventing segmentaion violation.
void TRestProcessRunner::FillThreadEventFunc(TRestThread* t) {
    if (fSortOutputEvents) {
        // Make sure the thread has the minimum event id in the all the
        // threads. Otherwise just wait.
        while (1) {
            bool smallest = true;
            for (TRestThread* otherT : fThreads) {
                if (otherT->Finished()) {
                    continue;
                }
                if (t->GetThreadId() == otherT->GetThreadId()) {
                    continue;
                }
                int id1 = t->GetInputEvent()->GetID();
                int id2 = otherT->GetInputEvent()->GetID();
                if (id1 > id2) {
                    smallest = false;
                } else if (id1 == id2) {
                    cout << "warning! Two events with same event id! output events maybe dis-ordered!"
                         << endl;
                }
            }

            if (smallest) break;
            usleep(1);
        }
    }

    // Start event saving, entering mutex lock region.
    mutex_write.lock();
#ifdef TIME_MEASUREMENT
    high_resolution_clock::time_point t5 = high_resolution_clock::now();
#endif
    if (t->GetOutputEvent() != nullptr) {
        fOutputEvent = t->GetOutputEvent();
        // copy address of analysis tree of the given thread
        // to the local tree, then fill the local tree
        TObjArray* branchesT;
        TObjArray* branchesL;

        if (fAnalysisTree != nullptr) {
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

        if (fEventTree != nullptr) {
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

        // cout << fTempOutputDataFile << " " << fTempOutputDataFile->GetEND() << " " <<
        // fAnalysisTree->GetDirectory() << endl; cout << fAutoSplitFileSize << endl; switch file if file size
        // reaches target
        if (fOutputDataFile->GetEND() > fFileSplitSize) {
            if (fAnalysisTree->GetDirectory() == (TDirectory*)fOutputDataFile) {
                fNFilesSplit++;
                cout << "TRestProcessRunner: file size reaches limit (" << fFileSplitSize
                     << " bytes), switching to new file with index " << fNFilesSplit << endl;

                // wait 0.1s for the process to finish
                usleep(100000);
                for (auto th : fThreads) {
                    for (int j = 0; j < fProcessNumber; j++) {
                        auto proc = th->GetProcess(j);
                        proc->NotifyAnalysisTreeReset();
                    }
                }

                fAnalysisTree->AutoSave();
                fAnalysisTree->Reset();

                if (fEventTree != nullptr) {
                    fEventTree->AutoSave();
                    fEventTree->Reset();
                }

                // write some information to the first(main) data file
                fRunInfo->SetNFilesSplit(fNFilesSplit);
                if (fOutputDataFile->GetName() != fOutputDataFileName) {
                    auto Mainfile = std::unique_ptr<TFile>{TFile::Open(fOutputDataFileName, "update")};
                    WriteMetadata();
                    Mainfile->Close();
                } else {
                    WriteMetadata();
                }

                TFile* newfile = new TFile(fOutputDataFileName + "." + ToString(fNFilesSplit), "recreate");

                TBranch* branch = nullptr;
                fAnalysisTree->SetDirectory(newfile);
                TIter nextb1(fAnalysisTree->GetListOfBranches());
                while ((branch = (TBranch*)nextb1())) {
                    branch->SetFile(newfile);
                }
                if (fAnalysisTree->GetBranchRef()) {
                    fAnalysisTree->GetBranchRef()->SetFile(newfile);
                }

                if (fEventTree != nullptr) {
                    fEventTree->SetDirectory(newfile);
                    TIter nextb2(fEventTree->GetListOfBranches());
                    while ((branch = (TBranch*)nextb2())) {
                        branch->SetFile(newfile);
                    }
                    if (fEventTree->GetBranchRef()) {
                        fEventTree->GetBranchRef()->SetFile(newfile);
                    }
                }

                fOutputDataFile->Close();
                delete fOutputDataFile;
                fOutputDataFile = newfile;
            } else {
                RESTError << "internal error!" << RESTendl;
            }
        }

#ifdef TIME_MEASUREMENT
        high_resolution_clock::time_point t6 = high_resolution_clock::now();
        writeTime += (int)duration_cast<microseconds>(t6 - t5).count();
#endif

        if (fProcStatus == kStep) {
            fProcStatus = kPause;
            cout << "Processed events:" << fProcessedEvents << endl;
        }
    }
    mutex_write.unlock();
}

///////////////////////////////////////////////
/// \brief Forming an output file
///
/// It first saves process metadata in to the main output file, then calls
/// TRestRun::FormOutputFile() to merge the main file with process's tmp file.
void TRestProcessRunner::ConfigOutputFile() {
    RESTEssential << "Configuring output file, writing metadata and tree objects" << RESTendl;
#ifdef TIME_MEASUREMENT
    fProcessInfo["ProcessTime"] = ToString(deltaTime) + "ms";
#endif
    // write tree
    fOutputDataFile->cd();
    if (fEventTree != nullptr) fEventTree->Write(0, kWriteDelete);
    if (fAnalysisTree != nullptr) fAnalysisTree->Write(0, kWriteDelete);

    // go back to the first file
    if (fOutputDataFile->GetName() != fOutputDataFileName) {
        delete fOutputDataFile;
        fOutputDataFile = new TFile(fOutputDataFileName, "update");
    }
    // write metadata
    WriteMetadata();
}

void TRestProcessRunner::WriteMetadata() {
    fRunInfo->SetNFilesSplit(fNFilesSplit);
    fRunInfo->Write();
    this->Write();
    char tmpString[256];
    if (fRunInfo->GetFileProcess() != nullptr) {
        // sprintf(tmpString, "Process-%d. %s", 0, fRunInfo->GetFileProcess()->GetName());
        fRunInfo->GetFileProcess()->Write(0, kWriteDelete);
    }
    for (int i = 0; i < fProcessNumber; i++) {
        // sprintf(tmpString, "Process-%d. %s", i + 1, fThreads[0]->GetProcess(i)->GetName());
        fThreads[0]->GetProcess(i)->Write(0, kWriteDelete);
    }
}

///////////////////////////////////////////////
/// \brief Forming an output file
///
/// It first saves process metadata in to the main output file, then calls
/// TRestRun::FormOutputFile() to merge the main file with process's tmp file.
void TRestProcessRunner::MergeOutputFile() {
    RESTEssential << "Merging thread files together" << RESTendl;
    // add threads file
    // processes may have their own TObject output. They are stored in the threads
    // file these files are mush smaller that data file, so they are merged to the
    // data file.
    vector<string> files_to_merge;
    for (int i = 0; i < fThreadNumber; i++) {
        TFile* f = fThreads[i]->GetOutputFile();
        if (f != nullptr) f->Close();
        files_to_merge.push_back(f->GetName());
    }

    fOutputDataFile->Close();
    fRunInfo->MergeToOutputFile(files_to_merge, fOutputDataFile->GetName());
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
    time_t tt = time(nullptr);
    fProcessInfo["ProcessDate"] = Split(ToDateTimeString(tt), " ")[0];
}

///////////////////////////////////////////////
/// \brief InstantiateProcess in sequential start up
///
/// It instantiates a the object by the method TClass::GetClass(), giving it
/// type name. Then it asks the process object to LoadConfigFromFile() with an
/// xml section.
TRestEventProcess* TRestProcessRunner::InstantiateProcess(TString type, TiXmlElement* ele) {
    TRestEventProcess* pc = REST_Reflection::Assembly((string)type);
    if (pc == nullptr) return nullptr;

    pc->SetConfigFile(fConfigFileName);
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
        if (fEventsToProcess == REST_MAXIMUM_EVENTS && fRunInfo->GetFileProcess() != nullptr)
        // Nevents is unknown, reading external data file
        {
            prog = fRunInfo->GetBytesReaded() / (double)fRunInfo->GetTotalBytes() * 100;
        } else if (fRunInfo->GetFileProcess() != nullptr)
        // Nevents is known, reading external data file
        {
            prog = fProcessedEvents / (double)fEventsToProcess * 100;
        } else if (fEventsToProcess == REST_MAXIMUM_EVENTS)
        // Nevents is unknown, reading root file
        {
            prog = fRunInfo->GetCurrentEntry() / (double)inputtreeentries * 100;
        }

        else {
            prog = fProcessedEvents / (double)fEventsToProcess * 100;
        }

        char* buffer = new char[500]();
        if (fRunInfo->GetFileProcess() != nullptr) {
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
        if (REST_Display_CompatibilityMode) {
            barlength = 50;
        } else {
            barlength = Console::GetWidth() - s1.size() - s2.size() - 9;
        }
        sprintf(buffer, ("%.1f%[" + MakeProgressBar(prog, barlength) + "]").c_str(), prog);
        string s3(buffer);

        delete[] buffer;

        if (REST_Display_CompatibilityMode) {
            if (((int)prog) != prog_last_printed) {
                cout << s1 << s2 << s3 << endl;
                prog_last_printed = (int)prog;
            }
        } else if (fThreads[0]->GetVerboseLevel() < TRestStringOutput::REST_Verbose_Level::REST_Debug) {
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
    else if (fProcStatus == kStopping)
        status = "Terminated";
    else
        status = "Unknown";

    RESTMetadata << "Status : " << status << RESTendl;
    RESTMetadata << "Processesed events : " << fProcessedEvents << RESTendl;
    RESTMetadata << "Analysis tree branches : " << fNBranches << RESTendl;
    RESTMetadata << "Thread number : " << fThreadNumber << RESTendl;
    RESTMetadata << "Processes in each thread : " << fProcessNumber << RESTendl;
    RESTMetadata << "File auto split size: " << fFileSplitSize << RESTendl;
    RESTMetadata << "File compression level: " << fFileCompression << RESTendl;
    // cout << "Input filename : " << fInputFilename << endl;
    // cout << "Output filename : " << fOutputFilename << endl;
    // cout << "Number of initial events : " << GetNumberOfEvents() << endl;
    // cout << "Number of processed events : " << fProcessedEvents << endl;
    RESTMetadata << "******************************************" << RESTendl;
    RESTMetadata << RESTendl;
    RESTMetadata << RESTendl;
}
