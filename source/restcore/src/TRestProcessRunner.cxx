#include "TRestManager.h"
#include "TRestThread.h"
#include "Math/MinimizerOptions.h"
#include "TMinuitMinimizer.h"
#include "TInterpreter.h"
#include "TMutex.h"
#include "TROOT.h"


std::mutex mutexx;

#define TIME_MEASUREMENT

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
vector<Long64_t> bytesAdded(ncalculated, 0);
vector<Double_t> progAdded(ncalculated, 0);
int poscalculated = 0;
int printInterval = 200000;//0.2s
int inputtreeentries = 0;
int barlength = 50;


ClassImp(TRestProcessRunner);

TRestProcessRunner::TRestProcessRunner()
{
	Initialize();
}


TRestProcessRunner::~TRestProcessRunner()
{
}




void TRestProcessRunner::Initialize()
{
	fInputEvent = NULL;
	fRunInfo = NULL;
	fThreads.clear();
	ProcessInfo.clear();
	fAnalysisTree = NULL;

	fThreadNumber = 0;
	firstEntry = 0;
	eventsToProcess = 0;
	fProcessedEvents = 0;
	fProcessNumber = 0;
	fProcStatus = kNormal;

}

void TRestProcessRunner::BeginOfInit()
{
	info << endl;
	if (fHostmgr != NULL)
	{
		fRunInfo = fHostmgr->GetRunInfo();
		if (fRunInfo == NULL) {
			error << "File IO has not been specified, " << endl;
			error << "please make sure the \"TRestFiles\" section is ahead of the \"TRestProcessRunner\" section" << endl;
			exit(0);
		}
	}
	else
	{
		error << "manager not initialized!" << endl;
		exit(0);
	}


	firstEntry = StringToInteger(GetParameter("firstEntry", "0"));
	int lastEntry = StringToInteger(GetParameter("lastEntry", "0"));
	eventsToProcess = StringToInteger(GetParameter("eventsToProcess", "0"));
	if (lastEntry - firstEntry > 0 && eventsToProcess == 0) {
		eventsToProcess = lastEntry - firstEntry;
	}
	else if (eventsToProcess > 0 && lastEntry - firstEntry > 0 && lastEntry - firstEntry != eventsToProcess) {
		warning << "Conflict number of events to process!" << endl;
	}
	else if (eventsToProcess > 0 && lastEntry - firstEntry == 0) {
		lastEntry = firstEntry + eventsToProcess;
	}
	else if (eventsToProcess == 0 && firstEntry == 0 && lastEntry == 0)
	{
		eventsToProcess = REST_MAXIMUM_EVENTS;
		lastEntry = REST_MAXIMUM_EVENTS;
	}
	else
	{
		warning << "error setting of event number" << endl;
		eventsToProcess = eventsToProcess > 0 ? eventsToProcess : REST_MAXIMUM_EVENTS;
		firstEntry = firstEntry > 0 ? firstEntry : 0;
		lastEntry = lastEntry == firstEntry + eventsToProcess ? lastEntry : REST_MAXIMUM_EVENTS;
	}
	fRunInfo->SetCurrentEntry(firstEntry);


	fThreadNumber = StringToDouble(GetParameter("threadNumber", "1"));
	fOutputItem = Spilt(GetParameter("treeBranches", "inputevent:processevent:outputevent:inputanalysis:outputanalysis"), ":");
	if (fThreadNumber < 1)fThreadNumber = 1;
	if (fThreadNumber > 15)fThreadNumber = 15;


	for (int i = 0; i < fThreadNumber; i++)
	{
		TRestThread* t = new TRestThread();
		t->SetTRestRunner(this);
		t->SetBranchConfig(fOutputItem);
		t->SetVerboseLevel(fVerboseLevel);
		t->SetThreadId(i);
		fThreads.push_back(t);
	}

}


Int_t TRestProcessRunner::ReadConfig(string keydeclare, TiXmlElement * e)
{

	if (keydeclare == "addProcess")
	{
		string active = GetParameter("value", e, "");
		if (active != "ON" && active != "On" && active != "on") return 0;

		string processName = GetParameter("name", e, "");

		string processType = GetParameter("type", e, "");

		if (processType == "") { warning << "Bad expression of addProcess" << endl; return 0; }
		else if (processName == "") {
			warning << "Event process " << processType << " has no name, it will be skipped" << endl;
			return 0;
		}

		info << "adding process " << processType << " \"" << processName << "\"" << endl;
		for (int i = 0; i < fThreadNumber; i++)
		{
			TRestEventProcess* p = InstantiateProcess(processType, e);
			if (p != NULL) {
				if (p->InheritsFrom("TRestRawToSignalProcess"))
				{
					fRunInfo->SetExtProcess(p);
					return 0;
				}
				else
				{
					if (p->GetVerboseLevel() >= REST_Debug) {
						fProcStatus = kIgnore;
						debug << "a process is in debug mode, pause menu disabled." << endl;
					}
					if (p->singleThreadOnly()) {
						fProcStatus = kIgnore;
						if (fThreadNumber > 1)
						{
							//If the process declared single thread only, then the whole process will be 
							//in single thread mode
							warning << "process: " << p->ClassName() << " can only run under single thread mode" << endl;
							warning << "the analysis run will be performed with single thread!" << endl;
							for (i = fThreadNumber; i > 1; i--) {
								fThreads.erase(fThreads.end() - 1);
								fThreadNumber--;
							}
							fThreads[0]->AddProcess(p);

							break;
						}
					}
					fThreads[i]->AddProcess(p);
				}
			}
			else
			{
				return 0;
			}
		}

		fProcessNumber++;
		debug << "process \"" << processType << "\" has been added!" << endl;
		return 0;
	}



	return 0;
}


void TRestProcessRunner::EndOfInit()
{
	debug << "Validating process chain..." << endl;
	if (fProcessNumber > 0)
	{
		if (fRunInfo->GetFileProcess() != NULL)
		{
			fInputEvent = fRunInfo->GetFileProcess()->GetOutputEvent();
		}
		else
		{
			if (fThreads[0]->GetProcessnum() > 0 && fThreads[0]->GetProcess(0)->GetInputEvent() != NULL)
			{
				string name = fThreads[0]->GetProcess(0)->GetInputEvent()->ClassName();
				auto a = (TRestEvent*)TClass::GetClass(name.c_str())->New();
				a->Initialize();
				fRunInfo->SetInputEvent(a);
			}
			fInputEvent = fRunInfo->GetInputEvent();
		}
		if (fThreads[0]->ValidateInput(fInputEvent) == -1) exit(1);
		if (fThreads[0]->ValidateChain() == -1)exit(1);
	}

	ReadProcInfo();


}

void TRestProcessRunner::ReadProcInfo()
{
	if (fRunInfo->GetFileProcess() != NULL)
	{
		ProcessInfo["FirstProcess"] = fRunInfo->GetFileProcess()->GetName();
	}
	else
	{
		if (fProcessNumber > 0)
			ProcessInfo["FirstProcess"] = fThreads[0]->GetProcess(0)->GetName();
	}
	int n = fProcessNumber;
	ProcessInfo["LastProcess"] = (n == 0 ? ProcessInfo["FirstProcess"] : fThreads[0]->GetProcess(n - 1)->GetName());
	ProcessInfo["ProcNumber"] = ToString(n);


}





///////////////////////////////////////////////
/// \brief The main executer of event process
///
/// Things doing in this method:
/// 1. Call each thread to prepare their process chain, output tree and output file. The method is PrepareToProcess().
/// 2. Set some parameters, initialize the single thread process if it exists.
/// 3. Call each thread to start. The threads running the processes are detatched after this calling.
/// 4. The main thread becomes idle waiting for the child threads to finish.
/// 5. Call the	ConfigOutputFile() method to merge output files of each threads together.
///
void TRestProcessRunner::RunProcess()
{

	fTempOutputDataFile = new TFile(fRunInfo->GetOutputFileName(), "recreate");
	info << endl;
	info << "TRestProcessRunner : perparing threads..." << endl;
	fRunInfo->ResetEntry();
	fRunInfo->SetCurrentEntry(firstEntry);
	bool testrun = ToUpper(GetParameter("testRun", "ON")) == "ON" || ToUpper(GetParameter("testRun", "ON")) == "TRUE";
	for (int i = 0; i < fThreadNumber; i++)
	{
		fThreads[i]->PrepareToProcess(testrun);
	}


	//print metadata
	if (fRunInfo->GetFileProcess() != NULL) {
		essential << this->ClassName() << ": 1 + " << fProcessNumber << " processes loaded, " << fThreadNumber << " threads prepared!" << endl;
	}
	else
	{
		essential << this->ClassName() << ": " << fProcessNumber << " processes loaded, " << fThreadNumber << " threads prepared!" << endl;
	}
	if (fVerboseLevel >= REST_Info) {
		if (fRunInfo->GetFileProcess() != NULL)fRunInfo->GetFileProcess()->PrintMetadata();

		for (int i = 0; i < fProcessNumber; i++)
		{
			fThreads[0]->GetProcess(i)->PrintMetadata();
		}
	}
	else if (fVerboseLevel >= REST_Essential)
	{
		if (fRunInfo->GetFileProcess() != NULL)
		{
			essential << "(external) " << fRunInfo->GetFileProcess()->ClassName() << " : " << fRunInfo->GetFileProcess()->GetName() << endl;
		}
		for (int i = 0; i < fProcessNumber; i++)
		{
			essential << "++ " << fThreads[0]->GetProcess(i)->ClassName() << " : " << fThreads[0]->GetProcess(i)->GetName() << endl;
		}
	}
	fout << "=" << endl;


	//copy thread tree to local
	fTempOutputDataFile->cd();
	TRestAnalysisTree* tree = fThreads[0]->GetEventTree();
	if (tree != NULL) {
		fEventTree = (TRestAnalysisTree*)tree->Clone();
		fEventTree->SetName("EventTree");
		fEventTree->SetTitle("EventTree");
		fEventTree->SetDirectory(fTempOutputDataFile);
	}
	else
	{
		fEventTree = NULL;
	}

	tree = (TRestAnalysisTree*)fThreads[0]->GetAnalysisTree();
	if (tree != NULL) {
		fAnalysisTree = (TRestAnalysisTree*)tree->Clone();
		fAnalysisTree->SetName("AnalysisTree");
		fAnalysisTree->SetTitle("AnalysisTree");
		fAnalysisTree->SetDirectory(fTempOutputDataFile);
	}
	else
	{
		fAnalysisTree = NULL;
	}

	nBranches = fAnalysisTree->GetListOfBranches()->GetEntriesFast();

	//reset runner
	this->ResetRunTimes();
	fProcessedEvents = 0;
	fRunInfo->ResetEntry();
	fRunInfo->SetCurrentEntry(firstEntry);
	inputtreeentries = fRunInfo->GetEntries();

	//set root mutex
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

	//start the thread!
	fout << this->ClassName() << ": Starting the Process.." << endl;
	for (int i = 0; i < fThreadNumber; i++)
	{
		fThreads[i]->StartThread();
	}

	cout << endl << endl;
	while (fProcStatus == kPause || (fRunInfo->GetInputEvent() != NULL && eventsToProcess > fProcessedEvents))
	{
		if (fProcStatus != kIgnore && kbhit())//if keyboard inputs
		{
			cursorUp(1);
			int a = getchar();//get char
			if (a != '\n')
				while (getchar() != '\n');//clear buffer
			if (a == 'p') {
				fProcStatus = kPause;
				clearLinesAfterCursor();
				TRestStringOutput cout;
				cout.setcolor(COLOR_BOLDWHITE);
				cout.setorientation(0);
				cout.setborder("|");
				cout << endl;
				cout << "-" << endl;
				cout << "PROCESS PAUSED!" << endl;
				cout << "-" << endl;
				cout << " " << endl;
				cout << "--------------MENU--------------" << endl;
			}

		}

		if (fProcStatus == kPause) {
			PauseMenu();
		}
		if (fProcStatus == kStop) {
			break;
		}

		PrintProcessedEvents(100);

#ifdef WIN32
		_sleep(50);
#else
		usleep(printInterval);
#endif

		//cout << eventsToProcess << " " << fProcessedEvents << " " << lastEntry << " " << fCurrentEvent << endl;
		//cout << fProcessedEvents << "\r";
		//printf("%d processed events now...\r", fProcessedEvents);
		//fflush(stdout);

	}

	if (kbhit())
		while (getchar() != '\n');//clear buffer

	cursorDown(4);

	essential << "Waiting for processes to finish ..." << endl;

	while (1)
	{
#ifdef WIN32
		_sleep(50);
#else
		usleep(100000);
#endif
		bool finish = fThreads[0]->Finished();
		for (int i = 1; i < fThreadNumber; i++)
		{
			finish = finish && fThreads[i]->Finished();
		}
		if (finish)break;
	}

	for (int i = 0; i < fThreadNumber; i++)
	{
		fThreads[i]->WriteFile();
	}

#ifdef TIME_MEASUREMENT
	high_resolution_clock::time_point t4 = high_resolution_clock::now();
	deltaTime = (int)duration_cast<microseconds>(t4 - t3).count();
#endif

	//reset the mutex to null
	delete gGlobalMutex;
	gGlobalMutex = NULL;
	gROOTMutex = NULL;
	gInterpreterMutex = NULL;

	fout << this->ClassName() << ": " << fProcessedEvents << " processed events" << endl;

#ifdef TIME_MEASUREMENT
	info << "Total processing time : " << ((Double_t)deltaTime) / 1000. << " ms" << endl;
	info << "Average read time from disk (per event) : " << ((Double_t)readTime) / fProcessedEvents / 1000. << " ms" << endl;
	info << "Average process time (per event) : " << ((Double_t)(deltaTime - readTime - writeTime)) / fProcessedEvents / 1000. << " ms" << endl;
	info << "Average write time to disk (per event) : " << ((Double_t)writeTime) / fProcessedEvents / 1000. << " ms" << endl;
	info << "=" << endl;
#endif

	ConfigOutputFile();


}


void TRestProcessRunner::PauseMenu() {
	TRestStringOutput cout;
	cout.setcolor(COLOR_BOLDWHITE);
	cout.setorientation(0);
	cout.setborder("|");
	clearLinesAfterCursor();
	cout << "\"v\" : change the verbose level" << endl;
	cout << "\"e\" : change the number of events to process" << endl;
	cout << "\"n\" : push foward one event, then pause" << endl;
	cout << "\"l\" : print the latest processed event" << endl;
	cout << "\"q\" : stop and quit the process" << endl;
	cout << "press \"p\" to continue process..." << endl;
	cout << "-" << endl;
	cout << endl;
	cout << endl;
	int mainmenuleng = 9;
	while (1) {
		cursorUp(1);
		clearCurrentLine();
		int b = getchar();
		if (b != '\n')
			while (getchar() != '\n');

		if (b == 'v')
		{
			cursorUp(mainmenuleng + 2);
			cout.setcolor(COLOR_BOLDGREEN);
			cout << "Changing verbose level for all the processes..." << endl;
			cout.setcolor(COLOR_BOLDWHITE);
			cursorDown(1);
			clearLinesAfterCursor();
			cout << "type \"0\"/\"s\" to set level silent" << endl;
			cout << "type \"1\"/\"e\" to set level essential" << endl;
			cout << "type \"2\"/\"i\" to set level info" << endl;
			cout << "type \"3\"/\"d\" to set level debug" << endl;
			cout << "type \"4\"/\"x\" to set level extreme" << endl;
			cout << "type other to return the pause menu" << endl;
			cout << "-" << endl;
			cout << endl;
			int submenuleng = 8;
			while (1) {
				cursorUp(1);
				int c = getchar();
				if (c != '\n')
					while (getchar() != '\n');
				REST_Verbose_Level l;
				if (c == '0' || c == 's') {
					l = REST_Silent;
				}
				else if (c == '1' || c == 'e') {
					l = REST_Essential;
				}
				else if (c == '2' || c == 'i') {
					l = REST_Info;
				}
				else if (c == '3' || c == 'd') {
					l = REST_Debug;
				}
				else if (c == '4' || c == 'x') {
					l = REST_Extreme;
				}
				else
				{
					cursorUp(submenuleng + 2);
					cout.setcolor(COLOR_BOLDYELLOW);
					cout << "Verbose level not set!" << endl;
					cout.setcolor(COLOR_BOLDWHITE);
					cursorDown(1);
					break;
				}

				this->SetVerboseLevel(l);
				for (int i = 0; i < fThreadNumber; i++) {
					fThreads[i]->SetVerboseLevel(l);
					for (int j = 0; j < fThreads[i]->GetProcessnum(); j++) {
						fThreads[i]->GetProcess(j)->SetVerboseLevel(l);
					}
				}
				cursorUp(submenuleng + 2);
				cout.setcolor(COLOR_BOLDGREEN);
				cout << "Verbose level has been set to " << ToString(l) << "!" << endl;
				cout.setcolor(COLOR_BOLDWHITE);
				cursorDown(1);
				break;
			}
			clearLinesAfterCursor();
			break;
		}
		else if (b == 'e')
		{
			cursorUp(mainmenuleng + 2);
			cout.setcolor(COLOR_BOLDGREEN);
			cout << "Changing number of events to process..." << endl;
			cout.setcolor(COLOR_BOLDWHITE);
			cursorDown(1);
			clearLinesAfterCursor();
			cout << "type the number you want" << endl;
			cout << "type other to return the pause menu" << endl;
			cout << "-" << endl;
			cout << endl;
			int submenuleng = 4;

			while (1) {
				cursorUp(1);
				int c = getchar();
				if (c != '\n')
				{
					char buffer[10];
					for (int i = 0; i < 10; i++) {
						buffer[i] = 0;
					}

					buffer[0] = c;
					int i = 1;
					int d = 0;
					while ((d = getchar()) != '\n') {
						if (i < 10)
							buffer[i] = d;
						i++;
					}

					int neve = StringToInteger(buffer);
					if (neve > 0) {
						eventsToProcess = neve;
						cursorUp(submenuleng + 2);
						cout.setcolor(COLOR_BOLDGREEN);
						cout << "Maximum number of events to process has been set to " << eventsToProcess << endl;
						cout.setcolor(COLOR_BOLDWHITE);
						cursorDown(1);
						break;
					}
					if (neve == 0) {
						int lastEntry = StringToInteger(GetParameter("lastEntry", "0"));
						if (lastEntry - firstEntry > 0)
							eventsToProcess = lastEntry - firstEntry;
						else
							eventsToProcess = REST_MAXIMUM_EVENTS;
						cursorUp(submenuleng + 2);
						cout.setcolor(COLOR_BOLDGREEN);
						cout << "Maximum number of events to process has been set to default (" << eventsToProcess <<")"<< endl;
						cout.setcolor(COLOR_BOLDWHITE);
						cursorDown(1);
						break;
					}

				}

				cursorUp(submenuleng + 2);
				cout.setcolor(COLOR_BOLDYELLOW);
				cout << "N events not set!" << endl;
				cout.setcolor(COLOR_BOLDWHITE);
				cursorDown(1);
				break;

			}
			clearLinesAfterCursor();
			break;
		}
		else if (b == 'n')
		{
			fProcStatus = kStep;
			break;
		}
		else if (b == 'l')
		{
			clearScreen();
			fOutputEvent->PrintEvent();
			cout << "press a key to continue..." << endl;
			int b = getchar();
			if (b != '\n')
				while (getchar() != '\n');
			clearScreen();
			break;
		}
		else if (b == 'q')
		{
			fProcStatus = kStop;
			break;
		}
		else if (b == 'p')
		{
			cursorUp(mainmenuleng + 6);
			clearLinesAfterCursor();
			fProcStatus = kNormal;
			break;
		}
		else
		{
			cursorUp(mainmenuleng + 2);
			cout.setcolor(COLOR_BOLDYELLOW);
			cout <<"Invailed option \"" << (char)b << "\" !" << endl;
			cout.setcolor(COLOR_BOLDWHITE);
			cursorDown(mainmenuleng + 1);
		}

	}
}








///////////////////////////////////////////////
/// \brief Get next event and copy it to the address of **targetevt**. 
///
/// If can, it will also set the observal value of **targettree** according to the local analysis tree.
///
/// This method is locked by mutex. There can never be two of it running simultaneously in two threads.
///
/// If there is a single thread process, the local input event will be set to the out put of this process.
/// The **targettree** will not be changed.
///
/// If not, the local input event and analysis tree will be updated after calling TTree::GetEntry(). 
/// The observables in the local tree will be copyed to the **targettree**.
///
/// Finally the data in the input event will get cloned to the **targetevt** by root streamer.
///
/// If the current entry is the last entry of the input tree, or the single thread process stops to
/// give a concret pointer as the output, the process is over. This method returns -1.
///
Int_t TRestProcessRunner::GetNextevtFunc(TRestEvent* targetevt, TRestAnalysisTree* targettree)
{
	mutexx.lock();//lock on
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
	if (fProcessedEvents >= eventsToProcess || targetevt == NULL || fProcStatus == kStop)
	{
		n = -1;
	}
	else
	{
		n = fRunInfo->GetNextEvent(targetevt, targettree);
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
/// This method is locked by mutex. There can never be two of it running simultaneously in two threads.
/// As a result threads will not write their files together, thus preventing segmentaion violation.
void TRestProcessRunner::FillThreadEventFunc(TRestThread* t)
{
	mutexx.lock();
#ifdef TIME_MEASUREMENT
	high_resolution_clock::time_point t5 = high_resolution_clock::now();
#endif
	if (t->GetOutputEvent() != NULL) {
		fOutputEvent = t->GetOutputEvent();
		//copy address of analysis tree of the given thread 
		//to the local tree, then fill the local tree
		TObjArray* branchesT;
		TObjArray* branchesL;

		if (fAnalysisTree != NULL) {
			t->GetAnalysisTree()->FillEvent(t->GetOutputEvent());
			branchesT = t->GetAnalysisTree()->GetListOfBranches();
			branchesL = fAnalysisTree->GetListOfBranches();
			for (int i = 0; i < nBranches; i++)
			{
				TBranch* branchT = (TBranch*)branchesT->UncheckedAt(i);
				TBranch* branchL = (TBranch*)branchesL->UncheckedAt(i);
				branchL->SetAddress(branchT->GetAddress());
			}
			fAnalysisTree->Fill();
		}

		if (fEventTree != NULL) {

			t->GetEventTree()->FillEvent(t->GetOutputEvent());
			branchesT = t->GetEventTree()->GetListOfBranches();
			branchesL = fEventTree->GetListOfBranches();
			for (int i = 0; i < branchesT->GetLast() + 1; i++)
			{
				TBranch* branchT = (TBranch*)branchesT->UncheckedAt(i);
				TBranch* branchL = (TBranch*)branchesL->UncheckedAt(i);
				branchL->SetAddress(branchT->GetAddress());
			}
			fEventTree->Fill();

			//cout << t->GetOutputEvent()->GetID() << endl;
		}
		fProcessedEvents++;

#ifdef TIME_MEASUREMENT
		high_resolution_clock::time_point t6 = high_resolution_clock::now();
		writeTime += (int)duration_cast<microseconds>(t6 - t5).count();
#endif



		if (fProcStatus == kStep)
		{
			fProcStatus = kPause;
			cout << "Processed events:" << fProcessedEvents << endl;
		}


	}
	mutexx.unlock();

}


///////////////////////////////////////////////
/// \brief Calling back the WriteFile() method in TRestThread.
///
/// This method is locked by mutex. There can never be two of it running simultaneously in two threads.
/// As a result threads will not write their files together, thus preventing segmentaion violation.
void TRestProcessRunner::WriteThreadFileFunc(TRestThread* t)
{
	cout << "writting files" << endl;
	mutexx.lock();
#ifdef TIME_MEASUREMENT
	high_resolution_clock::time_point t5 = high_resolution_clock::now();
#endif
	t->WriteFile();

#ifdef TIME_MEASUREMENT
	high_resolution_clock::time_point t6 = high_resolution_clock::now();
	writeTime += (int)duration_cast<microseconds>(t6 - t5).count();
#endif
	mutexx.unlock();

}


void TRestProcessRunner::ConfigOutputFile()
{
	essential << "Configuring output file, merging thread files together" << endl;
#ifdef TIME_MEASUREMENT
	ProcessInfo["ProcessTime"] = ToString(deltaTime) + "ms";
#endif


	vector<string> files_to_merge;

	//add data file
	string savemetadata = GetParameter("saveMetadata", "true");
	if (savemetadata == "true" || savemetadata == "True" || savemetadata == "yes" || savemetadata == "ON")
	{
		fTempOutputDataFile->cd();
		fRunInfo->Write();
		this->Write();
		char tmpString[256];
		if (fRunInfo->GetFileProcess() != NULL)
		{
			sprintf(tmpString, "Process-%d. %s", 0, fRunInfo->GetFileProcess()->GetName());
			fRunInfo->GetFileProcess()->Write();
		}
		for (int i = 0; i < fProcessNumber; i++) {
			sprintf(tmpString, "Process-%d. %s", i + 1, fThreads[0]->GetProcess(i)->GetName());
			fThreads[0]->GetProcess(i)->Write();
		}

	}
	if (fEventTree != NULL)fEventTree->Write();
	if (fAnalysisTree != NULL)fAnalysisTree->Write();
	fTempOutputDataFile->Close();
	//files_to_merge.push_back(fTempOutputDataFile->GetName());

	//add threads file
	//processes may have their own TObject output. They are stored in the threads file
	//these files are mush smaller that data file, so they are merged to the data file.
	for (int i = 0; i < fThreadNumber; i++) {
		TFile*f = fThreads[i]->GetOutputFile();
		if (f != NULL)
			f->Close();
		files_to_merge.push_back(f->GetName());
	}

	fRunInfo->MergeProcessFile(files_to_merge, fTempOutputDataFile->GetName());

}




//tools
void TRestProcessRunner::ResetRunTimes()
{
#ifdef TIME_MEASUREMENT
	readTime = 0; writeTime = 0; deltaTime = 0;
#endif
	time_t tt = time(NULL);
	ProcessInfo["ProcessDate"] = Spilt(ToDateTimeString(tt), " ")[0];

}


TRestEventProcess* TRestProcessRunner::InstantiateProcess(TString type, TiXmlElement* ele)
{
	TClass *cl = TClass::GetClass(type);
	if (cl == NULL)
	{
		cout << " " << endl;
		cout << "REST ERROR. Process : " << type << " not found!!" << endl;
		cout << "Please verify the process type and launch again." << endl;
		cout << "If you are not willing to use this process you can deactivate using value=\"off\"" << endl;
		cout << " " << endl;
		cout << "This process will be skipped." << endl;
		GetChar();
		return NULL;
	}
	TRestEventProcess *pc = (TRestEventProcess *)cl->New();


	pc->LoadConfigFromFile(ele, fElementGlobal);

	pc->SetRunInfo(this->fRunInfo);

	return pc;
}


void TRestProcessRunner::PrintProcessedEvents(Int_t rateE)
{

	if (fProcStatus == kNormal || fProcStatus == kIgnore) {
		clearLinesAfterCursor();

		TRestStringOutput cout;
		cout.setcolor(COLOR_BOLDWHITE);
		cout.setorientation(0);
		cout.setborder("|");
		cursorDown(1);

		Long64_t bytes = 0;
		for (auto& n : bytesAdded)
			bytes += n;
		double speedbyte = bytes / (double)printInterval*(double)1000000 / ncalculated;

		double prog = 0;
		double progsum = 0;
		for (auto& n : progAdded)
			progsum += n;
		double progspeed = progsum / ncalculated / printInterval * 1000000;

		if (eventsToProcess == REST_MAXIMUM_EVENTS && fRunInfo->GetFileProcess() != NULL)
			//Nevents is unknown, reading external data file
		{
			char* buffer=new char[100]();
			sprintf(buffer, "Reading : %ldB / %ldB (%.1fMB/s), %d Processed Events Now...", fRunInfo->GetBytesReaded(), fRunInfo->GetTotalBytes(), speedbyte / 1024 / 1024, fProcessedEvents);
			cout << buffer << endl;
			delete buffer;
			//cout << "Reading : " << fRunInfo->GetBytesReaded() << "B / " << fRunInfo->GetTotalBytes() << "B (" <<  speedbyte / 1024 / 1024 << " MB/s), ";
			//cout << fProcessedEvents << " Processed Events Now..." << endl;
			prog = fRunInfo->GetBytesReaded() / (double)fRunInfo->GetTotalBytes() * 100;
		}
		else if (fRunInfo->GetFileProcess() != NULL)
			//Nevents is known, reading external data file
		{
			char* buffer = new char[100]();
			sprintf(buffer, "Reading : %ldB / %ldB (%.1fMB/s), %d / %d Processed Events Now...", fRunInfo->GetBytesReaded(), fRunInfo->GetTotalBytes(), speedbyte / 1024 / 1024, fProcessedEvents, eventsToProcess);
			cout << buffer << endl;
			delete buffer;


			//cout << "Reading : " << fRunInfo->GetBytesReaded() << "B / " << fRunInfo->GetTotalBytes() << "B (" <<  speedbyte / 1024 / 1024 << " MB/s), ";
			//cout << fProcessedEvents << " / " << eventsToProcess << " Processed Events Now..." << endl;
			prog = fProcessedEvents / (double)eventsToProcess * 100;
		}
		else if (eventsToProcess == REST_MAXIMUM_EVENTS)
			//Nevents is unknown, reading root file
		{
			char* buffer = new char[100]();
			sprintf(buffer, "Entry : %d / %d (%.1fMB/s), %d Processed Events Now...", fRunInfo->GetCurrentEntry(), inputtreeentries, speedbyte / 1024 / 1024, fProcessedEvents);
			cout << buffer << endl;
			delete buffer;


			//cout << "Entry : " << fRunInfo->GetCurrentEntry() << " / " << inputtreeentries << " (" <<  speedbyte / 1024 / 1024 << " MB/s), ";
			//cout << fProcessedEvents << " Processed Events Now..." << endl;
			prog = fRunInfo->GetCurrentEntry() / (double)inputtreeentries * 100;
		}

		else
		{
			char* buffer = new char[100]();
			sprintf(buffer, "Entry : %d / %d (%.1fMB/s), %d / %d Processed Events Now...", fRunInfo->GetCurrentEntry(), inputtreeentries, speedbyte / 1024 / 1024, fProcessedEvents, eventsToProcess);
			cout << buffer << endl;
			delete buffer;


			//cout << "Entry : " << fRunInfo->GetCurrentEntry() << " / " << inputtreeentries << " (" <<  speedbyte / 1024 / 1024 << " MB/s), ";
			//cout << fProcessedEvents << " / " << eventsToProcess << " Processed Events Now..." << endl;
			prog = fProcessedEvents / (double)eventsToProcess * 100;
		}


		char* buffer = new char[100]();
		if (fProcStatus == kNormal)
		{
			sprintf(buffer, "ETA : %.1f Minuits... (Press \"p\" to enter pause menu)", (100 - prog_last) / progspeed / 60);
		}
		else
		{
			sprintf(buffer, "ETA : %.1f Minuits... (Pause menu disabled)", (100 - prog_last) / progspeed / 60);
		}
		cout << buffer << endl;

		sprintf(buffer, ("%.1f% [" + MakeProgressBar(prog, barlength) + "]").c_str(), prog);
		cout << buffer << endl;

		delete buffer;

		cursorUp(4);

		bytesAdded[poscalculated] = fRunInfo->GetBytesReaded() - bytesReaded_last;
		bytesReaded_last = fRunInfo->GetBytesReaded();
		progAdded[poscalculated] = prog - prog_last;
		prog_last = prog;

		poscalculated++;
		if (poscalculated >= ncalculated)
			poscalculated -= ncalculated;
	}

}

string TRestProcessRunner::MakeProgressBar(int progress100, int length)
{
	string progressbar(length, '-');
	int n = (double)progress100 / 100 * length;
	if (n < 0)n = 0;
	if (n > length + 1)n = length + 1;
	for (int i = 0; i < n; i++) {
		progressbar[i] = '=';
	}
	if (n < length + 1)progressbar[n] = '>';
	return progressbar;
}




TRestEvent* TRestProcessRunner::GetInputEvent() { return fRunInfo->GetInputEvent(); }

TRestAnalysisTree* TRestProcessRunner::GetAnalysisTree() { return fRunInfo->GetAnalysisTree(); }