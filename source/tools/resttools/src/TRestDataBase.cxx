#include "TRestDataBase.h"
#include <errno.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "TClass.h"
#include "TRestStringHelper.h"
#include "TRestTools.h"
#include "TSystem.h"

//////////////////////////////////////////////////////////////////////////
/// Interface class of REST database accessibility.
///
/// This class defines methods only. its inhert class will do the database work.
///
/// To startup, first install the package: restDataBaseImpl. It requires pgsql
/// database installed. Call TRestDataBase::instantiate() to get the functional
/// instance of TRestDataBase. If the package is not installed, a null ptr will
/// be returned.
///
/// Information stored in database:
/// * run id: The index of the run. Run is usually generated from daq software.
/// It means a pieroid of time when detector is running with constant
/// configuration.
/// * subrun id: The sub index of the run. Data taking run is asigned subrun id
/// = 0, analysis run is recorded as subrun and accumulates the subrun id.
/// * user: User name of datataking/analysis (sub)run. Generated automatically.
/// * version: REST version of this (sub)run. Generated automatically.
/// * tag: A tag which makes the run easier to be found. We need to set it
/// manually.
/// * type: Enumerator of run type. Includes 'SW_DEBUG', 'HW_DEBUG',
/// 'SIMULATION', 'CALIBRATION', 'PHYSICS_DBD', 'ANALYSIS' and 'OTHER'. By
/// default data taking has type: HW_DEBUG, restG4 has type: SIMULATION,
/// restManager has type: ANALYSIS.
/// * description: Detailed description string. Write the run config here.
/// * file names: a list of run output files. Added automatically.
///
/// How to use:
/// `TRestDataBase* db = TRestDataBase::instantiate()`
///
/// 1. To start a new run and add files in it:
/// \code
/// int runId = db->newrun();
/// db->new_runfile("abc.graw");
/// \endcode
///
/// 2. To append files in an existing run:
/// \code
/// int runId = db->getlastrun();
/// db->set_runnumber(runId);
/// db->new_runfile("abc.root");
/// \endcode
///
/// 3. To view the run info:
/// \code
/// int runId = deb->getlastrun();
/// db->print(runId);
/// \endcode
///
/// 4. To execute custom command. Suppose we are using pgsql database:
/// \code
/// db->exec("select * into rest_files_bk from rest_files");
/// \endcode
TRestDataBase* TRestDataBase::instantiate(string name) {
    // vector<string> list = TRestTools::GetListOfRESTLibraries();
    // for (unsigned int n = 0; n < list.size(); n++) {
    //    gSystem->Load(list[n].c_str());
    //}
    if (name != "") {
        TClass* c = TClass::GetClass(("TRestDataBase" + name).c_str());
        if (c != NULL)  // this means we have the package installed
        {
            return (TRestDataBase*)c->New();
        } else {
            cout << "warning! unrecognized TRestDataBase implementation: \"" << name << "\"" << endl;
            return new TRestDataBase();
        }
    }

    return new TRestDataBase();
}

DBFile::DBFile(string _filename) {
    filename = _filename;
    auto _fullname = TRestTools::ToAbsoluteName(filename);

    struct stat buf;
    int result = stat(_fullname.c_str(), &buf);

    if (result != 0) {
        cout << "Failed to load file \"" << _fullname << "\"!" << endl;
        fileSize = 0;
        evtRate = 0;
        quality = true;
        start = 0;
        stop = 0;
        for (int i = 0; i < 41; i++) {
            sha1sum[i] = 0;
        }
    } else {
        fileSize = buf.st_size;
        evtRate = 0;
        quality = true;
        start = buf.st_ctime;
        stop = buf.st_mtime;
        string sha1result = TRestTools::Execute("sha1sum " + _fullname);
        string sha1 = Split(sha1result, " ")[0];
        if (sha1.size() == 40) {
            for (int i = 0; i < 40; i++) {
                sha1sum[i] = sha1[i];
            }
            sha1sum[40] = 0;
        }
    }
}

void DBFile::Print() {
    cout << "----DBFile struct----" << endl;
    cout << "size: " << fileSize << endl;
    cout << "event rate: " << evtRate << endl;
    cout << "sha1sum: " << sha1sum << endl;
    cout << "quality: " << quality << endl;
    cout << "start time: " << ToDateTimeString(start) << endl;
    cout << "stop time: " << ToDateTimeString(stop) << endl;
}

TRestDataBase::TRestDataBase() {
    auto url = getenv("REST_DBURL");
    if (url != NULL) {
        fConnectionString = url;
    }
}

int TRestDataBase::get_lastrun() {
    int runNr;
    string runFilename = getenv("REST_PATH") + (string) "/runNumber";
    if (!TRestTools::fileExists(runFilename)) {
        if (TRestTools::isPathWritable(getenv("REST_PATH"))) {
            TRestTools::Execute("echo 1 > " + runFilename);
            runNr = 1;
        }
    } else {
        ifstream ifs(runFilename);
        ifs >> runNr;
    }
    return runNr - 1;
}

int TRestDataBase::add_run(DBEntry info) {
    int newRunNr;
    if (info.id == 0) {
        newRunNr = get_lastrun() + 1;
    } else if (info.id > 0) {
        newRunNr = info.id;
    } else {
        return -1;
    }

    string runFilename = getenv("REST_PATH") + (string) "/runNumber";
    if (TRestTools::isPathWritable(getenv("REST_PATH"))) {
        TRestTools::Execute("echo " + ToString(newRunNr + 1) + " > " + runFilename);
    } else {
        cout << "REST WARNING: runNumber file not writable. auto run number "
                "increment is disabled"
             << endl;
    }

    set_run_runinfo(newRunNr, info);

    return newRunNr;
}