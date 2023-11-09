//////////////////////////////////////////////////////////////////////////
///
/// REST data, including metadata and event data, are all handled by this class
/// A series of metadata objects is saved here after being loaded from rml/root
/// file with sequential startup. Input file is also opened here, either being
/// raw data file(opened with external process) or root file(opened with built-
/// in reader). TRestRun extracts event data in the input file and wraps it
/// into TRestEvent class, which is queried by other classes.
//
/// \class TRestRun
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

#include "TRestRun.h"
#ifdef WIN32
#include <io.h>
#include <process.h>
#include <windows.h>
#undef GetClassName
#else
#include <sys/stat.h>
#include <unistd.h>
#endif  // !WIN32

#include <filesystem>

#include "TRestDataBase.h"
#include "TRestEventProcess.h"
#include "TRestManager.h"
#include "TRestVersion.h"

using namespace std;

std::mutex mutex_read;

ClassImp(TRestRun);

TRestRun::TRestRun() { Initialize(); }

TRestRun::TRestRun(const string& filename) {
    if (filename.find(".root") != string::npos) {
        Initialize();
        OpenInputFile(filename);
    } else if (filename.find(".rml") != string::npos) {
        Initialize();
        LoadConfigFromFile(filename);
    } else {
        RESTError << "TRestRun::TRestRun(): input file error!" << RESTendl;
    }
}

TRestRun::~TRestRun() { CloseFile(); }

///////////////////////////////////////////////
/// \brief Set variables by default during initialization.
///
void TRestRun::Initialize() {
    SetSectionName(this->ClassName());

    time_t timev;
    time(&timev);
    fStartTime = (Double_t)timev;
    fEndTime = 0;  // We need a static value to avoid overwriting the time if another process sets the time

    fRunUser = REST_USER;
    fRunNumber = 0;        // run number where input file is from and where output file
                           // will be saved
    fParentRunNumber = 0;  // subrun number where input file is from
    fRunType = "Null";
    fExperimentName = "Null";
    fRunTag = "Null";
    fRunDescription = "Null";

    fInputFileName = "null";
    fOutputFileName = "rest_default.root";

    fBytesRead = 0;
    fTotalBytes = -1;
    fOverwrite = true;
    fEntriesSaved = -1;
    fNFilesSplit = 0;

    fInputMetadata.clear();
    fMetadata.clear();
    fInputFileNames.clear();
    fInputFile = nullptr;
    fOutputFile = nullptr;
    fInputEvent = nullptr;
    fAnalysisTree = nullptr;
    fEventTree = nullptr;
    fCurrentEvent = 0;
    fEventBranchLoc = -1;
    fFileProcess = nullptr;
    fSaveHistoricData = true;
}

///////////////////////////////////////////////
/// \brief Initialize logic of TRestRun
///
/// Things doing in this method:
/// 1. Read basic parameter. This is done by calling ReadAllParameters()
/// 2. Initialize runnumber and input file name. They follow non-trivial logic.
/// 3. Construct default output file name with runNumber, runTag, etc.
/// 4. Loop over sections to initialize metadata
/// 5. Open input file(s), read the stored metadata and trees, read file name
/// pattern to TRestDetector metadata.
///
void TRestRun::InitFromConfigFile() {
    RESTDebug << "Initializing TRestRun from config file, version: " << REST_RELEASE << RESTendl;
    fFromRML = true;
    ReSetVersion();

    // 1. Read basic parameter
    fRunUser = REST_USER;
    // fRunType = GetParameter("runType", "ANALYSIS").c_str();
    // fRunDescription = GetParameter("runDescription", "").c_str();
    // fExperimentName = GetParameter("experiment", "preserve").c_str();
    // fRunTag = GetParameter("runTag", "noTag").c_str();
    ReadAllParameters();

    // 2. Initialize runnumber and input file name. They follow non-trivial logic
    fRunNumber = -1;
    fParentRunNumber = 0;
    string runNstr = GetParameter("runNumber", "-1");
    string inputName = GetParameter("inputFileName", "");
    inputName = TRestTools::RemoveMultipleSlash(inputName);
    string inputNameOld = GetParameter("inputFile", "default");
    if (inputNameOld != "default") {
        RESTWarning << "Parameter \"inputFile\" in rml is obsolete! Please update it to \"inputFileName\""
                    << RESTendl;
        if (inputName.empty()) {
            inputName = inputNameOld;
        }
    }
    if (ToUpper(runNstr) == "AUTO" && ToUpper(inputName) == "AUTO") {
        RESTError << "TRestRun: run number and input file name cannot both be "
                     "\"AUTO\""
                  << RESTendl;
        exit(1);
    }

    if (ToUpper(inputName) != "AUTO") {
        fInputFileName = inputName;
        fInputFileNames = Vector_cast<string, TString>(TRestTools::GetFilesMatchingPattern(inputName));
    }

    if (ToUpper(runNstr) != "AUTO") {
        fRunNumber = atoi(runNstr.c_str());
    }

    if (ToUpper(inputName) == "AUTO") {
        TRestDataBase* db = gDataBase;
        auto files = db->query_run_files(fRunNumber);
        fInputFileName = db->query_run(fRunNumber).value;  // run entry value is file pattern
        fInputFileNames = Vector_cast<DBFile, TString>(files);
    }

    if (ToUpper(runNstr) == "AUTO") {
        TRestDataBase* db = gDataBase;
        auto runs = db->search_run_with_file((string)fInputFileName);
        if (!runs.empty()) {
            fRunNumber = runs[0];
        } else {
            fRunNumber = -1;
        }

        // add a new run
        if (fRunNumber == 0) {
            fRunNumber = db->get_lastrun() + 1;
            DBEntry entry;
            entry.runNr = fRunNumber;
            entry.description = fRunDescription;
            entry.tag = fRunTag;
            entry.type = fRunType;
            entry.version = REST_RELEASE;
            db->set_run(entry);
        }
    }

    // get some run information
    if (fRunNumber != -1) {
        DBEntry entry = gDataBase->query_run(fRunNumber);
        if (!entry.IsZombie()) {
            fStartTime = entry.tstart;
            fEndTime = entry.tend;
            fRunDescription = entry.description;
            fRunTag = entry.tag;
            fRunType = entry.type;
        }
    }

    if (fInputFileNames.empty()) {
        if (fInputFileName != "") {
            RESTError << "cannot find the input file!" << RESTendl;
            exit(1);
        } else {
            RESTWarning << "no input file added" << RESTendl;
        }
        // throw;
    }

    // 3. Construct output file name
    string outputDir = (string)GetDataPath();
    string outputName = GetParameter("outputFileName", "default");
    string outputNameOld = GetParameter("outputFile", "default");
    if (outputNameOld != "default") {
        RESTWarning << "Parameter \"outputFile\" in rml is obsolete! Please update it to \"outputFileName\""
                    << RESTendl;
        if (outputName == "default") {
            outputName = outputNameOld;
        }
    }
    if (ToUpper(outputName) == "DEFAULT") {
        string expName = RemoveWhiteSpaces((string)GetExperimentName());
        string runType = RemoveWhiteSpaces((string)GetRunType());
        char runParentStr[256];
        sprintf(runParentStr, "%05d", fParentRunNumber);
        char runNumberStr[256];
        sprintf(runNumberStr, "%05d", fRunNumber);

        fOutputFileName = outputDir + "Run_" + expName + "_" + fRunUser + "_" + runType + "_" + fRunTag +
                          "_" + (TString)runNumberStr + "_" + (TString)runParentStr + "_V" + REST_RELEASE +
                          ".root";

        // fOverwrite = ToUpper(GetParameter("overwrite", "on")) != "OFF";
        while (!fOverwrite && TRestTools::fileExists((string)fOutputFileName)) {
            fParentRunNumber++;
            sprintf(runParentStr, "%05d", fParentRunNumber);
            fOutputFileName = outputDir + "Run_" + expName + "_" + fRunUser + "_" + runType + "_" + fRunTag +
                              "_" + (TString)runNumberStr + "_" + (TString)runParentStr + "_V" +
                              REST_RELEASE + ".root";
        }
    } else if (ToUpper(outputName) == "NULL" || outputName == "/dev/null") {
        fOutputFileName = "/dev/null";
    } else if (TRestTools::isAbsolutePath(outputName)) {
        fOutputFileName = outputName;
        outputDir = TRestTools::SeparatePathAndName((string)fOutputFileName).first;
    } else {
        fOutputFileName = outputDir + "/" + outputName;
    }
    // remove multiple slashes from fOutputFileName
    fOutputFileName = (TString)TRestTools::RemoveMultipleSlash((string)fOutputFileName);

    if (!TRestTools::fileExists(outputDir)) {
        int z = system((TString) "mkdir -p " + outputDir);
        if (z != 0) RESTError << "Problem creating directory : " << outputDir << RESTendl;
    }
    if (!TRestTools::isPathWritable(outputDir)) {
        RESTWarning << "TRestRun: Output path '" << outputDir << "' does not exist or it is not writable."
                    << RESTendl;
    }

    // 4. Loop over sections to initialize metadata
    TiXmlElement* e = fElement->FirstChildElement();
    while (e != nullptr) {
        string key = e->Value();
        if (key == "addMetadata") {
            if (e->Attribute("file") != nullptr) {
                ImportMetadata(e->Attribute("file"), e->Attribute("name"), e->Attribute("type"), true);
            } else {
                RESTWarning << "Wrong definition of addMetadata! Metadata name or file name "
                               "is not given!"
                            << RESTendl;
            }
        } else if (Count(key, "TRest") > 0) {
            if (e->Attribute("file") != nullptr && TRestTools::isRootFile(e->Attribute("file"))) {
                RESTWarning << "TRestRun: A root file is being included in section <" << key
                            << " ! To import metadata from this file, use <addMetadata" << RESTendl;
                RESTWarning << "Skipping..." << RESTendl;
            }

            TRestMetadata* meta = REST_Reflection::Assembly(key);
            if (meta == nullptr) {
                RESTWarning << "failed to add metadata \"" << key << "\"" << RESTendl;
                e = e->NextSiblingElement();
                continue;
            }
            meta->SetConfigFile(fConfigFileName);
            meta->SetHostmgr(fHostmgr);
            fMetadata.push_back(meta);
            meta->LoadConfigFromElement(e, fElementGlobal);
        }
        e = e->NextSiblingElement();
    }

    // 5. Open input file(s). We open input file at the last stage in case the file name pattern
    // reading requires TRestDetector
    OpenInputFile(0);
    RESTDebug << "TRestRun::EndOfInit. InputFile pattern: \"" << fInputFileName << "\"" << RESTendl;
    RESTInfo << "which matches :" << RESTendl;
    for (const auto& inputFileName : fInputFileNames) {
        RESTInfo << inputFileName << RESTendl;
    }
    RESTEssential << "(" << fInputFileNames.size() << " added files)" << RESTendl;
}

///////////////////////////////////////////////
/// \brief Open the i th file in the file list
///
void TRestRun::OpenInputFile(int i) {
    if (fInputFileNames.size() > (unsigned int)i) {
        TString Filename = fInputFileNames[i];
        RESTInfo << "opening... " << Filename << RESTendl;
        OpenInputFile((string)Filename);
    }
}

///////////////////////////////////////////////
/// \brief Open the input file, read file info and the trees if it is root file.
///
/// TRestRun will:
/// 1. read the file info (size, date, etc.)
/// if it's root file from REST output, then
/// 2. update its class's data(version, tag, user, etc.) to the same as the one stored in the input file.
/// 3. link the input event and observables to the corresponding tree
///
void TRestRun::OpenInputFile(const TString& filename, const string& mode) {
    CloseFile();
    if (!filename.Contains("http") && !TRestTools::fileExists((string)filename)) {
        RESTError << "input file \"" << filename << "\" does not exist!" << RESTendl;
        exit(1);
    }

    if (!filename.Contains("http")) ReadFileInfo((string)filename);

    // add to fInputFileNames in case it is opening a new file
    bool inList = false;
    for (const auto& inputFileName : fInputFileNames) {
        if (inputFileName == filename) {
            inList = true;
            break;
        }
    }

    if (!inList) {
        fInputFileNames.push_back(filename);
    }

    if (TRestTools::isRootFile((string)filename)) {
        fInputFile = TFile::Open(filename, mode.c_str());

        if (GetMetadataClass("TRestRun", fInputFile)) {
            // This should be the values in RML (if it was initialized using RML)
            TString runTypeTmp = fRunType;
            //           TString runUserTmp = fRunUser;
            TString runTagTmp = fRunTag;
            TString runDescriptionTmp = fRunDescription;
            TString experimentNameTmp = fExperimentName;
            TString outputFileNameTmp = fOutputFileName;
            TString inputFileNameTmp = fInputFileName;
            TString cFileNameTmp = fConfigFileName;

            // We define fVersion to -1 to identify old REST files that did not have yet
            // versioning system
            this->UnSetVersion();

            // Now we load the values in the previous run file
            // If successfully read the input file, the version code will be changed
            // from -1 --> certain number
            this->Read(GetMetadataClass("TRestRun", fInputFile)->GetName());

            if (inputFileNameTmp != "null") fInputFileName = inputFileNameTmp;
            if (outputFileNameTmp != "rest_default.root") fOutputFileName = outputFileNameTmp;
            if (cFileNameTmp != "null") fConfigFileName = cFileNameTmp;

            // If the value was initialized from RML and is not preserve, we recover
            // back the value in RML
            if (fFromRML) {
                if (runTypeTmp != "Null" && runTypeTmp != "preserve") fRunType = runTypeTmp;

                // We should not recover the user. Only when writing. If not when I open a file
                // with restRoot just to read, and Print the run content from other user in my
                // own account, it will say it was me!
                // if (runUserTmp != "Null" && runTypeTmp != "preserve") fRunUser = runUserTmp;

                if (runTagTmp != "Null" && runTagTmp != "preserve") fRunTag = runTagTmp;
                if (runDescriptionTmp != "Null" && runDescriptionTmp != "preserve")
                    fRunDescription = runDescriptionTmp;
                if (experimentNameTmp != "Null" && experimentNameTmp != "preserve")
                    fExperimentName = experimentNameTmp;
            }

            // If version is lower than 2.2.1 we do not read/transfer the metadata to
            // output file?
            // Actually g4 files from v2.1.x is all compatible with the v2.2.x version
            // Only 2.2.0 is without auto schema evolution, whose metadata cannot be read
            if (fSaveHistoricData) {
                if (this->GetVersionCode() >= REST_VERSION(2, 2, 1) ||
                    this->GetVersionCode() <= REST_VERSION(2, 1, 8)) {
                    ReadInputFileMetadata();
                } else {
                    RESTWarning << "-- W : The metadata version found on input file is lower "
                                   "than 2.2.1!"
                                << RESTendl;
                    RESTWarning << "-- W : metadata from input file will not be read" << RESTendl;
                }
            }

            RESTDebug << "Initializing input file : version code : " << this->GetVersionCode() << RESTendl;
            RESTDebug << "Input file version : " << this->GetVersion() << RESTendl;
            ReadInputFileTrees();
            fCurrentEvent = 0;
        } else {
            RESTWarning << "TRestRun object not found in file! The input file may be incomplete!" << RESTendl;
            ReadInputFileTrees();
        }
    } else {
        fInputFile = nullptr;
        fAnalysisTree = nullptr;
        if (fFileProcess != nullptr) {
            fFileProcess->OpenInputFiles(Vector_cast<TString, string>(fInputFileNames));
            fFileProcess->InitProcess();
        }
    }

    if (fAnalysisTree == nullptr && fFileProcess == nullptr)
        RESTInfo << "Input file is not REST root file, an external process is needed!" << RESTendl;
}

void TRestRun::AddInputFileExternal(const string& file) {
    mutex_read.lock();
    if (fFileProcess != nullptr) {
        bool add = fFileProcess->AddInputFile(file);
        if (!add) {
            RESTError << "failed to add input file!" << RESTendl;
        }
        fInputFileNames.emplace_back(file);
    }
    mutex_read.unlock();
}

void TRestRun::ReadInputFileMetadata() {
    TFile* f = fInputFile;
    if (f != nullptr) {
        fInputMetadata.clear();

        TIter nextkey(f->GetListOfKeys());
        TKey* key;
        // we should make sure the input metadata has unique names
        set<string> addedNames;
        while ((key = (TKey*)nextkey())) {
            RESTDebug << "Reading key with name : " << key->GetName() << RESTendl;
            RESTDebug << "Key type (class) : " << key->GetClassName() << RESTendl;

            if (!TClass::GetClass(key->GetClassName()) ||
                !TClass::GetClass(key->GetClassName())->IsLoaded()) {
                RESTError << "-- Class " << key->GetClassName() << " has no dictionary!" << RESTendl;
                RESTError << "- Any relevant REST library missing? " << RESTendl;
                RESTError << "- File reading will continue without loading key: " << key->GetName()
                          << RESTendl;
                continue;
            }

            if (addedNames.count(key->GetName()) != 0) {
                continue;
            }

            TRestMetadata* a = (TRestMetadata*)f->Get(key->GetName());
            RESTDebug << "Key of type : " << a->ClassName() << "(" << a << ")" << RESTendl;

            if (!a) {
                RESTError << "TRestRun::ReadInputFileMetadata." << RESTendl;
                RESTError << "Key name : " << key->GetName() << RESTendl;
                RESTError << "Hidden key? Please, report this problem." << RESTendl;
            } else if (a->InheritsFrom("TRestMetadata") && a->ClassName() != (TString) "TRestRun") {
                /*
                //we make sure there is no repeated class added
                // However, we might have two processes with the same class name
                operating at different steps of the data chain
                // We just avoid to write TRestRun from previous file to the list of
                metadata structures

                bool flag = false;
                for (int i = 0; i < fInputMetadata.size(); i++) {
                if (a->ClassName() == fInputMetadata[i]->ClassName()) {
                flag = true;
                break;
                }
                }
                if (!flag) {
                fInputMetadata.push_back(a);
                }
                 */
                a->LoadConfigFromBuffer();
                fInputMetadata.push_back(a);
                fMetadata.push_back(a);
                addedNames.insert(key->GetName());
            }
        }
    }
}

void TRestRun::ReadInputFileTrees() {
    if (fInputFile != nullptr) {
        RESTDebug << "Finding TRestAnalysisTree.." << RESTendl;
        TTree* _eventTree = nullptr;
        string filename = fInputFile->GetName();

        if (fInputFile->Get("AnalysisTree") != nullptr) {
            fAnalysisTree = (TRestAnalysisTree*)fInputFile->Get("AnalysisTree");

            if (fNFilesSplit > 0) {  // fNFilesSplit=1: split to 1 additional file
                RESTEssential << "Linking analysis tree from split data files" << RESTendl;
                fAnalysisTree = (TRestAnalysisTree*)fAnalysisTree->Clone();  // we must make a copy to have
                                                                             // TBrowser correctly browsed.
                for (int i = 1; i <= fNFilesSplit; i++) {
                    string filename = fInputFile->GetName() + (string) "." + ToString(i);
                    RESTInfo << filename << " --> ";
                    RESTInfo << (fAnalysisTree->AddChainFile(filename) ? "success" : "failed") << RESTendl;
                }
                if (fAnalysisTree->GetChain() == nullptr ||
                    fAnalysisTree->GetChain()->GetNtrees() != fNFilesSplit + 1) {
                    RESTError << "Error adding split files, files missing?" << RESTendl;
                    RESTError << "Your data could be incomplete!" << RESTendl;
                }
            }

            // Note: we call GetEntries() to initialize total entry number
            // Otherwise the child analysis tree's observables will be reset
            // on the next call of GetEntries()
            fAnalysisTree->GetEntries();
            // Call GetEntry() to initialize observables and connect branches
            fAnalysisTree->GetEntry(0);

            _eventTree = (TTree*)fInputFile->Get("EventTree");
        } else if (fInputFile->FindKey("TRestAnalysisTree") != nullptr) {
            // This is v2.1.6- version of input file, we directly find EventTree and
            // AnalysisTree. The old name pattern is "TRestXXXEventTree-eventBranch"
            // and "TRestAnalysisTree"
            RESTWarning << "Loading root file from old version REST!" << RESTendl;
            fAnalysisTree = (TRestAnalysisTree*)fInputFile->Get("TRestAnalysisTree");
            fAnalysisTree->GetEntry(0);

            TIter nextkey(fInputFile->GetListOfKeys());
            TKey* key;
            while ((key = (TKey*)nextkey())) {
                // cout << key->GetName() << endl;
                if (((string)key->GetName()).find("EventTree") != string::npos) {
                    _eventTree = (TTree*)fInputFile->Get(key->GetName());
                    string eventName = Replace(key->GetName(), "Tree", "", 0);
                    TBranch* br = _eventTree->GetBranch("eventBranch");
                    br->SetName((eventName + "Branch").c_str());
                    br->SetTitle((eventName + "Branch").c_str());
                    break;
                }
            }
            RESTDebug << "Old REST file successfully recovered!" << RESTendl;
        } else {
            RESTWarning << "(OpenInputFile) : AnalysisTree was not found" << RESTendl;
            RESTWarning << "Inside file : " << filename << RESTendl;
            RESTWarning << "This may not be a REST output file!" << RESTendl;
        }

        if (_eventTree != nullptr) {
            if (fNFilesSplit > 0) {
                // eventTree shall be initialized as TChain
                delete _eventTree;
                RESTEssential << "Linking event tree from split data files" << RESTendl;
                TChain* _fEventTree = new TChain("EventTree");
                RESTInfo << fInputFile->GetName() << " --> ";
                RESTInfo << (_fEventTree->Add(fInputFile->GetName()) ? "success" : "failed") << RESTendl;

                for (int i = 1; i <= fNFilesSplit; i++) {
                    string filename = fInputFile->GetName() + (string) "." + ToString(i);
                    RESTInfo << filename << " --> ";
                    RESTInfo << (_fEventTree->Add(filename.c_str()) ? "success" : "failed") << RESTendl;
                }
                fEventTree = _fEventTree;
            } else {
                fEventTree = _eventTree;
            }

            RESTDebug << "Finding event branch.." << RESTendl;
            if (fInputEvent == nullptr) {
                TObjArray* branches = fEventTree->GetListOfBranches();
                // get the last event branch as input event branch
                if (branches->GetLast() > -1) {
                    TBranch* br = (TBranch*)branches->At(branches->GetLast());

                    if (br == nullptr || Count(br->GetName(), "EventBranch") == 0) {
                        RESTInfo << "No event branch inside file : " << filename << RESTendl;
                        RESTInfo << "This file may be a pure analysis file" << RESTendl;
                    } else {
                        string type = Replace(br->GetName(), "Branch", "", 0);
                        TClass* cl = TClass::GetClass(type.c_str());
                        if (cl->HasDictionary()) {
                            fInputEvent = REST_Reflection::Assembly(type);
                        } else if (fInputEvent != nullptr) {
                            delete fInputEvent;
                            fInputEvent = nullptr;
                        }

                        if (fInputEvent == nullptr) {
                            RESTError << "TRestRun:OpenInputFile. Cannot initialize input event, event "
                                         "tree not read"
                                      << RESTendl;
                            RESTError
                                << "Please install corresponding libraries to provide root dictionaries for "
                                   "class reading."
                                << RESTendl;
                            return;
                        }

                        fInputEvent->InitializeWithMetadata(this);
                        fEventTree->SetBranchAddress(br->GetName(), &fInputEvent);
                        fEventBranchLoc = branches->GetLast();
                        RESTDebug << "found event branch of event type: " << fInputEvent->ClassName()
                                  << RESTendl;
                    }
                }
            } else {
                string brname = (string)fInputEvent->ClassName() + "Branch";
                if (fEventTree->GetBranch(brname.c_str()) == nullptr) {
                    RESTWarning << "REST WARNING (OpenInputFile) : No matched event branch "
                                   "inside file : "
                                << filename << RESTendl;
                    RESTWarning << "Branch required: " << brname << RESTendl;
                } else {
                    fEventTree->SetBranchAddress(brname.c_str(), &fInputEvent);
                    RESTDebug << brname << " is found and set!" << RESTendl;
                }
            }
        } else {
            RESTDebug << "TRestRun:OpenInputFile. EventTree was not found" << RESTendl;
            RESTDebug << "This is a pure analysis file!" << RESTendl;
            fInputEvent = nullptr;
        }
    }
}

///////////////////////////////////////////////
/// \brief Extract file info from a file, and save it in the file info list
///
/// Items:
/// 1. matched file name formats
/// 2. Created time and date
/// 3. File size and entries
void TRestRun::ReadFileInfo(const string& filename) {
    RESTDebug << "begin collecting basic file info..." << filename << RESTendl;

    // basic file info: modify time, total bytes, etc.
    struct stat buf;
    FILE* fp = fopen(filename.c_str(), "rb");
    if (!fp) {
        RESTError << "TRestRun::ReadFileInfo. Something went wrong with fopen()!" << strerror(errno)
                  << RESTendl;
        exit(1);
    }
    int fd = fileno(fp);
    fstat(fd, &buf);
    fclose(fp);
    if (fEndTime == 0) {
        fEndTime = buf.st_mtime;
    }

    if (TRestTools::isRootFile((string)filename)) {
        fTotalBytes = buf.st_size;
    }

    RESTDebug << "begin matching file name pattern for more file info..." << RESTendl;
    // format example:
    // run[fRunNumber]_cobo[aaa]_frag[bbb]_Vm[TRestDetector::fAmplificationVoltage].graw
    // we are going to match it with inputfile:
    // run00042_cobo1_frag0000_Vm350.graw
    string format = GetParameter("inputFormat", "");
    string name = TRestTools::SeparatePathAndName(filename).second;

    vector<string> formatSectionList;
    vector<string> formatPrefixList;

    int pos = -1;
    int pos1 = 0;
    int pos2 = 0;
    while (true) {
        pos1 = format.find("[", pos + 1);
        pos2 = format.find("]", pos1);
        if (pos1 == -1 || pos2 == -1) {
            formatPrefixList.push_back(format.substr(pos + 1, -1));
            break;
        }

        formatSectionList.push_back(format.substr(pos1 + 1, pos2 - pos1 - 1));
        formatPrefixList.push_back(format.substr(pos + 1, pos1 - pos - 1));

        pos = pos2;
    }

    pos = -1;
    for (unsigned int i = 0; i < formatSectionList.size() && i < formatPrefixList.size() - 1; i++) {
        if (i != 0 && formatPrefixList[i].empty()) {
            RESTWarning << "file format reference contains error!" << RESTendl;
            return;
        }
        int pos1 = name.find(formatPrefixList[i], pos + 1) + formatPrefixList[i].size();
        if (formatPrefixList[i].empty()) pos1 = 0;
        int pos2 = name.find(formatPrefixList[i + 1], pos1);
        if (formatPrefixList[i + 1].empty()) pos2 = name.length();
        if (pos1 == -1 || pos2 == -1) {
            RESTWarning << "File pattern matching: file format mismatch!" << RESTendl;
            return;
        }

        string infoFromFileName = name.substr(pos1, pos2 - pos1);

        RESTDebug << "File pattern matching. key: " << formatSectionList[i] << " (between the mark \""
                  << formatPrefixList[i] << "\" and \"" << formatPrefixList[i + 1]
                  << "\"), value: " << infoFromFileName << RESTendl;

        // run[fRunNumber]_cobo[aaa]_frag[bbb]_Vm[TRestDetector::fAmplificationVoltage].graw
        bool inforead = false;
        if (!inforead) {
            // 1. store special file pattern parameters as TRestRun data member: fRunNumber
            if (DataMemberNameToParameterName(formatSectionList[i]) != "") {
                RESTValue member = RESTValue(this, this->ClassName()).GetDataMember(formatSectionList[i]);
                if (!member.IsZombie()) {
                    member.ParseString(infoFromFileName);
                    inforead = true;
                } else {
                    RESTWarning << "TRestRun: file name format field \"" << formatSectionList[i]
                                << "\"(value = " << infoFromFileName
                                << ") not registered, data member does not exist in TRestRun!" << RESTendl;
                }
            }
        }

        if (!inforead) {
            // 2. store special file pattern parameters as data member of the loaded
            // metadata class, e.g., TRestDetector::fAmplificationVoltage
            vector<string> classDataMember = Split(formatSectionList[i], "::");
            if (classDataMember.size() > 1) {
                TRestMetadata* meta = GetMetadataClass(classDataMember[0]);
                if (meta != nullptr) {
                    RESTValue member = RESTValue(meta, meta->ClassName()).GetDataMember(classDataMember[1]);
                    if (!member.IsZombie()) {
                        member.ParseString(infoFromFileName);
                        meta->UpdateMetadataMembers();
                        inforead = true;
                    } else {
                        RESTWarning << "TRestRun: file name format field \"" << formatSectionList[i]
                                    << "\"(value = " << infoFromFileName
                                    << ") not registered, metadata exist but without such datamember field!"
                                    << RESTendl;
                    }
                } else {
                    RESTWarning << "TRestRun: file name format field \"" << formatSectionList[i]
                                << "\"(value = " << infoFromFileName
                                << ") not registered, metadata does not exist!" << RESTendl;
                }
            }
        }

        // if (!inforead) {
        // 3. store file format fields to REST_ARGS as global parameter: aaa, bbb
        // REST_ARGS[formatsectionlist[i]] = infoFromFileName;
        // }

        pos = pos2 - 1;
    }
}

///////////////////////////////////////////////
/// \brief Reset file reading progress.
///
/// if input file is root file, just set current entry to be 0
/// if input file is external file, handled by external process, It will force
/// the process to reload the file.
void TRestRun::ResetEntry() {
    fCurrentEvent = 0;
    if (fFileProcess != nullptr) {
        fFileProcess->ResetEntry();
    }
}

///////////////////////////////////////////////
/// \brief Get next event by writing event data into target event and target
/// tree
///
/// returns 0 if success, returns -1 if failed, e.g. end of file
/// writing event data into target event calls the method TRestEvent::CloneTo()
/// writing observable data into target analysistree calls memcpy
/// It requires same branch structure, but we didn't verify it here.
Int_t TRestRun::GetNextEvent(TRestEvent* targetEvent, TRestAnalysisTree* targetTree) {
    bool messageShown = false;
    TRestEvent* eve = fInputEvent;

    if (fFileProcess != nullptr) {
        RESTDebug << "TRestRun: getting next event from external process" << RESTendl;
    GetEventExt:
        mutex_read.lock();
        fFileProcess->BeginOfEventProcess();
        eve = fFileProcess->ProcessEvent(nullptr);
        fFileProcess->EndOfEventProcess();
        mutex_read.unlock();
        fBytesRead = fFileProcess->GetTotalBytesRead();
        // if (targettree != nullptr) {
        //    for (int n = 0; n < fAnalysisTree->GetNumberOfObservables(); n++)
        //        targettree->SetObservable(n, fAnalysisTree->GetObservable(n));
        //}
        fCurrentEvent++;
    } else {
        if (eve == nullptr) {
            RESTDebug << "TRestRun::GetNextEvent(): input event has not been initialized!" << RESTendl;
        } else {
            RESTDebug << "TRestRun: getting next event from root file" << RESTendl;
            if (fAnalysisTree == nullptr) {
                RESTWarning << "error to get event from input file, missing analysis tree from input file"
                            << RESTendl;
                eve = nullptr;
            } else {
                if (fCurrentEvent >= fAnalysisTree->GetTree()->GetEntriesFast()) {
                    eve = nullptr;
                } else {
                    if (targetTree != nullptr) {
                        // normal reading procedure
                        eve->Initialize();
                        fBytesRead += fAnalysisTree->GetEntry(fCurrentEvent);
                        targetTree->SetEventInfo(fAnalysisTree);
                        for (int n = 0; n < fAnalysisTree->GetNumberOfObservables(); n++)
                            targetTree->SetObservable(n, fAnalysisTree->GetObservable(n));
                    }
                    if (fEventTree != nullptr) {
                        if (fEventTree->IsA() == TChain::Class()) {
                            Long64_t entry = fEventTree->LoadTree(fCurrentEvent);
                            fBytesRead += ((TBranch*)fEventTree->GetTree()->GetListOfBranches()->UncheckedAt(
                                               fEventBranchLoc))
                                              ->GetEntry(entry);
                        } else {
                            fBytesRead +=
                                ((TBranch*)fEventTree->GetListOfBranches()->UncheckedAt(fEventBranchLoc))
                                    ->GetEntry(fCurrentEvent);
                        }
                    }
                    fCurrentEvent++;
                }
            }
        }
    }

    // cout << fHangUpEndFile << endl;

    if (eve == nullptr) {
        if (fHangUpEndFile && fFileProcess != nullptr) {
            // if hangup is set, we continue calling ProcessEvent() of the
            // external process, until there is non-null event yielded
            if (!messageShown) {
                RESTEssential << "external process file reading reaches end, waiting for more files"
                              << RESTendl;
            }
            usleep(1000000);
            messageShown = true;
            fCurrentEvent--;
            goto GetEventExt;
        }
        fInputEvent = eve;
        // if (fFileProcess != nullptr) fFileProcess->EndProcess();
        return -1;
    }

    fInputEvent = eve;

    if (fInputEvent->GetID() == 0 && fInputEvent->GetSubID() == 0) {
        fInputEvent->SetID(fCurrentEvent - 1);
    }

    if (fInputEvent->GetRunOrigin() == 0) {
        fInputEvent->SetRunOrigin(fRunNumber);
    }

    targetEvent->Initialize();
    fInputEvent->CloneTo(targetEvent);

    return 0;
}

///////////////////////////////////////////////
/// \brief It returns a list of available event types inside the file
///
std::vector<std::string> TRestRun::GetEventTypesList() {
    std::vector<std::string> list;
    TObjArray* branches = GetEventTree()->GetListOfBranches();
    for (int i = 0; i <= branches->GetLast(); i++) {
        auto branch = (TBranch*)branches->At(i);
        if (((std::string)branch->GetName()).find("EventBranch") != string::npos) {
            std::string eventType = Replace((string)branch->GetName(), "Branch", "");
            list.emplace_back(eventType);
        }
    }
    return list;
}

///////////////////////////////////////////////
/// \brief Calls GetEntry() for both AnalysisTree and EventTree
///
void TRestRun::GetEntry(Long64_t entry) {
    if (entry >= GetEntries()) {
        RESTWarning << "TRestRun::GetEntry. Entry requested out of limits" << RESTendl;
        RESTWarning << "Total number of entries is : " << GetEntries() << RESTendl;
    }

    if (fAnalysisTree != nullptr) {
        fAnalysisTree->GetEntry(entry);
    }
    if (fEventTree != nullptr) {
        fEventTree->GetEntry(entry);
    }

    if (fInputEvent != nullptr) {
        fInputEvent->InitializeReferences(this);
    }

    fCurrentEvent = entry;
}

///////////////////////////////////////////////
/// \brief Form output file name according to file info list, proc info list and
/// run data.
///
/// It will replace the fields in output file name surrounded by "[]".
/// The file info list is created by TRestRun::ReadFileInfo(), the
/// proc info list is created by TRestProcessRunner::ReadProcInfo(),
/// the run data is from TRestRun's datamember(fRunNumber,fRunType, etc)
/// e.g. we can set output file name like:
/// \code Run[fRunNumber]_Proc_[LastProcess].root \endcode
/// and generates:
/// \code Run00000_T2018-01-01_08:00:00_Proc_sAna.root \endcode
TString TRestRun::FormFormat(const TString& FilenameFormat) {
    string inString = (string)FilenameFormat;
    string outString = (string)FilenameFormat;

    RESTDebug << "TRestRun::FormFormat. In string : " << inString << RESTendl;

    int pos = 0;
    while (true) {
        int pos1 = inString.find("[", pos);
        int pos2 = inString.find("]", pos1);
        if (pos1 == -1 || pos2 == -1) break;

        string targetstr = inString.substr(pos1, pos2 - pos1 + 1);   // with []
        string target = inString.substr(pos1 + 1, pos2 - pos1 - 1);  // without []
        string replacestr = GetRunInformation(target);

        RESTDebug << "TRestRun::FormFormat. target : " << target << RESTendl;
        RESTDebug << "TRestRun::FormFormat. replacestr : " << replacestr << RESTendl;

        // If we form an output file we are willing to form the output filename
        // using the latest version. But the version is set just before we
        // call Write.
        if (target == "fVersion") replacestr = (string)REST_RELEASE;
        if (target == "fCommit") replacestr = (string)REST_COMMIT;

        if (replacestr != target) {
            if (target == "fRunNumber" || target == "fParentRunNumber") {
                replacestr = Form("%05d", StringToInteger(replacestr));
            }
            outString = Replace(outString, targetstr, replacestr, 0);
        }
        pos = pos2 + 1;
    }

    return outString;
}

///////////////////////////////////////////////
/// \brief Form REST output file by merging a list of files together
///
/// If output file name is not given(=""), then it will recreate the output file
/// according to fOutputFileName. Otherwise it will update the given file. File
/// Merging is by calling TFileMerger. After this, it will format the merged file name.
/// This method is used to create output file after TRestProcessRunner is finished.
/// The metadata objects will also be written into the file.
TFile* TRestRun::MergeToOutputFile(vector<string> filenames, string outputfilename) {
    RESTDebug << "TRestRun::FormOutputFile. target : " << outputfilename << RESTendl;
    string filename;
    TFileMerger* m = new TFileMerger(false);
    if (outputfilename.empty()) {
        filename = fOutputFileName;
        RESTInfo << "Creating file : " << filename << RESTendl;
        m->OutputFile(filename.c_str(), "RECREATE");
    } else {
        filename = outputfilename;
        RESTInfo << "Updating file : " << filename << RESTendl;
        m->OutputFile(filename.c_str(), "UPDATE");
    }

    RESTDebug << "TRestRun::FormOutputFile. Starting to add files" << RESTendl;

    for (unsigned int i = 0; i < filenames.size(); i++) {
        m->AddFile(filenames[i].c_str(), false);
    }

    if (m->Merge()) {
        for (unsigned int i = 0; i < filenames.size(); i++) {
            remove(filenames[i].c_str());
        }
    } else {
        fOutputFileName = "";
        RESTError << "(Merge files) failed to merge process files." << RESTendl;
        exit(1);
    }

    delete m;

    // we rename the created output file
    fOutputFileName = FormFormat(filename);
    rename(filename.c_str(), fOutputFileName);

    // write metadata into the output file
    fOutputFile = new TFile(fOutputFileName, "update");
    RESTDebug << "TRestRun::FormOutputFile. Calling WriteWithDataBase()" << RESTendl;
    this->WriteWithDataBase();

    RESTcout << this->ClassName() << " Created ..." << RESTendl;
    RESTcout << "- Path : " << TRestTools::SeparatePathAndName((string)fOutputFileName).first << RESTendl;
    RESTcout << "- Filename : " << TRestTools::SeparatePathAndName((string)fOutputFileName).second
             << RESTendl;
    return fOutputFile;
}

///////////////////////////////////////////////
/// \brief Create a new TFile as REST output file. Writing metadata objects into it.
///
TFile* TRestRun::FormOutputFile() {
    CloseFile();

    fOutputFileName = FormFormat(fOutputFileName);
    // remove unwanted "./" etc. from the path while resolving them
    fOutputFileName = std::filesystem::weakly_canonical(fOutputFileName.Data());

    fOutputFile = new TFile(fOutputFileName, "recreate");
    fAnalysisTree = new TRestAnalysisTree("AnalysisTree", "AnalysisTree");
    fEventTree = new TTree("EventTree", "EventTree");

    fAnalysisTree->Write(nullptr, TObject::kOverwrite);
    fEventTree->Write(nullptr, TObject::kOverwrite);
    this->WriteWithDataBase();

    RESTcout << "TRestRun: Output File Created." << RESTendl;
    RESTcout << "- Path : " << TRestTools::SeparatePathAndName((string)fOutputFileName).first << RESTendl;
    RESTcout << "- Filename : " << TRestTools::SeparatePathAndName((string)fOutputFileName).second
             << RESTendl;
    return fOutputFile;
}

TFile* TRestRun::UpdateOutputFile() {
    if (fOutputFile != nullptr) {
        if (fOutputFile->IsOpen()) {
            fOutputFile->ReOpen("update");
        }

        fOutputFile->cd();

        fAnalysisTree->Write(nullptr, kOverwrite);
        fEventTree->Write(nullptr, kOverwrite);

        this->WriteWithDataBase();

        RESTcout << "TRestRun: Output File Updated." << RESTendl;
        RESTcout << "- Path : " << TRestTools::SeparatePathAndName((string)fOutputFileName).first << RESTendl;
        RESTcout << "- Filename : " << TRestTools::SeparatePathAndName((string)fOutputFileName).second
                 << RESTendl;
        return fOutputFile;

    } else {
        RESTError << "TRestRun::UpdateOutputFile(): output file is closed" << RESTendl;
    }
    return nullptr;
}

///////////////////////////////////////////////
/// \brief Write this object into TFile and add a new entry in database
///
/// level=0 : add a new run in database. run number is the next number, subrun
/// number is 0. level=1 (default) : add a new subrun in database. run number is
/// determined in BeginOfInit(). subrun number is the next number. if run does
/// not exist, it will create new if "force" is true. level>=2 : add a new file
/// in database. run number is determined in BeginOfInit(). subrun number is 0.
/// if not exist, it will create new if "force" is true.
void TRestRun::WriteWithDataBase() {
    TRestAnalysisTree* tree = nullptr;

    RESTDebug << "TRestRun::WriteWithDataBase. Getting entries in analysisTree" << RESTendl;

    // record the entries saved
    fEntriesSaved = -1;
    if (fOutputFile != nullptr) {
        tree = (TRestAnalysisTree*)fOutputFile->Get("AnalysisTree");
        if (tree != nullptr) {
            fEntriesSaved = tree->GetEntries();
        }
    }
    RESTDebug << "TRestRun::WriteWithDataBase. Entries found : " << fEntriesSaved << RESTendl;

    // If time was not written by any other process we record the current time
    if (fEndTime == 0) {
        time_t timev;
        time(&timev);
        fEndTime = (Double_t)timev;
    }

    fRunUser = REST_USER;

    // save metadata objects in file

    RESTDebug << "TRestRun::WriteWithDataBase. Calling this->Write(0, kOverwrite)" << RESTendl;
    this->Write(nullptr, kOverwrite);

    RESTDebug << "TRestRun::WriteWithDataBase. Succeed" << RESTendl;
    RESTDebug << "TRestRun::WriteWithDataBase. fMetadata.size() == " << fMetadata.size() << RESTendl;
    for (auto& metadata : fMetadata) {
        bool historic = false;
        RESTDebug << "TRestRun::WriteWithDataBase. fInputMetadata.size() == " << fInputMetadata.size()
                  << RESTendl;
        for (const auto& inputMetadata : fInputMetadata) {
            RESTDebug << metadata->GetName() << " == " << inputMetadata->GetName() << RESTendl;
            if (metadata == inputMetadata) {
                historic = true;
                break;
            }
        }

        if (!historic) {
            RESTDebug << "NO historic" << RESTendl;
            metadata->Write(metadata->GetName(), kOverwrite);
        } else {
            RESTDebug << "IS historic" << RESTendl;
            if (fSaveHistoricData) metadata->Write(metadata->GetName(), kOverwrite);
        }
    }

    // write to database
    RESTDebug << "TResRun::WriteWithDataBase. Run number is : " << fRunNumber << RESTendl;
    if (fRunNumber != -1) {
        int fileid = gDataBase->set_runfile(fRunNumber, (string)fOutputFileName);
        RESTcout << "DataBase Entry Added! Run Number: " << fRunNumber << ", File ID: " << fileid << RESTendl;
    }
}

///////////////////////////////////////////////
/// \brief Close both input file and output file, setting trees to nullptr also.
///
void TRestRun::CloseFile() {
    fEntriesSaved = -1;
    if (fAnalysisTree != nullptr) {
        fEntriesSaved = fAnalysisTree->GetEntries();
        if (fAnalysisTree->GetEntries() > 0 && fInputFile == nullptr) {
            if (fOutputFile != nullptr) {
                fOutputFile->cd();
                fAnalysisTree->Write(nullptr, kOverwrite);
                this->Write(nullptr, kOverwrite);
            }
        }
        delete fAnalysisTree;
        fAnalysisTree = nullptr;
    }

    if (fEventTree != nullptr) {
        if (fEventTree->GetEntries() > 0 && fInputFile == nullptr) fEventTree->Write(nullptr, kOverwrite);
        delete fEventTree;
        fEventTree = nullptr;
    }

    for (unsigned int i = 0; i < fMetadata.size(); i++) {
        for (unsigned int j = 0; j < fInputMetadata.size(); j++) {
            if (fMetadata[i] == fInputMetadata[j]) {
                delete fMetadata[i];
                fMetadata.erase(fMetadata.begin() + i);
                i--;
                fInputMetadata.erase(fInputMetadata.begin() + j);
                break;
            }
        }
    }

    if (fOutputFile != nullptr) {
        fOutputFile->Write(0, TObject::kOverwrite);
        fOutputFile->Close();
        delete fOutputFile;
        fOutputFile = nullptr;
    }
    if (fInputFile != nullptr) {
        fInputFile->Close();
        fInputFile = nullptr;
    }
}

///////////////////////////////////////////////
/// \brief Set external file process
///
void TRestRun::SetExtProcess(TRestEventProcess* p) {
    if (fFileProcess == nullptr && p != nullptr) {
        fFileProcess = p;

        fFileProcess->OpenInputFiles(Vector_cast<TString, string>(fInputFileNames));
        fFileProcess->InitProcess();
        fInputEvent = fFileProcess->GetOutputEvent();
        if (fInputEvent == nullptr) {
            RESTError << "The external process \"" << p->GetName() << "\" doesn't yield any output event!"
                      << RESTendl;
            exit(1);
        } else {
            fInputEvent->SetRunOrigin(fRunNumber);
            fInputEvent->SetSubRunOrigin(fParentRunNumber);
            fInputEvent->SetTimeStamp(fStartTime);
        }
        fInputFile = nullptr;
        // we make sure external processes can access to analysis tree
        fAnalysisTree = new TRestAnalysisTree("externalProcessAna", "externalProcessAna");
        p->SetAnalysisTree(fAnalysisTree);
        fTotalBytes = p->GetTotalBytes();

        GetNextEvent(fInputEvent, nullptr);
        // fAnalysisTree->CreateBranches();
        RESTInfo << "The external file process has been set! Name : " << fFileProcess->GetName() << RESTendl;
    } else {
        if (fFileProcess != nullptr) {
            RESTError << "There can only be one file process!" << RESTendl;
            exit(1);
        }
        if (p == nullptr) {
            RESTWarning << "Given file process is null, skipping..." << RESTendl;
        }
    }
}

///////////////////////////////////////////////
/// \brief Retarget input event in the tree
///
/// The input event is by default the last branch in EventTree, by calling
/// this method, it can be retargeted to other branches corresponding to the
/// given event.
void TRestRun::SetInputEvent(TRestEvent* event) {
    if (event != nullptr) {
        if (fEventTree != nullptr) {
            if (fInputEvent != nullptr) {
                fEventTree->SetBranchAddress((TString)fInputEvent->ClassName() + "Branch", nullptr);
                fEventTree->SetBranchStatus((TString)fInputEvent->ClassName() + "Branch", false);
            }
            TObjArray* branches = fEventTree->GetListOfBranches();
            string branchName = (string)event->ClassName() + "Branch";
            for (int i = 0; i <= branches->GetLast(); i++) {
                auto branch = (TBranch*)branches->At(i);
                if ((string)branch->GetName() == branchName) {
                    RESTDebug << "Setting input event.. Type: " << event->ClassName() << " Address: " << event
                              << RESTendl;
                    fInputEvent = event;
                    fEventTree->SetBranchAddress(branchName.c_str(), &fInputEvent);
                    fEventTree->SetBranchStatus(branchName.c_str(), false);
                    fEventBranchLoc = i;
                    break;
                } else if (i == branches->GetLast()) {
                    RESTWarning << "REST Warning : (TRestRun) cannot find corresponding "
                                   "branch in event tree!"
                                << RESTendl;
                    RESTWarning << "Event Type : " << event->ClassName() << RESTendl;
                    RESTWarning << "Input event not set!" << RESTendl;
                }
            }
        } else {
            fInputEvent = event;
        }
        this->GetEntry(fCurrentEvent);
    }
}

///////////////////////////////////////////////
/// \brief Add an event branch in output EventTree
///
void TRestRun::AddEventBranch(TRestEvent* event) {
    if (event != nullptr) {
        if (fEventTree != nullptr) {
            string eventName = (string)event->ClassName();
            string branchName = eventName + "Branch";
            fEventTree->Branch(branchName.c_str(), event);
            fEventTree->SetTitle((eventName + "Tree").c_str());
        }
    }
}

///////////////////////////////////////////////
/// \brief Open the root file and import the metadata of the given name.
///
/// The metadata class can be recovered to the same condition as when it is
/// saved.
void TRestRun::ImportMetadata(const TString& File, const TString& name, const TString& type, Bool_t store) {
    const TString thisFile = SearchFile(File.Data());
    if (thisFile == "") {
        RESTError << "(ImportMetadata): The file " << thisFile << " does not exist!" << RESTendl;
        RESTError << RESTendl;
        return;
    }
    if (!TRestTools::isRootFile(thisFile.Data())) {
        RESTError << "(ImportMetadata) : The file " << thisFile << " is not root file!" << RESTendl;
        RESTError << "If you want to initialize metadata from rml file, use <TRest section!" << RESTendl;
        return;
    }

    TFile* file = TFile::Open(thisFile);
    // TODO give error in case we try to obtain a class that is not TRestMetadata
    if (type == "" && name == "") {
        RESTError << "(ImportMetadata) : metadata type and name is not "
                     "specified!"
                  << RESTendl;
        return;
    }

    TRestMetadata* meta = nullptr;
    if (name != "") {
        meta = GetMetadata(name, file);
    } else if (type != "") {
        meta = GetMetadataClass(type, file);
    }

    if (meta == nullptr) {
        cout << "REST ERROR (ImportMetadata) : " << name << " does not exist." << endl;
        cout << "Inside root file : " << File << endl;
        GetChar();
        file->Close();
        delete file;
        return;
    }

    if (store)
        meta->Store();
    else
        meta->DoNotStore();

    fMetadata.push_back(meta);
    meta->LoadConfigFromBuffer();
    file->Close();
    delete file;
}

Int_t TRestRun::Write(const char* name, Int_t option, Int_t bufsize) {
    ReSetVersion();
    return TRestMetadata::Write(name, option, bufsize);
}

Double_t TRestRun::GetRunLength() const {
    if (fEndTime - fStartTime == -1) {
        cout << "Run time is not set\n";
    }
    return fEndTime - fStartTime;
}

Long64_t TRestRun::GetTotalBytes() {
    if (fFileProcess != nullptr) {
        fTotalBytes = fFileProcess->GetTotalBytes();
    }
    return fTotalBytes;
}

Long64_t TRestRun::GetEntries() const {
    if (fAnalysisTree != nullptr) {
        return fAnalysisTree->GetEntries();
    }
    return REST_MAXIMUM_EVENTS;
}

// Getters
TRestEvent* TRestRun::GetEventWithID(Int_t eventID, Int_t subEventID, const TString& tag) {
    if (fAnalysisTree != nullptr) {
        int nEntries = fAnalysisTree->GetEntries();

        // set analysis tree to read only three branches
        fAnalysisTree->SetBranchStatus("*", false);
        fAnalysisTree->SetBranchStatus("eventID", true);
        fAnalysisTree->SetBranchStatus("subEventID", true);
        fAnalysisTree->SetBranchStatus("subEventTag", true);

        // just look through the whole analysis tree and find the entry
        // this is not good!
        for (int i = 0; i < nEntries; i++) {
            fAnalysisTree->GetEntry(i);
            if (fAnalysisTree->GetEventID() == eventID) {
                if (subEventID != -1 && fAnalysisTree->GetSubEventID() != subEventID) continue;
                if (tag != "" && fAnalysisTree->GetSubEventTag() != tag) continue;
                if (fEventTree != nullptr) fEventTree->GetEntry(i);
                fAnalysisTree->SetBranchStatus("*", true);
                fAnalysisTree->GetEntry(i);
                fCurrentEvent = i;
                return fInputEvent;
            }
        }
        // reset the branch status
        fAnalysisTree->SetBranchStatus("*", true);
    }
    return nullptr;
}

std::vector<int> TRestRun::GetEventEntriesWithConditions(const string& cuts, int startingIndex,
                                                         int maxNumber) {
    int max = maxNumber;
    if (max < 0) max = GetEntries();

    std::vector<int> eventIds;
    // parsing cuts
    std::vector<string> observables;
    std::vector<string> operators;
    std::vector<Double_t> values;
    // it is necessary that this vector is sorted from longest to shortest
    const std::vector<string> validOperators = {"==", "<=", ">=", "=", ">", "<"};

    vector<string> cutsVector = Split(cuts, "&&", false, true);

    for (unsigned int i = 0; i < cutsVector.size(); i++) {
        string cut = cutsVector[i];
        for (unsigned int j = 0; j < validOperators.size(); j++) {
            if (cut.find(validOperators[j]) != string::npos) {
                operators.push_back(validOperators[j]);
                observables.push_back((string)cut.substr(0, cut.find(validOperators[j])));
                values.push_back(std::stod((string)cut.substr(
                    cut.find(validOperators[j]) + validOperators[j].length(), string::npos)));
                break;
            }
        }
    }

    // check if observable name corresponds to a valid observable on the tree
    if (fAnalysisTree == nullptr) {
        return eventIds;
    }
    Int_t nEntries = fAnalysisTree->GetEntries();
    auto branches = fAnalysisTree->GetListOfBranches();
    std::set<string> branchNames;
    for (int i = 0; i < branches->GetEntries(); i++) {
        branchNames.insert((string)branches->At(i)->GetName());
    }
    // verify all observables in cuts are branch names
    for (unsigned int i = 0; i < observables.size(); i++) {
        // verify operators
        if (std::find(validOperators.begin(), validOperators.end(), operators[i]) == validOperators.end()) {
            // invalid operation
            cout << "invalid operation '" << operators[i] << "' for 'TRestRun::GetEventIdsWithConditions'"
                 << endl;
            return eventIds;
        }
        // verify observables
        if (branchNames.count(observables[i]) == 0) {
            // invalid observable name
            cout << "invalid observable '" << observables[i] << "' for 'TRestRun::GetEventIdsWithConditions'"
                 << endl;
            cout << "valid branch names: ";
            for (auto branchName : branchNames) {
                cout << branchName << " ";
            }
            cout << endl;
            return eventIds;
        }
    }
    // read only the necessary branches
    fAnalysisTree->SetBranchStatus("*", false);
    for (unsigned int i = 0; i < observables.size(); i++) {
        fAnalysisTree->SetBranchStatus(observables[i].c_str(), true);
    }
    // comparison code
    Double_t valueToCompareFrom;
    bool comparisonResult;
    int i;
    for (int iNoOffset = 0; iNoOffset < nEntries; iNoOffset++) {
        i = (iNoOffset + startingIndex) % nEntries;
        fAnalysisTree->GetEntry(i);
        comparisonResult = true;
        for (unsigned int j = 0; j < observables.size(); j++) {
            valueToCompareFrom = fAnalysisTree->GetDblObservableValue(observables[j].c_str());
            if (operators[j] == "=" || operators[j] == "==") {
                comparisonResult = comparisonResult && (valueToCompareFrom == values[j]);
            } else if (operators[j] == "<") {
                comparisonResult = comparisonResult && (valueToCompareFrom < values[j]);
            } else if (operators[j] == "<=") {
                comparisonResult = comparisonResult && (valueToCompareFrom <= values[j]);
            } else if (operators[j] == ">") {
                comparisonResult = comparisonResult && (valueToCompareFrom > values[j]);
            } else if (operators[j] == ">=") {
                comparisonResult = comparisonResult && (valueToCompareFrom >= values[j]);
            }
        }

        if (comparisonResult) {
            if ((int)eventIds.size() < max) {
                eventIds.push_back(i);
            } else {
                break;
            }
        }
    }
    // reset branch status
    fAnalysisTree->SetBranchStatus("*", true);
    return eventIds;
}

std::vector<int> TRestRun::GetEventIdsWithConditions(const string& cuts, int startingIndex, int maxNumber) {
    int max = maxNumber;
    if (max < 0) max = GetEntries();

    auto indices = GetEventEntriesWithConditions(cuts, startingIndex, max);
    std::vector<int> ids;
    for (unsigned int i = 0; i < indices.size(); i++) {
        GetEntry(indices[i]);
        ids.push_back(fAnalysisTree->GetEventID());
    }
    return ids;
}

///////////////////////////////////////////////
/// \brief Load the next event that satisfies the conditions specified by a string
///
/// \param conditions: string specifying conditions, supporting multiple conditions separated by ":",
/// allowed symbols include "<", "<=", ">", ">=", "=", "==". For example "A>=2.2:B==4".
/// \return TRestEvent
TRestEvent* TRestRun::GetNextEventWithConditions(const string& cuts) {
    // we retrieve only one index starting from position set by the counter and increase by one
    if (fEventIndexCounter >= GetEntries()) {
        fEventIndexCounter = 0;
    }
    auto indices = GetEventEntriesWithConditions(cuts, fEventIndexCounter++, 1);
    if (indices.size() == 0) {
        // no events found
        return nullptr;
    } else {
        fAnalysisTree->GetEntry(indices[0]);
        fEventTree->GetEntry(indices[0]);
        fCurrentEvent = indices[0];
        return fInputEvent;
    }
}

string TRestRun::GetRunInformation(const string& info) {
    string result = GetParameter(info, "");
    if (result != "") {
        return result;
    }

    result = GetDataMemberValue(info);
    if (result != "") {
        return result;
    }

    result = GetDataMemberValue(ParameterNameToDataMemberName(info));
    if (result != "") {
        return result;
    }

    if (fHostmgr && fHostmgr->GetProcessRunner() != nullptr) {
        result = fHostmgr->GetProcessRunner()->GetProcInfo(info);
        if (result != "") {
            return result;
        }
    }

    return info;
}

TRestMetadata* TRestRun::GetMetadataClass(const TString& type, TFile* file) {
    if (file != nullptr) {
        TIter nextkey(file->GetListOfKeys());
        TKey* key;
        while ((key = (TKey*)nextkey())) {
            string kName = key->GetClassName();

            if (REST_Reflection::GetClassQuick(kName.c_str()) != nullptr &&
                REST_Reflection::GetClassQuick(kName.c_str())->InheritsFrom(type)) {
                TRestMetadata* metadata = file->Get<TRestMetadata>(key->GetName());

                if (metadata != nullptr && metadata->InheritsFrom("TRestMetadata")) {
                    return metadata;
                } else {
                    RESTWarning << "TRestRun::GetMetadataClass() : The object to import is "
                                   "not inherited from TRestMetadata"
                                << RESTendl;
                }
            }
        }
    } else {
        for (unsigned int i = 0; i < fMetadata.size(); i++)
            if (fMetadata[i]->InheritsFrom(type)) return fMetadata[i];

        if (fInputFile != nullptr && this->GetVersionCode() >= TRestTools::ConvertVersionCode("2.2.1")) {
            return GetMetadataClass(type, fInputFile);
        }
    }

    return nullptr;
}

TRestMetadata* TRestRun::GetMetadata(const TString& name, TFile* file) {
    if (file != nullptr) {
        TIter nextkey(file->GetListOfKeys());
        TKey* key;
        while ((key = (TKey*)nextkey())) {
            string kName = key->GetName();

            if (kName == name) {
                TRestMetadata* metadata = file->Get<TRestMetadata>(name);

                if (metadata->InheritsFrom("TRestMetadata")) {
                    return metadata;
                } else {
                    RESTWarning << "TRestRun::GetMetadata() : The object to import is not "
                                   "inherited from TRestMetadata"
                                << RESTendl;
                }
            }
        }
    } else {
        for (unsigned int i = 0; i < fMetadata.size(); i++) {
            if (fMetadata[i]->GetName() == name) {
                return fMetadata[i];
            }
        }
    }

    return nullptr;
}

std::vector<std::string> TRestRun::GetMetadataNames() {
    std::vector<std::string> strings;

    for (int n = 0; n < GetNumberOfMetadata(); n++) strings.push_back(fMetadata[n]->GetName());

    return strings;
}

std::vector<std::string> TRestRun::GetMetadataTitles() {
    std::vector<std::string> strings;

    for (int n = 0; n < GetNumberOfMetadata(); n++) strings.push_back(fMetadata[n]->GetTitle());

    return strings;
}

///////////////////////////////////////////////
/// \brief It will replace the data members contained inside the string given as input. The data members in
/// the input string should be written using the following format [MetadataClass::fDataMember].
///
/// \return The string with data members replaced
///
string TRestRun::ReplaceMetadataMembers(const string& instr, Int_t precision) {
    if (instr.find("[", 0) == string::npos) return instr;
    string outstring = instr;

    int startPosition = 0;
    int endPosition = 0;
    while ((startPosition = outstring.find("[", endPosition)) != (int)string::npos) {
        endPosition = outstring.find("]", startPosition + 1);
        string s = outstring.substr(startPosition + 1, endPosition - startPosition - 1);
        int cont = count(s.begin(), s.end(), '[') - count(s.begin(), s.end(), ']');

        if (cont < 0) RESTError << "This is a coding error at ReplaceMetadataMembers!" << RESTendl;

        // We search for the enclosing ']'. Since we might find a vector index inside.
        while (cont > 0) {
            endPosition = outstring.find("]", endPosition + 1);
            s = outstring.substr(startPosition + 1, endPosition - startPosition - 1);
            cont = count(s.begin(), s.end(), '[') - count(s.begin(), s.end(), ']');
            if (endPosition == (int)string::npos) break;
        }
        if (endPosition == (int)string::npos) break;

        string expressionToReplace = outstring.substr(startPosition + 1, endPosition - startPosition - 1);
        string value = ReplaceMetadataMember(expressionToReplace);

        outstring.replace(startPosition, endPosition - startPosition + 1, value);
        endPosition = 0;
    }

    outstring = Replace(outstring, "{{", "[");
    outstring = Replace(outstring, "}}", "]");

    return REST_StringHelper::ReplaceMathematicalExpressions(outstring, precision);
}

///////////////////////////////////////////////
/// \brief It will replace the data member from the corresponding metadata class type or name
/// defined in the input string.
//
/// The input string should contain the metadata class type or name following the format
/// `string instr = "mdName::fDataMember";` or `string instr = "TRestMetadataClass::fDataMember";`.
/// or the format `string instr = "mdName->fDataMember";`. Both, `::` and `->` are allowed
/// to separate class and member of the class.
///
/// The method will work with both, metadata class or metadata name. First it will be evaluated
/// if the metadata name is found, using the method TRestRun::GetMetadata. If not, it will try
/// to check if the corresponding input string is giving a metadata class type, using the method
/// TRestRun::GetMetadataClass.
///
/// \return The corresponding class data member value in string format.
///
string TRestRun::ReplaceMetadataMember(const string& instr, Int_t precision) {
    if (instr.find("::") == string::npos && instr.find("->") == string::npos) {
        return "{{" + instr + "}}";
    }
    vector<string> results = Split(instr, "::", false, true);
    if (results.size() == 1) results = Split(instr, "->", false, true);

    if (results.size() == 2) {
        size_t index = 0;
        int pos1 = results[1].find("[", 0);
        int pos2 = results[1].find("]", pos1);
        if (pos1 > 0 && pos2 > 0) {
            string indexStr = results[1].substr(pos1 + 1, pos2 - pos1 - 1);  // without []

            index = StringToInteger(indexStr);
            if (index < 0) index = 0;

            results[1] = results[1].substr(0, pos1);
        }

        if (GetMetadata(results[0])) {
            if (index >= this->GetMetadata(results[0])->GetDataMemberValues(results[1], precision).size()) {
                RESTWarning << "TRestRun::ReplaceMetadataMember. Index out of range!" << RESTendl;
                RESTWarning << "Returning the first element" << RESTendl;
                index = 0;
            }
            return this->GetMetadata(results[0])->GetDataMemberValues(results[1], precision)[index];
        }

        if (GetMetadataClass(results[0])) {
            if (index >=
                this->GetMetadataClass(results[0])->GetDataMemberValues(results[1], precision).size()) {
                RESTWarning << "TRestRun::ReplaceMetadataMember. Index out of range!" << RESTendl;
                RESTWarning << "Returning the first element" << RESTendl;
                index = 0;
            }
            return this->GetMetadataClass(results[0])->GetDataMemberValues(results[1], precision)[index];
        }

    } else
        RESTError << "TRestRun::ReplaceMetadataMember. Wrong number of elements found" << RESTendl;

    RESTWarning << "TRestRun::ReplaceMetadataMember. " << instr << " not found!" << RESTendl;
    return "";
}

///////////////////////////////////////////////
/// \brief It will evaluate the expression given including the data member from the
/// corresponding metadata class type or name defined in the input string.
//
/// The input expression should contain the metadata class type or name using the format
/// `metadata->member` or `metadata::fMember`, where metadata might be the class name or
/// the intrinsic name assigned to the metadata object.
///
/// Examples:
/// \code
/// run->EvaluateMetadataMember("TRestDetectorSetup->fDetectorPressure == 4");
/// run->EvaluateMetadataMember("sc->fMinValues[1] == 3" );
/// \endcode
///
/// Both, `::` and `->` are allowed to separate class and the data member.
///
/// \return The result of the evaluated expression. If the input string is empty
/// it will return true.
///
Bool_t TRestRun::EvaluateMetadataMember(const string& instr) {
    if (instr == "") return true;

    std::vector<string> oper = {"=", "==", "<=", "<", ">=", ">", "!="};

    string expOp = "";
    std::vector<string> results;
    for (unsigned int n = 0; n < oper.size(); n++) {
        size_t pos = 0;
        if (instr.find("->") != string::npos) pos = instr.find("->") + 2;

        if (instr.find(oper[n], pos) != string::npos) {
            expOp = oper[n];
            results = Split(instr, oper[n], false, true, pos);
            break;
        }
    }

    if (expOp == "") {
        RESTWarning << "TRestRun::EvaluateMetadataMember. Not valid operator found in expression : " << instr
                    << RESTendl;
        return false;
    }

    if (results.size() != 2) {
        RESTWarning << "TRestRun::EvaluateMetadataMember. Not valid expression : " << instr << RESTendl;
        return false;
    }

    if (!isANumber(results[1])) {
        if (ReplaceMetadataMember(results[0]).find(results[1]) != string::npos)
            return true;
        else
            return false;
    }

    Double_t lvalue = StringToDouble(ReplaceMetadataMember(results[0]));
    Double_t rvalue = StringToDouble(results[1]);

    if (expOp == "=" && lvalue == rvalue) return true;
    if (expOp == "==" && lvalue == rvalue) return true;
    if (expOp == "<=" && lvalue <= rvalue) return true;
    if (expOp == "<" && lvalue < rvalue) return true;
    if (expOp == ">=" && lvalue >= rvalue) return true;
    if (expOp == ">" && lvalue > rvalue) return true;
    if (expOp == "!=" && lvalue != rvalue) return true;

    return false;
}

///////////////////////////////////////////////
/// \brief Prints the basic run information
///
void TRestRun::PrintMetadata() {
    // cout.precision(10);
    TRestMetadata::PrintMetadata();

    RESTMetadata << "Version : " << this->GetVersion() << RESTendl;
    RESTMetadata << "Parent run number : " << GetParentRunNumber() << RESTendl;
    RESTMetadata << "Run number : " << GetRunNumber() << RESTendl;
    RESTMetadata << "Experiment/project : " << GetExperimentName() << RESTendl;
    RESTMetadata << "Run type : " << GetRunType() << RESTendl;
    RESTMetadata << "Run tag : " << GetRunTag() << RESTendl;
    RESTMetadata << "Run user : " << GetRunUser() << RESTendl;
    RESTMetadata << "Run description : " << GetRunDescription() << RESTendl;
    RESTMetadata << "Start Date/Time : " << ToDateTimeString(GetStartTimestamp()) << " ("
                 << GetStartTimestamp() << ")" << RESTendl;
    RESTMetadata << "End Date/Time : " << ToDateTimeString(GetEndTimestamp()) << " (" << GetEndTimestamp()
                 << ")" << RESTendl;
    RESTMetadata << "Input file : " << TRestTools::GetPureFileName((string)GetInputFileNamePattern())
                 << RESTendl;
    RESTMetadata << "Output file : " << TRestTools::GetPureFileName((string)GetOutputFileName()) << RESTendl;
    if (fInputFile != nullptr) {
        RESTMetadata << "Data file : " << fInputFile->GetName();
        if (fNFilesSplit > 0) {
            RESTMetadata << " (Splitted into " << fNFilesSplit + 1 << " files)" << RESTendl;
        } else {
            RESTMetadata << RESTendl;
        }
    }
    RESTMetadata << "Number of events : " << fEntriesSaved << RESTendl;
    // metadata << "Input filename : " << fInputFilename << endl;
    // metadata << "Output filename : " << fOutputFilename << endl;
    // metadata << "Number of initial events : " << GetNumberOfEvents() << endl;
    // metadata << "Number of processed events : " << fProcessedEvents << endl;
    RESTMetadata << "---------------------------------------" << RESTendl;
    RESTMetadata << RESTendl;
}

///////////////////////////////////////////////
/// \brief Prints the run start date and time in human format
///
void TRestRun::PrintStartDate() {
    cout.precision(10);

    cout << "------------------------" << endl;
    cout << "---- Run start date ----" << endl;
    cout << "------------------------" << endl;
    cout << "Unix time : " << fStartTime << endl;
    time_t tt = (time_t)fStartTime;
    struct tm* tm = localtime(&tt);

    char date[20];
    strftime(date, sizeof(date), "%Y-%m-%d", tm);
    cout << "Date : " << date << endl;

    char time[20];
    strftime(time, sizeof(time), "%H:%M:%S", tm);
    cout << "Time : " << time << endl;
    cout << "++++++++++++++++++++++++" << endl;
}

void TRestRun::PrintEndDate() {
    cout << "----------------------" << endl;
    cout << "---- Run end date ----" << endl;
    cout << "----------------------" << endl;
    cout << "Unix time : " << fEndTime << endl;
    time_t tt = (time_t)fEndTime;
    struct tm* tm = localtime(&tt);

    char date[20];
    strftime(date, sizeof(date), "%Y-%m-%d", tm);
    cout << "Date : " << date << endl;

    char time[20];
    strftime(time, sizeof(time), "%H:%M:%S", tm);
    cout << "Time : " << time << endl;
    cout << "++++++++++++++++++++++++" << endl;
}

///////////////////////////////////////////////
/// \brief Prints out all the warnings registered by metadata classes accessible to
/// TRestRun, thats metadata and processes previously used in a data chain.
///
void TRestRun::PrintErrors() {
    Int_t nErrors = 0;
    for (unsigned int n = 0; n < fMetadata.size(); n++)
        if (fMetadata[n]->GetError()) nErrors++;

    if (nErrors) {
        cout << endl;
        RESTError << "Found a total of " << nErrors << " metadata errors" << RESTendl;
        for (unsigned int n = 0; n < fMetadata.size(); n++)
            if (fMetadata[n]->GetError()) {
                cout << endl;
                RESTError << "Class: " << fMetadata[n]->ClassName() << " Name: " << fMetadata[n]->GetName()
                          << RESTendl;
                RESTError << "Number of errors: " << fMetadata[n]->GetNumberOfErrors() << RESTendl;
                RESTError << "Message: " << fMetadata[n]->GetErrorMessage() << RESTendl;
            }
        cout << endl;
    } else {
        cout << "No errors found!" << endl;
    }
}

///////////////////////////////////////////////
/// \brief Prints out all the warnings registered by metadata classes accessible to
/// TRestRun, thats metadata and processes previously used in a data chain.
///
void TRestRun::PrintWarnings() {
    Int_t nWarnings = 0;
    for (unsigned int n = 0; n < fMetadata.size(); n++)
        if (fMetadata[n]->GetWarning()) nWarnings++;

    if (nWarnings) {
        cout << endl;
        RESTWarning << "Found a total of " << nWarnings << " metadata warnings" << RESTendl;
        for (unsigned int n = 0; n < fMetadata.size(); n++)
            if (fMetadata[n]->GetWarning()) {
                cout << endl;
                RESTWarning << "Class: " << fMetadata[n]->ClassName() << " Name: " << fMetadata[n]->GetName()
                            << RESTendl;
                RESTWarning << "Number of warnings: " << fMetadata[n]->GetNumberOfWarnings() << RESTendl;
                RESTWarning << "Message: " << fMetadata[n]->GetWarningMessage() << RESTendl;
            }
        cout << endl;
    } else {
        cout << "No warnings found!" << endl;
    }
}
