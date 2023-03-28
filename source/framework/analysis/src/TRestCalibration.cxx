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
/// TRestCalibration performs the calibration of a TRestDataSet, the
/// calibration is performed over different peaks provided in the config
/// file. The first peak provided in the config file should correspond 
/// to the maximum in the spectra. The expected position of the rest 
/// of the peaks are estimated with respect to the maximum.
///
/// A summary of the basic parameters is described below:
/// * **calObservable**: Name of the observable to be calibrated
/// * **nBins**: Number of bins for the calibration spectra
/// * **calibRange**: Calibration range inside calObservable that
/// will be used for the spectrum
/// * **dataSetName**: Name of the dataSet to be calibrated
/// * **calibFile**: Name of the calibration file to be used for
/// the calibration parameters, if empty it performs the calibration
/// over the dataSet provided.
///
/// The energy peaks to calibrate are given using peak metadata:
/// * **energy**: Energy of the peak(s) to be calibrated in keV,
/// note that the first peak should correspond to the maximum
///
/// The following metadata members are used as output after the fit:
/// * **calibPeaks**: Position of the mean value of the peak after
/// the fit using the same order as the energy peaks
/// * **fCalibFWHM**: FWHM in percentage for the different peaks,
/// using the same order as the energy peaks.
///
/// If an output file name is provided a new dataset will be generated
/// with the corresponding metadata and calibration constants, while
/// adding a new observable, calib_energy. In addition, the spectrum
/// and the fit results will be stored in the output file.
///
///
/// ### Examples
/// RML example to calibrate a dataset with a peak at 5.89 keV
/// \code
/// <TRestCalibration name="EnergyCalibration" verboseLevel="info">
///    <peak energy="5.899" />
///    <parameter name="calObservable" value="tckAna_MaxTrackEnergy"/>
///    <parameter name="nBins" value="1024"/>
///    <parameter name="calibRange" value="(0,20000)" />
///    <parameter name="dataSetName" value="myDataSet.root"/>
/// </TRestCalibration>
/// \endcode
///
/// Example to perform calibration over a calibration dataSet using restRoot:
/// \code
/// [0] TRestCalibration cal ("calibration.rml");
/// [1] cal.SetDataSetName("myDataSet.root");
/// [2] cal.SetOutputFileName("myCalibratedDataSet.root");
/// [3] cal.Calibrate();
/// \endcode
///
/// Example to perform calibration over a background dataSet using restRoot:
/// \code
/// [0] TRestCalibration cal ("calibration.rml");
/// [1] cal.SetDataSetName("myBackgroundDataSet.root");
/// [2] cal.SetOutputFileName("myBackgroundCalibratedDataSet.root");
/// [3] cal.SetCalibrationFile("myCalibratedDataSet.root");
/// [4] cal.Calibrate();
/// \endcode
///
///----------------------------------------------------------------------
///
/// REST-for-Physics - Software for Rare Event Searches Toolkit
///
/// History of developments:
///
/// 2023-03: First implementation of TRestCalibration
/// JuanAn Garcia
///
/// \class TRestCalibration
/// \author: JuanAn Garcia   e-mail: juanangp@unizar.es
///
/// <hr>
///

#include "TRestCalibration.h"

#include "TRestDataSet.h"

ClassImp(TRestCalibration);

///////////////////////////////////////////////
/// \brief Default constructor
///
TRestCalibration::TRestCalibration() { Initialize(); }

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
/// corresponding TRestCalibration section inside the RML.
///
TRestCalibration::TRestCalibration(const char* configFilename, std::string name)
    : TRestMetadata(configFilename) {
    LoadConfigFromFile(fConfigFileName, name);
    Initialize();

    if (GetVerboseLevel() >= TRestStringOutput::REST_Verbose_Level::REST_Info) PrintMetadata();
}

///////////////////////////////////////////////
/// \brief Default destructor
///
TRestCalibration::~TRestCalibration() {}

///////////////////////////////////////////////
/// \brief Function to initialize input/output event members and define
/// the section name
///
void TRestCalibration::Initialize() { SetSectionName(this->ClassName()); }

///////////////////////////////////////////////
/// \brief Function to initialize some variables from
/// configfile, in case that the variable is not found
/// in the rml it checks the input file pattern.
///
void TRestCalibration::InitFromConfigFile() {
    Initialize();
    TRestMetadata::InitFromConfigFile();

    TiXmlElement* peakDefinition = GetElement("peak");
    while (peakDefinition != nullptr) {
        std::string energy = GetFieldValue("energy", peakDefinition);
        if (energy.empty() || energy == "Not defined") {
            RESTError << "< peak variable key does not contain energy!" << RESTendl;
            exit(1);
        } else {
            fEnergyPeaks.push_back(StringToDouble(energy));
        }

        peakDefinition = GetNextElement(peakDefinition);
    }

    if (fEnergyPeaks.empty()) {
        RESTError << "Energy peaks not provided, exiting..." << RESTendl;
        exit(1);
    }

    if (fOutputFileName == "") fOutputFileName = GetParameter("outputFileName", "");
}

/////////////////////////////////////////////
/// \brief Performs the calibration of a given dataSet. If
/// no calibration file is provided, it performs the gaussian
/// fit to the different parameters provided in the config file.
/// Otherwise, it takes the calibration constants from the
/// provided calibration file.
///
void TRestCalibration::Calibrate() {
    PrintMetadata();

    TRestDataSet dataSet;
    dataSet.Import(fDataSetName);

    dataSet.PrintMetadata();

    std::unique_ptr<TH1F> spectrum;
    std::unique_ptr<TGraph> gr;
    std::unique_ptr<TF1> linearFit;

    if (fCalibFile.empty()) {
        auto histo = dataSet.GetDataFrame().Histo1D(
            {"spectrum", "spectrum", fNBins, fCalibRange.X(), fCalibRange.X()}, fCalObservable);
        spectrum = std::unique_ptr<TH1F>(static_cast<TH1F*>(histo->DrawClone()));

        // Get position of the maximum
        const double max = spectrum->GetBinCenter(spectrum->GetMaximumBin());
        // Get ratio between maximum and energy peak
        const double ratio = max / fEnergyPeaks.front();

        RESTDebug << "Max peak position " << max << RESTendl;

        std::vector<TF1*> gauss;
        gr = std::unique_ptr<TGraph>(new TGraph());
        gr->SetName("grFit");

        int c = 0;
        for (const auto& energy : fEnergyPeaks) {
            std::string fName = "g" + std::to_string(c);
            double pos = energy * ratio;
            double start = pos * 0.85;
            double end = pos * 1.15;
            TF1* g = new TF1(fName.c_str(), "gaus", start, end);
            RESTDebug << "Fitting gaussian " << pos << " " << start << " " << end << RESTendl;
            spectrum->Fit(g, "R+");
            gauss.emplace_back(g);
            gr->SetPoint(c, g->GetParameter(1), energy);
            fCalibPeaks.push_back(g->GetParameter(1));
            fCalibFWHM.push_back(g->GetParameter(2) * 235. / g->GetParameter(1));
            c++;
        }

        if (GetVerboseLevel() >= TRestStringOutput::REST_Verbose_Level::REST_Debug) GetChar();

        gr->SetPoint(c, 0, 0);

        gr->Draw("AP");
        linearFit = std::unique_ptr<TF1>(new TF1("linearFit", "pol1"));
        gr->Fit("linearFit", "S");
    } else {
        TFile* f = TFile::Open(fCalibFile.c_str());
        if (f == nullptr) {
            RESTError << "Cannot open calibration file " << fCalibFile << RESTendl;
            exit(1);
        }
        RESTInfo << "Opening " << fCalibFile << RESTendl;
        gr = std::unique_ptr<TGraph>(f->Get<TGraph>("grFit"));
        linearFit = std::unique_ptr<TF1>(f->Get<TF1>("linearFit"));
    }

    fSlope = linearFit->GetParameter(1);
    fIntercept = linearFit->GetParameter(0);

    RESTDebug << "Slope " << fSlope << " Intercept " << fIntercept << RESTendl;

    auto df = dataSet.GetDataFrame();

    auto calibrate = [&linearFit](double val) {
        return linearFit->GetParameter(1) * val + linearFit->GetParameter(0);
    };
    df = df.Define("calib_Energy", calibrate, {fCalObservable});

    dataSet.SetDataSet(df);

    if (!fOutputFileName.empty()) {
        if (TRestTools::GetFileNameExtension(fOutputFileName) == "root") {
            dataSet.Export(fOutputFileName);
            TFile* f = TFile::Open(fOutputFileName.c_str(), "UPDATE");
            this->Write();
            if (gr) gr->Write();
            if (linearFit) linearFit->Write();
            // if(lFit)lFit->Write();
            // spectrumFit->Write();
            if (spectrum) spectrum->Write();
            f->Close();
        }
    }
}

/////////////////////////////////////////////
/// \brief Prints on screen the information about the metadata members of TRestCalibration
///
void TRestCalibration::PrintMetadata() {
    TRestMetadata::PrintMetadata();

    RESTMetadata << " Energy peaks to calibrate: ";
    for (const auto& peak : fEnergyPeaks) RESTMetadata << peak << " keV; ";
    RESTMetadata << RESTendl;
    RESTMetadata << " Calibrated peak position: ";
    for (const auto& peak : fCalibPeaks) RESTMetadata << peak << " ADC; ";
    RESTMetadata << RESTendl;
    RESTMetadata << " Calibrated FWHM: ";
    for (const auto& fwhm : fCalibFWHM) RESTMetadata << fwhm << " %; ";
    RESTMetadata << RESTendl;
    RESTMetadata << "Observable to calibrate: " << fCalObservable << RESTendl;
    RESTMetadata << "Calibration range: (" << fCalibRange.X() << ", " << fCalibRange.Y() << ")" << RESTendl;
    RESTMetadata << "Number of bins: " << fNBins << RESTendl;
    RESTMetadata << "Slope: " << fSlope << " keV/ADC" << RESTendl;
    RESTMetadata << "Intercept: " << fIntercept << " keV" << RESTendl;
    RESTMetadata << "----" << RESTendl;
}
