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
            warning << "unrecognized TRestDataBase implementation: \"" << name << "\"" << endl;
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
    fDataEntries.clear();
    string metaFilename = REST_USER_PATH + (string) "/dataURL";
    if (!TRestTools::fileExists(metaFilename)) {
        return;
    } else {
        vector<int> result;
        ifstream infile(metaFilename);
        string s;
        while (TRestTools::GetLine(infile, s)) {
            DBEntry info;
            vector<string> items = Split(s, "\t", true);
            if (items.size() <= 2) continue;
            for (auto item : items) {
                vector<string> pair = Split(item, "=", true);
                if (pair.size() == 2) {
                    if (pair[0] == "run")
                        info.runNr = atoi(pair[1].c_str());
                    else if (pair[0] == "type")
                        info.type = pair[1];
                    else if (pair[0] == "tag")
                        info.tag = pair[1];
                    else if (pair[0] == "description")
                        info.description = pair[1];
                    else if (pair[0] == "version")
                        info.version = pair[1];
                    else if (pair[0] == "value")
                        info.value = pair[1];
                }
            }

            fDataEntries.push_back(info);
        }
    }
}

DBFile DBFile::ParseFile(string _filename) {
    DBFile file;
    file.filename = _filename;
    auto _fullname = TRestTools::ToAbsoluteName(_filename);

    struct stat buf;
    int result = stat(_fullname.c_str(), &buf);

    if (result != 0) {
        ferr << "DBFile::ParseFile: Failed to load file \"" << _fullname << "\"!" << endl;
    } else {
        file.fileSize = buf.st_size;
        file.evtRate = 0;
        file.quality = true;
        file.start = buf.st_ctime;
        file.stop = buf.st_mtime;
        string sha1result = TRestTools::Execute("sha1sum " + _fullname);
        string sha1 = Split(sha1result, " ")[0];
        if (sha1.size() == 40) {
            for (int i = 0; i < 40; i++) {
                file.sha1sum[i] = sha1[i];
            }
            file.sha1sum[40] = 0;
        }
    }
    return file;
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
/// Get the latest run in database, by reading the file: $REST_USER_PATH/runNumber.
/// Note that this file saves run number of the **next** run. So it returns
/// The run number -1.
int TRestDataBase::get_lastrun() {
    int runNr;
    string runFilename = REST_USER_PATH + "/runNumber";
    if (!TRestTools::fileExists(runFilename)) {
        if (TRestTools::isPathWritable(REST_USER_PATH)) {
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
/// >0 --> directly use this run number, overwrite existing
///
/// It will write to the file: $REST_USER_PATH/runNumber is writable. The number written will be
/// the **next** run number
///
/// The method in derived class shall follow this rule.
int TRestDataBase::set_run(DBEntry info, bool overwrite) {
    int newRunNr;
    if (info.runNr == 0) {
        newRunNr = get_lastrun() + 1;
    } else if (info.runNr > 0) {
        newRunNr = info.runNr;
    } else {
        return -1;
    }

    string runFilename = REST_USER_PATH + "/runNumber";
    if (TRestTools::isPathWritable(REST_USER_PATH)) {
        TRestTools::Execute("echo " + ToString(newRunNr + 1) + " > " + runFilename);
    } else {
        warning << "runNumber file not writable. auto run number "
                "increment is disabled"
             << endl;
    }

    return newRunNr;
}

///////////////////////////////////////////////
/// The following specification of DBEntry's content means to match **any**:
/// id <= 0, type == "" ,usr == "" ,tag == "" ,description == "" ,version == "".
/// If all of them mean **any**, it will return a blank list.
DBEntry TRestDataBase::query_data(DBEntry _info) {
    vector<DBEntry> match;
    if (_info.runNr <= 0 && _info.type == "" && _info.tag == "" && _info.description == "" &&
        _info.version == "")
        return DBEntry();

    for (int i = 0; i < fDataEntries.size(); i++) {
        DBEntry info = fDataEntries[i];

        bool runmatch = (_info.runNr == 0 || info.runNr == 0 || info.runNr == _info.type);
        bool typematch = (_info.type == "" || info.type == _info.type);
        bool tagmatch = (_info.tag == "" || info.tag == _info.tag);
        bool descriptionmatch = (_info.description == "" || info.description == _info.description);
        bool versionmatch = (_info.version == "" || info.version == _info.version);
        bool valuematch = (_info.value == "" || info.value == _info.value);

        if (runmatch && typematch && tagmatch && descriptionmatch && versionmatch && valuematch) {
            match.push_back(info);
        }
    }

    if (match.size() == 1) {
        return match[0];
    } else if (match.size() > 1) {
        warning << "multiple metadata found! returning the first!" << endl;
        return match[0];
    } else {
        return DBEntry();
    }
    return DBEntry();
}

// int TRestDataBase::add_metadata(DBEntry info, string url, bool overwrite) {
//    if (TRestTools::isPathWritable(REST_USER_PATH)) {
//        cout << "error! path not writable" << endl;
//        return -1;
//    }
//
//    string metaFilename = REST_USER_PATH + "/dataURL";
//
//    if (info.runNr == 0) {
//        info.runNr = get_lastmetadata();
//    }
//    fDataEntries.push_back({info, url});
//
//    std::ofstream file(metaFilename, std::ios::app);
//    file << "\"" << info.runNr << "\" ";
//    file << "\"" << info.type << "\" ";
//    file << "\"" << info.tag << "\" ";
//    file << "\"" << info.description << "\" ";
//    file << "\"" << info.version << "\" ";
//    file << "\"" << url << "\" ";
//    file << endl;
//    file.close();
//
//    return info.runNr;
//}
//
