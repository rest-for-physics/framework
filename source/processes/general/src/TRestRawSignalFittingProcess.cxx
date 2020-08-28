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
///_______________________________________________________________________________
///
/// RESTsoft - Software for Rare Event Searches with TPCs
///
/// History of developments:
///
/// 2020-August First implementation of raw signal fitting process.
///                Created from TRestRawSignalAnalysisProcess.
///
/// \class      TRestRawSignalFittingProcess
/// \author     David Diez
///
///______________________________________________________________________________
///
//////////////////////////////////////////////////////////////////////////

#include "TRestRawSignalFittingProcess.h"
using namespace std;

ClassImp(TRestRawSignalFittingProcess);

///////////////////////////////////////////////
/// \brief Default constructor
///
TRestRawSignalFittingProcess::TRestRawSignalFittingProcess() { Initialize(); }

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
TRestRawSignalFittingProcess::TRestRawSignalFittingProcess(char* cfgFileName) {
    Initialize();

    if (LoadConfigFromFile(cfgFileName)) LoadDefaultConfig();
}

///////////////////////////////////////////////
/// \brief Default destructor
///
TRestRawSignalFittingProcess::~TRestRawSignalFittingProcess() {}

///////////////////////////////////////////////
/// \brief Function to load the default config in absence of RML input
///
void TRestRawSignalFittingProcess::LoadDefaultConfig() { SetTitle("Default config"); }

///////////////////////////////////////////////
/// \brief Function to initialize input/output event members and define the
/// section name
///
void TRestRawSignalFittingProcess::Initialize() {
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
void TRestRawSignalFittingProcess::LoadConfig(std::string cfgFilename, std::string name) {
    if (LoadConfigFromFile(cfgFilename, name)) LoadDefaultConfig();
}

///////////////////////////////////////////////
/// \brief Process initialization.
///
void TRestRawSignalFittingProcess::InitProcess() {
    // fSignalFittingObservables = TRestEventProcess::ReadObservables();
}

///////////////////////////////////////////////
/// \brief The main processing event function
///
TRestEvent* TRestRawSignalFittingProcess::ProcessEvent(TRestEvent* evInput) {
    // no need for verbose copy now
    fRawSignalEvent = (TRestRawSignalEvent*)evInput;

    cout << "=============================================" << endl;
    cout << "I am executing TRestRawSignalFittingProcess!" << endl;
    cout << "Event ID : " << fRawSignalEvent->GetID() << endl;

    Double_t SigmaMean = 0;
    Double_t Sigma[fRawSignalEvent->GetNumberOfSignals()];
    for (int s = 0; s < fRawSignalEvent->GetNumberOfSignals(); s++) {
        TRestRawSignal* singleSignal = fRawSignalEvent->GetSignal(s);

        int MaxPeakBin = singleSignal->GetMaxPeakBin();
        // cout << "MaxPeakBin: " << MaxPeakBin << endl;

        // ShaperSin function (AGET theoretic curve)
        TF1* f = new TF1("fit",
                         "[0]+[1]*TMath::Exp(-3. * (x-[3])/[2]) * (x-[3])/[2] * (x-[3])/[2] * (x-[3])/[2] * "
                         "sin((x-[3])/[2])",
                         MaxPeakBin - 25, MaxPeakBin + 45);
        f->SetParameters(0, 250);  // Initial values adjusted from Desmos
        f->SetParLimits(0, 0, 350);
        f->SetParameters(1, 50);
        f->SetParLimits(1, 30, 90000);
        f->SetParameters(2, 20);
        f->SetParLimits(2, 10, 80);
        f->SetParameters(3, 170);
        f->SetParLimits(3, 150, 250);
        f->SetParNames("Baseline", "ProdConstant", "ShapingTime", "XConstant");

        // Create histogram from signal
        Int_t nBins = singleSignal->GetNumberOfPoints();
        TH1D* h = new TH1D("histo", "Signal to histo", nBins, 0, nBins);

        for (int i = 0; i < nBins; i++) {
            h->Fill(i, singleSignal->GetData(i) + singleSignal->GetBaseLine());
        }

        // Fit histogram with ShaperSin
        h->Fit(f, "RNQ", "", MaxPeakBin - 25,
               MaxPeakBin + 45);  // Options: R->fit in range, N->No draw, Q->Quiet

        if (fRawSignalEvent->GetID() == 30875) {
            if (s == 4) {
                for (int j = MaxPeakBin - 25; j < MaxPeakBin + 45; j++) {
                    cout << "Pulse: " << singleSignal->GetData(j) + singleSignal->GetBaseLine()
                         << "  Fit: " << f->Eval(j) << endl;
                }
            }
        }

        // Standard deviation (sqrt of variance between fit and data)
        Double_t sigma = 0;
        for (int j = MaxPeakBin - 25; j < MaxPeakBin + 45; j++) {
            sigma += (singleSignal->GetData(j) + singleSignal->GetBaseLine() - f->Eval(j)) *
                     (singleSignal->GetData(j) + singleSignal->GetBaseLine() - f->Eval(j));
            // cout << j-MaxPeakBin << " " << singleSignal->GetData(j)-f->Eval(j) << endl;
        }
        Sigma[s] = TMath::Sqrt(sigma / (25 + 45));
        SigmaMean += Sigma[s];
        // cout << "Standard deviation of signal number " << s << ": " << Sigma[s] << endl;

        h->GetListOfFunctions()->Remove(h->GetFunction("fit"));
        h->Reset();
    }

    SigmaMean = SigmaMean / (fRawSignalEvent->GetNumberOfSignals());
    SetObservableValue("FitSigmaMean", SigmaMean);

    Double_t sigmaMeanStdDev = 0;
    for (int k = 0; k < fRawSignalEvent->GetNumberOfSignals(); k++) {
        sigmaMeanStdDev += (Sigma[k] - SigmaMean) * (Sigma[k] - SigmaMean);
    }
    Double_t SigmaMeanStdDev = TMath::Sqrt(sigmaMeanStdDev / fRawSignalEvent->GetNumberOfSignals());
    SetObservableValue("FitSigmaStdDev", SigmaMeanStdDev);

    cout << "SigmaMean: " << SigmaMean << endl;
    cout << "SigmaMeanStdDev: " << SigmaMeanStdDev << endl;
    for (int k = 0; k < fRawSignalEvent->GetNumberOfSignals(); k++) {
        cout << "Standard deviation of signal number " << k << ": " << Sigma[k] << endl;
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
void TRestRawSignalFittingProcess::EndProcess() {
    // Function to be executed once at the end of the process
    // (after all events have been processed)

    // Start by calling the EndProcess function of the abstract class.
    // Comment this if you don't want it.
    // TRestEventProcess::EndProcess();
}

///////////////////////////////////////////////
/// \brief Function to read input parameters.
///
void TRestRawSignalFittingProcess::InitFromConfigFile() {
    /* Parameters to initialize from RML
fBaseLineRange = StringTo2DVector(GetParameter("baseLineRange", "(5,55)"));
fIntegralRange = StringTo2DVector(GetParameter("integralRange", "(10,500)"));
fPointThreshold = StringToDouble(GetParameter("pointThreshold", "2"));
fNPointsOverThreshold = StringToInteger(GetParameter("pointsOverThreshold", "5"));
fSignalThreshold = StringToDouble(GetParameter("signalThreshold", "5"));
    */
}
