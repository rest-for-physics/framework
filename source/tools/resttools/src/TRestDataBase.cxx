#include "TRestDataBase.h"
#include <errno.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "TClass.h"
#include "TRestStringHelper.h"
#include "TRestTools.h"
#include "TSystem.h"
#include "TRestStringOutput.h"

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

void TRestDataBase::Initialize() {
    fMetaDataFile.clear();
    string metaFilename = getenv("REST_PATH") + (string) "/dataURL";
    if (!TRestTools::fileExists(metaFilename)) {
        return;
    } else {
        vector<int> result;
        ifstream infile(metaFilename);
        string s;
        while (TRestTools::GetLine(infile, s)) {
            vector<string> items = Split(s, "\" \"", true);
            if (items.size() != 7) continue;

			if (items[0][0] == '\"') items[0] = items[0].substr(1, -1);
            if (items[6][items[6].size() - 1] == '\"') items[6] = items[6].substr(0, items[6].size() - 1);
            DBEntry info;
            info.id = atoi(items[0].c_str());
            info.type = items[1];
            info.usr = items[2];
            info.tag = items[3];
            info.description = items[4];
            info.version = items[5];
            string dburl = items[6];

            fMetaDataFile[info] = dburl;
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

int TRestDataBase::get_lastrun() {
    int runNr;
    string runFilename = getenv("REST_PATH") + (string) "/runNumber";
    if (!TRestTools::fileExists(runFilename)) {
        if (TRestTools::isPathWritable(getenv("REST_PATH"))) {
            TRestTools::Execute("echo 1 > " + runFilename);
            runNr = 1;
        }
    } else {
        ifstream ifs(runFilename);
        ifs >> runNr;
    }
    return runNr - 1;
}

int TRestDataBase::add_run(DBEntry info) {
    int newRunNr;
    if (info.id == 0) {
        newRunNr = get_lastrun() + 1;
    } else if (info.id > 0) {
        newRunNr = info.id;
    } else {
        return -1;
    }

    string runFilename = getenv("REST_PATH") + (string) "/runNumber";
    if (TRestTools::isPathWritable(getenv("REST_PATH"))) {
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
    DBEntry info;
    info.id = id;

    auto list = search_metadata_with_info(info);
    if (list.size() == 1) return id;
    return -1;
}

string TRestDataBase::query_metadata_fileurl(int id) {
    auto iter = fMetaDataFile.begin();
    while (iter != fMetaDataFile.end()) {
        DBEntry entry = iter->first;
        string url = iter->second;
        if (entry.id == id) {
            return url;
        }
        iter++;
    }
}
DBEntry TRestDataBase::query_metadata_info(int id) {
    auto iter = fMetaDataFile.begin();
    while (iter != fMetaDataFile.end()) {
        DBEntry entry = iter->first;
        string url = iter->second;
        if (entry.id == id) {
            return entry;
        }
        iter++;
    }
}

vector<int> TRestDataBase::search_metadata_with_fileurl(string _url) {
    vector<int> result;
    auto iter = fMetaDataFile.begin();
    while (iter != fMetaDataFile.end()) {
        DBEntry entry = iter->first;
        string url = iter->second;
        if (url.find(_url) != -1) {
            result.push_back(entry.id);
        }
        iter++;
    }
    return result;
}

///////////////////////////////////////////////
/// \brief Get a list of matched matadata id, according to the DBEntry's content
///
/// The following specification of DBEntry's content means to match **any**:
/// id <= 0, type == "" ,usr == "" ,tag == "" ,description == "" ,version == "".
/// If all of them mean **any**, it will return a blank list.
vector<int> TRestDataBase::search_metadata_with_info(DBEntry _info) {
    vector<int> result;
    if (_info.id <= 0 && _info.type == "" && _info.usr == "" && _info.tag == "" && _info.description == "" &&
        _info.version == "")
        return result;

    auto iter = fMetaDataFile.begin();
    while (iter != fMetaDataFile.end()) {
        DBEntry info = iter->first;
        string url = iter->second;
        if (_info.id > 0 && info.id == _info.id) {
            result.push_back(info.id);
        } else {
            bool typematch = (_info.type == "" || info.type == _info.type);
            bool usrmatch = (_info.usr == "" || info.usr == _info.usr);
            bool tagmatch = (_info.tag == "" || info.tag == _info.tag);
            bool descriptionmatch = (_info.description == "" || info.description == _info.description);
            bool versionmatch = (_info.version == "" || info.version == _info.version);

            if (typematch && usrmatch && tagmatch && descriptionmatch && versionmatch) {
                result.push_back(info.id);
            }
        }
        iter++;
    }
    return result;
}

string TRestDataBase::get_metadatafile(string url) {
    string purename = TRestTools::GetPureFileName(url);
    if (purename == "") return "";

    if (url.find("local:") == 0) {
        return Replace(url, "local:", "");
    } else {
        string fullpath;
        if (TRestTools::isPathWritable(getenv("REST_PATH"))) {
            fullpath = getenv("REST_PATH") + (string) "/data/download/" + purename;
            if (DownloadRemoteFile(url, fullpath)) {
                return fullpath;
            } else {
                return "";
            }
        } else {
            fullpath = "/tmp/REST_" + (string)getenv("USER") + "_Download_" + purename;
            if (DownloadRemoteFile(url, fullpath)) {
                return fullpath;
            } else {
                return "";
            }
        }
    }

    return "";


}

string TRestDataBase::get_metadatafile(int id, string name) {
    string url = query_metadata_fileurl(id);
	string purename = TRestTools::GetPureFileName(url);
    if (purename == "")
        purename = name;
    else
        name = "";

	return get_metadatafile(url + name);
}

int TRestDataBase::get_lastmetadata() {
    auto iter = fMetaDataFile.end();
    iter--;
    return iter->first.id;
}

int TRestDataBase::add_metadata(DBEntry info, string url) {
    if (TRestTools::isPathWritable(getenv("REST_PATH"))) {
        cout << "error" << endl;
        return -1;
    }

    string metaFilename = getenv("REST_PATH") + (string) "/dataURL";

    if (info.id == 0) {
        info.id = get_lastmetadata();
    }
    fMetaDataFile[info] = url;

	std::ofstream file(metaFilename, std::ios::app);
    file << "\"" << info.id << "\" ";
    file << "\"" << info.type << "\" ";
    file << "\"" << info.usr << "\" ";
    file << "\"" << info.tag << "\" ";
    file << "\"" << info.description << "\" ";
    file << "\"" << info.version << "\" ";
    file << "\"" << url << "\" ";
    file << endl;
    file.close();

	return info.id;
}

///////////////////////////////////////////////
/// \brief Assign a new file url to this metadata entry
///
/// If the file is a remote url, it will directly update the database
///
/// If the file is a local file, it will upload it and overwrite the remote one
/// The database will remain unchanged.
/// 
int TRestDataBase::set_metadatafile(int id, string url) {
    cout << "error" << endl;

	string cmd = "scp " + url + " gasUser@sultan.unizar.es:./gasFiles/";
    int a = system(cmd.c_str());

    if (a != 0) {
        ferr << __PRETTY_FUNCTION__ << endl;
        ferr << "problem copying gases definitions to remote server" << endl;
        ferr << "Please report this problem at "
                 "http://gifna.unizar.es/rest-forum/"
              << endl;
        return -1;
    }

    return 0;
}
///////////////////////////////////////////////
/// \brief It will upload the file from **url** to **urlremote**, and update the database
///
int TRestDataBase::set_metadatafile(int id, string url, string urlremote) {
    cout << "error" << endl;
    return 0;
}
///////////////////////////////////////////////
/// \brief It will update the information of database
///
/// The following specification of DBEntry's content will not be updated:
/// id <= 0, type == "" ,usr == "" ,tag == "" ,description == "" ,version == "".
int TRestDataBase::set_metadata_info(int id, DBEntry info) {
    cout << "error" << endl;
    return 0;
}

///////////////////////////////////////////////
/// \brief It will download the remote file provided in the argument using wget.
///
/// If it succeeds to download the file, this method will return the location of
/// the local temporary file downloaded. If it fails, the method will invoke an
/// exit call and print out some error.
bool TRestDataBase::DownloadRemoteFile(string remoteFile, string localFile) {
    if (remoteFile.find("https:") == 0 || remoteFile.find("http:") == 0) {
        string cmd = "wget --no-check-certificate " + remoteFile + " -O " + localFile + " -q";

        int a = system(cmd.c_str());

        if (a == 0) {
            return true;
        } else {
            cout << "-- Error : download failed!" << endl;
            if (a == 1024) cout << "-- Error : Network connection problem?" << endl;
            if (a == 2048) {cout << "Gas definition does NOT exist in database?" << endl;
                cout << "File name: " << remoteFile << endl;
			}
            cout << "Please specify a local file" << endl;
        }
    } else if (remoteFile.find("ssh:") == 0) {
    } else {
        cout << "-- Error : unknown protocol!" << endl;
    }

    return false;
}