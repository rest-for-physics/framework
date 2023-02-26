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
/// This class is used to merge different TRestRun files inside one file.
/// The merging is performed using TFileMerger class from ROOT, all files
/// with the matching pattern are merged. However, further checks are
/// performed in order to avoid different run numbers to be merged in the
/// same file. It is not a good practise to merger files with a big size,
/// consider to reduce the size of the file before attempting to merge.
///
/// ### Parameters
/// Describe any parameters this process receives:
/// * **inputFilePattern **: Pattern from which the input files to be
/// merged are generated. If not provided it checks the the input file
/// name at restManager
/// * **outputFileName**: Name of the output file to be generated. If
/// not provides tries to generate the output file from the TRestRun
/// section inside the rml file.
/// * **removeInputFiles**: If true the input files that have been
/// merged are removed.
///
/// ### Examples
/// This example merge the input files that matches the following pattern
/// e.g. R00622_00XXX_EventSelection_LSC_Calibrationn_user_2.3.15.root
/// To merge the files with this pattern you should run the following cmd
/// \code
///  restManager --c ../RML/mergeFiles.rml --f "../data/R00622_000*EventSelection*.root"
/// \endcode
/// The rml file is below, note that the TRestRun section is required in order
/// to deduce the output file name from the input format.
/// \code
/// Note that in the example below the run number
/// <TRestRun name="IAXOD1" title="IAXOD1" verboseLevel="debug">
///	<parameter name="experiment" value="IAXO"/>
///	<parameter name="runNumber" value="preserve"/>
///	<parameter name="runTag" value="preserve"/>
///	<parameter name="runType" value="${RUN_TYPE}" overwrite="true"/>
///	<parameter name="runDescription" value=""/>
///	<parameter name="user" value="${USER}"/>
///	<parameter name="verboseLevel" value="0"/>
///	<parameter name="overwrite" value="off" />
///
///	<parameter name="inputFormat"
///value="R[fRunNumber]_[fParentRunNumber]_[fRunType]_LSC_[fRunTag]_[fRunUser]_[fVersion].root"/> 	<parameter
///name="outputFileName" value="R[fRunNumber]_EventSelection_LSC_[fRunTag]_[fRunUser]_[fVersion].root" />
///	<parameter name="readOnly" value="false" />
///</TRestRun>
///
///	<TRestRunMerger name="Merger" title="File merger" verboseLevel="debug">
///	</TRestRunMerger>
///	<addTask command="Merger->MergeFiles()" value="ON" />
///
/// \endcode
///
/// The example below generates the following file name:
/// R00622_EventSelection_LSC_Calibration_user_2.3.15.root
///
/// It is possible to merge files from a macro, the example below merge
/// all the files following the file pattern:
///  \code
///    Int_t REST_MergeFiles(TString pathAndPattern, TString outputFilename) {
///    vector<string> files = TRestTools::GetFilesMatchingPattern((string)pathAndPattern);
///    TRestRunMerger merger;
///    return merger.MergeFiles(files, std::string(outputFilename.Data() ));
///    }
///  \endcode
///
///----------------------------------------------------------------------
///
/// REST-for-Physics - Software for Rare Event Searches Toolkit
///
/// History of developments:
///
/// 2023-February First implementation
/// JuanAn Garcia
///
/// \class TRestRunMerger
/// \author: JuanAn Garcia   e-mail: juanangp@unizar.es
///
/// <hr>
///

#include "TRestRunMerger.h"

#include <TSystem.h>

#include "TRestRun.h"

ClassImp(TRestRunMerger);

///////////////////////////////////////////////
/// \brief Default constructor
///
TRestRunMerger::TRestRunMerger() { Initialize(); }

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
/// \param configFilename A const char* that defines the RML filename.
/// \param name The name of the metadata section. It will be used to find the
/// corresponding TRestRunMerger section inside the RML.
///
TRestRunMerger::TRestRunMerger(const char* configFilename, std::string name) : TRestMetadata(configFilename) {
    LoadConfigFromFile(fConfigFileName, name);
}

///////////////////////////////////////////////
/// \brief Default destructor
///
TRestRunMerger::~TRestRunMerger() {}

///////////////////////////////////////////////
/// \brief Function to initialize input/output event members and define
/// the section name
///
void TRestRunMerger::Initialize() { SetSectionName(this->ClassName()); }

///////////////////////////////////////////////
/// \brief Function to initialize some variables from
/// configfile, in case that the variable is not found
/// in the rml it checks the input and output name from
/// restManager. In case the output name is empty
///
void TRestRunMerger::InitFromConfigFile() {
    Initialize();
    TRestMetadata::InitFromConfigFile();

    if (fInputFilePattern == "") fInputFilePattern = GetParameter("inputFileName", "");
    if (fOutputFileName == "") fOutputFileName = GetParameter("outputFileName", "");

    if (fOutputFileName == "") {
        TRestRun restRun(fConfigFileName);
        auto fileNames = TRestTools::GetFilesMatchingPattern(std::string(fInputFilePattern));
        restRun.OpenInputFile(fileNames.front().c_str());
        fOutputFileName = restRun.FormFormat(restRun.GetOutputFileName());
    }
}

///////////////////////////////////////////////
/// \brief This function is a wrapper to merge the
/// different input files when it is called from
/// the rml file
///
void TRestRunMerger::MergeFiles() {
    auto fileNames = TRestTools::GetFilesMatchingPattern(std::string(fInputFilePattern));

    MergeFiles(fileNames, std::string(fOutputFileName), fRemoveInputFiles);
}

///////////////////////////////////////////////
/// \brief This function merge the different input
/// files inside the outputFile
///
/// \param files vector with the name of the files
/// that are meant to be merged
///
/// \param outputFileName is the name of the output
/// file that is generated after merging
///
/// \param removeInputFiles if true it erases the input
/// files that have been merged
///
Int_t TRestRunMerger::MergeFiles(const std::vector<std::string>& files, std::string outputFileName,
                                 bool removeInputFiles) {
    RESTDebug << "TRestRunMerger::Merge target : " << outputFileName << RESTendl;

    if (files.size() <= 1) {
        RESTError << "Cannot merge less than two files" << RESTendl;
        return 1;
    }

    int runNumber = -1;
    double startTime = -1;
    std::vector<std::string> filesAdd;

    // Check how many files matches
    for (const auto& f : files) {
        TRestRun cRun(f.c_str());
        if (runNumber == -1 && startTime == -1) {
            startTime = cRun.GetStartTimestamp();
            runNumber = cRun.GetRunNumber();
        }

        if (runNumber == cRun.GetRunNumber()) {
            filesAdd.push_back(f);
        } else {
            RESTWarning << "Run number mistmatch " << runNumber << " vs " << cRun.GetRunNumber()
                        << " only runs with same run number will be merged" << RESTendl;
            break;
        }
    }

    const int nMerged = filesAdd.size();

    if (nMerged <= 1) {
        RESTError << "Only found " << nMerged << " that matches, skipping" << RESTendl;
        return nMerged;
    }

    // Merge the files
    TFileMerger m(kFALSE);
    m.OutputFile(outputFileName.c_str(), "RECREATE");

    for (const auto& f : filesAdd) {
        m.AddFile(f.c_str());
        RESTDebug << "Adding file " << f << RESTendl;
    }

    m.Merge();

    TRestRun mergedRun(outputFileName.c_str());
    mergedRun.SetOutputFileName(outputFileName);
    mergedRun.SetStartTimeStamp(startTime);
    mergedRun.OpenAndUpdateOutputFile();
    mergedRun.PrintMetadata();

    if (removeInputFiles) {
        for (const auto& f : filesAdd) {
            gSystem->Unlink(f.c_str());
        }
    }

    return nMerged;
}

/////////////////////////////////////////////
/// \brief Prints on screen the information about the different metadata members
///
void TRestRunMerger::PrintMetadata() {
    TRestMetadata::PrintMetadata();

    RESTMetadata << " Input file pattern : " << fInputFilePattern << RESTendl;
    RESTMetadata << " Output file name : " << fOutputFileName << RESTendl;
    if (fRemoveInputFiles) RESTMetadata << " Input files will be REMOVED" << RESTendl;
    RESTMetadata << RESTendl;
}
