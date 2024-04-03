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

#ifndef REST_TRestDataSetGainMap
#define REST_TRestDataSetGainMap

#include <TCanvas.h>
#include <TFile.h>
#include <TGraph.h>
#include <TH1.h>
#include <TH2.h>
#include <TRestStringOutput.h>
#include <TSpectrum.h>
#include <TTree.h>
#include <TVector2.h>

#include "TRestCut.h"
#include "TRestDataSet.h"
#include "TRestMetadata.h"

/// Metadata class to calculate,store and apply the gain corrected calibration of a group of detectors.
class TRestDataSetGainMap : public TRestMetadata {
   public:
    class Module;

   private:
    /// Observable that will be used to calculate the gain map
    std::string fObservable = "";  //<

    /// Observable that will be used to segmentize the gain map in the x direction
    std::string fSpatialObservableX = "";  //<

    /// Observable that will be used to segmentize the gain map in the y direction
    std::string fSpatialObservableY = "";  //<

    /// List of modules
    std::vector<Module> fModulesCal = {};  //<

    /// Name of the file that contains the calibration data
    std::string fCalibFileName = "";  //<

    /// Name of the file where the gain map was (or will be) exported
    std::string fOutputFileName = "";  //<

    /// Cut to be applied to the calibration data
    TRestCut* fCut = nullptr;  //<

    void Initialize() override;
    void InitFromConfigFile() override;

   public:
    std::set<int> GetPlaneIDs() const;
    std::set<int> GetModuleIDs(const int planeId) const;
    std::map<int, std::set<int>> GetModuleIDs() const;
    Int_t GetNumberOfPlanes() const { return GetPlaneIDs().size(); }
    Int_t GetNumberOfModules() const {
        int sum = 0;
        for (auto pID : GetPlaneIDs()) sum += GetModuleIDs(pID).size();
        return sum;
    }

    std::string GetCalibrationFileName() const { return fCalibFileName; }
    std::string GetOutputFileName() const { return fOutputFileName; }
    std::string GetObservable() const { return fObservable; }
    std::string GetSpatialObservableX() const { return fSpatialObservableX; }
    std::string GetSpatialObservableY() const { return fSpatialObservableY; }
    TRestCut* GetCut() const { return fCut; }

    Module* GetModule(const size_t index = 0);
    Module* GetModule(const int planeID, const int moduleID);
    double GetSlopeParameter(const int planeID, const int moduleID, const double x, const double y);
    double GetInterceptParameter(const int planeID, const int moduleID, const double x, const double y);
    double GetSlopeParameterFullSpc(const int planeID, const int moduleID);
    double GetInterceptParameterFullSpc(const int planeID, const int moduleID);

    void SetCalibrationFileName(const std::string& fileName) { fCalibFileName = fileName; }
    void SetOutputFileName(const std::string& fileName) { fOutputFileName = fileName; }
    void SetModule(const Module& moduleCal);
    void SetObservable(const std::string& observable) { fObservable = observable; }
    void SetSpatialObservableX(const std::string& spatialObservableX) {
        fSpatialObservableX = spatialObservableX;
    }
    void SetSpatialObservableY(const std::string& spatialObservableY) {
        fSpatialObservableY = spatialObservableY;
    }
    void SetCut(TRestCut* cut) { fCut = cut; }

    void Import(const std::string& fileName);
    void Export(const std::string& fileName = "");

    TRestDataSetGainMap& operator=(TRestDataSetGainMap& src);

    void PrintMetadata() override;

    void GenerateGainMap();
    void CalibrateDataSet(const std::string& dataSetFileName, std::string outputFileName = "",
                          std::vector<std::string> excludeColumns = {});

    TRestDataSetGainMap();
    TRestDataSetGainMap(const char* configFilename, std::string name = "");
    ~TRestDataSetGainMap();

    ClassDefOverride(TRestDataSetGainMap, 2);

    class Module {
       private:
        /// Pointer to the parent class
        const TRestDataSetGainMap* p = nullptr;  //<!

        std::pair<double, double> FitPeaks(TH1F* hSeg, TGraph* gr);
        std::pair<double, double> UpdateCalibrationFits(TH1F* hSeg, TGraph* gr);

       public:
        /// Plane ID (unique identifier). Although it is not linked to any TRestDetectorReadout it is
        /// recommended to use the same.
        Int_t fPlaneId = -1;  //<

        // Module ID (unique identifier). Although it is not linked to any TRestDetectorReadout it is
        // recommended to use the same.
        Int_t fModuleId = -1;  //<

        /// Energy of the peaks to be used for the calibration.
        std::vector<double> fEnergyPeaks = {};  //<

        /// Range of the peaks to be used for the calibration. If empty it will be automatically calculated.
        std::vector<TVector2> fRangePeaks = {};  //<

        /// Calibration range. If fCalibRange.X()>=fCalibRange.Y() the range will be automatically calculated.
        TVector2 fCalibRange = TVector2(0, 0);  //<

        /// Number of bins for the spectrum histograms.
        Int_t fNBins = 100;  //<

        /// Cut that defines which events are from this module.
        std::string fDefinitionCut = "";  //<

        /// Number of segments in the x direction.
        Int_t fNumberOfSegmentsX = 1;  //<

        /// Number of segments in the y direction.
        Int_t fNumberOfSegmentsY = 1;  //<

        /// Readout dimensions
        TVector2 fReadoutRange = TVector2(-1, 246.24);  //<

        /// Split points in the x direction.
        std::set<double> fSplitX = {};  //<

        /// Split points in the y direction.
        std::set<double> fSplitY = {};  //<

        /// Name of the file that contains the calibration data. If empty, it will use its parent
        /// TRestDataSetGainMap::fCalibFileName.
        std::string fDataSetFileName = "";  //<

        /// Array containing the slope of the linear fit for each segment.
        std::vector<std::vector<double>> fSlope = {};  //<

        /// Slope of the calibration linear fit of whole module
        double fFullSlope = 0;  //<

        /// Intercept of the calibration linear fit of whole module
        double fFullIntercept = 0;  //<

        /// Array containing the intercept of the linear fit for each segment.
        std::vector<std::vector<double>> fIntercept = {};  //<

        /// Add zero point to the calibration linear fit. Zero point will be automatically added if there are
        /// less than 2 points.
        bool fZeroPoint = false;  //<

        /// Automatic range for the peaks fitting. See GenerateGainMap() for more information of the logic.
        bool fAutoRangePeaks = true;  //<

        /// Array containing the observable spectrum for each segment.
        std::vector<std::vector<TH1F*>> fSegSpectra = {};  //<

        /// Array containing the calibration linear fit for each segment.
        std::vector<std::vector<TGraph*>> fSegLinearFit = {};  //<

        /// Spectrum of the observable for the whole module.
        TH1F* fFullSpectrum = nullptr;  //<

        /// Calibration linear fit for the whole module.
        TGraph* fFullLinearFit = nullptr;  //<

       public:
        void AddPeak(const double& energyPeak, const TVector2& rangePeak = TVector2(0, 0)) {
            fEnergyPeaks.push_back(energyPeak);
            fRangePeaks.push_back(rangePeak);
        }

        void LoadConfigFromTiXmlElement(const TiXmlElement* module);

        TRestDataSetGainMap* GetParent() const { return const_cast<TRestDataSetGainMap*>(p); }
        std::pair<int, int> GetIndexMatrix(const double x, const double y) const;
        double GetSlope(const double x, const double y) const;
        double GetIntercept(const double x, const double y) const;
        double GetSlopeFullSpc() const { return fFullSlope; };
        double GetInterceptFullSpc() const { return fFullIntercept; };

        Int_t GetPlaneId() const { return fPlaneId; }
        Int_t GetModuleId() const { return fModuleId; }
        std::string GetObservable() const { return p->fObservable; }
        std::string GetSpatialObservableX() const { return p->fSpatialObservableX; }
        std::string GetSpatialObservableY() const { return p->fSpatialObservableY; }
        inline std::string GetModuleDefinitionCut() const { return fDefinitionCut; }
        Int_t GetNumberOfSegmentsX() const { return fNumberOfSegmentsX; }
        Int_t GetNumberOfSegmentsY() const { return fNumberOfSegmentsY; }

        std::set<double> GetSplitX() const { return fSplitX; }
        std::set<double> GetSplitY() const { return fSplitY; }
        std::string GetDataSetFileName() const { return fDataSetFileName; }
        TVector2 GetReadoutRange() const { return fReadoutRange; }

        void DrawSpectrum(const bool drawFits = true, const int color = -1, TCanvas* c = nullptr);
        void DrawSpectrum(const TVector2& position, bool drawFits = true, int color = -1,
                          TCanvas* c = nullptr);
        void DrawSpectrum(const int index_x, const int index_y, bool drawFits = true, int color = -1,
                          TCanvas* c = nullptr);
        void DrawFullSpectrum(const bool drawFits = true, const int color = -1, TCanvas* c = nullptr);

        void DrawLinearFit(TCanvas* c = nullptr);
        void DrawLinearFit(const TVector2& position, TCanvas* c = nullptr);
        void DrawLinearFit(const int index_x, const int index_y, TCanvas* c = nullptr);

        void DrawGainMap(const int peakNumber = 0, const bool fullModuleAsRef = true);

        void Refit(const TVector2& position, const double energy, const TVector2& range);
        void Refit(const size_t x, const size_t y, const size_t peakNumber, const TVector2& range);
        void RefitFullSpc(const double energy, const TVector2& range);
        void RefitFullSpc(const size_t peakNumber, const TVector2& range);
        void UpdateCalibrationFits(const size_t x, const size_t y);
        void UpdateCalibrationFitsFullSpc();

        void SetPlaneId(const Int_t& planeId) { fPlaneId = planeId; }
        void SetModuleId(const Int_t& moduleId) { fModuleId = moduleId; }
        void SetModuleDefinitionCut(const std::string& moduleDefinitionCut) {
            fDefinitionCut = moduleDefinitionCut;
        }
        void SetCalibrationRange(const TVector2& calibrationRange) { fCalibRange = calibrationRange; }
        void SetNBins(const Int_t& nBins) { fNBins = nBins; }
        void SetSplitX();
        void SetSplitY();
        void SetSplitX(const std::set<double>& splitX);
        void SetSplitY(const std::set<double>& splitY);
        void SetSplits();
        void SetSplits(const std::set<double>& splitXandY) {
            SetSplitX(splitXandY);
            SetSplitY(splitXandY);
        }

        void SetNumberOfSegmentsX(const Int_t& numberOfSegmentsX) { fNumberOfSegmentsX = numberOfSegmentsX; }
        void SetNumberOfSegmentsY(const Int_t& numberOfSegmentsY) { fNumberOfSegmentsY = numberOfSegmentsY; }

        void SetDataSetFileName(const std::string& dataSetFileName) { fDataSetFileName = dataSetFileName; }
        void SetReadoutRange(const TVector2& readoutRange) { fReadoutRange = readoutRange; }
        void SetZeroPoint(const bool& ZeroPoint) { fZeroPoint = ZeroPoint; }
        void SetAutoRangePeaks(const bool& autoRangePeaks) { fAutoRangePeaks = autoRangePeaks; }

        void Print() const;

        void GenerateGainMap();
        void Initialize();

        Module() {}
        Module(const TRestDataSetGainMap& parent) : p(&parent){};
        Module(const TRestDataSetGainMap& parent, const Int_t planeId, const Int_t moduleId) : p(&parent) {
            SetPlaneId(planeId);
            SetModuleId(moduleId);
        };
        ~Module(){};
    };
};
#endif
