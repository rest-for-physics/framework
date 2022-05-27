#ifndef RestCore_TRestDataBase
#define RestCore_TRestDataBase

#include <iostream>
#include <map>
#include <vector>

#include "TRestTools.h"

// Contains basic information of the row. These information also appears in TRestRun.
// One or more DBEntry objects stores the information of one run.
//
// * During run querying, the information shows the corresponding run information
//
// * During metadata querying:
// runNr: the corresponding run of this metadata.
// type: It can be the name of one of the store metadata, or the name of some saved constants.
// tag: when type is "META_RML", it indicates which REST class the rml is for.
// value: the value of the stored metadata. for example, type="gas_pressure", value="10"
// type="META_RML", tag="TRestDetectorGas", value="https://sultan.unizar.es/gasFiles/gases.rml"
//
// When used for database searching, if the struct field value is not default value,
// they will be regarded as "any"
struct DBEntry {
    DBEntry(int _runNr = 0, std::string _type = "", std::string _tag = "", std::string _description = "",
            std::string _version = "") {
        runNr = _runNr;
        type = _type;
        tag = _tag;
        description = _description;
        version = _version;
    }
    DBEntry(std::vector<std::string> defs);
    int runNr = 0;
    std::string type = "";
    std::string tag = "";
    std::string user = "";
    std::string description = "";
    std::string version = "";
    time_t tstart = 0;
    time_t tend = 0;

    std::string value = "";

    bool IsZombie() { return runNr == 0 || (type == "" && value == ""); }
};

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

    DBFile(std::string filename) { this->filename = filename; }
    static DBFile ParseFile(std::string filename);

    void Print();
    operator std::string() { return filename; }

    std::string filename;
    long fileSize;
    double evtRate;
    char sha1sum[41];  // last bit is \0
    bool quality;
    time_t start;
    time_t stop;
};

struct DBTable : public std::vector<std::vector<std::string>> {
    std::vector<std::string> headerline;
    int rows() { return size(); }
    int columns() { return headerline.size(); }
};

class TRestDataBase {
   protected:
    std::vector<DBEntry> fDataEntries;
    std::string fConnectionString;

   public:
    /// default constructor, setting fConnectionString according to the env
    TRestDataBase();
    /// destructor
    ~TRestDataBase() {}

    /// default: read the dataURL file
    virtual void Initialize();
    /// test function
    virtual void test() {}
    /// print the table after query std::string cmd
    virtual void print(std::string cmd) {}
    /// return a table from query std::string cmd
    virtual DBTable exec(std::string cmd) { return DBTable(); }

    ///////////////////////  run number management interface  //////////////////////
    /// return the run number of the run. If not exist, return 0
    virtual DBEntry query_run(int runnumber) { return DBEntry(runnumber); }
    /// return file of certain file id in certain run
    virtual DBFile query_run_file(int runnumber, int fileid) { return DBFile(); }
    /// return all the files of the run
    virtual std::vector<DBFile> query_run_files(int runnumber) { return std::vector<DBFile>(); }

    /// search runs according to the run info. return a list of run numbers
    virtual std::vector<int> search_run(DBEntry info) { return std::vector<int>{0}; }
    /// search runs according to the file name. return a list of run numbers
    virtual std::vector<int> search_run_with_file(std::string filepattern) { return std::vector<int>{0}; }

    /// get the latest run id in database
    virtual int get_lastrun();

    /// add/update a run, with run info as struct DBEntry. returns the added run id
    virtual int set_run(DBEntry info, bool overwrite = true);
    /// add/update a runfile to the specified run
    virtual int set_runfile(int runnumber, std::string filename) { return 0; }

    ///////////////////////  metadata management interface  //////////////////////
    /// return the piece of data in fMetadataEntries
    virtual DBEntry query_data(DBEntry info);
    /// add/update a new record of metadata in database
    virtual int set_data(DBEntry info, bool overwrite = true) { return 0; }
};

EXPORT_EXTERN TRestDataBase* gDataBase;

#endif
