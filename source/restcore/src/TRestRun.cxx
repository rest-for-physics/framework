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
	//if (fOutputFile != NULL) CloseOutputFile();
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
	fOutputFileName = "default";


	fProcessedEvents = 0;

	fInputFileNames.clear();
	fInputFile = NULL;
	fOutputFile = NULL;
	fInputEvent = NULL;
	fAnalysisTree = NULL;
	fEventTree = NULL;
	fCurrentEvent = 0;
	fFileProcess = NULL;
	fOutputFileName = "";

	return;
}


void TRestRun::BeginOfInit()
{
	if (fHostmgr == NULL)
	{
		error << "manager not initialized!" << endl;
		exit(0);
	}


}


///////////////////////////////////////////////
/// \brief Respond to the input xml element.
///
/// If the declaration of the input element is:
/// 1. addProcess: Add the name and type of this element into the process list, ready 
/// for the method InitThreads() to use. If the field value is not "on", the process will be omitted.
/// 2. TRest***Process: Add this element to config list, ready for the method InitThreads() to use.
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
/// 2. Initialize the processes and threads. Calling the method InitThreads(). TRestThread 
/// objects are not following sequencial startup procedure, as they are not thoroughly resident classes.
/// 3. Initialize input event. If the first process is external, then the input event is set to 
/// the process's output event. If not, the input event is instantiated by calling the method
/// TClass::GetClass()->New(). The address of input event is fixed since now. 
/// 4. Open the input file and find the tree. Then link the input event to the input tree. This is done
/// with in method OpenInputFile().
/// 5. Print some message.
///
void TRestRun::EndOfInit()
{
	//Get some infomation
	fRunUser = GetParameter("user").c_str();
	fRunType = GetParameter("runType", "NotDefined").c_str();
	fRunDescription = GetParameter("runDescription").c_str();
	fInputFileName = GetParameter("inputFile").c_str();
	fInputFileNames = GetFilesMatchingPattern(fInputFileName);
	fOutputFileName = GetParameter("outputFile", "rest_default.root").c_str();
	fExperimentName = GetParameter("experiment", "preserve").c_str();
	fRunTag = GetParameter("runTag", "preserve").c_str();

	OpenInputFile(0);
	info << this->ClassName() << " : InputFile : " << fInputFileName << endl;
	if (fInputFileNames.size() > 1)
	{
		info << "which matches :" << endl;
		for (int i = 0; i < fInputFileNames.size(); i++) {
			info << fInputFileNames[i] << endl;
		}
		cout << endl;
	}
	else if (fInputFileNames.size() == 0) {
		warning << "REST WARNING(TRestRun): Input File does not match anything!" << endl;
		GetChar();
	}

	info << this->ClassName() << " : OutputFile : " << fOutputFileName << endl;
}




void TRestRun::OpenInputFile(int i)
{
	if (fInputFileNames.size() > i)
	{

		TString Filename = fInputFileNames[i];
		info << "opening... " << Filename << endl;
		OpenInputFile((string)Filename);
	}

}


void TRestRun::OpenInputFile(TString filename, string mode)
{
	CloseFile();
	ReadFileInfo((string)filename);
	if (isRootFile((string)filename))
	{
		debug << "Initializing input file" << endl;


		fInputFile = new TFile(filename, mode.c_str());

		debug << "Finding TRestAnalysisTree.." << endl;
		TRestAnalysisTree * Tree = (TRestAnalysisTree *)fInputFile->Get("AnalysisTree");
		if (Tree != NULL)
		{
			fAnalysisTree = Tree;
			fAnalysisTree->ConnectObservables();
			fAnalysisTree->ConnectEventBranches();
		}
		else
		{
			error << "REST ERROR (OpenInputFile) : AnalysisTree was not found" << endl;
			error << "Inside file : " << filename << endl;
			exit(1);
		}

		Tree = (TRestAnalysisTree *)fInputFile->Get("EventTree");
		if (Tree != NULL)
		{
			fEventTree = Tree;
			fEventTree->ConnectEventBranches();

			debug << "Finding event branch.." << endl;
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
				string type = Replace(br->GetName(), "Branch", "",0);
				fInputEvent = (TRestEvent*)TClass::GetClass(type.c_str())->New();
				br->SetAddress(&fInputEvent);
				debug << "found event branch of event type: " << fInputEvent->ClassName() << endl;
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
	if (formatsectionlist.size() == 0)return;
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
	FileInfo["Entries"] = ToString(GetTotalEntries());

}



void TRestRun::ResetEntry()
{
	fCurrentEvent = 0;
	if (fFileProcess != NULL) { fFileProcess->OpenInputFiles(fInputFileNames); fFileProcess->InitProcess(); }
}


Int_t TRestRun::GetNextEvent(TRestEvent* targetevt, TRestAnalysisTree* targettree)
{
	if (fFileProcess != NULL)
	{
		debug << "TRestRun: getting next event from external process" << endl;
		fInputEvent = fFileProcess->ProcessEvent(NULL);
		fCurrentEvent++;
	}
	else
	{
		debug << "TRestRun: getting next event from root file" << endl;
		if (fAnalysisTree != NULL)
		{
			if (fAnalysisTree->GetEntries() <= fCurrentEvent - 1)fInputEvent = NULL;

			fAnalysisTree->GetEntry(fCurrentEvent);
			if (targettree != NULL) {
				for (int n = 0; n < fAnalysisTree->GetNumberOfObservables(); n++)
					targettree->SetObservableValue(n, fAnalysisTree->GetObservableValue(n));
			}
			if (fEventTree != NULL) {
				fEventTree->GetEntry(fCurrentEvent);
			}
			fCurrentEvent++;
		}

		else
		{
			warning << "error to get event from input file, missing file process or analysis tree" << endl;
			fInputEvent == NULL;
		}
	}

	if (fInputEvent == NULL)
	{
		return -1;
	}

	fInputEvent->CloneTo(targetevt);


	return 0;
}




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
		if (replacestr == target)replacestr = fHostmgr->GetProcessRunner()==NULL?replacestr: fHostmgr->GetProcessRunner()->GetProcInfo(target);
		if (replacestr == target)replacestr = this->Get(target) == "" ? targetstr : this->Get(target);
		outString = Replace(outString, targetstr, replacestr, 0);
		pos = pos2 + 1;

	}

	return outString;
}

//Merge a list of string of file names together. 
//if target file name is given, then all other files
//in the file name list will be merged into it.
//otherwise files will be merged to a new file defined in parameter: outputfile
void TRestRun::MergeProcessFile(vector<string> filenames, string targetfilename)
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

}


TFile* TRestRun::FormOutputFile() 
{
	CloseFile();
	fOutputFileName = FormFormat(fOutputFileName);
	fOutputFile = new TFile(fOutputFileName, "recreate");
	this->Write();
	for (int i = 0; i < fMetadataInfo.size(); i++) {
		fMetadataInfo[i]->Write();
	}
	if (fInputEvent != NULL) {
		//in future we will add lines to create event tree
	}
	return fOutputFile;
}


void TRestRun::CloseFile()
{
	if (fOutputFile != NULL) {
		fOutputFile->Close(); delete fOutputFile; fOutputFile = NULL;
	}
	if (fInputFile != NULL) {
		fInputFile->Close(); delete fInputFile; fInputFile = NULL;
	}
	if (fAnalysisTree != NULL) {
		delete fAnalysisTree; fAnalysisTree = NULL;
	}
	if (fEventTree != NULL) {
		delete fEventTree; fEventTree = NULL;
	}
}



void TRestRun::SetExtProcess(TRestEventProcess* p)
{
	if (fFileProcess == NULL&&p != NULL) {
		fFileProcess = p;

		if (fFileProcess->OpenInputFiles(fInputFileNames) == 0) {
			error << "no files has been loaded by the external process!" << endl;
			exit(0);
		}

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
/// \brief Open the root file and import the metadata of this class.
///
/// The class should be recovered to the same condition of the saved one.
///
void TRestRun::ImportMetadata(TString File, TString name, Bool_t store)
{
	if (!fileExists(File.Data()))
	{
		error << "REST ERROR (ImportMetadata) : The file " << File << " does not exist" << endl;
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
	//cout << "Input filename : " << fInputFilename << endl;
	//cout << "Output filename : " << fOutputFilename << endl;
	//cout << "Number of initial events : " << GetNumberOfEvents() << endl;
	//cout << "Number of processed events : " << fProcessedEvents << endl;
	cout << "+++++++++++++++++++++++++++++++++++++++++++++" << endl;

	if(fAnalysisTree!=NULL)
		fAnalysisTree->Show(0);

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


