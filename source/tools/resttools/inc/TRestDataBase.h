#ifndef RestCore_TRestDataBase
#define RestCore_TRestDataBase

#include <time.h>
#include <iostream>
#include <map>
#include <vector>

using namespace std;

class DataBaseFileInfo {
   public:
    DataBaseFileInfo() {
        fileSize = 0;
        evtRate = 0;
        for (int i = 0; i < 41; i++) {
            sha1sum[i] = 0;
        }
        quality = true;
        start = 0;
        stop = 0;
    }

    DataBaseFileInfo(string filename);

    void Print();

    long fileSize;
    double evtRate;
    char sha1sum[41];  // last bit is \0
    bool quality;
    time_t start;
    time_t stop;
};

class TRestDataBase {
   protected:
    string fURL;

   public:
    TRestDataBase();
    ~TRestDataBase() {}

    static TRestDataBase* instantiate(string name = "");
    virtual void test() {}
    virtual void print(int runnumber) {}
    virtual void exec(string cmd) {}

    virtual int query_run(int runnumber) { return runnumber; }
    virtual vector<string> query_files(int runnumber) { return vector<string>(0); }
    virtual string query_file(int runnumber, int fileid = 0) { return ""; }
    virtual string query_filepattern(int runnumber) { return ""; }
    virtual DataBaseFileInfo query_fileinfo(int runnumber, string filename = "") {
        return DataBaseFileInfo();
    }
    virtual DataBaseFileInfo query_fileinfo(int runnumber, int fileid = 0) { return DataBaseFileInfo(); }
    virtual double query_start(int runnumber) { return 0; }
    virtual double query_end(int runnumber) { return 0; }
    virtual string query_type(int runnumber) { return ""; }
    virtual string query_user(int runnumber) { return ""; }
    virtual string query_tag(int runnumber) { return ""; }
    virtual string query_description(int runnumber) { return ""; }
    virtual string query_version(int runnumber) { return ""; }

    virtual vector<int> search_with_filepattern(string filepattern) { return vector<int>{0}; }
    virtual vector<int> search_with_timeperiod(time_t t1, time_t t2) { return vector<int>{0}; }
    virtual vector<int> search_with_tag(string tag) { return vector<int>{0}; }
    virtual vector<int> search_with_user(string user) { return vector<int>{0}; }
    virtual vector<int> search_with_type(string type) { return vector<int>{0}; }
    virtual vector<int> search_with_description(string description) { return vector<int>{0}; }
    virtual vector<int> search_with_version(string version) { return vector<int>{0}; }
    virtual vector<int> search_with_expression(string expresstion) { return vector<int>{0}; }

    virtual int get_lastrun();

	//-1 --> do not add new run
	//0  --> append a new run in run list
	//>0 --> directly add the corresponding run.
    virtual int add_run(int runnumber = 0);
    virtual int add_runfile(int runnumber, string filename) { return 0; }
    virtual int add_runfile(int runnumber, string filename, DataBaseFileInfo info) { return 0; }

    virtual int set_type(int runnumber, string type) { return 0; }
    virtual int set_user(int runnumber, string user) { return 0; }
    virtual int set_tag(int runnumber, string tag) { return 0; }
    virtual int set_description(int runnumber, string description) { return 0; }
    virtual int set_version(int runnumber, string version) { return 0; }
    virtual int set_runstart(int runnumber, double starttime) { return 0; }
    virtual int set_runend(int runnumber, double endtime) { return 0; }
    virtual int set_fileinfo(int runnumber, int fileid, DataBaseFileInfo info) { return 0; }
    virtual int set_fileinfo(string filename, DataBaseFileInfo info) { return 0; }
};

#endif