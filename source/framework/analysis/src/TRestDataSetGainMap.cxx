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
/// TRestDataSetGainMap calculates and stores the calibration
/// parameters for a given detector with multiple (or just one) modules.
/// The modules are defined using the Module class (defined internally).
/// It performs a gain correction based on a spatial segmentation of the
/// detector module. This is useful forbig modules such as the ones used
/// in TREX-DM experiment. The input data files for this methods are
/// TRestDataSet for both calculating the calibration parameters and
/// performing the calibration of the desired events.
///
/// To correct the gain inhomogeneities, the module is divided in
/// fNumberOfSegmentsX*fNumberOfSegmentsY segments. The energy peaks provided
/// are fitted in each segment. The events are associated to each segment based on
/// the observables fSpatialObservableX and fSpatialObservablesY. This results in
/// the following plot that can be obtain with the function
/// TRestDataSetGainMap::Module::DrawSpectrum()
///
/// \htmlonly <style>div.image img[src="drawSpectrum.png"]{width:300px;}</style> \endhtmlonly
/// ![Peak fitting of each segment. Plot obtain with
/// TRestDataSetGainMap::Module::DrawSpectrum()](drawSpectrum.png)
///
/// Also, the peak position provides a gain map that can be plotted with the function
/// TRestDataSetGainMap::Module::DrawGainMap(peakNumber)
///
/// \htmlonly <style>div.image img[src="drawGainMap.png"]{width:200px;}</style> \endhtmlonly
/// ![Gain map. Plot obtain with TRestDataSetGainMap::Module::DrawGainMap()](drawGainMap.png)
///
/// The result is a better energy resolution with the gain corrected
/// calibration (red) than the plain calibration (blue).
///
/// \htmlonly <style>div.image img[src="gainCorrectionComparison.png"]{width:200px;}</style> \endhtmlonly
/// ![Gain correction comparison.](gainCorrectionComparison.png)

/// ### Parameters
/// * **calibFileName**: name of the file to use for the calibration. It should be a TRestDataSet
/// * **outputFileName**: name of the file to save this calibration metadata
/// * **observable**: name of the observable to be calibrated. It must be a branch of the calibration
/// TRestDataSet
/// * **spatialObservableX**: name of the observable to be used for the spatial segmentation along the X axis.
/// It must be a column (branch) of the calibration TRestDataSet
/// * **spatialObservableY**: name of the observable to be used for the spatial segmentation along the Y axis.
/// It must be a column (branch) of the calibration TRestDataSet
/// * **modulesCal**: vector of Module objects
///
/// ### Examples
/// A example of RML definition of this class can be found inside the file
/// `REST_PATH/examples/calibrationCorrection.rml`. This have the following structure:
/// \code
///     <TRestDataSetGainMap name="calib">
///             ...
///             <module>
///                 ...
///             </module>
///             <module>
///                 ...
///             </module>
///   </TRestDataSetGainMap>
/// \endcode
///
/// Example to calculate the calibration parameters over a calibration dataSet using restRoot:
/// \code
/// TRestDataSetGainMap gm ("calibrationCorrection.rml");
/// gm.SetCalibrationFileName("myDataSet.root"); //if not already defined in rml file
/// gm.SetOutputFileName("myCalibration.root"); //if not already defined in rml file
/// gm.GenerateGainMap();
/// gm.Export(); // gm.Export("anyOtherFileName.root")
/// \endcode
///
/// Example to calibrate a dataSet with the previously calculated calibration parameters
/// and draw the result (using restRoot):
/// \code
/// TRestDataSetGainMap gm;
/// gm.Import("myCalibration.root");
/// gm.CalibrateDataSet("dataSetToCalibrate.root", "calibratedDataSet.root");
/// TRestDataSet ds;
/// ds.Import("calibratedDataSet.root");
/// auto h = ds.GetDataFrame().Histo1D({"hname", "",100,-1,40.}, "calib_ThresholdIntegral");
/// h->Draw();
/// \endcode
///
/// Example to refit manually the peaks of the gain map if any of them is not well fitted
/// (using restRoot):
/// \code
/// TRestDataSetGainMap gm;
/// gm.Import("myCalibration.root");
/// // Draw all segmented spectra and check if any need a refit
/// for (auto pID : gm.GetPlaneIDs())
///     for (auto mID : gm.GetModuleIDs(pID))
///         gm.GetModule(pID,mID)->DrawSpectrum();
/// // Draw only the desired spectrum (segment 0,0 in this case)
/// gm.GetModule(0,0)->DrawSpectrum(0, 0);
/// // Refit the desired peak (peak with energy 22.5 in this case) with a new range
/// TVector2 range(100000, 200000); // Define here the new range for the fit as you wish
/// gm.GetModule(0,0)->Refit(TVector2(0,0), 22.5, range);
/// // Check the result
/// gm.GetModule(0,0)->DrawSpectrum(TVector2(0.0,0.0)); // using x,y physical coord is possible
/// // Export the new calibration
/// gm.Export(); // gm.Export("anyOtherFileName.root")
/// \endcode
///
///----------------------------------------------------------------------
///
/// REST-for-Physics - Software for Rare Event Searches Toolkit
///
/// History of developments:
///
/// 2023-September: First implementation of TRestDataSetGainMap
/// Álvaro Ezquerro
///
/// \class TRestDataSetGainMap
/// \author: Álvaro Ezquerro aezquerro@unizar.es
///
/// <hr>
///

#include "TRestDataSetGainMap.h"

ClassImp(TRestDataSetGainMap);
///////////////////////////////////////////////
/// \brief Default constructor
///
TRestDataSetGainMap::TRestDataSetGainMap() { Initialize(); }

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
/// corresponding TRestDataSetGainMap section inside the RML.
///
TRestDataSetGainMap::TRestDataSetGainMap(const char* configFilename, std::string name)
    : TRestMetadata(configFilename) {
    LoadConfigFromFile(fConfigFileName, name);
    if (GetVerboseLevel() >= TRestStringOutput::REST_Verbose_Level::REST_Info) PrintMetadata();
}

///////////////////////////////////////////////
/// \brief Default destructor
///
TRestDataSetGainMap::~TRestDataSetGainMap() {}

void TRestDataSetGainMap::Initialize() { SetSectionName(this->ClassName()); }
///////////////////////////////////////////////
/// \brief Initialization of TRestDataSetGainMap
/// members through a RML file
///
void TRestDataSetGainMap::InitFromConfigFile() {
    this->Initialize();
    TRestMetadata::InitFromConfigFile();

    // Load Module from RML
    TiXmlElement* moduleDefinition = GetElement("module");
    while (moduleDefinition != nullptr) {
        fModulesCal.push_back(Module(*this));
        fModulesCal.back().LoadConfigFromTiXmlElement(moduleDefinition);

        moduleDefinition = GetNextElement(moduleDefinition);
    }

    fCut = (TRestCut*)InstantiateChildMetadata("TRestCut");

    if (GetVerboseLevel() >= TRestStringOutput::REST_Verbose_Level::REST_Debug) PrintMetadata();
}

/////////////////////////////////////////////
/// \brief Function to calculate the calibration parameters of all modules
///
void TRestDataSetGainMap::GenerateGainMap() {
    for (auto& mod : fModulesCal) {
        RESTInfo << "Calibrating plane " << mod.GetPlaneId() << " module " << mod.GetModuleId() << RESTendl;
        mod.GenerateGainMap();
        if (GetVerboseLevel() >= TRestStringOutput::REST_Verbose_Level::REST_Info) {
            mod.DrawSpectrum();
            mod.DrawGainMap();
        }
    }
}

/////////////////////////////////////////////
/// \brief Function to calibrate a dataset.
///
/// \param dataSetFileName the name of the root file where the TRestDataSet to be calibrated is stored.
/// \param outputFileName the name of the output (root) file where the calibrated TRestDataSet will be
/// exported. If empty, the output file will be named as the input file with the suffix "_cc". E.g.
/// "data/myDataSet.root" -> "data/myDataSet_cc.root".
///
void TRestDataSetGainMap::CalibrateDataSet(const std::string& dataSetFileName, std::string outputFileName) {
    if (fModulesCal.empty()) {
        RESTError << "TRestDataSetGainMap::CalibrateDataSet: No modules defined." << RESTendl;
        return;
    }

    TRestDataSet dataSet;
    dataSet.EnableMultiThreading(true);
    dataSet.Import(dataSetFileName);
    auto dataFrame = dataSet.GetDataFrame();

    // Define a new column with the identifier (pmID) of the module for each row (event)
    std::string pmIDname = (std::string)GetName() + "_pmID";
    std::string modCut = fModulesCal[0].GetModuleDefinitionCut();
    int pmID = fModulesCal[0].GetPlaneId() * 10 + fModulesCal[0].GetModuleId();

    auto columnList = dataFrame.GetColumnNames();
    if (std::find(columnList.begin(), columnList.end(), pmIDname) == columnList.end())
        dataFrame = dataFrame.Define(pmIDname, modCut + " ? " + std::to_string(pmID) + " : -1");
    else
        dataFrame = dataFrame.Redefine(pmIDname, modCut + " ? " + std::to_string(pmID) + " : -1");

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
        // RESTError << "TRestDataSetGainMap::CalibrateDataSet: Module not found for pmID " << pmID <<
        // RESTendl;
        return std::numeric_limits<double>::quiet_NaN();
    };
    std::string calibObsName = (std::string)GetName() + "_";
    calibObsName += GetObservable().erase(0, GetObservable().find("_") + 1);  // remove the "rawAna_" part
    dataFrame = dataFrame.Define(calibObsName, calibrate,
                                 {fObservable, fSpatialObservableX, fSpatialObservableY, pmIDname});

    dataSet.SetDataFrame(dataFrame);

    // Format the output file name and export the dataSet
    if (outputFileName.empty()) outputFileName = dataSetFileName;

    if (outputFileName == dataSetFileName) {  // TRestDataSet cannot be overwritten
        outputFileName = outputFileName.substr(0, outputFileName.find_last_of(".")) + "_cc.";
        outputFileName += TRestTools::GetFileNameExtension(dataSetFileName);
    }

    RESTInfo << "Exporting calibrated dataSet to " << outputFileName << RESTendl;
    dataSet.Export(outputFileName);

    // Add this TRestDataSetGainMap metadata to the output file
    TFile* f = TFile::Open(outputFileName.c_str(), "UPDATE");
    this->Write();
    f->Close();
    delete f;
}

/////////////////////////////////////////////
/// \brief Function to retrieve the module calibration with planeID and moduleID
///
///
TRestDataSetGainMap::Module* TRestDataSetGainMap::GetModule(const int planeID, const int moduleID) {
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
double TRestDataSetGainMap::GetSlopeParameter(const int planeID, const int moduleID, const double x,
                                              const double y) {
    Module* moduleCal = GetModule(planeID, moduleID);
    if (moduleCal == nullptr) return 0;  // return numeric_limits<double>::quiet_NaN()
    return moduleCal->GetSlope(x, y);
}

/////////////////////////////////////////////
/// \brief Function to get the intercept parameter of the module with
/// planeID and moduleID at physical position (x,y)
///
///
double TRestDataSetGainMap::GetInterceptParameter(const int planeID, const int moduleID, const double x,
                                                  const double y) {
    Module* moduleCal = GetModule(planeID, moduleID);
    if (moduleCal == nullptr) return 0;  // return numeric_limits<double>::quiet_NaN()
    return moduleCal->GetIntercept(x, y);
}

/////////////////////////////////////////////
/// \brief Function to get a list (set) of the plane IDs
///
///
std::set<int> TRestDataSetGainMap::GetPlaneIDs() const {
    std::set<int> planeIDs;
    for (const auto& mc : fModulesCal) planeIDs.insert(mc.GetPlaneId());
    return planeIDs;
}

/////////////////////////////////////////////
/// \brief Function to get a list (set) of the module IDs
/// of the plane with planeID
///
std::set<int> TRestDataSetGainMap::GetModuleIDs(const int planeId) const {
    std::set<int> moduleIDs;
    for (const auto& mc : fModulesCal)
        if (mc.GetPlaneId() == planeId) moduleIDs.insert(mc.GetModuleId());
    return moduleIDs;
}

/////////////////////////////////////////////
/// \brief Function to get the map of the module IDs for each plane ID
///
std::map<int, std::set<int>> TRestDataSetGainMap::GetModuleIDs() const {
    std::map<int, std::set<int>> moduleIds;
    for (const int planeId : GetPlaneIDs())
        moduleIds.insert(std::pair<int, std::set<int>>(planeId, GetModuleIDs(planeId)));
    return moduleIds;
}

TRestDataSetGainMap& TRestDataSetGainMap::operator=(TRestDataSetGainMap& src) {
    SetName(src.GetName());
    fOutputFileName = src.GetOutputFileName();
    fObservable = src.GetObservable();
    fSpatialObservableX = src.GetSpatialObservableX();
    fSpatialObservableY = src.GetSpatialObservableY();
    fCut = src.GetCut();
    fModulesCal.clear();
    for (auto pID : src.GetPlaneIDs())
        for (auto mID : src.GetModuleIDs(pID)) fModulesCal.push_back(*src.GetModule(pID, mID));
    return *this;
}

/////////////////////////////////////////////
/// \brief Function to set a module calibration. If the module calibration
/// already exists (same planeId and moduleId), it will be replaced.
///
void TRestDataSetGainMap::SetModuleCalibration(const Module& moduleCal) {
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
void TRestDataSetGainMap::Import(const std::string& fileName) {
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

        TRestDataSetGainMap* cal = nullptr;
        if (f != nullptr) {
            TIter nextkey(f->GetListOfKeys());
            TKey* key;
            while ((key = (TKey*)nextkey())) {
                std::string kName = key->GetClassName();
                if (REST_Reflection::GetClassQuick(kName.c_str()) != nullptr &&
                    REST_Reflection::GetClassQuick(kName.c_str())->InheritsFrom("TRestDataSetGainMap")) {
                    cal = f->Get<TRestDataSetGainMap>(key->GetName());
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
/// \param fileName The name of the output file. If empty, the output file
/// will be the fOutputFileName class member.
///
void TRestDataSetGainMap::Export(const std::string& fileName) {
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
void TRestDataSetGainMap::PrintMetadata() {
    TRestMetadata::PrintMetadata();
    RESTMetadata << " Calibration dataset: " << fCalibFileName << RESTendl;
    if (fCut) fCut->Print();
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
std::pair<int, int> TRestDataSetGainMap::Module::GetIndexMatrix(const double x, const double y) const {
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
double TRestDataSetGainMap::Module::GetSlope(const double x, const double y) const {
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
double TRestDataSetGainMap::Module::GetIntercept(const double x, const double y) const {
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
void TRestDataSetGainMap::Module::SetSplits() {
    SetSplitX();
    SetSplitY();
}
/////////////////////////////////////////////
/// \brief Function to set the class members for segmentation of
/// the detector plane along the X axis.
///
/// It uses the number of segments and the readout range to define the
/// edges of the segments.
void TRestDataSetGainMap::Module::SetSplitX() {
    if (fNumberOfSegmentsX < 1) {
        RESTError << "SetSplitX: fNumberOfSegmentsX must be >=1." << p->RESTendl;
        return;
    }
    std::set<double> split;
    for (int i = 0; i <= fNumberOfSegmentsX; i++) {  // <= so that the last segment is included
        double x =
            fReadoutRange.X() + ((fReadoutRange.Y() - fReadoutRange.X()) / (float)fNumberOfSegmentsX) * i;
        split.insert(x);
    }
    SetSplitX(split);
}

void TRestDataSetGainMap::Module::SetSplitX(const std::set<double>& splitX) {
    if (splitX.size() < 2) {
        RESTError << "SetSplitX: split size must be >=2 (start and end of range must be included)."
                  << p->RESTendl;
        return;
    }
    if (!fSlope.empty())
        RESTWarning << "SetSplitX: changing split but current gain map and calibration paremeters correspond "
                       "to previous splitting. Use GenerateGainMap() to update them."
                    << p->RESTendl;
    fSplitX = splitX;
    fNumberOfSegmentsX = fSplitX.size() - 1;
}
/////////////////////////////////////////////
/// \brief Function to set the class members for segmentation of
/// the detector plane along the Y axis.
///
/// It uses the number of segments and the readout range to define the
/// edges of the segments.
void TRestDataSetGainMap::Module::SetSplitY() {
    if (fNumberOfSegmentsY < 1) {
        RESTError << "SetSplitY: fNumberOfSegmentsY must be >=1." << p->RESTendl;
        return;
    }
    std::set<double> split;
    for (int i = 0; i <= fNumberOfSegmentsY; i++) {  // <= so that the last segment is included
        double y =
            fReadoutRange.X() + ((fReadoutRange.Y() - fReadoutRange.X()) / (float)fNumberOfSegmentsY) * i;
        split.insert(y);
    }
    SetSplitY(split);
}

void TRestDataSetGainMap::Module::SetSplitY(const std::set<double>& splitY) {
    if (splitY.size() < 2) {
        RESTError << "SetSplitY: split size must be >=2 (start and end of range must be included)."
                  << p->RESTendl;
        return;
    }
    if (!fSlope.empty())
        RESTWarning << "SetSplitY: changing split but current gain map and calibration paremeters correspond "
                       "to previous splitting. Use GenerateGainMap() to update them."
                    << p->RESTendl;
    fSplitY = splitY;
    fNumberOfSegmentsY = fSplitY.size() - 1;
}

/////////////////////////////////////////////
/// \brief Function that calculates the calibration parameters for each segment
/// defined at fSplitX and fSplitY ang generate their spectra and gain map.
///
/// It uses the data of the observable fObservable from the TRestDataSet
/// at fDataSetFileName (or fCalibFileName if first is empty).
/// The segmentation is given by the splits.
///
/// Ranges for peak fitting follows this logic:
/// 1. If fRangePeaks is defined and fAutoRangePeaks is false: fRangePeaks is used.
/// 2. If fRangePeaks is defined and fAutoRangePeaks is true: the fitting range
///     is calculated by the peak position found by TSpectrum inside fRangePeaks.
/// 3. If fRangePeaks is not defined and fAutoRangePeaks is false: use the peak position found by TSpectrum
///    and the peak position of the next peak to define the range.
/// 4. If fRangePeaks is not defined and fAutoRangePeaks is true: same as 3.
///
void TRestDataSetGainMap::Module::GenerateGainMap() {
    //--- Initial checks and settings ---
    std::string dsFileName = fDataSetFileName;
    if (dsFileName.empty()) dsFileName = p->GetCalibrationFileName();
    if (dsFileName.empty()) {
        RESTError << "No calibration file defined" << p->RESTendl;
        return;
    }

    if (!TRestTools::fileExists(dsFileName)) {
        RESTError << "Calibration file " << dsFileName << " does not exist." << p->RESTendl;
        return;
    }
    if (!TRestTools::isDataSet(dsFileName)) RESTWarning << dsFileName << " is not a dataset." << p->RESTendl;
    TRestDataSet dataSet;
    dataSet.EnableMultiThreading(true);
    dataSet.Import(dsFileName);
    fDataSetFileName = dsFileName;

    dataSet.SetDataFrame(dataSet.MakeCut(p->GetCut()));

    SetSplits();

    if (fSplitX.empty()) SetSplitX();
    if (fSplitY.empty()) SetSplitY();

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
            std::string name = "hSpc_" + std::to_string(fPlaneId) + "_" + std::to_string(fModuleId) + "_" +
                               std::to_string(i) + "_" + std::to_string(j);
            h[i][j] = new TH1F(name.c_str(), "", fNBins, fCalibRange.X(),
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
/// \param position position along X and Y axes at the detector module (in physical units).
/// \param energyPeak The energy of the peak to be fitted (in physical units).
/// \param range The range for the fitting of the peak (in the observables corresponding units).
///
void TRestDataSetGainMap::Module::Refit(const TVector2& position, const double energyPeak,
                                        const TVector2& range) {
    auto [index_x, index_y] = GetIndexMatrix(position.X(), position.Y());
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
    Refit((size_t)index_x, (size_t)index_y, (size_t)peakNumber, range);
}

/////////////////////////////////////////////
/// \brief Function to fit again manually a peak for a given segment of the module. The
/// calibration curve is updated after the fit.
///
/// \param x index along X-axis of the corresponding segment.
/// \param y index along Y-axis of the corresponding segment.
/// \param peakNumber The index of the peak to be fitted.
/// \param range The range for the fitting of the peak (in the observables corresponding units).
///
void TRestDataSetGainMap::Module::Refit(const size_t x, const size_t y, const size_t peakNumber,
                                        const TVector2& range) {
    if (fSegSpectra.empty()) {
        RESTError << "No gain map found. Use GenerateGainMap() first." << p->RESTendl;
        return;
    }
    if (x >= fSegSpectra.size() || y >= fSegSpectra.at(0).size()) {
        RESTError << "Segment with index (" << x << ", " << y << ") not found" << p->RESTendl;
        return;
    }
    if (peakNumber >= fEnergyPeaks.size()) {
        RESTError << "Peak with index " << peakNumber << " not found" << p->RESTendl;
        return;
    }

    // Refit the desired peak
    std::string name = "g" + std::to_string(peakNumber);
    TF1* g = new TF1(name.c_str(), "gaus", range.X(), range.Y());
    TH1F* h = fSegSpectra.at(x).at(y);
    while (h->GetFunction(name.c_str()))  // clear previous fits for this peakNumber
        h->GetListOfFunctions()->Remove(h->GetFunction(name.c_str()));
    h->Fit(g, "R+Q0");  // use 0 to not draw the fit but save it

    // Change the point of the graph
    UpdateCalibrationFits(x, y);
}

/////////////////////////////////////////////
/// \brief Function to update the calibration curve for a given segment of the module. The calibration
/// curve is cleared and then the means of the gaussian fits for each energy peak are added. If there are
/// less than 2 fits, zero points are added. Then, the calibration curve is refitted (linearFit).
///
/// \param x index along X-axis of the corresponding segment.
/// \param y index along Y-axis of the corresponding segment.
///
void TRestDataSetGainMap::Module::UpdateCalibrationFits(const size_t x, const size_t y) {
    if (fSegSpectra.empty()) {
        RESTError << "No gain map found. Use GenerateGainMap() first." << p->RESTendl;
        return;
    }
    if (x >= fSegSpectra.size() || y >= fSegSpectra.at(0).size()) {
        RESTError << "Segment with index (" << x << ", " << y << ") not found" << p->RESTendl;
        return;
    }

    TH1F* h = fSegSpectra.at(x).at(y);
    TGraph* gr = fSegLinearFit.at(x).at(y);

    // Clear the points of the graph
    for (size_t i = 0; i < fEnergyPeaks.size(); i++) gr->RemovePoint(i);
    // Add the new points to the graph
    int c = 0;
    for (size_t i = 0; i < fEnergyPeaks.size(); i++) {
        std::string fitName = (std::string) "g" + std::to_string(i);
        TF1* g = h->GetFunction(fitName.c_str());
        if (!g) {
            RESTWarning << "No fit ( " << fitName << " ) found for energy peak " << fEnergyPeaks[i]
                        << " in segment " << x << "," << y << p->RESTendl;
            continue;
        }
        gr->SetPoint(c++, g->GetParameter(1), fEnergyPeaks[i]);
    }

    // Add zero points if needed (if there are less than 2 points)
    while (gr->GetN() < 2) {
        gr->SetPoint(c++, 0, 0);
        RESTWarning << "Not enough points for linear fit at segment (" << x << ", " << y
                    << "). Adding zero point." << p->RESTendl;
    }

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
void TRestDataSetGainMap::Module::LoadConfigFromTiXmlElement(const TiXmlElement* module) {
    if (module == nullptr) {
        RESTError << "TRestDataSetGainMap::Module::LoadConfigFromTiXmlElement: module is nullptr"
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

void TRestDataSetGainMap::Module::DrawSpectrum(const TVector2& position, bool drawFits, int color,
                                               TCanvas* c) {
    std::pair<size_t, size_t> index = GetIndexMatrix(position.X(), position.Y());
    DrawSpectrum(index.first, index.second, drawFits, color, c);
}

void TRestDataSetGainMap::Module::DrawSpectrum(const int index_x, const int index_y, bool drawFits, int color,
                                               TCanvas* c) {
    if (fSegSpectra.size() == 0) {
        RESTError << "Spectra matrix is empty." << p->RESTendl;
        return;
    }
    if (index_x < 0 || index_y < 0 || index_x >= (int)fSegSpectra.size() ||
        index_y >= (int)fSegSpectra.at(index_x).size()) {
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

    if (color > 0) fSegSpectra[index_x][index_y]->SetLineColor(color);
    size_t colorT = fSegSpectra[index_x][index_y]->GetLineColor();
    fSegSpectra[index_x][index_y]->Draw("same");

    if (drawFits)
        for (size_t c = 0; c < fEnergyPeaks.size(); c++) {
            auto fit = fSegSpectra[index_x][index_y]->GetFunction(("g" + std::to_string(c)).c_str());
            if (!fit) RESTWarning << "Fit for energy peak" << fEnergyPeaks[c] << " not found." << p->RESTendl;
            if (!fit) continue;
            fit->SetLineColor(c + 2 != colorT ? c + 2 : ++colorT); /* does not work with kRed, kBlue, etc.
                  as they are not defined with the same number as the first 10 basic colors. See
                  https://root.cern.ch/doc/master/classTColor.html#C01 and
                  https://root.cern.ch/doc/master/classTColor.html#C02 */
            fit->Draw("same");
        }
}

/////////////////////////////////////////////
/// \brief Function to draw the spectrum for each segment of the module
/// on the same canvas. The canvas is divided in fNumberOfSegmentsX x fNumberOfSegmentsY
/// pads. The segments are drawn with the bottom left corner corresponding to the
/// minimun x and y values of the readout range and top right corner corresponding
/// to the maximum x and y values of the readout range.
/// Tip: define a canvas and use this same canvas along different calls to this function
/// to draw the spectra of different modules on the same canvas.
/// Example:
/// TCanvas *myCanvas = new TCanvas();
/// module1->DrawSpectrum(false, kBlue, myCanvas);
/// module2->DrawSpectrum(false, kRed, myCanvas);
///
/// \param drawFits A bool to also draw the fits or not.
/// \param color An int to set the color of the spectra. If negative,
/// the color of the spectra is not changed.
/// \param c A TCanvas pointer to draw the spectra. If none (nullptr) is given,
/// a new one is created.
///
void TRestDataSetGainMap::Module::DrawSpectrum(const bool drawFits, const int color, TCanvas* c) {
    if (fSegSpectra.size() == 0) {
        RESTError << "Spectra matrix is empty." << p->RESTendl;
        return;
    }
    if (!c) {
        std::string t = "spectrum_" + std::to_string(fPlaneId) + "_" + std::to_string(fModuleId);
        c = new TCanvas(t.c_str(), t.c_str());
    }

    size_t nPads = 0;
    for (const auto& object : *c->GetListOfPrimitives())
        if (object->InheritsFrom(TVirtualPad::Class())) ++nPads;
    if (nPads != 0 && nPads != fSegSpectra.size() * fSegSpectra.at(0).size()) {
        RESTError << "Canvas " << c->GetName() << " has " << nPads << " pads, but "
                  << fSegSpectra.size() * fSegSpectra.at(0).size() << " are needed." << p->RESTendl;
        return;
    } else if (nPads == 0)
        c->Divide(fSegSpectra.size(), fSegSpectra.at(0).size());

    for (size_t i = 0; i < fSegSpectra.size(); i++) {
        for (size_t j = 0; j < fSegSpectra[i].size(); j++) {
            int pad = fSegSpectra.size() * (fSegSpectra[i].size() - 1) + 1 + i - fSegSpectra.size() * j;
            c->cd(pad);
            DrawSpectrum(i, j, drawFits, color, c);
        }
    }
}
void TRestDataSetGainMap::Module::DrawFullSpectrum() {
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

void TRestDataSetGainMap::Module::DrawLinearFit(const TVector2& position, TCanvas* c) {
    std::pair<size_t, size_t> index = GetIndexMatrix(position.X(), position.Y());
    DrawLinearFit(index.first, index.second, c);
}

void TRestDataSetGainMap::Module::DrawLinearFit(const int index_x, const int index_y, TCanvas* c) {
    if (fSegLinearFit.size() == 0) {
        RESTError << "Spectra matrix is empty." << p->RESTendl;
        return;
    }
    if (index_x < 0 || index_y < 0 || index_x >= (int)fSegLinearFit.size() ||
        index_y >= (int)fSegLinearFit.at(index_x).size()) {
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

void TRestDataSetGainMap::Module::DrawLinearFit(TCanvas* c) {
    if (fSegLinearFit.size() == 0) {
        RESTError << "Spectra matrix is empty." << p->RESTendl;
        return;
    }
    if (!c) {
        std::string t = "linearFits_" + std::to_string(fPlaneId) + "_" + std::to_string(fModuleId);
        c = new TCanvas(t.c_str(), t.c_str());
    }

    size_t nPads = 0;
    for (const auto& object : *c->GetListOfPrimitives())
        if (object->InheritsFrom(TVirtualPad::Class())) ++nPads;
    if (nPads != 0 && nPads != fSegLinearFit.size() * fSegLinearFit.at(0).size()) {
        RESTError << "Canvas " << c->GetName() << " has " << nPads << " pads, but "
                  << fSegLinearFit.size() * fSegLinearFit.at(0).size() << " are needed." << p->RESTendl;
        return;
    } else if (nPads == 0)
        c->Divide(fSegLinearFit.size(), fSegLinearFit.at(0).size());

    for (size_t i = 0; i < fSegLinearFit.size(); i++) {
        for (size_t j = 0; j < fSegLinearFit[i].size(); j++) {
            int pad = fSegLinearFit.size() * (fSegLinearFit[i].size() - 1) + 1 + i - fSegLinearFit.size() * j;
            c->cd(pad);
            DrawLinearFit(i, j, c);
        }
    }
}

void TRestDataSetGainMap::Module::DrawGainMap(const int peakNumber) {
    if (peakNumber < 0 || peakNumber >= (int)fEnergyPeaks.size()) {
        RESTError << "Peak number out of range (peakNumber should be between 0 and "
                  << fEnergyPeaks.size() - 1 << " )" << p->RESTendl;
        return;
    }
    if (fSegLinearFit.size() == 0) {
        RESTError << "Linear fit matrix is empty." << p->RESTendl;
        return;
    }

    double peakEnergy = fEnergyPeaks[peakNumber];
    std::string title = "Gain map for energy " + DoubleToString(peakEnergy, "%g") + ";" +
                        GetSpatialObservableX() + ";" + GetSpatialObservableY();  // + " keV";
    std::string t = "gainMap" + std::to_string(peakNumber) + "_" + std::to_string(fPlaneId) + "_" +
                    std::to_string(fModuleId);
    TCanvas* gainMap = new TCanvas(t.c_str(), t.c_str());
    gainMap->cd();
    TH2F* hGainMap = new TH2F(("h" + t).c_str(), title.c_str(), fNumberOfSegmentsX, fReadoutRange.X(),
                              fReadoutRange.Y(), fNumberOfSegmentsY, fReadoutRange.X(), fReadoutRange.Y());

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
            float xMean = (xUpper + xLower) / 2.;
            float yMean = (yUpper + yLower) / 2.;
            auto [index_x, index_y] = GetIndexMatrix(xMean, yMean);
            hGainMap->Fill(xMean, yMean, fSegLinearFit[index_x][index_y]->GetPointX(peakNumber) / peakPosRef);
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
void TRestDataSetGainMap::Module::Print() const {
    RESTMetadata << "-----------------------------------------------" << p->RESTendl;
    RESTMetadata << " Plane ID: " << fPlaneId << p->RESTendl;
    RESTMetadata << " Module ID: " << fModuleId << p->RESTendl;
    RESTMetadata << " Definition cut: " << fDefinitionCut << p->RESTendl;
    RESTMetadata << p->RESTendl;

    RESTMetadata << " Calibration dataset: " << fDataSetFileName << p->RESTendl;
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
