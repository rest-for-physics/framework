#include <iostream>

#include "TRestDataBase.h"
#include "TRestTools.h"
using namespace std;

class TRestDataBasePSQL : public TRestDataBase {
   public:
    TRestDataBasePSQL();
    ~TRestDataBasePSQL();

    virtual void Initialize();
    void test();
    void print(int runnumber);
    void exec(string cmd);

    ///////////////////////  run number management interface  //////////////////////
    /// return the run number of the run. If not exist, return 0
    virtual int query_run(int runnumber);
    /// return the file list of the run.
    virtual vector<string> query_run_files(int runnumber);
    /// return the file pattern of the run.
    virtual string query_run_filepattern(int runnumber);
    /// return the information of the run, as struct DBEntry
    virtual DBEntry query_run_info(int runnumber);
    /// return the file information of the run, as struct DBFile
    virtual DBFile query_run_info_files(int runnumber, int fileid = 0);
    /// return the start time of the run
    virtual double query_run_start(int runnumber);
    /// return the end time of the run
    virtual double query_run_end(int runnumber);

    /// search runs according to the file name. return a list of run numbers
    virtual vector<int> search_run_with_file(string filepattern);
    /// search runs according to the time period. return a list of run numbers
    virtual vector<int> search_run_with_timeperiod(time_t t1, time_t t2);
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
    /// return the value of the metadata database entry
    virtual string query_metadata_value(int id);
    /// return a file containing the value string of the entry
    virtual string query_metadata_valuefile(int id, string name = "");
    /// return the information of the metadata database entry
    virtual DBEntry query_metadata_info(int id);

    /// search metadata according to the value. return a list of entry ids
    virtual vector<int> search_metadata_with_value(string url);
    /// search metadata according to the entry information. return a list of entry ids
    virtual vector<int> search_metadata_with_info(DBEntry info);

    /// get the id of the last metadata.
    virtual int get_lastmetadata();

    /// add a new record of metadata in database, entry information and remote url should be given
    virtual int add_metadata(DBEntry info, string value);
    /// update entry information for the specified entry
    virtual int update_metadata(int id, DBEntry info);

};
