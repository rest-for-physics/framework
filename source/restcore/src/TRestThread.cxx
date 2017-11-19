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
/// \brief Check if the output of **procinput** matches the input event of process chain.
///
/// Sometimes there is an external process ahead of the threaded processes. We need to check 
/// if the event type matches. 
///
Int_t TRestThread::ValidateInput(TRestEventProcess* procinput)
{
	if (procinput == NULL)
	{
		cout << "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++" << endl;
		cout << "REST ERROR : The Process " << procinput->GetName() << " is null!" << endl;
		cout << "Maybe you should instantiate it first" << endl;
		cout << "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++" << endl;
		GetChar();
		return -1;
	}
	TRestEvent* inputevt = procinput->GetOutputEvent();
	if (inputevt == NULL&&fProcessChain[0]->GetInputEvent() != NULL)
	{
		cout << "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++" << endl;
		cout << "REST ERROR : output Event of " << procinput->GetName() << " is null!" << endl;
		cout << "Maybe you should instantiate the i-o event into type of TRestEvent in the Initialize() function" << endl;
		cout << "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++" << endl;
		GetChar();
		return -1;
	}
	if (fProcessChain[0]->GetInputEvent()->ClassName() == "TRestEvent")
		return 0;
	if (inputevt->ClassName() != fProcessChain[0]->GetInputEvent()->ClassName())
	{
		cout << "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++" << endl;
		cout << "REST ERROR : Event process input/output does not match" << endl;
		cout << "The event output for process " << procinput->GetName() << " is " << inputevt->ClassName() << endl;
		cout << "The event input for process " << fProcessChain[0]->GetInputEvent()->GetName() << " is " << fProcessChain[0]->GetInputEvent()->ClassName() << endl;
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
		if(inEventType=="TRestEvent"||outEventType=="TRestEvent")
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

///////////////////////////////////////////////
/// \brief Propare some thing before we can satrt process
///
/// This method will:
/// 1. Setup the processes in process chain:(set analysis tree, set readonly, call the method InitProcess())
/// 2. Instantiate the input event and make a test run, calling GetNextevtFunc() and ProcessEvent()
/// in sequence. The address of output event is fixed after the test run.
/// 3. Open the output file and create output tree inside it. Creating branches according to the output level.
/// 4. Reset the processes by calling InitProcess() again
///
/// Note: this methed runs under single thread node, so there is no conflict when creating files.
void TRestThread::PrepareToProcess()
{

	if (fProcessChain.size() > 0)
	{
		debug << "Preparing Thread " << fThreadId << "..." << endl;
		TRestAnalysisTree* tempTree = new TRestAnalysisTree("AnalysisTree_tmp", "anaTree_tmp");
		for (unsigned int i = 0; i < fProcessChain.size(); i++)
		{
			debug << fProcessChain[i]->ClassName() << endl;
			fProcessChain[i]->SetAnalysisTree(tempTree);
			fProcessChain[i]->InitProcess();
		}

		//test run
		debug << "Test Run..." << endl;

		fInputEvent = fProcessChain[0]->GetInputEvent();
		if (fHostRunner->GetNextevtFunc(fInputEvent, tempTree) != 0)
		{
			cout << "REST ERROR(" << "In thread " << fThreadId << ")::Failed to get the first input event, process cannot start!" << endl;
			GetChar();
			exit(1);
		}
		debug << "Processing Test Event" << endl;
		for (int i = 0; i < 5; i++) {
			ProcessEvent();
			if (fOutputEvent != NULL) {
				break;
			}
		}
		if (fOutputEvent == NULL)
		{
			error << "REST ERROR(" << "In thread " << fThreadId << ")::Process result is null after 5 times of retry. REST cannot determing the output event!" << endl;
			GetChar();
			exit(1);
		}
		delete tempTree;
		debug << "Test Run success!" << endl;



		//////////////////////////////////////////
		//create dummy tree to store branch addresses.
		debug << "Creating Analysis Tree..." << endl;
		fAnalysisTree = new TRestAnalysisTree("AnalysisTree_"+ToString(fThreadId), "dummyTree");
		fAnalysisTree->CreateEventBranches();
		fEventTree = new TRestAnalysisTree("EventTree_" + ToString(fThreadId), "dummyTree");
		fEventTree->CreateEventBranches();

		for (unsigned int i = 0; i < fProcessChain.size(); i++)
		{
			fProcessChain[i]->SetAnalysisTree(fAnalysisTree);
		}

		for (int i = 0; i < fTreeBranchDef.size(); i++) 
		{
			if (fTreeBranchDef[i] == "inputevent") 
			{
				TString BranchName = (TString)fInputEvent->GetName() + "Branch";
				fEventTree->Branch(BranchName, fInputEvent->ClassName(), fInputEvent);

			}
			if (fTreeBranchDef[i] == "processevent") 
			{
				for (unsigned int i = 0; i < fProcessChain.size(); i++)
				{
					TRestEvent*processevent = fProcessChain[i]->GetOutputEvent();
					TString BranchName = (TString)processevent->GetName() + "Branch";
					fEventTree->Branch(BranchName, processevent->ClassName(), processevent);
				}
			}
			if (fTreeBranchDef[i] == "outputevent")
			{
				TString BranchName = (TString)fOutputEvent->GetName() + "Branch";
				fEventTree->Branch(BranchName, fOutputEvent->ClassName(), fOutputEvent);
			}
			if (fTreeBranchDef[i] == "outputanalysis")
			{
				for (unsigned int i = 0; i < fProcessChain.size(); i++)
				{
					fProcessChain[i]->ConfigAnalysisTree();
				}
			}
			if (fTreeBranchDef[i] == "inputanalysis")
			{
				if (fHostRunner->GetAnalysisTree() != NULL)
					fAnalysisTree->ConnectObservables(fHostRunner->GetAnalysisTree());
			}

		}

		if (fEventTree->GetListOfBranches()->GetLast()+1 < 6)
		{
			delete fEventTree; fEventTree = NULL;
		}
		
		fAnalysisTree->CreateObservableBranches();

		//create output temp file for process-defined output object
		stringstream Filename;
		Filename << "rest_thread_tmp" << fThreadId << ".root";
		debug << "Creating file : " << Filename.str() << endl;
		fOutputFile = new TFile(Filename.str().c_str(), "recreate");
		fOutputFile->SetCompressionLevel(0);
		fOutputFile->cd();

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
		fAnalysisTree = new TRestAnalysisTree("AnalysisTree", "anaTree");
		fAnalysisTree->CreateEventBranches();
		TString BranchName = (TString)fInputEvent->GetName() + "Branch";
		fAnalysisTree->Branch(BranchName, fInputEvent->ClassName(), fInputEvent);
	}

}

///////////////////////////////////////////////
/// \brief The main function of this class. Thread will run this function until the end.
///
/// It will start a loop, calling GetNextevtFunc(), ProcessEvent(), and FillThreadEventFunc() repeatedly.
/// If the function GetNextEvent() returns false, the loop will break.
///
/// Before return it will call WriteThreadFileFunc().
///
/// Note: The methods GetNextevtFunc(), FillThreadEventFunc() and WriteThreadFileFunc() are all from 
/// TRestRun. The later two will call back the method FillEvent(), WriteFile() in this class. The idea to do so is 
/// to make these i-o related methods managed by the host run, preventing segmentation violation. All the three
/// method are mutex locked, which makes them running individualy. In other words, these methods
/// will be paused until the host run allows it to run. It avoids the threads writing a same file
/// (or memory) at the same time.
void TRestThread::StartProcess()
{
	isFinished = false;


	while (fHostRunner->GetNextevtFunc(fInputEvent, fAnalysisTree) == 0)
	{
		ProcessEvent();
		fHostRunner->FillThreadEventFunc(this);

	}



	fHostRunner->WriteThreadFileFunc(this);
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

		if (ProcessedEvent == NULL) break;

		fProcessChain[j]->EndOfEventProcess();



	}

	fOutputEvent = ProcessedEvent;

}

/////////////////////////////////////////////////
///// \brief Call the output tree to fill. 
/////
///// This method is called back by FillThreadEventFunc() in TRestRun. 
//void TRestThread::FillEvent()
//{
//	if (fOutputEvent == NULL)return;
//
//	fOutputFile->cd();
//	fAnalysisTree->FillEvent(fOutputEvent);
//
//
//}


///////////////////////////////////////////////
/// \brief Write and close the output file
///
/// This method is called back by WriteThreadFileFunc() in TRestProcessRunner. 
void TRestThread::WriteFile()
{
	if (fOutputFile == NULL) return;

	fOutputFile->cd();


	for (unsigned int i = 0; i < fProcessChain.size(); i++)
		fProcessChain[i]->EndProcess();//the processes must call "object->Write()" in this method


	delete fAnalysisTree;

	fOutputFile->Close();

}