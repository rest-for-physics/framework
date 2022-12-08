/*************************************************************************
 * This file is part of the REST software framework.                     *
 *                                                                       *
 * Copyright (C) 2016 GIFNA/TREX (University of Zaragoza)                *
 * For more information see https://gifna.unizar.es/trex                 *
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
 * If not, see https://www.gnu.org/licenses/.                            *
 * For the list of contributors see $REST_PATH/CREDITS.                  *
 *************************************************************************/

/////////////////////////////////////////////////////////////////////////
/// Write the class description Here
///
/// ### Parameters
/// * **startTime**: Only files with start run after `startTime` will be considered.
/// * **endTime**: Only files with end run before `endTime` will be considered.
///
/// ### Examples
/// \code
///	<TRestDataSet name="28Nov_test">
///
/// 	<parameter name="startTime" value = "2022/04/28 00:00" />
/// 	<parameter name="endTime" value = "2022/04/28 23:59" />
/// 	<parameter name="filePattern" value="test*.root"/>
///
/// 	<filter metadata="TRestRun::fRunTag" contains="Baby" />
///
///		// Will add to the final tree only the specific observables
///		<addObservables list="g4Ana_totalEdep;hitsAna_energy" />
///
///		// Will add all the observables from the process `rawAna`
///		<addProcessObservables list="rawAna" />
///
/// </TRestDataSet>
/// \endcode
///
///
///----------------------------------------------------------------------
///
/// REST-for-Physics - Software for Rare Event Searches Toolkit
///
/// History of developments:
///
/// 2022-November: First implementation of TRestDataSet
/// Javier Galan
///
/// \class TRestDataSet
/// \author: Javier Galan (javier.galan.lacarra@cern.ch)
///
/// <hr>
///

#include "TRestDataSet.h"
#include "TRestRun.h"
#include "TRestTools.h"

ClassImp(TRestDataSet);

///////////////////////////////////////////////
/// \brief Default constructor
///
TRestDataSet::TRestDataSet() { Initialize(); }

/////////////////////////////////////////////
/// \brief Constructor loading data from a config file
///
/// If no configuration path is defined using TRestMetadata::SetConfigFilePath
/// the path to the config file must be specified using full path, absolute or
/// relative.
///
/// The default behaviour is that the config file must be specified with
/// full path, absolute or relative.
///
/// \param cfgFileName A const char* giving the path to an RML file.
/// \param name The name of the specific metadata. It will be used to find the
/// corresponding TRestAxionMagneticField section inside the RML.
///
TRestDataSet::TRestDataSet(const char* cfgFileName, std::string name) : TRestMetadata(cfgFileName) {
    LoadConfigFromFile(fConfigFileName, name);

    if (GetVerboseLevel() >= TRestStringOutput::REST_Verbose_Level::REST_Info) PrintMetadata();
}

///////////////////////////////////////////////
/// \brief Default destructor
///
TRestDataSet::~TRestDataSet() {}

///////////////////////////////////////////////
/// \brief It will initialize the data frame with the filelist and column names
/// (or observables) that have been defined by the user.
///
void TRestDataSet::Initialize() {
    SetSectionName(this->ClassName());

    if (fTree != nullptr) {
        RESTWarning << "Tree has already been loaded. Skipping TRestDataSet::LoadDataFrame ... " << RESTendl;
        return;
    }

    if (fFileSelection.empty()) FileSelection();

    // We are not ready yet
    if (fFileSelection.empty()) return;

    ///// Disentangling process observables --> producing finalList
    TRestRun* run = new TRestRun(fFileSelection[0]);
    std::vector<std::string> finalList;
    finalList.push_back("runOrigin");
    finalList.push_back("eventID");
    finalList.push_back("timeStamp");

    for (const auto& obs : fObservablesList) finalList.push_back(obs);

    std::vector<std::string> obsNames = run->GetAnalysisTree()->GetObservableNames();
    for (const auto& name : obsNames) {
        for (const auto& pcs : fProcessObservablesList) {
            if (name.find(pcs) == 0) finalList.push_back(name);
        }
    }
    delete run;
    ///////

    ROOT::EnableImplicitMT();

    fDataSet = ROOT::RDataFrame("AnalysisTree", fFileSelection);

    std::string user = getenv("USER");
    std::string foutname = "/tmp/rest_output_" + user + ".root";
    fDataSet.Snapshot("AnalysisTree", foutname, finalList);

    fDataSet = ROOT::RDataFrame("AnalysisTree", foutname);

    TFile* f = new TFile((TString)foutname);
    fTree = (TTree*)f->Get("AnalysisTree");

    int cont = 0;
    std::string obsListStr;
    for (const auto& l : finalList) {
        if (cont > 0) obsListStr += ":";
        obsListStr += l;
        cont++;
    }

    // We do this so that later we can recover the values using TTree::GetVal
    fTree->Draw((TString)obsListStr, "", "goff");

    std::cout << " - Dataset initialized!" << std::endl;
}

///////////////////////////////////////////////
/// \brief Function to determine the filenames that satisfy the dataset conditions
///
std::vector<std::string> TRestDataSet::FileSelection() {
    fFileSelection.clear();

    std::time_t time_stamp_start = REST_StringHelper::StringToTimeStamp(fStartTime);
    std::time_t time_stamp_end = REST_StringHelper::StringToTimeStamp(fEndTime);

    if (!time_stamp_end || !time_stamp_start) {
        RESTError << "TRestDataSet::FileSelect. Start or end dates not properly formed. Please, check "
                     "REST_StringHelper::StringToTimeStamp documentation for valid formats"
                  << RESTendl;
        return fFileSelection;
    }

    std::vector<std::string> fileNames = TRestTools::GetFilesMatchingPattern(fFilePattern);

    if (!fileNames.empty()) {
        RESTInfo << " - Starting to select files matching pattern. Total files : " << fileNames.size()
                 << RESTendl;
    }

    for (const auto& file : fileNames) {
        TRestRun* run = new TRestRun(file);
        double runStart = run->GetStartTimestamp();
        double runEnd = run->GetEndTimestamp();

        if (runStart < time_stamp_start && runEnd > time_stamp_end) {
            delete run;
            continue;
        }

        int n = 0;
        bool accept = true;
        for (const auto md : fFilterMetadata) {
            std::string mdValue = run->GetMetadataMember(md);

            if (!fFilterContains[n].empty())
                if (mdValue.find(fFilterContains[n]) == std::string::npos) accept = false;

            if (fFilterGreaterThan[n] != -1) {
                if (StringToDouble(mdValue) <= fFilterGreaterThan[n]) accept = false;
            }

            if (fFilterLowerThan[n] != -1)
                if (StringToDouble(mdValue) >= fFilterLowerThan[n]) accept = false;

            n++;
        }
        delete run;

        if (!accept) continue;

        fFileSelection.push_back(file);
    }
    RESTInfo << RESTendl;

    return fFileSelection;
}

/////////////////////////////////////////////
/// \brief Prints on screen the information about the metadata members of TRestAxionSolarFlux
///
void TRestDataSet::PrintMetadata() {
    TRestMetadata::PrintMetadata();

    RESTMetadata << " - StartTime : " << fStartTime << RESTendl;
    RESTMetadata << " - EndTime : " << fEndTime << RESTendl;
    RESTMetadata << " - Path : " << TRestTools::SeparatePathAndName(fFilePattern).first << RESTendl;
    RESTMetadata << " - File pattern : " << TRestTools::SeparatePathAndName(fFilePattern).second << RESTendl;

    RESTMetadata << "  " << RESTendl;

    if (!fObservablesList.empty()) {
        RESTMetadata << " Single observables added" << RESTendl;
        RESTMetadata << "---------" << RESTendl;
        for (const auto& l : fObservablesList) RESTMetadata << " - " << l << RESTendl;

        RESTMetadata << "  " << RESTendl;
    }

    if (!fProcessObservablesList.empty()) {
        RESTMetadata << " Process observables added" << RESTendl;
        RESTMetadata << "---------" << RESTendl;
        for (const auto& l : fProcessObservablesList) RESTMetadata << " - " << l << RESTendl;

        RESTMetadata << "  " << RESTendl;
    }

    if (!fFilterMetadata.empty()) {
        RESTMetadata << " Metadata filters" << RESTendl;
        RESTMetadata << "---------" << RESTendl;

        int n = 0;
        for (const auto& mdFilter : fFilterMetadata) {
            RESTMetadata << " - " << mdFilter << ".";

            if (!fFilterContains[n].empty()) RESTMetadata << " Contains: " << fFilterContains[n];
            if (fFilterGreaterThan[n] != -1) RESTMetadata << " Greater than: " << fFilterGreaterThan[n];
            if (fFilterLowerThan[n] != -1) RESTMetadata << " Lower than: " << fFilterLowerThan[n];

            RESTMetadata << RESTendl;
            n++;
        }
    }

    RESTMetadata << "----" << RESTendl;
}

///////////////////////////////////////////////
/// \brief Initialization of specific TRestDataSet members through an RML file
///
void TRestDataSet::InitFromConfigFile() {
    TRestMetadata::InitFromConfigFile();

    /// Reading filters
    TiXmlElement* filterDefinition = GetElement("filter");
    while (filterDefinition != nullptr) {
        std::string metadata = GetFieldValue("metadata", filterDefinition);
        if (metadata.empty()) {
            RESTError << "Filter key defined without metadata member!" << RESTendl;
            exit(1);
        }

        fFilterMetadata.push_back(metadata);

        std::string contains = GetFieldValue("contains", filterDefinition);
        Double_t greaterThan = StringToDouble(GetFieldValue("greaterThan", filterDefinition));
        Double_t lowerThan = StringToDouble(GetFieldValue("lowerThan", filterDefinition));

        fFilterContains.push_back(contains);
        fFilterGreaterThan.push_back(greaterThan);
        fFilterLowerThan.push_back(lowerThan);

        filterDefinition = GetNextElement(filterDefinition);
    }

    /// Reading observables
    TiXmlElement* observablesDefinition = GetElement("observables");
    while (observablesDefinition != nullptr) {
        std::string observables = GetFieldValue("list", observablesDefinition);
        if (observables.empty()) {
            RESTError << "<addObservables key does not contain a list!" << RESTendl;
            exit(1);
        }

        std::vector<std::string> obsList = REST_StringHelper::Split(observables, ":");

        for (const auto& l : obsList) fObservablesList.push_back(l);

        observablesDefinition = GetNextElement(observablesDefinition);
    }

    /// Reading process observables
    TiXmlElement* obsProcessDefinition = GetElement("processObservables");
    while (obsProcessDefinition != nullptr) {
        std::string observables = GetFieldValue("list", obsProcessDefinition);
        if (observables.empty()) {
            RESTError << "<addProcessObservables key does not contain a list!" << RESTendl;
            exit(1);
        }

        std::vector<std::string> obsList = REST_StringHelper::Split(observables, ":");

        for (const auto& l : obsList) fProcessObservablesList.push_back(l);

        obsProcessDefinition = GetNextElement(obsProcessDefinition);
    }
}

///////////////////////////////////////////////
/// \brief It will generate an output file with the dataset compilation. Only the selected
/// branches and the files that fulfill the metadata filter conditions will be included.
///
/// For the moment we produce two different output files.
///
/// - A plain-text (file recognized with `csv` or `txt` extension): It will produce a table
/// with observable values, including a header of the dataset conditions.
///
/// - A root file (recognized through its `root` extension): It will write to disk an
/// Snapshot of the current dataset, i.e. in standard TTree format, together with a copy
/// of the TRestDataSet instance that contains the conditions used to generate the dataset.
///
void TRestDataSet::Export(std::string fname) {
    if (TRestTools::GetFileNameExtension(fname) == "txt" ||
        TRestTools::GetFileNameExtension(fname) == "csv") {
        std::vector<std::string> dataTypes;
        for (int n = 0; n < fTree->GetListOfBranches()->GetEntries(); n++) {
            std::string bName = fTree->GetListOfBranches()->At(n)->GetName();
            std::string type = fTree->GetLeaf((TString)bName)->GetTypeName();
            dataTypes.push_back(type);
            if (type != "Double_t" && type != "Int_t") {
                RESTError << "Branch name : " << bName << " is type : " << type << RESTendl;
                RESTError << "Only Int_t and Double_t types are allowed for "
                             "exporting to ASCII table"
                          << RESTendl;
                RESTError << "File will not be generated" << RESTendl;
                return;
            }
        }

        FILE* f = fopen(fname.c_str(), "wt");
        ///// Writting header
        fprintf(f, "### TRestDataSet generated file\n");
        fprintf(f, "### \n");
        fprintf(f, "### StartTime : %s\n", fStartTime.c_str());
        fprintf(f, "### EndTime : %s\n", fEndTime.c_str());
        fprintf(f, "###\n");
        fprintf(f, "### Data path : %s\n", TRestTools::SeparatePathAndName(fFilePattern).first.c_str());
        fprintf(f, "### File pattern : %s\n", TRestTools::SeparatePathAndName(fFilePattern).second.c_str());
        fprintf(f, "###\n");
        if (!fFilterMetadata.empty()) {
            fprintf(f, "### Metadata filters : \n");
            int n = 0;
            for (const auto& md : fFilterMetadata) {
                fprintf(f, "### - %s.", md.c_str());
                if (!fFilterContains[n].empty()) fprintf(f, " Contains: %s.", fFilterContains[n].c_str());
                if (fFilterGreaterThan[n] != -1) fprintf(f, " Greater than: %6.3lf.", fFilterGreaterThan[n]);
                if (fFilterLowerThan[n] != -1) fprintf(f, " Lower than: %6.3lf.", fFilterLowerThan[n]);
                fprintf(f, "\n");
                n++;
            }
        }
        fprintf(f, "###\n");
        fprintf(f, "### Observables list: ");
        for (int n = 0; n < fTree->GetListOfBranches()->GetEntries(); n++) {
            std::string bName = fTree->GetListOfBranches()->At(n)->GetName();
            fprintf(f, " %s", bName.c_str());
        }
        fprintf(f, "\n");
        fprintf(f, "###\n");
        fprintf(f, "### Data starts here\n");

        for (int n = 0; n < fTree->GetEntries(); n++) {
            for (int m = 0; m < GetNumberOfBranches(); m++) {
                std::string bName = fTree->GetListOfBranches()->At(m)->GetName();
                if (m > 0) fprintf(f, "\t");
                if (dataTypes[m] == "Double_t")
                    if (bName == "timeStamp")
                        fprintf(f, "%010.0lf", fTree->GetVal(m)[n]);
                    else
                        fprintf(f, "%05.3e", fTree->GetVal(m)[n]);
                else
                    fprintf(f, "%06d", (Int_t)fTree->GetVal(m)[n]);
            }
            fprintf(f, "\n");
        }
        fclose(f);

        return;
    } else if (TRestTools::GetFileNameExtension(fname) == "root") {
        fDataSet.Snapshot("AnalysisTree", fname);

        TFile* f = new TFile((TString)fname, "UPDATE");
        this->Write();
        f->Close();
    } else {
        RESTWarning << "TRestDataSet::Export. Extension " << TRestTools::GetFileNameExtension(fname)
                    << " not recognized" << RESTendl;
    }
}
