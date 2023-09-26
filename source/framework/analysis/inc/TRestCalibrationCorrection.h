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

#ifndef REST_TRestCalibrationCorrection
#define REST_TRestCalibrationCorrection

#include <TCanvas.h>
#include <TFile.h>
#include <TGraph.h>
#include <TH1.h>
#include <TH2.h>
#include <TRestStringOutput.h>
#include <TSpectrum.h>
#include <TTree.h>
#include <TVector2.h>

#include "TRestDataSet.h"
#include "TRestMetadata.h"

/// Metadata class to calculate,store and apply the gain corrected calibration of a group of detectors.
class TRestCalibrationCorrection : public TRestMetadata {
   public:
    class Module;

   private:
    std::string fObservable = "";          //"rawAna_ThresholdIntegral"; //<
    std::string fSpatialObservableX = "";  //"hitsAna_xMean"; //<
    std::string fSpatialObservableY = "";  //"hitsAna_yMean"; //<

    std::vector<Module> fModulesCal = {};
    std::string fCalibFileName = "";
    std::string fOutputFileName = "";

    void Initialize() override;
    void InitFromConfigFile() override;

   public:
    std::set<int> GetPlaneIDs() const;
    std::set<int> GetModuleIDs(const int planeId) const;
    std::map<int, std::set<int>> GetModuleIDs() const;
    Int_t GetNumberOfPlanes() const { return GetPlaneIDs().size(); }
    Int_t GetNumberOfModules() const {
        int sum = 0;
        for (auto pID : GetPlaneIDs())
            for (auto n : GetModuleIDs(pID)) sum += n;
        return sum;
    }
    // Int_t GetNumberOfModulesOfPlane(const int planeID) const { return fNModules.at(planeID);}
    std::string GetCalibrationFileName() const { return fCalibFileName; }
    std::string GetOutputFileName() const { return fOutputFileName; }
    std::string GetObservable() const { return fObservable; }
    std::string GetSpatialObservableX() const { return fSpatialObservableX; }
    std::string GetSpatialObservableY() const { return fSpatialObservableY; }

    Module* GetModuleCalibration(const int planeID, const int moduleID);
    double GetSlopeParameter(const int planeID, const int moduleID, const double x, const double y);
    double GetInterceptParameter(const int planeID, const int moduleID, const double x, const double y);

    void SetCalibrationFileName(const std::string& fileName) { fCalibFileName = fileName; }
    void SetOutputFileName(const std::string& fileName) { fOutputFileName = fileName; }
    void SetModuleCalibration(const Module& moduleCal);
    void SetObservable(const std::string& observable) { fObservable = observable; }
    void SetSpatialObservableX(const std::string& spatialObservableX) {
        fSpatialObservableX = spatialObservableX;
    }
    void SetSpatialObservableY(const std::string& spatialObservableY) {
        fSpatialObservableY = spatialObservableY;
    }

    void Import(const std::string& fileName);
    void Export(const std::string& fileName = "");

    TRestCalibrationCorrection& operator=(TRestCalibrationCorrection& src);

   public:
    void PrintMetadata() override;

    void Calibrate();
    void CalibrateDataSet(const std::string& dataSetFileName, std::string outputFileName = "");

    TRestCalibrationCorrection();
    TRestCalibrationCorrection(const char* configFilename, std::string name = "");
    ~TRestCalibrationCorrection();

    // REMOVE COMMENT. ROOT class definition helper. Increase the number in it every time
    // you add/rename/remove the metadata members
    ClassDefOverride(TRestCalibrationCorrection, 1);

    class Module {
       private:
        const TRestCalibrationCorrection* p = nullptr;  //<! Pointer to the parent class
       public:                                          /// Members that will be written to the ROOT file.
        Int_t fPlaneId = -1;                            //< // Plane ID
        Int_t fModuleId = -1;                           //< // Module ID

        std::vector<double> fEnergyPeaks = {};  //{22.5, 8.0};
        std::vector<TVector2> fRangePeaks =
            {};  //{TVector2(230000, 650000), TVector2(40000, 230000)}; //in development...
        TVector2 fCalibRange = TVector2(0, 0);  //< // Calibration range
        Int_t fNBins = 100;                     //< // Number of bins for the spectrum histograms

        /*std::string fObservable = ""; //"rawAna_ThresholdIntegral"; //<
        std::string fSpatialObservableX = ""; //"hitsAna_xMean"; //<
        std::string fSpatialObservableY = ""; //"hitsAna_yMean"; //<*/
        std::string fDefinitionCut = "";  //"TREXsides_tagId == 2"; //<

        Int_t fNumberOfSegmentsX = 1;               //<
        Int_t fNumberOfSegmentsY = 1;               //<
        TVector2 fReadoutRange = TVector2(0, 246);  //< // Readout dimensions
        std::set<double> fSplitX = {};              //<
        std::set<double> fSplitY = {};              //<

        std::string fDataSetFileName = "";  //< // File name for the dataset

        std::vector<std::vector<double>> fSlope = {};      //<
        std::vector<std::vector<double>> fIntercept = {};  //<

        bool fZeroPoint = false;  //< Zero point will be automatically added if there are less than 2 peaks
        bool fAutoRangePeaks = true;                           //< Automatic range peaks
        std::vector<std::vector<TH1F*>> fSegSpectra = {};      // fSegmentedSpectra
        std::vector<std::vector<TGraph*>> fSegLinearFit = {};  // fSegmentedLinearFit

       public:
        void SetSplitX();
        void SetSplitY();
        void SetSplits();

        void AddPeak(const double& energyPeak, const TVector2& rangePeak = TVector2(0, 0)) {
            fEnergyPeaks.push_back(energyPeak);
            fRangePeaks.push_back(rangePeak);
        }

        void LoadConfigFromTiXmlElement(const TiXmlElement* module);

        std::pair<int, int> GetIndexMatrix(const double x, const double y) const;
        double GetSlope(const double x, const double y) const;
        double GetIntercept(const double x, const double y) const;

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
        TVector2 GetReadoutRangeVar() const { return fReadoutRange; }

        void DrawSpectrum();
        void DrawSpectrum(const double x, const double y, TCanvas* c = nullptr);
        void DrawSpectrum(const size_t index_x, const size_t index_y, TCanvas* c = nullptr);
        void DrawFullSpectrum();

        void DrawLinearFit();
        void DrawLinearFit(const double x, const double y, TCanvas* c = nullptr);
        void DrawLinearFit(const size_t index_x, const size_t index_y, TCanvas* c = nullptr);

        void DrawGainMap(const int peakNumber = 0);

        void Refit(const double x, const double y, const double energy, const TVector2& range);
        void Refit(const int x, const int y, const int peakNumber, const TVector2& range);

        void SetPlaneId(const Int_t& planeId) { fPlaneId = planeId; }
        void SetModuleId(const Int_t& moduleId) { fModuleId = moduleId; }
        void SetModuleDefinitionCut(const std::string& moduleDefinitionCut) {
            fDefinitionCut = moduleDefinitionCut;
        }
        void SetCalibrationRange(const TVector2& calibrationRange) { fCalibRange = calibrationRange; }
        void SetNBins(const Int_t& nBins) { fNBins = nBins; }

        void SetNumberOfSegmentsX(const Int_t& numberOfSegmentsX) {
            fNumberOfSegmentsX = numberOfSegmentsX;
            SetSplitX();
        }
        void SetNumberOfSegmentsY(const Int_t& numberOfSegmentsY) {
            fNumberOfSegmentsY = numberOfSegmentsY;
            SetSplitY();
        }
        // void SetInputFile( const std::string &inputFile) { fInputFile = inputFile;}
        // void SetOutputFile( const std::string &outputFile) { fOutputFile = outputFile;}
        void SetDataSetFileName(const std::string& dataSetFileName) { fDataSetFileName = dataSetFileName; }
        void SetReadoutRange(const TVector2& readoutRange) { fReadoutRange = readoutRange; }
        void SetZeroPoint(const bool& ZeroPoint) { fZeroPoint = ZeroPoint; }
        void SetAutoRangePeaks(const bool& autoRangePeaks) { fAutoRangePeaks = autoRangePeaks; }

        void Print();

        void CalculateCalibrationParameters();
        void Initialize();

        Module() {}
        Module(const TRestCalibrationCorrection& parent) : p(&parent){};
        Module(const TRestCalibrationCorrection& parent, const Int_t planeId, const Int_t moduleId)
            : p(&parent) {
            SetPlaneId(planeId);
            SetModuleId(moduleId);
        };
        ~Module(){};
    };
};
#endif
