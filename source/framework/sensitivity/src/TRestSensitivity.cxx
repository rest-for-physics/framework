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
TRestSensitivity::TRestSensitivity(const char* cfgFileName, const std::string& name)
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
Double_t TRestSensitivity::ApproachByFactor(Double_t node, Double_t g4, Double_t chi0, Double_t target,
                                            Double_t factor) {
    if (factor == 1) {
        return 0;
    }

    /// Coarse movement to get to Chi2 above target
    Double_t Chi2 = 0;
    do {
        Chi2 = 0;
        for (const auto& exp : fExperiments) Chi2 += -2 * UnbinnedLogLikelihood(exp, node, g4);

        g4 = factor * g4;
    } while (Chi2 - chi0 < target);
    g4 = g4 / factor;

    /// Coarse movement to get to Chi2 below target (/2)
    do {
        Chi2 = 0;
        for (const auto& exp : fExperiments) Chi2 += -2 * UnbinnedLogLikelihood(exp, node, g4);

        g4 = g4 / factor;
    } while (Chi2 - chi0 > target);

    return g4 * factor;
}

void TRestSensitivity::GenerateCurve() {
    ExtractExperimentParameterizationNodes();

    if (GetNumberOfCurves() > 0)
        for (const auto& exp : fExperiments) {
            exp->GenerateMockDataSet();
        }

    RESTInfo << "Generating sensitivity curve" << RESTendl;
    std::vector<Double_t> curve;
    for (const auto& node : fParameterizationNodes) {
        RESTInfo << "Generating node : " << node << RESTendl;
        curve.push_back(GetCoupling(node));
    }
    fCurves.push_back(curve);

    RESTInfo << "Curve has been generated. You may use now TRestSensitivity::ExportCurve( fname.txt )."
             << RESTendl;
}

void TRestSensitivity::GenerateCurves(Int_t N) {
	std::cout << "TRestSensitivity::GenerateCurves." << std::endl;
	std::cout << "There is a potential memory leak when generating several curves." << std::endl;
	std::cout << "This code needs to be reviewed" << std::endl;
	return;

    for (int n = 0; n < N; n++) GenerateCurve();
}

std::vector<Double_t> TRestSensitivity::GetCurve(size_t n) {
    if (n >= GetNumberOfCurves()) {
        RESTWarning << "Requested curve number : " << n << " but only " << GetNumberOfCurves() << " generated"
                    << RESTendl;
        return std::vector<Double_t>();
    }
    return fCurves[n];
}

std::vector<Double_t> TRestSensitivity::GetAveragedCurve() {
    if (GetNumberOfCurves() <= 0) return std::vector<Double_t>();

    std::vector<double> averagedCurve(fCurves[0].size(), 0.0);  // Initialize with zeros

    for (const auto& row : fCurves) {
        for (size_t i = 0; i < row.size(); ++i) {
            averagedCurve[i] += row[i];
        }
    }

    for (double& avg : averagedCurve) {
        avg /= static_cast<double>(fCurves.size());
    }

    return averagedCurve;
}

void TRestSensitivity::ExportAveragedCurve(std::string fname) {
    std::vector<Double_t> curve = GetAveragedCurve();
    if (curve.empty()) std::cout << "Curve is empty" << std::endl;
    if (curve.empty()) return;

    // Open a file for writing
    std::ofstream outputFile(fname);

    // Check if the file is opened successfully
    if (!outputFile) {
        RESTError << "TRestSensitivity::ExportCurve. Error opening file for writing!" << RESTendl;
        return;
    }

    if (fParameterizationNodes.size() != curve.size()) {
        RESTError << "TRestSensitivity::ExportCurve. Curve has not been properly generated" << RESTendl;
        RESTError << "Parameterization nodes: " << fParameterizationNodes.size() << RESTendl;
        RESTError << "Try invoking TRestSensitivity::GenerateCurve" << RESTendl;
        return;
    }

    int m = 0;
    for (const auto& node : fParameterizationNodes) {
        outputFile << node << " " << curve[m] << std::endl;
        m++;
    }

    outputFile.close();

    RESTInfo << "TRestSensitivity::ExportCurve. File has been written successfully!" << RESTendl;
}

void TRestSensitivity::ExportCurve(std::string fname, int n = 0) {
    std::vector<Double_t> curve = GetCurve(n);
    if (curve.empty()) return;

    // Open a file for writing
    std::ofstream outputFile(fname);

    // Check if the file is opened successfully
    if (!outputFile) {
        RESTError << "TRestSensitivity::ExportCurve. Error opening file for writing!" << RESTendl;
        return;
    }

    if (fParameterizationNodes.size() != curve.size()) {
        RESTError << "TRestSensitivity::ExportCurve. Curve has not been properly generated" << RESTendl;
        RESTError << "Try invoking TRestSensitivity::GenerateCurve" << RESTendl;
        return;
    }

    int m = 0;
    for (const auto& node : fParameterizationNodes) {
        outputFile << node << " " << curve[m] << std::endl;
        m++;
    }

    outputFile.close();

    RESTInfo << "TRestSensitivity::ExportCurve. File has been written successfully!" << RESTendl;
}

///////////////////////////////////////////////
/// \brief It will return the coupling value for which Chi=sigma
///
Double_t TRestSensitivity::GetCoupling(Double_t node, Double_t sigma, Double_t precision) {
    Double_t Chi2_0 = 0;
    for (const auto& exp : fExperiments) {
        Chi2_0 += -2 * UnbinnedLogLikelihood(exp, node, 0);
    }

    Double_t target = sigma * sigma;

    Double_t g4 = 0.5;

    g4 = ApproachByFactor(node, g4, Chi2_0, target, 2);
    g4 = ApproachByFactor(node, g4, Chi2_0, target, 1.2);
    g4 = ApproachByFactor(node, g4, Chi2_0, target, 1.02);
    g4 = ApproachByFactor(node, g4, Chi2_0, target, 1.0002);

    return g4;
}

///////////////////////////////////////////////
/// \brief It returns the Log(L) for the experiment and coupling given by argument.
///
Double_t TRestSensitivity::UnbinnedLogLikelihood(const TRestExperiment* experiment, Double_t node,
                                                 Double_t g4) {
    Double_t lhood = 0;
    if (!experiment->IsDataReady()) {
        RESTError << "TRestSensitivity::UnbinnedLogLikelihood. Experiment " << experiment->GetName()
                  << " is not ready!" << RESTendl;
        return lhood;
    }

    /// We check if the signal component is sensitive to that particular node
    /// If not, this experiment will not contribute to that node
    Int_t nd = experiment->GetSignal()->FindActiveNode(node);
    if (nd >= 0)
        experiment->GetSignal()->SetActiveNode(nd);
    else
        return 0.0;

    /// We could check if background has also components, but for the moment we do not have a background
    /// for each node, although it could be the case, if for example the background depends on the detector
    /// conditions. For example, higher pressure inside the detector gains in signal sensitivity but
    /// it will produce also higher background.

    if (experiment->GetBackground()->HasNodes()) {
        RESTWarning
            << "TRestSensitivity::UnbinnedLogLikelihood is not ready to have background parameter nodes!"
            << RESTendl;
        return 0.0;
    }

    Double_t signal = g4 * experiment->GetSignal()->GetTotalRate() * experiment->GetExposureInSeconds();

    lhood = -signal;

    if (experiment->GetExperimentalCounts() == 0) return lhood;

    if (ROOT::IsImplicitMTEnabled()) ROOT::DisableImplicitMT();

    std::vector<std::vector<Double_t>> trackingData;
    for (const auto& var : experiment->GetSignal()->GetVariables()) {
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
TH1D* TRestSensitivity::SignalStatisticalTest(Double_t node, Int_t N) {
    std::vector<Double_t> couplings;
    for (int n = 0; n < N; n++) {
        for (const auto& exp : fExperiments) exp->GetSignal()->RegenerateActiveNodeDensity();

        Double_t coupling = TMath::Sqrt(TMath::Sqrt(GetCoupling(node)));
        couplings.push_back(coupling);
    }

    // Directly assign the minimum and maximum values
    double min_value = *std::min_element(couplings.begin(), couplings.end());
    double max_value = *std::max_element(couplings.begin(), couplings.end());

    if (fSignalTest) delete fSignalTest;
    fSignalTest = new TH1D("SignalTest", "A signal test", 100, 0.9 * min_value, 1.1 * max_value);
    for (const auto& coup : couplings) fSignalTest->Fill(coup);

    return fSignalTest;
}

/////////////////////////////////////////////
/// \brief It customizes the retrieval of XML data values of this class
///
void TRestSensitivity::InitFromConfigFile() {
    TRestMetadata::InitFromConfigFile();

    int cont = 0;
    TRestMetadata* metadata = (TRestMetadata*)this->InstantiateChildMetadata(cont, "Experiment");
    while (metadata != nullptr) {
        cont++;
        if (metadata->InheritsFrom("TRestExperimentList")) {
            TRestExperimentList* experimentsList = (TRestExperimentList*)metadata;
            std::vector<TRestExperiment*> exList = experimentsList->GetExperiments();
            fExperiments.insert(fExperiments.end(), exList.begin(), exList.end());
        } else if (metadata->InheritsFrom("TRestExperiment")) {
            fExperiments.push_back((TRestExperiment*)metadata);
        }

        metadata = (TRestMetadata*)this->InstantiateChildMetadata(cont, "Experiment");
    }

    Initialize();
}

/////////////////////////////////////////////
/// \brief This method is used to obtain the list of curves that satisfy that each value inside
/// the curve is placed at a specified level. E.g. if we provide a level 0.5, then the corresponding
/// curve will be constructed with the central value extracted at each parameter point.
//
/// We may then construct the profile of the sensitivity curves at 98%, 95% and 68% C.L. as follows:
///
/// \code
/// TRestSensitivity::GetLevelCurves( {0.01, 0.025, 0.16, 0.84, 0.975, 0.99} );
/// \endcode
///
std::vector<std::vector<Double_t>> TRestSensitivity::GetLevelCurves(const std::vector<Double_t>& levels) {
    std::vector<std::vector<Double_t>> curves(levels.size());

    for (const auto& l : levels) {
        if (l >= 1 || l <= 0) {
            RESTError << "The level value should be between 0 and 1" << RESTendl;
            return curves;
        }
    }

    std::vector<int> intLevels;
    for (const auto& l : levels) {
        int val = (int)round(l * fCurves.size());
        if (val >= (int)fCurves.size()) val = fCurves.size() - 1;
        if (val < 0) val = 0;

        intLevels.push_back(val);
    }

    for (size_t m = 0; m < fCurves[0].size(); m++) {
        std::vector<Double_t> v;
        for (size_t n = 0; n < fCurves.size(); n++) v.push_back(fCurves[n][m]);

        std::sort(v.begin(), v.begin() + v.size());

        for (size_t n = 0; n < intLevels.size(); n++) curves[n].push_back(v[intLevels[n]]);
    }

    return curves;
}

TCanvas* TRestSensitivity::DrawCurves() {
    if (fCanvas != NULL) {
        delete fCanvas;
        fCanvas = NULL;
    }
    fCanvas = new TCanvas("canv", "This is the canvas title", 600, 450);
    fCanvas->Draw();

    TPad* pad1 = new TPad("pad1", "This is pad1", 0.01, 0.02, 0.99, 0.97);
    // pad1->Divide(2, 2);
    pad1->Draw();

    ////// Drawing reflectivity versus angle
    //   pad1->cd()->SetLogx();
    pad1->cd()->SetRightMargin(0.09);
    pad1->cd()->SetLeftMargin(0.15);
    pad1->cd()->SetBottomMargin(0.15);

    std::vector<TGraph*> graphs;

    for (size_t n = 0; n < 20; n++) {
        std::string grname = "gr" + IntegerToString(n);
        TGraph* gr = new TGraph();
        gr->SetName(grname.c_str());
        for (size_t m = 0; m < this->GetCurve(n).size(); m++)
            gr->SetPoint(gr->GetN(), fParameterizationNodes[m],
                         TMath::Sqrt(TMath::Sqrt(this->GetCurve(n)[m])));

        gr->SetLineColorAlpha(kBlue + n, 0.3);
        gr->SetLineWidth(1);
        graphs.push_back(gr);
    }

    TGraph* avGr = new TGraph();
    std::vector<Double_t> avCurve = GetAveragedCurve();
    for (size_t m = 0; m < avCurve.size(); m++)
        avGr->SetPoint(avGr->GetN(), fParameterizationNodes[m], TMath::Sqrt(TMath::Sqrt(avCurve[m])));
    avGr->SetLineColor(kBlack);
    avGr->SetLineWidth(2);

    graphs[0]->GetXaxis()->SetLimits(0, 0.25);
    //   graphs[0]->GetHistogram()->SetMaximum(1);
    //   graphs[0]->GetHistogram()->SetMinimum(lowRange);

    graphs[0]->GetXaxis()->SetTitle("mass [eV]");
    graphs[0]->GetXaxis()->SetTitleSize(0.05);
    graphs[0]->GetXaxis()->SetLabelSize(0.05);
    graphs[0]->GetYaxis()->SetTitle("g_{a#gamma} [10^{-10} GeV^{-1}]");
    graphs[0]->GetYaxis()->SetTitleOffset(1.5);
    graphs[0]->GetYaxis()->SetTitleSize(0.05);
    // graphs[0]->GetYaxis()->SetLabelSize(0.05);
    // graphs[0]->GetYaxis()->SetLabelOffset(0.0);
    // pad1->cd()->SetLogy();
    graphs[0]->Draw("AL");
    for (unsigned int n = 1; n < graphs.size(); n++) graphs[n]->Draw("L");
    avGr->Draw("L");

    /*
Double_t lx1 = 0.6, ly1 = 0.75, lx2 = 0.9, ly2 = 0.95;
if (eLegendCoords.size() > 0) {
    lx1 = eLegendCoords[0];
    ly1 = eLegendCoords[1];
    lx2 = eLegendCoords[2];
    ly2 = eLegendCoords[3];
}
TLegend* legend = new TLegend(lx1, ly1, lx2, ly2);

legend->SetTextSize(0.03);
legend->SetHeader("Energies", "C");  // option "C" allows to center the header
for (unsigned int n = 0; n < energies.size(); n++) {
    std::string lname = "gr" + IntegerToString(n);
    std::string ltitle = DoubleToString(energies[n]) + " keV";

    legend->AddEntry(lname.c_str(), ltitle.c_str(), "l");
}
legend->Draw();
    */

    return fCanvas;
}

TCanvas* TRestSensitivity::DrawLevelCurves() {
    if (fCanvas != NULL) {
        delete fCanvas;
        fCanvas = NULL;
    }
    fCanvas = new TCanvas("canv", "This is the canvas title", 500, 400);
    fCanvas->Draw();
    fCanvas->SetLeftMargin(0.15);
    fCanvas->SetRightMargin(0.04);
    fCanvas->SetLogx();

    std::vector<std::vector<Double_t>> levelCurves = GetLevelCurves({0.025, 0.16, 0.375, 0.625, 0.84, 0.975});

    std::vector<TGraph*> graphs;
    for (size_t n = 0; n < levelCurves.size(); n++) {
        std::string grname = "gr" + IntegerToString(n);
        TGraph* gr = new TGraph();
        gr->SetName(grname.c_str());
        for (size_t m = 0; m < levelCurves[n].size(); m++)
            gr->SetPoint(gr->GetN(), fParameterizationNodes[m], TMath::Sqrt(TMath::Sqrt(levelCurves[n][m])));

        gr->SetLineColor(kBlack);
        gr->SetLineWidth(1);
        graphs.push_back(gr);
    }

    TGraph* centralGr = new TGraph();
    std::vector<Double_t> centralCurve = GetLevelCurves({0.5})[0];
    for (size_t m = 0; m < centralCurve.size(); m++)
        centralGr->SetPoint(centralGr->GetN(), fParameterizationNodes[m],
                            TMath::Sqrt(TMath::Sqrt(centralCurve[m])));
    centralGr->SetLineColor(kBlack);
    centralGr->SetLineWidth(2);
    centralGr->SetMarkerSize(0.1);

    graphs[0]->GetYaxis()->SetRangeUser(0, 0.5);
    graphs[0]->GetXaxis()->SetRangeUser(0.001, 0.25);
    graphs[0]->GetXaxis()->SetLimits(0.0001, 0.25);
    graphs[0]->GetXaxis()->SetTitle("mass [eV]");
    graphs[0]->GetXaxis()->SetTitleSize(0.04);
    graphs[0]->GetXaxis()->SetTitleOffset(1.15);
    graphs[0]->GetXaxis()->SetLabelSize(0.04);

    //   graphs[0]->GetYaxis()->SetLabelFont(43);
    graphs[0]->GetYaxis()->SetTitle("g_{a#gamma} [10^{-10} GeV^{-1}]");
    graphs[0]->GetYaxis()->SetTitleOffset(1.5);
    graphs[0]->GetYaxis()->SetTitleSize(0.04);
    graphs[0]->GetYaxis()->SetLabelSize(0.04);
    // graphs[0]->GetYaxis()->SetLabelOffset(0);
    // graphs[0]->GetYaxis()->SetLabelFont(43);
    graphs[0]->Draw("AL");

    TGraph* randomGr = new TGraph();
    std::vector<Double_t> randomCurve = fCurves[13];
    for (size_t m = 0; m < randomCurve.size(); m++)
        randomGr->SetPoint(randomGr->GetN(), fParameterizationNodes[m],
                           TMath::Sqrt(TMath::Sqrt(randomCurve[m])));
    randomGr->SetLineColor(kBlack);
    randomGr->SetLineWidth(1);
    randomGr->SetMarkerSize(0.3);
    randomGr->SetMarkerStyle(4);

    std::vector<TGraph*> shadeGraphs;

    int M = (int)levelCurves.size();
    for (int x = 0; x < M / 2; x++) {
        TGraph* shade = new TGraph();
        int N = levelCurves[0].size();
        for (size_t m = 0; m < levelCurves[0].size(); m++)
            shade->SetPoint(shade->GetN(), fParameterizationNodes[m],
                            TMath::Sqrt(TMath::Sqrt(levelCurves[x][m])));
        for (int m = N - 1; m >= 0; --m)
            shade->SetPoint(shade->GetN(), fParameterizationNodes[m],
                            TMath::Sqrt(TMath::Sqrt(levelCurves[M - 1 - x][m])));
        shade->SetFillColorAlpha(kRed, 0.25);
        shade->Draw("f");
        shadeGraphs.push_back(shade);
    }

    for (unsigned int n = 1; n < graphs.size(); n++) graphs[n]->Draw("Lsame");
    randomGr->Draw("LPsame");
    // centralGr->Draw("Lsame");

    return fCanvas;
}

/////////////////////////////////////////////
/// \brief It scans all the experiment signals parametric nodes to build a complete list
/// of nodes used to build a complete sensitivity curve. Some experiments may be
/// sensitivy to a particular node, while others may be sensitivy to another. If more
/// than one experiment is sensitivy to a given node, the sensitivity will be combined
/// later on.
///
void TRestSensitivity::ExtractExperimentParameterizationNodes(Bool_t rescan) {
    if (fParameterizationNodes.empty() || rescan) {
        fParameterizationNodes.clear();

        for (const auto& experiment : fExperiments) {
            std::vector<Double_t> nodes = experiment->GetSignal()->GetParameterizationNodes();
            fParameterizationNodes.insert(fParameterizationNodes.end(), nodes.begin(), nodes.end());

            std::sort(fParameterizationNodes.begin(), fParameterizationNodes.end());
            auto last = std::unique(fParameterizationNodes.begin(), fParameterizationNodes.end());
            fParameterizationNodes.erase(last, fParameterizationNodes.end());
        }
    }
}

void TRestSensitivity::PrintParameterizationNodes() {
    std::cout << "Curve sensitivity nodes: ";
    for (const auto& node : fParameterizationNodes) std::cout << node << "\t";
    std::cout << std::endl;
}

/////////////////////////////////////////////
/// \brief Prints on screen the information about the metadata members of TRestAxionSolarFlux
///
void TRestSensitivity::PrintMetadata() {
    TRestMetadata::PrintMetadata();

    RESTMetadata << " - Number of parameterization nodes : " << GetNumberOfNodes() << RESTendl;
    RESTMetadata << " - Number of experiments loaded : " << GetNumberOfExperiments() << RESTendl;
    RESTMetadata << " - Number of sensitivity curves generated : " << GetNumberOfCurves() << RESTendl;
    RESTMetadata << " " << RESTendl;
    RESTMetadata << " You may access experiment info using TRestSensitivity::GetExperiment(n)" << RESTendl;

    RESTMetadata << "----" << RESTendl;
}
