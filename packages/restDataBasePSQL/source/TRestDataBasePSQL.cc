#include "TRestDataBasePSQL.hh"

#include <map>
#include <set>

#include "TRestStringHelper.h"
#include "TRestStringOutput.h"
#include "pg.hh"

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

int TRestDataBasePSQL::AddDataEntryUnique(DBEntry entry) {
    auto search = TRestDataBase::search_data(entry);
    if (search.size() > 0) {
        return search[0];
    } else {
        fDataEntries.push_back(entry);
        return fDataEntries.size() - 1;
    }

}

void TRestDataBasePSQL::print(string cmd) {
    // if (ToUpper(cmd).find("DROP") != -1) {
    //    cout << "ERROR!!!FORBIDDEN OPERATION!!!" << endl;
    //    return;
    //}

    // if (cmd.find("select") == -1 && cmd.find(" ") != -1) {
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

    auto result = pg::query(conn, cmd);

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
    auto qresult = pg::query(conn, cmd);
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

DBEntry TRestDataBasePSQL::query_run(int runnumber) {
    DBEntry info;
    auto q = exec(
        Form("select run_id,type,tag,description,version,run_start,run_end from rest_runs where run_id=%d;",
             runnumber));
    if (q.rows() == 1) {
        info.runNr = atoi(q[0][0].c_str());
        info.type = q[0][1];
        info.tag = q[0][2];
        info.description = q[0][3];
        info.version = q[0][4];
        info.tstart = ToTime(q[0][5]);
        info.tend = ToTime(q[0][6]);

        // extract file pattern
        vector<string> files = Vector_cast<DBFile, string>(query_run_files(runnumber));
        if (files.size() == 0) {
            info.value = "";
        } else if (files.size() == 1) {
            info.value = files[0];
        } else {
            // we find the pattern of the files
            string pattern = files[0];
            for (int i = 0; i < pattern.size(); i++) {
                for (int j = 1; j < files.size(); j++) {
                    if (files[j][i] != files[j - 1][i]) {
                        pattern[i] = '*';
                        break;
                    }
                }
            }
            info.value = pattern;
        }
    }
    return info;
}

vector<DBFile> TRestDataBasePSQL::query_run_files(int runnumber) {
    vector<DBFile> result;
    auto q = exec(Form("select file_name from rest_files where run_id=%d;", runnumber));
    for (int i = 0; i < q.rows(); i++) {
        result.push_back(DBFile(q[i][0]));
    }
    return result;
}

DBFile TRestDataBasePSQL::query_run_file(int runnumber, int fileid) {
    DBFile info;
    auto q =
        exec(Form("select file_name,file_size,sha1sum,quality,start_time,stop_time from rest_files "
                  "where run_id=%d and file_id=%d;",
                  runnumber, fileid));
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

vector<int> TRestDataBasePSQL::search_run_with_file(string filepattern) {
    vector<int> result;
    string lsoutput = TRestTools::Execute(Form("ls %s", filepattern.c_str()));
    if (lsoutput == "") return result;

    vector<string> files = Split(lsoutput, "\n");
    set<int> runids;
    for (int i = 0; i < files.size(); i++) {
        auto q = exec(Form("select run_id from rest_files where file_name='%s';", files[i].c_str()));
        if (q.rows() >= 1) {
            runids.insert(atoi(q[0][0].c_str()));
        }
    }

    for (auto id : runids) {
        result.push_back(id);
    }
    return result;
}

vector<int> TRestDataBasePSQL::search_run(DBEntry info) {
    vector<int> result;
    //////////////

    return result;
}

int TRestDataBasePSQL::get_lastrun() {
    return StringToInteger(exec(Form("select max(run_id) from rest_runs;"))[0][0]);
}

/// add a new run, with run info as struct DBEntry. returns the added run id
int TRestDataBasePSQL::set_run(DBEntry info, bool overwrite) {
    int last_run = get_lastrun();
    int runid = 0;
    if (info.runNr > last_run + 1) {
        ferr << "cannot add run with number: " << info.runNr << endl;
        ferr << "run number cannot be incontinuous" << endl;
        return -1;
    } else if (info.runNr == 0 || info.runNr == last_run + 1) {
        runid = last_run + 1;
        exec(Form("insert into rest_runs (run_id) values (%i);", runid));

    } else if (info.runNr > 0 && overwrite) {
        runid = info.runNr;
    }

    if (runid > 0) {
        exec(Form("update rest_runs set description = '%s' where run_id=%i;", info.description.c_str(),
                  runid));
        exec(Form("update rest_runs set type = '%s' where run_id=%i;", info.type.c_str(), runid));
        exec(Form("update rest_runs set tag = '%s' where run_id=%i;", info.tag.c_str(), runid));
        exec(Form("update rest_runs set version = '%s' where run_id=%i;", info.version.c_str(), runid));
        exec(Form("update rest_runs set run_start = '%s' where run_id=%i;",
                  ToDateTimeString(info.tstart).c_str(), runid));
    }
    return runid;
}

int TRestDataBasePSQL::set_runfile(int runnumber, string filename) {
    if (!TRestTools::fileExists(filename)) {
        return -1;
    }
    if (TRestTools::isRootFile(filename)) {
        // maybe in future we will keep root file together as run file
        return -1;
    }

    filename = TRestTools::ToAbsoluteName(filename);

    bool create = true;
    int fileid = -1;
    auto files = query_run_files(runnumber);
    for (int i = 0; i < files.size(); i++) {
        if (files[i].filename == filename) {
            warning << "file: " << filename << " already exists in run: " << runnumber << endl;
            warning << "file info updated" << endl;
            create = false;
            fileid = i;
            break;
        }
    }
    if (fileid == -1) fileid = files.size();

    time_t filetime;
    long long filesize;
    struct stat _stat;
    if (stat(filename.c_str(), &_stat) == 0) {
        filetime = _stat.st_ctime;  // creation time
        filesize = _stat.st_size;
    }

    if (create) {
        exec(
            Form("insert into rest_files (run_id,file_id,file_name,file_size,start_time) values "
                 "(%i,%i,'%s',%i,'%s');",
                 runnumber, fileid, filename.c_str(), filesize, ToDateTimeString(filetime).c_str()));
    } else {
        exec(Form("update rest_files set file_size = '%i' where run_id=%i and file_id=%i;", filesize,
                  runnumber, fileid));
        exec(Form("update rest_files set start_time = '%s' where run_id=%i and file_id=%i;",
                  ToDateTimeString(filetime).c_str(), runnumber, fileid));
    }

    return fileid;
}

DBEntry TRestDataBasePSQL::query_data(int id) { return TRestDataBase::query_data(id); }

DBFile TRestDataBasePSQL::wrap_data(DBEntry entry, string name) {
    return TRestDataBase::wrap_data(entry, name);
}

vector<int> TRestDataBasePSQL::search_data(DBEntry info) {
    vector<int> result;

    if (info.runNr == 0) {
        // In future we shall return default data setting for our detector.
        return TRestDataBase::search_data(info);

    } else if (info.runNr > 0) {
        if (info.type == "GAS_SERVER") {
            return TRestDataBase::search_data(info);
        } else if (info.type == "META_RML") {
            // we query database and collect information of the metadata
            //...

            return TRestDataBase::search_data(info);

        } else if (info.type == "DB_COLUMN") {
            // we list all the database entry of this run
            DBTable table;

            if (info.tstart == 0 || info.tend == 0) {
                if (info.tag == "") {
                    table = exec(Form("select * from rest_metadata where run_id=%i", info.runNr));

                } else {
                    table = exec(
                        Form("select %s from rest_metadata where run_id=%i", info.tag.c_str(), info.runNr));
                }
            } else {
                // In future we can query slow control data from here
            }

            for (int i = 0; i < table.columns(); i++) {
                DBEntry entry;
                entry.runNr = info.runNr;
                entry.type = info.type;
                entry.tag = table.headerline[i];
                entry.value = table[0][i];

                result.push_back(AddDataEntryUnique(entry));
            }
        }
    }

    return result;
}

int TRestDataBasePSQL::get_lastdata() { return fDataEntries.size() - 1; }

int TRestDataBasePSQL::set_data(DBEntry info, bool overwrite) {
    bool exists = exec(Form("select run_id from rest_metadata where run_id=%i", info.runNr)).rows();
    if (exists) {
        if (overwrite) {
            AddDataEntryUnique(info);
            cout << Form("update rest_metadata set %s = '%s' where run_id=%i;", info.tag.c_str(),
                         info.value.c_str(), info.runNr)
                 << endl;
            exec(Form("update rest_metadata set %s = '%s' where run_id=%i;", info.tag.c_str(),
                      info.value.c_str(), info.runNr));
        }
    } else {
        fDataEntries.push_back(info);
        cout << Form("insert into rest_metadata (run_id) values (%i);", info.runNr) << endl;
        exec(Form("insert into rest_metadata (run_id) values (%i);", info.runNr));
        exec(Form("update rest_metadata set %s = '%s' where run_id=%i;", info.tag.c_str(), info.value.c_str(),
                  info.runNr));
    }
}
