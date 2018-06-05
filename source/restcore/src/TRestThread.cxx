//////////////////////////////////////////////////////////////////////////
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

ClassImp(TRestThread);

///////////////////////////////////////////////
/// \brief Set variables by default during initialization.
///
void TRestThread::Initialize()
{
	fOutputEvent = NULL;
	fInputEvent = NULL;

	fAnalysisTree = NULL;
	fEventTree = NULL;

	fOutputFile = NULL;

	fProcessChain.clear();

	isFinished = false;
}


///////////////////////////////////////////////
/// \brief Check if the output of **input** matches the input event of process chain.
///
/// Sometimes there is an external process ahead of the threaded processes. We need to check 
/// if the event type matches. 
///
Int_t TRestThread::ValidateInput(TRestEvent* input)
{

	if (input == NULL&&fProcessChain[0]->GetInputEvent() != NULL)
	{
		cout << "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++" << endl;
		cout << "REST ERROR (ValidateInput): Given input event is null!" << endl;
		cout << "If you have a external process, make sure its output event is instantiated" << endl;
		cout << "in the Initialize() function." << endl;
		cout << "If you are using rest file as imput, make sure its contains event tree." << endl;
		cout << "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++" << endl;
		GetChar();
		return -1;
	}
	//general event input is accepted
	if ((string)fProcessChain[0]->GetInputEvent()->ClassName() == "TRestEvent")
		return 0;
	if (input->ClassName() != fProcessChain[0]->GetInputEvent()->ClassName())
	{
		cout << "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++" << endl;
		cout << "REST ERROR (ValidateInput): Input event type does not match the input of the " << endl;
		cout << "first non-external process in the chain!" << endl;
		cout << "The gievn input type: " <<input->ClassName() << endl;
		cout << "Input of the first non-external process in the chain: " << fProcessChain[0]->GetInputEvent()->ClassName() << endl;
		cout << "No events will be processed. Please correct event process input/output." << endl;
		cout << "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++" << endl;
		GetChar();
		return -1;
	}
	return 0;
}

///////////////////////////////////////////////
/// \brief Check if the input/output of each process in the process chain matches
///
/// The outevent of the previous process should be of the same type as the input event 
/// of the later process. Both of them cannot be null.
///
Int_t TRestThread::ValidateChain()
{

	for (unsigned int i = 0; i < fProcessChain.size() - 1; i++)
	{
		TRestEvent* outEvent = fProcessChain[i]->GetOutputEvent();
		TRestEvent* inEvent = fProcessChain[i + 1]->GetInputEvent();

		if (outEvent == NULL)
		{
			cout << "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++" << endl;
			cout << "REST ERROR : output Event of " << fProcessChain[i]->GetName() << " has not been initialized!" << endl;
			cout << "You should instantiate the i-o event into type of TRestEvent in the Initialize() function" << endl;
			cout << "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++" << endl;
			GetChar();
			return -1;
		}
		if (inEvent == NULL)
		{
			cout << "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++" << endl;
			cout << "REST ERROR : input Event of " << fProcessChain[i + 1]->GetName() << " has not been initialized!" << endl;
			cout << "You should instantiate the i-o event into type of TRestEvent in the Initialize() function" << endl;
			cout << "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++" << endl;
			GetChar();
			return -1;
		}
		TString outEventType = outEvent->ClassName();
		TString inEventType = inEvent->ClassName();
		//general input type
		if (inEventType == "TRestEvent" || outEventType == "TRestEvent")
			return 0;
		if (outEventType != inEventType)
		{
			cout << "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++" << endl;
			cout << "REST ERROR : Event process input/output does not match" << endl;
			cout << "The event output for process " << fProcessChain[i]->GetName() << " is " << outEventType << endl;
			cout << "The event input for process " << fProcessChain[i + 1]->GetName() << " is " << inEventType << endl;
			cout << "No events will be processed. Please correct event process input/output." << endl;
			cout << "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++" << endl;
			GetChar();
			return -1;
		}
	}
	return 0;



}


void TRestThread::SetThreadId(Int_t id)
{
	fThreadId = id;
	if (fThreadId != 0 && fVerboseLevel > REST_Essential)
		fVerboseLevel = REST_Essential;
}


TRestAnalysisTree* tempTree;
///////////////////////////////////////////////
/// \brief Make a test run of our process chain
///
/// The reason we use test run is that we need to determine the real output event 
/// address of a process chain. This is because when we write our code like this:
/// \code
/// TRestEvent* TRestRawSignalAnalysisProcess::ProcessEvent( TRestEvent *evInput )
/// {
/// 	fSignalEvent = (TRestRawSignalEvent *)evInput;
/// 	fOutputEvent = fSignalEvent;
///		...
/// }
/// \endcode
/// The output event address can be changed after running. 
/// 
/// Test run calls TRestEventProcess::ProcessEvent() first, and uses the output of
/// TRestEventProcess::GetOutputEvent() as the next process's input event. This avoids
/// NULL returned events from ProcessEvent due to cut. In debug mode, we can also 
/// observe a process sequence printed by this method
///
/// returns false when fOutputEvent is null after 5 times of retry, returns true
/// when fOutputEvent address is determined.
bool TRestThread::TestRun() 
{
	debug << "Processing ..." << endl;
	for (int i = 0; i < 5; i++) {
		TRestEvent* ProcessedEvent = fInputEvent;
		debug << "Test run " << i << " : Input Event ---- " << fInputEvent->ClassName() << "(" << fInputEvent << ")" << endl;
		for (unsigned int j = 0; j < fProcessChain.size(); j++)
		{
			debug << j << " " << fProcessChain[j]->GetName() << "(" << fProcessChain[j]->ClassName() << ")";
			fProcessChain[j]->BeginOfEventProcess();
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
		}
		else
		{
			debug << "Null output, trying again" << endl;
		}
	}
	if (fOutputEvent == NULL)
	{
		//fOutputEvent = fProcessChain[fProcessChain.size() - 1]->GetOutputEvent();
		return false;
	}
	return true;
}


///////////////////////////////////////////////
/// \brief Propare some thing before we can satrt process
///
/// This method will:
/// 1. Setup the processes in process chain:(set analysis tree, set readonly, call the method InitProcess())
/// 2. Instantiate the input event and make a test run, calling TestRun(). The address of output event 
/// is fixed after the test run.
/// 3. Open the output file and create output tree inside it. Creating branches according to the output level.
/// 4. Reset the processes by calling InitProcess() again
///
/// Note: this methed runs under single thread node, so there is no conflict when creating files.
void TRestThread::PrepareToProcess(bool testrun)
{

	if (fProcessChain.size() > 0)
	{
		stringstream Filename;
		Filename << "rest_thread_tmp" << fThreadId << ".root";
		debug << "Creating file : " << Filename.str() << endl;
		fOutputFile = new TFile(Filename.str().c_str(), "recreate");
		fOutputFile->SetCompressionLevel(0);

		tempTree = new TRestAnalysisTree("AnalysisTree_tmp", "anaTree_tmp");
		for (unsigned int i = 0; i < fProcessChain.size(); i++)
		{
			debug << fProcessChain[i]->ClassName() << endl;
			fProcessChain[i]->SetAnalysisTree(tempTree);
			for (unsigned int j = 0; j < fProcessChain.size(); j++)
			{
				fProcessChain[i]->SetFriendProcess(fProcessChain[j]);
			}
			fProcessChain[i]->InitProcess();
		}


		fInputEvent = (TRestEvent*)fProcessChain[0]->GetInputEvent();
		if (fInputEvent == NULL) {
			error << "REST ERROR(" << "In thread " << fThreadId << ")::Input event of the first process is not specified!" << endl;
			GetChar();
			exit(1);
		}
		fInputEvent = (TRestEvent*)fInputEvent->Clone();
		if (fHostRunner->GetNextevtFunc(fInputEvent, tempTree) != 0)
		{
			error << "REST ERROR(" << "In thread " << fThreadId << ")::Failed to get the first input event, process cannot start!" << endl;
			GetChar();
			exit(1);
		}

		//test run
		if (testrun) {
			debug << "Test Run..." << endl;
			if (!TestRun()) {
				error << "REST WARNING(" << "In thread " << fThreadId << ")::test run failed!" << endl;
				error << "One of the processes has NULL pointer fOutputEvent!" << endl;
				if(fVerboseLevel<REST_Debug)
					error << "To see more detail, turn on debug mode for TRestProcessRunner!" << endl;
				exit(0);
			}
			debug << "Test Run complete!" << endl;
		}
		else
		{
			debug << "Setting output event address with out test run... This may cause empty event problem!" << endl;
			fOutputEvent = fProcessChain[fProcessChain.size() - 1]->GetOutputEvent();
		}
		delete tempTree;

		//////////////////////////////////////////
		//create dummy tree to store branch addresses.
		debug << "Creating Analysis Tree..." << endl;
		fAnalysisTree = new TRestAnalysisTree("AnalysisTree_"+ToString(fThreadId), "dummyTree");
		fAnalysisTree->CreateEventBranches();
		fEventTree = new TTree((TString)"EventTree_" + ToString(fThreadId), "dummyTree");
		//fEventTree->CreateEventBranches();
		for (unsigned int i = 0; i < fProcessChain.size(); i++)
		{
			fProcessChain[i]->SetAnalysisTree(fAnalysisTree);
			fProcessChain[i]->ConfigAnalysisTree();
		}
		vector<pair<TString, TRestEvent*>> branchesToAdd;
		//avoid duplicated branch
		//if event type is same, we only create branch for the last of this type event
		for (int i = 0; i < fTreeBranchDef.size(); i++) 
		{
			if (fTreeBranchDef[i] == "inputevent")
			{
				TRestEvent*evt = fInputEvent;
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
				for (unsigned int i = 0; i < fProcessChain.size(); i++)
				{
					TRestEvent*evt = fProcessChain[i]->GetOutputEvent();
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
			if (fTreeBranchDef[i] == "outputevent")
			{
				TRestEvent*evt = fOutputEvent;
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
			if (fTreeBranchDef[i] == "inputanalysis")
			{
				if (fHostRunner->GetAnalysisTree() != NULL)
					fAnalysisTree->ConnectObservables(fHostRunner->GetAnalysisTree());
			}

		}
		auto iter = branchesToAdd.begin();
		while (iter != branchesToAdd.end()) {
			fEventTree->Branch(iter->first, iter->second->ClassName(), iter->second);
			iter++;
		}

		if (fEventTree->GetListOfBranches()->GetLast() < 0)//if no event branches are created
		{
			delete fEventTree; fEventTree = NULL;
		}

		fAnalysisTree->CreateObservableBranches();


		//create output temp file for process-defined output object

		fOutputFile->cd();
		fOutputFile->Clear();
		for (unsigned int i = 0; i < fProcessChain.size(); i++)
		{
			fProcessChain[i]->InitProcess();
		}


		debug << "Thread " << fThreadId << " Ready!" << endl;
	}
	else
	{
		string tmp = fHostRunner->GetInputEvent()->ClassName();
		fInputEvent = (TRestEvent*)TClass::GetClass(tmp.c_str())->New();
		fOutputEvent = fInputEvent;
		stringstream Filename;
		Filename << "rest_thread_tmp" << fThreadId << ".root";
		fOutputFile = new TFile(Filename.str().c_str(), "recreate");
		fOutputFile->SetCompressionLevel(0);
		fOutputFile->cd();


		debug << "Creating Analysis Tree..." << endl;
		fAnalysisTree = new TRestAnalysisTree("AnalysisTree_" + ToString(fThreadId), "dummyTree");
		fAnalysisTree->CreateEventBranches();
		fEventTree = new TTree((TString)"EventTree_" + ToString(fThreadId), "dummyTree");
		//fEventTree->CreateEventBranches();

		for (int i = 0; i < fTreeBranchDef.size(); i++)
		{
			if (fTreeBranchDef[i] == "outputevent")
			{
				TString BranchName = (TString)fInputEvent->GetName() + "Branch";
				if (fEventTree->GetBranch(BranchName) == NULL)//avoid duplicated branch
					fEventTree->Branch(BranchName, fInputEvent->ClassName(), fInputEvent);
			}
			//currently external process analysis is not supported!
		}
		if (fEventTree->GetListOfBranches()->GetLast() < 1)
		{
			delete fEventTree; fEventTree = NULL;
		}
		fAnalysisTree->CreateObservableBranches();


	}

}

///////////////////////////////////////////////
/// \brief The main function of this class. Thread will run this function until the end.
///
/// It will start a loop, calling GetNextevtFunc(), ProcessEvent(), and FillThreadEventFunc() repeatedly.
/// If the function GetNextEvent() returns false, the loop will break, meaning that we are at the
/// end of the process. Before return it will set "isFinished" to true.
///
/// Note: The methods GetNextevtFunc() and FillThreadEventFunc() are all from  TRestProcessRunner. 
/// The later two will call back the method FillEvent(), WriteFile() in this class. The idea 
/// to do so is to make a unified managemenet of these i-o related methods. In TRestRun the three
/// methods are mutex locked. They will be paused until the host run allows it to run. This prevents
/// segmentation violation due to simultaneously read/write. 
void TRestThread::StartProcess()
{
	isFinished = false;


	while (fHostRunner->GetNextevtFunc(fInputEvent, fAnalysisTree) == 0)
	{
		ProcessEvent();
		if(fOutputEvent!=NULL)
			fHostRunner->FillThreadEventFunc(this);
	}



	//fHostRunner->WriteThreadFileFunc(this);
	isFinished = true;
}

///////////////////////////////////////////////
/// \brief Create a thread with the method StartProcess().
///
/// The thread will be detached afterwards. 
void TRestThread::StartThread()
{

	t = thread(&TRestThread::StartProcess, this);
	t.detach();
	//t.join();
}


///////////////////////////////////////////////
/// \brief Process a single event. 
///
/// This is the only method that can running purely under multi thread mode here. 
/// It gives the input event to the first process in process chain, then it 
/// gives the process result to the next process, so on. Finally it gets a result
/// and saves it in the local output event.
void TRestThread::ProcessEvent()
{


	TRestEvent* ProcessedEvent = fInputEvent;
	for (unsigned int j = 0; j < fProcessChain.size(); j++)
	{
		fProcessChain[j]->BeginOfEventProcess();


		ProcessedEvent = fProcessChain[j]->ProcessEvent(ProcessedEvent);


		fProcessChain[j]->EndOfEventProcess();

		if (ProcessedEvent == NULL) break;

	}

	fOutputEvent = ProcessedEvent;

}


///////////////////////////////////////////////
/// \brief Write and close the output file
///
/// This method is called back at the end of TRestProcessRunner::RunProcess() in TRestProcessRunner. 
void TRestThread::WriteFile()
{
	info << "TRestThread : Writting temp file" << endl;

	if (fOutputFile == NULL) return;

	fOutputFile->cd();
	for (unsigned int i = 0; i < fProcessChain.size(); i++)
	{
		fProcessChain[i]->EndProcess();//the processes must call "object->Write()" in this method
	}

	delete fAnalysisTree;
	//fOutputFile->Close();
}