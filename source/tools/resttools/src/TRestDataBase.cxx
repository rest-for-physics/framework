#include "TRestDataBase.h"

#include <errno.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "TClass.h"
#include "TRestStringHelper.h"
#include "TRestStringOutput.h"
#include "TRestTools.h"
#include "TSystem.h"
#include "TUrl.h"

//////////////////////////////////////////////////////////////////////////
/// Interface class of REST database accessibility.
///
/// This class defines methods only. its inhert class will do the database work.
///
/// To startup, first install the package: restDataBaseImpl. It requires pgsql
/// database installed. Call TRestDataBase::instantiate() to get the functional
/// instance of TRestDataBase. If the package is not installed, a null ptr will
/// be returned.
///
/// Information stored in database:
/// * (run) id: The index of the data line. During datataking/simulation, run id
/// marks a continuous operation which is under constant configuration. For
/// metadata database, id is just an unique mark of the entry.
/// * user: User name of this datataking run. Generated automatically.
/// * version: REST version of this run. Generated automatically.
/// * tag: A tag which makes the run easier to be found. We need to set it
/// manually.
/// * type: Enumerator of run type. Includes 'SW_DEBUG', 'HW_DEBUG',
/// 'SIMULATION', 'CALIBRATION', 'PHYSICS_DBD', 'ANALYSIS' and 'OTHER'. By
/// default data taking has type: HW_DEBUG, restG4 has type: SIMULATION,
/// restManager has type: ANALYSIS.
/// * description: Detailed description string. Write the run config here.
/// * file(s): The associated file(s) of this data taking run or metadata entry.
///
/// How to use:
/// `TRestDataBase* db = TRestDataBase::instantiate()`
///
/// 1. To start a new run and add files in it:
/// \code
/// int runId = db->add_run()
/// int fileId=db->add_runfile(runId,"abc.graw")
/// \endcode
///
/// 2. To view the run info:
/// \code
/// int runId = db->get_lastrun();
/// db->print(runId);
/// \endcode
///

TRestDataBase* RestDataBase_Instance = NULL;
TRestDataBase* TRestDataBase::GetDataBase() { return RestDataBase_Instance; }

TRestDataBase* TRestDataBase::instantiate(string name) {
    // vector<string> list = TRestTools::GetListOfRESTLibraries();
    // for (unsigned int n = 0; n < list.size(); n++) {
    //    gSystem->Load(list[n].c_str());
    //}
    TRestDataBase* db = NULL;
    if (name != "") {
        TClass* c = TClass::GetClass(("TRestDataBase" + name).c_str());
        if (c != NULL)  // this means we have the package installed
        {
            db = (TRestDataBase*)c->New();
        } else {
            cout << "warning! unrecognized TRestDataBase implementation: \"" << name << "\"" << endl;
            db = new TRestDataBase();
        }
    } else {
        db = new TRestDataBase();
    }
    db->Initialize();
    if (RestDataBase_Instance != NULL) delete RestDataBase_Instance;
    RestDataBase_Instance = db;
    return db;
}

DBEntry::DBEntry(vector<string> items) {
    this->runNr = atoi(items[0].c_str());
    this->type = items[1];
    this->tag = items[2];
    this->description = items[3];
    this->version = items[4];
}

void TRestDataBase::Initialize() {
    fMetaDataValues.clear();
    string metaFilename = REST_PATH + (string) "/dataURL";
    if (!TRestTools::fileExists(metaFilename)) {
        return;
    } else {
        vector<int> result;
        ifstream infile(metaFilename);
        string s;
        while (TRestTools::GetLine(infile, s)) {
            vector<string> items = Split(s, "\" \"", true);
            if (items.size() != 6) continue;
            if (items[0][0] == '\"') items[0] = items[0].substr(1, -1);
            if (items[5][items[5].size() - 1] == '\"') items[5] = items[5].substr(0, items[5].size() - 1);

            DBEntry info(items);
            fMetaDataValues.push_back({info, items[5]});
        }
    }
}

DBFile::DBFile(string _filename) {
    filename = _filename;
    auto _fullname = TRestTools::ToAbsoluteName(filename);

    struct stat buf;
    int result = stat(_fullname.c_str(), &buf);

    if (result != 0) {
        cout << "Failed to load file \"" << _fullname << "\"!" << endl;
        fileSize = 0;
        evtRate = 0;
        quality = true;
        start = 0;
        stop = 0;
        for (int i = 0; i < 41; i++) {
            sha1sum[i] = 0;
        }
    } else {
        fileSize = buf.st_size;
        evtRate = 0;
        quality = true;
        start = buf.st_ctime;
        stop = buf.st_mtime;
        string sha1result = TRestTools::Execute("sha1sum " + _fullname);
        string sha1 = Split(sha1result, " ")[0];
        if (sha1.size() == 40) {
            for (int i = 0; i < 40; i++) {
                sha1sum[i] = sha1[i];
            }
            sha1sum[40] = 0;
        }
    }
}

void DBFile::Print() {
    cout << "----DBFile struct----" << endl;
    cout << "size: " << fileSize << endl;
    cout << "event rate: " << evtRate << endl;
    cout << "sha1sum: " << sha1sum << endl;
    cout << "quality: " << quality << endl;
    cout << "start time: " << ToDateTimeString(start) << endl;
    cout << "stop time: " << ToDateTimeString(stop) << endl;
}

TRestDataBase::TRestDataBase() {
    auto url = getenv("REST_DBURL");
    if (url != NULL) {
        fConnectionString = url;
    }
}

///////////////////////////////////////////////
/// \brief get the latest run in database
///
/// Get the latest run in database, by reading the file: $REST_PATH/runNumber.
/// Note that this file saves run number of the **next** run. So it returns
/// The run number -1.
int TRestDataBase::get_lastrun() {
    int runNr;
    string runFilename = REST_PATH + "/runNumber";
    if (!TRestTools::fileExists(runFilename)) {
        if (TRestTools::isPathWritable(REST_PATH)) {
            // we fix the "runNumber" file
            TRestTools::Execute("echo 1 > " + runFilename);
            runNr = 1;
        }
    } else {
        ifstream ifs(runFilename);
        ifs >> runNr;
    }
    // the number recorded in "runNumber" file is for the next run, we subtract 1 to get the latest run.
    return runNr - 1;
}

///////////////////////////////////////////////
/// \brief add a new run, with run info as struct DBEntry. returns the added run id
///
/// runs are added according to info.id of input DBEntry object:
/// -1 --> do not add
/// 0  --> append a new run in run list
/// >0 --> directly use this run number
///
/// It will write to the file: $REST_PATH/runNumber is writable. The number written will be
/// the **next** run number
///
/// The method in derived class shall follow this rule.
int TRestDataBase::add_run(DBEntry info) {
    int newRunNr;
    if (info.runNr == 0) {
        newRunNr = get_lastrun() + 1;
    } else if (info.runNr > 0) {
        newRunNr = info.runNr;
    } else {
        return -1;
    }

    string runFilename = REST_PATH + "/runNumber";
    if (TRestTools::isPathWritable(REST_PATH)) {
        TRestTools::Execute("echo " + ToString(newRunNr + 1) + " > " + runFilename);
    } else {
        cout << "REST WARNING: runNumber file not writable. auto run number "
                "increment is disabled"
             << endl;
    }

    set_run_info(newRunNr, info);

    return newRunNr;
}

int TRestDataBase::query_metadata(int id) {
    if (fMetaDataValues.size() > id) return id;
    return -1;
}

string TRestDataBase::query_metadata_value(int id) { return fMetaDataValues[id].second; }

DBEntry TRestDataBase::query_metadata_info(int id) { return fMetaDataValues[id].first; }

vector<int> TRestDataBase::search_metadata_with_value(string _val) {
    vector<int> result;
    for (int i = 0; i < fMetaDataValues.size(); i++) {
        DBEntry info = fMetaDataValues[i].first;
        string val = fMetaDataValues[i].second;
        if (val.find(_val) != -1) {
            result.push_back(i);
        }
    }
    return result;
}

///////////////////////////////////////////////
/// The following specification of DBEntry's content means to match **any**:
/// id <= 0, type == "" ,usr == "" ,tag == "" ,description == "" ,version == "".
/// If all of them mean **any**, it will return a blank list.
vector<int> TRestDataBase::search_metadata_with_info(DBEntry _info) {
    vector<int> result;
    if (_info.runNr <= 0 && _info.type == "" && _info.tag == "" && _info.description == "" &&
        _info.version == "")
        return result;

    auto iter = fMetaDataValues.begin();
    for (int i = 0; i < fMetaDataValues.size(); i++) {
        DBEntry info = fMetaDataValues[i].first;
        string val = fMetaDataValues[i].second;
        if (_info.runNr > 0 && info.runNr == _info.runNr) {
            // match runNr only
            result.push_back(info.runNr);
        } else if (_info.runNr <= 0 || (_info.runNr > 0 && info.runNr == 0)) {
            // we match other items instead of runNr
            bool typematch = (_info.type == "" || info.type == _info.type);
            bool tagmatch = (_info.tag == "" || info.tag == _info.tag);
            bool descriptionmatch = (_info.description == "" || info.description == _info.description);
            bool versionmatch = (_info.version == "" || info.version == _info.version);

            if (typematch && tagmatch && descriptionmatch && versionmatch) {
                result.push_back(i);
            }
        }
        iter++;
    }
    return result;
}

///////////////////////////////////////////////
/// In base class of database, we suppose the value of metadata entry is a file url.
/// So we directly download them, to the local directory $REST_PATH/data/download/.
/// If the "name" is given, it will replace the file name from metadata value
string TRestDataBase::query_metadata_valuefile(int id, string name) {
    string url = query_metadata_value(id);

    if (name != "") {
        int pos = url.find_last_of('/', -1);
        url.replace(url.begin() + pos + 1, url.end(), name);
    }

    string purename = TRestTools::GetPureFileName(url);
    if (purename == "") {
        cout << "error! entry " << id << " in the database is recorded as a file path" << endl;
        cout << "please specify a concrete file name in this path" << endl;
        cout << "url: " << url << endl;
        return "";
    }

    if (url.find("local:") == 0) {
        return Replace(url, "local:", "");
    } else {
        string fullpath;
        if (TRestTools::isPathWritable(REST_PATH)) {
            fullpath = REST_PATH + "/data/download/" + purename;

        } else {
            fullpath = "/tmp/REST_" + REST_USER + "_Download_" + purename;
        }

        if (TRestTools::DownloadRemoteFile(url, fullpath) == 0) {
            return fullpath;
        } else if(TRestTools::fileExists(fullpath)){
            return fullpath;
        } else {
            return "";
        }
    }

    return "";
}

int TRestDataBase::get_lastmetadata() { return fMetaDataValues.size() - 1; }

int TRestDataBase::add_metadata(DBEntry info, string url) {
    if (TRestTools::isPathWritable(REST_PATH)) {
        cout << "error! path not writable" << endl;
        return -1;
    }

    string metaFilename = REST_PATH + "/dataURL";

    if (info.runNr == 0) {
        info.runNr = get_lastmetadata();
    }
    fMetaDataValues.push_back({info, url});

    std::ofstream file(metaFilename, std::ios::app);
    file << "\"" << info.runNr << "\" ";
    file << "\"" << info.type << "\" ";
    file << "\"" << info.tag << "\" ";
    file << "\"" << info.description << "\" ";
    file << "\"" << info.version << "\" ";
    file << "\"" << url << "\" ";
    file << endl;
    file.close();

    return info.runNr;
}

///////////////////////////////////////////////
/// The following specification of DBEntry's content will not be updated:
/// id <= 0, type == "" ,usr == "" ,tag == "" ,description == "" ,version == "".
int TRestDataBase::update_metadata(int id, DBEntry info) {
    cout << "error! not implemented" << endl;
    return 0;
}
