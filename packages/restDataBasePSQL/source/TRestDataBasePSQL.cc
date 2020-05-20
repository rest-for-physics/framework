#include <map>
#include <set>

#include "TRestDataBasePSQL.hh"

#include "TRestStringHelper.h"
#include "TRestStringOutput.h"
#include "pg.hh"

using namespace pg;
std::shared_ptr<PGconn> conn;

TRestDataBasePSQL::TRestDataBasePSQL() { Initialize(); }

TRestDataBasePSQL::~TRestDataBasePSQL() {}

void TRestDataBasePSQL::Initialize() {
    TRestDataBase::Initialize();
    TString url = getenv("REST_DBURL");
    if (url == "") {
        url = "postgresql://p3daq:ilovepanda@localhost/p3_daq";
    }
    try {
        conn = pg::connect((string)url);
    } catch (std::runtime_error) {
        cout << "REST ERROR!! unable to connect the database:" << endl;
        cout << COLOR_BOLDRED << url << COLOR_RESET << endl;
        cout << "If this url is not you want, change it by setting environmental "
                "variable \"REST_DBURL\" in your operation system."
             << endl;
        cout << "If you don't want to use database, set parameter \"runNumber\" to "
                "\"-1\" instead of \"auto\" in the section \"TRestRun\""
             << endl;
        exit(1);
    }
}

void TRestDataBasePSQL::print(string cmd) {
    //if (ToUpper(cmd).find("DROP") != -1) {
    //    cout << "ERROR!!!FORBIDDEN OPERATION!!!" << endl;
    //    return;
    //}

    //if (cmd.find("select") == -1 && cmd.find(" ") != -1) {
    //    cout << "WARNING!!!CRITICAL OPERATION!!!" << endl;
    //    cout << "Type \"continue\" to continue if you are sure!" << endl;
    //    string str;
    //    cin >> str;
    //    if (str != "continue") {
    //        cout << "(aborted)" << endl;
    //        return;
    //    } else {
    //        cout << "(confirmed)" << endl;
    //    }
    //}

    auto result = query(conn, cmd);

    if (result[-1].size() > 0) {
        vector<int> maxlength(result[-1].size());

        for (int row = -1; row < result.size(); row++) {
            for (int column = 0; column < result[row].size(); column++) {
                if (result[row][column].size() > maxlength[column])
                    maxlength[column] = result[row][column].size();
            }
        }

        for (int row = -1; row < result.size(); row++) {
            for (int column = 0; column < result[row].size(); column++) {
                string item = result[row][column];
                int n = (maxlength[column] - item.size());

                cout << result[row][column] << string(n, ' ');
                if (column == result[row].size() - 1) {
                    cout << endl;
                } else {
                    cout << " | ";
                }
            }
        }
    }
}

DBTable TRestDataBasePSQL::exec(string cmd) {
    auto qresult = query(conn, cmd);
    DBTable result;

    if (qresult.rows() > 0) {
        for (int column = 0; column < qresult[-1].size(); column++) {
            result.headerline.push_back(qresult[-1][column]);
        }
        for (int row = 0; row < qresult.rows(); row++) {
            result.push_back(vector<string>());
            for (int column = 0; column < qresult[row].size(); column++) {
                result[row].push_back(qresult[row][column]);
            }
        }
    }
    return result;
}


int TRestDataBasePSQL::query_run(int runnumber) {
    auto q = pg::query(conn, Form("select run_id from rest_runs where run_id=%d;", runnumber));
    if (q.size() > 0) return runnumber;
    return 0;
}

vector<string> TRestDataBasePSQL::query_run_files(int runnumber) { 
    vector<string> result;
    auto q = pg::query(conn, Form("select file_name from rest_files where run_id=%d;", runnumber));
    for (int i = 0; i < q.rows(); i++) {
        result.push_back(q[i][0]);
    }
    return result;
}

string TRestDataBasePSQL::query_run_filepattern(int runnumber) {
    vector<string> files = query_run_files(runnumber);
    if (files.size() == 0) return "";
    if (files.size() == 1) return files[0];

    // we find the pattern of the files
    for (int i = 1; i < files.size(); i++) {
        if (files[i].size() != files[i - 1].size()) {
            cout << "error! files in run " << runnumber << " has different length!" << endl;
            cout << "cannot evaluate file pattern, returning blank!" << endl;
            return "";
        }
    }

    string pattern = files[0];
    for (int i = 0; i < pattern.size(); i++) {
        for (int j = 1; j < files.size(); j++) {
            if (files[j][i] != files[j - 1][i]) {
                pattern[i] = '*';
                break;
            }
        }
    }
    return pattern;
}

DBEntry TRestDataBasePSQL::query_run_info(int runnumber) {
    DBEntry info;
    auto q = pg::query(conn, Form("select run_id,type,tag,description,version from rest_runs where run_id=%d;", runnumber));
    if (q.rows() == 1) {
        info.runNr = atoi(q[0][0].c_str());
        info.type = q[0][1];
        info.tag = q[0][2];
        info.description = q[0][3];
        info.version = q[0][4];
    }
    return info;
}

DBFile TRestDataBasePSQL::query_run_info_files(int runnumber, int fileid) {
    DBFile info;
    auto q = pg::query(conn,
                       "select file_name,file_size,sha1sum,quality,start_time,stop_time from rest_files "
                       "where run_id=%d and file_id=%d;",
                       runnumber, fileid);
    if (q.rows() == 1) {
        info.filename = q[0][0];
        info.fileSize = atoi(q[0][1].c_str());
        memcpy(info.sha1sum, &q[0][2][0], q[0][2].size() == 40 ? 41 : 0);
        info.quality = (q[0][3] == "t");
        info.start = ToTime(q[0][4]);
        info.stop = ToTime(q[0][5]);
    }
    return info;
}

double TRestDataBasePSQL::query_run_start(int runnumber) {
    auto q = pg::query(conn, Form("select run_start from rest_runs where run_id=%d;", runnumber));
    if (q.rows() == 1) {
        return ToTime(q[0][1]);
    }
    return -1;
}

double TRestDataBasePSQL::query_run_end(int runnumber) {
    auto q = pg::query(conn, Form("select run_end from rest_runs where run_id=%d;", runnumber));
    if (q.rows() == 1) {
        return ToTime(q[0][1]);
    }
    return -1;
}


vector<int> TRestDataBasePSQL::search_run_with_file(string filepattern) {
    vector<int> result;
    string lsoutput = TRestTools::Execute(Form("ls %s", filepattern.c_str()));
    if (lsoutput == "") return result;

    vector<string> files = Split(lsoutput, "\n");
    set<int> runids;
    for (int i = 0; i < files.size(); i++) {
        auto q =
            pg::query(conn, Form("select run_id from rest_files where file_name='%s';", files[i].c_str()));
        if (q.rows() >= 1) {
            runids.insert(atoi(q[0][0].c_str()));        
        }
    }
    
    for (auto id : runids) {
        result.push_back(id);
    }
    return result;
}

vector<int> TRestDataBasePSQL::search_run_with_timeperiod(time_t t1, time_t t2) {
    vector<int> result;
    auto q = pg::query(conn, Form("select run_id from rest_runs where run_start>'%s' and run_start<'%s';",
                                  ToDateTimeString(t1).c_str(), ToDateTimeString(t2).c_str()));

    for (int i = 0; i < q.rows(); i++) {
        result.push_back(atoi(q[i][0].c_str()));
    }
}



int TRestDataBasePSQL::get_lastrun() {
    return StringToInteger(pg::query(conn, Form("select max(run_id) from rest_runs;"))[0][0]);
}

/// add a new run, with run info as struct DBEntry. returns the added run id
int TRestDataBasePSQL::add_run(DBEntry info) {
    
    return 0;
}


int TRestDataBasePSQL::query_metadata(int id) {
    return 0; 
}


string TRestDataBasePSQL::query_metadata_value(int id) { return TRestDataBase::query_metadata_value(id); }


string TRestDataBasePSQL::query_metadata_valuefile(int id, string name) { 
    return TRestDataBase::query_metadata_valuefile(id, name);
}


DBEntry TRestDataBasePSQL::query_metadata_info(int id) { 
    return TRestDataBase::query_metadata_info(id); }


vector<int> TRestDataBasePSQL::search_metadata_with_value(string url) { 
    return TRestDataBase::search_metadata_with_value(url);
}


vector<int> TRestDataBasePSQL::search_metadata_with_info(DBEntry info) { 
    return TRestDataBase::search_metadata_with_info(info);
}


int TRestDataBasePSQL::get_lastmetadata() { 
    return 0; 
}


int TRestDataBasePSQL::add_metadata(DBEntry info, string value) {
    return 0;
}


int TRestDataBasePSQL::update_metadata(int id, DBEntry info) {
    return 0; 
}
