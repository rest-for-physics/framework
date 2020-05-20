#include <iostream>

#include "TRestDataBase.h"
#include "TRestTools.h"
using namespace std;

class TRestDataBasePSQL : public TRestDataBase {
   public:
    TRestDataBasePSQL();
    ~TRestDataBasePSQL();

    virtual void Initialize() override;
    void test() override;
    void print(string cmd) override;
    DBTable exec(string cmd) override;
    virtual DBFile wrap_data(DBEntry data, string name = "") override;

    virtual DBEntry query_run(int runnumber) override;
    virtual DBFile query_run_file(int runnumber, int fileid) override;
    virtual vector<DBFile> query_run_files(int runnumber) override;

    virtual vector<int> search_run(DBEntry info) override;
    virtual vector<int> search_run_with_file(string filepattern) override;

    virtual int get_lastrun() override;

    virtual int set_run(DBEntry info, bool overwrite = true) override;
    virtual int set_runfile(int runnumber, string filename) override;

    virtual DBEntry query_data(int id) override;

    virtual vector<int> search_data(DBEntry info) override;

    virtual int get_lastdata() override;

    virtual int set_data(DBEntry info, bool overwrite = true) override;

};
