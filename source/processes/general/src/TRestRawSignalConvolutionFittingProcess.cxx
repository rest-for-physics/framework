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
///
/// Fit every TRestRawSignal in a TRestRawSignalEvent with AGET theoretical curve
/// times a logistic function convoluted with a gaussian pulse. The logistic function
/// acts like a step function to select only the positive range of the AGET function.
///
/// From TRestRawSignal -> TH1 -> Measure goodness of fit
///
/// Analytic expression to fit:
///
/// AGET function:
/// [0]*TMath::Exp(-3. * (x-[2])/[1]) * (x-[2])/[1] * (x-[2])/[1] * (x-[2])/[1] *
/// sin((x-[2])/[1])/(1+TMath::Exp(-10000*(x-[2])))
///
/// [0] = "Amplitude",
/// [1] = "ShapingTime",
/// [2] = "PeakPosition"
///
/// Gauss function:
/// exp(-0.5*((x)/[0])*((x)/[0]))
///
/// [0] = "VarianceGauss"
///
///
/// ### Observables
///
/// * **FitBaseline_map**: For each pulse, save first fit's parameter.
///
/// * **FitAmplitude_map**: For each pulse, save second fit's parameter.
///
/// * **FitShapingTime_map**: For each pulse, save third fit's parameter.
///
/// * **FitPeakPosition_map**: For each pulse, save fourth fit's parameter.
///
/// * **FitSigmaMean**: Mean over all pulses in the event of square root of the squared
/// difference betweeen raw signal and fit divided by number of bins.
///
/// * **FitSigmaStdDev**: Standard deviation over all pulses in the event of square root of the squared
/// difference betweeen raw signal and fit divided by number of bins.
///
/// * **FitChiSquareMean**: Mean over all pulses in the event of chi square of the fit.
///
/// * **FitRatioSigmaMaxPeakMean**: Mean over all pulses in the event of square root of the squared
/// difference betweeen raw signal and fit divided by number of bins and divided by amplitude of the pulse.
///
///_______________________________________________________________________________
///
/// RESTsoft - Software for Rare Event Searches with TPCs
///
/// History of developments:
///
/// 2020-October First implementation of raw signal convolution fitting process.
///              Created from TRestRawSignalAnalysisProcess.
///
/// \class      TRestRawSignalConvolutionFittingProcess
/// \author     David Diez
///
///______________________________________________________________________________
///
//////////////////////////////////////////////////////////////////////////

#include "TRestRawSignalConvolutionFittingProcess.h"
using namespace std;

ClassImp(TRestRawSignalConvolutionFittingProcess);

///////////////////////////////////////////////
/// \brief Default constructor
///
TRestRawSignalConvolutionFittingProcess::TRestRawSignalConvolutionFittingProcess() { Initialize(); }

///////////////////////////////////////////////
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
///
TRestRawSignalConvolutionFittingProcess::TRestRawSignalConvolutionFittingProcess(char* cfgFileName) {
    Initialize();

    if (LoadConfigFromFile(cfgFileName)) LoadDefaultConfig();
}

///////////////////////////////////////////////
/// \brief Default destructor
///
TRestRawSignalConvolutionFittingProcess::~TRestRawSignalConvolutionFittingProcess() {}

///////////////////////////////////////////////
/// \brief Function to load the default config in absence of RML input
///
void TRestRawSignalConvolutionFittingProcess::LoadDefaultConfig() { SetTitle("Default config"); }

///////////////////////////////////////////////
/// \brief Function to initialize input/output event members and define the
/// section name
///
void TRestRawSignalConvolutionFittingProcess::Initialize() {
    SetSectionName(this->ClassName());

    fRawSignalEvent = NULL;
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
/// correspondig TRestGeant4AnalysisProcess section inside the RML.
///
void TRestRawSignalConvolutionFittingProcess::LoadConfig(std::string cfgFilename, std::string name) {
    if (LoadConfigFromFile(cfgFilename, name)) LoadDefaultConfig();
}

///////////////////////////////////////////////
/// \brief Process initialization.
///
void TRestRawSignalConvolutionFittingProcess::InitProcess() {
    // fSignalFittingObservables = TRestEventProcess::ReadObservables();
}

///////////////////////////////////////////////
/// \brief The main processing event function
///
TRestEvent* TRestRawSignalConvolutionFittingProcess::ProcessEvent(TRestEvent* evInput) {
    // no need for verbose copy now
    fRawSignalEvent = (TRestRawSignalEvent*)evInput;

    debug << "TRestRawSignalConvolutionFittingProcess::ProcessEvent. Event ID : " << fRawSignalEvent->GetID()
          << endl;

    Double_t SigmaMean = 0;
    Double_t Sigma[fRawSignalEvent->GetNumberOfSignals()];
    Double_t RatioSigmaMaxPeakMean = 0;
    Double_t RatioSigmaMaxPeak[fRawSignalEvent->GetNumberOfSignals()];
    Double_t ChiSquareMean = 0;
    Double_t ChiSquare[fRawSignalEvent->GetNumberOfSignals()];

    map<int, Double_t> amplitudeFit;
    map<int, Double_t> shapingtimeFit;
    map<int, Double_t> peakpositionFit;
    map<int, Double_t> variancegaussFit;

    amplitudeFit.clear();
    shapingtimeFit.clear();
    peakpositionFit.clear();
    variancegaussFit.clear();

    int MinBinRange = 25;
    int MaxBinRange = 45;

    for (int s = 0; s < fRawSignalEvent->GetNumberOfSignals(); s++) {
        TRestRawSignal* singleSignal = fRawSignalEvent->GetSignal(s);
        singleSignal->CalculateBaseLine(20, 150);
        int MaxPeakBin = singleSignal->GetMaxPeakBin();

        // ShaperSin function (AGET theoretical curve) times logistic function
        TF1* f = new TF1("Aget",
                         "[0]*TMath::Exp(-3. * (x-[2])/[1]) * (x-[2])/[1] * (x-[2])/[1] * (x-[2])/[1] * "
                         " sin((x-[2])/[1])/(1+TMath::Exp(-10000*(x-[2])))",
                         0, 511);
        f->SetParameters(0., 300., 42., 20.);  // Initial values adjusted from Desmos
        f->SetParNames("Amplitude", "ShapingTime", "PeakPosition");

        // Gaussian pulse
        TF1* g = new TF1("pulse", "exp(-0.5*((x)/[0])*((x)/[0]))", 0, 511);
        g->SetParameters(0, 7.);

        // Convolution of AGET and gaussian functions
        TF1Convolution* conv = new TF1Convolution("Aget", "pulse", 0, 511, true);
        conv->SetRange(0, 511);
        conv->SetNofPointsFFT(10000);

        TF1* fit_conv = new TF1("fit", *conv, 0, 511, conv->GetNpar());
        fit_conv->SetParNames("Amplitude", "ShapingTime", "PeakPosition", "VarianceGauss");

        // Create histogram from signal
        Int_t nBins = singleSignal->GetNumberOfPoints();
        TH1D* h = new TH1D("histo", "Signal to histo", nBins, 0, nBins);

        for (int i = 0; i < nBins; i++) {
            h->Fill(i, singleSignal->GetRawData(i) - singleSignal->GetBaseLine());
            h->SetBinError(i, singleSignal->GetBaseLineSigma());
        }

        // Fit histogram with convolution
        fit_conv->SetParameters(singleSignal->GetData(MaxPeakBin), 25., MaxPeakBin - 25., 8.);
        fit_conv->FixParameter(0, singleSignal->GetData(MaxPeakBin));

        h->Fit(fit_conv, "LRNQ", "", MaxPeakBin - MinBinRange, MaxPeakBin + MaxBinRange);
        // Options: L->Likelihood minimization, R->fit in range, N->No draw, Q->Quiet

        Double_t sigma = 0;
        for (int j = MaxPeakBin - MinBinRange; j < MaxPeakBin + MaxBinRange; j++) {
            sigma += (singleSignal->GetData(j) - fit_conv->Eval(j)) *
                     (singleSignal->GetData(j) - fit_conv->Eval(j));
        }
        Sigma[s] = TMath::Sqrt(sigma / (MinBinRange + MaxBinRange));
        RatioSigmaMaxPeak[s] = Sigma[s] / singleSignal->GetData(MaxPeakBin);
        RatioSigmaMaxPeakMean += RatioSigmaMaxPeak[s];
        SigmaMean += Sigma[s];
        ChiSquare[s] = fit_conv->GetChisquare();
        ChiSquareMean += ChiSquare[s];

        amplitudeFit[singleSignal->GetID()] = fit_conv->GetParameter(0);
        shapingtimeFit[singleSignal->GetID()] = fit_conv->GetParameter(1);
        peakpositionFit[singleSignal->GetID()] = fit_conv->GetParameter(2);
        variancegaussFit[singleSignal->GetID()] = fit_conv->GetParameter(3);

        h->Delete();
    }

    //////////// Fitted parameters Map Observables /////////////
    SetObservableValue("FitAmplitude_map", amplitudeFit);
    SetObservableValue("FitShapingTime_map", shapingtimeFit);
    SetObservableValue("FitPeakPosition_map", peakpositionFit);
    SetObservableValue("FitVarianceGauss_map", variancegaussFit);

    //////////// Sigma Mean Observable /////////////
    SigmaMean = SigmaMean / (fRawSignalEvent->GetNumberOfSignals());
    SetObservableValue("FitSigmaMean", SigmaMean);

    //////////// Sigma Mean Standard Deviation  Observable /////////////
    Double_t sigmaMeanStdDev = 0;
    for (int k = 0; k < fRawSignalEvent->GetNumberOfSignals(); k++) {
        sigmaMeanStdDev += (Sigma[k] - SigmaMean) * (Sigma[k] - SigmaMean);
    }
    Double_t SigmaMeanStdDev = TMath::Sqrt(sigmaMeanStdDev / fRawSignalEvent->GetNumberOfSignals());
    SetObservableValue("FitSigmaStdDev", SigmaMeanStdDev);

    //////////// Chi Square Mean Observable /////////////
    ChiSquareMean = ChiSquareMean / fRawSignalEvent->GetNumberOfSignals();
    SetObservableValue("FitChiSquareMean", ChiSquareMean);

    //////////// Ratio Sigma MaxPeak Mean Observable /////////////
    RatioSigmaMaxPeakMean = RatioSigmaMaxPeakMean / fRawSignalEvent->GetNumberOfSignals();
    SetObservableValue("FitRatioSigmaMaxPeakMean", RatioSigmaMaxPeakMean);

    debug << "SigmaMean: " << SigmaMean << endl;
    debug << "SigmaMeanStdDev: " << SigmaMeanStdDev << endl;
    debug << "ChiSquareMean: " << ChiSquareMean << endl;
    debug << "RatioSigmaMaxPeakMean: " << RatioSigmaMaxPeakMean << endl;
    for (int k = 0; k < fRawSignalEvent->GetNumberOfSignals(); k++) {
        debug << "Standard deviation of signal number " << k << ": " << Sigma[k] << endl;
        debug << "Chi square of fit signal number " << k << ": " << ChiSquare[k] << endl;
        debug << "Sandard deviation divided by amplitude of signal number " << k << ": "
              << RatioSigmaMaxPeak[k] << endl;
    }

    /// We define (or re-define) the baseline range and calculation range of our raw-signals.
    // This will affect the calculation of observables, but not the stored TRestRawSignal data.
    // fRawSignalEvent->SetBaseLineRange(fBaseLineRange);
    // fRawSignalEvent->SetRange(fIntegralRange);

    /* Perhaps we want to identify the points over threshold where to apply the fitting?
     * Then, we might need to initialize points over threshold
     *
for (int s = 0; s < fRawSignalEvent->GetNumberOfSignals(); s++) {
    TRestRawSignal* sgnl = fRawSignalEvent->GetSignal(s);

    /// Important call we need to initialize the points over threshold in a TRestRawSignal
    sgnl->InitializePointsOverThreshold(TVector2(fPointThreshold, fSignalThreshold),
                                        fNPointsOverThreshold);

}
    */

    // If cut condition matches the event will be not registered.
    if (ApplyCut()) return NULL;

    return fRawSignalEvent;
}

///////////////////////////////////////////////
/// \brief Function to include required actions after all events have been
/// processed. This method will write the channels histogram.
///
void TRestRawSignalConvolutionFittingProcess::EndProcess() {
    // Function to be executed once at the end of the process
    // (after all events have been processed)

    // Start by calling the EndProcess function of the abstract class.
    // Comment this if you don't want it.
    // TRestEventProcess::EndProcess();
}

///////////////////////////////////////////////
/// \brief Function to read input parameters.
///
void TRestRawSignalConvolutionFittingProcess::InitFromConfigFile() {
    /* Parameters to initialize from RML
fBaseLineRange = StringTo2DVector(GetParameter("baseLineRange", "(5,55)"));
fIntegralRange = StringTo2DVector(GetParameter("integralRange", "(10,500)"));
fPointThreshold = StringToDouble(GetParameter("pointThreshold", "2"));
fNPointsOverThreshold = StringToInteger(GetParameter("pointsOverThreshold", "5"));
fSignalThreshold = StringToDouble(GetParameter("signalThreshold", "5"));
    */
}
