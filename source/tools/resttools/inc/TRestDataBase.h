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
    int id = 0;
    string type = "";
    string usr = "";
    string tag = "";
    string description = "";
    string version = "";
};

class TRestDataBase {
   protected:
    string fConnectionString;

   public:
    TRestDataBase();
    ~TRestDataBase() {}

    static TRestDataBase* instantiate(string name = "");
    virtual void test() {}
    virtual void print(int runnumber) {}
    virtual void exec(string cmd) {}

    //////////////////////  run number management interface  //////////////////////
    virtual int query_run(int runnumber) { return runnumber; }
    virtual vector<string> query_run_files(int runnumber) { return vector<string>(0); }
    virtual string query_run_filepattern(int runnumber) { return ""; }
    virtual DBEntry query_run_info(int runnumber) { return DBEntry(); }
    virtual DBFile query_run_info_files(int runnumber, int fileid = 0) { return DBFile(); }
    virtual double query_run_start(int runnumber) { return 0; }
    virtual double query_run_end(int runnumber) { return 0; }

    virtual vector<int> search_run_with_file(string filepattern) { return vector<int>{0}; }
    virtual vector<int> search_run_with_timeperiod(time_t t1, time_t t2) { return vector<int>{0}; }
    virtual vector<int> search_run_with_info(DBEntry info) { return vector<int>{0}; }
    virtual vector<int> search_custom(string expresstion) { return vector<int>{0}; }

    virtual int get_firstrun() { return 1; }
    virtual int get_lastrun();

    // info.id =
    //-1 --> do not add new run
    // 0  --> append a new run in run list
    //>0 --> directly add the corresponding run.
    virtual int add_run(DBEntry info = DBEntry());
    virtual int add_runfile(int runnumber, string filename) { return 0; }
    virtual int add_runfile(int runnumber, string filename, DBFile info) { return 0; }

    virtual int set_run_info(int runnumber, DBEntry info) { return 0; }
    virtual int set_run_info_files(int runnumber, int fileid, DBFile info) { return 0; }
    virtual int set_runstart(int runnumber, double starttime) { return 0; }
    virtual int set_runend(int runnumber, double endtime) { return 0; }

    //////////////////////  metadata management interface  //////////////////////
    virtual int query_metadata(int id) { return id; }
    virtual string query_metadata_fileurl(int id) { return ""; }
    virtual DBEntry query_metadata_info(int id) { return DBEntry(); }

    virtual vector<int> search_metadata_with_fileurl(string url) { return vector<int>{0}; }
    virtual vector<int> search_metadata_with_info(DBEntry info) { return vector<int>{0}; }

	virtual string get_metadatafile(int id) { return ""; }

    virtual int add_metadata(DBEntry info = DBEntry()) { return 0; }
    virtual int add_metadatafile(int id, string url) { return 0; }
    virtual int add_metadatafile(int id, string url, string urlremote) { return 0; }

    virtual int set_metadata_info(int id, DBEntry info) { return 0; }
};

#endif