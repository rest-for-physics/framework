#include <fstream>
#include <iostream>
#include <streambuf>
#include <string>

#include "TRestStringHelper.h"
#include "TRestTask.h"
using namespace std;

#include <TString.h>

string classifyString =
    "<<TRestDetectorSetup::fDetectorPressure>> & <<TRestDetectorSetup::fMeshVoltage>> & "
    "<<TRestDetectorSetup::fRunTag>>";

// Pressure, mesh voltage and mesh current from SlowControl
std::vector<string> metadataConditions = {};  // {"sc->fMinValues[0]> 3.8", "sc->fMinValues[1]> 100",
                                              //"sc->fMaxValues[2] < 0.1"};

//*******************************************************************************************************
//*** Description: This macro will print on screen a summary classifying the data according to the
//*** classifyBy data members hard coded inside this macro.
//***
//*** --------------
//*** The first and mandatory argument must provide a full data and pattern to filter the files given
//*** for classification. I.e. to add all the Hits files from run number 123 the first argument would be
//*** pattern = "/path/to/data/Run00123*Hits*root".
//***
//*** --------------
//*** Optionally we can specify the range of dates where the run files must be found.
//*** The format of the date must be provided as YYYY-MM-DD.
//***
//*** --------------
//*** Usage: restManager DataSummary /full/path/file_*pattern*.root [startDate] [endDate]
//*** --------------
//*** TODO: Make this macro more efficient by accessing to the SQL database created by restSQL.
//*** This could be done preserving the actual funcionality by identifying a particular format
//*** from the first string argument where we provide the SQL server access credentials.
//*** Then we could use both modes ROOT and SQL to crosscheck.
//***
//*******************************************************************************************************
Int_t REST_DataSummary(TString pattern, TString startDate = "", TString endDate = "") {
    vector<string> files = TRestTools::GetFilesMatchingPattern((string)pattern);

    std::tm t{};

    Double_t startTimeStamp = 0;
    if (startDate != "") {
        startTimeStamp = (Double_t)ToTime((string)startDate);
        cout.precision(12);
        cout << "Start time stamp : " << startTimeStamp << endl;
        cout.precision(4);
    }

    Double_t endTimeStamp = 0;
    if (endDate != "") {
        endTimeStamp = (Double_t)ToTime((string)endDate);
        cout.precision(12);
        cout << "End time stamp : " << endTimeStamp << endl;
        cout.precision(4);
    }

    Double_t runLength = 0;
    std::map<string, Double_t> timeInHours;
    std::map<string, Int_t> counts;
    std::map<string, Double_t> rate;
    for (int n = 0; n < files.size(); n++) {
        if (n % 25 == 0) cout << "File " << n << " of " << files.size() << endl;
        TRestRun* run = new TRestRun();
        // run->SetVerboseLevel(REST_Debug);
        run->SetHistoricMetadataSaving(false);

        run->OpenInputFile((string)files[n]);

        Double_t runStartTimeStamp = run->GetStartTimestamp();
        if (startTimeStamp > 0 && startTimeStamp > run->GetStartTimestamp()) {
            cout << "Skipping file : " << n << " --> " << TRestTools::GetPureFileName(files[n]) << endl;
            delete run;
            continue;
        }

        Double_t runEndTimeStamp = run->GetEndTimestamp();
        if (endTimeStamp > 0 && endTimeStamp < run->GetEndTimestamp()) {
            cout << "Skipping file : " << n << " --> " << TRestTools::GetPureFileName(files[n]) << endl;
            delete run;
            continue;
        }

        if (runEndTimeStamp - runStartTimeStamp <= 0) {
            warning << "Problem with start/end timestamps" << endl;
            warning << "Skipping file : " << TRestTools::GetPureFileName(files[n]) << endl;
            continue;
        }

        for (int m = 0; m < metadataConditions.size(); m++) {
            cout << metadataConditions[m] << endl;
            if (!run->EvaluateMetadataMember(metadataConditions[m])) {
                warning << "Condition : " << metadataConditions[m] << " not satisfied" << endl;
                warning << "Skipping file : " << TRestTools::GetPureFileName(files[n]) << endl;
                continue;
            }
        }

        string mapKey = run->ReplaceMetadataMembers(classifyString);

        // cout << "Using file : " << n << " --> " << TRestTools::GetPureFileName(files[n]) << endl;

        if (timeInHours.find(mapKey) == timeInHours.end())
            timeInHours[mapKey] = (runEndTimeStamp - runStartTimeStamp) / 3600.;
        else
            timeInHours[mapKey] += (runEndTimeStamp - runStartTimeStamp) / 3600.;

        if (counts.find(mapKey) == counts.end())
            counts[mapKey] = run->GetEntries();
        else
            counts[mapKey] += run->GetEntries();

        delete run;
    }

    cout.precision(3);
    /// This header first columns correspond with the classifyString
    cout << "Pressure (bar) & V$_{mesh}$(V) & Run type  & Time (Hours) & #Events & Rate (Hz) \\\\" << endl;
    for (auto const& x : counts) {
        cout << x.first << " & " << timeInHours[x.first] << " & " << x.second << " & "
             << (Double_t)x.second / timeInHours[x.first] / 3600. << "\\\\" << endl;
    }

    return 0;
}
