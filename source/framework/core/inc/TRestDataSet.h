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

#ifndef REST_TRestDataSet
#define REST_TRestDataSet

#include <TTimeStamp.h>

#include <ROOT/RDataFrame.hxx>

#include "TRestCut.h"
#include "TRestMetadata.h"

/// It allows to group a number of runs that satisfy given metadata conditions
class TRestDataSet : public TRestMetadata {
   public:
    struct RelevantQuantity {
        /// The associated metadata member used to register the relevant quantity
        std::string metadata;

        /// It determines how to produce the relevant quantity (accumulate/unique/last/max/min)
        std::string strategy;

        /// A user given description that can be used to define the relevant quantity
        std::string description;

        /// The quantity value
        std::string value;
    };

   private:
    /// All the selected runs will have a starting date after fStartTime
    std::string fFilterStartTime = "2000/01/01";  //<

    /// All the selected runs will have an ending date before fEndTime
    std::string fFilterEndTime = "3000/12/31";  //<

    /// A glob file pattern that must be satisfied by all files
    std::string fFilePattern = "";  //<

    /// It contains a list of the observables that will be added to the final tree or exported file
    std::vector<std::string> fObservablesList;  //<

    /// It contains a list of the process where all observables should be added
    std::vector<std::string> fProcessObservablesList;  //<

    /// A list of metadata members where filters will be applied
    std::vector<std::string> fFilterMetadata;  //<

    /// If not empty it will check if the metadata member contains the string
    std::vector<std::string> fFilterContains;  //<

    /// If the corresponding element is not empty it will check if the metadata member is greater
    std::vector<Double_t> fFilterGreaterThan;  //<

    /// If the corresponding element is not empty it will check if the metadata member is lower
    std::vector<Double_t> fFilterLowerThan;  //<

    /// If the corresponding element is not empty it will check if the metadata member is equal
    std::vector<Double_t> fFilterEqualsTo;  //<

    /// The properties of a relevant quantity that we want to store together with the dataset
    std::map<std::string, RelevantQuantity> fQuantity;  //<

    /// Parameter cuts over the selected dataset
    TRestCut* fCut = nullptr;  //<

    /// The total integrated run time of selected files
    Double_t fTotalDuration = 0;  //<

    /// A list populated by the FileSelection method using the conditions of the dataset
    std::vector<std::string> fFileSelection;  //<

    /// TimeStamp for the start time of the first file
    Double_t fStartTime = REST_StringHelper::StringToTimeStamp(fFilterEndTime);  //<

    /// TimeStamp for the end time of the last file
    Double_t fEndTime = REST_StringHelper::StringToTimeStamp(fFilterStartTime);  //<

    /// It keeps track if the generated dataset is a pure dataset or a merged one
    Bool_t fMergedDataset = false;  //<

    /// The list of dataset files imported
    std::vector<std::string> fImportedFiles;  //<

    /// A list of new columns together with its corresponding expressions added to the dataset
    std::vector<std::pair<std::string, std::string>> fColumnNameExpressions;  //<

    /// A flag to enable Multithreading during dataframe generation
    Bool_t fMT = false;  //<

    // If the dataframe was defined externally it will be true
    Bool_t fExternal = false;  //<

    /// The resulting RDF::RNode object after initialization
    ROOT::RDF::RNode fDataFrame = ROOT::RDataFrame(0);  //!

    /// A pointer to the generated tree
    TChain* fTree = nullptr;  //!

    void InitFromConfigFile() override;

   protected:
    virtual std::vector<std::string> FileSelection();

    void RegenerateTree(std::vector<std::string> finalList = {});

   public:
    /// Gives access to the RDataFrame
    ROOT::RDF::RNode GetDataFrame() const {
        if (!fExternal && fTree == nullptr)
            RESTWarning << "DataFrame has not been yet initialized" << RESTendl;
        return fDataFrame;
    }

    void EnableMultiThreading(Bool_t enable = true) { fMT = enable; }

    /// Gives access to the tree
    TTree* GetTree() const {
        if (fTree == nullptr && fExternal) {
            RESTInfo << "The tree is not accessible. Only GetDataFrame can be used in an externally "
                        "generated dataset"
                     << RESTendl;
            RESTInfo << "You may write a tree using GetDataFrame()->Snapshot(\"MyTree\", \"output.root\");"
                     << RESTendl;
            return fTree;
        }

        if (fTree == nullptr) {
            RESTError << "Tree has not been yet initialized" << RESTendl;
            RESTError << "You should invoke TRestDataSet::GenerateDataSet() or " << RESTendl;
            RESTError << "TRestDataSet::Import( fname ) before trying to access the tree" << RESTendl;
        }
        return fTree;
    }

    /// Number of variables (or observables)
    size_t GetNumberOfColumns() { return fDataFrame.GetColumnNames().size(); }

    /// Number of variables (or observables)
    size_t GetNumberOfBranches() { return GetNumberOfColumns(); }

    /// It returns a list of the files that have been finally selected
    std::vector<std::string> GetFileSelection() { return fFileSelection; }

    /// It returns the accumulated run time in seconds
    Double_t GetTotalTimeInSeconds() const { return fTotalDuration; }

    inline auto GetFilterStartTime() const { return fFilterStartTime; }
    inline auto GetFilterEndTime() const { return fFilterEndTime; }
    inline auto GetStartTime() const { return fStartTime; }
    inline auto GetEndTime() const { return fEndTime; }
    inline auto GetFilePattern() const { return fFilePattern; }
    inline auto GetObservablesList() const { return fObservablesList; }
    inline auto GetFileSelection() const { return fFileSelection; }
    inline auto GetProcessObservablesList() const { return fProcessObservablesList; }
    inline auto GetFilterMetadata() const { return fFilterMetadata; }
    inline auto GetFilterContains() const { return fFilterContains; }
    inline auto GetFilterGreaterThan() const { return fFilterGreaterThan; }
    inline auto GetFilterLowerThan() const { return fFilterLowerThan; }
    inline auto GetFilterEqualsTo() const { return fFilterEqualsTo; }
    inline auto GetQuantity() const { return fQuantity; }
    inline auto GetAddedColumns() const { return fColumnNameExpressions; }
    inline auto GetCut() const { return fCut; }
    inline auto IsMergedDataSet() const { return fMergedDataset; }

    inline void SetObservablesList(const std::vector<std::string>& obsList) { fObservablesList = obsList; }
    inline void SetFilePattern(const std::string& pattern) { fFilePattern = pattern; }
    inline void SetQuantity(const std::map<std::string, RelevantQuantity>& quantity) { fQuantity = quantity; }

    void SetTotalTimeInSeconds(Double_t seconds) { fTotalDuration = seconds; }
    void SetDataFrame(const ROOT::RDF::RNode& dS) {
        fDataFrame = dS;
        fExternal = true;
    }

    TRestDataSet& operator=(TRestDataSet& dS);
    Bool_t Merge(const TRestDataSet& dS);
    void Import(const std::string& fileName);
    void Import(std::vector<std::string> fileNames);
    void Export(const std::string& filename, std::vector<std::string> excludeColumns = {});

    ROOT::RDF::RNode MakeCut(const TRestCut* cut);
    ROOT::RDF::RNode ApplyRange(size_t from, size_t to);
    ROOT::RDF::RNode Range(size_t from, size_t to);
    ROOT::RDF::RNode DefineColumn(const std::string& columnName, const std::string& formula);

    size_t GetEntries();

    void PrintMetadata() override;
    void Initialize() override;

    void GenerateDataSet();

    TRestDataSet();
    TRestDataSet(const char* cfgFileName, const std::string& name = "");
    ~TRestDataSet();

    ClassDefOverride(TRestDataSet, 8);
};
#endif
