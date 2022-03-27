
#ifndef RestCore_TRestRun
#define RestCore_TRestRun

#define REST_MAXIMUM_EVENTS 2E9

#include <TFile.h>
#include <TFileMerger.h>
#include <TKey.h>

#include "TRestAnalysisTree.h"
#include "TRestEvent.h"
#include "TRestMetadata.h"

class TRestEventProcess;

/// Data provider and manager in REST
class TRestRun : public TRestMetadata {
   protected:
    // runinfo
    Int_t fRunNumber;  //< first identificative number
    Int_t fParentRunNumber;
    TString fRunClassName;
    TString fRunType;  //< Stores bit by bit the type of run. e.g. calibration, background, pedestal,
                       // simulation, datataking
    TString fRunUser;  //< To identify the author it has created the run. It might be also a word describing
                       // the origin of the run (I.e. REST_Prototype, T-REX, etc)
    TString fRunTag;   //< A tag to be written to the output file
    TString fRunDescription;  //< A word or sentence describing the run (I.e. Fe55 calibration, cosmics, etc)
    TString fExperimentName;

    // program data
    TString fInputFileName;
    TString fOutputFileName;
    Double_t fStartTime;  ///< Event absolute starting time/date (unix timestamp)
    Double_t fEndTime;    ///< Event absolute ending time/date (unix timestamp)
    Int_t fEntriesSaved;

    // data-like metadata objects
    std::vector<TRestMetadata*> fMetadata;       //!
    std::vector<TRestMetadata*> fInputMetadata;  //!

    // temp data member
    std::vector<TString> fInputFileNames;  //!
    TFile* fInputFile;                     //!
    TFile* fOutputFile;                    //!
    TRestEvent* fInputEvent;               //!
    TTree* fEventTree;                     //!
    TRestAnalysisTree* fAnalysisTree;      //!
    bool fOverwrite;                       //!
    bool fSaveHistoricData;                //!
    TRestEventProcess* fFileProcess;       //!
    int fCurrentEvent;                     //!
    Long64_t fBytesReaded;                 //!
    Long64_t fTotalBytes;                  //!
    int fEventBranchLoc;                   //!
    int fEventIndexCounter = 0;            //!
    bool fHangUpEndFile = false;           //!
    bool fFromRML = false;                 //!
   private:
    std::string ReplaceMetadataMember(const std::string instr);

   public:
    /// REST run class
    void Initialize();
    void InitFromConfigFile();

    // file operation
    void OpenInputFile(int i);
    void OpenInputFile(const TString& filename, const std::string& mode = "");
    void AddInputFileExternal(std::string file);
    void ReadFileInfo(std::string filename);
    void ReadInputFileMetadata();
    void ReadInputFileTrees();

    void ResetEntry();

    Int_t GetNextEvent(TRestEvent* targetEvent, TRestAnalysisTree* targetTree);
    void GetEntry(int i) {
        if (fAnalysisTree) {
            fAnalysisTree->GetEntry(i);
        }
        if (fEventTree) {
            fEventTree->GetEntry(i);
        }

        if (i >= GetEntries()) {
            warning << "TRestRun::GetEntry. Entry requested out of limits" << endl;
            warning << "Total number of entries is : " << GetEntries() << endl;
        }

        fCurrentEvent = i;
    }

    void GetNextEntry() {
        if (fCurrentEvent + 1 >= GetEntries()) fCurrentEvent = -1;
        GetEntry(fCurrentEvent + 1);
    }

    TString FormFormat(const TString& FilenameFormat);
    TFile* MergeToOutputFile(std::vector<std::string> filefullnames, std::string outputfilename = "");
    TFile* FormOutputFile();
    TFile* UpdateOutputFile();

    void PassOutputFile() {
        fOutputFile = fInputFile;
        fOutputFileName = fOutputFile->GetName();
    }

    void WriteWithDataBase();

    void CloseFile();

    void ImportMetadata(const TString& rootFile, const TString& name, const TString& type, Bool_t store);

    /// add metadata object to the metadata list
    void AddMetadata(TRestMetadata* meta) {
        if (meta) {
            fMetadata.push_back(meta);
        } else {
            warning << "REST Warning! A null matadata wants to be added in TRestRun!" << endl;
        }
    }
    void AddEventBranch(TRestEvent* eve);
    void SkipEventTree() {}

    // Getters
    Int_t GetParentRunNumber() { return fParentRunNumber; }
    Int_t GetRunNumber() { return fRunNumber; }
    TString GetRunType() { return fRunType; }
    TString GetRunUser() { return fRunUser; }
    TString GetRunTag() { return fRunTag; }
    TString GetRunDescription() { return fRunDescription; }
    Double_t GetRunLength();
    Double_t GetStartTimestamp() { return fStartTime; }
    Double_t GetEndTimestamp() { return fEndTime; }
    TString GetExperimentName() { return fExperimentName; }

    std::vector<TString> GetInputFileNames() { return fInputFileNames; }
    std::string GetInputFileName(int i) {
        return i < fInputFileNames.size() ? (std::string)fInputFileNames[i] : "";
    }
    std::string GetInputFileNamepattern() { return (std::string)fInputFileName; }
    TString GetOutputFileName() { return fOutputFileName; }
    TFile* GetInputFile() { return fInputFile; }
    TFile* GetOutputFile() { return fOutputFile; }
    int GetCurrentEntry() { return fCurrentEvent; }
    Long64_t GetBytesReaded() { return fBytesReaded; }
    Long64_t GetTotalBytes();
    int GetEntries();

    TRestEvent* GetInputEvent() { return fInputEvent; }
    TRestEvent* GetEventWithID(Int_t eventID, Int_t subEventID = -1, const TString& tag = "");
    std::vector<int> GetEventEntriesWithConditions(const std::string, int startingIndex = 0,
                                                   int maxNumber = -1);
    std::vector<int> GetEventIdsWithConditions(const std::string, int startingIndex = 0, int maxNumber = -1);
    TRestEvent* GetNextEventWithConditions(const std::string);
    TRestEventProcess* GetFileProcess() { return fFileProcess; }
    std::string GetRunInformation(std::string infoname);
    Int_t GetObservableID(std::string name) { return fAnalysisTree->GetObservableID(name); }
    Bool_t ObservableExists(std::string name) { return fAnalysisTree->ObservableExists(name); }
    TString GetInputEventName() { return fInputEvent->ClassName(); }
    TRestAnalysisTree* GetAnalysisTree() { return fAnalysisTree; }
    TTree* GetEventTree() { return fEventTree; }
    Int_t GetInputFileNumber() { return fFileProcess == nullptr ? fInputFileNames.size() : 1; }

    TRestMetadata* GetMetadata(const TString& name, TFile* f = 0);
    TRestMetadata* GetMetadataClass(TString type, TFile* f = 0);
    std::vector<std::string> GetMetadataStructureNames();
    std::vector<std::string> GetMetadataStructureTitles();
    int GetNumberOfMetadataStructures() { return fMetadata.size(); }

    std::string GetMetadataMember(const std::string instr) { return ReplaceMetadataMember(instr); }
    std::string ReplaceMetadataMembers(std::string instr);

    Bool_t EvaluateMetadataMember(const std::string instr);

    // Setters
    void SetInputFileName(std::string s) {
        fInputFileName = s;
        fInputFileNames = Vector_cast<std::string, TString>(
            TRestTools::GetFilesMatchingPattern((std::string)fInputFileName));
    }
    void SetOutputFileName(std::string s) { fOutputFileName = s; }
    void SetExtProcess(TRestEventProcess* p);
    void SetCurrentEntry(int i) { fCurrentEvent = i; }
    // void AddFileTask(TRestFileTask* t) { fFileTasks.push_back(t); }
    void SetInputEvent(TRestEvent* eve);
    void SetRunNumber(Int_t number) { fRunNumber = number; }
    void SetParentRunNumber(Int_t number) { fParentRunNumber = number; }
    void SetRunType(TString type) {
        std::string cleanType = RemoveWhiteSpaces((std::string)type);
        fRunType = (TString)cleanType;
    }
    void SetRunTag(TString tag) {
        std::string cleanTag = RemoveWhiteSpaces((std::string)tag);
        fRunTag = (TString)cleanTag;
    }
    void SetRunUser(TString user) {
        std::string cleanUser = RemoveWhiteSpaces((std::string)user);
        fRunUser = (TString)cleanUser;
    }
    inline void SetRunDescription(const TString& description) { fRunDescription = description; }
    void SetStartTimeStamp(Double_t tStamp) { fStartTime = tStamp; }
    void SetEndTimeStamp(Double_t tStamp) { fEndTime = tStamp; }
    void SetTotalBytes(Long64_t b) { fTotalBytes = b; }
    void SetHistoricMetadataSaving(bool save) { fSaveHistoricData = save; }
    void HangUpEndFile() { fHangUpEndFile = true; }
    void ReleaseEndFile() { fHangUpEndFile = false; }
    // Printers
    void PrintStartDate();
    void PrintEndDate();

    void PrintMetadata();
    void PrintAllMetadata() {
        PrintMetadata();
        for (unsigned int i = 0; i < fMetadata.size(); i++) fMetadata[i]->PrintMetadata();
    }
    void PrintTrees() {
        if (fEventTree) {
            fout << endl;
            fout << "=====EventTree=====" << endl;
            fEventTree->Print();
        }
        if (fAnalysisTree) {
            fout << endl;
            fout << "=====AnalysisTree=====" << endl;
            fAnalysisTree->Print();
        }
    }
    void PrintObservables() {
        if (fAnalysisTree) fAnalysisTree->PrintObservables();
    }

    void PrintEvent() { fInputEvent->PrintEvent(); }
    void PrintErrors();
    void PrintWarnings();

    Int_t Write(const char* name = nullptr, Int_t option = 0, Int_t bufsize = 0);

    // Constructor & Destructor
    TRestRun();
    TRestRun(const std::string& rootfilename);
    ~TRestRun();

    ClassDef(TRestRun, 5);
};

#endif
