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

#ifndef REST_TRestDataSetCalibration
#define REST_TRestDataSetCalibration

#include "TRestMetadata.h"

/// This class is meant to perform the calibration of different runs
class TRestDataSetCalibration : public TRestMetadata {
   private:

    /// Name of the output file
    std::string fOutputFileName = "";

    /// Name of the dataSet inside the config file
    std::string fDataSetName = "";

    /// Vector containing expected energy peaks in keV must be sorted
    std::vector<double> fEnergyPeaks;

    /// Vector containing calibrated peaks in ADCs
    std::vector<double> fCalibPeaks;

    /// Vector containing calibrated sigma in ADCs
    std::vector<double> fCalibFWHM;

    /// Name of the calibration file to be used
    std::string fCalibFile = "";

    /// Calibration variable to be used
    std::string fCalObservable = "";

    /// Range to be calibrated
    TVector2 fCalibRange;

    /// Number of bins used in the calibration
    Int_t fNBins;

    /// Slope from the calibration fit
    Double_t fSlope = 0;

    /// Intercept of the calibration fit
    Double_t fIntercept = 0;

    void Initialize() override;
    void InitFromConfigFile() override;

   public:
    void PrintMetadata() override;

    void Calibrate();

    inline void SetDataSetName(const std::string& dSName) { fDataSetName = dSName; }
    inline void SetOutputFileName(const std::string& outName) { fOutputFileName = outName; }
    inline void SetCalibrationFile(const std::string& calibFile) { fCalibFile = calibFile; }

    inline auto GetCalibPeaks() const { return fCalibPeaks; }
    inline auto GetCalibFWHM() const { return fCalibFWHM; }

    inline double GetSlope() const { return fSlope; }
    inline double GetIntercept() const { return fIntercept; }
    inline std::string GetCalObservable() const { return fCalObservable; }

    TRestDataSetCalibration();
    TRestDataSetCalibration(const char* configFilename, std::string name = "");
    ~TRestDataSetCalibration();

    ClassDefOverride(TRestDataSetCalibration, 1);
};
#endif
