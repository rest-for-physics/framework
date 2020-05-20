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

#ifndef RestCore_TRestMySQLToAnalysisProcess
#define RestCore_TRestMySQLToAnalysisProcess

#include "TRestEventProcess.h"

//! This process connects to a SQL database and adds new observables inside the analysis tree.
class TRestMySQLToAnalysisProcess : public TRestEventProcess {
   private:
    /// A pointer to the event data
    TRestEvent* fEvent = NULL;  //!

    /// The host indentification name or IP
    string fDBServerName;  //!

    /// The database username
    string fDBUserName;  //!

    /// The database user password
    string fDBUserPass;  //!

    /// The database name
    string fDBName;  //<

    /// The database table
    string fDBTable;  //<

    /// The name of the entries defined by user that will be retrieved at the SQL database
    std::vector<TString> fSQLVariables;  //<

    /// The name of the varibles will be renamed to these values in the analysisTree
    std::vector<TString> fAnaTreeVariables;  //<

    /// The minimum value of the corresponding extracted field variable
    std::vector<Double_t> fMinValues;  //<

    /// The maximum value of the corresponding extracted field variable
    std::vector<Double_t> fMaxValues;  //<

    /// Stores the start timestamp used to extract the SQL data
    Double_t fStartTimestamp;  //!

    /// Stores the end timestamp used to extract the SQL data
    Double_t fEndTimestamp;  //!

    /// Defines the internal sampling of the retrieved data for quick access
    Double_t fSampling;  //!

    /// A matrix containning extracted SQL data with fixed time bin
    std::vector<std::vector<Double_t> > fDBdata;  //!

    /// A boolean to output a warning message just once inside the process
    Bool_t fCheckSQL = true;  //!

    string BuildQueryString();

    void FillDBArrays();

    Double_t GetDBValueAtTimestamp(Int_t index, Double_t timestamp);

   protected:
    void InitProcess();

    void InitFromConfigFile();

    void Initialize();

    void LoadDefaultConfig();

   public:
    any GetInputEvent() { return fEvent; }
    any GetOutputEvent() { return fEvent; }

    TRestEvent* ProcessEvent(TRestEvent* evInput);

    void LoadConfig(std::string cfgFilename, std::string name = "");

    void PrintMetadata();

    /// Returns a new instance of this class
    TRestEventProcess* Maker() { return new TRestMySQLToAnalysisProcess; }

    /// Returns the name of this process
    TString GetProcessName() { return (TString) "SQLToAnalysisProces"; }

    TRestMySQLToAnalysisProcess();
    TRestMySQLToAnalysisProcess(char* cfgFileName);

    ~TRestMySQLToAnalysisProcess();

    // If new members are added, removed or modified in this class version number must be increased!
    ClassDef(TRestMySQLToAnalysisProcess, 1);
};
#endif
