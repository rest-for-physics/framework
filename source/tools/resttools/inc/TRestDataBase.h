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
    int fRunNumber = 0;
    int fSubRunNumber = 0;

   public:
    TRestDataBase() {}
    ~TRestDataBase() {}

    static TRestDataBase* instantiate();
    virtual void test() {}
    virtual void print(int runnumber, int subrun = 0) {}
    virtual void exec(string cmd) {}

    virtual int query_run(int runnumber, int subrun = 0) { return runnumber; }
    virtual int query_subrun(int runnumber, int subrun = 0) { return subrun; }
    virtual vector<string> query_files(int runnumber, int subrun = 0) { return vector<string>(0); }
    virtual string query_file(int runnumber, int subrun = 0, int fileid = 0) { return ""; }
    virtual string query_filepattern(int runnumber, int subrun = 0) { return ""; }
    virtual DataBaseFileInfo query_fileinfo(int runnumber, int subrun = 0, string filename = "") {
        return DataBaseFileInfo();
    }
    virtual DataBaseFileInfo query_fileinfo(int runnumber, int subrun = 0, int fileid = 0) {
        return DataBaseFileInfo();
    }
    virtual double query_start(int runnumber, int subrun = 0) { return 0; }
    virtual double query_end(int runnumber, int subrun = 0) { return 0; }
    virtual string query_type(int runnumber, int subrun = 0) { return ""; }
    virtual string query_user(int runnumber, int subrun = 0) { return ""; }
    virtual string query_tag(int runnumber, int subrun = 0) { return ""; }
    virtual string query_description(int runnumber, int subrun = 0) { return ""; }
    virtual string query_version(int runnumber, int subrun = 0) { return ""; }

    virtual vector<pair<int, int> > search_filepattern(string filepattern) {
        return vector<pair<int, int> >(0);
    }
    virtual vector<pair<int, int> > search_withintime(time_t t1, time_t t2) {
        return vector<pair<int, int> >(0);
    }
    virtual vector<pair<int, int> > search_tag(string tag) { return vector<pair<int, int> >(0); }
    virtual vector<pair<int, int> > search_user(string user) { return vector<pair<int, int> >(0); }
    virtual vector<pair<int, int> > search_type(string type) { return vector<pair<int, int> >(0); }
    virtual vector<pair<int, int> > search_description(string description) {
        return vector<pair<int, int> >(0);
    }
    virtual vector<pair<int, int> > search_version(string version) { return vector<pair<int, int> >(0); }
    virtual vector<pair<int, int> > searchexp(string expresstion) { return vector<pair<int, int> >(0); }

    virtual pair<int, int> getrunwithfilename(string filename) { return pair<int, int>(); }
    virtual int getlastrun() { return 0; }
    virtual int getlastsubrun(int runnumber) { return 0; }
    virtual int getcurrentrun() { return fRunNumber; }
    virtual int getcurrentsubrun() { return fSubRunNumber; }

    virtual int new_run() { return 0; }
    virtual int new_run(int runnumber) { return 0; }
    virtual int new_runfile(string filename) { return new_runfile(filename, DataBaseFileInfo(filename)); }
    virtual int new_runfile(string filename, DataBaseFileInfo info) { return 0; }

    virtual int set_runnumber(int runnumber) { return 0; }
    virtual int set_subrun(int subrun) { return 0; }
    virtual int set_type(string type) { return 0; }
    virtual int set_user(string user) { return 0; }
    virtual int set_tag(string tag) { return 0; }
    virtual int set_description(string description) { return 0; }
    virtual int set_version(string version) { return 0; }
    virtual int set_runstart(double starttime) { return 0; }
    virtual int set_runend(double endtime) { return 0; }
    virtual int set_fileinfo(int fileid, DataBaseFileInfo info) { return 0; }
    virtual int set_fileinfo(string filename, DataBaseFileInfo info) { return 0; }
};

#endif