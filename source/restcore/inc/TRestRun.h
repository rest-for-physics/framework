
#ifndef RestCore_TRestRun
#define RestCore_TRestRun


#include "TRestEvent.h"
#include "TRestMetadata.h"
#include "TRestAnalysisTree.h"
#include "TFileMerger.h"
#include "TFile.h"
#include "TKey.h"

//#include "TRestThread.h"

class TRestManager;
class TRestExternalFileProcess;



class TRestRun : public TRestMetadata {
public:
	/// REST run class
	ClassDef(TRestRun, 1);

	//overridden starters
	TRestRun();
	TRestRun(string rootfilename);
	~TRestRun();
	void Initialize();
	void BeginOfInit();
	Int_t ReadConfig(string keydeclare, TiXmlElement* e);
	void EndOfInit();


	//file operation
	void OpenInputFile(int i);
	void OpenInputFile(TString filename,string mode = "");
	void ReadFileInfo(string filename);

	void ResetEntry();

	Int_t GetNextEvent(TRestEvent* targetevt, TRestAnalysisTree* targettree);
	void GetEntry(int i) { if (fInputAnalysisTree != NULL) { fInputAnalysisTree->GetEntry(i); } }

	TString FormFormat(TString FilenameFormat);
	void MergeProcessFile(vector<string> filenames);
	void PassOutputFile() { fOutputFileName = fInputFile->GetName(); }

	void RunTasks();

	void CloseFile();

	void ImportMetadata(TString rootFile, TString name, Bool_t store);

	void SkipEventTree() {}

	


	//Getters
	TString GetVersion() { return  fVersion; }
	Int_t GetParentRunNumber() { return fParentRunNumber; }
	Int_t GetRunNumber() { return fRunNumber; }
	TString GetRunType() { return fRunType; }
	TString GetRunUser() { return fRunUser; }
	TString GetRunTag() { return fRunTag; }
	TString GetRunDescription() { return fRunDescription; }
	Double_t GetStartTimestamp() { return fStartTime; }
	Double_t GetEndTimestamp() { return fEndTime; }
	TString GetExperimentName() { return fExperimentName; }
	TString GetTime(Double_t runTime);
	TString GetDateFormatted(Double_t runTime);

	string Get(string target);

	vector<TString> GetInputFileNames() { return fInputFileNames; }
	string GetInputFileName(int i) { return (string)fInputFileNames[i]; }
	string GetInputFileNamepattern() { return (string)fInputFileName; }
	TString GetOutputFileName() { return fOutputFileName; }
	int GetCurrentEntry() { return fCurrentEvent; }
	int GetEntries() { return GetTotalEntries(); }
	int GetTotalEntries() { if (fInputAnalysisTree != NULL) { return fInputAnalysisTree->GetEntries(); } return 2E9; }
	TRestEvent* GetInputEvent() { return fInputEvent; }
	TRestExternalFileProcess* GetFileProcess() { return fFileProcess; }
	string GetFileInfo(string infoname) { return FileInfo[infoname] == "" ? infoname : FileInfo[infoname]; }
	Int_t GetObservableID(TString name) { return fInputAnalysisTree->GetObservableID(name); }
	Bool_t ObservableExists(TString name) { return fInputAnalysisTree->ObservableExists(name); }
	TString GetInputEventName() { return fInputEvent->ClassName(); }
	TRestAnalysisTree* GetAnalysisTree() { return fInputAnalysisTree; }
	Int_t GetInputFileNumber() { return fFileProcess == NULL ? fInputFileNames.size() : 1; }
	TRestMetadata* GetMetadataInfo(string type);
	TRestMetadata* GetMetadataClass(string type) { return GetMetadataInfo(type); }

	//Setters
	void SetHostmgr(TRestManager*m) { fHostmgr = m; }
	void SetInputFileName(string s) { fInputFileName = s; fInputFileNames = GetFilesMatchingPattern(fInputFileName); }
	void SetExtProcess(TRestExternalFileProcess* p);
	void SetCurrentEntry(int i) { fCurrentEvent = i; }
	//void AddFileTask(TRestFileTask* t) { fFileTasks.push_back(t); }
	void SetInputEvent(TRestEvent* eve) { fInputEvent = eve; }



	// Printers
	void PrintStartDate();
	void PrintEndDate();

	void PrintInfo();
	void PrintMetadata() { PrintInfo(); }
	void PrintAllMetadata() { PrintInfo(); }

	void PrintEvent() { fInputEvent->PrintEvent(); }

	void PrintProcessedEvents(Int_t rateE);



protected:
	Int_t fRunNumber;                 //< first identificative number
	Int_t fParentRunNumber;
	TString fRunClassName;
	TString fRunType;             //< Stores bit by bit the type of run. 0: Calibration 1: Background 2: Pedestal 3: Simulation 4: DataTaking 
	TString fRunUser;	          //< To identify the author it has created the run. It might be also a word describing the origin of the run (I.e. REST_Prototype, T-REX, etc)
	TString fRunTag;            //< A tag to be written to the output file
	TString fRunDescription;	  //< A word or sentence describing the run (I.e. Fe55 calibration, cosmics, etc)
	TString fExperimentName;
	TString fOutputFilename;
	TString fInputFilename;
	TString fVersion;

	Double_t fStartTime;            ///< Event absolute starting time/date (unix timestamp)
	Double_t fEndTime;              ///< Event absolute starting time/date (unix timestamp)
	Int_t fProcessedEvents;

	vector<TRestMetadata*> fMetadataInfo;//->

	TRestManager* fHostmgr;//!

	TString fInputFileName;
	vector<TString> fInputFileNames;
	TFile *fInputFile;//!
	map<string, string> FileInfo;

	TRestEvent* fInputEvent;//!
	TRestAnalysisTree *fInputAnalysisTree;//!

	TRestExternalFileProcess* fFileProcess;//!
	int fCurrentEvent;

	//vector<TRestFileTask*> fFileTasks;//!
	TString fOutputFileName;








};




#endif

