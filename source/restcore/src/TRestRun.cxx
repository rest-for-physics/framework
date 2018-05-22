//////////////////////////////////////////////////////////////////////////
///
/// RESTsoft - Software for Rare Event Searches with TPCs
///
/// \class      TRestRun
/// Data provider and manager in REST
///
/// REST data, including metadata and event data, are all handled by this class
/// A series of metadata objects is saved here after being loaded from rml/root 
/// file with sequential startup. Input file is also opened here, either being
/// raw data file(opened with external process) or root file(opened with built-
/// in reader). TRestRun extracts event data in the input file and wraps it 
/// into TRestEvent class, which is queried by other classes.
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
//////////////////////////////////////////////////////////////////////////



#include "TRestRun.h"
#include "TRestManager.h"
#include "TRestEventProcess.h"



ClassImp(TRestRun);

TRestRun::TRestRun()
{
	Initialize();
}

TRestRun::TRestRun(string rootfilename)
{
	Initialize();
	OpenInputFile(rootfilename);
}

TRestRun::~TRestRun()
{
	if (fInputFile != NULL) {
		fInputFile->Close(); delete fInputFile;
	}
	if (fOutputFile != NULL) {
		fOutputFile->Close(); delete fOutputFile;
	}
	//CloseFile();
}

///////////////////////////////////////////////
/// \brief Set variables by default during initialization.
///
void TRestRun::Initialize()
{
	SetSectionName(this->ClassName());

	fVersion = "2.2";
	time_t  timev; time(&timev);
	fStartTime = (Double_t)timev;
	fEndTime = fStartTime - 1; // So that run length will be -1 if fEndTime is not set

	fRunUser = "";
	fRunNumber = 0;
	fParentRunNumber = 0;
	fRunType = "Null";
	fExperimentName = "Null";
	fRunTag = "Null";


	//fOutputAnalysisTree = NULL;

	fInputFileName = "null";
	fOutputFileName = "rest_default.root";


	fBytesReaded = 0;
	fTotalBytes = -1;

	fInputFileNames.clear();
	fInputFile = NULL;
	fOutputFile = NULL;
	fInputEvent = NULL;
	fAnalysisTree = NULL;
	fEventTree = NULL;
	fCurrentEvent = 0;
	fEventBranchLoc = -1;
	fFileProcess = NULL;

	return;
}

///////////////////////////////////////////////
/// \brief Begin of startup
///
/// Things doing in this method:
/// 1. Load from rml the input file name pattern, and search all the files matching pattern.
/// 2. Load from rml the output file name
///
void TRestRun::BeginOfInit()
{
	//if (fHostmgr == NULL)
	//{
	//	error << "manager not initialized!" << endl;
	//	exit(0);
	//}

	fInputFileName = GetParameter("inputFile","").c_str();
	fInputFileNames = GetFilesMatchingPattern(fInputFileName);
	fOutputFileName = GetDataPath()+GetParameter("outputFile", "rest_default.root").c_str();

}

///////////////////////////////////////////////
/// \brief Respond to the input xml element.
///
/// If the declaration of the input element is:
/// 1. addMetadata: Instantiate the metadata by reading the root file, and add it to the 
/// handled metadata list. Calling the method ImportMetadata().
/// 2. TRestXXX: Instantiate the metadata by using sequential startup, and add it to the 
/// handled metadata list.
/// 3. addProcess: Add an external process, which reads the input file and forms events
///
/// Other types of declarations will be omitted.
///
Int_t TRestRun::ReadConfig(string keydeclare, TiXmlElement* e)
{
	if (keydeclare == "addMetadata") 
	{
		if (e->Attribute("name") != NULL&&e->Attribute("file")!=NULL)
		{
			ImportMetadata(e->Attribute("file"), e->Attribute("name"), true);
			return 0;
		}
		else
		{
			warning << "Wrong definition of addMetadata! Metadata name or file name is not given!" << endl;
			return -1;
		}
	}
	else if (keydeclare == "addProcess") 
	{
		string active = GetParameter("value", e, "");
		if (active != "ON" && active != "On" && active != "on") return 0;
		string processName = GetParameter("name", e, "");
		string processType = GetParameter("type", e, "");
		if (processType == "") { warning << "Bad expression of addProcess" << endl; return 0; }
		else if (processName == "") {
			warning << "Event process " << processType << " has no name, it will be skipped" << endl;
			return -1;
		}
		TClass *cl = TClass::GetClass(processType.c_str());
		if (cl == NULL)
		{
			cout << " " << endl;
			cout << "REST ERROR. Process : " << processType << " not found!!" << endl;
			cout << "Please verify the process type and launch again." << endl;
			cout << "If you are not willing to use this process you can deactivate using value=\"off\"" << endl;
			cout << " " << endl;
			cout << "This process will be skipped." << endl;
			GetChar();
			return -1;
		}
		TRestEventProcess *pc = (TRestEventProcess *)cl->New();

		pc->LoadConfigFromFile(e, fElementGlobal);

		pc->SetRunInfo(this);

		if (pc->InheritsFrom("TRestRawToSignalProcess"))
		{
			SetExtProcess(pc);
			return 0;
		}
		else
		{
			warning << "This is not an external file process!" << endl;
		}

	}
	
	else if (Count(keydeclare, "TRest")>0)
	{
		TClass*c = TClass::GetClass(keydeclare.c_str());
		if (c == NULL) {
			cout << " " << endl;
			cout << "REST ERROR. Class : " << keydeclare << " not found!!" << endl;
			cout << "This class will be skipped." << endl;
			return -1;
		}
		TRestMetadata*meta = (TRestMetadata*)c->New();
		meta->SetHostmgr(NULL);
		meta->LoadConfigFromFile(e, fElementGlobal);
		fMetadataInfo.push_back(meta);

		return 0;
	}

	return -1;
}

///////////////////////////////////////////////
/// \brief End of startup
///
/// Things doing in this method:
/// 1. Get some run information from the main config element(fElement).
/// 2. Open the first input file and find the tree. Then link the input event/analysis
/// to the input tree. This is done within method OpenInputFile().
/// 3. Print some message.
///
void TRestRun::EndOfInit()
{
	//Get some infomation
	fRunNumber = StringToInteger(GetParameter("runNumber", "0"));
	fRunUser = GetParameter("user").c_str();
	fRunType = GetParameter("runType", "NotDefined").c_str();
	fRunDescription = GetParameter("runDescription").c_str();
	fExperimentName = GetParameter("experiment", "preserve").c_str();
	fRunTag = GetParameter("runTag", "preserve").c_str();

	OpenInputFile(0);
	essential << this->ClassName() << " : InputFile : \"" << fInputFileName << "\", " << endl;
	if (fInputFileNames.size() > 1)
	{
		info << "which matches :" << endl;
		for (int i = 0; i < fInputFileNames.size(); i++) {
			info << fInputFileNames[i] << endl;
		}
		info << endl;
	}
	else if (fInputFileNames.size() == 0) {
		info << "(no input file added)";
	}
	

	essential << this->ClassName() << " : OutputFile : \"" << fOutputFileName <<"\""<< endl;
}

///////////////////////////////////////////////
/// \brief Open the i th file in the file list
///
void TRestRun::OpenInputFile(int i)
{
	if (fInputFileNames.size() > i)
	{

		TString Filename = fInputFileNames[i];
		info << "opening... " << Filename << endl;
		OpenInputFile((string)Filename);
	}

}

///////////////////////////////////////////////
/// \brief Open the file. If is root file, link the input event/analysis with input tree
///
void TRestRun::OpenInputFile(TString filename, string mode)
{
	CloseFile();
	ReadFileInfo((string)filename);
	if (isRootFile((string)filename))
	{
		debug << "Initializing input file" << endl;


		fInputFile = new TFile(filename, mode.c_str());

		debug << "Finding TRestAnalysisTree.." << endl;
		TRestAnalysisTree * Tree1 = (TRestAnalysisTree *)fInputFile->Get("AnalysisTree");
		if (Tree1 != NULL)
		{
			fAnalysisTree = Tree1;
			fAnalysisTree->ConnectObservables();
			fAnalysisTree->ConnectEventBranches();
		}
		else
		{
			error << "REST ERROR (OpenInputFile) : AnalysisTree was not found" << endl;
			error << "Inside file : " << filename << endl;
			exit(1);
		}

		TTree* Tree2 = (TTree *)fInputFile->Get("EventTree");
		if (Tree2 != NULL)
		{
			fEventTree = Tree2;
			//fEventTree->ConnectEventBranches();


			debug << "Finding event branch.." << endl;
			if (fInputEvent == NULL) 
			{
				TObjArray* branches = fEventTree->GetListOfBranches();
				//get the last event branch as input event branch
				TBranch *br = (TBranch*)branches->At(branches->GetLast());

				if (Count(br->GetName(), "EventBranch") == 0)
				{
					warning << "REST WARNING (OpenInputFile) : No event branch inside file : " << filename << endl;
					warning << "This file may be a pure analysis file" << endl;
				}
				else
				{
					string type = Replace(br->GetName(), "Branch", "", 0);
					fInputEvent = (TRestEvent*)TClass::GetClass(type.c_str())->New();
					fEventTree->SetBranchAddress(br->GetName(), &fInputEvent);
					fEventBranchLoc = branches->GetLast();
					debug << "found event branch of event type: " << fInputEvent->ClassName() << endl;
				}
			}
			else
			{
				string brname = (string)fInputEvent->ClassName()+"Branch";
				if (fEventTree->GetBranch(brname.c_str()) == NULL) {
					warning << "REST WARNING (OpenInputFile) : No matched event branch inside file : " << filename << endl;
					warning << "Branch required: "<< brname << endl;
				}
				else
				{
					fEventTree->SetBranchAddress(brname.c_str(), &fInputEvent);
					debug << brname << " is found and set!" << endl;
				}
	
			}


		}
		else
		{
			warning << "REST WARNING (OpenInputFile) : EventTree was not found" << endl;
			warning << "This is a pure analysis file!"<< endl;
			fInputEvent = NULL;
		}

	}
	else
	{
		if (fFileProcess == NULL)
			info << "Input file is not root file, a TRestExtFileProcess is needed!" << endl;
		fInputFile = NULL;
		fAnalysisTree = NULL;
	}




}

///////////////////////////////////////////////
/// \brief Extract file info from a file, and save it in the file info list
///
/// Items:
/// 1. matched file name formats
/// 2. Created time and date
/// 3. File size and entries
void TRestRun::ReadFileInfo(string filename)
{
	//format example:
	//run[aaaa]_cobo[bbbb]_frag[cccc]_[time].graw
	//we are going to match it with inputfile:
	//run00042_cobo1_frag0000.graw

	FileInfo.clear();

	string format = GetParameter("inputFormat", "");
	string name = (string)filename;

	//remove extension
	if (format.find(".") != -1) { format = Spilt(format, ".")[0]; }
	if (name.find(".") != -1) { name = Spilt(name, ".")[0]; }

	//remove path name
	if (format.find("/") != -1) { format = Spilt(format, "/")[Spilt(format, "/").size() - 1]; }
	if (name.find("/") != -1) { name = Spilt(name, "/")[Spilt(name, "/").size() - 1]; }

	vector<string> formatsectionlist = Spilt(format, "_");
	vector<string> inputfilesectionlist = Spilt(name, "_");

	debug << "begin matching file names" << endl;
	for (int i = 0; i < formatsectionlist.size(); i++)
	{
		int pos1 = formatsectionlist[i].find("[");
		int pos2 = formatsectionlist[i].find("]");
		if (pos1 == -1 || pos2 == -1) { continue; }

		string prefix = formatsectionlist[i].substr(0, pos1 - 0);
		string FormatName = formatsectionlist[i].substr(pos1 + 1, pos2 - pos1 - 1);
		string FormatValue = "";
		if (prefix == "")
		{
			//if formatsectionlist meets its end, while inputfilesectionlist does not,
			//then the last place of the format is replaced by the remaining of inputfilesectionlist
			if (i == formatsectionlist.size() - 1 && inputfilesectionlist.size() >= formatsectionlist.size())
			{
				for (int j = 0; j <= inputfilesectionlist.size() - formatsectionlist.size(); j++)
				{
					FormatValue += inputfilesectionlist[j + i] + "_";
				}
				FormatValue = FormatValue.substr(0, FormatValue.size() - 1);
			}
			//if not, replace just as the position indicate
			else if (i < formatsectionlist.size() - 1 && inputfilesectionlist.size() >i)
			{
				FormatValue = inputfilesectionlist[i];
			}
			else continue;
		}
		else
		{
			bool find;
			for (int j = 0; j < inputfilesectionlist.size(); j++)
			{
				if (inputfilesectionlist[j].find(prefix) == 0)
				{
					FormatValue = inputfilesectionlist[j].substr(prefix.size(), -1);
					find = true;
					break;
				}
				else if (j == inputfilesectionlist.size() - 1) find = false;
			}
			if (find == false) continue;
		}
		FileInfo[FormatName] = FormatValue;
	}

	debug << "begin collecting file info: " << filename << endl;
	struct stat buf;
	int fd = fileno(fopen(filename.c_str(), "rb"));
	fstat(fd, &buf);

	string datetime = ToDateTimeString(buf.st_mtime);
	FileInfo["Time"] = Spilt(datetime, " ")[1];
	FileInfo["Date"] = Spilt(datetime, " ")[0];
	FileInfo["Size"] = ToString(buf.st_size) + "B";
	FileInfo["Entries"] = ToString(GetEntries());

	if (isRootFile((string)filename))
	{
		fTotalBytes = buf.st_size;
	}
}

///////////////////////////////////////////////
/// \brief Reset file reading progress. 
///
/// if input file is root file, just set current entry to be 0
/// if input file is external file, handled by external process, It will force the process
/// to reload the file.
void TRestRun::ResetEntry()
{
	fCurrentEvent = 0;
	if (fFileProcess != NULL) { fFileProcess->OpenInputFiles(fInputFileNames); fFileProcess->InitProcess(); }
}

///////////////////////////////////////////////
/// \brief Get next event by writting event data into target event and target tree
///
/// returns 0 if success, returns -1 if failed, e.g. end of file
/// writting event data into target event calls the method TRestEvent::CloneTo()
Int_t TRestRun::GetNextEvent(TRestEvent* targetevt, TRestAnalysisTree* targettree)
{
	if (fFileProcess != NULL)
	{
		debug << "TRestRun: getting next event from external process" << endl;
		fFileProcess->BeginOfEventProcess();
		fInputEvent = fFileProcess->ProcessEvent(NULL);
		fFileProcess->EndOfEventProcess();
		fBytesReaded = fFileProcess->GetTotalBytesReaded();
		fCurrentEvent++;
	}
	else
	{
		debug << "TRestRun: getting next event from root file" << endl;
		if (fAnalysisTree != NULL)
		{
			if (fCurrentEvent > fAnalysisTree->GetEntries()) { fInputEvent = NULL; }
			else
			{
				fInputEvent->Initialize();
				fBytesReaded += fAnalysisTree->GetEntry(fCurrentEvent);
				if (targettree != NULL && targettree->isConnected()) {
					for (int n = 0; n < fAnalysisTree->GetNumberOfObservables(); n++)
						targettree->SetObservableValue(n, fAnalysisTree->GetObservableValue(n));
				}
				if (fEventTree != NULL) {
					fBytesReaded += ((TBranch*)fEventTree->GetListOfBranches()->UncheckedAt(fEventBranchLoc))->GetEntry(fCurrentEvent);
					//fBytesReaded += fEventTree->GetEntry(fCurrentEvent);
				}
				fCurrentEvent++;
			}
		}

		else
		{
			warning << "error to get event from input file, missing file process or analysis tree" << endl;
			fInputEvent == NULL;
		}
	}

	if (fInputEvent == NULL)
	{
		if(fFileProcess!=NULL)
			fFileProcess->EndProcess();
		return -1;
	}

	targetevt->Initialize();
	fInputEvent->CloneTo(targetevt);


	return 0;
}

///////////////////////////////////////////////
/// \brief Form output file name according to file info list, proc info list and run data.
///
/// It will replace the fields in output file name surrounded by "[]".
/// The file info list is created by TRestRun::ReadFileInfo(), the 
/// proc info list is created by TRestProcessRunner::ReadProcInfo(),
/// the run data is from TRestRun's datamember(fRunNumber,fRunType, etc)
/// e.g. we can set output file name like:
/// \code Run[fRunNumber]_T[Date]_[Time]_Proc_[LastProcess].root \endcode
/// and generates:
/// \code Run00000_T2018-01-01_08:00:00_Proc_sAna.root \endcode
TString TRestRun::FormFormat(TString FilenameFormat)
{
	string inString = (string)FilenameFormat;
	string outString = (string)FilenameFormat;

	int pos = 0;
	while (1)
	{
		int pos1 = inString.find("[", pos);
		int pos2 = inString.find("]", pos1);
		if (pos1 == -1 || pos2 == -1)break;

		string targetstr = inString.substr(pos1, pos2 - pos1 + 1);//with []
		string target = inString.substr(pos1 + 1, pos2 - pos1 - 1);//without []
		string replacestr = GetFileInfo(target);
		if (replacestr == target && fHostmgr != NULL)replacestr = fHostmgr->GetProcessRunner()==NULL?replacestr: fHostmgr->GetProcessRunner()->GetProcInfo(target);
		if (replacestr == target)replacestr = this->Get(target) == "" ? targetstr : this->Get(target);
		outString = Replace(outString, targetstr, replacestr, 0);
		pos = pos2 + 1;

	}

	return outString;
}

///////////////////////////////////////////////
/// \brief Form REST output file by merging a list of files together
///
/// if target file name is given, then all other files in the file name list will be merged into it.
/// otherwise files will be merged to a new file defined in parameter: outputfile.
/// This method is used to create output file after TRestProcessRunner is finished.
/// The metadata objects will also be written into the file.
TFile* TRestRun::FormOutputFile(vector<string> filenames, string targetfilename)
{
	string filename;
	TFileMerger* m = new TFileMerger();
	if (targetfilename == "") 
	{
		filename = fOutputFileName;
		info << "Creating file : " << filename << endl;
		m->OutputFile(filename.c_str(), "RECREATE");
	}
	else
	{
		filename = targetfilename;
		info << "Creating file : " << filename << endl;
		m->OutputFile(filename.c_str(), "UPDATE");
	}


	for (int i = 0; i < filenames.size(); i++)
	{
		m->AddFile(filenames[i].c_str(), false);
	}

	if (m->Merge())
	{
		for (int i = 0; i < filenames.size(); i++)
		{
			remove(filenames[i].c_str());
		}

	}
	else
	{
		fOutputFileName = "";
		error << "REST ERROR: (Merge files) failed to merge process files." << endl;
		exit(0);
	}

	delete m;

	//we rename the created output file
	fOutputFileName = FormFormat(filename);
	rename(filename.c_str(), fOutputFileName);

	//write metadata into the output file
	fOutputFile = new TFile(fOutputFileName, "UPDATE");
	for (int i = 0; i < fMetadataInfo.size(); i++) {
		fMetadataInfo[i]->Write();
	}

	fout << this->ClassName() << " Created: " << fOutputFileName << endl;
	return fOutputFile;
}

///////////////////////////////////////////////
/// \brief Create a new TFile as REST output file. Writting metadata objects into it.
///
TFile* TRestRun::FormOutputFile() 
{
	CloseFile();
	fOutputFileName = FormFormat(fOutputFileName);
	fOutputFile = new TFile(fOutputFileName, "recreate");
	fAnalysisTree = new TRestAnalysisTree("AnalysisTree","AnalysisTree");
	fEventTree = new TTree("EventTree", "EventTree");
	fAnalysisTree->CreateEventBranches();
	//fEventTree->CreateEventBranches();
	this->Write();
	for (int i = 0; i < fMetadataInfo.size(); i++) {
		fMetadataInfo[i]->Write();
	}
	if (fInputEvent != NULL) {
		//in future we will add lines to create event tree
	}
	return fOutputFile;
}

///////////////////////////////////////////////
/// \brief Close both input file and output file, setting trees to NULL also.
///
void TRestRun::CloseFile()
{
	if (fAnalysisTree != NULL) {
		if (fAnalysisTree->GetEntries() > 0 && fInputFile == NULL)
			fAnalysisTree->Write();
		fAnalysisTree = NULL;
	}

	if (fEventTree != NULL) {
		if (fEventTree->GetEntries() > 0 && fInputFile == NULL)
			fEventTree->Write();
		fEventTree = NULL;
	}

	if (fOutputFile != NULL) {
		fOutputFile->Close(); delete fOutputFile; fOutputFile = NULL;
	}
	if (fInputFile != NULL) {
		fInputFile->Close(); delete fOutputFile; fInputFile = NULL;
	}


}

///////////////////////////////////////////////
/// \brief Set external file process
///
void TRestRun::SetExtProcess(TRestEventProcess* p)
{
	if (fFileProcess == NULL&&p != NULL) {
		fFileProcess = p;

		if (fFileProcess->OpenInputFiles(fInputFileNames) == 0) {
			error << "no files has been loaded by the external process!" << endl;
			exit(0);
		}
		fFileProcess->InitProcess();
		fInputEvent = fFileProcess->GetOutputEvent();
		fInputFile = NULL;
		fAnalysisTree = NULL;
		info << "The external file process has been set! Name : " << fFileProcess->GetName() << endl;
	}
	else
	{
		if (fFileProcess != NULL) {
			error << "There can only be one file process!" << endl; exit(0);
		}
		if (p == NULL) {
			warning << "Given file process is null, skipping..." << endl;
		}
	}

}

///////////////////////////////////////////////
/// \brief Retarget input event in the tree
///
/// The input event is by default the last branch in EventTree, by calling 
/// this method, it can be retargeted to other branches corresponding to the 
/// given event.
void TRestRun::SetInputEvent(TRestEvent* eve)
{
	if (eve != NULL) {

		if (fEventTree != NULL) {
			//if (fEventBranchLoc != -1) {
			//	TBranch *br = (TBranch*)branches->At(fEventBranchLoc);
			//	br->SetAddress(0);
			//}
			if(fInputEvent!=NULL)
				fEventTree->SetBranchAddress((TString)fInputEvent->ClassName()+"Branch", 0);
			TObjArray* branches = fEventTree->GetListOfBranches();
			string brname = (string)eve->ClassName() + "Branch";
			for (int i = 0; i <= branches->GetLast(); i++) {
				TBranch *br = (TBranch*)branches->At(i);
				if ((string)br->GetName() == brname) {
					debug << "Setting input event.. Type: " << eve->ClassName() << " Address: " << eve << endl;
					if (fInputEvent != NULL && (char*)fInputEvent != (char*)eve){
						delete fInputEvent;
					}
					fInputEvent = eve;
					fEventTree->SetBranchAddress(brname.c_str(), &fInputEvent);
					fEventBranchLoc = i;
					break;
				}
				else if(i == branches->GetLast())
				{
					warning << "REST Warning : (TRestRun) cannot find corresponding branch in event tree!" << endl;
					warning << "Event Type : " << eve->ClassName() << endl;
					warning << "Input event not set!" << endl;
				}

			}
		}
		else
		{
			fInputEvent = eve;
		}
	}
}

///////////////////////////////////////////////
/// \brief Add an event branch in output EventTree
///
void TRestRun::AddEventBranch(TRestEvent* eve)
{
	if (eve != NULL) {
		
		if (fEventTree != NULL) {
			string brname = (string)eve->ClassName() + "Branch";
			fEventTree->Branch(brname.c_str(), eve);
		}
	}

}



///////////////////////////////////////////////
/// \brief Open the root file and import the metadata of the given name.
///
/// The metadata class can be recovered to the same condition as when it is saved.
void TRestRun::ImportMetadata(TString File, TString name, Bool_t store)
{
	File = SearchFile(File.Data());
	if (File == "") {
		error << "REST ERROR (ImportMetadata): The file " << File << " does not exist!" << endl;
		error << endl;
		return;
	}
	if (!isRootFile(File.Data()))
	{
		error << "REST ERROR (ImportMetadata) : The file " << File << " is not root file!" << endl;
		return;
	}


	TFile *f = new TFile(File);
	// TODO give error in case we try to obtain a class that is not TRestMetadata
	TRestMetadata *meta = (TRestMetadata *)f->Get(name);

	if (meta == NULL)
	{
		cout << "REST ERROR (ImportMetadata) : " << name << " does not exist." << endl;
		cout << "Inside root file : " << File << endl;
		GetChar();
		f->Close();
		delete f;
		return;
	}

	if (store) meta->Store();
	else meta->DoNotStore();

	meta->InitFromRootFile();
	this->AddMetadata(meta);
	f->Close();
	delete f;
}


Double_t TRestRun::GetRunLength()
{
	if (fEndTime - fStartTime == -1)
		cout << "Run time is not set" << endl;
	return fEndTime - fStartTime;
}



//Getters
TString TRestRun::GetDateFormatted(Double_t runTime)
{
	time_t tt = (time_t)runTime;
	struct tm *tm = localtime(&tt);

	char date[256];
	strftime(date, sizeof(date), "%Y-%B-%d", tm);

	return date;
}

TString TRestRun::GetTime(Double_t runTime)
{
	time_t tt = (time_t)runTime;
	struct tm *tm = localtime(&tt);

	char time[256];
	strftime(time, sizeof(time), "%H:%M:%S", tm);

	return time;
}

///////////////////////////////////////////////
/// \brief Get a string of data member value according to its defined name
///
/// It uses reflection method GetDataMemberWithName() and GetDataMemberValString()
string TRestRun::Get(string target)
{
	auto a = GetDataMemberWithName(target);
	if (a != NULL)
	{
		return GetDataMemberValString(a);
	}

	return "";
}


TRestMetadata* TRestRun::GetMetadataClass(string type)
{
	for (int i = 0; i < fMetadataInfo.size(); i++)
	{
		if (fMetadataInfo[i]->ClassName() == type)
		{
			return fMetadataInfo[i];
		}
	}
	return NULL;
}

TRestMetadata *TRestRun::GetMetadata(TString name)
{
	for (unsigned int i = 0; i < fMetadataInfo.size(); i++)
		if (fMetadataInfo[i]->GetName() == name) return fMetadataInfo[i];

	return NULL;

}


std::vector <std::string> TRestRun::GetMetadataStructureNames()
{
	std::vector <std::string> strings;

	for (int n = 0; n < GetNumberOfMetadataStructures(); n++)
		strings.push_back(fMetadataInfo[n]->GetName());

	return strings;
}


std::vector <std::string> TRestRun::GetMetadataStructureTitles()
{
	std::vector <std::string> strings;

	for (int n = 0; n < GetNumberOfMetadataStructures(); n++)
		strings.push_back(fMetadataInfo[n]->GetTitle());

	return strings;
}




//Printers
void TRestRun::PrintInfo()
{

	cout.precision(10);
	cout << endl;
	cout << "+++++++++++++++++++++++++++++++++++++++++++++" << endl;
	cout << "TRestRun content" << endl;
	cout << "+++++++++++++++++++++++++++++++++++++++++++++" << endl;
	// section name given in the constructor of TRestSpecificMetadata
	cout << "---------------------------------------" << endl;
	cout << "Name : " << GetName() << endl;
	cout << "Title : " << GetTitle() << endl;
	cout << "Version : " << GetVersion() << endl;
	cout << "---------------------------------------" << endl;
	cout << "Parent run number : " << GetParentRunNumber() << endl;
	cout << "Run number : " << GetRunNumber() << endl;
	cout << "Experiment/project : " << GetExperimentName() << endl;
	cout << "Run type : " << GetRunType() << endl;
	cout << "Run tag : " << GetRunTag() << endl;
	cout << "Run user : " << GetRunUser() << endl;
	cout << "Run description : " << GetRunDescription() << endl;
	cout << "Start timestamp : " << GetStartTimestamp() << endl;
	cout << "Date/Time : " << GetDateFormatted(GetStartTimestamp()) << " / " << GetTime(GetStartTimestamp()) << endl;
	cout << "End timestamp : " << GetEndTimestamp() << endl;
	cout << "Date/Time : " << GetDateFormatted(GetEndTimestamp()) << " / " << GetTime(GetEndTimestamp()) << endl;
	cout << "Input file : " << GetInputFileNamepattern() << endl;
	cout << "Output file : " << GetOutputFileName() << endl;
	//cout << "Input filename : " << fInputFilename << endl;
	//cout << "Output filename : " << fOutputFilename << endl;
	//cout << "Number of initial events : " << GetNumberOfEvents() << endl;
	//cout << "Number of processed events : " << fProcessedEvents << endl;
	cout << "+++++++++++++++++++++++++++++++++++++++++++++" << endl;

}


void TRestRun::PrintStartDate()
{
	cout.precision(10);

	cout << "------------------------" << endl;
	cout << "---- Run start date ----" << endl;
	cout << "------------------------" << endl;
	cout << "Unix time : " << fStartTime << endl;
	time_t tt = (time_t)fStartTime;
	struct tm *tm = localtime(&tt);

	char date[20];
	strftime(date, sizeof(date), "%Y-%m-%d", tm);
	cout << "Date : " << date << endl;

	char time[20];
	strftime(time, sizeof(time), "%H:%M:%S", tm);
	cout << "Time : " << time << endl;
	cout << "++++++++++++++++++++++++" << endl;
}

void TRestRun::PrintEndDate()
{
	cout << "----------------------" << endl;
	cout << "---- Run end date ----" << endl;
	cout << "----------------------" << endl;
	cout << "Unix time : " << fEndTime << endl;
	time_t tt = (time_t)fEndTime;
	struct tm *tm = localtime(&tt);

	char date[20];
	strftime(date, sizeof(date), "%Y-%m-%d", tm);
	cout << "Date : " << date << endl;

	char time[20];
	strftime(time, sizeof(time), "%H:%M:%S", tm);
	cout << "Time : " << time << endl;
	cout << "++++++++++++++++++++++++" << endl;
}


