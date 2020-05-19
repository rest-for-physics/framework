/*************************************************************************
 * This file is part of the REST software framework.                     *
 *                                                                       *
 * Copyright (C) 2016 GIFNA/TREX (University of Zaragoza)                *
 * For more information see http://gifna.unizar.es/trex                  *
 *                                                                       *
 * REST is free software: you can redistribute it and/or modify          *
 * it under the terms of the GNU General Public License as published by  *
 * the Free Software Foundation, either version 3 of the License, or     *
 * (at your option) any later version.                                   *
 *                                                                       *
 * REST is distributed in the hope that it will be useful,               *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the          *
 * GNU General Public License for more details.                          *
 *                                                                       *
 * You should have a copy of the GNU General Public License along with   *
 * REST in $REST_PATH/LICENSE.                                           *
 * If not, see http://www.gnu.org/licenses/.                             *
 * For the list of contributors see $REST_PATH/CREDITS.                  *
 *************************************************************************/

//////////////////////////////////////////////////////////////////////////
///
/// It will open a connection to an existing SQL database and during
/// the process initialization it will dump the required fields into a
/// temporary buffer internally managed.
///
/// The database entries will be correlated with the event timestamp, and
/// the corresponding database field value will be assigned to the
/// corresponding event entry at the analysis tree. Therefore, it is required
/// that the SQL database contains a field entry named `timestamp`, which
/// name is for the moment hardcoded inside.
///
/// Linear interpolation has been considered when retrieving the fields
/// from the database for intermediate timestamps.
///
/// We must specify the fields from the SQL database that we wish to be
/// extracted, and associate it to a given *future* name in the analysis tree.
/// For each field we want to extract we need to defined a new `<dbEntry` key
/// specifying the correspoding database field, as `sqlName`, and the
/// corresponging new name it will take at the analysis tree `anaName`.
///
/// The following example shows a RML process definition as it would be
/// written inside a TRestProcessRunner chain.
///
/// ```
/// <addProcess type="TRestMySQLToAnalysisProcess" name="sc" value="ON" verboseLevel="warning"
/// observable="all" >
/// 	<parameter name="server" value="localhost" />
/// 	<parameter name="database" value="dbname" />
/// 	<parameter name="user" value="dbuser" />
/// 	<parameter name="password" value="%i8X$s±^7K1" />
/// 	<parameter name="table" value="tablename" />
///
/// 	<dbEntry sqlName="db_field_1" anaName="Pressure" />
/// 	<dbEntry sqlName="db_field_2" anaName="Voltage" />
/// 	<dbEntry sqlName="db_field_3" anaName="Temperaturet" />
/// </addProcess>
/// ```
///
///--------------------------------------------------------------------------
///
/// RESTsoft - Software for Rare Event Searches with TPCs
///
/// History of developments:
///
/// 2020-May:  First implementation
///            Javier Galan
///
/// \class      TRestMySQLToAnalysisProcess
/// \author     Javier Galan
///
/// <hr>
///
#include "TRestMySQLToAnalysisProcess.h"
using namespace std;

#if defined USE_SQL
#include <mysql/mysql.h>
#endif

ClassImp(TRestMySQLToAnalysisProcess);

///////////////////////////////////////////////
/// \brief Default constructor
///
TRestMySQLToAnalysisProcess::TRestMySQLToAnalysisProcess() { Initialize(); }

///////////////////////////////////////////////
/// \brief Constructor loading data from a config file
///
/// The path to the config file can be specified using full path,
/// absolute or relative.
///
/// If the file is not found then REST will try to find the file on
/// the default paths defined in REST Framework, usually at the
/// REST_PATH installation directory. Additional search paths may be
/// defined using the parameter `searchPath` in globals section. See
/// TRestMetadata description.
///
/// \param cfgFileName A const char* giving the path to an RML file.
///
TRestMySQLToAnalysisProcess::TRestMySQLToAnalysisProcess(char* cfgFileName) {
    Initialize();

    LoadConfig(cfgFileName);
}

///////////////////////////////////////////////
/// \brief Default destructor
///
TRestMySQLToAnalysisProcess::~TRestMySQLToAnalysisProcess() {}

///////////////////////////////////////////////
/// \brief Function to load the default config in absence of RML input
///
void TRestMySQLToAnalysisProcess::LoadDefaultConfig() {
    SetName(this->ClassName());
    SetTitle("Default config");
}

///////////////////////////////////////////////
/// \brief Function to load the configuration from an external configuration
/// file.
///
/// If no configuration path is defined in TRestMetadata::SetConfigFilePath
/// the path to the config file must be specified using full path, absolute or
/// relative.
///
/// \param cfgFileName A const char* giving the path to an RML file.
/// \param name The name of the specific metadata. It will be used to find the
/// correspondig TRestMySQLToAnalysisProcess section inside the RML.
///
void TRestMySQLToAnalysisProcess::LoadConfig(std::string cfgFilename, std::string name) {
    if (LoadConfigFromFile(cfgFilename, name)) LoadDefaultConfig();
}

///////////////////////////////////////////////
/// \brief Function to use in initialization of process members before starting
/// to process the event
///
void TRestMySQLToAnalysisProcess::InitProcess() {
#if defined USE_SQL
    fStartTimestamp = fRunInfo->GetStartTimestamp();
    fEndTimestamp = fRunInfo->GetEndTimestamp();

    if (fSQLVariables.size() > 0)
        FillDBArrays();
    else {
        warning << "TRestMySQLToAnalysisProcess::InitProcess. No data base field entries have been specified!"
                << endl;
        warning << "This process will do nothing!" << endl;
    }
#else
    warning << "REST was not linked to SQL libraries. Run cmake using -DREST_SQL=ON" << endl;
#endif
}

///////////////////////////////////////////////
/// \brief Function to initialize input/output event members and define the
/// section name and library version
///
void TRestMySQLToAnalysisProcess::Initialize() { SetSectionName(this->ClassName()); }

///////////////////////////////////////////////
/// \brief The main processing event function
///
TRestEvent* TRestMySQLToAnalysisProcess::ProcessEvent(TRestEvent* evInput) {
    fEvent = evInput;

    debug << "TRestMySQLToAnalysisProcess. Ev ID : " << fEvent->GetID() << endl;
    debug << "TRestMySQLToAnalysisProcess. Get timestamp : " << fEvent->GetTime() << endl;
    for (int n = 0; n < fAnaTreeVariables.size(); n++)
        debug << "TRestMySQLToAnalysisProcess. Variable : " << fAnaTreeVariables[n]
              << " value : " << GetDBValueAtTimestamp(n, fEvent->GetTime()) << endl;

    if (GetVerboseLevel() >= REST_Extreme) GetChar();

    for (int n = 0; n < fAnaTreeVariables.size(); n++)
        SetObservableValue((string)fAnaTreeVariables[n], GetDBValueAtTimestamp(n, fEvent->GetTime()));

    return fEvent;
}

///////////////////////////////////////////////
/// \brief Function reading input parameters from the RML
/// TRestMySQLToAnalysisProcess section
///
void TRestMySQLToAnalysisProcess::InitFromConfigFile() {
    size_t pos = 0;

    fDBServerName = GetParameter("server", "");
    if (fDBServerName == "") ferr << "TRestMySQLToAnalysisProcess. Database server name not found!" << endl;

    fDBName = GetParameter("database", "");
    if (fDBName == "") ferr << "TRestMySQLToAnalysisProcess. Database not found!" << endl;

    fDBUserName = GetParameter("user", "");
    if (fDBUserName == "") ferr << "TRestMySQLToAnalysisProcess. Database user name not found!" << endl;

    fDBUserPass = GetParameter("password", "");
    if (fDBUserPass == "") ferr << "TRestMySQLToAnalysisProcess. Database user password not found!" << endl;

    fDBTable = GetParameter("table", "");
    if (fDBTable == "") ferr << "TRestMySQLToAnalysisProcess. Database table name not found!" << endl;

    string definition;
    while ((definition = GetKEYDefinition("dbEntry", pos)) != "") {
        TString sqlName = GetFieldValue("sqlName", definition);
        fSQLVariables.push_back((string)sqlName);

        TString anaName = GetFieldValue("anaName", definition);
        fAnaTreeVariables.push_back((string)anaName);
    }
}

///////////////////////////////////////////////
/// \brief It prints out basic information of the SQL database used to generate the analysis tree observables.
/// And the SQL fields extracted together with its corresponding analysis tree name, and minimum and maximum
/// values during the duration of the run.
///
void TRestMySQLToAnalysisProcess::PrintMetadata() {
    BeginPrintProcess();

    metadata << "SQL data extracted from:" << endl;
    metadata << "- database : " << fDBName << endl;
    metadata << "- table : " << fDBTable << endl;
    metadata << " " << endl;

    metadata << "List of variables added to the analysis tree" << endl;
    metadata << " ------------------------------------------ " << endl;
    for (int n = 0; n < fAnaTreeVariables.size(); n++) {
        metadata << " + SQL field : " << fSQLVariables[n] << endl;
        metadata << "   - Tree name : " << fAnaTreeVariables[n] << endl;
        metadata << "   - Min value : " << fMinValues[n] << endl;
        metadata << "   - Max value : " << fMaxValues[n] << endl;
        metadata << "  " << endl;
    }
    EndPrintProcess();
}

///////////////////////////////////////////////
/// \brief This method is the one accessing the SQL database and filling the internal arrays
/// with data that will be directly used to fill the analysisTree.
///
/// The resulting fDBdata will contain equi-spaced temporal points of the retrieved SQL data
/// with a sampling rate, fSampling, initialized by this method.
///
/// This method will also initialize the value of the fMinValues and fMaxValues vectors.
///
void TRestMySQLToAnalysisProcess::FillDBArrays() {
    MYSQL* conn = mysql_init(NULL);
    if (conn == NULL) {
        ferr << "TRestMySQLToAnalysisProcess::InitProcess. mysql_init() failed" << endl;
        exit(1);
    }

    if (!mysql_real_connect(conn, fDBServerName.c_str(), fDBUserName.c_str(), fDBUserPass.c_str(),
                            fDBName.c_str(), 0, NULL, 0)) {
        ferr << "TRestMySQLToAnalysisProcess::InitProcess. Connection to DB failed!" << endl;
        ferr << mysql_error(conn) << endl;
        exit(1);
    }

    string sqlQuery = BuildQueryString();
    debug << sqlQuery << endl;
    if (mysql_query(conn, sqlQuery.c_str())) {
        ferr << "Error making query to SQL database" << endl;
        ferr << mysql_error(conn) << endl;
        ferr << "Query string : " << sqlQuery << endl;
        exit(1);
    }

    MYSQL_RES* result = mysql_store_result(conn);

    if (result == NULL) {
        ferr << "Error getting result from SQL query" << endl;
        ferr << mysql_error(conn) << endl;
        ferr << "Query string : " << sqlQuery << endl;
        exit(1);
    }

    int num_fields = mysql_num_fields(result);
    int num_rows = mysql_num_rows(result);

    // We double the average sampling of data in the given range
    fSampling = (fEndTimestamp - fStartTimestamp) / num_rows / 2;

    if (num_rows < 3) {
        ferr << "Not enough data found on the event data range" << endl;
        ferr << "If no database entries exist remove TRestMySQLToAnalysisProcess from your processing chain"
             << endl;
        // We take the decision to stop processing to make sure we are aware of the problem.
        // Specially to identify possible errors in this code.
        // But we might get more flexible with time, and this process just prompts a worning and does nothing.
        exit(1);
    }

    // We register all the data inside a std::vector.
    MYSQL_ROW row;
    std::vector<std::vector<Double_t>> data;

    while ((row = mysql_fetch_row(result))) {
        std::vector<Double_t> dataRow;
        for (int i = 0; i < num_fields; i++) {
            string value;
            if (row[i])
                value = row[i];
            else
                value = "NULL";

            dataRow.push_back(StringToDouble(value));
        }
        data.push_back(dataRow);
    }

    fMinValues.clear();
    fMaxValues.clear();
    for (int j = 0; j < data.size(); j++) {
        if (j == 0)
            for (int n = 1; n < data.front().size(); n++) {
                fMinValues.push_back(data.front()[n]);
                fMaxValues.push_back(data.front()[n]);
            }

        for (int n = 1; n < data[j].size(); n++) {
            if (fMinValues[n - 1] > data[j][n]) fMinValues[n - 1] = data[j][n];
            if (fMaxValues[n - 1] < data[j][n]) fMaxValues[n - 1] = data[j][n];
        }
    }

    debug << "Raw data size " << data.size() << endl;

    fDBdata.clear();
    cout.precision(10);
    // We finally fill the array with a fixed time bin (given by fSampling)
    Double_t timeNow = fStartTimestamp;
    Int_t dbEntry = 0;
    while (timeNow < fEndTimestamp) {
        while (timeNow > data[dbEntry][0] && dbEntry + 1 < data.size()) dbEntry++;

        std::vector<Double_t> dataBuff;
        if (timeNow < data.front()[0]) {
            for (int n = 1; n < data.front().size(); n++) dataBuff.push_back(data.front()[n]);
        } else if (timeNow > data.back()[0]) {
            for (int n = 1; n < data.back().size(); n++) dataBuff.push_back(data.back()[n]);
        } else {
            for (int n = 1; n < data[dbEntry].size(); n++) {
                double y2 = data[dbEntry][n];
                double y1 = data[dbEntry - 1][n];

                // Normalized field
                double x2 = data[dbEntry][0];
                double x1 = data[dbEntry - 1][0];

                double m = (y2 - y1) / (x2 - x1);
                double l = y1 - m * x1;

                dataBuff.push_back(m * timeNow + l);
            }
        }

        timeNow += fSampling;
        fDBdata.push_back(dataBuff);
    }

    debug << "Added entries : " << fDBdata.size() << endl;
    if (GetVerboseLevel() >= REST_Debug) TRestTools::PrintTable(fDBdata, 0, 5);

    mysql_close(conn);
}

///////////////////////////////////////////////
/// \brief Dedicated method to help building the SQL query string
///
string TRestMySQLToAnalysisProcess::BuildQueryString() {
    string sqlQuery = "SELECT timestamp";
    for (int n = 0; n < fSQLVariables.size(); n++) {
        sqlQuery += ",";
        sqlQuery += fSQLVariables[n];
    }
    sqlQuery += " FROM " + fDBTable + " where timestamp between ";

    string startStr = Form("%10.0lf", fStartTimestamp);
    string endStr = Form("%10.0lf", fEndTimestamp);

    sqlQuery += startStr + " and " + endStr;

    return sqlQuery;
}

///////////////////////////////////////////////
/// \brief This method will retrieve the given data field component, specified by the argument `index`
/// at the given timestamp. The returned value will be interpolated with 2 known values of the
/// neighbouring timestamps inside the fDBdata dataset.
///
Double_t TRestMySQLToAnalysisProcess::GetDBValueAtTimestamp(Int_t index, Double_t timestamp) {
    Int_t bin = (Int_t)((timestamp - fStartTimestamp) / fSampling);

    if (bin < 0) return fDBdata.front()[index];
    if (bin + 1 >= fDBdata.size()) return fDBdata.back()[index];

    double y2 = fDBdata[bin + 1][index];
    double y1 = fDBdata[bin][index];

    double m = (y2 - y1) / fSampling;
    double l = y1 - m * (fStartTimestamp + bin * fSampling);

    return m * timestamp + l;
}
