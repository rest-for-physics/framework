#include "TRestDataBaseImpl.hh"
#include "TRestStringHelper.h"
using namespace pg;

TRestDataBaseImpl::TRestDataBaseImpl() {
	conn = pg::connect("postgresql://p3daq:ilovepanda@localhost/p3_daq");
}

TRestDataBaseImpl::~TRestDataBaseImpl() {

}

// returns input runnumber when run exists, returns -1 if not
int TRestDataBaseImpl::query_run(int runnumber, int subrun) {
	auto a = query(conn, "select run_id from rest_runs where run_id=$1::int", runnumber);
	return a.size() > 0 ? StringToInteger(a[0][0]) : -1;
}
// returns input runnumber when subrun exists, returns -1 if not
int TRestDataBaseImpl::query_subrun(int runnumber, int subrun) {
	auto a = query(conn, "select subrun_id from rest_runs where run_id=$1::int and subrun_id=$2::int", runnumber, subrun);
	return a.size() > 0 ? StringToInteger(a[0][0]) : -1;
}
vector<string> TRestDataBaseImpl::query_files(int runnumber, int subrun)
{
	auto a = query(conn, "select file_name from rest_files where run_id=$1::int and subrun_id=$2::int", runnumber, subrun);

	vector<string> files;
	for (int i = 0; i < a.size(); i++) {
		files.push_back(a[i][0]);
	}
	return files;
}
string TRestDataBaseImpl::query_file(int runnumber, int subrun, int fileid) {
	auto a = query(conn, "select file_name from rest_files where run_id=$1::int and subrun_id=$2::int and file_id=$3::int", runnumber, subrun, fileid);
	if (a.size() > 0) {
		return a[0][0];
	}
	return "";
}
string TRestDataBaseImpl::query_filepattern(int runnumber, int subrun)
{
	auto a = query_files(runnumber, subrun);
	return a.size() > 0 ? a[0] : "";
}
DataBaseFileInfo TRestDataBaseImpl::query_fileinfo(int runnumber, int subrun, string filename) {
	string querystring;
	if (filename != "") {
		querystring = "select file_size, start_time, stop_time, event_rate, sha1sum, quality from rest_files where run_id=$1::int and subrun_id=$2::int and file_name=$3::text";
	}
	else
	{
		querystring = "select file_size, start_time, stop_time, event_rate, sha1sum, quality from rest_files where run_id=$1::int and subrun_id=$2::int and file_id=0";
	}

	auto a = query(conn, querystring, runnumber, subrun, filename);

	if (a.size() > 0) {
		DataBaseFileInfo result;
		result.fileSize = StringToLong(a[0][0]);
		result.start = ToTime(a[0][1]);
		result.stop = ToTime(a[0][2]);
		result.evtRate = StringToDouble(a[0][3]);
		if (a[0][4].size() == 40) {
			for (int i = 0; i < 40; i++) {
				result.sha1sum[i] = a[0][4][i];
			}
		}
		result.quality = a[0][5] == "t";

		return result;
	}

	return DataBaseFileInfo();

}
DataBaseFileInfo TRestDataBaseImpl::query_fileinfo(int runnumber, int subrun, int fileid) {
	string querystring = "select file_size, start_time, stop_time, event_rate, sha1sum, quality from rest_files where run_id=$1::int and subrun_id=$2::int and file_id=$3::int";

	auto a = query(conn, querystring, runnumber, subrun, fileid);

	if (a.size() > 0) {
		DataBaseFileInfo result;
		result.fileSize = StringToLong(a[0][0]);
		result.start = ToTime(a[0][1]);
		result.stop = ToTime(a[0][2]);
		result.evtRate = StringToDouble(a[0][3]);
		if (a[0][4].size() == 40) {
			for (int i = 0; i < 40; i++) {
				result.sha1sum[i] = a[0][4][i];
			}
		}
		result.quality = a[0][5] == "t";

		return result;
	}

	return DataBaseFileInfo();

}
double TRestDataBaseImpl::query_start(int runnumber, int subrun) {
	auto a = query(conn, "select run_start from rest_runs where run_id=$1::int and subrun_id=$2::int", runnumber, subrun);
	return a.size() > 0 ? ToTime(a[0][0]) : 0;
}
double TRestDataBaseImpl::query_end(int runnumber, int subrun) {
	auto a = query(conn, "select run_end from rest_runs where run_id=$1::int and subrun_id=$2::int", runnumber, subrun);
	return a.size() > 0 ? ToTime(a[0][0]) : 0;
}
string TRestDataBaseImpl::query_type(int runnumber, int subrun) {
	auto a = query(conn, "select type from rest_runs where run_id=$1::int and subrun_id=$2::int", runnumber, subrun);
	return a.size() > 0 ? a[0][0] : "";
}
string TRestDataBaseImpl::query_user(int runnumber, int subrun) {
	auto a = query(conn, "select usr from rest_runs where run_id=$1::int and subrun_id=$2::int", runnumber, subrun);
	return a.size() > 0 ? a[0][0] : "";
}
string TRestDataBaseImpl::query_tag(int runnumber, int subrun) {
	auto a = query(conn, "select tag from rest_runs where run_id=$1::int and subrun_id=$2::int", runnumber, subrun);
	return a.size() > 0 ? a[0][0] : "";
}
string TRestDataBaseImpl::query_description(int runnumber, int subrun) {
	auto a = query(conn, "select description from rest_runs where run_id=$1::int and subrun_id=$2::int", runnumber, subrun);
	return a.size() > 0 ? a[0][0] : "";
}
string TRestDataBaseImpl::query_version(int runnumber, int subrun) {
	auto a = query(conn, "select version from rest_runs where run_id=$1::int and subrun_id=$2::int", runnumber, subrun);
	return a.size() > 0 ? a[0][0] : "";
}


vector<pair<int, int>> TRestDataBaseImpl::search_filepattern(string filepattern) {
	string namepattern = Replace(filepattern, "*", "%", 0);
	namepattern = Replace(namepattern, "?", "%", 0);

	auto a = query(conn, "select run_id, subrun_id from rest_files where file_name like $1::text", namepattern);
	vector<pair<int, int>> result(0);
	for (int i = 0; i < a.size(); i++) {
		result.push_back(pair<int, int>(StringToInteger(a[i][0]), StringToInteger(a[i][1])));
	}
	return result;
}
vector<pair<int, int>> TRestDataBaseImpl::search_withintime(time_t t1, time_t t2) {
	auto a = query(conn, "select run_id, subrun_id from rest_runs where run_start>=to_timestamp($1::bigint) and run_end<=to_timestamp($2::bigint)", t1, t2);
	vector<pair<int, int>> result(0);
	for (int i = 0; i < a.size(); i++) {
		result.push_back(pair<int, int>(StringToInteger(a[i][0]), StringToInteger(a[i][1])));
	}
	return result;
}
vector<pair<int, int>> TRestDataBaseImpl::search_tag(string tag) {
	string _tag = Replace(tag, "*", "%", 0);
	_tag = Replace(_tag, "?", "%", 0);
	auto a = query(conn, "select run_id, subrun_id from rest_runs where tag like $1::text", _tag);
	vector<pair<int, int>> result(0);
	for (int i = 0; i < a.size(); i++) {
		result.push_back(pair<int, int>(StringToInteger(a[i][0]), StringToInteger(a[i][1])));
	}
	return result;
}
vector<pair<int, int>> TRestDataBaseImpl::search_user(string user) {
	string _user = Replace(user, "*", "%", 0);
	_user = Replace(_user, "?", "%", 0);
	auto a = query(conn, "select run_id, subrun_id from rest_runs where usr like $1::text", _user);
	vector<pair<int, int>> result(0);
	for (int i = 0; i < a.size(); i++) {
		result.push_back(pair<int, int>(StringToInteger(a[i][0]), StringToInteger(a[i][1])));
	}
	return result;
}
vector<pair<int, int>> TRestDataBaseImpl::search_type(string type) {
	string _type = Replace(type, "*", "%", 0);
	_type = Replace(_type, "?", "%", 0);
	auto a = query(conn, "select run_id, subrun_id from rest_runs where cast(type as text) like $1::text", _type);
	vector<pair<int, int>> result(0);
	for (int i = 0; i < a.size(); i++) {
		result.push_back(pair<int, int>(StringToInteger(a[i][0]), StringToInteger(a[i][1])));
	}
	return result;
}
vector<pair<int, int>> TRestDataBaseImpl::search_description(string description) {
	string _description = Replace(description, "*", "%", 0);
	_description = Replace(_description, "?", "%", 0);
	auto a = query(conn, "select run_id, subrun_id from rest_runs where description like $1::text", _description);
	vector<pair<int, int>> result(0);
	for (int i = 0; i < a.size(); i++) {
		result.push_back(pair<int, int>(StringToInteger(a[i][0]), StringToInteger(a[i][1])));
	}
	return result;
}
vector<pair<int, int>> TRestDataBaseImpl::search_version(string version) {
	string _version = Replace(version, "*", "%", 0);
	_version = Replace(_version, "?", "%", 0);
	auto a = query(conn, "select run_id, subrun_id from rest_runs where version like $1::text", _version);
	vector<pair<int, int>> result(0);
	for (int i = 0; i < a.size(); i++) {
		result.push_back(pair<int, int>(StringToInteger(a[i][0]), StringToInteger(a[i][1])));
	}
	return result;
}
vector<pair<int, int>> TRestDataBaseImpl::searchexp(string expresstion) {
	auto a = query(conn, "select run_id, subrun_id from rest_runs where " + expresstion);
	vector<pair<int, int>> result(0);
	for (int i = 0; i < a.size(); i++) {
		result.push_back(pair<int, int>(StringToInteger(a[i][0]), StringToInteger(a[i][1])));
	}
	return result;
}


pair<int, int> TRestDataBaseImpl::getrunwithfilename(string filename) {
	auto a = search_filepattern(filename);

	if (a.size() == 0) {
		cout << "REST_WARNING : find zero runs with file \"" << filename << "\"!" << endl;
		return pair<int, int>(0, 0);
	}
	if (a.size() > 1) {
		cout << "REST_WARNING : find multiple runs with file \"" << filename << "\", returning the first one" << endl;
	}
	return a[0];
}
int TRestDataBaseImpl::getlastrun() {
	auto a = query(conn, "select MAX(run_id) from rest_runs");
	return StringToInteger(a[0][0]);
}
int TRestDataBaseImpl::getlastsubrun(int runnumber) {
	auto a = query(conn, "select MAX(subrun_id) from rest_runs where run_id=$1::int", runnumber);
	return StringToInteger(a[0][0]);
}


//create new run in data base
//returning the created run id
int TRestDataBaseImpl::new_run() {
	int last_run = getlastrun();
	fRunNumber = last_run + 1;
	int last_subrun = getlastsubrun(fRunNumber);
	fSubRunNumber = last_subrun + 1;

	string user = getenv("USER") == NULL ? "" : getenv("USER");
	string version = ExecuteShellCommand("rest-config --version");
	auto a = query(conn, "insert into rest_runs(run_id, subrun_id, usr, version) values($1::int, $2::int, $3::text, $4::text)",
		fRunNumber,
		fSubRunNumber,
		user,
		version
	);

	return fRunNumber;
}
//create subrun for specific run number
//returning the created subrun id
int TRestDataBaseImpl::new_run(int runnumber) {
	int last_run = getlastrun();
	if (runnumber > last_run + 1) {
		cout << "REST ERROR : cannot create run with run id incoherent! (last run: " << last_run << ", given run: " << runnumber << ")" << endl;
		cout << "using next run id..." << endl;
		fRunNumber = last_run + 1;
	}
	else
	{
		fRunNumber = runnumber;
	}

	int last_subrun = getlastsubrun(fRunNumber);
	fSubRunNumber = last_subrun + 1;

	string user = getenv("USER") == NULL ? "" : getenv("USER");
	string version = ExecuteShellCommand("rest-config --version");
	auto a = query(conn, "insert into rest_runs(run_id, subrun_id, usr, version) values($1::int, $2::int, $3::text, $4::text)",
		fRunNumber,
		fSubRunNumber,
		user,
		version
	);


	return fSubRunNumber;
}
// create new run file according to the given file name and DataBaseFileInfo object
// returns the added file id if success, returns -1 if failed
int TRestDataBaseImpl::new_runfile(string filename, DataBaseFileInfo info) {
	int nFiles = -1;
	if (fRunNumber >= 0 && fSubRunNumber >= 0) {
		nFiles = query_files(fRunNumber, fSubRunNumber).size();

		auto a = query(conn, "insert into rest_files(run_id, subrun_id, file_id, file_name, file_size, start_time, stop_time, event_rate, sha1sum, quality ) values($1::int, $2::int, $3::int, $4::text, $5::bigint, $6::timestamp with time zone, $7::timestamp with time zone, $8::real, $9::text, $10::bool)",
			fRunNumber,
			fSubRunNumber,
			nFiles,
			ToAbsoluteName(filename),
			ToString(info.fileSize),
			ToDateTimeString(info.start),
			ToDateTimeString(info.stop),
			info.evtRate,
			(string)info.sha1sum,
			info.quality
		);
	}
	else
	{
		cout << "REST ERROR : cannot add run file, run number error!" << endl;
	}
	return nFiles;
}


int TRestDataBaseImpl::set_runnumber(int runnumber) {
	if (query_run(runnumber) == -1) {
		cout << "REST ERROR : run does not exist!" << endl;
		return -1;
	}
	else
	{
		fRunNumber = runnumber;
	}
	return 0;
}
int TRestDataBaseImpl::set_subrun(int subrun) {
	if (query_subrun(fRunNumber, subrun) == -1) {
		cout << "REST ERROR : subrun does not exist!" << endl;
		return -1;
	}
	else
	{
		fSubRunNumber = subrun;
	}
	return 0;
}
int TRestDataBaseImpl::set_type(string type) {
	query(conn, "update rest_runs set type=$1::run_type where run_id=$2::int and subrun_id=$3::int", type, fRunNumber, fSubRunNumber);
	return 0;
}
int TRestDataBaseImpl::set_user(string user) {
	query(conn, "update rest_runs set usr=$1::text where run_id=$2::int and subrun_id=$3::int", user, fRunNumber, fSubRunNumber);
	return 0;
}
int TRestDataBaseImpl::set_tag(string tag) {
	query(conn, "update rest_runs set tag=$1::text where run_id=$2::int and subrun_id=$3::int", tag, fRunNumber, fSubRunNumber);
	return 0;
}
int TRestDataBaseImpl::set_description(string description) {
	query(conn, "update rest_runs set description=$1::text where run_id=$2::int and subrun_id=$3::int", description, fRunNumber, fSubRunNumber);
	return 0;
}
int TRestDataBaseImpl::set_version(string version) {
	query(conn, "update rest_runs set version=$1::text where run_id=$2::int and subrun_id=$3::int", version, fRunNumber, fSubRunNumber);
	return 0;
}
int TRestDataBaseImpl::set_runstart(double starttime) {
	query(conn, "update rest_runs set run_start=$1::timestamp with time zone where run_id=$2::int and subrun_id=$3::int", ToDateTimeString(starttime), fRunNumber, fSubRunNumber);
	return 0;
}
int TRestDataBaseImpl::set_runend(double endtime) {
	query(conn, "update rest_runs set run_end=$1::timestamp with time zone where run_id=$2::int and subrun_id=$3::int", ToDateTimeString(endtime), fRunNumber, fSubRunNumber);
	return 0;
}
int TRestDataBaseImpl::set_fileinfo(int fileid, DataBaseFileInfo info) {
	query(conn, "update rest_files set file_size=$4::bigint, start_time=$5::timestamp with time zone, stop_time=$6::timestamp with time zone, event_rate=$7::real, sha1sum=$8::text, quality=$9::bool where run_id=$1::int and subrun_id=$2::int and file_id=$3::int",
		fRunNumber,
		fSubRunNumber,
		fileid,
		ToString(info.fileSize),
		ToDateTimeString(info.start),
		ToDateTimeString(info.stop),
		info.evtRate,
		(string)info.sha1sum,
		info.quality
	);

	return 0;

}
int TRestDataBaseImpl::set_fileinfo(string filename, DataBaseFileInfo info) {
	query(conn, "update rest_files set file_size=$4::bigint, start_time=$5::timestamp with time zone, stop_time=$6::timestamp with time zone, event_rate=$7::real, sha1sum=$8::text, quality=$9::bool where run_id=$1::int and subrun_id=$2::int and file_name=$3::text",
		fRunNumber,
		fSubRunNumber,
		ToAbsoluteName(filename),
		ToString(info.fileSize),
		ToDateTimeString(info.start),
		ToDateTimeString(info.stop),
		info.evtRate,
		(string)info.sha1sum,
		info.quality
	);
	return 0;
}


void TRestDataBaseImpl::test()
{
	auto result = pg::query(conn, "select * from rest_files", 1, 16, 1);
	for (size_t row = 0; row < result.size() && row < 10; ++row) {
		cout << result[row][3] << endl;
	}
}
void TRestDataBaseImpl::print(int runnumber, int subrun) {
	auto a = query(conn, "select run_id, run_start, run_end, type, description, usr from rest_runs where run_id=$1::int and subrun_id=$2::int", runnumber, subrun);
	if (a.size() == 1) {
		cout << "RUN " << runnumber << ", SubRun " << subrun << ":" << endl;
		cout << "-------------------" << endl;
		cout << "Time: " << ToDateTimeString(ToTime(a[0][1])) << " to " << ToDateTimeString(ToTime(a[0][2])) << endl;
		cout << "Type: " << a[0][3] << endl;
		cout << "Description: " << a[0][4] << endl;
		cout << "User: " << a[0][5] << endl;
		auto b = query_files(runnumber, subrun);
		cout << "------" << b.size() << " files ------" << endl;
		if (b.size() <= 10) {
			for (int i = 0; i < b.size(); i++) {
				cout << b[i] << endl;
			}
		}
		else
		{
			cout << "(showing first 5 files)" << endl;
			for (int i = 0; i < 5; i++) {
				cout << b[i] << endl;
			}
			cout << "..." << endl;
		}

	}
	else
	{
		cout << "REST ERROT : query failed!" << "(RUN " << runnumber << ", SubRun " << subrun << ")" << endl;
	}
}
void TRestDataBaseImpl::exec(string cmd) {
	query(conn, cmd);
}