#include "TRestManager.h"
#include "TRestThread.h"
#include "Math/MinimizerOptions.h"
#include "TMinuitMinimizer.h"
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
		eventsToProcess = fRunInfo->GetTotalEntries();
		lastEntry = fRunInfo->GetTotalEntries();
	}
	else
	{
		warning << "error setting of event number" << endl;
		eventsToProcess = eventsToProcess > 0 ? eventsToProcess : fRunInfo->GetTotalEntries();
		firstEntry = firstEntry > 0 ? firstEntry : 0;
		lastEntry = lastEntry == firstEntry + eventsToProcess ? lastEntry : fRunInfo->GetTotalEntries();
	}
	fRunInfo->SetCurrentEntry(firstEntry);


	fThreadNumber = StringToDouble(GetParameter("threadNumber", "1"));
	fOutputItem = Spilt(GetParameter("treeBranches", "inputevent:processevent:outputevent:inputanalysis:outputanalysis"), ":");
	if (fThreadNumber < 1)fThreadNumber = 1;


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
			if (p->InheritsFrom("TRestRawToSignalProcess"))
			{
				fRunInfo->SetExtProcess(p);
				return 0;
			}
			else
			{
				if (p->singleThreadOnly() && fThreadNumber > 1)
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
					fProcStatus = kIgnore;
					break;
				}
				fThreads[i]->AddProcess(p);
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
		ProcessInfo["FirstProcess"] = fRunInfo->GetFileProcess()->GetName();
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
	bool testrun = ToUpper(GetParameter("testRun", "ON")) == "ON"|| ToUpper(GetParameter("testRun", "ON")) == "TRUE";
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
	else if(fVerboseLevel >= REST_Essential)
	{
		if (fRunInfo->GetFileProcess() != NULL)
		{
			essential << "(external) " << fRunInfo->GetFileProcess()->ClassName() << " : " << fRunInfo->GetFileProcess()->GetName()<< endl;
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

	//set root mutex
	//!!!!!!!!!!!!Important!!!!!!!!!!!!
	ROOT::Math::MinimizerOptions::SetDefaultMinimizer("Minuit");
	TMinuitMinimizer::UseStaticMinuit(false);
	if (gGlobalMutex == NULL) {
		gGlobalMutex = new TMutex(true);
		gROOTMutex = gGlobalMutex;
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

	while (fRunInfo->GetInputEvent() != NULL && eventsToProcess > fProcessedEvents)
	{
		if (fProcStatus != kIgnore && kbhit())//if keyboard inputs
		{
			int a = getchar();//get char
			if(a!='\n')
				while (getchar() != '\n');//clear buffer
			if (a == 'p') {
				fProcStatus = kPause;
				cout << endl;
				cout << "process paused!" << endl;
				cout << "you can change some settings here" << endl;
			}
			else if(a=='q'){
				fProcStatus = kStop;
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
		usleep(200000);
#endif

		//cout << eventsToProcess << " " << fProcessedEvents << " " << lastEntry << " " << fCurrentEvent << endl;
		//cout << fProcessedEvents << "\r";
		//printf("%d processed events now...\r", fProcessedEvents);
		//fflush(stdout);
		PrintProcessedEvents(100);
	}

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

	cout << "--------------menu--------------" << endl;
	cout << "\"v\" : change the verbose level" << endl;
	cout << "\"e\" : change the number of events to process" << endl;
	cout << "\"n\" : push foward one event, then pause" << endl;
	cout << "\"l\" : print the latest processed event" << endl;
	cout << "\"q\" : stop and quit the process" << endl;
	cout << "press \"p\" to continue process..." << endl;
	cout << endl;
	while (1) {
		int b = getchar();
		while (getchar() != '\n');

		if (b == 'v')
		{
			cout << endl;
			cout << "changing verbose level for all the processes..." << endl;
			cout << "type \"0\"/\"s\" to set level silent" << endl;
			cout << "type \"1\"/\"e\" to set level essential" << endl;
			cout << "type \"2\"/\"i\" to set level info" << endl;
			cout << "type \"3\"/\"d\" to set level debug" << endl;
			cout << "type \"4\"/\"x\" to set level extreme" << endl;
			cout << "type other to return the pause menu" << endl;
			cout << endl;
			while (1) {
				int c = getchar();
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
					break;
				}

				this->SetVerboseLevel(l);
				for (int i = 0; i < fProcessNumber; i++) {
					fThreads[i]->SetVerboseLevel(l);
					for (int j = 0; j < fThreads[i]->GetProcessnum(); j++) {
						fThreads[i]->GetProcess(j)->SetVerboseLevel(l);
					}
				}
				fout << "verbose level has been set to " << ToString(l) << endl;
				cout << endl;
				break;
			}
			break;
		}
		else if (b == 'e')
		{
			cout << endl;
			cout << "changing number of events to process..." << endl;
			cout << "type the number you want" << endl;
			cout << "type other to return the pause menu" << endl;
			cout << endl;

			string s;

			cin >> s;
			cin.ignore();
			
			if (isANumber(s)) {
				eventsToProcess = StringToInteger(s);
				fout << "maximum number of events to process has been set to " << eventsToProcess << endl;
				cout << endl;
			}
			break;
		}
		else if (b == 'n')
		{
			fProcStatus = kStep;
			break;
		}
		else if (b == 'l')
		{
			fOutputEvent->PrintEvent();
			cout << endl;
			break;
		}
		else if (b == 'q')
		{
			fProcStatus = kStop;
			break;
		}
		else if (b == 'p')
		{
			fProcStatus = kNormal;
			break;
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
	if (fProcessedEvents >= eventsToProcess || targetevt == NULL || fProcStatus==kStop)
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
			cout << "Processed events:" <<fProcessedEvents << endl;
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
	if (fProcStatus == kNormal) {
		if (eventsToProcess == 2E9)
		{
			printf("%d processed events now...(Press \"q\" to stop, Press \"p\" to pause)\r", (fProcessedEvents));
			fflush(stdout);
		}
		else
		{
			printf("Completed : %.2lf %%(Press \"q\" to stop, Press \"p\" to pause)\r", (100.0 * (Double_t)fProcessedEvents) / eventsToProcess);
			fflush(stdout);
		}
	}
	if (fProcStatus == kIgnore) {
		if (eventsToProcess == 2E9)
		{
			printf("%d processed events now...\r", (fProcessedEvents));
			fflush(stdout);
		}
		else
		{
			printf("Completed : %.2lf %%\r", (100.0 * (Double_t)fProcessedEvents) / eventsToProcess);
			fflush(stdout);
		}
	}

}






TRestEvent* TRestProcessRunner::GetInputEvent() { return fRunInfo->GetInputEvent(); }

TRestAnalysisTree* TRestProcessRunner::GetAnalysisTree() { return fRunInfo->GetAnalysisTree(); }