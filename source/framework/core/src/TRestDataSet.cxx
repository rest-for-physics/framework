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
/// This class allows to make a selection of ROOT data files that fulfill
/// certain metadata conditions allowing to create a group of files that
/// define a particular dataset. The files will be searched in a relative
/// or absolute path that is given together the `filePattern` parameter.
///
/// ### Basic file selection
///
/// We will be able to define the dates range where files will be
/// accepted, using `startTime` and `endTime` parameters. The run start
/// time and end time stored inside TRestRun will be evaluated to decide
/// if the file should be considered.
///
/// A summary of the basic parameters follows:
///
/// * **filePattern**: A full path glob pattern to the files that will
/// be considered. It is a first filter considering the path and the
/// filename. Usual wild cards such as * or ? will be allowed to target
/// a given range of files.
/// * **startTime**: Only files with start run after `startTime` will be
/// considered.
/// * **endTime**: Only files with end run before `endTime` will be
/// considered.
///
/// ### Metadata rules
///
/// We may add rules for any metadata class existing inside our ROOT
/// datafiles. For such, we use the `filter` key where we define the
/// metadata member name where we want to evaluate the rules. We need
/// to define the `metadata` field where we specify the class name or
/// metadata user given name, together with the metadata member we want
/// to access, the metadata member must be named using the coventions
/// defined inside the methods TRestRun::ReplaceMetadataMember and
/// TRestRun::ReplaceMetadataMembers.
///
/// Three optional fields can be used to apply the rule:
///
/// - **contains**: It will accept the file if the metadata member
/// contains the string given inside this field, it can also be a
/// number.
/// - **greaterThan**: It will accept only runs with the corresponding
/// metadata member above the given value. Obviously, the value must
/// be a numeric value.
/// - **lowerThan**: It will accept only runs with the corresponding
/// metadata member below the given value. Obviously, the value must
/// be a numeric value.
///
/// Example of metadata rule:
/// \code
/// 	<filter metadata="TRestRun::fRunTag" contains="Baby" />
/// \endcode
///
/// ### Branches (or observables) selection
///
/// Once the files that fulfill the given dates, filename pattern and
/// metadata rules have been identified, the initialization will produce
/// an instance to a ROOT::RDataFrame and an instance to a ROOT::TTree
/// that will give access to the unified analysis tree. The available
/// columns or branches at those instances will be defined by the user
/// inside this metadata class, through the special keywords
/// `observables` and `processObservables`.
///
/// - **observables**: This key will allow to add a list of observables
/// present at the analysis tree to be considered for the new compilation.
/// The values must be separated by colons, ":".
///
/// - **processObservables**: This key will allow to add a list of process
/// names for which all the observables found inside will be added to
/// the compilation. The values must be separated by colons, ":".
///
/// Their use can be seen in the following example:
/// \code
/// <TRestDataSet name="DummySet">
///
///    <parameter name="startTime" value = "2022/04/28 00:00" />
///    <parameter name="endTime" value = "2022/04/28 23:59" />
///    <parameter name="filePattern" value="test*.root"/>
///
///    <filter metadata="TRestRun::fRunTag" contains="Baby" />
///
///    // Will add to the final tree only the specific observables
///    <observables list="g4Ana_totalEdep:hitsAna_energy" />
///
///    // Will apply a cut to the observables
///    <TRestCut>
///      <cut name="c1" variable="rawAna_NumberOfGoodSignals" condition=">1" />
///      <cut name="c1" variable="rawAna_NumberOfGoodSignals" condition="<100" />
///    </TRestCut>
///    // Will add all the observables from the process `rawAna`
///    <processObservables list="rate:rawAna" />
///
/// </TRestDataSet>
/// \endcode
///
/// ### Basic usage
///
/// The basic usage of this class is by loading the metadata class as any
/// other metadata class.  After initialization, the user will get access
/// to the internal RDataFrame and TTree data members, as shown in the
/// following example:
///
/// \code
/// restRoot
/// [0] TRestDataSet d("dataset");
/// [1] d.GenerateDataSet();
/// [2] d.GetTree()->GetEntries()
/// [3] d.GetDataFrame().GetColumnNames()
/// \endcode
///
/// We can then use our favorite ROOT::RDataFrame or TTree methods.
///
/// ### Exporting datasets
///
/// On top of performing a compilation of runs to construct a dataset
/// and access the data in a unified way, we may also save the generated
/// dataset to disk. This feature might be used to generate easier to
/// handle data compilations that have been extracted from an **official
/// data repository**.
///
/// Different output formats are supported:
///
/// - `root`: It will store the simplified `TTree` with the observables
/// selected by the user and compiled with the corresponding file selection.
/// The root file will also contain a TRestDataSet object to allow future
/// users of the output file generated to identify the origin of the data.
///
/// - `txt` or `csv`: It will create an ASCII table where each column
/// will contain the data of a given branch. A header will be written
/// inside the file with all the information found inside the TRestDataSet
/// instance.
///
///
/// Example 1 Generate DataSet from config file:
/// \code
/// restRoot
/// [0] TRestDataSet d("dataset", "dataSetName");
/// [1] d.GenerateDataSet();
/// [2] d.Export("mydataset.csv");
/// [3] d.Export("mydataset.root");
/// \endcode
///
/// Example 2 Import existing DataSet:
/// \code
/// restRoot
/// [0] TRestDataSet d();
/// [1] d.Import("myDataSet.root");
/// [2] d.GetTree()->GetEntries()
/// \endcode
///
/// ### Relevant quantities
///
/// Sometimes we will be willing that our dataset contains few variables
/// that are extremelly meaningful for the data compilation, and that will
/// be required for further calculations or for the proper interpretation
/// of the data. The key `<quantity` will allow the user to define relevant
/// quantities that will be stored together with the dataset. These
/// quantitites must be extracted from existing metadata members that are
/// present at the original files. There are different fields allowed
/// inside, such as: `name`, `metadata`, `strategy` and `description`.
///
/// Example:
/// \code
/// <quantity name="Nsim" metadata="[TRestProcessRunner::fEventsToProcess]"
///           strategy="accumulate" description="The total number of simulated events."/>
/// \endcode
///
/// The `name` field will be the user given name of the quantity. The
/// `metadata` field inside the `<quantity` definition will allow to
/// include a metadata member or a calculation based on a formula where
/// metadata members intervine. The method TRestRun::ReplaceMetadataMembers
/// is the responsible to translate the given metadata formula into a
/// numeric value, check the documentation inside that method to find out
/// the proper format of metadata members inside this field.
///
/// There are also different strategies for extracting the quantity value,
/// which are defined by the user using the field `strategy`, the different
/// options available are:
/// - **accumulate**: It will add the value of the `metadata` definition
/// for each of the selected files that will be included in the dataset.
/// - **max**: It will register the maximum metadata value found in each
/// of the files included in the dataset.
/// - **min**: It will register the minimum metadata value found in each
/// of the files included in the dataset.
/// - **unique**: It will simply register the value of the metadata member.
/// If the metadata member value found at each of the selected files is
/// not exactly the same, then a warning output message will be prompted.
/// - **last**: It will simply register the value of the metadata member
/// from the last file in the list of selected files.
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
TRestDataSet::TRestDataSet(const char* cfgFileName, const std::string& name) : TRestMetadata(cfgFileName) {
    LoadConfigFromFile(fConfigFileName, name);

    if (GetVerboseLevel() >= TRestStringOutput::REST_Verbose_Level::REST_Info) PrintMetadata();
}

///////////////////////////////////////////////
/// \brief Default destructor
///
TRestDataSet::~TRestDataSet() {}

///////////////////////////////////////////////
/// \brief This function initialize different parameters
/// from the TRestDataSet
///
void TRestDataSet::Initialize() { SetSectionName(this->ClassName()); }

///////////////////////////////////////////////
/// \brief This function generates the data frame with the filelist and column names
/// (or observables) that have been defined by the user.
///
void TRestDataSet::GenerateDataSet() {
    if (fTree != nullptr) {
        RESTWarning << "Tree has already been loaded. Skipping TRestDataSet::GenerateDataSet ... "
                    << RESTendl;
        return;
    }

    if (fFileSelection.empty()) FileSelection();

    // We are not ready yet
    if (fFileSelection.empty()) {
        RESTError << "File selection is empty " << RESTendl;
        return;
    }

    ///// Disentangling process observables --> producing finalList
    TRestRun run(fFileSelection.front());
    std::vector<std::string> finalList;
    finalList.push_back("runOrigin");
    finalList.push_back("eventID");
    finalList.push_back("timeStamp");

    auto obsNames = run.GetAnalysisTree()->GetObservableNames();
    for (const auto& obs : fObservablesList) {
        if (std::find(obsNames.begin(), obsNames.end(), obs) != obsNames.end()) {
            finalList.push_back(obs);
        } else {
            RESTWarning << " Observable " << obs << " not found in observable list, skipping..." << RESTendl;
        }
    }

    for (const auto& name : obsNames) {
        for (const auto& pcs : fProcessObservablesList) {
            if (name.find(pcs) == 0) finalList.push_back(name);
        }
    }

    // Remove duplicated observables if any
    std::sort(finalList.begin(), finalList.end());
    finalList.erase(std::unique(finalList.begin(), finalList.end()), finalList.end());

    ROOT::EnableImplicitMT();

    fDataSet = ROOT::RDataFrame("AnalysisTree", fFileSelection);

    fDataSet = MakeCut(fCut);

    std::string user = getenv("USER");
    std::string fOutName = "/tmp/rest_output_" + user + ".root";
    fDataSet.Snapshot("AnalysisTree", fOutName, finalList);

    fDataSet = ROOT::RDataFrame("AnalysisTree", fOutName);

    TFile* f = TFile::Open(fOutName.c_str());
    fTree = (TTree*)f->Get("AnalysisTree");

    RESTInfo << " - Dataset initialized!" << RESTendl;
}

///////////////////////////////////////////////
/// \brief Function to determine the filenames that satisfy the dataset conditions
///
std::vector<std::string> TRestDataSet::FileSelection() {
    fFileSelection.clear();

    std::time_t time_stamp_start = REST_StringHelper::StringToTimeStamp(fFilterStartTime);
    std::time_t time_stamp_end = REST_StringHelper::StringToTimeStamp(fFilterEndTime);

    if (!time_stamp_end || !time_stamp_start) {
        RESTError << "TRestDataSet::FileSelect. Start or end dates not properly formed. Please, check "
                     "REST_StringHelper::StringToTimeStamp documentation for valid formats"
                  << RESTendl;
        return fFileSelection;
    }

    std::vector<std::string> fileNames = TRestTools::GetFilesMatchingPattern(fFilePattern);

    if (!fileNames.empty()) {
        RESTInfo << "TRestDataSet::FileSelection. Starting file selection." << RESTendl;
        RESTInfo << "Total files : " << fileNames.size() << RESTendl;
        RESTInfo << "This process may take long computation time in case there are many files." << RESTendl;
    }

    fTotalDuration = 0;
    for (const auto& file : fileNames) {
        TRestRun run(file);
        double runStart = run.GetStartTimestamp();
        double runEnd = run.GetEndTimestamp();

        if (runStart < time_stamp_start || runEnd > time_stamp_end) {
            continue;
        }

        int n = 0;
        bool accept = true;
        for (const auto& md : fFilterMetadata) {
            std::string mdValue = run.GetMetadataMember(md);

            if (!fFilterContains[n].empty())
                if (mdValue.find(fFilterContains[n]) == std::string::npos) accept = false;

            if (fFilterGreaterThan[n] != -1) {
                if (StringToDouble(mdValue) <= fFilterGreaterThan[n]) accept = false;
            }

            if (fFilterLowerThan[n] != -1)
                if (StringToDouble(mdValue) >= fFilterLowerThan[n]) accept = false;

            if (fFilterEqualsTo[n] != -1)
                if (StringToDouble(mdValue) != fFilterEqualsTo[n]) accept = false;

            n++;
        }

        if (!accept) continue;

        for (auto& [name, properties] : fQuantity) {
            Double_t value =
                REST_StringHelper::StringToDouble(run.ReplaceMetadataMembers(properties.metadata));

            if (properties.strategy == "accumulate") properties.value += value;

            if (properties.strategy == "max")
                if (properties.value == 0 || properties.value < value) properties.value = value;

            if (properties.strategy == "min")
                if (properties.value == 0 || properties.value > value) properties.value = value;

            if (properties.strategy == "unique") {
                if (properties.value == 0)
                    properties.value = value;
                else if (properties.value != value) {
                    RESTWarning << "TRestDataSet::FileSelection. Relevant quantity retrieval." << RESTendl;
                    RESTWarning << "A unique metadata member used for the `" << name
                                << "` quantity is not unique!!" << RESTendl;
                    RESTWarning << "Pre-registered value : " << properties.value << " New value : " << value
                                << RESTendl;
                }
            }

            if (properties.strategy == "last") properties.value = value;
        }

        if (run.GetStartTimestamp() < fStartTime) fStartTime = run.GetStartTimestamp();

        if (run.GetEndTimestamp() > fEndTime) fEndTime = run.GetEndTimestamp();

        fTotalDuration += run.GetEndTimestamp() - run.GetStartTimestamp();
        fFileSelection.push_back(file);
    }
    RESTInfo << RESTendl;

    return fFileSelection;
}

///////////////////////////////////////////////
/// \brief This function apply a TRestCut to the dataframe
/// and returns a dataframe with the applied cuts. Note that
/// the cuts are not applied directly to the dataframe on
/// TRestDataSet, to do so you should do fDataSet = MakeCut(fCut);
///
ROOT::RDF::RNode TRestDataSet::MakeCut(const TRestCut* cut) {
    auto df = fDataSet;

    if (cut == nullptr) return df;

    auto paramCut = cut->GetParamCut();
    auto obsList = df.GetColumnNames();
    for (const auto& [param, condition] : paramCut) {
        if (std::find(obsList.begin(), obsList.end(), param) != obsList.end()) {
            std::string pCut = param + condition;
            RESTDebug << "Applying cut " << pCut << RESTendl;
            df = df.Filter(pCut);
        } else {
            RESTWarning << " Cut observable " << param << " not found in observable list, skipping..."
                        << RESTendl;
        }
    }

    auto cutString = cut->GetCutStrings();
    for (const auto& pCut : cutString) {
        bool added = false;
        for (const auto& obs : obsList) {
            if (pCut.find(obs) != std::string::npos) {
                RESTDebug << "Applying cut " << pCut << RESTendl;
                df = df.Filter(pCut);
                added = true;
                break;
            }
        }

        if (!added) {
            RESTWarning << " Cut string " << pCut << " not found in observable list, skipping..." << RESTendl;
        }
    }

    return df;
}

///////////////////////////////////////////////
/// \brief This function will add a new column to the RDataFrame using
/// the same scheme as the usual RDF::Define method, but it will on top of
/// that evaluate the values of any relevant quantities used.
///
/// For example, the following code line would create a new column named
/// `test` replacing the relevant quantity `Nsim` and the previously
/// existing column `probability`.
/// \code
/// d.Define("test", "Nsim * probability");
/// \endcode
///
ROOT::RDF::RNode TRestDataSet::Define(const std::string& columnName, const std::string& formula) {
    std::string evalFormula = formula;
    for (auto const& [name, properties] : fQuantity)
        evalFormula =
            REST_StringHelper::Replace(evalFormula, name, DoubleToString(properties.value, "%12.10e"));

    fDataSet = fDataSet.Define(columnName, evalFormula);

    return fDataSet;
}

/////////////////////////////////////////////
/// \brief Prints on screen the information about the metadata members of TRestDataSet
///
void TRestDataSet::PrintMetadata() {
    TRestMetadata::PrintMetadata();

    RESTMetadata << " - StartTime : " << REST_StringHelper::ToDateTimeString(fStartTime) << RESTendl;
    RESTMetadata << " - EndTime : " << REST_StringHelper::ToDateTimeString(fEndTime) << RESTendl;
    RESTMetadata << " - Path : " << TRestTools::SeparatePathAndName(fFilePattern).first << RESTendl;
    RESTMetadata << " - File pattern : " << TRestTools::SeparatePathAndName(fFilePattern).second << RESTendl;
    RESTMetadata << "  " << RESTendl;
    RESTMetadata << " - Accumulated run time (seconds) : " << fTotalDuration << RESTendl;
    RESTMetadata << " - Accumulated run time (hours) : " << fTotalDuration / 3600. << RESTendl;
    RESTMetadata << " - Accumulated run time (days) : " << fTotalDuration / 3600. / 24. << RESTendl;

    RESTMetadata << "  " << RESTendl;

    if (!fObservablesList.empty()) {
        RESTMetadata << " Single observables added:" << RESTendl;
        RESTMetadata << " -------------------------" << RESTendl;
        for (const auto& l : fObservablesList) RESTMetadata << " - " << l << RESTendl;

        RESTMetadata << "  " << RESTendl;
    }

    if (!fProcessObservablesList.empty()) {
        RESTMetadata << " Process observables added: " << RESTendl;
        RESTMetadata << " -------------------------- " << RESTendl;
        for (const auto& l : fProcessObservablesList) RESTMetadata << " - " << l << RESTendl;

        RESTMetadata << "  " << RESTendl;
    }

    if (!fFilterMetadata.empty()) {
        RESTMetadata << " Metadata filters: " << RESTendl;
        RESTMetadata << " ----------------- " << RESTendl;
        RESTMetadata << " - StartTime : " << fFilterStartTime << RESTendl;
        RESTMetadata << " - EndTime : " << fFilterEndTime << RESTendl;
        int n = 0;
        for (const auto& mdFilter : fFilterMetadata) {
            RESTMetadata << " - " << mdFilter << ".";

            if (!fFilterContains[n].empty()) RESTMetadata << " Contains: " << fFilterContains[n];
            if (fFilterGreaterThan[n] != -1) RESTMetadata << " Greater than: " << fFilterGreaterThan[n];
            if (fFilterLowerThan[n] != -1) RESTMetadata << " Lower than: " << fFilterLowerThan[n];
            if (fFilterEqualsTo[n] != -1) RESTMetadata << " Equals to: " << fFilterEqualsTo[n];

            RESTMetadata << RESTendl;
            n++;
        }

        RESTMetadata << "  " << RESTendl;
    }

    if (!fQuantity.empty()) {
        RESTMetadata << " Relevant quantities: " << RESTendl;
        RESTMetadata << " -------------------- " << RESTendl;

        int n = 0;
        for (auto const& [name, properties] : fQuantity) {
            RESTMetadata << " - Name : " << name << ". Value : " << properties.value
                         << ". Strategy: " << properties.strategy << RESTendl;
            RESTMetadata << " - Metadata: " << properties.metadata << RESTendl;
            RESTMetadata << " - Description: " << properties.description << RESTendl;

            RESTMetadata << " " << RESTendl;
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
        if (metadata.empty() || metadata == "Not defined") {
            RESTError << "Filter key defined without metadata member!" << RESTendl;
            exit(1);
        }

        fFilterMetadata.push_back(metadata);

        std::string contains = GetFieldValue("contains", filterDefinition);
        if (contains == "Not defined") contains = "";
        Double_t greaterThan = StringToDouble(GetFieldValue("greaterThan", filterDefinition));
        Double_t lowerThan = StringToDouble(GetFieldValue("lowerThan", filterDefinition));
        Double_t equalsTo = StringToDouble(GetFieldValue("equalsTo", filterDefinition));

        fFilterContains.push_back(contains);
        fFilterGreaterThan.push_back(greaterThan);
        fFilterLowerThan.push_back(lowerThan);
        fFilterEqualsTo.push_back(equalsTo);

        filterDefinition = GetNextElement(filterDefinition);
    }

    /// Reading observables
    TiXmlElement* observablesDefinition = GetElement("observables");
    while (observablesDefinition != nullptr) {
        std::string observables = GetFieldValue("list", observablesDefinition);
        if (observables.empty() || observables == "Not defined") {
            RESTError << "<observables key does not contain a list!" << RESTendl;
            exit(1);
        }

        std::vector<std::string> obsList = REST_StringHelper::Split(observables, ",");

        fObservablesList.insert(fObservablesList.end(), obsList.begin(), obsList.end());

        observablesDefinition = GetNextElement(observablesDefinition);
    }

    /// Reading process observables
    TiXmlElement* obsProcessDefinition = GetElement("processObservables");
    while (obsProcessDefinition != nullptr) {
        std::string observables = GetFieldValue("list", obsProcessDefinition);
        if (observables.empty() || observables == "Not defined") {
            RESTError << "<processObservables key does not contain a list!" << RESTendl;
            exit(1);
        }

        std::vector<std::string> obsList = REST_StringHelper::Split(observables, ",");

        for (const auto& l : obsList) fProcessObservablesList.push_back(l);

        obsProcessDefinition = GetNextElement(obsProcessDefinition);
    }

    /// Reading relevant quantities
    TiXmlElement* quantityDefinition = GetElement("quantity");
    while (quantityDefinition != nullptr) {
        std::string name = GetFieldValue("name", quantityDefinition);
        if (name.empty() || name == "Not defined") {
            RESTError << "<quantity key does not contain a name!" << RESTendl;
            exit(1);
        }

        std::string metadata = GetFieldValue("metadata", quantityDefinition);
        if (metadata.empty() || metadata == "Not defined") {
            RESTError << "<quantity key does not contain a metadata value!" << RESTendl;
            exit(1);
        }

        std::string strategy = GetFieldValue("strategy", quantityDefinition);
        if (strategy.empty() || strategy == "Not defined") {
            strategy = "unique";
        }

        std::string description = GetFieldValue("description", quantityDefinition);

        RelevantQuantity quantity;
        quantity.metadata = metadata;
        quantity.strategy = strategy;
        quantity.description = description;
        quantity.value = 0;

        fQuantity[name] = quantity;

        quantityDefinition = GetNextElement(quantityDefinition);
    }

    fCut = (TRestCut*)InstantiateChildMetadata("TRestCut");
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
void TRestDataSet::Export(const std::string& filename) {
    if (TRestTools::GetFileNameExtension(filename) == "txt" ||
        TRestTools::GetFileNameExtension(filename) == "csv") {
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

        FILE* f = fopen(filename.c_str(), "wt");
        ///// Writing header
        fprintf(f, "### TRestDataSet generated file\n");
        fprintf(f, "### \n");
        fprintf(f, "### StartTime : %s\n", fFilterStartTime.c_str());
        fprintf(f, "### EndTime : %s\n", fFilterEndTime.c_str());
        fprintf(f, "###\n");
        fprintf(f, "### Accumulated run time (seconds) : %lf\n", fTotalDuration);
        fprintf(f, "### Accumulated run time (hours) : %lf\n", fTotalDuration / 3600.);
        fprintf(f, "### Accumulated run time (days) : %lf\n", fTotalDuration / 3600. / 24.);
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
                if (fFilterEqualsTo[n] != -1) fprintf(f, " Equals to: %6.3lf.", fFilterLowerThan[n]);
                fprintf(f, "\n");
                n++;
            }
        }
        fprintf(f, "###\n");
        fprintf(f, "### Relevant quantities: \n");
        for (auto& [name, properties] : fQuantity) {
            fprintf(f, "### - %s : %lf - %s\n", name.c_str(), properties.value,
                    properties.description.c_str());
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

        auto obsNames = fDataSet.GetColumnNames();
        std::string obsListStr = "";
        for (const auto& l : obsNames) {
            if (!obsListStr.empty()) obsListStr += ":";
            obsListStr += l;
        }

        // We do this so that later we can recover the values using TTree::GetVal
        fTree->Draw((TString)obsListStr, "", "goff");

        for (unsigned int n = 0; n < fTree->GetEntries(); n++) {
            for (unsigned int m = 0; m < GetNumberOfBranches(); m++) {
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
    } else if (TRestTools::GetFileNameExtension(filename) == "root") {
        fDataSet.Snapshot("AnalysisTree", filename);

        TFile* f = TFile::Open(filename.c_str(), "UPDATE");
        this->Write();
        f->Close();
    } else {
        RESTWarning << "TRestDataSet::Export. Extension " << TRestTools::GetFileNameExtension(filename)
                    << " not recognized" << RESTendl;
    }
}

///////////////////////////////////////////////
/// \brief Operator to copy TRestDataSet metadata
///
TRestDataSet& TRestDataSet::operator=(TRestDataSet& dS) {
    SetName(dS.GetName());
    fFilterStartTime = dS.GetFilterStartTime();
    fFilterEndTime = dS.GetFilterEndTime();
    fStartTime = dS.GetStartTime();
    fEndTime = dS.GetEndTime();
    fFilePattern = dS.GetFilePattern();
    fObservablesList = dS.GetObservablesList();
    fProcessObservablesList = dS.GetProcessObservablesList();
    fFilterMetadata = dS.GetFilterMetadata();
    fFilterContains = dS.GetFilterContains();
    fFilterGreaterThan = dS.GetFilterGreaterThan();
    fFilterLowerThan = dS.GetFilterLowerThan();
    fFilterEqualsTo = dS.GetFilterEqualsTo();
    fQuantity = dS.GetQuantity();
    fTotalDuration = dS.GetTotalTimeInSeconds();
    fCut = dS.GetCut();

    return *this;
}

///////////////////////////////////////////////
/// \brief This function imports metadata from a root file
/// it import metadata info from the previous dataSet
/// while it opens the analysis tree
///
void TRestDataSet::Import(const std::string& fileName) {
    if (TRestTools::GetFileNameExtension(fileName) != "root") {
        RESTError << "Datasets can only be imported from root files" << RESTendl;
        return;
    }

    TFile* file = TFile::Open(fileName.c_str(), "READ");
    if (file != nullptr) {
        TIter nextkey(file->GetListOfKeys());
        TKey* key;
        while ((key = (TKey*)nextkey())) {
            std::string kName = key->GetClassName();
            if (REST_Reflection::GetClassQuick(kName.c_str()) != nullptr &&
                REST_Reflection::GetClassQuick(kName.c_str())->InheritsFrom("TRestDataSet")) {
                TRestDataSet* dS = file->Get<TRestDataSet>(key->GetName());
                if (GetVerboseLevel() >= TRestStringOutput::REST_Verbose_Level::REST_Info)
                    dS->PrintMetadata();
                *this = *dS;
            }
        }
    } else {
        RESTError << "Cannot open " << fileName << RESTendl;
        exit(1);
    }

    RESTInfo << "Opening " << fileName << RESTendl;
    fDataSet = ROOT::RDataFrame("AnalysisTree", fileName);

    fTree = (TTree*)file->Get("AnalysisTree");
}
