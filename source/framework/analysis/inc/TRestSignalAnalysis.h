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

#include <Rtypes.h>
#include <TMath.h>

#include <iostream>
#include <vector>

#ifndef RestCore_TRestSignalAnalysis
#define RestCore_TRestSignalAnalysis

#include <TVector2.h>

#include <numeric>

/// This namespace define utilities (functions) to calculate different signal parameters
namespace TRestSignalAnalysis {

/// Generic functions for different calculations

template <typename T>
void CalculateBaselineAndSigmaSD(const std::vector<T>& signal, Int_t startBin, Int_t endBin,
                                 Double_t& baseLine, Double_t& baseLineSigma);

template <typename T>
void CalculateBaselineAndSigmaIQR(const std::vector<T>& signal, Int_t startBin, Int_t endBin,
                                  Double_t& baseLine, Double_t& baseLineSigma);

template <typename T>
Double_t GetAverage(const std::vector<T>& signal, Int_t startBin, Int_t endBin);

///////////////////////////////////////////////
/// \brief Return smoothing of signal
///
///
/// \param neighbours It defines the number of neighbours
/// points used to average the signal
///
/// \param option If the option is set to "EXCLUDE OUTLIERS", points that are too far away from the median
/// baseline will be ignored to improve the smoothing result
///
template <typename T>
std::vector<Float_t> GetSignalSmoothed(const std::vector<T>& signal, int averagingPoints = 3);

template <typename T>
std::vector<Float_t> GetSignalSmoothed_ExcludeOutliers(const std::vector<T>& signal, int averagingPoints,
                                                       Double_t& baseLine, Double_t& baseLineSigma);

template <typename T>
std::vector<Int_t> GetPointsOverThreshold(const std::vector<T>& signal, TVector2& range,
                                          const TVector2& thrPar, Int_t nPointsOver, Int_t nPointsFlat,
                                          Double_t baseLine, Double_t baseLineSigma);

template <typename T>
Int_t GetMaxBin(const std::vector<T>& signal) {
    return std::distance(signal.begin(), std::max_element(signal.begin(), signal.end()));
}

template <typename T>
Int_t GetMinBin(const std::vector<T>& signal) {
    return std::distance(signal.begin(), std::min_element(signal.begin(), signal.end()));
}

}  // namespace TRestSignalAnalysis

#endif
