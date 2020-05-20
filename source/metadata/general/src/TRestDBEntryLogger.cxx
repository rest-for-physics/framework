#include "TRestManager.h"
#include "TRestDataBase.h"
#include "TRestProcessRunner.h"
#include "TRestStringOutput.h"

#include "TRestDBEntryLogger.h"

ClassImp(TRestDBEntryLogger);

//______________________________________________________________________________
TRestDBEntryLogger::TRestDBEntryLogger() { Initialize(); }

void TRestDBEntryLogger::InitFromConfigFile() {
    // check environment
    if (gDataBase == NULL) {
        ferr << "REST database is not initailized!" << endl;
        abort();
    }

    //if (ToUpper(GetParameter("database", "")) != "PSQL") {
    //    ferr << "TRestDBEntryLogger: the used database by REST (" << any(gDataBase).type
    //         << ") is not supported" << endl;
    //    ferr << "hint: you need to switch to PSQL in your rml:" << endl;
    //    ferr << "<globals>" << endl;
    //    ferr << "  <parameter name=\"database\" value=\"PSQL\"/>" << endl;
    //    ferr << "</globals>" << endl;
    //    abort();
    //}

    // parameters
    fSkipIfNotEmpty = StringToBool(GetParameter("skipIfNotEmpty", "false"));
    fTextOpenCommand = GetParameter("editWith", "vim");

    fRun = fHostmgr->GetRunInfo();
    if (fRun->GetInputFileNames().size() != 0) {
        auto runs = gDataBase->search_run_with_file(fRun->GetInputFileName(0));
        if (runs.size() == 0) {
            AskForFilling(0);
        } else {
            auto runtype = gDataBase->query_run(runs[0]).type;
            if (ToUpper(runtype == "")) {
                AskForFilling(runs[0]);
            } else if (!fSkipIfNotEmpty) {
                AskForFilling(runs[0]);
            }
        }
    }
}

void TRestDBEntryLogger::AskForFilling(int run_id) {
    // get some information
    bool create = false;
    if (run_id == 0) {
        run_id = gDataBase->get_lastrun() + 1;
        create = true;
    }

    //auto infolist = gDataBase->exec(Form("select * from rest_metadata where run_id=%i", lastvalirrun));

    // search metadata, run number=this run, metadata type = any
    auto ids = gDataBase->search_data({(create ? run_id - 1 : run_id), ""});
    map<string, string> infolist;
    for (auto id : ids) {
        auto info = gDataBase->query_data(id);
        infolist[info.type] = info.value;
    }

    DBEntry entry = gDataBase->query_run(create ? run_id - 1 : run_id);

    string type = entry.type;
    string tag = entry.tag;
    string description = entry.description;
    string version = entry.version;

    string txtfilename = "/tmp/REST_" + REST_USER + "_tempDBLog.txt";
    ofstream ofs(txtfilename);
    ofs << (create ? "-- creating" : "-- updating") << " run in database, id: " << run_id
        << ", version: " << version << ". Delete this line to cancel." << endl;
    ofs << "type: " << type << endl;
    ofs << "tag: " << tag << endl;
    ofs << "description: " << description << endl;

    if (infolist.size() > 0) {
        for (auto info:infolist) {
            // i=0 --> run_id, we skip it
            ofs << info.first << ": " << info.second << endl;
        }
    } 
    ofs.flush();
    ofs.close();
    system((fTextOpenCommand + " " + txtfilename).c_str());

    // we read the file
    ifstream ifs(txtfilename);
    string s;
    bool valid = false;
    while (getline(ifs, s)) {
        if (!valid && s[0] != '-') {
            fout << "TRestDBEntryLogger: DataBase writting successful" << endl;
            ifs.close();
            return;
        } else {
            valid = true;
            continue;
        }

        auto infopair = Split(s, ":");
        if (infopair.size() != 2) {
            continue;
        }
        while (infopair[1][0] == ' ') {
            infopair[1].erase(0, 1);
        }

        if (infopair[0] == "type")
            type = infopair[1];
        else if (infopair[0] == "tag")
            tag = infopair[1];
        else if (infopair[0] == "description")
            description = infopair[1];
        else
            fMetainfo[infopair[0]] = infopair[1];
    }
    fRun->SetRunType(type);
    fRun->SetRunDescription(description);
    fRun->SetRunTag(tag);

    if (create) {
        // add entry
        DBEntry runentry;
        runentry.runNr = run_id;
        runentry.type = type;
        runentry.tag = tag;
        runentry.description = description;
        runentry.version = version;
        gDataBase->set_run(runentry);

       /* cout << Form(
                    "insert into rest_runs (run_id, type, tag, description, version) values (%i, '%s', '%s', "
                    "'%s', "
                    "'%s');",
                    run_id, type.c_str(), tag.c_str(), description.c_str(), version.c_str())
             << endl;
        gDataBase->exec(Form(
            "insert into rest_runs (run_id, type, tag, description, version) values (%i, '%s', '%s', '%s', "
            "'%s');",
            run_id, type.c_str(), tag.c_str(), description.c_str(), version.c_str()));*/

        // add run file
        vector<string> files = Vector_cast<TString, string>(fRun->GetInputFileNames());
        for (int i = 0; i < files.size(); i++) {
            int fileid = i;
            struct stat _stat;
            time_t filetime;
            long long filesize;
            if (stat(files[i].c_str(), &_stat) == 0) {
                filetime = _stat.st_ctime;  // creation time
                filesize = _stat.st_size;
            }

            DBFile afile;
            afile.filename = files[i];
            afile.fileSize = filesize;
            afile.start = filetime;
            gDataBase->set_runfile(run_id, files[i], afile);
            /*gDataBase->exec(
                Form("insert into rest_files (run_id,file_id,file_name,file_size,start_time) values "
                     "(%i,%i,'%s',%i,'%s');",
                     run_id, fileid, files[i].c_str(), filesize, ToDateTimeString(filetime).c_str()));*/
            if (i == 0) {
                //gDataBase->exec(Form("update rest_runs set run_start = '%s' where run_id=%i;",
                //                     ToDateTimeString(filetime).c_str(), run_id));
                runentry.tstart = filetime;
                gDataBase->set_run(runentry, true);
            }
        }

        // add run metadata
        //gDataBase->exec(Form("insert into rest_metadata (run_id) values (%i);", run_id));

    }

    for (auto iter : fMetainfo) {
        //gDataBase->exec(Form("update rest_metadata set %s = '%s' where run_id=%i;", iter.first.c_str(),
        //                     iter.second.c_str(), run_id));
        DBEntry dataentry;
        dataentry.runNr = run_id;
        dataentry.type = iter.first;
        dataentry.value = iter.second;
        gDataBase->set_data(dataentry, true);
    }

    fout << "TRestDBEntryLogger: DataBase writting successful" << endl;
}

void TRestDBEntryLogger::Initialize() {}

void TRestDBEntryLogger::PrintMetadata() {}
