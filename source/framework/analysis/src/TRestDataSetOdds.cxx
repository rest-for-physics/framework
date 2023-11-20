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
/// TRestDataSetOdds performs the log odds of the different observables given
/// in the config file and for a particular dataSet. To perform the log odds
/// first the probability density funcion (PDF) is obtained for a set of
/// observables in the desired range. Later on, the log odds is computed as
/// log(1. - odds) - log(odds) obtaining a number which is proportional to
/// how likely is an event with respect the desired distribution; lower the number,
/// more likely is the event to the input distribution. New observables are created in
/// the output dataSet odds_obserbable and the addition of all of them in odds_total.
/// If an input odds file is provided, the different PDFs are retrieved from the input
/// file.
///
/// A summary of the basic parameters is described below:
/// * **dataSetName**: Name of the dataSet to be computed
/// * **oddsFile**: Name of the input odds file to be used for the definition of the
/// different PDFs.
///
/// The different observables, range and nBins are added as follow:
/// \code
/// <observable name="tckAna_MaxTrack_XYZ_SigmaZ2" range="(0,20)" nBins="100"/>
/// \endcode
/// * **name**: Name of the observable PDF to be computed
/// * **range**: Range of the observable PDF to be used
/// * **nBins**: Number of bins for the observable PDF
///
/// In addition a TRestCut is used as input for the generation of PDFs, check TRestCut
/// class for more info.
///
/// ### Examples
/// Example of RML config file:
/// \code
///     <TRestDataSetOdds name="LogOdds" verboseLevel="info">
///    <observable name="tckAna_MaxTrack_XYZ_SigmaZ2" range="(0,20)" nBins="100"/>
///    <observable name="tckAna_MaxTrackEnergyBalanceXY" range="(-5,5)" nBins="100"/>
///    <observable name="tckAna_MaxTrack_XZ_nHits" range="(0,20)" nBins="20" />
///    <observable name="tckAna_MaxTrack_YZ_nHits" range="(0,20)" nBins="20" />
///    <observable name="tckAna_MaxTrackEnergyRatio" range="(0,0.1)" nBins="100" />
///    <observable name="tckAna_MaxTrack_XZ_SigmaX" range="(0,10)" nBins="100" />
///    <observable name="tckAna_MaxTrack_YZ_SigmaY" range="(0,10)" nBins="100" />
///    <observable name="tckAna_MaxTrackxySigmaBalance" range="(-1,1)" nBins="25"/>
///    <TRestCut name="EnergyCut" verboseLevel="info">
///      <cut name="c1" variable="calib_Energy" condition=">4" />
///      <cut name="c2" variable="calib_Energy" condition="<8" />
///    </TRestCut>
///    <parameter name="dataSetName" value="myDataSet.root"/>
/// </TRestDataSetOdds>
/// \endcode
///
/// Example to compute the the odds over a dataSet using restRoot:
/// \code
/// [0] TRestDataSetOdds odds ("odds.rml");
/// [1] odds.SetDataSetName("myDataSet.root");
/// [2] odds.SetOutputFileName("myComputedOddsDataSet.root");
/// [3] odds.ComputeLogOdds();
/// \endcode
///
/// Example to compute the the odds over a dataSet with input odds file using restRoot:
/// \code
/// [0] TRestDataSetOdds odds ("odds.rml");
/// [1] odds.SetDataSetName("myDataSet.root");
/// [2] odds.SetOutputFileName("myComputedOddsDataSet.root");
/// [3] odds.odds.SetOddsFile("myOddsDataSet.root");
/// [4] odds.ComputeLogOdds();
/// \endcode
///
///----------------------------------------------------------------------
///
/// REST-for-Physics - Software for Rare Event Searches Toolkit
///
/// History of developments:
///
/// 2023-03: First implementation of TRestDataSetOdds
/// JuanAn Garcia
///
/// \class TRestDataSetOdds
/// \author: JuanAn Garcia   e-mail: juanangp@unizar.es
///
/// <hr>
///

#include "TRestDataSetOdds.h"

#include "TRestDataSet.h"

ClassImp(TRestDataSetOdds);

///////////////////////////////////////////////
/// \brief Default constructor
///
TRestDataSetOdds::TRestDataSetOdds() { Initialize(); }

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
/// corresponding TRestDataSetOdds section inside the RML.
///
TRestDataSetOdds::TRestDataSetOdds(const char* configFilename, std::string name)
    : TRestMetadata(configFilename) {
    LoadConfigFromFile(fConfigFileName, name);
    Initialize();

    if (GetVerboseLevel() >= TRestStringOutput::REST_Verbose_Level::REST_Info) PrintMetadata();
}

///////////////////////////////////////////////
/// \brief Default destructor
///
TRestDataSetOdds::~TRestDataSetOdds() {}

///////////////////////////////////////////////
/// \brief Function to initialize input/output event members and define
/// the section name
///
void TRestDataSetOdds::Initialize() { SetSectionName(this->ClassName()); }

///////////////////////////////////////////////
/// \brief Function to initialize some variables from
/// configfile
///
void TRestDataSetOdds::InitFromConfigFile() {
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

        std::string range = GetFieldValue("range", obsDefinition);
        if (range.empty() || range == "Not defined") {
            RESTError << "< observable key does not contain a range value!" << RESTendl;
            exit(1);
        } else {
            TVector2 roi = StringTo2DVector(range);
            fObsRange.push_back(roi);
        }

        std::string nBins = GetFieldValue("nBins", obsDefinition);
        if (nBins.empty() || nBins == "Not defined") {
            RESTError << "< observable key does not contain a nBins value!" << RESTendl;
            exit(1);
        } else {
            fObsNbins.push_back(StringToInteger(nBins));
        }

        obsDefinition = GetNextElement(obsDefinition);
    }

    if (fObsName.empty() || fObsRange.empty()) {
        RESTError << "No observables provided, exiting..." << RESTendl;
        exit(1);
    }

    if (fOutputFileName == "") fOutputFileName = GetParameter("outputFileName", "");

    fCut = (TRestCut*)InstantiateChildMetadata("TRestCut");
}

/////////////////////////////////////////////
/// \brief This function computes the log odds for
/// a given dataSet. If no calibration odds file is
/// provided it computes the PDF for the given
/// observables. Otherwise, it takes the PDF from the
/// input file. This function generate different observables
/// odds_obsName and the addition of all of them for a further
/// processing, which is stored in odds_total observable.
///
void TRestDataSetOdds::ComputeLogOdds() {
    PrintMetadata();

    TRestDataSet dataSet;
    dataSet.Import(fDataSetName);

    if (fOddsFile.empty()) {
        auto DF = dataSet.MakeCut(fCut);
        RESTInfo << "Generating PDFs for dataset: " << fDataSetName << RESTendl;
        for (size_t i = 0; i < fObsName.size(); i++) {
            const std::string obsName = fObsName[i];
            const TVector2 range = fObsRange[i];
            const std::string histName = "h" + obsName;
            const int nBins = fObsNbins[i];
            RESTDebug << "\tGenerating PDF for " << obsName << " with range: (" << range.X() << ", "
                      << range.Y() << ") and nBins: " << nBins << RESTendl;
            auto histo =
                DF.Histo1D({histName.c_str(), histName.c_str(), nBins, range.X(), range.Y()}, obsName);
            TH1F* h = static_cast<TH1F*>(histo->DrawClone());
            RESTDebug << "\tNormalizing by integral = " << h->Integral() << RESTendl;
            h->Scale(1. / h->Integral());
            fHistos[obsName] = h;
        }
    } else {
        TFile* f = TFile::Open(fOddsFile.c_str());
        if (f == nullptr) {
            RESTError << "Cannot open calibration odds file " << fOddsFile << RESTendl;
            exit(1);
        }
        RESTInfo << "Opening " << fOddsFile << " as oddsFile." << RESTendl;
        for (size_t i = 0; i < fObsName.size(); i++) {
            const std::string obsName = fObsName[i];
            const std::string histName = "h" + obsName;
            TH1F* h = (TH1F*)f->Get(histName.c_str());
            fHistos[obsName] = h;
        }
    }

    auto df = dataSet.GetDataFrame();
    std::string totName = "";
    RESTDebug << "Computing log odds from " << fDataSetName << RESTendl;
    for (const auto& [obsName, histo] : fHistos) {
        const std::string oddsName = "odds_" + obsName;
        auto GetLogOdds = [&histo = histo](double val) {
            double odds = histo->GetBinContent(histo->GetXaxis()->FindBin(val));
            if (odds == 0) return 1000.;
            return log(1. - odds) - log(odds);
        };

        if (df.GetColumnType(obsName) != "Double_t") {
            RESTWarning << "Column " << obsName << " is not of type 'double'. It will be converted."
                        << RESTendl;
            df = df.Redefine(obsName, "static_cast<double>(" + obsName + ")");
        }
        df = df.Define(oddsName, GetLogOdds, {obsName});
        auto h = df.Histo1D(oddsName);

        if (!totName.empty()) totName += "+";
        totName += oddsName;
    }

    RESTDebug << "Computing total log odds" << RESTendl;
    RESTDebug << "\tTotal log odds = " << totName << RESTendl;
    df = df.Define("odds_total", totName);

    dataSet.SetDataFrame(df);

    if (!fOutputFileName.empty()) {
        if (TRestTools::GetFileNameExtension(fOutputFileName) == "root") {
            RESTDebug << "Exporting dataset to " << fOutputFileName << RESTendl;
            dataSet.Export(fOutputFileName);
            TFile* f = TFile::Open(fOutputFileName.c_str(), "UPDATE");
            this->Write();
            RESTDebug << "Writing histograms to " << fOutputFileName << RESTendl;
            for (const auto& [obsName, histo] : fHistos) histo->Write();
            f->Close();
        }
    }
}

/////////////////////////////////////////////
/// \brief Prints on screen the information about the metadata members of TRestDataSetOdds
///
void TRestDataSetOdds::PrintMetadata() {
    TRestMetadata::PrintMetadata();

    RESTMetadata << " Observables to compute: " << RESTendl;
    for (size_t i = 0; i < fObsName.size(); i++) {
        RESTMetadata << fObsName[i] << "; Range: (" << fObsRange[i].X() << ", " << fObsRange[i].Y()
                     << "); nBins: " << fObsNbins[i] << RESTendl;
    }
    RESTMetadata << "----" << RESTendl;
}
