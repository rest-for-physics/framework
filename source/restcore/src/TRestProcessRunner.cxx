#include "TRestManager.h"
#include "TRestThread.h"

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


	fThreadNumber = 0;
	firstEntry = 0;
	eventsToProcess = 0;
	fProcessedEvents = 0;
	fProcessNumber = 0;
	
}

void TRestProcessRunner::BeginOfInit() 
{
	if (fHostmgr != NULL)
	{
		fRunInfo = fHostmgr->GetRunInfo();
		if (fRunInfo== NULL) {
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
	else if (eventsToProcess > 0&& lastEntry - firstEntry > 0&&lastEntry - firstEntry !=eventsToProcess) {
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
	fOutputItem = Spilt(GetParameter("treeBranches", ""),":");
	if (fThreadNumber < 1)fThreadNumber = 1;

	fTempOutputDataFile = new TFile("rest_anaTree_tmp.root","recreate");
	for (int i = 0; i < fThreadNumber; i++)
	{
		TRestThread* t = new TRestThread();
		t->SetTRestRunner(this);
		t->SetBranchConfig(fOutputItem);
		t->SetThreadId(i);
		t->SetVerboseLevel(fVerboseLevel);
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

		info << "adding process " << processType << " \"" << processName <<"\""<< endl;
		for (int i = 0; i < fThreadNumber; i++) 
		{
			TRestEventProcess* p=InstantiateProcess(processType, e);
			if (p->singleThreadOnly()) 
			{
				//FIX ME: what if single thread process is not file process?
				//eg. event-viewer process
				fRunInfo->SetExtProcess((TRestExternalFileProcess*)p);
				break;
			}
			else
			{
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
	if (fThreads[0]->GetProcessnum() > 0)
	{
		if (fThreads[0]->ValidateInput(fRunInfo->GetFileProcess()) == -1) exit(1);
		if (fThreads[0]->ValidateChain() == -1)exit(1);
	}
	fInputEvent = fRunInfo->GetInputEvent();

	ReadProcInfo();

	//print metadata
	if (fVerboseLevel >= REST_Essential) {
		if (fRunInfo->GetFileProcess() != NULL)fRunInfo->GetFileProcess()->PrintMetadata();

		for (int i = 0; i < fThreads[0]->GetProcessnum(); i++)
		{
			fThreads[0]->GetProcess(i)->PrintMetadata();
		}
	}
	info << this->ClassName() << ": " << fThreads[0]->GetProcessnum() << " Processes Loaded, " << fThreadNumber << " Threads Requested." << endl;

}

void TRestProcessRunner::ReadProcInfo() 
{
	if (fRunInfo->GetFileProcess() != NULL)
		ProcessInfo["FirstProcess"] = fRunInfo->GetFileProcess()->GetName();
	int n = fThreads[0]->GetProcessnum();
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
	if (fProcessNumber > 0) {
		debug << "Initializing processes in threads. " << fThreadNumber << " threads are requested" << endl;
		fRunInfo->ResetEntry();
		for (int i = 0; i < fThreadNumber; i++)
		{
			fThreads[i]->PrepareToProcess();
		}
		//copy thread tree to local
		fTempOutputDataFile->cd();
		fAnalysisTree = (TRestAnalysisTree*)fThreads[0]->GetAnalysisTree()->Clone();
		fAnalysisTree->Write();
		nBranches = fAnalysisTree->GetListOfBranches()->GetEntriesFast();

		//reset runner
		this->ResetRunTimes();
		fProcessedEvents = 0;
		fRunInfo->ResetEntry();

#ifdef TIME_MEASUREMENT
		high_resolution_clock::time_point t3 = high_resolution_clock::now();
#endif

		//start the thread!
		fout << this->ClassName() << ": Starting the Process.." << endl;
		for (int i = 0; i < fThreadNumber; i++)
		{
			fThreads[i]->StartThread();
		}

		while (fRunInfo->GetInputEvent() != NULL&&eventsToProcess > fProcessedEvents)
		{

#ifdef WIN32
			_sleep(50);
#else
			usleep(100000);
#endif

			//cout << eventsToProcess << " " << fProcessedEvents << " " << lastEntry << " " << fCurrentEvent << endl;
			//cout << fProcessedEvents << "\r";
			//printf("%d processed events now...\r", fProcessedEvents);
			//fflush(stdout);
			PrintProcessedEvents(100);
		}


#ifdef TIME_MEASUREMENT
		high_resolution_clock::time_point t4 = high_resolution_clock::now();
		deltaTime = (int)duration_cast<microseconds>(t4 - t3).count();
#endif

		fout << this->ClassName() << ": " << fProcessedEvents << " processed events" << endl;

#ifdef TIME_MEASUREMENT
		info << "Total processing time (read&process): " << ((Double_t)(deltaTime - writeTime)) / 1000. << " ms" << endl;
		info << "Total write time to disk (per event) : " << ((Double_t)writeTime) / fProcessedEvents / 1000. << " ms" << endl;
		info << "Average read time from disk : " << ((Double_t)readTime) / fProcessedEvents / 1000. << " ms" << endl;
#endif


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
				finish = finish&&fThreads[i]->Finished();
			}
			if (finish)break;
		}

	}
	else
	{
		info << "skipping null process chain..." << endl;
	}


	ConfigOutputFile();

	
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

#ifdef TIME_MEASUREMENT
	high_resolution_clock::time_point t1 = high_resolution_clock::now();
#endif
	int n;
	if (fProcessedEvents >= eventsToProcess || targetevt == NULL)
	{
		n= -1;
	}
	else
	{
		fProcessedEvents++;
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
	//copy address of analysis tree of the given thread 
	//to the local tree, then fill the local tree
	TObjArray* branchesT = t->GetAnalysisTree()->GetListOfBranches();
	TObjArray* branchesL = fAnalysisTree->GetListOfBranches();
	for (int i = 0; i < nBranches; i++) 
	{
		TBranch* branchT = (TBranch*)branchesT->UncheckedAt(i);
		TBranch* branchL = (TBranch*)branchesL->UncheckedAt(i);
		((TRestBranch*)branchL)->SetBranchAddressQuick(branchT->GetAddress());
	}
	fAnalysisTree->Fill();

#ifdef TIME_MEASUREMENT
	high_resolution_clock::time_point t6 = high_resolution_clock::now();
	writeTime += (int)duration_cast<microseconds>(t6 - t5).count();
#endif
	mutexx.unlock();

}


///////////////////////////////////////////////
/// \brief Calling back the WriteFile() method in TRestThread.
///
/// This method is locked by mutex. There can never be two of it running simultaneously in two threads.
/// As a result threads will not write their files together, thus preventing segmentaion violation.
void TRestProcessRunner::WriteThreadFileFunc(TRestThread* t)
{
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
	debug << "Configuring output file, merging thread files together" << endl;
#ifdef TIME_MEASUREMENT
	ProcessInfo["ProcessTime"] = ToString(deltaTime) + "ms";
#endif

	if (fProcessNumber > 0) {
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
				for (int i = 0; i < fProcessNumber-1; i++) {
					sprintf(tmpString, "Process-%d. %s", i + 1, fThreads[0]->GetProcess(i)->GetName());
					fThreads[0]->GetProcess(i)->Write();
				}
			}
			else
			{
				for (int i = 0; i < fProcessNumber; i++) {
					sprintf(tmpString, "Process-%d. %s", i + 1, fThreads[0]->GetProcess(i)->GetName());
					fThreads[0]->GetProcess(i)->Write();
				}
			}

		}
		fAnalysisTree->Write();
		fTempOutputDataFile->Close();
		files_to_merge.push_back(fTempOutputDataFile->GetName());

		//add threads file
		//processes may have their own TObject output. They are stored in the threads file
		//these files are mush smaller that data file, so they are merged to the data file.
		for (int i = 0; i < fThreadNumber; i++) {
			TFile*f = fThreads[i]->GetOutputFile();
			if(f!=NULL)
				files_to_merge.push_back(f->GetName());
		}

		fRunInfo->MergeProcessFile(files_to_merge);
	}
	else
	{
		fRunInfo->PassOutputFile();
	}
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


TRestEventProcess* TRestProcessRunner::InstantiateProcess(TString type,TiXmlElement* ele)
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

	if (eventsToProcess==2E9)
	{
		printf("%d processed events now...\r", (fProcessedEvents / rateE)*rateE);
		fflush(stdout);
	}
	else
	{
		printf("Completed : %.2lf %%\r", (100.0 * (Double_t)fProcessedEvents) / eventsToProcess);
		fflush(stdout);
	}


}






TRestEvent* TRestProcessRunner::GetInputEvent() { return fRunInfo->GetInputEvent(); }

TRestAnalysisTree* TRestProcessRunner::GetAnalysisTree() { return fRunInfo->GetAnalysisTree(); }