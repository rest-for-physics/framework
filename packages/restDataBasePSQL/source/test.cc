#include "TRestDataBasePSQL.hh"
#include "TRestReflector.h"
#include "TRestStringHelper.h"

void TRestDataBasePSQL::test() {
    exec("select * from rest_files where run_id < 10;");

    //////////
    ///
    cout << "resetting database! causion!" << endl;
    string path = getenv("REST_PATH");
    system(("psql -h localhost -d p3_daq -U p3daq -f " + REST_PATH + "/data/pgsql/setup.sql").c_str());

    ifstream ifs(REST_PATH + "/data/pgsql/runs.txt");
    if (!ifs.is_open()) {
        cout << "failed to open run file" << endl;
    }
    string s;
    while (getline(ifs, s)) {
        vector<string> items = Split(s, "\t", true);
        if (items.size() != 14) {
            cout << "error raw!!!" << endl;
            cout << s << endl;
            getchar();
        }
        for (int i = 0; i < items.size(); i++) {
            if (items[i].size() > 2 && items[i][0] == '\"' && items[i][items[i].size() - 1] == '\"') {
                items[i].erase(items[i].begin());
                items[i].erase(items[i].end() - 1);
            }
        }

        // runid
        int runid = atoi(items[0].c_str());
        if (runid <= 0) continue;

        int fileid = atoi(items[1].c_str());
        string runtime = items[2];
        string filename = items[3];
        string MMs_activestr = items[5];
        int Gas_driftvoltage = items[4] == "" ? 0 : atoi(items[4].c_str());
        string MMs_HV_Meshstr = items[6];
        string MMs_HV_IRstr = items[7];
        string MMs_HV_ERstr = items[8];
        string MMs_HV_DRstr = items[9];
        string DAQ_threshold = items[10];
        int DAQ_samplingrate = items[11] == "" ? 5 : atoi(items[11].c_str());
        int DAQ_triggerdalay = items[12] == "" ? 150 : atoi(items[12].c_str());
        string description = items[13];

        // file time, file size
        struct stat _stat;
        time_t filetime;
        long long filesize;
        if (stat(filename.c_str(), &_stat) == 0) {
            filetime = _stat.st_ctime;
            filesize = _stat.st_size;
        }

        if (fileid == 0) {
            // we add the run first
            cout << Form("insert into rest_runs (run_id, run_start) values (%i, '%s');", runid,
                         runtime.c_str())
                 << endl;
            exec(
                Form("insert into rest_runs (run_id, run_start) values (%i, '%s');", runid, runtime.c_str()));

            if (MMs_activestr != "") {
                // set MMs active list
                string MMs_layout = "Prototype_7MM";
                vector<int> MMs_idlist{0, 2, 3, 4, 6, 8, 9};
                vector<int> MMs_active;
                int MMs_nactive = 0;
                for (auto MM : MMs_idlist) {
                    if (MMs_activestr.find(ToString(MM)) != -1) {
                        MMs_active.push_back(1);
                        MMs_nactive++;
                    } else {
                        MMs_active.push_back(0);
                    }
                }

                vector<string> MMs_HV_Meshstrs = Split(MMs_HV_Meshstr, ",", false, true);
                vector<int> MMs_HV_Mesh(MMs_idlist.size());
                vector<string> MMs_HV_IRstrs = Split(MMs_HV_IRstr, ",", false, true);
                vector<int> MMs_HV_IR(MMs_idlist.size());
                vector<string> MMs_HV_ERstrs = Split(MMs_HV_ERstr, ",", false, true);
                vector<int> MMs_HV_ER(MMs_idlist.size());
                vector<string> MMs_HV_DRstrs = Split(MMs_HV_DRstr, ",", false, true);
                vector<int> MMs_HV_DR(MMs_idlist.size());
                for (int i = 0; i < MMs_active.size(); i++) {
                    // how many MMs are active before this(include this)
                    int indexinstrs = count(MMs_active.begin(), MMs_active.begin() + i + 1, 1);
                    if (indexinstrs == 0) continue;

                    if (MMs_HV_Meshstrs.size() == 0)
                        MMs_HV_Mesh[i] = 0;
                    else if (MMs_HV_Meshstrs.size() > 1 && MMs_HV_Meshstrs.size() != MMs_nactive)
                        cout << "error record in MMs mesh voltage! run: " << runid << endl;
                    else if (MMs_HV_Meshstrs.size() == 1)
                        if (MMs_active[i] == 1)
                            MMs_HV_Mesh[i] = atoi(MMs_HV_Meshstrs[0].c_str());
                        else
                            MMs_HV_Mesh[i] = 0;
                    else
                        MMs_HV_Mesh[i] = atoi(MMs_HV_Meshstrs[indexinstrs - 1].c_str());

                    if (MMs_HV_IRstrs.size() == 0)
                        MMs_HV_IR[i] = 0;
                    else if (MMs_HV_IRstrs.size() > 1 && MMs_HV_IRstrs.size() != MMs_nactive)
                        cout << "error record in MMs IR voltage! run: " << runid << endl;
                    else if (MMs_HV_IRstrs.size() == 1)
                        if (MMs_active[i] == 1)
                            MMs_HV_IR[i] = atoi(MMs_HV_IRstrs[0].c_str());
                        else
                            MMs_HV_IR[i] = 0;
                    else
                        MMs_HV_IR[i] = atoi(MMs_HV_IRstrs[indexinstrs - 1].c_str());

                    if (MMs_HV_ERstrs.size() == 0)
                        MMs_HV_ER[i] = 0;
                    else if (MMs_HV_ERstrs.size() > 1 && MMs_HV_ERstrs.size() != MMs_nactive)
                        cout << "error record in MMs ER voltage! run: " << runid << endl;
                    else if (MMs_HV_ERstrs.size() == 1)
                        if (MMs_active[i] == 1)
                            MMs_HV_ER[i] = atoi(MMs_HV_ERstrs[0].c_str());
                        else
                            MMs_HV_ER[i] = 0;
                    else
                        MMs_HV_ER[i] = atoi(MMs_HV_ERstrs[indexinstrs - 1].c_str());

                    if (MMs_HV_DRstrs.size() == 0)
                        MMs_HV_DR[i] = 120;
                    else if (MMs_HV_DRstrs.size() > 1 && MMs_HV_DRstrs.size() != MMs_nactive)
                        cout << "error record in MMs DR! run: " << runid << endl;
                    else if (MMs_HV_DRstrs.size() == 1)
                        if (MMs_active[i] == 1)
                            MMs_HV_DR[i] = atoi(MMs_HV_DRstrs[0].c_str());
                        else
                            MMs_HV_DR[i] = 120;
                    else
                        MMs_HV_DR[i] = atoi(MMs_HV_DRstrs[indexinstrs - 1].c_str());
                }

                // description
                description = Replace(description, "'", "''");
                description = Replace(description, "\r", "");
                if (description.size() > 2 && description[0] == '\"' &&
                    description[description.size() - 1] == '\"') {
                    description.erase(description.begin());
                    description.erase(description.end() - 1);
                }

                // update table!
                exec(Form("update rest_runs set description = '%s' where run_id=%i;", description.c_str(),
                          runid));
                exec(Form("update rest_runs set type = 'HW_DEBUG' where run_id=%i;", runid));

                exec(Form("insert into rest_metadata (run_id, MMs_layout) values (%i, '%s');", runid,
                          MMs_layout.c_str()));
                exec(Form("update rest_metadata set MMs_id = '%s' where run_id=%i;",
                          ToString(any(MMs_idlist)).c_str(), runid));
                exec(Form("update rest_metadata set MMs_active = '%s' where run_id=%i;",
                          ToString(any(MMs_active)).c_str(), runid));
                exec(Form("update rest_metadata set DAQ_HVmesh = '%s' where run_id=%i;",
                          ToString(any(MMs_HV_Mesh)).c_str(), runid));
                exec(Form("update rest_metadata set DAQ_HVinternalrim = '%s' where run_id=%i;",
                          ToString(any(MMs_HV_IR)).c_str(), runid));
                exec(Form("update rest_metadata set DAQ_HVexternalrim = '%s' where run_id=%i;",
                          ToString(any(MMs_HV_ER)).c_str(), runid));
                exec(Form("update rest_metadata set MMs_board = '{2,1,2,1,0,3,0}' where run_id=%i;",
                          runid));
                exec(Form("update rest_metadata set MMs_slot = '{3,12,12,3,12,6,3}' where run_id=%i;",
                          runid));
                exec(Form("update rest_metadata set DAQ_dynamicrange = '%s' where run_id=%i;",
                          ToString(any(MMs_HV_DR)).c_str(), runid));

                exec(Form("update rest_metadata set DAQ_threshold = '%s' where run_id=%i;",
                          DAQ_threshold.c_str(), runid));
                exec(Form("update rest_metadata set DAQ_samplingrate = %i where run_id=%i;", DAQ_samplingrate,
                          runid));
                exec(Form("update rest_metadata set DAQ_triggerdelay = %i where run_id=%i;", DAQ_triggerdalay,
                          runid));
                exec(Form("update rest_metadata set Gas_driftvoltage = %i where run_id=%i;", Gas_driftvoltage,
                          runid));
            }
        }

        // we add file entries
        exec(
            Form("insert into rest_files (run_id,file_id,file_name,file_size,start_time) values "
                 "(%i,%i,'%s',%i,'%s');",
                 runid, fileid, filename.c_str(), filesize, ToDateTimeString(filetime).c_str()));

        // usleep(100000);
    }
    ifs.close();
}