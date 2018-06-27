#ifndef RestCore_TRestDataBase
#define RestCore_TRestDataBase

#include <iostream>
#include "TClass.h"
#include <map>
#include <vector>

using namespace std;

class TRestDataBase {
public:
	TRestDataBase() {}
	~TRestDataBase() {}

	static TRestDataBase* instantiate() {
		TClass*c = TClass::GetClass("TRestDataBaseImpl");
		if (c != NULL)//this means we have the package installed
		{
			return (TRestDataBase*)c->New();
		}
		return NULL;
	}
	virtual int test() { return 0; }

	virtual string query_file(int runnumber) { return ""; }
	virtual double query_start(int runnumber) { return 0; }
	virtual double query_end(int runnumber) { return 0; }
	virtual string query_type(int runnumber) { return ""; }
	virtual string query_user(int runnumber) { return ""; }
	virtual string query_tag(int runnumber) { return ""; }
	virtual string query_description(int runnumber) { return ""; }
	virtual map<string, string> query_config(int runnumber) { return map<string,string>(); }

	virtual vector<int> getrunswithintime(double t1, double t2) { return vector<int>(0); }
	virtual vector<int> getrunswithtag(string tag) { return vector<int>(0); }
	virtual int getrunnumber(string filename) { return 0; }
	virtual int getnumberofruns() { return 0; }

	virtual int newrun() { return 0; }

	virtual int set_runtype(int runnumber, string type) { return 0; }
	virtual int set_runuser(int runnumber, string user) { return 0; }
	virtual int set_runtag(int runnumber, string tag) { return 0; }
	virtual int set_rundescription(int runnumber, string description) { return 0; }
	virtual int set_runconfig(int runnumber, map<string, string> config) { return 0; }

	virtual int add_runconfig(int runnumber, string key, string value) { return 0; }
	virtual int add_runfile(int runnumber, string filename) { return 0; }

};


#endif