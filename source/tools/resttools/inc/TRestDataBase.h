#ifndef RestCore_TRestDataBase
#define RestCore_TRestDataBase

#include <time.h>

#include <iostream>
#include <map>
#include <vector>

using namespace std;

struct DBFile {
    DBFile() {
        filename = "";
        fileSize = 0;
        evtRate = 0;
        for (int i = 0; i < 41; i++) {
            sha1sum[i] = 0;
        }
        quality = true;
        start = 0;
        stop = 0;
    }

    DBFile(string filename);

    void Print();

    string filename;
    long fileSize;
    double evtRate;
    char sha1sum[41];  // last bit is \0
    bool quality;
    time_t start;
    time_t stop;
};

struct DBEntry {
    DBEntry(int _id = 0, string _type = "", string _tag = "", string _description = "",
            string _version = "") {
        id = _id;
        type = _type;
        tag = _tag;
        description = _description;
        version = _version;
    }
    DBEntry(vector<string> defs);
    int id = 0;
    string type = "";
    string tag = "";
    string description = "";
    string version = "";

    bool operator<(const DBEntry& d) const {
        if (id < d.id) {
            return true;
        }
        return false;
    }

    bool operator>(const DBEntry& d) const {
        if (id > d.id) {
            return true;
        }
        return false;
    }

    bool operator==(const DBEntry& d) const {
        if (id == d.id) {
            return true;
        }
        return false;
    }
};

class TRestDataBase {
   private:
    map<DBEntry, string> fRunFile;
    map<DBEntry, string> fMetaDataFile;
    bool DownloadRemoteFile(string remoteFile, string localFile);

   protected:
    string fConnectionString;

   public:
    /// default constructor, setting fConnectionString according to the env
    TRestDataBase();
    /// destructor
    ~TRestDataBase() {}

    /// gDataBase
    static TRestDataBase* GetDataBase();
    /// instantiate specific database TRestDataBaseXXX according to the name XXX
    static TRestDataBase* instantiate(string name = "");
    /// default: read the dataURL file
    virtual void Initialize();
    virtual void test() {}
    virtual void print(int runnumber) {}
    virtual void exec(string cmd) {}

    ///////////////////////  run number management interface  //////////////////////
    /// return the run number of the run. If not exist, return 0
    virtual int query_run(int runnumber) { return runnumber; }
    /// return the file list of the run.
    virtual vector<string> query_run_files(int runnumber) { return vector<string>(0); }
    /// return the file pattern of the run.
    virtual string query_run_filepattern(int runnumber) { return ""; }
    /// return the information of the run, as struct DBEntry
    virtual DBEntry query_run_info(int runnumber) { return DBEntry(); }
    /// return the file information of the run, as struct DBFile
    virtual DBFile query_run_info_files(int runnumber, int fileid = 0) { return DBFile(); }
    /// return the start time of the run
    virtual double query_run_start(int runnumber) { return 0; }
    /// return the end time of the run
    virtual double query_run_end(int runnumber) { return 0; }

    /// search runs according to the file name. return a list of run numbers
    virtual vector<int> search_run_with_file(string filepattern) { return vector<int>{0}; }
    /// search runs according to the time period. return a list of run numbers
    virtual vector<int> search_run_with_timeperiod(time_t t1, time_t t2) { return vector<int>{0}; }
    /// search runs according to the run info. return a list of run numbers
    virtual vector<int> search_run_with_info(DBEntry info) { return vector<int>{0}; }
    /// search runs according to the database command. return a list of run numbers
    virtual vector<int> search_custom(string expresstion) { return vector<int>{0}; }

    /// get the first run in databse
    virtual int get_firstrun() { return 1; }
    /// get the latest run in database
    virtual int get_lastrun();

    /// add a new run, with run info as struct DBEntry. returns the added run id
    virtual int add_run(DBEntry info = DBEntry());
    /// add a new runfile to the specified run
    virtual int add_runfile(int runnumber, string filename) { return 0; }
    /// add a new runfile to the specified run, set the file info together
    virtual int add_runfile(int runnumber, string filename, DBFile info) { return 0; }

    /// set run info for the specified run
    virtual int set_run_info(int runnumber, DBEntry info) { return 0; }
    /// set file info for the specified file of the specified run
    virtual int set_run_info_files(int runnumber, int fileid, DBFile info) { return 0; }
    /// set run start time for the specified run
    virtual int set_runstart(int runnumber, double starttime) { return 0; }
    /// set run end time for the specified run
    virtual int set_runend(int runnumber, double endtime) { return 0; }

    ///////////////////////  metadata management interface  //////////////////////
    /// return the id of the metadata database entry. If not exist, return 0
    virtual int query_metadata(int id);
    /// return the file url of the metadata database entry
    virtual string query_metadata_fileurl(int id);
    /// return the information of the metadata database entry
    virtual DBEntry query_metadata_info(int id);

    /// search metadata according to the file url. return a list of entry ids
    virtual vector<int> search_metadata_with_fileurl(string url);
    /// search metadata according to the entry information. return a list of entry ids
    virtual vector<int> search_metadata_with_info(DBEntry info);

    /// get the specified metadata file. return the local file path
    virtual string get_metadatafile(int id, string name = "");
    /// get the last metadata file. return the local file path
    virtual int get_lastmetadata();

    /// add a new record of metadata file in database, entry information and remote url should be given
    virtual int add_metadata(DBEntry info, string url);
    /// update entry information for the specified entry
    virtual int update_metadata(int id, DBEntry info);
    /// update remote file for the specified entry. file url will not be updated
    virtual int update_metadatafile(int id, string localfile, string method = "", int port = 0,
                                    string user = "");
};

#define gDataBase (TRestDataBase::GetDataBase())

#endif