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
/// Documentation TOBE written
///
///
///----------------------------------------------------------------------
///
/// REST-for-Physics - Software for Rare Event Searches Toolkit
///
/// History of developments:
///
/// 2022-December: First implementation of TRestSensitivity
/// Javier Galan
///
/// \class TRestSensitivity
/// \author: Javier Galan (javier.galan.lacarra@cern.ch)
///
/// <hr>
///
#include <TRestExperimentList.h>
#include <TRestSensitivity.h>

ClassImp(TRestSensitivity);

///////////////////////////////////////////////
/// \brief Default constructor
///
TRestSensitivity::TRestSensitivity() { Initialize(); }

///////////////////////////////////////////////
/// \brief Default destructor
///
TRestSensitivity::~TRestSensitivity() {}

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
TRestSensitivity::TRestSensitivity(const char *cfgFileName, const std::string &name)
    : TRestMetadata(cfgFileName) {
    LoadConfigFromFile(fConfigFileName, name);
}

///////////////////////////////////////////////
/// \brief It will initialize the data frame with the filelist and column names
/// (or observables) that have been defined by the user.
///
void TRestSensitivity::Initialize() { SetSectionName(this->ClassName()); }

///////////////////////////////////////////////
/// \brief It will return a value of the coupling, g4, such that (chi-chi0) gets
/// closer to the target value given by argument. The factor will be used to
/// increase or decrease the coupling, and evaluate the likelihood.
///
Double_t TRestSensitivity::ApproachByFactor(Double_t g4, Double_t chi0, Double_t target, Double_t factor) {
    if (factor == 1) {
        return 0;
    }

    /// Coarse movement to get to Chi2 above target
    Double_t Chi2 = 0;
    do {
        Chi2 = 0;
        for (const auto &exp : fExperiments) Chi2 += -2 * UnbinnedLogLikelihood(exp, g4);

        g4 = factor * g4;
    } while (Chi2 - chi0 < target);
    g4 = g4 / factor;

    /// Coarse movement to get to Chi2 below target (/2)
    do {
        Chi2 = 0;
        for (const auto &exp : fExperiments) Chi2 += -2 * UnbinnedLogLikelihood(exp, g4);

        g4 = g4 / factor;
    } while (Chi2 - chi0 > target);

    return g4 * factor;
}

///////////////////////////////////////////////
/// \brief It will return the coupling value for which Chi=sigma
///
Double_t TRestSensitivity::GetCoupling(Double_t sigma, Double_t precision) {
    Double_t Chi2_0 = 0;
    for (const auto &exp : fExperiments) Chi2_0 += -2 * UnbinnedLogLikelihood(exp, 0);

    Double_t target = sigma * sigma;

    Double_t g4 = 0.5;

    g4 = ApproachByFactor(g4, Chi2_0, target, 2);
    g4 = ApproachByFactor(g4, Chi2_0, target, 1.2);
    g4 = ApproachByFactor(g4, Chi2_0, target, 1.02);
    g4 = ApproachByFactor(g4, Chi2_0, target, 1.0002);

    return g4;
}

///////////////////////////////////////////////
/// \brief It returns the Log(L) for the experiment and coupling given by argument.
///
Double_t TRestSensitivity::UnbinnedLogLikelihood(const TRestExperiment *experiment, Double_t g4) {
    Double_t lhood = 0;
    if (!experiment->IsDataReady()) {
        RESTError << "TRestSensitivity::UnbinnedLogLikelihood. Experiment " << experiment->GetName()
                  << " is not ready!" << RESTendl;
        return lhood;
    }

    Double_t signal = g4 * experiment->GetSignal()->GetTotalRate() * experiment->GetExposureInSeconds();

    lhood = -signal;

    if (ROOT::IsImplicitMTEnabled()) ROOT::DisableImplicitMT();

    std::vector<std::vector<Double_t>> trackingData;
    for (const auto &var : experiment->GetSignal()->GetVariables()) {
        auto values = experiment->GetExperimentalDataFrame().Take<Double_t>(var);
        std::vector<Double_t> vDbl = std::move(*values);
        trackingData.push_back(vDbl);
    }

    for (size_t n = 0; n < trackingData[0].size(); n++) {
        std::vector<Double_t> point;
        for (size_t m = 0; m < trackingData.size(); m++) point.push_back(trackingData[m][n]);

        Double_t bckRate = experiment->GetBackground()->GetRate(point);
        Double_t sgnlRate = experiment->GetSignal()->GetRate(point);

        Double_t expectedRate = bckRate + g4 * sgnlRate;
        lhood += TMath::Log(expectedRate);
    }

    return lhood;
}

///////////////////////////////////////////////
/// \brief
///
TH1D *TRestSensitivity::SignalStatisticalTest(Int_t N) {

    std::vector<Double_t> couplings;
    for (int n = 0; n < N; n++) {
        for (const auto &exp : fExperiments) exp->GetSignal()->RegenerateActiveNodeDensity();

        Double_t coupling = TMath::Sqrt(TMath::Sqrt(GetCoupling()));
        couplings.push_back(coupling);
    }

    // Directly assign the minimum and maximum values
    double min_value = *std::min_element(couplings.begin(), couplings.end());
    double max_value = *std::max_element(couplings.begin(), couplings.end());

    if (fSignalTest) delete fSignalTest;
    fSignalTest = new TH1D("SignalTest", "A signal test", 100, 0.9 * min_value, 1.1 * max_value);
    for (const auto &coup : couplings) fSignalTest->Fill(coup);

    return fSignalTest;
}

/////////////////////////////////////////////
/// \brief It customizes the retrieval of XML data values of this class
///
void TRestSensitivity::InitFromConfigFile() {
    TRestMetadata::InitFromConfigFile();

    int cont = 0;
    TRestMetadata *metadata = (TRestMetadata *)this->InstantiateChildMetadata(cont, "Experiment");
    while (metadata != nullptr) {
        cont++;
        if (metadata->InheritsFrom("TRestExperimentList")) {
            TRestExperimentList *experimentsList = (TRestExperimentList *)metadata;
            std::vector<TRestExperiment *> exList = experimentsList->GetExperiments();
            fExperiments.insert(fExperiments.end(), exList.begin(), exList.end());
        } else if (metadata->InheritsFrom("TRestExperiment")) {
            fExperiments.push_back((TRestExperiment *)metadata);
        }

        metadata = (TRestMetadata *)this->InstantiateChildMetadata(cont, "Experiment");
    }

    Initialize();
}

/////////////////////////////////////////////
/// \brief Prints on screen the information about the metadata members of TRestAxionSolarFlux
///
void TRestSensitivity::PrintMetadata() {
    TRestMetadata::PrintMetadata();

    RESTMetadata << "----" << RESTendl;
}
