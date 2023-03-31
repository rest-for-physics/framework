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

#ifndef RestCore_TRestSignalAnalysis
#define RestCore_TRestSignalAnalysis

#include <Rtypes.h>
#include <TGraph.h>
#include <TMath.h>
#include <TVector2.h>

#include <iostream>
#include <numeric>
#include <vector>

/// This namespace define generic functions to calculate different signal parameters
namespace TRestSignalAnalysis {

template <typename T>
void CalculateBaselineAndSigmaSD(const std::vector<T>& signal, Int_t startBin, Int_t endBin,
                                 Double_t& baseLine, Double_t& baseLineSigma);

template <typename T>
void CalculateBaselineAndSigmaIQR(const std::vector<T>& signal, Int_t startBin, Int_t endBin,
                                  Double_t& baseLine, Double_t& baseLineSigma);

template <typename T>
Double_t GetAverage(const std::vector<T>& signal, Int_t startBin, Int_t endBin);

template <typename T>
std::vector<Float_t> GetSignalSmoothed(const std::vector<T>& signal, int averagingPoints = 3);

template <typename T>
std::vector<Float_t> GetSignalSmoothed_ExcludeOutliers(const std::vector<T>& signal, int averagingPoints,
                                                       Double_t& baseLine, Double_t& baseLineSigma);

template <typename T>
std::vector<Float_t> GetDerivative(const std::vector<T>& signal);

template <typename T>
std::vector<std::pair<Float_t, Float_t> > GetPointsOverThreshold(const std::vector<T>& signal,
                                                                 TVector2& range, const TVector2& thrPar,
                                                                 Int_t nPointsOver, Int_t nPointsFlat,
                                                                 Double_t baseLineSigma);

template <typename T>
Int_t GetMaxBin(const std::vector<T>& signal, int startBin = 0, int endBin = 0) {
    if (endBin <= 0 || endBin > (int)signal.size()) endBin = signal.size();
    if (startBin < 0) startBin = 0;

    return std::distance(signal.begin(),
                         std::max_element(signal.begin() + startBin, signal.begin() + endBin));
}

template <typename T>
Int_t GetMinBin(const std::vector<T>& signal, int startBin = 0, int endBin = 0) {
    if (endBin <= 0 || endBin > (int)signal.size()) endBin = signal.size();
    if (startBin < 0) startBin = 0;

    return std::distance(signal.begin(),
                         std::min_element(signal.begin() + startBin, signal.begin() + endBin));
}

template <typename T>
Double_t GetIntegral(const std::vector<T>& signal, Int_t startBin, Int_t endBin);

template <typename T>
Double_t GetMaxPeakWidth(const std::vector<T>& signal);

Double_t GetSlopeIntegral(const std::vector<std::pair<Float_t, Float_t> >& signal);
Double_t GetRiseSlope(const std::vector<std::pair<Float_t, Float_t> >& signal);
Double_t GetRiseTime(const std::vector<std::pair<Float_t, Float_t> >& signal);

std::vector<std::pair<double, double> > GetIntWindow(TGraph* signal, double intWindow);
std::array<std::pair<Double_t, Double_t>, 3> GetTripleMax(TGraph* signal);
TVector2 GetTripleMaxAverage(TGraph* signal);
Double_t GetTripleMaxIntegral(TGraph* signal);
TVector2 GetMaxGauss(TGraph* signal);
TVector2 GetMaxLandau(TGraph* signal);
TVector2 GetMaxAget(TGraph* signal);

}  // namespace TRestSignalAnalysis

#endif
