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

#include "TRestDataBase.h"
#include "TRestEventProcess.h"
#include "TRestManager.h"
#include "TRestVersion.h"

using namespace std;

std::mutex mutex2;

ClassImp(TRestRun);

TRestRun::TRestRun() { Initialize(); }

TRestRun::TRestRun(const string& rootfilename) {
    Initialize();
    OpenInputFile(rootfilename);
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

    // fOutputAnalysisTree = nullptr;

    fInputFileName = "null";
    fOutputFileName = "rest_default.root";

    fBytesReaded = 0;
    fTotalBytes = -1;
    fOverwrite = true;
    fEntriesSaved = -1;

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
/// 2. Initialize runnumber and input file name. They follow non-trival logic.
/// 3. Construct default output file name with runNumber, runTag, etc.
/// 4. Loop over sections to initialize metadata
/// 5. Open input file(s), read the stored metadata and trees, read file name
/// pattern to TRestDetector metadata.
///
void TRestRun::InitFromConfigFile() {
    debug << "Initializing TRestRun from config file, version: " << REST_RELEASE << endl;
    fFromRML = true;
    ReSetVersion();

    // 1. Read basic parameter
    fRunUser = REST_USER;
    // fRunType = GetParameter("runType", "ANALYSIS").c_str();
    // fRunDescription = GetParameter("runDescription", "").c_str();
    // fExperimentName = GetParameter("experiment", "preserve").c_str();
    // fRunTag = GetParameter("runTag", "noTag").c_str();
    ReadAllParameters();

    // 2. Initialize runnumber and input file name. They follow non-trival logic
    fRunNumber = -1;
    fParentRunNumber = 0;
    string runNstr = GetParameter("runNumber", "-1");
    string inputname = GetParameter("inputFileName", "");
    inputname = TRestTools::RemoveMultipleSlash(inputname);
    string inputnameold = GetParameter("inputFile", "default");
    if (inputnameold != "default") {
        warning << "Parameter \"inputFile\" in rml is obsolete! Please update it to \"inputFileName\""
                << endl;
        if (inputname == "") {
            inputname = inputnameold;
        }
    }
    if (ToUpper(runNstr) == "AUTO" && ToUpper(inputname) == "AUTO") {
        ferr << "TRestRun: run number and input file name cannot both be "
                "\"AUTO\""
             << endl;
        exit(1);
    }

    if (ToUpper(inputname) != "AUTO") {
        fInputFileName = inputname;
        fInputFileNames = Vector_cast<string, TString>(TRestTools::GetFilesMatchingPattern(inputname));
    }

    if (ToUpper(runNstr) != "AUTO") {
        fRunNumber = atoi(runNstr.c_str());
        fStartTime = gDataBase->query_run(fRunNumber).tstart;
        fEndTime = gDataBase->query_run(fRunNumber).tend;
    }

    if (ToUpper(inputname) == "AUTO") {
        TRestDataBase* db = gDataBase;
        auto files = db->query_run_files(fRunNumber);
        fInputFileName = db->query_run(fRunNumber).value;  // run entry value is file pattern
        fInputFileNames = Vector_cast<DBFile, TString>(files);
    }

    if (ToUpper(runNstr) == "AUTO") {
        TRestDataBase* db = gDataBase;
        auto runs = db->search_run_with_file((string)fInputFileName);
        if (runs.size() > 0) {
            fRunNumber = runs[0];
        } else {
            fRunNumber = -1;
        }

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

    if (fInputFileNames.size() == 0) {
        if (fInputFileName != "") {
            ferr << "cannot find the input file!" << endl;
            exit(1);
        } else {
            warning << "no input file added" << endl;
        }
        // throw;
    }

    // 3. Construct output file name
    string outputdir = (string)GetDataPath();
    string outputname = GetParameter("outputFileName", "default");
    string outputnameold = GetParameter("outputFile", "default");
    if (outputnameold != "default") {
        warning << "Parameter \"outputFile\" in rml is obsolete! Please update it to \"outputFileName\""
                << endl;
        if (outputname == "default") {
            outputname = outputnameold;
        }
    }
    if (ToUpper(outputname) == "DEFAULT") {
        string expName = RemoveWhiteSpaces((string)GetExperimentName());
        string runType = RemoveWhiteSpaces((string)GetRunType());
        char runParentStr[256];
        sprintf(runParentStr, "%05d", fParentRunNumber);
        char runNumberStr[256];
        sprintf(runNumberStr, "%05d", fRunNumber);

        fOutputFileName = outputdir + "Run_" + expName + "_" + fRunUser + "_" + runType + "_" + fRunTag +
                          "_" + (TString)runNumberStr + "_" + (TString)runParentStr + "_V" + REST_RELEASE +
                          ".root";

        // fOverwrite = ToUpper(GetParameter("overwrite", "on")) != "OFF";
        while (!fOverwrite && TRestTools::fileExists((string)fOutputFileName)) {
            fParentRunNumber++;
            sprintf(runParentStr, "%05d", fParentRunNumber);
            fOutputFileName = outputdir + "Run_" + expName + "_" + fRunUser + "_" + runType + "_" + fRunTag +
                              "_" + (TString)runNumberStr + "_" + (TString)runParentStr + "_V" +
                              REST_RELEASE + ".root";
        }
    } else if (ToUpper(outputname) == "NULL" || outputname == "/dev/null") {
        fOutputFileName = "/dev/null";
    } else if (TRestTools::isAbsolutePath(outputname)) {
        fOutputFileName = outputname;
        outputdir = TRestTools::SeparatePathAndName((string)fOutputFileName).first;
    } else {
        fOutputFileName = outputdir + "/" + outputname;
    }
    // remove multiple slashes from fOutputFileName
    fOutputFileName = (TString)TRestTools::RemoveMultipleSlash((string)fOutputFileName);

    if (!TRestTools::fileExists(outputdir)) {
        system((TString) "mkdir -p " + outputdir);
    }
    if (!TRestTools::isPathWritable(outputdir)) {
        ferr << "TRestRun: Output path does not exist or it is not writable." << endl;
        ferr << "Path : " << outputdir << endl;
        exit(1);
    }

    // 4. Loop over sections to initialize metadata
    TiXmlElement* e = fElement->FirstChildElement();
    while (e != nullptr) {
        string keydeclare = e->Value();
        if (keydeclare == "addMetadata") {
            if (e->Attribute("file") != nullptr) {
                ImportMetadata(e->Attribute("file"), e->Attribute("name"), e->Attribute("type"), true);
            } else {
                warning << "Wrong definition of addMetadata! Metadata name or file name "
                           "is not given!"
                        << endl;
            }
        } else if (keydeclare == "addProcess") {
            bool active = StringToBool(GetParameter("value", e, ""));
            if (!active) {
                e = e->NextSiblingElement();
                continue;
            }
            string processName = GetParameter("name", e, "");
            string processType = GetParameter("type", e, "");
            if (processType == "") {
                warning << "Bad expression of addProcess" << endl;
            } else if (processName == "") {
                warning << "Event process " << processType << " has no name, it will be skipped" << endl;
            }
            TRestEventProcess* pc = REST_Reflection::Assembly(processType);
            if (!pc->isExternal()) {
                warning << "This is not an external file process!" << endl;
            } else {
                pc->LoadConfigFromElement(e, fElementGlobal);
                pc->SetRunInfo(this);
                pc->SetHostmgr(fHostmgr);

                SetExtProcess(pc);
            }
        } else if (Count(keydeclare, "TRest") > 0) {
            if (e->Attribute("file") != nullptr && TRestTools::isRootFile(e->Attribute("file"))) {
                warning << "TRestRun: A root file is being included in section <" << keydeclare
                        << " ! To import metadata from this file, use <addMetadata" << endl;
                warning << "Skipping..." << endl;
            }
            // if (e->Attribute("file") != nullptr && (string)e->Attribute("file") == "server") {
            //    // read meta-sections from database
            //    auto url = gDataBase->query_data(DBEntry(fRunNumber, "META_RML", e->Value())).value;
            //    string file = TRestTools::DownloadRemoteFile(url);
            //    e->SetAttribute("file", file.c_str());
            //    ExpandIncludeFile(e);
            //}

            TRestMetadata* meta = REST_Reflection::Assembly(keydeclare);
            if (meta == nullptr) {
                warning << "failed to add metadata \"" << keydeclare << "\"" << endl;
                e = e->NextSiblingElement();
                continue;
            }
            meta->SetHostmgr(fHostmgr);
            fMetadata.push_back(meta);
            meta->LoadConfigFromElement(e, fElementGlobal);
        }
        e = e->NextSiblingElement();
    }

    // 5. Open input file(s). We open input file at the last stage in case the file name pattern
    // reading requires TRestDetector
    OpenInputFile(0);
    debug << "TRestRun::EndOfInit. InputFile pattern: \"" << fInputFileName << "\"" << endl;
    info << "which matches :" << endl;
    for (int i = 0; i < fInputFileNames.size(); i++) {
        info << fInputFileNames[i] << endl;
    }
    essential << "(" << fInputFileNames.size() << " added files)" << endl;
}

///////////////////////////////////////////////
/// \brief Open the i th file in the file list
///
void TRestRun::OpenInputFile(int i) {
    if (fInputFileNames.size() > i) {
        TString Filename = fInputFileNames[i];
        info << "opening... " << Filename << endl;
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
void TRestRun::OpenInputFile(TString filename, const string& mode) {
    CloseFile();
    if (!filename.Contains("http") && !TRestTools::fileExists((string)filename)) {
        ferr << "input file \"" << filename << "\" does not exist!" << endl;
        exit(1);
    }

    if (!filename.Contains("http")) ReadFileInfo((string)filename);

    // add to fInputFileNames in case it is opening a new file
    bool inList = false;
    for (auto addedfilename : fInputFileNames) {
        if (addedfilename == filename) {
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

                // We should not recover the user. Only when writting. If not when I open a file
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
                    warning << "-- W : The metadata version found on input file is lower "
                               "than 2.2.1!"
                            << endl;
                    warning << "-- W : metadata from input file will not be read" << endl;
                }
            }

            debug << "Initializing input file : version code : " << this->GetVersionCode() << endl;
            debug << "Input file version : " << this->GetVersion() << endl;
            ReadInputFileTrees();
            fCurrentEvent = 0;
        } else {
            fAnalysisTree = nullptr;

            // set its analysistree as the first TTree object in the file, if exists
            TIter nextkey(fInputFile->GetListOfKeys());
            TKey* key;
            while ((key = (TKey*)nextkey())) {
                if ((string)key->GetClassName() == "TTree") {
                    fAnalysisTree =
                        TRestAnalysisTree::ConvertFromTTree((TTree*)fInputFile->Get(key->GetName()));
                }
            }
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
        info << "Input file is not REST root file, an external process is needed!" << endl;
}

void TRestRun::AddInputFileExternal(string file) {
    mutex2.lock();
    if (fFileProcess != nullptr) {
        bool add = fFileProcess->AddInputFile(file);
        if (add == false) {
            ferr << "failed to add input file!" << endl;
        }
        fInputFileNames.push_back(file);
    }
    mutex2.unlock();
}

void TRestRun::ReadInputFileMetadata() {
    TFile* f = fInputFile;
    if (f != nullptr) {
        fInputMetadata.clear();

        TIter nextkey(f->GetListOfKeys());
        TKey* key;
        // we should make sure the input metadata has unique names
        set<string> addednames;
        while ((key = (TKey*)nextkey())) {
            debug << "Reading key with name : " << key->GetName() << endl;
            if (addednames.count(key->GetName()) != 0) continue;

            TRestMetadata* a = (TRestMetadata*)f->Get(key->GetName());
            if (!a) {
                ferr << "TRestRun::ReadInputFileMetadata." << endl;
                ferr << "Key name : " << key->GetName() << endl;
                ferr << "Hidden key? Please, report this problem." << endl;
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
                addednames.insert(key->GetName());
            }
        }
    }
}

void TRestRun::ReadInputFileTrees() {
    if (fInputFile != nullptr) {
        debug << "Finding TRestAnalysisTree.." << endl;
        TTree* _eventTree = nullptr;
        string filename = fInputFile->GetName();

        if (fInputFile->Get("AnalysisTree") != nullptr) {
            fAnalysisTree = (TRestAnalysisTree*)fInputFile->Get("AnalysisTree");
            fAnalysisTree->GetEntry(0);  // we call GetEntry() to connect branches

            _eventTree = (TTree*)fInputFile->Get("EventTree");
        } else if (fInputFile->FindKey("TRestAnalysisTree") != nullptr) {
            // This is v2.1.6- version of input file, we directly find EventTree and
            // AnalysisTree. The old name pattern is "TRestXXXEventTree-eventBranch"
            // and "TRestAnalysisTree"
            warning << "Loading root file from old version REST!" << endl;
            fAnalysisTree = (TRestAnalysisTree*)fInputFile->Get("TRestAnalysisTree");
            fAnalysisTree->GetEntry(0);

            TIter nextkey(fInputFile->GetListOfKeys());
            TKey* key;
            while ((key = (TKey*)nextkey())) {
                // cout << key->GetName() << endl;
                if (((string)key->GetName()).find("EventTree") != -1) {
                    _eventTree = (TTree*)fInputFile->Get(key->GetName());
                    string eventname = Replace(key->GetName(), "Tree", "", 0);
                    TBranch* br = _eventTree->GetBranch("eventBranch");
                    br->SetName((eventname + "Branch").c_str());
                    br->SetTitle((eventname + "Branch").c_str());
                    break;
                }
            }
            // if(Tree2!=nullptr)
            //	fAnalysisTree->SetEntries(Tree2->GetEntries());
            debug << "Old REST file successfully recovered!" << endl;
        } else {
            ferr << "(OpenInputFile) : AnalysisTree was not found" << endl;
            ferr << "Inside file : " << filename << endl;
            ferr << "This may be not REST output file!" << endl;
            exit(1);
        }

        if (_eventTree != nullptr) {
            fEventTree = _eventTree;

            debug << "Finding event branch.." << endl;
            if (fInputEvent == nullptr) {
                TObjArray* branches = fEventTree->GetListOfBranches();
                // get the last event branch as input event branch
                if (branches->GetLast() > -1) {
                    TBranch* br = (TBranch*)branches->At(branches->GetLast());

                    if (br == nullptr || Count(br->GetName(), "EventBranch") == 0) {
                        info << "No event branch inside file : " << filename << endl;
                        info << "This file may be a pure analysis file" << endl;
                    } else {
                        string type = Replace(br->GetName(), "Branch", "", 0);
                        fInputEvent = REST_Reflection::Assembly(type);
                        fInputEvent->InitializeWithMetadata(this);
                        fEventTree->SetBranchAddress(br->GetName(), &fInputEvent);
                        fEventBranchLoc = branches->GetLast();
                        debug << "found event branch of event type: " << fInputEvent->ClassName() << endl;
                    }
                }
            } else {
                string brname = (string)fInputEvent->ClassName() + "Branch";
                if (fEventTree->GetBranch(brname.c_str()) == nullptr) {
                    warning << "REST WARNING (OpenInputFile) : No matched event branch "
                               "inside file : "
                            << filename << endl;
                    warning << "Branch required: " << brname << endl;
                } else {
                    fEventTree->SetBranchAddress(brname.c_str(), &fInputEvent);
                    debug << brname << " is found and set!" << endl;
                }
            }
        } else {
            debug << "TRestRun:OpenInputFile. EventTree was not found" << endl;
            debug << "This is a pure analysis file!" << endl;
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
void TRestRun::ReadFileInfo(string filename) {
    debug << "begin collecting basic file info..." << filename << endl;

    // basic file info: modify time, total bytes, etc.
    struct stat buf;
    FILE* fp = fopen(filename.c_str(), "rb");
    if (!fp) {
        ferr << "TRestRun::ReadFileInfo. Something went wrong with fopen()!" << strerror(errno) << endl;
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

    debug << "begin matching file name pattern for more file info..." << endl;
    // format example:
    // run[fRunNumber]_cobo[aaa]_frag[bbb]_Vm[TRestDetector::fAmplificationVoltage].graw
    // we are going to match it with inputfile:
    // run00042_cobo1_frag0000_Vm350.graw
    string format = GetParameter("inputFormat", "");
    string name = TRestTools::SeparatePathAndName(filename).second;

    vector<string> formatsectionlist;
    vector<string> formatprefixlist;

    int pos = -1;
    int pos1 = 0;
    int pos2 = 0;
    while (1) {
        pos1 = format.find("[", pos + 1);
        pos2 = format.find("]", pos1);
        if (pos1 == -1 || pos2 == -1) {
            formatprefixlist.push_back(format.substr(pos + 1, -1));
            break;
        }

        formatsectionlist.push_back(format.substr(pos1 + 1, pos2 - pos1 - 1));
        formatprefixlist.push_back(format.substr(pos + 1, pos1 - pos - 1));

        pos = pos2;
    }

    pos = -1;
    for (int i = 0; i < formatsectionlist.size() && i < formatprefixlist.size() - 1; i++) {
        if (i != 0 && formatprefixlist[i] == "") {
            warning << "file format reference contains error!" << endl;
            return;
        }
        int pos1 = name.find(formatprefixlist[i], pos + 1) + formatprefixlist[i].size();
        if (formatprefixlist[i] == "") pos1 = 0;
        int pos2 = name.find(formatprefixlist[i + 1], pos1);
        if (pos1 == -1 || pos2 == -1) {
            warning << "File pattern matching: file format mismatch!" << endl;
            return;
        }

        string infoFromFileName = name.substr(pos1, pos2 - pos1);

        debug << "File pattern matching. key: " << formatsectionlist[i] << " (between the mark \""
              << formatprefixlist[i] << "\" and \"" << formatprefixlist[i + 1]
              << "\"), value: " << infoFromFileName << endl;

        // run[fRunNumber]_cobo[aaa]_frag[bbb]_Vm[TRestDetector::fAmplificationVoltage].graw
        bool inforead = false;
        if (!inforead) {
            // 1. store special file pattern parameters as TRestRun data member: fRunNumber
            if (DataMemberNameToParameterName(formatsectionlist[i]) != "") {
                RESTValue member = RESTValue(this, this->ClassName()).GetDataMember(formatsectionlist[i]);
                if (!member.IsZombie()) {
                    member.ParseString(infoFromFileName);
                    inforead = true;
                } else {
                    warning << "TRestRun: file name format field \"" << formatsectionlist[i]
                            << "\"(value = " << infoFromFileName
                            << ") not registered, data member does not exist in TRestRun!" << endl;
                }
            }
        }

        if (!inforead) {
            // 2. store special file pattern parameters as data member of the loaded
            // metadata class, e.g., TRestDetector::fAmplificationVoltage
            vector<string> class_datamember = Split(formatsectionlist[i], "::");
            if (class_datamember.size() > 1) {
                TRestMetadata* meta = GetMetadataClass(class_datamember[0]);
                if (meta != nullptr) {
                    RESTValue member = RESTValue(meta, meta->ClassName()).GetDataMember(class_datamember[1]);
                    if (!member.IsZombie()) {
                        member.ParseString(infoFromFileName);
                        meta->UpdateMetadataMembers();
                        inforead = true;
                    } else {
                        warning << "TRestRun: file name format field \"" << formatsectionlist[i]
                                << "\"(value = " << infoFromFileName
                                << ") not registered, metadata exist but without such datamember field!"
                                << endl;
                    }
                } else {
                    warning << "TRestRun: file name format field \"" << formatsectionlist[i]
                            << "\"(value = " << infoFromFileName
                            << ") not registered, metadata does not exist!" << endl;
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
Int_t TRestRun::GetNextEvent(TRestEvent* targetevt, TRestAnalysisTree* targettree) {
    bool messageShown = false;
    TRestEvent* eve = fInputEvent;

    if (fFileProcess != nullptr) {
        debug << "TRestRun: getting next event from external process" << endl;
    GetEventExt:
        mutex2.lock();
        fFileProcess->BeginOfEventProcess();
        eve = fFileProcess->ProcessEvent(nullptr);
        fFileProcess->EndOfEventProcess();
        mutex2.unlock();
        fBytesReaded = fFileProcess->GetTotalBytesReaded();
        if (targettree != nullptr) {
            for (int n = 0; n < fAnalysisTree->GetNumberOfObservables(); n++)
                targettree->SetObservable(n, fAnalysisTree->GetObservable(n));
        }
        fCurrentEvent++;
    } else {
        debug << "TRestRun: getting next event from root file" << endl;
        if (fAnalysisTree != nullptr) {
            if (fCurrentEvent >= fAnalysisTree->GetEntriesFast()) {
                eve = nullptr;
            } else {
                eve->Initialize();
                fBytesReaded += fAnalysisTree->GetEntry(fCurrentEvent);
                if (targettree != nullptr) {
                    for (int n = 0; n < fAnalysisTree->GetNumberOfObservables(); n++)
                        targettree->SetObservable(n, fAnalysisTree->GetObservable(n));
                }
                if (fEventTree != nullptr) {
                    fBytesReaded += ((TBranch*)fEventTree->GetListOfBranches()->UncheckedAt(fEventBranchLoc))
                                        ->GetEntry(fCurrentEvent);
                    // fBytesReaded += fEventTree->GetEntry(fCurrentEvent);
                }
                fCurrentEvent++;
            }
        }

        else {
            warning << "error to get event from input file, missing file process or "
                       "analysis tree"
                    << endl;
            eve = nullptr;
        }
    }

    // cout << fHangUpEndFile << endl;

    if (eve == nullptr) {
        if (fHangUpEndFile && fFileProcess != nullptr) {
            // if hangup is set, we continue calling ProcessEvent() of the
            // external process, until there is non-null event yielded
            if (!messageShown) {
                essential << "external process file reading reaches end, waiting for more files" << endl;
            }
            sleep(1);
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

    targetevt->Initialize();
    fInputEvent->CloneTo(targetevt);

    return 0;
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

    debug << "TRestRun::FormFormat. In string : " << inString << endl;

    int pos = 0;
    while (1) {
        int pos1 = inString.find("[", pos);
        int pos2 = inString.find("]", pos1);
        if (pos1 == -1 || pos2 == -1) break;

        string targetstr = inString.substr(pos1, pos2 - pos1 + 1);   // with []
        string target = inString.substr(pos1 + 1, pos2 - pos1 - 1);  // without []
        string replacestr = GetRunInformation(target);

        debug << "TRestRun::FormFormat. target : " << target << endl;
        debug << "TRestRun::FormFormat. replacestr : " << replacestr << endl;

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
    debug << "TRestRun::FormOutputFile. target : " << outputfilename << endl;
    string filename;
    auto m = new TFileMerger(false);
    if (outputfilename == "") {
        filename = fOutputFileName;
        info << "Creating file : " << filename << endl;
        m->OutputFile(filename.c_str(), "RECREATE");
    } else {
        filename = outputfilename;
        info << "Creating file : " << filename << endl;
        m->OutputFile(filename.c_str(), "UPDATE");
    }

    debug << "TRestRun::FormOutputFile. Starting to add files" << endl;

    for (int i = 0; i < filenames.size(); i++) {
        m->AddFile(filenames[i].c_str(), false);
    }

    if (m->Merge()) {
        for (int i = 0; i < filenames.size(); i++) {
            remove(filenames[i].c_str());
        }
    } else {
        fOutputFileName = "";
        ferr << "(Merge files) failed to merge process files." << endl;
        exit(1);
    }

    delete m;

    // we rename the created output file
    fOutputFileName = FormFormat(filename);
    rename(filename.c_str(), fOutputFileName);

    // write metadata into the output file
    fOutputFile = new TFile(fOutputFileName, "update");
    debug << "TRestRun::FormOutputFile. Calling WriteWithDataBase()" << endl;
    this->WriteWithDataBase();

    fout << this->ClassName() << " Created ..." << endl;
    fout << "- Path : " << TRestTools::SeparatePathAndName((string)fOutputFileName).first << endl;
    fout << "- Filename : " << TRestTools::SeparatePathAndName((string)fOutputFileName).second << endl;
    return fOutputFile;
}

///////////////////////////////////////////////
/// \brief Create a new TFile as REST output file. Writing metadata objects into
/// it.
///
TFile* TRestRun::FormOutputFile() {
    CloseFile();
    fOutputFileName = FormFormat(fOutputFileName);
    fOutputFile = new TFile(fOutputFileName, "recreate");
    fAnalysisTree = new TRestAnalysisTree("AnalysisTree", "AnalysisTree");
    fEventTree = new TTree("EventTree", "EventTree");
    // fAnalysisTree->CreateBranches();
    // fEventTree->CreateEventBranches();
    fAnalysisTree->Write();
    fEventTree->Write();
    this->WriteWithDataBase();

    fout << "TRestRun: Output File Created." << endl;
    fout << "- Path : " << TRestTools::SeparatePathAndName((string)fOutputFileName).first << endl;
    fout << "- Filename : " << TRestTools::SeparatePathAndName((string)fOutputFileName).second << endl;
    return fOutputFile;
}

TFile* TRestRun::UpdateOutputFile() {
    if (fOutputFile != nullptr) {
        if (fOutputFile->IsOpen()) {
            fOutputFile->ReOpen("update");
        }

        fOutputFile->cd();
        fAnalysisTree->Write(0, kWriteDelete);
        fEventTree->Write(0, kWriteDelete);
        this->WriteWithDataBase();

        fout << "TRestRun: Output File Updated." << endl;
        fout << "- Path : " << TRestTools::SeparatePathAndName((string)fOutputFileName).first << endl;
        fout << "- Filename : " << TRestTools::SeparatePathAndName((string)fOutputFileName).second << endl;
        return fOutputFile;

    } else {
        ferr << "TRestRun::UpdateOutputFile(): output file is closed" << endl;
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

    debug << "TRestRun::WriteWithDataBase. Getting entries in analysisTree" << endl;

    // record the entries saved
    fEntriesSaved = -1;
    if (fOutputFile != nullptr) {
        tree = (TRestAnalysisTree*)fOutputFile->Get("AnalysisTree");
        if (tree != nullptr) {
            fEntriesSaved = tree->GetEntries();
        }
    }
    debug << "TRestRun::WriteWithDataBase. Entries found : " << fEntriesSaved << endl;

    // If time was not written by any other process we record the current time
    if (fEndTime == 0) {
        time_t timev;
        time(&timev);
        fEndTime = (Double_t)timev;
    }

    fRunUser = REST_USER;

    // save metadata objects in file
    debug << "TRestRun::WriteWithDataBase. Calling this->Write(0,kWriteDelete)" << endl;
    this->Write(0, kWriteDelete);
    debug << "TRestRun::WriteWithDataBase. Succeed" << endl;
    debug << "TRestRun::WriteWithDataBase. fMetadata.size() == " << fMetadata.size() << endl;
    for (int i = 0; i < fMetadata.size(); i++) {
        bool historic = false;
        debug << "TRestRun::WriteWithDataBase. fInputMetadata.size() == " << fInputMetadata.size() << endl;
        for (int j = 0; j < fInputMetadata.size(); j++) {
            debug << fMetadata[i]->GetName() << " == " << fInputMetadata[j]->GetName() << endl;
            if (fMetadata[i] == fInputMetadata[j]) {
                historic = true;
                break;
            }
        }

        if (!historic) {
            debug << "NO historic" << endl;
            fMetadata[i]->Write(fMetadata[i]->GetName(), kWriteDelete);
        } else {
            debug << "IS historic" << endl;
            if (fSaveHistoricData) fMetadata[i]->Write(fMetadata[i]->GetName(), kWriteDelete);
        }
    }

    // write to database
    debug << "TResRun::WriteWithDataBase. Run number is : " << fRunNumber << endl;
    if (fRunNumber != -1) {
        int fileid = gDataBase->set_runfile(fRunNumber, (string)fOutputFileName);
        fout << "DataBase Entry Added! Run Number: " << fRunNumber << ", File ID: " << fileid << endl;
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
                fAnalysisTree->Write(0, kWriteDelete);
                this->Write(0, kWriteDelete);
            }
        }
        delete fAnalysisTree;
        fAnalysisTree = nullptr;
    }

    if (fEventTree != nullptr) {
        if (fEventTree->GetEntries() > 0 && fInputFile == nullptr) fEventTree->Write(0, kWriteDelete);
        delete fEventTree;
        fEventTree = nullptr;
    }

    for (int i = 0; i < fMetadata.size(); i++) {
        for (int j = 0; j < fInputMetadata.size(); j++) {
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
            ferr << "The external process \"" << p->GetName() << "\" doesn't yield any output event!" << endl;
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

        GetNextEvent(fInputEvent, 0);
        // fAnalysisTree->CreateBranches();
        info << "The external file process has been set! Name : " << fFileProcess->GetName() << endl;
    } else {
        if (fFileProcess != nullptr) {
            ferr << "There can only be one file process!" << endl;
            exit(1);
        }
        if (p == nullptr) {
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
void TRestRun::SetInputEvent(TRestEvent* eve) {
    if (eve != nullptr) {
        if (fEventTree != nullptr) {
            // if (fEventBranchLoc != -1) {
            //	TBranch *br = (TBranch*)branches->At(fEventBranchLoc);
            //	br->SetAddress(0);
            //}
            if (fInputEvent != nullptr) {
                fEventTree->SetBranchAddress((TString)fInputEvent->ClassName() + "Branch", 0);
                fEventTree->SetBranchStatus((TString)fInputEvent->ClassName() + "Branch", false);
            }
            TObjArray* branches = fEventTree->GetListOfBranches();
            string brname = (string)eve->ClassName() + "Branch";
            for (int i = 0; i <= branches->GetLast(); i++) {
                TBranch* br = (TBranch*)branches->At(i);
                if ((string)br->GetName() == brname) {
                    debug << "Setting input event.. Type: " << eve->ClassName() << " Address: " << eve
                          << endl;
                    // if (fInputEvent != nullptr && (char*)fInputEvent != (char*)eve) {
                    //    delete fInputEvent;
                    //}

                    fInputEvent = eve;
                    fEventTree->SetBranchAddress(brname.c_str(), &fInputEvent);
                    fEventTree->SetBranchStatus(brname.c_str(), false);
                    fEventBranchLoc = i;
                    break;
                } else if (i == branches->GetLast()) {
                    warning << "REST Warning : (TRestRun) cannot find corresponding "
                               "branch in event tree!"
                            << endl;
                    warning << "Event Type : " << eve->ClassName() << endl;
                    warning << "Input event not set!" << endl;
                }
            }
        } else {
            fInputEvent = eve;
        }
    }
}

///////////////////////////////////////////////
/// \brief Add an event branch in output EventTree
///
void TRestRun::AddEventBranch(TRestEvent* eve) {
    if (eve != nullptr) {
        if (fEventTree != nullptr) {
            string evename = (string)eve->ClassName();
            string brname = evename + "Branch";
            fEventTree->Branch(brname.c_str(), eve);
            fEventTree->SetTitle((evename + "Tree").c_str());
        }
    }
}

///////////////////////////////////////////////
/// \brief Open the root file and import the metadata of the given name.
///
/// The metadata class can be recovered to the same condition as when it is
/// saved.
void TRestRun::ImportMetadata(const TString& File, const TString& name, const TString& type, Bool_t store) {
    auto fileold = File;
    File = SearchFile(File.Data());
    if (File == "") {
        ferr << "(ImportMetadata): The file " << fileold << " does not exist!" << endl;
        ferr << endl;
        return;
    }
    if (!TRestTools::isRootFile(File.Data())) {
        ferr << "(ImportMetadata) : The file " << File << " is not root file!" << endl;
        ferr << "If you want to initialize metadata from rml file, use <TRest section!" << endl;
        return;
    }

    auto f = new TFile(File);
    // TODO give error in case we try to obtain a class that is not TRestMetadata
    if (type == "" && name == "") {
        ferr << "(ImportMetadata) : metadata type and name is not "
                "specified!"
             << endl;
        return;
    }

    TRestMetadata* meta;
    if (name != "") {
        meta = GetMetadata(name, f);
    } else if (type != "") {
        meta = GetMetadataClass(type, f);
    }

    if (meta == nullptr) {
        cout << "REST ERROR (ImportMetadata) : " << name << " does not exist." << endl;
        cout << "Inside root file : " << File << endl;
        GetChar();
        f->Close();
        delete f;
        return;
    }

    if (store)
        meta->Store();
    else
        meta->DoNotStore();

    fMetadata.push_back(meta);
    meta->LoadConfigFromBuffer();
    f->Close();
    delete f;
}

Int_t TRestRun::Write(const char* name, Int_t option, Int_t bufsize) {
    ReSetVersion();
    return TRestMetadata::Write(name, option, bufsize);
}

Double_t TRestRun::GetRunLength() {
    if (fEndTime - fStartTime == -1) cout << "Run time is not set" << endl;
    return fEndTime - fStartTime;
}

Long64_t TRestRun::GetTotalBytes() {
    if (fFileProcess != nullptr) {
        fTotalBytes = fFileProcess->GetTotalBytes();
    }
    return fTotalBytes;
}

int TRestRun::GetEntries() {
    if (fAnalysisTree != nullptr) {
        return fAnalysisTree->GetEntries();
    }
    return REST_MAXIMUM_EVENTS;
}

// Getters
TRestEvent* TRestRun::GetEventWithID(Int_t eventID, Int_t subEventID, const TString& tag) {
    if (fAnalysisTree != nullptr) {
        int nentries = fAnalysisTree->GetEntries();

        // set analysis tree to read only three branches
        fAnalysisTree->SetBranchStatus("*", false);
        fAnalysisTree->SetBranchStatus("eventID", true);
        fAnalysisTree->SetBranchStatus("subEventID", true);
        fAnalysisTree->SetBranchStatus("subEventTag", true);

        // just look through the whole analysis tree and find the entry
        // this is not good!
        for (int i = 0; i < nentries; i++) {
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

std::vector<int> TRestRun::GetEventEntriesWithConditions(const string cuts, int startingIndex,
                                                         int maxNumber) {
    std::vector<int> eventIds;
    // parsing cuts
    std::vector<string> observables;
    std::vector<string> operators;
    std::vector<Double_t> values;
    // it is necessary that this vector vector is sorted from longest to shortest
    const std::vector<string> validOperators = {"==", "<=", ">=", "=", ">", "<"};

    vector<string> cutsVector = Split(cuts, "&&", false, true);

    for (int i = 0; i < cutsVector.size(); i++) {
        string cut = cutsVector[i];
        for (int j = 0; j < validOperators.size(); j++) {
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
    for (int i = 0; i < observables.size(); i++) {
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
    for (int i = 0; i < observables.size(); i++) {
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
        for (int j = 0; j < observables.size(); j++) {
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
            if (eventIds.size() < maxNumber) {
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

std::vector<int> TRestRun::GetEventIdsWithConditions(const string cuts, int startingIndex, int maxNumber) {
    auto indices = GetEventEntriesWithConditions(cuts, startingIndex, maxNumber);
    std::vector<int> ids;
    for (int i = 0; i < indices.size(); i++) {
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
TRestEvent* TRestRun::GetNextEventWithConditions(const string cuts) {
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

string TRestRun::GetRunInformation(string infoname) {
    string result = GetParameter(infoname, "");
    if (result != "") {
        return result;
    }

    result = GetDataMemberValue(infoname);
    if (result != "") {
        return result;
    }

    result = GetDataMemberValue(ParameterNameToDataMemberName(infoname));
    if (result != "") {
        return result;
    }

    if (fHostmgr && fHostmgr->GetProcessRunner() != nullptr) {
        result = fHostmgr->GetProcessRunner()->GetProcInfo(infoname);
        if (result != "") {
            return result;
        }
    }

    return infoname;
}

TRestMetadata* TRestRun::GetMetadataClass(TString type, TFile* f) {
    if (f != nullptr) {
        TIter nextkey(f->GetListOfKeys());
        TKey* key;
        while ((key = (TKey*)nextkey())) {
            string kName = key->GetClassName();

            if (REST_Reflection::GetClassQuick(kName.c_str()) != nullptr &&
                REST_Reflection::GetClassQuick(kName.c_str())->InheritsFrom(type)) {
                TRestMetadata* a = (TRestMetadata*)f->Get(key->GetName());

                if (a != nullptr && a->InheritsFrom("TRestMetadata")) {
                    return a;
                } else {
                    warning << "TRestRun::GetMetadataClass() : The object to import is "
                               "not inherited from TRestMetadata"
                            << endl;
                }
            }
        }
    } else {
        for (int i = 0; i < fMetadata.size(); i++)
            if (fMetadata[i]->InheritsFrom(type)) return fMetadata[i];

        if (fInputFile != nullptr && this->GetVersionCode() >= TRestTools::ConvertVersionCode("2.2.1")) {
            return GetMetadataClass(type, fInputFile);
        }
    }

    return nullptr;
}

TRestMetadata* TRestRun::GetMetadata(const TString& name, TFile* f) {
    if (f != nullptr) {
        TIter nextkey(f->GetListOfKeys());
        TKey* key;
        while ((key = (TKey*)nextkey())) {
            string kName = key->GetName();

            if (kName == name) {
                TRestMetadata* a = (TRestMetadata*)f->Get(name);

                if (a->InheritsFrom("TRestMetadata")) {
                    return a;
                } else {
                    warning << "TRestRun::GetMetadata() : The object to import is not "
                               "inherited from TRestMetadata"
                            << endl;
                }
            }
        }
    } else {
        for (unsigned int i = 0; i < fMetadata.size(); i++)
            if (fMetadata[i]->GetName() == name) return fMetadata[i];

        // if (fInputFile != nullptr && this->GetVersionCode() >=
        // ConvertVersionCode("2.2.1")) { 	return GetMetadata(name, fInputFile);
        //}
    }

    return nullptr;
}

std::vector<std::string> TRestRun::GetMetadataStructureNames() {
    std::vector<std::string> strings;

    for (int n = 0; n < GetNumberOfMetadataStructures(); n++) strings.push_back(fMetadata[n]->GetName());

    return strings;
}

std::vector<std::string> TRestRun::GetMetadataStructureTitles() {
    std::vector<std::string> strings;

    for (int n = 0; n < GetNumberOfMetadataStructures(); n++) strings.push_back(fMetadata[n]->GetTitle());

    return strings;
}

///////////////////////////////////////////////
/// \brief It will replace the data members contained inside the string given as input. The data members in
/// the input string should be written using the following format [MetadataClass::fDataMember].
///
/// \return The string with data members replaced
///
string TRestRun::ReplaceMetadataMembers(const string instr) {
    if (instr.find("[", 0) == (int)string::npos) return instr;
    string outstring = instr;

    int startPosition = 0;
    int endPosition = 0;
    while ((startPosition = outstring.find("[", endPosition)) != (int)string::npos) {
        endPosition = outstring.find("]", startPosition + 1);
        string s = outstring.substr(startPosition + 1, endPosition - startPosition - 1);
        int cont = count(s.begin(), s.end(), '[') - count(s.begin(), s.end(), ']');

        if (cont < 0) ferr << "This is a coding error at ReplaceMetadataMembers!" << endl;

        // We search for the enclosing ]. Since we might find a vector index inside.
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

    outstring = Replace(outstring, "<<", "[");
    outstring = Replace(outstring, ">>", "]");

    return outstring;
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
string TRestRun::ReplaceMetadataMember(const string instr) {
    if (instr.find("::") == string::npos && instr.find("->") == string::npos) {
        return "<<" + instr + ">>";
    }

    vector<string> results = Split(instr, "::", false, true);
    if (results.size() == 1) results = Split(instr, "->", false, true);

    if (results.size() == 2) {
        Int_t index = 0;
        int pos1 = results[1].find("[", 0);
        int pos2 = results[1].find("]", pos1);
        if (pos1 > 0 && pos2 > 0) {
            string indexStr = results[1].substr(pos1 + 1, pos2 - pos1 - 1);  // without []

            index = StringToInteger(indexStr);
            if (index < 0) index = 0;

            results[1] = results[1].substr(0, pos1);
        }

        if (GetMetadata(results[0])) {
            if (index >= this->GetMetadata(results[0])->GetDataMemberValues(results[1]).size()) {
                warning << "TRestRun::ReplaceMetadataMember. Index out of range!" << endl;
                warning << "Returning the first element" << endl;
                index = 0;
            }
            return this->GetMetadata(results[0])->GetDataMemberValues(results[1])[index];
        }

        if (GetMetadataClass(results[0])) {
            if (index >= this->GetMetadataClass(results[0])->GetDataMemberValues(results[1]).size()) {
                warning << "TRestRun::ReplaceMetadataMember. Index out of range!" << endl;
                warning << "Returning the first element" << endl;
                index = 0;
            }
            return this->GetMetadataClass(results[0])->GetDataMemberValues(results[1])[index];
        }

    } else
        ferr << "TRestRun::ReplaceMetadataMember. Wrong number of elements found" << endl;

    warning << "TRestRun::ReplaceMetadataMember. " << instr << " not found!" << endl;
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
Bool_t TRestRun::EvaluateMetadataMember(const string instr) {
    if (instr == "") return true;

    std::vector<string> oper = {"=", "==", "<=", "<", ">=", ">", "!="};

    string expOp = "";
    std::vector<string> results;
    for (int n = 0; n < oper.size(); n++) {
        size_t pos = 0;
        if (instr.find("->") != string::npos) pos = instr.find("->") + 2;

        if (instr.find(oper[n], pos) != string::npos) {
            expOp = oper[n];
            results = Split(instr, oper[n], false, true, pos);
            break;
        }
    }

    if (expOp == "") {
        warning << "TRestRun::EvaluateMetadataMember. Not valid operator found in expression : " << instr
                << endl;
        return false;
    }

    if (results.size() != 2) {
        warning << "TRestRun::EvaluateMetadataMember. Not valid expression : " << instr << endl;
        return false;
    }

    if (!isANumber(results[1])) {
        if (ReplaceMetadataMember(results[0]) == results[1])
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

    metadata << "Version : " << this->GetVersion() << endl;
    metadata << "Parent run number : " << GetParentRunNumber() << endl;
    metadata << "Run number : " << GetRunNumber() << endl;
    metadata << "Experiment/project : " << GetExperimentName() << endl;
    metadata << "Run type : " << GetRunType() << endl;
    metadata << "Run tag : " << GetRunTag() << endl;
    metadata << "Run user : " << GetRunUser() << endl;
    metadata << "Run description : " << GetRunDescription() << endl;
    metadata << "Start Date/Time : " << ToDateTimeString(GetStartTimestamp()) << " (" << GetStartTimestamp()
             << ")" << endl;
    metadata << "End Date/Time : " << ToDateTimeString(GetEndTimestamp()) << " (" << GetEndTimestamp() << ")"
             << endl;
    metadata << "Input file : " << TRestTools::GetPureFileName((string)GetInputFileNamepattern()) << endl;
    metadata << "Output file : " << TRestTools::GetPureFileName((string)GetOutputFileName()) << endl;
    metadata << "Number of events : " << fEntriesSaved << endl;
    // metadata << "Input filename : " << fInputFilename << endl;
    // metadata << "Output filename : " << fOutputFilename << endl;
    // metadata << "Number of initial events : " << GetNumberOfEvents() << endl;
    // metadata << "Number of processed events : " << fProcessedEvents << endl;
    metadata << "---------------------------------------" << endl;
    metadata << endl;
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

///////////////////////////////////////////////
/// \brief Prints the run end date and time in human format
///
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
    for (int n = 0; n < fMetadata.size(); n++)
        if (fMetadata[n]->GetError()) nErrors++;

    if (nErrors) {
        cout << endl;
        ferr << "Found a total of " << nErrors << " metadata errors" << endl;
        for (int n = 0; n < fMetadata.size(); n++)
            if (fMetadata[n]->GetError()) {
                cout << endl;
                ferr << "Class: " << fMetadata[n]->ClassName() << " Name: " << fMetadata[n]->GetName()
                     << endl;
                ferr << "Number of errors: " << fMetadata[n]->GetNumberOfErrors() << endl;
                ferr << "Message: " << fMetadata[n]->GetErrorMessage() << endl;
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
    for (int n = 0; n < fMetadata.size(); n++)
        if (fMetadata[n]->GetWarning()) nWarnings++;

    if (nWarnings) {
        cout << endl;
        warning << "Found a total of " << nWarnings << " metadata warnings" << endl;
        for (int n = 0; n < fMetadata.size(); n++)
            if (fMetadata[n]->GetWarning()) {
                cout << endl;
                warning << "Class: " << fMetadata[n]->ClassName() << " Name: " << fMetadata[n]->GetName()
                        << endl;
                warning << "Number of warnings: " << fMetadata[n]->GetNumberOfWarnings() << endl;
                warning << "Message: " << fMetadata[n]->GetWarningMessage() << endl;
            }
        cout << endl;
    } else {
        cout << "No warnings found!" << endl;
    }
}
