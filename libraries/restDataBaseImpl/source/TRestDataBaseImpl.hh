#include <iostream>
#include "TRestDataBase.h"
#include "pg.hh"
using namespace std;


class TRestDataBaseImpl:public TRestDataBase {
public:
	TRestDataBaseImpl();
	~TRestDataBaseImpl();

	void test();
	void print(int runnumber, int subrun = 0);
	void exec(string cmd);

	int query_run(int runnumber, int subrun = 0);
	int query_subrun(int runnumber, int subrun = 0);
	vector<string> query_files(int runnumber, int subrun = 0);
	string query_file(int runnumber, int subrun = 0, int fileid = 0);
	string query_filepattern(int runnumber, int subrun = 0);
	DataBaseFileInfo query_fileinfo(int runnumber, int subrun = 0, string filename = "");
	DataBaseFileInfo query_fileinfo(int runnumber, int subrun = 0, int fileid = 0);
	double query_start(int runnumber, int subrun = 0);
	double query_end(int runnumber, int subrun = 0);
	string query_type(int runnumber, int subrun = 0);
	string query_user(int runnumber, int subrun = 0);
	string query_tag(int runnumber, int subrun = 0);
	string query_description(int runnumber, int subrun = 0);
	string query_version(int runnumber, int subrun = 0);

	vector<pair<int, int>> search_filepattern(string filepattern);
	vector<pair<int, int>> search_withintime(time_t t1, time_t t2);
	vector<pair<int, int>> search_tag(string tag);
	vector<pair<int, int>> search_user(string user);
	vector<pair<int, int>> search_type(string type);
	vector<pair<int, int>> search_description(string description);
	vector<pair<int, int>> search_version(string version);
	vector<pair<int, int>> searchexp(string expresstion);

	pair<int, int> getrunwithfilename(string filename);
	int getlastrun();
	int getlastsubrun(int runnumber);

	int new_run();
	int new_run(int runnumber);
	int new_runfile(string filename, DataBaseFileInfo info);

	int set_runnumber(int runnumber);
	int set_subrun(int subrun);
	int set_type(string type);
	int set_user(string user);
	int set_tag(string tag);
	int set_description(string description);
	int set_version(string version);
	int set_runstart(double starttime);
	int set_runend(double endtime);
	int set_fileinfo(int fileid, DataBaseFileInfo info);
	int set_fileinfo(string filename, DataBaseFileInfo info);

protected:
	std::shared_ptr<PGconn> conn;
};




