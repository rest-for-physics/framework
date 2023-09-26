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
/// TRestCalibrationCorrection calculates and stores the calibration
/// parameters for a given detector with multiple (or just one) modules.
/// This modules are defined in the Module class. It performs a gain correction
/// based on a spatial segmentation of the detector module. This is useful for
/// big modules such as the ones used in TREX-DM experiment.
///
/// ### Parameters
/// * **calibFileName**: name of the file to use for the calibration. It should be a DataSet
/// * **outputFileName**: name of the file to save the this calibration metadata
/// * **observable**: name of the observable to be calibrated. It must be a branch of the calibration DataSet
/// * **spatialObservableX**: name of the observable to be used for the spatial segmentation along the X axis.
/// It must be a branch of the calibration DataSet
/// * **spatialObservableY**: name of the observable to be used for the spatial segmentation along the Y axis.
/// It must be a branch of the calibration DataSet
/// * **modulesCal**: vector of Module objects
///
/// ### Examples
/// Give examples of usage and RML descriptions that can be tested.
/// \code
///     <TRestCalibrationCorrection name="calib" verboseLevel="info">
///             <parameter name="calibFileName" value="myDataSet.root"/>
///             <parameter name="outputFileName" value = "myCalibration.root"/>
///             <parameter name = "observable" value="rawAna_ThresholdIntegral"/>
///             <parameter name = "spatialObservableX" value="hitsAna_xMean"/>
///             <parameter name = "spatialObservableY" value="hitsAna_yMean"/>
///             <module planeId="0" moduleId="0"
///                 moduleDefinitionCut="TREXsides_tagId==1"
///                 numberOfSegmentsX="5"
///                 numberOfSegmentsY="5"
///                 readoutRange="(-1,246.24)">
///                 <peak energy="22.5" range=""/>
///                 <peak energy="8.0"  range=""/>
///             </module>
///             <module planeId="1" moduleId="0"
///                 moduleDefinitionCut="TREXsides_tagId==2"
///                 numberOfSegmentsX="5"
///                 numberOfSegmentsY="5"
///                 readoutRange="(-1,246.24)">
///                 <peak energy="22.5" range=""/>
///                 <peak energy="8.0"  range=""/>
///             </module>
///   </TRestCalibrationCorrection>
/// \endcode
///
/// Example to calculate the calibration parameters over a calibration dataSet using restRoot:
/// \code
/// [0] TRestCalibrationCorrection cal ("makeCalibration.rml");
/// [1] cal.SetCalibrationFileName("myDataSet.root"); //if not already defined in rml file
/// [2] cal.SetOutputFileName("myCalibration.root"); //if not already defined in rml file
/// [3] cal.Calibrate();
/// [4] cal.Export();
/// \endcode
///
/// Example to calibrate a dataSet with the previously calculated calibration parameters
/// and draw the result (using restRoot):
/// \code
/// [0] TRestCalibrationCorrection cal;
/// [1] cal.Import("myCalibration.root");
/// [2] cal.CalibrateDataSet("dataSetToCalibrate.root", "calibratedDataSet.root");
/// [3] TRestDataSet ds("calibratedDataSet.root");
/// [4] auto h = ds->GetDataFrame().Histo1D({"hname", "",100,-1,40.}, "calib_ThresholdIntegral");
/// [5] h->Draw();
/// \endcode
///----------------------------------------------------------------------
///
/// REST-for-Physics - Software for Rare Event Searches Toolkit
///
/// History of developments:
///
/// 2023-May: First implementation of TRestCalibrationCorrection
/// Álvaro Ezquerro
///
/// \class TRestCalibrationCorrection
/// \author: Álvaro Ezquerro aezquerro@unizar.es
///
/// <hr>
///

#include "TRestCalibrationCorrection.h"

ClassImp(TRestCalibrationCorrection);
///////////////////////////////////////////////
/// \brief Default constructor
///
TRestCalibrationCorrection::TRestCalibrationCorrection() { Initialize(); }

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
/// corresponding TRestCalibrationCorrection section inside the RML.
///
TRestCalibrationCorrection::TRestCalibrationCorrection(const char* configFilename, std::string name)
    : TRestMetadata(configFilename) {
    LoadConfigFromFile(fConfigFileName, name);
    if (GetVerboseLevel() >= TRestStringOutput::REST_Verbose_Level::REST_Info) PrintMetadata();
}

///////////////////////////////////////////////
/// \brief Default destructor
///
TRestCalibrationCorrection::~TRestCalibrationCorrection() {}

void TRestCalibrationCorrection::Initialize() { SetSectionName(this->ClassName()); }
///////////////////////////////////////////////
/// \brief Initialization of TRestCalibrationCorrection
/// members through a RML file
///
void TRestCalibrationCorrection::InitFromConfigFile() {
    this->Initialize();
    TRestMetadata::InitFromConfigFile();

    // Load Module from RML
    TiXmlElement* moduleDefinition = GetElement("module");
    while (moduleDefinition != nullptr) {
        fModulesCal.push_back(Module(*this));
        fModulesCal.back().LoadConfigFromTiXmlElement(moduleDefinition);

        moduleDefinition = GetNextElement(moduleDefinition);
    }

    if (GetVerboseLevel() >= TRestStringOutput::REST_Verbose_Level::REST_Debug) PrintMetadata();
}

/////////////////////////////////////////////
/// \brief Function to calculate the calibration parameters of all modules
///
void TRestCalibrationCorrection::Calibrate() {
    for (auto& mod : fModulesCal) {
        RESTInfo << "Calibrating plane " << mod.GetPlaneId() << " module " << mod.GetModuleId() << RESTendl;
        mod.CalculateCalibrationParameters();
        if (GetVerboseLevel() >= TRestStringOutput::REST_Verbose_Level::REST_Info) {
            mod.DrawSpectrum();
            mod.DrawGainMap();
        }
    }
}

/////////////////////////////////////////////
/// \brief Function to calibrate a dataSet
///
void TRestCalibrationCorrection::CalibrateDataSet(const std::string& dataSetFileName,
                                                  std::string outputFileName) {
    if (fModulesCal.empty()) {
        RESTError << "TRestCalibrationCorrection::CalibrateDataSet: No modules defined." << RESTendl;
        return;
    }

    TRestDataSet dataSet;
    dataSet.Import(dataSetFileName);
    auto dataFrame = dataSet.GetDataFrame();

    // Define a new column with the identifier (pmID) of the module for each row (event)
    std::string modCut = fModulesCal[0].GetModuleDefinitionCut();
    std::string pmIDname = (std::string)GetName() + "_pmID";
    int pmID = fModulesCal[0].GetPlaneId() * 10 + fModulesCal[0].GetModuleId();
    dataFrame = dataFrame.Define(pmIDname, modCut + " ? " + std::to_string(pmID) + " : -1");
    for (size_t n = 1; n < fModulesCal.size(); n++) {
        modCut = fModulesCal[n].GetModuleDefinitionCut();
        pmID = fModulesCal[n].GetPlaneId() * 10 + fModulesCal[n].GetModuleId();
        dataFrame = dataFrame.Redefine(pmIDname, (modCut + " ? " + std::to_string(pmID) + " : " + pmIDname));
    }

    // Define a new column with the calibrated observable
    auto calibrate = [this](double val, double x, double y, int pmID) {
        for (auto& m : fModulesCal) {
            if (pmID == m.GetPlaneId() * 10 + m.GetModuleId())
                return m.GetSlope(x, y) * val + m.GetIntercept(x, y);
        }
        // RESTError << "TRestCalibrationCorrection::CalibrateDataSet: Module not found for pmID " << pmID <<
        // RESTendl;
        return std::numeric_limits<double>::quiet_NaN();
    };
    std::string calibObsName = (std::string)GetName() + "_";
    calibObsName += GetObservable().erase(0, GetObservable().find("_") + 1);  // remove the "rawAna_" part
    dataFrame = dataFrame.Define(calibObsName, calibrate,
                                 {fObservable, fSpatialObservableX, fSpatialObservableY, pmIDname});

    dataSet.SetDataFrame(dataFrame);

    // Format the output file name and export the dataSet
    if (outputFileName.empty()) {
        outputFileName = dataSetFileName;
        RESTWarning << "TRestCalibrationCorrection::CalibrateDataSet: No output file name defined. "
                    << "Using input file name " << outputFileName << RESTendl;
    } else if (TRestTools::GetFileNameExtension(outputFileName) != "root")
        outputFileName = outputFileName + ".root";

    RESTInfo << "Exporting calibrated dataSet to " << outputFileName << RESTendl;
    dataSet.Export(outputFileName);

    // Add this TRestCalibrationCorrection metadata to the output file
    TFile* f = TFile::Open(outputFileName.c_str(), "UPDATE");
    this->Write();
    f->Close();
    delete f;
}

/////////////////////////////////////////////
/// \brief Function to retrieve the module calibration with planeID and moduleID
///
///
TRestCalibrationCorrection::Module* TRestCalibrationCorrection::GetModuleCalibration(const int planeID,
                                                                                     const int moduleID) {
    for (auto& i : fModulesCal) {
        if (i.GetPlaneId() == planeID && i.GetModuleId() == moduleID) return &i;
    }
    RESTError << "No ModuleCalibration with planeID " << planeID << " and moduleID " << moduleID << RESTendl;
    return nullptr;
}

/////////////////////////////////////////////
/// \brief Function to get the slope parameter of the module with
/// planeID and moduleID at physical position (x,y)
///
///
double TRestCalibrationCorrection::GetSlopeParameter(const int planeID, const int moduleID, const double x,
                                                     const double y) {
    Module* moduleCal = GetModuleCalibration(planeID, moduleID);
    if (moduleCal == nullptr) return 0;  // return numeric_limits<double>::quiet_NaN()
    return moduleCal->GetSlope(x, y);
}

/////////////////////////////////////////////
/// \brief Function to get the intercept parameter of the module with
/// planeID and moduleID at physical position (x,y)
///
///
double TRestCalibrationCorrection::GetInterceptParameter(const int planeID, const int moduleID,
                                                         const double x, const double y) {
    Module* moduleCal = GetModuleCalibration(planeID, moduleID);
    if (moduleCal == nullptr) return 0;  // return numeric_limits<double>::quiet_NaN()
    return moduleCal->GetIntercept(x, y);
}

/////////////////////////////////////////////
/// \brief Function to get a list (set) of the plane IDs
///
///
std::set<int> TRestCalibrationCorrection::GetPlaneIDs() const {
    std::set<int> planeIDs;
    for (const auto& mc : fModulesCal) planeIDs.insert(mc.GetPlaneId());
    return planeIDs;
}

/////////////////////////////////////////////
/// \brief Function to get a list (set) of the module IDs
/// of the plane with planeID
///
std::set<int> TRestCalibrationCorrection::GetModuleIDs(const int planeId) const {
    std::set<int> moduleIDs;
    for (const auto& mc : fModulesCal)
        if (mc.GetPlaneId() == planeId) moduleIDs.insert(mc.GetModuleId());
    return moduleIDs;
}

/////////////////////////////////////////////
/// \brief Function to get the map of the module IDs for each plane ID
///
std::map<int, std::set<int>> TRestCalibrationCorrection::GetModuleIDs() const {
    std::map<int, std::set<int>> moduleIds;
    for (const int planeId : GetPlaneIDs())
        moduleIds.insert(std::pair<int, std::set<int>>(planeId, GetModuleIDs(planeId)));
    return moduleIds;
}

TRestCalibrationCorrection& TRestCalibrationCorrection::operator=(TRestCalibrationCorrection& src) {
    SetName(src.GetName());
    fOutputFileName = src.GetOutputFileName();
    fObservable = src.GetObservable();
    fSpatialObservableX = src.GetSpatialObservableX();
    fSpatialObservableY = src.GetSpatialObservableY();
    fModulesCal.clear();
    for (auto pID : src.GetPlaneIDs())
        for (auto mID : src.GetModuleIDs(pID)) fModulesCal.push_back(*src.GetModuleCalibration(pID, mID));
    return *this;
}

/////////////////////////////////////////////
/// \brief Function to set a module calibration. If the module calibration
/// already exists (same planeId and moduleId), it will be replaced.
///
void TRestCalibrationCorrection::SetModuleCalibration(const Module& moduleCal) {
    for (auto& i : fModulesCal) {
        if (i.GetPlaneId() == moduleCal.GetPlaneId() && i.GetModuleId() == moduleCal.GetModuleId()) {
            i = moduleCal;
            return;
        }
    }
    fModulesCal.push_back(moduleCal);
}

/////////////////////////////////////////////
/// \brief Function to import the calibration parameters
/// from the root file fileName.
///
void TRestCalibrationCorrection::Import(const std::string& fileName) {
    if (fileName.empty()) {
        RESTError << "No input calibration file defined" << RESTendl;
        return;
    }

    if (TRestTools::isRootFile(fileName)) {
        RESTInfo << "Opening " << fileName << RESTendl;
        TFile* f = TFile::Open(fileName.c_str(), "READ");
        if (f == nullptr) {
            RESTError << "Cannot open calibration file " << fileName << RESTendl;
            return;
        }

        TRestCalibrationCorrection* cal = nullptr;
        if (f != nullptr) {
            TIter nextkey(f->GetListOfKeys());
            TKey* key;
            while ((key = (TKey*)nextkey())) {
                std::string kName = key->GetClassName();
                if (REST_Reflection::GetClassQuick(kName.c_str()) != nullptr &&
                    REST_Reflection::GetClassQuick(kName.c_str())
                        ->InheritsFrom("TRestCalibrationCorrection")) {
                    cal = f->Get<TRestCalibrationCorrection>(key->GetName());
                    *this = *cal;
                }
            }
        }
    } else
        RESTError << "File extension not supported for " << fileName << RESTendl;
    if (GetVerboseLevel() >= TRestStringOutput::REST_Verbose_Level::REST_Debug) PrintMetadata();
}

/////////////////////////////////////////////
/// \brief Function to export the calibration
/// to the file fileName.
///
void TRestCalibrationCorrection::Export(const std::string& fileName) {
    if (!fileName.empty()) fOutputFileName = fileName;
    if (fOutputFileName.empty()) {
        RESTError << "No output file defined" << RESTendl;
        return;
    }

    if (TRestTools::GetFileNameExtension(fOutputFileName) == "root") {
        TFile* f = TFile::Open(fOutputFileName.c_str(), "UPDATE");
        this->Write(GetName());
        f->Close();
        delete f;
        RESTInfo << "Calibration saved to " << fOutputFileName << RESTendl;
    } else
        RESTError << "File extension for " << fOutputFileName << "is not supported." << RESTendl;
}

/////////////////////////////////////////////
/// \brief Prints on screen the information about the metadata members
///
void TRestCalibrationCorrection::PrintMetadata() {
    TRestMetadata::PrintMetadata();
    RESTMetadata << " Calibration file: " << fCalibFileName << RESTendl;
    RESTMetadata << " Output file: " << fOutputFileName << RESTendl;
    RESTMetadata << " Number of planes:  " << GetNumberOfPlanes() << RESTendl;
    RESTMetadata << " Number of modules: " << GetNumberOfModules() << RESTendl;
    RESTMetadata << " Calibration observable: " << fObservable << RESTendl;
    RESTMetadata << " Spatial observable X: " << fSpatialObservableX << RESTendl;
    RESTMetadata << " Spatial observable Y: " << fSpatialObservableY << RESTendl;
    RESTMetadata << "-----------------------------------------------" << RESTendl;
    for (auto& i : fModulesCal) i.Print();
    RESTMetadata << "***********************************************" << RESTendl;
    RESTMetadata << RESTendl;
}

/////////////////////////////////////////////
/// \brief Function to get the index of the matrix of calibration parameters
/// for a given x and y position on the detector plane.
///
/// \param x A const double that defines the x position on the detector plane.
/// \param y A const double that defines the y position on the detector plane.
///
std::pair<int, int> TRestCalibrationCorrection::Module::GetIndexMatrix(const double x, const double y) const {
    int index_x = -1, index_y = -1;

    if (fSplitX.upper_bound(x) != fSplitX.end()) {
        index_x = std::distance(fSplitX.begin(), fSplitX.upper_bound(x)) - 1;
        if (index_x < 0) {
            RESTWarning << "index_x < 0 for x = " << x << " and fSplitX[0]=" << *fSplitX.begin()
                        << p->RESTendl;
            index_x = 0;
        }
    } else {
        RESTWarning << "x is out of split for x = " << x << p->RESTendl;
        index_x = fSplitX.size() - 2;
    }

    if (fSplitY.upper_bound(y) != fSplitY.end()) {
        index_y = std::distance(fSplitY.begin(), fSplitY.upper_bound(y)) - 1;
        if (index_y < 0) {
            RESTWarning << "index_y < 0 for y = " << y << " and fSplitY[0]=" << *fSplitY.begin()
                        << p->RESTendl;
            index_y = 0;
        }
    } else {
        RESTWarning << "y is out of split for y = " << y << p->RESTendl;
        index_y = fSplitY.size() - 2;
    }

    return std::make_pair(index_x, index_y);
}
/////////////////////////////////////////////
/// \brief Function to get the calibration parameter slope for a
/// given x and y position on the detector plane.
///
/// \param x A const double that defines the x position on the detector plane.
/// \param y A const double that defines the y position on the detector plane.
///
double TRestCalibrationCorrection::Module::GetSlope(const double x, const double y) const {
    auto [index_x, index_y] = GetIndexMatrix(x, y);
    if (fSlope.empty()) {
        RESTError << "Calibration slope matrix is empty. Returning 0" << p->RESTendl;
        return 0;
    }

    if (index_x > (int)fSlope.size() || index_y > (int)fSlope.at(0).size()) {
        RESTError << "Index out of range. Returning 0" << p->RESTendl;
        return 0;
    }

    return fSlope[index_x][index_y];
}

/////////////////////////////////////////////
/// \brief Function to get the calibration parameter intercept for a
/// given x and y position on the detector plane.
///
/// \param x A const double that defines the x position on the detector plane.
/// \param y A const double that defines the y position on the detector plane.
///
double TRestCalibrationCorrection::Module::GetIntercept(const double x, const double y) const {
    auto [index_x, index_y] = GetIndexMatrix(x, y);
    if (fIntercept.empty()) {
        RESTError << "Calibration constant matrix is empty. Returning 0" << p->RESTendl;
        return 0;
    }

    if (index_x > (int)fIntercept.size() || index_y > (int)fIntercept.at(0).size()) {
        RESTError << "Index out of range. Returning 0" << p->RESTendl;
        return 0;
    }

    return fIntercept[index_x][index_y];
}

/////////////////////////////////////////////
/// \brief Function to set the class members for segmentation of
/// the detector plane along the X and Y axis.
void TRestCalibrationCorrection::Module::SetSplits() {
    SetSplitX();
    SetSplitY();
}
/////////////////////////////////////////////
/// \brief Function to set the class members for segmentation of
/// the detector plane along the X axis.
///
/// It uses the number of segments and the readout range to define the
/// edges of the segments.
void TRestCalibrationCorrection::Module::SetSplitX() {
    fSplitX.clear();
    for (int i = 0; i <= fNumberOfSegmentsX; i++) {  // <= so that the last segment is included
        double x =
            fReadoutRange.X() + ((fReadoutRange.Y() - fReadoutRange.X()) / (float)fNumberOfSegmentsX) * i;
        fSplitX.insert(x);
    }
}
/////////////////////////////////////////////
/// \brief Function to set the class members for segmentation of
/// the detector plane along the Y axis.
///
/// It uses the number of segments and the readout range to define the
/// edges of the segments.
void TRestCalibrationCorrection::Module::SetSplitY() {
    fSplitY.clear();
    for (int i = 0; i <= fNumberOfSegmentsY; i++) {  // <= so that the last segment is included
        double y =
            fReadoutRange.X() + ((fReadoutRange.Y() - fReadoutRange.X()) / (float)fNumberOfSegmentsY) * i;
        fSplitY.insert(y);
    }
}

/////////////////////////////////////////////
/// \brief Function calculate the calibration parameters for each segment
/// defined at fSplitX and fSplitY.
///
/// It uses the data of the observable fObservable from the TRestDataSet
/// at fDataSetFileName (or fCalibrationFileName if first is empty).
/// The segmentation is given by the splits.
///
/// Fitting ranges logic is as follows:
/// 1. If fRangePeaks is defined and fAutoRangePeaks is false: fRangePeaks is used.
/// 2. If fRangePeaks is defined and fAutoRangePeaks is true: the fitting range
///     is calculated by the peak position found by TSpectrum inside fRangePeaks.
/// 3. If fRangePeaks is not defined and fAutoRangePeaks is false: use the peak position found by TSpectrum
///    and the peak position of the next peak to define the range.
/// 4. If fRangePeaks is not defined and fAutoRangePeaks is true: same as 3.
///
void TRestCalibrationCorrection::Module::CalculateCalibrationParameters() {
    //--- Initial checks and settings ---
    if (fDataSetFileName.empty()) fDataSetFileName = p->GetCalibrationFileName();
    if (fDataSetFileName.empty()) {
        RESTError << "No calibration file defined" << p->RESTendl;
        return;
    }

    if (!TRestTools::isDataSet(fDataSetFileName))
        RESTWarning << fDataSetFileName << " is not a dataset." << p->RESTendl;
    TRestDataSet dataSet;
    dataSet.Import(fDataSetFileName);
    SetSplits();

    //--- Get the calibration range if not provided (default is 0,0) ---
    if (fCalibRange.X() >= fCalibRange.Y()) {
        // Get spectrum for this file
        std::string cut = fDefinitionCut;
        if (cut.empty()) cut = "1";
        auto histo = dataSet.GetDataFrame().Filter(cut).Histo1D({"temp", "", fNBins, 0, 0}, GetObservable());
        std::unique_ptr<TH1F> hpunt = std::unique_ptr<TH1F>(static_cast<TH1F*>(histo->Clone()));
        // double xMin = hpunt->GetXaxis()->GetXmin();
        double xMax = hpunt->GetXaxis()->GetXmax();

        // Reduce the range to avoid the possible empty (nCounts<1%) end part of the spectrum
        double fraction = 1, nAtEndSpc = 0, step = 0.66;
        while (nAtEndSpc * 1. / hpunt->Integral() < 0.01 && fraction > 0.001) {
            fraction *= step;
            nAtEndSpc = hpunt->Integral(hpunt->FindFixBin(hpunt->GetXaxis()->GetXmax() * fraction),
                                        hpunt->FindFixBin(hpunt->GetXaxis()->GetXmax()));
        }
        xMax = hpunt->GetXaxis()->GetXmax() * fraction /
               step;  // previous step is the last one that nAtEndSpc<1%
        // Set the calibration range if needed
        // fCalibRange.SetX(xMin);
        fCalibRange.SetY(xMax);
        hpunt.reset();  // delete hpunt;
        RESTDebug << "Calibration range (auto)set to (" << fCalibRange.X() << "," << fCalibRange.Y() << ")"
                  << p->RESTendl;
    }

    //--- Definition of histogram matrix ---
    std::vector<std::vector<TH1F*>> h(fNumberOfSegmentsX, std::vector<TH1F*>(fNumberOfSegmentsY, nullptr));
    for (size_t i = 0; i < h.size(); i++) {
        for (size_t j = 0; j < h.at(0).size(); j++) {
            h[i][j] = new TH1F("", "", fNBins, fCalibRange.X(),
                               fCalibRange.Y());  // h[column][row] equivalent to h[x][y]
        }
    }
    std::vector<std::vector<double>> calParSlope(fNumberOfSegmentsX,
                                                 std::vector<double>(fNumberOfSegmentsY, 0));
    std::vector<std::vector<double>> calParIntercept(fNumberOfSegmentsX,
                                                     std::vector<double>(fNumberOfSegmentsY, 0));

    // build the spectrum for each segment
    auto itX = fSplitX.begin();
    for (size_t i = 0; i < h.size(); i++) {
        auto itY = fSplitY.begin();
        for (size_t j = 0; j < h.at(0).size(); j++) {
            // Get the segment limits from the splits
            auto xLower = *itX;
            auto xUpper = *std::next(itX);
            auto yLower = *itY;
            auto yUpper = *std::next(itY);

            std::string segment_cut = "";
            if (!GetSpatialObservableX().empty())
                segment_cut += GetSpatialObservableX() + ">=" + std::to_string(xLower) + "&&" +
                               GetSpatialObservableX() + "<" + std::to_string(xUpper);
            if (!GetSpatialObservableY().empty())
                segment_cut += "&&" + GetSpatialObservableY() + ">=" + std::to_string(yLower) + "&&" +
                               GetSpatialObservableY() + "<" + std::to_string(yUpper);
            if (!fDefinitionCut.empty()) segment_cut += "&&" + fDefinitionCut;
            if (segment_cut.empty()) segment_cut = "1";
            RESTExtreme << "Segment[" << i << "][" << j << "] cut: " << segment_cut << p->RESTendl;
            auto histo = dataSet.GetDataFrame()
                             .Filter(segment_cut)
                             .Histo1D({"temp", "", h[i][j]->GetNbinsX(), h[i][j]->GetXaxis()->GetXmin(),
                                       h[i][j]->GetXaxis()->GetXmax()},
                                      GetObservable());
            std::unique_ptr<TH1F> hpunt = std::unique_ptr<TH1F>(static_cast<TH1F*>(histo->Clone()));
            h[i][j]->Add(hpunt.get());
            hpunt.reset();  // delete hpunt;
            itY++;
        }
        itX++;
    }

    //--- Fit every peak energy for every segment ---
    fSegLinearFit = std::vector(h.size(), std::vector<TGraph*>(h.at(0).size(), nullptr));
    for (size_t i = 0; i < h.size(); i++) {
        for (size_t j = 0; j < h.at(0).size(); j++) {
            RESTExtreme << "Segment[" << i << "][" << j << "]" << p->RESTendl;
            // Search for peaks --> peakPos
            std::unique_ptr<TSpectrum> s(new TSpectrum(2 * fEnergyPeaks.size() + 1));
            std::vector<double> peakPos;
            s->Search(h[i][j], 2, "goff", 0.1);
            for (int k = 0; k < s->GetNPeaks(); k++) peakPos.push_back(s->GetPositionX()[k]);
            std::sort(peakPos.begin(), peakPos.end(), std::greater<double>());
            const double ratio = peakPos.size() == 0
                                     ? 1
                                     : peakPos.front() / fEnergyPeaks.front();  // to estimate peak position

            // Initialize graph for linear fit
            std::shared_ptr<TGraph> gr;
            gr = std::shared_ptr<TGraph>(new TGraph());
            gr->SetName("grFit");
            gr->SetTitle((";" + GetObservable() + ";energy").c_str());

            // Fit every energy peak
            int c = 0;
            double mu = 0;
            for (const auto& energy : fEnergyPeaks) {
                RESTExtreme << "\t fitting energy " << DoubleToString(energy, "%g") << p->RESTendl;
                // estimation of the peak position is between start and end
                double pos = energy * ratio;
                double start = pos * 0.8;
                double end = pos * 1.2;
                if (fRangePeaks.at(c).X() < fRangePeaks.at(c).Y()) {  // if range is defined use it
                    start = fRangePeaks.at(c).X();
                    end = fRangePeaks.at(c).Y();
                }

                do {
                    if (fAutoRangePeaks) {
                        if (peakPos.size() > 0) {
                            // Find the peak position that is between start and end
                            pos = peakPos.at(0);
                            while (!(start < pos && pos < end)) {
                                // if none of the peak position is
                                // between start and end, use the greater one.
                                if (pos == peakPos.back()) {
                                    pos = peakPos.at(0);
                                    break;
                                }
                                pos = *std::next(std::find(peakPos.begin(), peakPos.end(),
                                                           pos));  // get next peak position
                            }
                            peakPos.erase(std::find(peakPos.begin(), peakPos.end(),
                                                    pos));  // remove this peak position from the list
                            // final estimation of the peak range (idem fitting window) with this peak
                            // position pos
                            start = pos * 0.8;
                            end = pos * 1.2;
                            const double relDist = peakPos.size() > 0 ? (pos - peakPos.front()) / pos : 999;
                            if (relDist < 0.2) {  // if the next peak is too close reduce the window width
                                start = pos * (1 - relDist / 2);
                                end = pos * (1 + relDist / 2);
                            }
                        }
                    }

                    std::string name = "g" + std::to_string(c);
                    TF1* g = new TF1(name.c_str(), "gaus", start, end);
                    RESTExtreme << "\t\tat " << DoubleToString(pos, "%.3g") << ". Range("
                                << DoubleToString(start, "%.3g") << ", " << DoubleToString(end, "%.3g") << ")"
                                << p->RESTendl;

                    if (h[i][j]->GetFunction(name.c_str()))  // remove previous fit
                        h[i][j]->GetListOfFunctions()->Remove(h[i][j]->GetFunction(name.c_str()));

                    h[i][j]->Fit(g, "R+Q0");  // use 0 to not draw the fit but save it
                    mu = g->GetParameter(1);
                    RESTExtreme << "\t\tgaus mean " << DoubleToString(mu, "%g") << p->RESTendl;
                } while (fAutoRangePeaks && peakPos.size() > 0 &&
                         !(start < mu && mu < end));  // avoid small peaks on main peak tail
                gr->SetPoint(c++, mu, energy);
            }
            s.reset();  // delete s;

            if (fZeroPoint) gr->SetPoint(c++, 0, 0);
            while (gr->GetN() < 2) {  // minimun 2 points needed for linear fit
                gr->SetPoint(c++, 0, 0);
                SetZeroPoint(true);
                RESTDebug << "Not enough points for linear fit. Adding and setting zero point to true"
                          << p->RESTendl;
            }

            // Linear fit
            std::unique_ptr<TF1> linearFit;
            linearFit = std::unique_ptr<TF1>(new TF1("linearFit", "pol1"));
            gr->Fit("linearFit", "SQ");  // Q for quiet mode

            fSegLinearFit.at(i).at(j) = (TGraph*)gr->Clone();

            const double slope = linearFit->GetParameter(1);
            const double intercept = linearFit->GetParameter(0);
            calParSlope.at(i).at(j) = slope;
            calParIntercept.at(i).at(j) = intercept;
        }
    }
    fSegSpectra = h;
    fSlope = calParSlope;
    fIntercept = calParIntercept;
}

/////////////////////////////////////////////
/// \brief Function to fit again manually a peak for a given segment of the module.
///
/// \param x position along X-axis at the detector module (in physical units).
/// \param y position along Y-axis at the detector module (in physical units).
/// \param energyPeak The energy of the peak to be fitted (in physical units).
/// \param range The range for the fitting of the peak (in the observables corresponding units).
///
void TRestCalibrationCorrection::Module::Refit(const double x, const double y, const double energyPeak,
                                               const TVector2& range) {
    auto [index_x, index_y] = GetIndexMatrix(x, y);
    int peakNumber = -1;
    for (size_t i = 0; i < fEnergyPeaks.size(); i++)
        if (fEnergyPeaks.at(i) == energyPeak) {
            peakNumber = i;
            break;
        }
    if (peakNumber == -1) {
        RESTError << "Energy " << energyPeak << " not found in the list of energy peaks" << p->RESTendl;
        return;
    }
    Refit(index_x, index_y, peakNumber, range);
}

/////////////////////////////////////////////
/// \brief Function to fit again manually a peak for a given segment of the module.
///
/// \param x index along X-axis of the corresponding segment.
/// \param y index along Y-axis of the corresponding segment.
/// \param peakNumber The index of the peak to be fitted.
/// \param range The range for the fitting of the peak (in the observables corresponding units).
///
void TRestCalibrationCorrection::Module::Refit(const int x, const int y, const int peakNumber,
                                               const TVector2& range) {
    // Refit the desired peak
    std::string name = "g" + std::to_string(peakNumber);
    TF1* g = new TF1(name.c_str(), "gaus", range.X(), range.Y());
    TH1F* h = fSegSpectra.at(x).at(y);
    if (h->GetFunction(name.c_str()))  // remove previous fit
        h->GetListOfFunctions()->Remove(h->GetFunction(name.c_str()));
    h->Fit(g, "R+Q0");  // use 0 to not draw the fit but save it
    const double mu = g->GetParameter(1);

    // Change the point of the graph
    TGraph* gr = fSegLinearFit.at(x).at(y);
    gr->SetPoint(peakNumber, mu, fEnergyPeaks.at(peakNumber));

    // Refit the calibration curve
    TF1* lf = nullptr;
    if (gr->GetFunction("linearFit"))
        lf = gr->GetFunction("linearFit");
    else
        lf = new TF1("linearFit", "pol1");
    gr->Fit(lf, "SQ");  // Q for quiet mode
    fSlope.at(x).at(y) = lf->GetParameter(1);
    fIntercept.at(x).at(y) = lf->GetParameter(0);
}

/////////////////////////////////////////////
/// \brief Function to read the parameters from the RML element (TiXmlElement)
/// and set those class members.
///
/// \param module A const TiXmlElement pointer that contains the information.
/// Example of this RML element:
///        <module planeId="0" moduleId="0"
///                moduleDefinitionCut="TREXsides_tagId==1"
///                numberOfSegmentsX="5"
///                numberOfSegmentsY="5"
///                <peak energy="22.5" range="(250000,370000)"/>
///                <peak energy="8.0"  range=""/>
///         </module>
///
void TRestCalibrationCorrection::Module::LoadConfigFromTiXmlElement(const TiXmlElement* module) {
    if (module == nullptr) {
        RESTError << "TRestCalibrationCorrection::Module::LoadConfigFromTiXmlElement: module is nullptr"
                  << p->RESTendl;
        return;
    }

    std::string el = !module->Attribute("planeId") ? "Not defined" : module->Attribute("planeId");
    if (!(el.empty() || el == "Not defined")) this->SetPlaneId(StringToInteger(el));
    el = !module->Attribute("moduleId") ? "Not defined" : module->Attribute("moduleId");
    if (!(el.empty() || el == "Not defined")) this->SetModuleId(StringToInteger(el));

    el = !module->Attribute("moduleDefinitionCut") ? "Not defined" : module->Attribute("moduleDefinitionCut");
    if (!(el.empty() || el == "Not defined")) this->SetModuleDefinitionCut(el);

    el = !module->Attribute("numberOfSegmentsX") ? "Not defined" : module->Attribute("numberOfSegmentsX");
    if (!(el.empty() || el == "Not defined")) this->SetNumberOfSegmentsX(StringToInteger(el));
    el = !module->Attribute("numberOfSegmentsY") ? "Not defined" : module->Attribute("numberOfSegmentsY");
    if (!(el.empty() || el == "Not defined")) this->SetNumberOfSegmentsY(StringToInteger(el));
    el = !module->Attribute("readoutRange") ? "Not defined" : module->Attribute("readoutRange");
    if (!(el.empty() || el == "Not defined")) this->SetReadoutRange(StringTo2DVector(el));

    el = !module->Attribute("calibRange") ? "Not defined" : module->Attribute("calibRange");
    if (!(el.empty() || el == "Not defined")) this->SetCalibrationRange(StringTo2DVector(el));
    el = !module->Attribute("nBins") ? "Not defined" : module->Attribute("nBins");
    if (!(el.empty() || el == "Not defined")) this->SetNBins(StringToInteger(el));

    el = !module->Attribute("dataSetFileName") ? "Not defined" : module->Attribute("dataSetFileName");
    if (!(el.empty() || el == "Not defined")) this->SetDataSetFileName(el);

    el = !module->Attribute("zeroPoint") ? "Not defined" : module->Attribute("zeroPoint");
    if (!(el.empty() || el == "Not defined")) this->SetZeroPoint(ToLower(el) == "true");
    el = !module->Attribute("autoRangePeaks") ? "Not defined" : module->Attribute("autoRangePeaks");
    if (!(el.empty() || el == "Not defined")) this->SetAutoRangePeaks(ToLower(el) == "true");

    // Get peaks energy and range
    TiXmlElement* peakDefinition = (TiXmlElement*)module->FirstChildElement("peak");
    while (peakDefinition != nullptr) {
        double energy = 0;
        TVector2 range = TVector2(0, 0);

        std::string ell =
            !peakDefinition->Attribute("energy") ? "Not defined" : peakDefinition->Attribute("energy");
        if (ell.empty() || ell == "Not defined") {
            RESTError << "< peak variable key does not contain energy!" << p->RESTendl;
            exit(1);
        }
        energy = StringToDouble(ell);

        ell = !peakDefinition->Attribute("range") ? "Not defined" : peakDefinition->Attribute("range");
        if (!(ell.empty() || ell == "Not defined")) range = StringTo2DVector(ell);

        this->AddPeak(energy, range);
        peakDefinition = (TiXmlElement*)peakDefinition->NextSiblingElement();
    }
}

void TRestCalibrationCorrection::Module::DrawSpectrum(const double x, const double y, TCanvas* c) {
    std::pair<size_t, size_t> index = GetIndexMatrix(x, y);
    DrawSpectrum(index.first, index.second, c);
}

void TRestCalibrationCorrection::Module::DrawSpectrum(const size_t index_x, const size_t index_y,
                                                      TCanvas* c) {
    if (fSegSpectra.size() == 0) {
        RESTError << "Spectra matrix is empty." << p->RESTendl;
        return;
    }
    if (index_x >= fSegSpectra.size() || index_y >= fSegSpectra.at(index_x).size()) {
        RESTError << "Index out of range." << p->RESTendl;
        return;
    }

    if (!c) {
        std::string t = "spectrum_" + std::to_string(fPlaneId) + "_" + std::to_string(fModuleId) + "_" +
                        std::to_string(index_x) + "_" + std::to_string(index_y);
        c = new TCanvas(t.c_str(), t.c_str());
    }

    auto xLower = *std::next(fSplitX.begin(), index_x);
    auto xUpper = *std::next(fSplitX.begin(), index_x + 1);
    auto yLower = *std::next(fSplitY.begin(), index_y);
    auto yUpper = *std::next(fSplitY.begin(), index_y + 1);
    std::string tH = "Spectrum x=[" + DoubleToString(xLower, "%g") + ", " + DoubleToString(xUpper, "%g") +
                     ") y=[" + DoubleToString(yLower, "%g") + ", " + DoubleToString(yUpper, "%g") + ");" +
                     GetObservable() + ";counts";
    fSegSpectra[index_x][index_y]->SetTitle(tH.c_str());
    fSegSpectra[index_x][index_y]->Draw();
    for (size_t c = 0; c < fEnergyPeaks.size(); c++) {
        auto fit = fSegSpectra[index_x][index_y]->GetFunction(("g" + std::to_string(c)).c_str());
        if (!fit) RESTError << "Fit for energy peak" << fEnergyPeaks[c] << " not found." << p->RESTendl;
        if (!fit) break;
        fit->SetLineColor(c + 2);  // skipe 0 which is white and 1 which is blue as histogram
        fit->Draw("same");
    }
}

void TRestCalibrationCorrection::Module::DrawSpectrum() {
    if (fSegSpectra.size() == 0) {
        RESTError << "Spectra matrix is empty." << p->RESTendl;
        return;
    }
    std::string t = "spectra_" + std::to_string(fPlaneId) + "_" + std::to_string(fModuleId);
    TCanvas* myCanvas = new TCanvas(t.c_str(), t.c_str());
    myCanvas->Divide(fSegSpectra.size(), fSegSpectra.at(0).size());
    for (size_t i = 0; i < fSegSpectra.size(); i++) {
        for (size_t j = 0; j < fSegSpectra[i].size(); j++) {
            myCanvas->cd(i + 1 + fSegSpectra[i].size() * j);
            DrawSpectrum(i, fSegSpectra[i].size() - 1 - j, myCanvas);
        }
    }
}
void TRestCalibrationCorrection::Module::DrawFullSpectrum() {
    if (fSegSpectra.size() == 0) {
        RESTError << "Spectra matrix is empty." << p->RESTendl;
        return;
    }
    TH1F* sumHist =
        new TH1F("fullSpc", "", fSegSpectra[0][0]->GetNbinsX(), fSegSpectra[0][0]->GetXaxis()->GetXmin(),
                 fSegSpectra[0][0]->GetXaxis()->GetXmax());

    sumHist->SetTitle(("Full spectrum;" + GetObservable() + ";counts").c_str());
    for (size_t i = 0; i < fSegSpectra.size(); i++) {
        for (size_t j = 0; j < fSegSpectra.at(0).size(); j++) {
            sumHist->Add(fSegSpectra[i][j]);
        }
    }
    std::string t = "fullSpc_" + std::to_string(fPlaneId) + "_" + std::to_string(fModuleId);
    TCanvas* c = new TCanvas(t.c_str(), t.c_str());
    c->cd();
    sumHist->Draw();
}

void TRestCalibrationCorrection::Module::DrawLinearFit(const double x, const double y, TCanvas* c) {
    std::pair<size_t, size_t> index = GetIndexMatrix(x, y);
    DrawLinearFit(index.first, index.second, c);
}

void TRestCalibrationCorrection::Module::DrawLinearFit(const size_t index_x, const size_t index_y,
                                                       TCanvas* c) {
    if (fSegLinearFit.size() == 0) {
        RESTError << "Spectra matrix is empty." << p->RESTendl;
        return;
    }
    if (index_x >= fSegLinearFit.size() || index_y >= fSegLinearFit.at(index_x).size()) {
        RESTError << "Index out of range." << p->RESTendl;
        return;
    }
    if (!c) {
        std::string t = "linearFit_" + std::to_string(fPlaneId) + "_" + std::to_string(fModuleId) + "_" +
                        std::to_string(index_x) + "_" + std::to_string(index_y);
        c = new TCanvas(t.c_str(), t.c_str());
    }
    auto xLower = *std::next(fSplitX.begin(), index_x);
    auto xUpper = *std::next(fSplitX.begin(), index_x + 1);
    auto yLower = *std::next(fSplitY.begin(), index_y);
    auto yUpper = *std::next(fSplitY.begin(), index_y + 1);
    std::string tH = "Linear Fit x=[" + DoubleToString(xLower, "%g") + ", " + DoubleToString(xUpper, "%g") +
                     ") y=[" + DoubleToString(yLower, "%g") + ", " + DoubleToString(yUpper, "%g") + ");" +
                     GetObservable() + ";energy";
    fSegLinearFit[index_x][index_y]->SetTitle(tH.c_str());
    fSegLinearFit[index_x][index_y]->Draw("AL*");
}

void TRestCalibrationCorrection::Module::DrawLinearFit() {
    std::string t = "linearFits_" + std::to_string(fPlaneId) + "_" + std::to_string(fModuleId);
    TCanvas* myCanvas = new TCanvas(t.c_str(), t.c_str());
    myCanvas->Divide(fSegLinearFit.size(), fSegLinearFit.at(0).size());
    for (size_t i = 0; i < fSegLinearFit.size(); i++) {
        for (size_t j = 0; j < fSegLinearFit[i].size(); j++) {
            myCanvas->cd(i + 1 + fSegLinearFit[i].size() * j);
            // fSegLinearFit[i][j]->Draw("AL*");
            DrawLinearFit(i, fSegSpectra[i].size() - 1 - j, myCanvas);
        }
    }
}

void TRestCalibrationCorrection::Module::DrawGainMap(const int peakNumber) {
    if (peakNumber < 0 || peakNumber >= (int)fEnergyPeaks.size()) {
        RESTError << "Peak number out of range (peakNumber should be between 0 and "
                  << fEnergyPeaks.size() - 1 << " )" << p->RESTendl;
        return;
    }
    double peakEnergy = fEnergyPeaks[peakNumber];
    std::string title = "Gain map for energy " + DoubleToString(peakEnergy, "%g") + ";" +
                        GetSpatialObservableX() + ";" + GetSpatialObservableY();  // + " keV";
    std::string t = "gainMap" + std::to_string(peakNumber) + "_" + std::to_string(fPlaneId) + "_" +
                    std::to_string(fModuleId);
    TCanvas* gainMap = new TCanvas(t.c_str(), t.c_str());
    gainMap->cd();
    TH2F* hGainMap = new TH2F(("h" + t).c_str(), title.c_str(), fNumberOfSegmentsY, fReadoutRange.X(),
                              fReadoutRange.Y(), fNumberOfSegmentsX, fReadoutRange.X(), fReadoutRange.Y());

    const double peakPosRef =
        fSegLinearFit[(fNumberOfSegmentsX - 1) / 2][(fNumberOfSegmentsY - 1) / 2]->GetPointX(peakNumber);

    auto itX = fSplitX.begin();
    for (size_t i = 0; i < fSegLinearFit.size(); i++) {
        auto itY = fSplitY.begin();
        for (size_t j = 0; j < fSegLinearFit.at(0).size(); j++) {
            auto xLower = *itX;
            auto xUpper = *std::next(itX);
            auto yLower = *itY;
            auto yUpper = *std::next(itY);
            hGainMap->Fill((xUpper + xLower) / 2., (yUpper + yLower) / 2.,
                           fSegLinearFit[i][j]->GetPointX(peakNumber) / peakPosRef);
            itY++;
        }
        itX++;
    }
    hGainMap->SetStats(0);
    hGainMap->Draw("colz");
    hGainMap->SetBarOffset(0.2);
    hGainMap->Draw("TEXT SAME");
}

/////////////////////////////////////////////
/// \brief Prints on screen the information about the
/// members of Module
///
void TRestCalibrationCorrection::Module::Print() const {
    RESTMetadata << "-----------------------------------------------" << p->RESTendl;
    RESTMetadata << " Plane ID: " << fPlaneId << p->RESTendl;
    RESTMetadata << " Module ID: " << fModuleId << p->RESTendl;
    RESTMetadata << " Definition cut: " << fDefinitionCut << p->RESTendl;
    RESTMetadata << p->RESTendl;

    RESTMetadata << " DataSet: " << fDataSetFileName << p->RESTendl;
    RESTMetadata << p->RESTendl;

    RESTMetadata << " Energy peaks: ";
    for (const auto& peak : fEnergyPeaks) RESTMetadata << peak << ", ";
    RESTMetadata << p->RESTendl;
    bool anyRange = false;
    for (const auto& r : fRangePeaks)
        if (r.X() < r.Y()) {
            RESTMetadata << " Range peaks: ";
            anyRange = true;
            break;
        }
    if (anyRange)
        for (const auto& r : fRangePeaks) RESTMetadata << "(" << r.X() << ", " << r.Y() << ") ";
    if (anyRange) RESTMetadata << p->RESTendl;
    RESTMetadata << " Auto range peaks: " << (fAutoRangePeaks ? "true" : "false") << p->RESTendl;
    RESTMetadata << " Zero point: " << (fZeroPoint ? "true" : "false") << p->RESTendl;
    RESTMetadata << " Calibration range: (" << fCalibRange.X() << ", " << fCalibRange.Y() << " )"
                 << p->RESTendl;
    RESTMetadata << " Number of bins: " << fNBins << p->RESTendl;
    RESTMetadata << p->RESTendl;

    RESTMetadata << " Number of segments X: " << fNumberOfSegmentsX << p->RESTendl;
    RESTMetadata << " Number of segments Y: " << fNumberOfSegmentsY << p->RESTendl;
    // RESTMetadata << " Draw: " << (fDrawVar ? "true" : "false") << p->RESTendl;
    RESTMetadata << " Readout range (" << fReadoutRange.X() << ", " << fReadoutRange.Y() << " )"
                 << p->RESTendl;
    RESTMetadata << "SplitX: ";
    for (auto& i : fSplitX) {
        RESTMetadata << "    " << i;
    }
    RESTMetadata << p->RESTendl;
    RESTMetadata << "SplitY: ";
    for (auto& i : fSplitY) {
        RESTMetadata << "    " << i;
    }
    RESTMetadata << p->RESTendl;
    RESTMetadata << p->RESTendl;

    RESTMetadata << " Slope: " << p->RESTendl;
    size_t maxSize = 0;
    for (auto& x : fSlope)
        if (maxSize < x.size()) maxSize = x.size();
    for (size_t j = 0; j < maxSize; j++) {
        for (size_t k = 0; k < fSlope.size(); k++) {
            if (j < fSlope[k].size())
                RESTMetadata << DoubleToString(fSlope[k][fSlope[k].size() - 1 - j], "%.3e") << "   ";
            else
                RESTMetadata << "         ";
        }
        RESTMetadata << p->RESTendl;
    }
    RESTMetadata << " Intercept: " << p->RESTendl;
    maxSize = 0;
    for (auto& x : fIntercept)
        if (maxSize < x.size()) maxSize = x.size();
    for (size_t j = 0; j < maxSize; j++) {
        for (size_t k = 0; k < fIntercept.size(); k++) {
            if (j < fIntercept[k].size())
                RESTMetadata << DoubleToString(fIntercept[k][fIntercept[k].size() - 1 - j], "%+.3e") << "   ";
            else
                RESTMetadata << "         ";
        }
        RESTMetadata << p->RESTendl;
    }
    RESTMetadata << "-----------------------------------------------" << p->RESTendl;
}
