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
/// TRestDataSetTMVA is meant to evaluate different TMVA methods in datasets.
/// For more information about TMVA, check https://root.cern.ch/download/doc/tmva/TMVAUsersGuide.pdf
/// So far, only Likelihood, LikelihoodKDE, Fisher, BDT and MLP methods are
/// supported. TMVA requires a signal and a background dataset from which the
/// different TMVA methods are computed. The different methods are evaluated
/// in a set of observables that are provided in the RML file. Different cuts
/// can be performed in either the signal or the background datasets prior to
/// the TMVA evaluation. The output of this class is a root file which contains
/// a signal and a background tree with the cuts applied and the different observables
/// that are generated with the TMVA analysis. In addition, a folder is created
/// with different xml files that contain the output of the TMVA evaluation that
/// can be used to compute the TMVA classification via TRestDataSetTMVAClassification.
///
/// A summary of the basic parameters is described below:
/// * **outputFileName**: Name of the output file
/// * **dataSetSignal**: Name of the dataset file containing the signal
/// * **dataSetBackground**: Name of the dataset file containing the background
/// * **outputPath**: Name of the output path with the evaluation results
/// * **drawROCCurve**: If true display the ROC curve for the evaluation of all methods
///
/// The different observables for the TMVA analysis can be added with the following key:
/// \code
/// <observable name="tckAna_MaxTrack_XYZ_SigmaZ2" />
/// \endcode
///
/// * **name**: Name of the observable be computed
///
/// The different signal and background cuts can be added awith the following key:
/// \code
///   <addBackgroundCut name="ParamCut"/>
///   <addSignalCut name="ParamCut"/>
///   <addSignalCut name="EnergyCut"/>
/// \endcode
///
/// Where the cut name (e.g. ParamCut or Energy cut from above) have to be defined inside
/// the RML file, e.g.:
/// \code
///   <TRestCut name="ParamCut" verboseLevel="info">
///     <cut name="c1" variable="tckAna_MaxTrackEnergyRatio" condition="<0.1" />
///     <cut name="c2" variable="tckAna_MaxTrack_XYZ_SigmaZ2" condition="<20." />
///     <cut name="c3" variable="tckAna_MaxTrackEnergyBalanceXY" condition="<5"/>
///     <cut name="c4" variable="tckAna_MaxTrackEnergyBalanceXY" condition=">-5"/>
///     <cut name="c5" variable="tckAna_MaxTrackxySigmaBalance" condition=">-1"/>
///     <cut name="c6" variable="tckAna_MaxTrackxySigmaBalance" condition="<1"/>
///   </TRestCut>
/// \endcode
///
/// Please, check TRestCut class for more info.
///
/// The different TMVA methods can be added wit the following key:
/// \code
///  <addMethod name="MLP"
///  parameters="!H:!V:NeuronType=tanh:VarTransform=N:NCycles=100:HiddenLayers=N+5:TestRate=5:!UseRegulator"/>
/// \endcode
/// The different parameters for adding TMVA methods are described below:
/// * **name**: Name of the TMVA method, only Likelihood, LikelihoodKDE, Fisher, BDT and MLP
/// are supported so far.
/// * **parameters**: String parameters to be used in the TMVA method, for more information
/// check https://root.cern.ch/download/doc/tmva/TMVAUsersGuide.pdf
///
/// ### Examples
/// Example of RML config file:
/// \code
/// <TRestDataSetTMVA name="TMVA" verboseLevel="info">
///    <observable name="tckAna_MaxTrack_XYZ_SigmaZ2"/>
///    <observable name="tckAna_MaxTrackEnergyBalanceXY"/>
///    <observable name="tckAna_MaxTrackEnergyRatio"/>
///    <observable name="tckAna_MaxTrack_XZ_SigmaX" />
///    <observable name="tckAna_MaxTrack_YZ_SigmaY" />
///    <observable name="tckAna_MaxTrackxySigmaBalance"/>
///    <addBackgroundCut name="ParamCut"/>
///    <addSignalCut name="ParamCut"/>
///    <addSignalCut name="EnergyCut"/>
///    <TRestCut name="EnergyCut" verboseLevel="info">
///      <cut name="c1" variable="calib_Energy" condition=">4" />
///      <cut name="c2" variable="calib_Energy" condition="<8" />
///    </TRestCut>
///   <TRestCut name="ParamCut" verboseLevel="info">
///      <cut name="c3" variable="tckAna_nTracks_X" condition=">0" />
///      <cut name="c4" variable="tckAna_nTracks_Y" condition=">0" />
///      <cut name="c5" variable="tckAna_MaxTrackEnergyRatio" condition="<0.1" />
///      <cut name="c8" variable="tckAna_MaxTrack_XYZ_SigmaZ2" condition="<20." />
///      <cut name="c9" variable="tckAna_MaxTrackEnergyBalanceXY" condition="<5"/>
///      <cut name="c10" variable="tckAna_MaxTrackEnergyBalanceXY" condition=">-5"/>
///      <cut name="c11" variable="tckAna_MaxTrackxySigmaBalance" condition=">-1"/>
///      <cut name="c12" variable="tckAna_MaxTrackxySigmaBalance" condition="<1"/>
///    </TRestCut>
///    <addMethod name="Likelihood"
///    parameters="H:!V:TransformOutput:PDFInterpol=Spline2:NSmoothSig[0]=20:NSmoothBkg[0]=10:NSmoothBkg[1]=5:NSmooth=1:NAvEvtPerBin=10"/>
///    <addMethod name="LikelihoodKDE"
///    parameters="!H:!V:!TransformOutput:PDFInterpol=KDE:KDEtype=Gauss:KDEiter=Adaptive:KDEFineFactor=0.3:KDEborder=None:NAvEvtPerBin=10"/>
///    <addMethod name="Fisher"
///    parameters="H:!V:Fisher:VarTransform=None:CreateMVAPdfs:PDFInterpolMVAPdf=Spline2:NbinsMVAPdf=50:NsmoothMVAPdf=10"/>
///    <addMethod name="BDT"
///    parameters="!V:NTrees=200:MinNodeSize=2.5%:MaxDepth=2:BoostType=AdaBoost:AdaBoostBeta=0.5:UseBaggedBoost:BaggedSampleFraction=0.5:SeparationType=GiniIndex:nCuts=20"/>
///    <addMethod name="MLP"
///    parameters="!H:!V:NeuronType=tanh:VarTransform=N:NCycles=100:HiddenLayers=N+5:TestRate=5:!UseRegulator"/>
/// </TRestDataSetTMVA>
/// \endcode
///
/// Example to perform TMVA evaluation using restRoot:
/// \code
/// [0] TRestDataSetTMVA tmva("tmva.rml");
/// [1] tmva.SetDataSetSignal("DataSetSignal.root");
/// [2] tmva.SetDataSetBackground("DataSetBackground.root");
/// [3] tmva.SetOutputFileName("MyDataSetEvaluation.root");
/// [4] tmva.SetOutputPath("MyDataSetFiles");
/// [5] tmva.ComputeTMVA();
/// \endcode
///
/// In addition it is possible to display TMVA results after evaluating all methods,
/// using root or restRoot;
/// \code
/// [0] TMVA::TMVAGui("MyDataSetEvaluation.root")
/// \endcode
///
///----------------------------------------------------------------------
///
/// REST-for-Physics - Software for Rare Event Searches Toolkit
///
/// History of developments:
///
/// 2023-05: First implementation of TRestDataSetTMVA
/// JuanAn Garcia
///
/// \class TRestDataSetTMVA
/// \author: JuanAn Garcia   e-mail: juanangp@unizar.es
///
/// <hr>
///

#include "TRestDataSetTMVA.h"

#include "ROOT/RDFHelpers.hxx"
#include "TMVA/CrossValidation.h"
#include "TMVA/DataLoader.h"
#include "TMVA/Factory.h"
#include "TMVA/TMVAGui.h"
#include "TMVA/Tools.h"
#include "TRestDataSet.h"

ClassImp(TRestDataSetTMVA);

///////////////////////////////////////////////
/// \brief Default constructor
///
TRestDataSetTMVA::TRestDataSetTMVA() { Initialize(); }

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
/// corresponding TRestDataSetTMVA section inside the RML.
///
TRestDataSetTMVA::TRestDataSetTMVA(const char* configFilename, std::string name)
    : TRestMetadata(configFilename) {
    LoadConfigFromFile(fConfigFileName, name);
    Initialize();

    if (GetVerboseLevel() >= TRestStringOutput::REST_Verbose_Level::REST_Info) PrintMetadata();
}

///////////////////////////////////////////////
/// \brief Default destructor
///
TRestDataSetTMVA::~TRestDataSetTMVA() {}

///////////////////////////////////////////////
/// \brief Function to initialize input/output event members and define
/// the section name
///
void TRestDataSetTMVA::Initialize() { SetSectionName(this->ClassName()); }

///////////////////////////////////////////////
/// \brief Function to initialize some variables from
/// configfile
///
void TRestDataSetTMVA::InitFromConfigFile() {
    Initialize();
    TRestMetadata::InitFromConfigFile();

    TiXmlElement* obsDefinition = GetElement("observable");
    while (obsDefinition != nullptr) {
        std::string obsName = GetFieldValue("name", obsDefinition);
        if (obsName.empty() || obsName == "Not defined") {
            RESTError << "< observable variable key does not contain a name!" << RESTendl;
            exit(1);
        } else {
            fObsName.push_back(obsName);
        }

        obsDefinition = GetNextElement(obsDefinition);
    }

    TiXmlElement* cutele = GetElement("addBackgroundCut");
    while (cutele != nullptr) {
        std::string cutName = GetParameter("name", cutele, "");
        if (!cutName.empty()) {
            if (fBackgroundCut == nullptr) {
                fBackgroundCut = (TRestCut*)InstantiateChildMetadata("TRestCut", cutName);
            } else {
                fBackgroundCut->AddCut((TRestCut*)InstantiateChildMetadata("TRestCut", cutName));
            }
        }
        cutele = GetNextElement(cutele);
    }

    cutele = GetElement("addSignalCut");
    while (cutele != nullptr) {
        std::string cutName = GetParameter("name", cutele, "");
        if (!cutName.empty()) {
            if (fSignalCut == nullptr) {
                fSignalCut = (TRestCut*)InstantiateChildMetadata("TRestCut", cutName);
            } else {
                fSignalCut->AddCut((TRestCut*)InstantiateChildMetadata("TRestCut", cutName));
            }
        }
        cutele = GetNextElement(cutele);
    }

    TiXmlElement* method = GetElement("addMethod");
    while (method != nullptr) {
        std::string name = GetParameter("name", method, "");
        std::string params = GetParameter("parameters", method, "");
        if (name.empty() || params.empty()) {
            RESTWarning << "Empty method" << RESTendl;
        } else {
            fMethod.push_back(std::make_pair(name, params));
        }
        method = GetNextElement(method);
    }

    if (fObsName.empty()) {
        RESTError << "No observables provided, exiting..." << RESTendl;
        exit(1);
    }

    if (fOutputFileName == "") fOutputFileName = GetParameter("outputFileName", "");
}

/////////////////////////////////////////////
/// \brief This function computes the TMVA using
/// the different methods provided via config file
/// and the signal and background dataSets. The results
/// are stored in an output root file and a folder. Note
/// that it doesn't provide any usable dataset, just standard
/// root files.
///
void TRestDataSetTMVA::ComputeTMVA() {
    if (fOutputFileName.empty() || fOutputPath.empty() || fDataSetSignal.empty() ||
        fDataSetBackground.empty()) {
        RESTError << "Empty output file name, path, signal or background files " << RESTendl;
        PrintMetadata();
        exit(1);
    }

    if (fMethod.empty()) {
        RESTError << "No TMVA methods have been added " << RESTendl;
        PrintMetadata();
        exit(1);
    }

    // Add signal dataset
    TRestDataSet signal;
    signal.Import(fDataSetSignal);
    auto dfSignal = signal.MakeCut(fSignalCut);
    dfSignal.Snapshot("Signal", fOutputFileName);

    // Add background dataset
    TRestDataSet bck;
    bck.Import(fDataSetBackground);
    auto dfBackground = bck.MakeCut(fBackgroundCut);
    ROOT::RDF::RSnapshotOptions opt;
    opt.fMode = "update";
    dfBackground.Snapshot("Background", fOutputFileName, "", opt);

    auto outputFile = TFile::Open(fOutputFileName.c_str(), "UPDATE");

    auto signalTree = outputFile->Get<TTree>("Signal");
    auto bckTree = outputFile->Get<TTree>("Background");

    TMVA::Factory factory("TMVA_Classification", outputFile,
                          "!V:ROC:!Silent:Color:AnalysisType=Classification");

    TMVA::DataLoader loader(fOutputPath);

    // Add observables for the evaluation
    for (const auto& obs : fObsName) loader.AddVariable(obs);

    loader.AddSignalTree(signalTree, 1.0);
    loader.AddBackgroundTree(bckTree, 1.0);
    loader.PrepareTrainingAndTestTree("", "",
                                      ":SplitMode=Random"
                                      ":NormMode=NumEvents"
                                      ":!V");

    // Add different TMVA methods
    for (const auto& [name, params] : fMethod) {
        auto it = fMethodMap.find(name);
        if (it == fMethodMap.end()) {
            RESTWarning << "Method " << name << " not supported " << RESTendl;
            RESTWarning << "Currently supported methods: ";
            for (const auto& [method, val] : fMethodMap) RESTWarning << method << ", ";
            RESTWarning << RESTendl;
            continue;
        }
        std::cout << "Added method " << name << " " << it->second << " " << params << std::endl;
        factory.BookMethod(&loader, it->second, name.c_str(), params.c_str());
    }

    // Train, test and evaluate all methods
    factory.TrainAllMethods();
    factory.TestAllMethods();
    factory.EvaluateAllMethods();

    // Draw ROC curve
    if (fDrawROCCurve && gApplication != nullptr && gApplication->IsRunning()) {
        auto c1 = factory.GetROCCurve(&loader);
        c1->Draw();
    }

    outputFile->Close();
}

/////////////////////////////////////////////
/// \brief Prints on screen the information about the metadata members of TRestDataSetTMVA
///
void TRestDataSetTMVA::PrintMetadata() {
    TRestMetadata::PrintMetadata();

    RESTMetadata << " Observables to compute: " << RESTendl;
    for (const auto& obs : fObsName) {
        RESTMetadata << obs << RESTendl;
    }
    RESTMetadata << " TMVA Methods " << RESTendl;
    for (const auto& [name, params] : fMethod) {
        RESTMetadata << name << " " << params << RESTendl;
    }
    RESTMetadata << "----" << RESTendl;
}
